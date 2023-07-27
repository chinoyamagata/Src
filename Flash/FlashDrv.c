/*
 *******************************************************************************
 *	File name	:	FlashDrv.c
 *
 *	[内容]
 *		外付けフラッシュメモリドライバ処理
 *	[適応]
 *		winbond製 W25Q80DV
 *		※1Mbyte
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2017.12.22		Softex N.I		新規作成
 *******************************************************************************
 */
#include "cpu_sfr.h"															/* SFR定義ヘッダー */

#include "typedef.h"															/* データ型定義 */
#include "UserMacro.h"															/* ユーザー作成マクロ定義 */
#include "immediate.h"															/* immediate定数定義 */
#include "switch.h"																/* コンパイルスイッチ定義 */
#include "enum.h"																/* 列挙型の定義 */
#include "struct.h"																/* 構造体定義 */
#include "func.h"																/* 関数の外部参照(extern)定義 */
#include "glLabel.h"															/* グローバル変数・定数(const)ラベル定義 */

#include <string.h>

/*
 *==============================================================================
 *	define定義
 *==============================================================================
 */
/* コマンド */
#define	imFlashDrv_WREN		0x06U												/* Write Enable */
#define	imFlashDrv_RDSR1	0x05U												/* Read Status Register-1 */
#define	imFlashDrv_RDDT		0x03U												/* Read Data */
#define	imFlashDrv_PGPR		0x02U												/* Page Program */
#define	imFlashDrv_SCER		0x20U												/* Sector Erase */
#define	imFlashDrv_BLER		0xD8U												/* Block Erase */
#define	imFlashDrv_CSER		0xC7U												/* Chip Erase */
#define	imFlashDrv_ENRE		0x66U												/* Enable Reset */
#define	imFlashDrv_REST		0x99U												/* Reset */
#define	imFlashDrv_PDWN		0xB9U												/* Power-down */
#define	imFlashDrv_RPDWN	0xABU												/* Release Power-down */


#define	imFlashDrv_FlashSize	0x100000U										/* Flashの総サイズ */
#define	imFlashDrv_MaxAddr		0xFFFFFU										/* アドレスの最大値 */
#define	imFlashDrv_DummySnd		0x00U											/* ダミー送信データ */
#define	imFlashDrv_MaxWrNum		256U											/* 書き込みデータバイト数の最大値 */

#define imReadStatusReg1CmdNum	2U												/* ステータスレジスタ1読み出しコマンド数 */
#define imWriteMemoryCmdNum		260U											/* データ書き込みコマンド数 */
#define imReadDataCmdNum		260U											/* データ読み出しコマンド数 */
#define imSectorEraseCmdNum		4U												/* セクタ消去コマンド数 */
#define imBlockEraseCmdNum		4U												/* ブロック消去コマンド数 */

/* 高速オンチップオシレータへ変更 */
#define imRetryTimerMax			24000U											/* リトライ1ms待ち(1ステート：1/24MHz≒41.67ns、1ms÷41.67ns≒24000回) */
#define imBlErTimerMax			24000000U										/* ブロック消去1s待ち(1ステート：1/24MHz≒41.67ns、1s÷41.67ns≒24000000回) */
#define imCSErTimerMax			144000000U										/* チップ消去6s待ち(1ステート：1/24MHz≒41.67ns、6s÷41.67ns≒144000000回) */
#define imResetWaitTm			720U											/* リセット30μs待ち(1ステート：1/24MHz≒41.67ns、30μs÷41.67ns≒720回) */

#define	imFlashPageSize		0x100U

/*
 *==============================================================================
 *	構造体定義
 *==============================================================================
 */



/*
 *==============================================================================
 *	変数定義
 *==============================================================================
 */



/*
 *==============================================================================
 *	プロトタイプ宣言
 *==============================================================================
 */
//static ET_Error_t SubFlashDrv_WriteEnable( ET_FlashKind_t arKind );				/* 書き込み許可(Write Enable)コマンド送信 */
static ET_Error_t SubFlashDrv_CmdSnd( ET_FlashKind_t arKind, uint8_t arCmd );
static ET_Error_t SubFlashDrv_EraseCmdSnd( uint32_t arAddress, ET_FlashKind_t arKind, uint8_t arCmd );
static ET_Error_t SubFlashDrv_SndFlgChk( void );


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	フラッシュドライバ処理 初期化処理
 *
 *	[内容]
 *		フラッシュドライバ処理 初期化処理
 *******************************************************************************
 */
void ApiFlashDrv_Initial( void )
{
	R_CSI10_Start();															/* CSI10開始 */
	ApiFlashDrv_CSCtl( ecFlashKind_Prim, imLow );
	ApiFlashDrv_CSCtl( ecFlashKind_Second, imLow );
}

/*
 *******************************************************************************
 *	パワーダウンコマンド送信
 *
 *	[内容]
 *		パワーダウンコマンドを送信します。
 *	[引数]
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_PowerDown( ET_FlashKind_t arKind )
{
	ET_Error_t	wkRet;					/* 戻り値(エラー情報) */
	
	wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_PDWN );
	
	return wkRet;
}


/*
 *******************************************************************************
 *	パワーダウン開放コマンド送信
 *
 *	[内容]
 *		パワーダウン開放コマンドを送信します。
 *	[引数]
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_ReleasePowerDown( ET_FlashKind_t arKind )
{
	ET_Error_t	wkRet;					/* 戻り値(エラー情報) */
	
	wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_RPDWN );
	
	return wkRet;
}

#if 0
/*
 *******************************************************************************
 *	書き込み許可(Write Enable)コマンド送信
 *
 *	[内容]
 *		書き込み許可コマンドを送信します。
 *	[引数]
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *******************************************************************************
 */
ET_Error_t SubFlashDrv_WriteEnable( ET_FlashKind_t arKind )
{
	ET_Error_t	wkRet;					/* 戻り値(エラー情報) */
	
	wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_WREN );
	
	return wkRet;
}
#endif


/*
 *******************************************************************************
 *	ステータスレジスタ1読み出し
 *
 *	[内容]
 *		ステータスレジスタ1の内容を読み出します。
 *	[引数]
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_ReadStatusReg1( uint8_t *parRegData, ET_FlashKind_t arKind )
{
	ET_Error_t			wkRet;													/* 戻り値(エラー情報) */
	uint8_t 			wkSndData[imReadStatusReg1CmdNum];						/* 送信データ */
	uint8_t				wkRcvData[imReadStatusReg1CmdNum];						/* 受信データ */
	uint16_t 			wkSndNum;												/* 送信コマンド個数 */
	
	/* 変数初期化 */
	wkRet = ecERR_OK;															/* エラー情報初期化 */
	wkSndNum = 0U;																/* 送信コマンド数初期化 */
	
	/* チップセレクト＝Low */
	ApiFlashDrv_CSCtl( arKind, imLow );
	
	M_NOP;																		/* 1ステート(=1/(18.4*10^6)=54ns) */
	
	/* コマンド作成 */
	wkSndData[wkSndNum] = imFlashDrv_RDSR1;										/* Read Status Register-1 */
	wkSndNum++;
	wkSndData[wkSndNum] = imFlashDrv_DummySnd;									/* ダミー送信データ */
	wkSndNum++;
	
	/* コマンド送信 */
	gvCsi10SndEndFlg = imOFF;													/* 送信完了フラグOFF */
	R_CSI10_Send_Receive(&wkSndData[0], wkSndNum, &wkRcvData[0]);				/* 送信 */
	wkRet = SubFlashDrv_SndFlgChk();											/* 送信完了フラグがONになるまで */
	
	if( wkRet == ecERR_OK )														/* エラーチェック */
	{
		*parRegData = wkRcvData[1];												/* 受信データ格納 */
	}
	
	/* チップセレクト＝High */
	ApiFlashDrv_CSCtl( arKind, imHigh );
	
	return wkRet;
}


/*
 *******************************************************************************
 *	データ読み出し
 *
 *	[内容]
 *		指定したアドレスのデータを読み出します。
 *	[引数]
 *		uint32_t arAddress：アドレス
 *		uint8_t arDataTable[]：データの格納先テーブル
 *		uint16_t arNum：データ数
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *	[注意事項]
 *		一度に読み出せるデータは256Byteです。
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_ReadData( uint32_t arAddress, uint8_t arDataTable[], uint16_t arNum, ET_FlashKind_t arKind )
{
	ET_Error_t			wkRet;													/* 戻り値(エラー情報) */
	uint16_t			wkLoop;													/* ループカウンタ */
	uint32_t			wkAddr;													/* アドレス */
	ET_Error_t			wkReg1Ret;												/* エラー情報 */
	uint8_t				wkReg1Data;												/* レジスタデータ */
	uint8_t 			wkSndData[imReadDataCmdNum];							/* 送信データ */
	uint8_t				wkRcvData[imReadDataCmdNum];							/* 受信データ */
	uint16_t 			wkSndNum;												/* 送信コマンド個数 */
	
	/* 変数初期化 */
	wkRet = ecERR_OK;															/* エラー情報初期化 */
	wkSndNum = 0U;																/* 送信コマンド数初期化 */
	
	wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);				/* ステータスレジスタ1読み出し */
	
	if ((M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY))							/* BUSY */
	||	(arDataTable == M_NULL)												/* NULL */
	||	(arAddress > imFlashDrv_MaxAddr)										/* アドレス指定NG */
	||	((arAddress + arNum) > (imFlashDrv_MaxAddr + 1U))						/* データ数指定NG */
	||	(wkReg1Ret == ecERR_NG)													/* ステータスレジスタ1読み出しNG */
	)
	{
		wkRet = ecERR_NG;														/* エラー情報＝NG */
	}
	else
	{
		/* チップセレクト＝Low */
		ApiFlashDrv_CSCtl( arKind, imLow );
		
		M_NOP;																	/* 1ステート(=1/(18.4*10^6)=54ns) */
		
		/* コマンド作成 */
		wkSndData[wkSndNum] = imFlashDrv_RDDT;									/* Read Data */
		wkSndNum++;
		wkAddr = arAddress;
		wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x00FF0000U) >> 16U);
		wkSndNum++;
		wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x0000FF00U) >> 8U);
		wkSndNum++;
		wkSndData[wkSndNum] = (uint8_t)(wkAddr & 0x000000FFU);
		wkSndNum++;
		for (wkLoop = 0U; wkLoop < arNum; wkLoop++)
		{
			wkSndData[wkSndNum] = imFlashDrv_DummySnd;							/* ダミー送信データ */
			wkSndNum++;
		}
		
		/* コマンド送信 */
		gvCsi10SndEndFlg = imOFF;												/* 送信完了フラグOFF */
		R_CSI10_Send_Receive(&wkSndData[0], wkSndNum, &wkRcvData[0]);			/* 送信 */
		wkRet = SubFlashDrv_SndFlgChk();										/* 送信完了フラグがONになるまで */
		
		if (wkRet == ecERR_OK)													/* エラーチェック */
		{
			/* 読み出しデータ格納 */
			for (wkLoop = 0U; wkLoop < arNum; wkLoop++)
			{
				arDataTable[wkLoop] = wkRcvData[wkLoop + 4U];					/* データ格納 */
			}
		}
		
		/* チップセレクト＝High */
		ApiFlashDrv_CSCtl( arKind, imHigh );
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	データ書き込み
 *
 *	[内容]
 *		指定したアドレスにデータを書き込みます。
 *	[引数]
 *		uint32_t arAddress：アドレス
 *		const uint8_t arDataTable[]：書き込みデータ格納テーブル
 *		uint8_t arNum：書き込みデータ数
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *	[注意事項]
 *		一度に書き込めるデータは256Byteです。
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_WriteMemory( uint32_t arAddress, const uint8_t arDataTable[], uint16_t arNum, ET_FlashKind_t arKind )
{
	ET_Error_t			wkRet;													/* 戻り値(エラー情報) */
	uint16_t			wkLoop;													/* ループカウンタ */
	uint32_t			wkAddr;													/* アドレス */
	ET_Error_t			wkReg1Ret;												/* エラー情報 */
	uint8_t				wkReg1Data;												/* レジスタデータ */
	uint8_t 			wkSndData[imWriteMemoryCmdNum];							/* 送信データ */
	uint8_t				wkRcvData[imWriteMemoryCmdNum];							/* 受信データ */
	uint16_t 			wkSndNum;												/* 送信コマンド個数 */
	uint16_t			wkRetryTimer;											/* リトライタイマー */
	uint32_t			wkPage1;
	uint32_t			wkPage2;
	uint8_t				wkPageCnt;
	uint32_t			wkVal;
	uint8_t				wkLoop2;
	uint16_t			wkNum;
	uint16_t			wkNum1;
	
	/* 変数初期化 */
	wkRet = ecERR_OK;															/* エラー情報初期化 */
	wkSndNum = 0U;																/* 送信コマンド数初期化 */
	wkRetryTimer = 0U;															/* リトライタイマー初期化 */
	
	wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);				/* ステータスレジスタ1読み出し */
	
	if ((M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY))							/* Busy */
	||	(arDataTable == M_NULL)													/* NULL */
	||	(arAddress > imFlashDrv_MaxAddr)										/* アドレス指定NG */
	||	(arNum > imFlashDrv_MaxWrNum)											/* 書き込みデータ数指定NG */
	||	((arAddress + arNum) > (imFlashDrv_MaxAddr + 1U))						/* 書き込みデータ数指定NG */
	||	(wkReg1Ret == ecERR_NG)													/* ステータスレジスタ1読み出しNG */
	)
	{
		wkRet = ecERR_NG;														/* エラー情報＝NG */
	}
	else
	{
		wkPage1 = arAddress / imFlashPageSize;
		wkPage2 = ( arAddress + arNum ) / imFlashPageSize;
		if( wkPage1 != wkPage2 )
		{
			wkPageCnt = 2U;
			wkPage2 *= imFlashPageSize;
			wkVal = wkPage2 - arAddress;
			wkNum = (uint16_t)wkVal;
			wkNum1 = wkNum;
		}
		else
		{
			wkPageCnt = 1U;
			wkNum = arNum;
		}
		wkAddr = arAddress;
		
		for( wkLoop2 = 0U; wkLoop2 < wkPageCnt; wkLoop2++ )
		{
#if 0
			wkRet = SubFlashDrv_WriteEnable(arKind);								/* 書き込み許可(Write Enable)コマンド送信 */
#else
			wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_WREN );					/* 書き込み許可(Write Enable)コマンド送信 */
#endif
			
			/* チップセレクト＝Low */
			ApiFlashDrv_CSCtl( arKind, imLow );
			
			M_NOP;																	/* 1ステート(=1/(18.4*10^6)=54ns) */
			
			if( wkLoop2 == 1U )
			{
				wkAddr = wkPage2;
				wkNum = arNum - wkNum1;
				wkSndNum = 0U;
			}
			/* コマンド作成 */
			wkSndData[wkSndNum] = imFlashDrv_PGPR;									/* Page Program */
			wkSndNum++;
			wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x00FF0000U) >> 16U);
			wkSndNum++;
			wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x0000FF00U) >> 8U);
			wkSndNum++;
			wkSndData[wkSndNum] = (uint8_t)(wkAddr & 0x000000FFU);
			wkSndNum++;
			for( wkLoop = 0U; wkLoop < wkNum; wkLoop++ )
			{
				if( wkLoop2 == 1U )
				{
					wkSndData[wkSndNum] = arDataTable[ wkLoop + wkNum1 ];			/* 送信データ */
				}
				else
				{
					wkSndData[wkSndNum] = arDataTable[ wkLoop ];					/* 送信データ */
				}
				wkSndNum++;
			}
			
			/* コマンド送信 */
			gvCsi10SndEndFlg = imOFF;												/* 送信完了フラグOFF */
			R_CSI10_Send_Receive(&wkSndData[0], wkSndNum, &wkRcvData[0]);			/* 送信 */
			wkRet = SubFlashDrv_SndFlgChk();										/* 送信完了フラグがONになるまで */
			
			/* チップセレクト＝High */
			ApiFlashDrv_CSCtl( arKind, imHigh );
			
			/* busyが0になるまで待つ */
			while( 1 )
			{
				wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);		/* ステータスレジスタ1読み出し */
				if( !M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY) )
				{
					M_NOP;
					break;
				}
				if( wkRetryTimer >= imRetryTimerMax )								/* 1m超過した場合 */
				{
					wkRet = ecERR_NG;												/* エラー情報＝NG */
					break;
				}
				wkRetryTimer++;														/* リトライタイマー更新 */
			}
		}
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	セクタ消去
 *
 *	[内容]
 *		指定したセクタの消去を行います。
 *	[引数]
 *		uint32_t arAddress：アドレス
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
*******************************************************************************
 */
ET_Error_t ApiFlashDrv_SectorErase( uint32_t arAddress, ET_FlashKind_t arKind )
{
	ET_Error_t	wkRet;						/* 戻り値(エラー情報) */
	
	wkRet = SubFlashDrv_EraseCmdSnd( arAddress, arKind, imFlashDrv_SCER );
	
	return wkRet;
}

#if 0
/*
 *******************************************************************************
 *	ブロック消去
 *
 *	[内容]
 *		指定したブロック(64KB)の消去を行います。
 *	[引数]
 *		uint32_t arAddress：アドレス
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_BlockErase( uint32_t arAddress, ET_FlashKind_t arKind )
{
	ET_Error_t	wkRet;						/* 戻り値(エラー情報) */
	
	wkRet = SubFlashDrv_EraseCmdSnd( arAddress, arKind, imFlashDrv_BLER );
	
	return wkRet;
}
#endif

/*
 *******************************************************************************
 *	チップイレース送信
 *
 *	[内容]
 *		チップイレースコマンドを送信します。
 *	[引数]
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_ChipErase( ET_FlashKind_t arKind )
{
	ET_Error_t			wkRet;													/* 戻り値(エラー情報) */
	ET_Error_t			wkReg1Ret;												/* エラー情報 */
	uint8_t				wkReg1Data;												/* レジスタデータ */
	uint8_t 			wkSndData;												/* 送信データ */
	uint8_t				wkRcvData;												/* 受信データ */
	uint16_t 			wkSndNum;												/* 送信コマンド個数 */
	uint32_t			wkRetryTimer;											/* リトライタイマー */
	
	/* 変数初期化 */
	wkRet = ecERR_OK;															/* エラー情報初期化 */
	wkSndNum = 0U;																/* 送信コマンド数初期化 */
	wkRetryTimer = 0U;															/* リトライタイマー初期化 */
	
	wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);				/* ステータスレジスタ1読み出し */
	
	if( (M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY))							/* Busy = 1 */
	||	(wkReg1Ret == ecERR_NG)													/* ステータスレジスタ1読み出しNG */
	)
	{
		wkRet = ecERR_NG;														/* エラー情報＝NG */
	}
	else
	{
#if 0
		wkRet = SubFlashDrv_WriteEnable(arKind);								/* 書き込み許可(Write Enable)コマンド送信 */
#else
		wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_WREN );					/* 書き込み許可(Write Enable)コマンド送信 */
#endif
		
		/* チップセレクト＝Low */
		ApiFlashDrv_CSCtl( arKind, imLow );
		
		M_NOP;																	/* 1ステート(=1/(18.4*10^6)=54ns) */
		
		/* コマンド作成 */
		wkSndData = imFlashDrv_CSER;
		wkSndNum++;
		
		/* コマンド送信 */
		gvCsi10SndEndFlg = imOFF;												/* 送信完了フラグOFF */
		R_CSI10_Send_Receive(&wkSndData, wkSndNum, &wkRcvData);					/* 送信 */
		wkRet = SubFlashDrv_SndFlgChk();										/* 送信完了フラグがONになるまで */
		
		/* チップセレクト＝High */
		ApiFlashDrv_CSCtl( arKind, imHigh );
		
		/* busyが0になるまで待つ */
		while( 1 )
		{
			wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);		/* ステータスレジスタ1読み出し */
			if( !M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY) )
			{
				M_NOP;
				break;
			}
			if( wkRetryTimer >= imCSErTimerMax )								/* 6s超過した場合 */
			{
				wkRet = ecERR_NG;												/* エラー情報＝NG */
				break;
			}
			wkRetryTimer++;														/* リトライタイマー更新 */
			
			R_WDT_Restart();
		}
	}
	
	return wkRet;
}


/*
 ***************************************************************************************
 *	許可リセット(66h)＆リセット(99h)コマンド送信
 *
 *	[内容]
 *		進行中の内部動作は終了し、デバイスはデフォルトのパワーオン状態に戻り、
 *		揮発性ステータスレジスタビット、ライトイネーブルラッチ（WEL）ステータスなどの
 *		現在のすべての揮発性設定を失う。
 *	[引数]
 *		uint32_t arAddress：アドレス
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *	[注意事項]
 *		「許可リセット(66h)」コマンドの後に「リセット(99h)」以外のコマンドを使用すると、
 *		「許可リセット」状態が無効となる為、注意。
 ***************************************************************************************
 */
ET_Error_t ApiFlashDrv_Reset( ET_FlashKind_t arKind )
{
	uint16_t			wkLoop;													/* ループカウンタ */
	uint8_t 			wkSndData;												/* 送信データ */
	uint8_t				wkRcvData;												/* 受信データ */
	uint16_t 			wkSndNum;												/* 送信コマンド個数 */
	ET_Error_t	wkRet;															/* 戻り値(エラー情報) */
	
	/* 変数初期化 */
	wkSndNum = 0U;
 	
	/*** 許可リセットコマンド送信開始 ***/
	/* チップセレクト＝Low */
	ApiFlashDrv_CSCtl( arKind, imLow );
	
	M_NOP;																		/* 1ステート(=1/(18.4*10^6)=54ns) */
	
	/* コマンド作成 */
	wkSndData = imFlashDrv_ENRE;												/* Enable Reset */
	wkSndNum++;
	
	/* コマンド送信 */
	gvCsi10SndEndFlg = imOFF;													/* 送信完了フラグOFF */
	R_CSI10_Send_Receive(&wkSndData, wkSndNum, &wkRcvData);						/* 送信 */
	wkRet = SubFlashDrv_SndFlgChk();											/* 送信完了フラグがONになるまで */
	
	/* チップセレクト＝High */
	ApiFlashDrv_CSCtl( arKind, imHigh );
	
	/*** 許可リセットコマンド送信終了 ***/
	
	
	for (wkLoop = 0U; wkLoop < imResetWaitTm; wkLoop++)
	{
		M_NOP;
	}
	
	/*** リセットコマンド送信開始 ***/
	/* チップセレクト＝Low */
	ApiFlashDrv_CSCtl( arKind, imLow );
	
	M_NOP;																		/* 1ステート(=1/(18.4*10^6)=54ns) */
	
	/* コマンド作成 */
	wkSndData = imFlashDrv_REST;												/*	Reset */
	wkSndNum++;
	
	/* コマンド送信 */
	gvCsi10SndEndFlg = imOFF;													/* 送信完了フラグOFF */
	R_CSI10_Send_Receive(&wkSndData, wkSndNum, &wkRcvData);						/* 送信 */
	wkRet = SubFlashDrv_SndFlgChk();											/* 送信完了フラグがONになるまで */
	
	/* チップセレクト＝High */
	ApiFlashDrv_CSCtl( arKind, imHigh );
	
	/*** 許可リセットコマンド送信終了 ***/
	
	return wkRet;
}




/*
 *******************************************************************************
 *	コマンド送信
 *
 *	[内容]
 *		コマンドを送信します。
 *	[引数]
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *		uint8_t arCmd: 送信コマンド
 *******************************************************************************
 */
static ET_Error_t SubFlashDrv_CmdSnd( ET_FlashKind_t arKind, uint8_t arCmd )
{
	uint8_t 	wkSndData;														/* 送信データ */
	uint8_t 	wkRcvData;														/* 受信データ */
	uint16_t 	wkSndNum;														/* 送信コマンド数 */
	ET_Error_t	wkRet;															/* 戻り値(エラー情報) */
	
	/* 変数初期化 */
	wkRet = ecERR_OK;															/* エラー情報初期化 */
	wkSndNum = 0U;																/* 送信コマンド数初期化 */
	
	/* チップセレクト＝Low */
	ApiFlashDrv_CSCtl( arKind, imLow );
	
	M_NOP;																		/* 1ステート(=1/(18.4*10^6)=54ns) */
	
	/* コマンド作成 */
	wkSndData = arCmd;
	wkSndNum++;
	
	/* コマンド送信 */
	gvCsi10SndEndFlg = imOFF;													/* 送信完了フラグOFF */
	R_CSI10_Send_Receive(&wkSndData, wkSndNum, &wkRcvData);						/* 送信 */
	wkRet = SubFlashDrv_SndFlgChk();											/* 送信完了フラグがONになるまで */
	
	/* チップセレクト＝High */
	ApiFlashDrv_CSCtl( arKind, imHigh );
	
	return wkRet;
}

/*
 *******************************************************************************
 *	イレースコマンド送信
 *
 *	[内容]
 *		イレースコマンドを送信します。
 *	[引数]
 *		uint32_t	arAddress: イレースのアドレス
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *		uint8_t arCmd: イレース送信コマンド
 *******************************************************************************
 */
static ET_Error_t SubFlashDrv_EraseCmdSnd( uint32_t arAddress, ET_FlashKind_t arKind, uint8_t arCmd )
{
	ET_Error_t			wkRet;													/* 戻り値(エラー情報) */
	uint32_t			wkAddr;													/* アドレス */
	ET_Error_t			wkReg1Ret;												/* エラー情報 */
	uint8_t				wkReg1Data;												/* レジスタデータ */
	uint8_t 			wkSndData[imSectorEraseCmdNum];							/* 送信データ */
	uint8_t				wkRcvData[imSectorEraseCmdNum];							/* 受信データ */
	uint16_t 			wkSndNum;												/* 送信コマンド個数 */
	
	/* 変数初期化 */
	wkRet = ecERR_OK;															/* エラー情報初期化 */
	wkSndNum = 0U;																/* 送信コマンド数初期化 */
	
	wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);				/* ステータスレジスタ1読み出し */
	
	if( (M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY))							/* Busy = 1 */
	||	(arAddress > imFlashDrv_MaxAddr)										/* アドレス指定NG */
	||	(wkReg1Ret == ecERR_NG)													/* ステータスレジスタ1読み出しNG */
	)
	{
		wkRet = ecERR_NG;														/* エラー情報＝NG */
	}
	else
	{
#if 0
		wkRet = SubFlashDrv_WriteEnable(arKind);								/* 書き込み許可(Write Enable)コマンド送信 */
#else
		wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_WREN );					/* 書き込み許可(Write Enable)コマンド送信 */
#endif
		
		/* チップセレクト＝Low */
		ApiFlashDrv_CSCtl( arKind, imLow );
		
		M_NOP;																	/* 1ステート(=1/(18.4*10^6)=54ns) */
		
		/* コマンド作成 */
		wkSndData[wkSndNum] = arCmd;
		wkSndNum++;
		wkAddr = arAddress;
		wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x00FF0000U) >> 16U);
		wkSndNum++;
		wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x0000FF00U) >> 8U);
		wkSndNum++;
		wkSndData[wkSndNum] = (uint8_t)(wkAddr & 0x000000FFU);
		wkSndNum++;
		
		/* コマンド送信 */
		gvCsi10SndEndFlg = imOFF;												/* 送信完了フラグOFF */
		R_CSI10_Send_Receive(&wkSndData[0], wkSndNum, &wkRcvData[0]);			/* 送信 */
		wkRet = SubFlashDrv_SndFlgChk();										/* 送信完了フラグがONになるまで */
		
		/* チップセレクト＝High */
		ApiFlashDrv_CSCtl( arKind, imHigh );
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	チップセレクト制御
 *
 *	[内容]
 *		チップセレクトを制御する
 *	[引数]
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *******************************************************************************
 */
void ApiFlashDrv_CSCtl( ET_FlashKind_t arKind, uint8_t arCtl )
{
	uint16_t	wkLoop;
	
	/* チップセレクト＝High */
	if( arCtl == imHigh )
	{
		/* 対象のフラッシュがプライマリ */
		if( arKind == ecFlashKind_Prim )
		{
			P_CS_PRIM = (uint8_t)imHigh;				/* チップセレクト(プライマリ)＝High */
		}
		else
		{
			P_CS_SECOND = (uint8_t)imHigh;				/* チップセレクト(セカンダリ)＝High */
			for( wkLoop = 0U ; wkLoop < 85U; wkLoop++ )
			{
				M_NOP;												/* 1ステート(=1/(18.4*10^6)=54ns, 54ns * 926 = 50us) */
			}
		}
	}
	/* チップセレクト＝Low */
	else
	{
		/* 対象のフラッシュがプライマリ */
		if( arKind == ecFlashKind_Prim )
		{
			P_CS_PRIM = (uint8_t)imLow;					/* チップセレクト(プライマリ)＝Low */
		}
		else
		{
			P_CS_SECOND = (uint8_t)imLow;				/* チップセレクト(セカンダリ)＝Low */
		}
	}
}

/*
 *******************************************************************************
 *	外付けFlash通信送信フラグ監視
 *
 *	[内容]
 *		外付けFlashとの通信時の送信フラグ監視
 *	[引数]
 *		なし
 *	[戻り値]
 *		ET_Error_t	wkRet: エラー情報
 *******************************************************************************
 */
static ET_Error_t SubFlashDrv_SndFlgChk( void )
{
	uint16_t	wkErrJdgCnt = 0U;
	ET_Error_t	wkRet = ecERR_OK;
	
	while( gvCsi10SndEndFlg == imOFF )										/* 送信完了フラグがONになるまで */
	{
		wkErrJdgCnt++;
		if( wkErrJdgCnt > imRetryTimerMax )				/* 1ms超過で外付けFlashへのアクセス失敗 */
		{
			wkRet = ecERR_NG;												/* RTC異常 */
			break;
		}
	}
	return wkRet;
}


/*
 *******************************************************************************
 *	ファーム領域イレース
 *
 *	[内容]
 *		外付けFlashのファーム格納領域を消去する
 *	[引数]
 *		ET_FlashKind_t arKind：フラッシュメモリ種類
 *								ecFlashKind_Prim：プライマリ
 *								ecFlashKind_Second：セカンダリ
 *	[戻り値]
 *		ET_Error_t	wkRet: エラー情報
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_FirmMemErase( ET_FlashKind_t arKind )
{
	uint8_t		wkLoop;
	uint8_t		wkReg1Data;
	uint16_t	wkCnt;
	uint32_t	wkAdr;
	ET_Error_t	wkRet;
	
	/* Flash電源ON */
//	ApiFlash_FlashPowerCtl( imON, imON );
	
	/* Blockイレース */
	for( wkLoop = 0U, wkAdr = 0xC0000U ; wkLoop < 4U ; wkLoop++, wkAdr += 0x10000U )
	{
//		wkRet = ApiFlashDrv_BlockErase( wkAdr, arKind );
		wkRet = SubFlashDrv_EraseCmdSnd( wkAdr, arKind, imFlashDrv_BLER );
		
		wkCnt = 0;
		do
		{
			/* ステータスレジスタ1読み出し */
			wkRet = ApiFlashDrv_ReadStatusReg1( &wkReg1Data, arKind );
			
			wkCnt++;
			if( wkCnt == 0xFFFF )
			{
				wkRet = ecERR_NG;
				break;
			}
		}
		while( M_TSTBIT( wkReg1Data, imFlashDrv_Sts1Bit_BUSY ) );
	}
	
	/* Flash電源OFF */
//	ApiFlash_FlashPowerCtl( imOFF, imON );
	
	return wkRet;
}

#if (swKouteiMode == imEnable)
/*
 *******************************************************************************
 *	外付けFlash動作検査
 *
 *	[内容]
 *		外付けFlashの全領域の動作を検査する
 *			値をライトし、リードして比較する
 *	[引数]
 *		 ET_FlashKind_t arKind：フラッシュメモリ種類
 *						ecFlashKind_Prim：プライマリ
 *						ecFlashKind_Second：セカンダリ
 *	[戻り値]
 *		ET_Error_t	wkRet: エラー情報
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_TestFlash( ET_FlashKind_t arKind )
{
	uint32_t		wkLoop;
	ET_Error_t		wkRet;
	int16_t			wkCmpRet;
	uint8_t			wkRdData[ imFlashDrv_MaxWrNum ];
	uint8_t			wkWrData[ imFlashDrv_MaxWrNum ];
	
	for( wkLoop = 0 ; wkLoop < imFlashDrv_MaxWrNum ; wkLoop++ ){
		wkWrData[ wkLoop ] = wkLoop;
	}
	
	wkRet = ApiFlashDrv_ChipErase( arKind );
	
	if( wkRet == ecERR_OK )
	{
		for( wkLoop = 0 ; wkLoop < imFlashDrv_FlashSize/imFlashDrv_MaxWrNum ; wkLoop++ ){
			R_WDT_Restart();
			wkRet = ApiFlashDrv_WriteMemory( (wkLoop * imFlashDrv_MaxWrNum), &wkWrData[ 0U ], imFlashDrv_MaxWrNum, arKind );
			if( wkRet == ecERR_OK )
			{
				wkRet = ApiFlashDrv_ReadData( (wkLoop * imFlashDrv_MaxWrNum), &wkRdData[ 0U ], imFlashDrv_MaxWrNum, arKind );
				if( wkRet == ecERR_OK )
				{
					wkCmpRet = memcmp( &wkWrData[ 0U ], &wkRdData[ 0U ], imFlashDrv_MaxWrNum );
					if( wkCmpRet != 0U )
					{
						wkRet = ecERR_NG;
					}
				}
			}
			if( wkRet == ecERR_NG )
			{
				break;
			}
		}
	}
	
	ApiFlashDrv_ChipErase( arKind );
	
	return wkRet;
}
#endif

#pragma section
