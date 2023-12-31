/*
 *******************************************************************************
 *	File name	:	FirmUpMain.c
 *
 *	[内容]
 *		『本ファイルの処理概要を記述』
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2018.10.25		Softex T.K		新規作成
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

/* フラッシュ・セルフ・プログラミング・ライブラリType01 Ver.2.21B */
#include "fsl.h" 																/* ライブラリ・ヘッダーファイル */
#include "fsl_types.h"															/* ライブラリ・ヘッダーファイル */

/*
 *==============================================================================
 *	define定義
 *==============================================================================
 */
/* コマンド */
#define	imFlashDrv_RDSR1		0x05U											/* Read Status Register-1 */
#define	imFlashDrv_RDDT			0x03U											/* Read Data */

#define	P_CS_PRIM				(P12_bit.no5)									/* チップセレクト(プライマリ) */
#define	P_CS_SECOND				(P6_bit.no1)									/* チップセレクト(セカンダリ) */
#define	P_SDO					(P0_bit.no7)									/* シリアル通信 DataOut(Q) */
#define	P_SDI					(P0_bit.no6)									/* シリアル通信 DataInput(D) */
#define	P_SCLK					(P0_bit.no5)									/* SCLK(シリアルクロック) */

#define	imFirmUp_SctAddr		0xE7000U										/* セクタアドレスの先頭位置 */
#define	imFirmUp_FlashErsBlkTopAddr		0xD0000U								/* ブロック消去アドレスの先頭位置 */


#define	imFirmUp_TopAddr		0xE7000U										/* アドレスの先頭位置 */
#define	imFirmUp_RomMaxAddr		0xFEFFFU										/* ロムデータ保存アドレスの最大値 */
#define	imFrimUp_ChkSumTopAddr	0xFF000U										/* チェックサム書き込み先先頭アドレス */
#define	imFirmUp_MaxAddr		0xFFFFFU										/* アドレスの最大値 */

#define	imFirmUp_MaxWriteNum	256U											/* 書込みデータバイト数の最大値 */
#define	imFirmUp_MaxReadNum		256U											/* 読込みデータバイト数の最大値 */
#define	imFirmUp_ChkSumReadMaxNum	2U											/* チェックサム読込みデータバイト数の最大値 */

#define	imFirmUp_SctrErsMaxCnt	6U												/* セクタ(4KB)消去最大カウント(6cnt×50ms周期＝300ms) */
#define	imFirmUp_BlkErsMaxCnt	20U												/* ブロック(64KB)消去最大カウント(20cnt×50ms周期＝1000ms) */

#define	imFirmUp_BlkWrMaxNum	95U												/* ファームアップのROMブロックサイズ */

#define	FLASH_OK				0U												/* OK */
#define	FLASH_SUCCESS			1U												/* 正常 */
#define	FLASH_FAILURE			2U												/* 異常 */
#define	FLASH_BUSY				3U												/* ビジー */
#define	FLASH_COMPLETE			4U												/* 完了 */

#define FL_HEX_ADDR_SIZE		2												/* HEXデータ アドレスサイズ */
#define FL_HEX_TYP_SIZE 		1												/* HEXデータ タイプサイズ */
#define FL_HEX_SUM_SIZE 		1												/* HEXデータ 合計サイズ */


/*
 *==============================================================================
 *	列挙型定義
 *==============================================================================
 */


/*
 *==============================================================================
 *	構造体定義
 *==============================================================================
 */

/* buffer for HEX format data */
typedef struct {
	uint8_t type[2];															/* ":"0", ":"1" and so on */
	uint8_t len[2]; 															/* "0-255" */
	uint8_t addr_data_sum[520];
} Fl_prg_hex_t;

/* buffer for write data
	(this data is the converted data from HEX format data) */
typedef struct {
	uint8_t len;
	uint32_t addr;
	uint8_t data[260];
} Fl_prg_hex_binary_t;


/* buffer for writing flash */
typedef struct {
	uint32_t addr;
	uint8_t data[260];
} Fl_prg_writing_data_t;

/*
 *==============================================================================
 *	変数定義
 *==============================================================================
 */

#pragma section bss FIRMUP_DATA
/* Flag for HEX format complete */
static	uint8_t				fl_hex_data_flag;									/* HEXデータフラグ */
static	uint32_t			flash_addr;							 				/* フラッシュライト時のアドレス */
static	uint16_t			fl_hex_total_data_cnt;								/* HEXデータ合計データカウント */
static	uint8_t				fl_hex_length;										/* HEXデータ長 */
static	uint32_t			sAdr_upper;											/* アドレス(上位) */
static	uint8_t				vSurplusCnt;										/* 余剰数 */

/* Buffer for the surplus data.  */
static	Fl_prg_hex_binary_t fl_wr_hex_bin_surplus;

/* Writing address */
static	Fl_prg_writing_data_t fl_writing_data;

/* Buffer for HEX format data */
static	Fl_prg_hex_t fl_wr_hex;

/* Buffer for HEX format data which is converted to Binary */
static	Fl_prg_hex_binary_t fl_wr_hex_bin;

#pragma section

/*
 *==============================================================================
 *	定数定義
 *==============================================================================
 */



/*
 *==============================================================================
 *	プロトタイプ宣言
 *==============================================================================
 */
static void SubStartFirmUpdate( void );
static __far void SubFirmUpdate( void );
static __far fsl_u08 SubFSLStart( void );
static __far ET_Error_t SubFirmUpMain_SubFlash_ReadData( uint32_t arAddress, uint8_t *parRdData, uint16_t arNum );
static __far void SubFSLCmdSnd( uint8_t wkSndData );
static __far uint8_t SubFSLCmdRcv( void );

static __far uint8_t SubR_Fl_PrgTrgtArea( void );
static __far uint8_t SubR_Fl_Prg_StoreHEX( uint8_t hex_data );
static __far void SubR_Fl_Prg_ClearHEXVariables( void );
static __far uint8_t SubR_Fl_AsciiToHexByte( uint8_t in_upper, uint8_t in_lower );
static __far uint8_t SubR_Fl_Prg_ProcessForHEX_data( void );
static __far uint8_t SubR_Fl_Prg_HEX_AsciiToBinary(
						Fl_prg_hex_t *tmp_hex,
						Fl_prg_hex_binary_t *tmp_hex_binary );
static __far uint8_t SubR_Fl_Prg_WriteData( void );
static __far uint8_t SubR_Fl_Prg_MakeWriteData( void );
static __far uint8_t SubR_Fl_Prg_SurplusWriteData( void );

#pragma section text MY_APP2
#if 0
/*
 *******************************************************************************
 *	ファームアップデート初期化処理
 *
 *	[内容]
 *	ファームアップデート用変数の初期化を行う。
 *******************************************************************************
 */
void ApiFirmUpdateInit( void )
{
	M_NOP;
	}
#endif

/*
 *******************************************************************************
 *	ファームアップデートメイン処理
 *
 *	[内容]
 *	シーケンス状態に応じて各処理を実行する。
 *******************************************************************************
 */
void ApiFirmUpdateMain( void )
{
#if 0
			ApiLcd_FirmUpDisp();					/* ファームアップ表示処理 */
#else
	ApiLcd_SegSelectOnOff( ecDispSegSelect_SegAllOff );
	ApiLcd_UppLow7SegDsp("FIR", imHigh);
	ApiLcd_UppLow7SegDsp("Up", imLow);
	ApiLcd_Main();
#endif
	ApiInFlash_ParmWrite( ecInFlashWrSts_ParmNow, 0U );		/* 現在の変数を内蔵Flashに書く */
	SubStartFirmUpdate();
}

/*
 *******************************************************************************
 *	ファーム更新履歴
 *
 *	[内容]
 *	起動時にVer確認し、更新されていたら履歴を残す
 *******************************************************************************
 */
void ApiFirmUpdate_History( void )
{
	uint32_t wkU32;

	/* ファーム更新履歴 */
	if( (gvInFlash.mProcess.mUpVer[ 0U ] != cRomVer[ 0U ]) ||
		(gvInFlash.mProcess.mUpVer[ 1U ] != cRomVer[ 1U ] * 10U + cRomVer[ 2U ]) ||
		(gvInFlash.mProcess.mUpVer[ 2U ] != cRomVer[ 3U ]) )
	{
		gvInFlash.mProcess.mUpVer[ 0U ] = cRomVer[ 0U ];
		gvInFlash.mProcess.mUpVer[ 1U ] = cRomVer[ 1U ] * 10U + cRomVer[ 2U ];
		gvInFlash.mProcess.mUpVer[ 2U ] = cRomVer[ 3U ];
		
		wkU32 = ((uint32_t)cRomVer[ 0U ] << 16U) + ((uint32_t)cRomVer[ 1U ] << 8U) + cRomVer[ 2U ];
		
		/* 操作履歴(ファームウェア更新: Ver.、自動操作) */
		ApiFlash_WriteActLog( ecActLogItm_FrmUpdate, wkU32, gvInFlash.mProcess.mUpUserId );
		gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
	}
}

/*
 *******************************************************************************
 *	ROM書き換え開始処理
 *
 *	[内容]
 *	ROM書き換えを開始する。
 *******************************************************************************
 */
static void SubStartFirmUpdate( void )
{
	uint8_t __far	*pSrc;
	uint8_t __far	*pDst;
	
	/* 割込み禁止 */
	DI();
	
	/* SPIポートを手動に切り替え */
	R_CSI10_Stop();
	SO0 |= _0400_SAU_CH2_CLOCK_OUTPUT_1;
	SO0 |= _0004_SAU_CH2_DATA_OUTPUT_1;
	PFSEG6 = _00_PFSEG50_PORT | _00_PFSEG49_PORT | _00_PFSEG48_PORT | _01_PFSEG47_SEG;
	P0 = _00_Pn5_OUTPUT_0 | _00_Pn7_OUTPUT_0;
	PM0 = _01_PMn0_NOT_USE | _02_PMn1_NOT_USE | _04_PMn2_NOT_USE | _08_PMn3_NOT_USE | _10_PMn4_NOT_USE | 
		  _00_PMn5_MODE_OUTPUT | _40_PMn6_MODE_INPUT | _00_PMn7_MODE_OUTPUT;
	
	/* ファーム更新処理をROMからRAMにコピー */
	pSrc = (uint8_t __far *)( __sectop( "FIRMUP_APP_f" ) );						/* FIRMUP_APPをROMからRAMにコピー	*/
	pDst = (uint8_t __far *)( __sectop( "FIRMUP_APP_RAM" ) );
	
	for( ; pSrc < (uint8_t __far *)( __secend( "FIRMUP_APP_f" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDTのカウントリフレッシュ		*/
	}
	
	/* FSLライブラリ使用領域をROMからRAMにコピー */
	pSrc = (uint8_t __far *)( __sectop( "FSL_FCD" ) ); 							/* FSL_FCDをROMからRAMにコピー 		*/
	pDst = (uint8_t __far *)( __sectop( "FSL_FCD_RAM" ) );
	
	for( ; pSrc < (uint8_t __far *)( __secend( "FSL_FCD" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDTのカウントリフレッシュ		*/
	}
	
#if 0
	pSrc = (uint8_t __far *)( __sectop( "FSL_FECD" ) ); 						/* FSL_FECDをROMからRAMにコピー		*/
	pDst = (uint8_t __far *)( __sectop( "FSL_FECD_RAM" ) );

	for( ; pSrc < (uint8_t __far *)( __secend( "FSL_FECD" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDTのカウントリフレッシュ		*/
	}
#endif
	
	pSrc = (uint8_t __far *)( __sectop( "FSL_RCD" ) ); 							/* FSL_RCDをROMからRAMにコピー		*/
	pDst = (uint8_t __far *)( __sectop( "FSL_RCD_RAM" ) );
	
	for( ; pSrc < (uint8_t __far *)( __secend( "FSL_RCD" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDTのカウントリフレッシュ		*/
	}
	
	pSrc = (uint8_t __far *)( __sectop( "FSL_BCD" ) ); 							/* FSL_BCDをROMからRAMにコピー		*/
	pDst = (uint8_t __far *)( __sectop( "FSL_BCD_RAM" ) );
	
	for( ; pSrc < (uint8_t __far *)( __secend( "FSL_BCD" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDTのカウントリフレッシュ		*/
	}
	
	pSrc = (uint8_t __far *)( __sectop( "FSL_BECD" ) ); 						/* FSL_BECDをROMからRAMにコピー		*/
	pDst = (uint8_t __far *)( __sectop( "FSL_BECD_RAM" ) );
	
	for( ; pSrc < (uint8_t __far *)( __secend( "FSL_BECD" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDTのカウントリフレッシュ		*/
	}
	
	SubFirmUpdate();															/* ROM書換え実行					*/
}
#pragma section

/* **** Section **** */
#pragma section text FIRMUP_APP
/*
 *******************************************************************************
 *	ROM書き換え処理
 *
 *	[内容]
 *	実際にROM書き換えを実施する。
 *	※現状、途中でリセットや電源断があることを想定していない。 
 *******************************************************************************
 */
static __far void SubFirmUpdate( void )
{
	fsl_u08		wkResult = FSL_OK;
	uint8_t		wkRetryCnt = 0U;
	ET_FirmUpSts_t	wkFirmUpSts = ecFirmUpSts_Init;
	fsl_u16		wkBlock = 0U;
	uint8_t			wkRet = FLASH_SUCCESS;
	
	while( 1 )
	{
		WDTE = 0xACU;													/* WDTのカウントリフレッシュ */
		if( wkResult != FSL_OK )
		{
			wkRetryCnt++;
			
			if( wkRetryCnt > 2U )
			{
				/* 連続3回失敗で強制リセット */
				WDTE = 0xFFU;											/* 強制リセット */
			}
			else
			{
				wkBlock = 0U;
				wkFirmUpSts = ecFirmUpSts_Erase;						/* 最初のブロックから消去をやり直す */
			}
		}
		
		switch( wkFirmUpSts )
		{
			/* 初期化 */
			case ecFirmUpSts_Init :
				wkResult = SubFSLStart();								/* FSLライブラリ使用準備 */
				if( wkResult == FSL_OK )
				{
					wkFirmUpSts = ecFirmUpSts_Erase;
					wkBlock = 0U;
					wkRetryCnt = 0U;
				}
				break;
			/* 消去 */
			case ecFirmUpSts_Erase :
				wkResult = FSL_Erase( wkBlock );
				if( wkResult == FSL_OK )
				{														/* 正常終了 */
					wkBlock++;											/* 次ブロックに更新 */
					if( wkBlock > imFirmUp_BlkWrMaxNum )
					{
						wkBlock = 0U;									/* ブランクチェック用に初期化 */
						wkRetryCnt = 0U;								/* リトライ回数初期化 */
						/* 最終ブロックまで正常に消去が終了したらブランクチェックに移行 */
						wkFirmUpSts = ecFirmUpSts_BlankCheck;
					}
				}
				else
				{														/* 異常終了 */
					;
				}
				break;													/* break(リトライ動作) */
			/* ブランクチェック */
			case ecFirmUpSts_BlankCheck :
				wkResult = FSL_BlankCheck( wkBlock );
				if( wkResult == FSL_OK )
				{														/* 正常終了 */
					wkBlock++;											/* 次ブロックに更新 */
					if( wkBlock > imFirmUp_BlkWrMaxNum )
					{
						wkBlock = 0U;									/* ブランクチェック用に初期化 */
						wkRetryCnt = 0U;								/* リトライ回数初期化 */
						/* 最終ブロックまで正常にブランクされていたら書き込みに移行 */
						wkFirmUpSts = ecFirmUpSts_Write;
					}
				}
				else
				{														/* 異常終了 */
					;
				}
				break;
			/* 書込み */
			case ecFirmUpSts_Write :
				wkRet = SubR_Fl_PrgTrgtArea();
				if( wkRet == FLASH_COMPLETE )
				{														/* 正常終了 */
					/* 正常終了なら終了処理に移行 */
					wkFirmUpSts = ecFirmUpSts_Close;
					wkRetryCnt = 0U;
				}
				else
				{
					wkResult = FSL_ERR_WRITE;							/* 書き込み異常 */
				}
				break;
			/* 終了処理 */
			case ecFirmUpSts_Close :
				FSL_Close();
				wkFirmUpSts = ecFirmUpSts_End;
				break;
				
			/* 終了 */
			case ecFirmUpSts_End :
				WDTE = 0xFFU;					/* 強制リセット */
				while( 1U );
				
			default :
				break;
		}
	}
}


/*
 *******************************************************************************
 *	FSL使用準備処理
 *
 *	[内容]
 *	FSLライブラリの使用準備を行う。 
 *******************************************************************************
 */
static __far fsl_u08 SubFSLStart( void )
{
#if (swKouteiMode == imDisable)
	/* フルスピードモード、動作クロック 24MHz、ステータスチェックインターナルモード設定 */
	const __far fsl_descriptor_t cFSL_Descriptor_pStr = { 0x00, 0x18, 0x01 };
	fsl_u08 wkResult;
	
	WDTE = 0xACU;																/* WDTのカウントリフレッシュ */
	/* フラッシュ・セルフ・プログラミング・ライブラリ初期化実行 */
	wkResult = FSL_Init( &cFSL_Descriptor_pStr );
	
	/* 初期化が正常に終了した場合 */
	if( wkResult == FSL_OK )
	{
		/* フラッシュ・セルフ・プログラミング・ライブラリ開始処理 */
		FSL_Open();
		FSL_PrepareFunctions();
#if 0
		FSL_PrepareExtFunctions();
#endif
	}
	
	return wkResult;
#endif
}


/*
 *******************************************************************************
 *	データ読み出し
 *
 *	[内容]
 *		指定したアドレスのデータをプライマリメモリから読み出します。
 *	[引数]
 *		uint32_t arAddress：アドレス
 *		uint8_t arRdData：データ格納先
 *		uint16_t arNum：データ数
 *	[注意事項]
 *		一度に読み出せるデータは256Byteです。
 *******************************************************************************
 */
static __far ET_Error_t SubFirmUpMain_SubFlash_ReadData( uint32_t arAddress, uint8_t *parRdData, uint16_t arNum )
{
#if (swKouteiMode == imDisable)
	ET_Error_t			wkError;												/* エラー情報 */
	uint8_t				wkLoop;													/* ループカウンタ */
	uint8_t				wkReg1Data;												/* レジスタデータ */
	uint8_t				wkRcvData;												/* 受信データ */
	uint16_t			wkNum;													/* 読み出しデータ数 */
	uint32_t			wkAddr;													/* アドレス */
	uint32_t			wkTestBit;												/* ビットテスト位置 */
	uint32_t			cBitTable;
	
	/*
	 ***************************************
	 *	const定義にするとコンパイラにより
	 *	memcpyに変換されてしまうため、
	 *	手動で値を格納している。
	 ***************************************
	 */
	cBitTable  = M_BIT23;
	
	/* ステータスレジスタ1読み出し */
	
	/* チップセレクト＝Low */
	P_CS_PRIM = (uint8_t)imLow;													/* チップセレクト(プライマリ)＝Low */
	
	M_NOP;																		/* 1ステート(=1/(24*10^6)=41ns) */
	M_NOP;																		/* 1ステート(=1/(24*10^6)=41ns) */
	
	/* コマンド送信 */
	/* シリアルクロック＝Low */
	P_SCLK = imLow;																/* SCLK(シリアルクロック)＝Low */
	
	/* RDSR1コマンド送信 */
	SubFSLCmdSnd( imFlashDrv_RDSR1 );
	
	/* ステータスレジスタ受信 */
	wkReg1Data = SubFSLCmdRcv();
	
	M_NOP;																		/* 1ステート(=1/(24*10^6)=41ns) */
	M_NOP;																		/* 1ステート(=1/(24*10^6)=41ns) */
	
	/* チップセレクト＝High */
	P_CS_PRIM = imHigh;															/* チップセレクト(プライマリ)＝High */
	
	
	if ( ( M_TSTBIT( wkReg1Data, imFlashDrv_Sts1Bit_BUSY ) )					/* BUSY */
	||	 ( parRdData == M_NULL )												/* NULL */
	||	 ( arAddress > imFirmUp_RomMaxAddr )									/* アドレス指定NG */
	||	 ( ( arAddress + arNum ) > ( imFirmUp_RomMaxAddr + 1U ) )				/* データ数指定NG */
	||	 ( arNum > imFirmUp_MaxReadNum )										/* データ数指定NG */
	)
	{
		wkError = ecERR_NG;														/* エラー情報＝NG */
	}
	else
	{
		/* 変数初期化 */
		wkError = ecERR_OK;														/* エラー情報初期化 */
		
		/* チップセレクト＝Low */
		P_CS_PRIM = imLow;														/* チップセレクト(プライマリ)＝Low */
		
		M_NOP;																	/* 1ステート(=1/(24*10^6)=41ns) */
		M_NOP;																	/* 1ステート(=1/(24*10^6)=41ns) */
		
		/* コマンド送信 */
		/* シリアルクロック＝Low */
		P_SCLK = imLow;															/* SCLK(シリアルクロック)＝Low */
		
		/* READコマンド送信 */
		SubFSLCmdSnd( imFlashDrv_RDDT );
		/* EEPROMアドレスの送信 */
		wkAddr = arAddress;														/* EEPROMアドレス */
		for( wkLoop = 0U; wkLoop < 24U; wkLoop++ )
		{
			/*
			 ***************************************
			 *	2byteを超えるシフト演算はランタイム
			 *	ライブラリ呼び出しとなってしまうため、
			 *	演算しないよう固定値代入としている。
			 ***************************************
			 */
			if( wkLoop == 0U )
			{
				wkTestBit = cBitTable;
			}
			else
			{
				wkTestBit /= 2U;
			}
			
			if( (wkAddr & wkTestBit) == 0U )
			{
				P_SDO = imLow;													/* シリアル通信データ出力 */
			}
			else
			{
				P_SDO = imHigh;													/* シリアル通信データ出力 */
			}
			
			P_SCLK = imHigh;													/* SCLK(シリアルクロック)＝High */
			
			M_NOP;																/* 1ステート(=1/(24*10^6)=41ns) */
			
			P_SCLK = imLow;														/* SCLK(シリアルクロック)＝Low */
		}
		
		/* データ受信 */
		for( wkNum = 0U; wkNum < arNum; wkNum++ )
		{
			wkRcvData = SubFSLCmdRcv();
			*parRdData = wkRcvData;												/* EEPROMデータ格納 */
		}
		
		M_NOP;																	/* 1ステート(=1/(24*10^6)=41ns) */
		M_NOP;																	/* 1ステート(=1/(24*10^6)=41ns) */
		
		/* チップセレクト＝High */
		P_CS_PRIM = imHigh;														/* チップセレクト(プライマリ)＝High */
	}

	return wkError;
#endif
}


/*
 *******************************************************************************
 *	Flashへのデータ送信
 *
 *	[内容]
 *		Flashへデータを送信する。
 *	[引数]
 *		uint8_t arSndData：送信データ
 *	[注意事項]
 *		送信できるデータサイズは1Byte
 *******************************************************************************
 */
static __far void SubFSLCmdSnd( uint8_t arSndData )
{
#if (swKouteiMode == imDisable)
	uint8_t				wkLoop;												/* ループカウンタ */
	uint8_t				wkTestBit;											/* ビットテスト位置 */
	
	for( wkLoop = 0U; wkLoop < 8U; wkLoop++ )
	{
		wkTestBit = M_BIT7 >> wkLoop;										/* ビットテスト位置 */
		if( (arSndData & wkTestBit) == 0U )
		{
			P_SDO = imLow;													/* シリアル通信データ出力 */
		}
		else
		{
			P_SDO = imHigh;													/* シリアル通信データ出力 */
		}
		
		P_SCLK = imHigh;													/* SCLK(シリアルクロック)＝High */
		
		M_NOP;																/* 1ステート(=1/(24*10^6)=41ns) */
		
		P_SCLK = imLow;														/* SCLK(シリアルクロック)＝Low */
	}
#endif
}


/*
 *******************************************************************************
 *	Flashからデータリード
 *
 *	[内容]
 *		Flashからデータをリードする。
 *	[引数]
 *		uint8_t wkRcvData：リードデータ
 *	[注意事項]
 *		リードするデータサイズは1Byte
 *******************************************************************************
 */
static __far uint8_t SubFSLCmdRcv( void )
{
#if (swKouteiMode == imDisable)
	uint8_t				wkLoop;												/* ループカウンタ */
	uint8_t				wkRcvData;											/* 受信データ */
	
	wkRcvData = 0U;
	for( wkLoop = 0U; wkLoop < 8U; wkLoop++ )
	{
		wkRcvData <<= 1U;
		wkRcvData |= (uint8_t)P_SDI;										/* EEPROMのシリアル通信データ入力 */
		P_SCLK = imHigh;													/* SCLK(シリアルクロック)＝High */
		
		M_NOP;																/* 1ステート(=1/(24*10^6)=41ns) */
		
		P_SCLK = imLow;														/* SCLK(シリアルクロック)＝Low */
	}
	return wkRcvData;
#endif
}

/******************************************************************************
* [ﾓｼﾞｭｰﾙ名] : R_Fl_PrgTrgtArea
*	[日本名] : ROMエリアへのプログラム書き込み
*	  [概要] : 〃
*	  [機能] : 〃
*	  [入力] : なし
*	  [出力] : FLASH_COMPLETE、FLASH_FAILURE
*	  [備考] : なし
*	  [上位] : Flash_main();
*	  [下位] :	SubFirmUpMain_SubFlash_ReadData();				Flash ICからデータリード
*				SubR_Fl_Prg_StoreHEX();
*				SubR_Fl_Prg_ProcessForHEX_data();
*				SubR_Fl_Prg_MakeWriteData();
*				SubR_Fl_Prg_WriteData();
******************************************************************************/
static __far uint8_t SubR_Fl_PrgTrgtArea( void )
{
#if (swKouteiMode == imDisable)
	uint8_t		tmp_ret;
	uint8_t		tmp_data;
	sint16_t	ret = FLASH_OK;

	/* **** Clear all data for HEX format **** */
	SubR_Fl_Prg_ClearHEXVariables();
	
	while( 1 )
	{
		if( 0 == fl_hex_data_flag )
		{
			/* **** If there is no HEX format data. **** */
			while( 1 )
			{
				if( (flash_addr > 0xFFFFF) || (ret != FLASH_OK) ){
					tmp_ret = FLASH_FAILURE;									/* Flashアドレスオーバーエラー	*/
					break;
				}
				ret = SubFirmUpMain_SubFlash_ReadData( flash_addr, &tmp_data, 1u );
				flash_addr++;

				tmp_ret = SubR_Fl_Prg_StoreHEX( tmp_data );

				/* SubR_Fl_Prg_StoreHEXからの返り値がFLASH_BUSYからFLASH_SUCCESSまでLoopする */
				if( FLASH_SUCCESS == tmp_ret )
				{
					/* ==== If complete 1 format, process for HEX format ==== */
					tmp_ret = SubR_Fl_Prg_ProcessForHEX_data();
					/* After analyze, break loop */
					break;
				}
				WDTE = 0xACU;													/* WDTのカウントリフレッシュ */
			}
		}
		else
		{
		/* **** Prepare the write data **** */
			tmp_ret = SubR_Fl_Prg_MakeWriteData();
			if( FLASH_SUCCESS == tmp_ret )
			{
				/* ==== If complete preparation, write data to flash ==== */
				SubR_Fl_Prg_WriteData();
			}
		}

		if( (tmp_ret == FLASH_COMPLETE ) || (tmp_ret == FLASH_FAILURE) )
		{
			break;
		}
	}

	return tmp_ret;
#endif
}


/******************************************************************************
* [ﾓｼﾞｭｰﾙ名] : SubR_Fl_Prg_StoreHEX
*	[日本名] : HEXフォーマットデータを格納
*	  [概要] : 〃
*	  [機能] : 〃
*	  [入力] : HEXデータ
*	  [出力] : FLASH_SUCCESS、FLASH_FAILURE、FLASH_BUSY
*	  [備考] : なし
*	  [上位] : SubR_Fl_PrgTrgtArea();
*	  [下位] : SubR_Fl_AsciiToHexByte();
******************************************************************************/
static __far uint8_t SubR_Fl_Prg_StoreHEX( uint8_t hex_data )
{
#if (swKouteiMode == imDisable)
	uint8_t ret_code = FLASH_BUSY;
	
	if( fl_hex_total_data_cnt == 0 )
	{
		/* **** Header field (1byte) **** */
		if(':' == hex_data)
		{
			/* ==== If data is ':', store the data. ==== */
			fl_wr_hex.type[ 0 ] = hex_data;

			/* Increment counter */
			fl_hex_total_data_cnt++;
		}
		else
		{
			/* ==== If type field is not ':', ignore and clear data ==== */
				ret_code = FLASH_FAILURE;
		}
	}
	else if( (fl_hex_total_data_cnt == 1) || (fl_hex_total_data_cnt ==2) )
	{
		/* **** Length field (2byte) **** */

		/* Store the length */
		fl_wr_hex.len[ fl_hex_total_data_cnt - 1 ] = hex_data;

		/* Increment counter */
		fl_hex_total_data_cnt++;

		if( fl_hex_total_data_cnt == 3 ){
			/* Convert the length data from ascii code to binary */
			fl_hex_length = SubR_Fl_AsciiToHexByte( fl_wr_hex.len[ 0 ], fl_wr_hex.len[ 1 ] );
		}
	}
	else if(	(fl_hex_total_data_cnt == 3) || (fl_hex_total_data_cnt == 4) ||
				(fl_hex_total_data_cnt == 5) || (fl_hex_total_data_cnt == 6) )
	{
		/* **** Address (4byte) **** */

		/* Store the data */
		fl_wr_hex.addr_data_sum[ fl_hex_total_data_cnt - 3 ] = hex_data;

		/* Increment counter */
		fl_hex_total_data_cnt++;
	}
	else if( (fl_hex_total_data_cnt == 7) || (fl_hex_total_data_cnt == 8) )
	{
		/* **** レコードタイプ(2byte) **** */
		fl_wr_hex.type[ 1 ] = hex_data; 										/*下位1byteを採用	*/

		/* Store the data */
		fl_wr_hex.addr_data_sum[ fl_hex_total_data_cnt - 3 ] = hex_data;

		/* ==== Increment counter ==== */
		fl_hex_total_data_cnt++;
	}
	else{
		/* **** Data or Sum **** */

		/* Store the data */
		fl_wr_hex.addr_data_sum[ fl_hex_total_data_cnt - 3 ] = hex_data;

		/* Increment counter */
		fl_hex_total_data_cnt++;

		/* Received all data? */
		if( fl_hex_total_data_cnt - 9 >= (fl_hex_length * 2 + FL_HEX_SUM_SIZE * 2) )
		{
			/* Clear counter */
			fl_hex_total_data_cnt = 0;
			/* When received all data, return OK */
			ret_code = FLASH_SUCCESS;
		}
	}

	return ret_code;
#endif
}


/******************************************************************************
* [ﾓｼﾞｭｰﾙ名] : SubR_Fl_Prg_ClearHEXVariables
*	[日本名] : モトローラSにおける変数のクリア
*	  [概要] : 〃
*	  [機能] : 〃
*	  [入力] : なし
*	  [出力] : なし
*	  [備考] : なし
*	  [上位] : SubR_Fl_Prg_WriteData();
*	  [下位] : なし
******************************************************************************/
static __far void SubR_Fl_Prg_ClearHEXVariables( void )
{
#if (swKouteiMode == imDisable)
	uint16_t	wkLoop;

	fl_hex_data_flag = 0;
	flash_addr = 0xC0000;
	fl_hex_total_data_cnt = 0;
	fl_hex_length = 0;
	sAdr_upper = 0U;
	vSurplusCnt = 0U;
	
	fl_wr_hex_bin_surplus.len = 0U;
	fl_wr_hex_bin_surplus.addr = 0U;
	fl_writing_data.addr = 0;
	fl_wr_hex_bin.len = 0U;
	fl_wr_hex_bin.addr = 0U;
	for( wkLoop = 0U; wkLoop < 260U; wkLoop++ )
	{
		fl_wr_hex_bin_surplus.data[ wkLoop ] = 0xFFU;
		fl_writing_data.data[ wkLoop ] = 0xFFU;
		fl_wr_hex_bin.data[ wkLoop ] = 0xFFU;
	}
	
	fl_wr_hex.type[0] = 0;
	fl_wr_hex.type[1] = 0;
	fl_wr_hex.len[0] = 0;
	fl_wr_hex.len[1] = 0;
	for( wkLoop = 0U; wkLoop < 520U; wkLoop++ )
	{
		fl_wr_hex.addr_data_sum[ wkLoop ] = 0xFFU;
	}
#endif
}


/******************************************************************************
* [ﾓｼﾞｭｰﾙ名] : SubR_Fl_AsciiToHexByte
*	[日本名] : ASCIIデータをHEXデータへ変換
*	  [概要] : 〃
*	  [機能] : 〃
*	  [入力] :	in_upper		ASCIIコード上位
*				in_lower		ASCIIコード下位
*	  [出力] : バイナリデータ
*	  [備考] : なし
*	  [上位] :	SubR_Fl_Prg_StoreHEX();
*				SubR_Fl_Prg_HEX_AsciiToBinary();
*	  [下位] : なし
******************************************************************************/
static __far uint8_t SubR_Fl_AsciiToHexByte( uint8_t in_upper, uint8_t in_lower )
{
#if (swKouteiMode == imDisable)
	uint8_t in_buff8;

	/* **** Convert upper 4 bits **** */
	if( in_upper <= 0x39 )
	{
		/* The data is 0 to 9 */
		in_upper = ( in_upper & 0x0F );
	}
	else
	{
		/* The data is A to F */
		in_upper = ( in_upper - 0x37 );
	}

	/* **** Convert lower 4 bits **** */
	if( in_lower <= 0x39 )
	{
		/* The data is 0 to 9 */
		in_lower = ( in_lower & 0x0F );
	}
	else
	{
		/* The data is A to F */
		in_lower = ( in_lower - 0x37 );
	}

	/* **** Combine upper and lower **** */
	in_buff8 = (in_upper << 4) | in_lower;

	return in_buff8;
#endif
}


/******************************************************************************
* [ﾓｼﾞｭｰﾙ名] : SubR_Fl_Prg_ProcessForHEX_data
*	[日本名] : HEXヘッダの解析とバイナリデータへの変換
*	  [概要] : 〃
*	  [機能] : 〃
*	  [入力] : なし
*	  [出力] : FLASH_SUCCESS、FLASH_FAILURE、FLASH_COMPLETE
*	  [備考] : なし
*	  [上位] : SubR_Fl_PrgTrgtArea();
*	  [下位] :	SubR_Fl_Prg_HEX_AsciiToBinary();
*				SubR_Fl_Prg_ClearHEXVariables();
*				SubR_Fl_Prg_WriteData();
******************************************************************************/
static __far uint8_t SubR_Fl_Prg_ProcessForHEX_data( void )
{
#if (swKouteiMode == imDisable)
	uint8_t tmp_ret = FLASH_SUCCESS;
	uint8_t wk[ 4 ],i;

	switch( fl_wr_hex.type[ 1 ] )
	{
		case '4':
			/* **** 04:拡張リニアアドレスレコード **** */
			for( i=0;i<4;i++ )
			{
				wk[ i ] = fl_wr_hex.addr_data_sum[ i + 6 ];
			}
			sAdr_upper = SubR_Fl_AsciiToHexByte( wk[ 0 ], wk[ 1 ] );
			sAdr_upper <<= 8;
			sAdr_upper |= SubR_Fl_AsciiToHexByte( wk[ 2 ], wk[ 3 ] );
			sAdr_upper <<= 16;
		break;

		case '2':
			/* **** 02:セグメントアドレスレコード **** */
			for( i=0;i<4;i++ )
			{
				wk[ i ] = fl_wr_hex.addr_data_sum[ i + 6 ];
			}
			sAdr_upper = SubR_Fl_AsciiToHexByte( wk[ 0 ], wk[ 1 ] );
			sAdr_upper <<= 8;
			sAdr_upper |= SubR_Fl_AsciiToHexByte( wk[ 2 ], wk[ 3 ] );
			sAdr_upper <<= 4;
		break;

		case '0':
			/* **** 00:データレコード **** */
			/* Convert all HEX data from Ascii to Binary */
			tmp_ret = SubR_Fl_Prg_HEX_AsciiToBinary( &fl_wr_hex, &fl_wr_hex_bin );
			if( FLASH_SUCCESS == tmp_ret )
			{
				/* If data complete, set HEX format flag */
				fl_hex_data_flag = 1;
			}
		break;

		case '5':
			/* **** 05:スタートリニアアドレス **** */
			break;

		case '1':
			/* **** 01:エンドレコード **** */
			tmp_ret = FLASH_COMPLETE;

			SubR_Fl_Prg_WriteData();

			/* Clear all data for Mot S format */
			SubR_Fl_Prg_ClearHEXVariables();
		break;

		default:
			/* **** If receive others **** */
			tmp_ret = FLASH_FAILURE;			/* 無効データ		*/

			/* Clear all data for Mot S format */
			SubR_Fl_Prg_ClearHEXVariables();
		break;
	}

	return tmp_ret;
#endif
}


/******************************************************************************
* [ﾓｼﾞｭｰﾙ名] : SubR_Fl_Prg_HEX_AsciiToBinary
*	[日本名] : ASCIIコードをバイナリデータに変換
*	  [概要] : 〃
*	  [機能] : 〃
*	  [入力] :	Fl_prg_hex_t *tmp_hex : ASCIIデータ
*				Fl_prg_hex_binary_t *tmp_hex_binary : バイナリデータ
*	  [出力] : FLASH_SUCCESS、FLASH_FAILURE
*	  [備考] : なし
*	  [上位] : SubR_Fl_Prg_ProcessForHEX_data();
*	  [下位] : SubR_Fl_AsciiToHexByte();
*				memcpy();
******************************************************************************/
static __far uint8_t SubR_Fl_Prg_HEX_AsciiToBinary(
						Fl_prg_hex_t *tmp_hex,
						Fl_prg_hex_binary_t *tmp_hex_binary )
{
#if (swKouteiMode == imDisable)
	uint8_t 		ret_code = FLASH_FAILURE;
	uint32_t		cnt;
	uint16_t		tmp_len;
	uint8_t 		tmp_sum;
	uint8_t 		fl_wr_hex_bin_tmp[ 256 + FL_HEX_ADDR_SIZE + FL_HEX_SUM_SIZE ];
	uint16_t		wkLoop;

	/* **** Store length **** */
	tmp_len = SubR_Fl_AsciiToHexByte( tmp_hex->len[ 0 ], tmp_hex->len[ 1 ] );
	tmp_sum = tmp_len;

	/* **** Store all data **** */
	for( cnt = 0; cnt < (tmp_len + FL_HEX_ADDR_SIZE + FL_HEX_TYP_SIZE + FL_HEX_SUM_SIZE); cnt++ )
	{
		fl_wr_hex_bin_tmp[ cnt ] = SubR_Fl_AsciiToHexByte(
							tmp_hex->addr_data_sum[ cnt * 2 ],
							tmp_hex->addr_data_sum[ cnt * 2 + 1 ] );
		tmp_sum += fl_wr_hex_bin_tmp[ cnt ];
	}

	/* **** Check sum **** */
	if( 0x00 == tmp_sum )
	{
		/* ==== Copy length ==== */
		tmp_hex_binary->len = tmp_len;

		/* ==== Copy address ==== */
		tmp_hex_binary->addr = 0;
		for(cnt = 0; cnt < FL_HEX_ADDR_SIZE; cnt++)
		{
			/* Shift data */
			tmp_hex_binary->addr = tmp_hex_binary->addr << 8;
			/* Read data */
			tmp_hex_binary->addr |= fl_wr_hex_bin_tmp[ cnt ];
		}
		tmp_hex_binary->addr |= sAdr_upper;

		/* ==== Copy data ==== */
		for( wkLoop = 0U; wkLoop < tmp_hex_binary->len; wkLoop++ )
		{
			tmp_hex_binary->data[ wkLoop ] = fl_wr_hex_bin_tmp[ FL_HEX_ADDR_SIZE + FL_HEX_TYP_SIZE + wkLoop ];
		}
		ret_code = FLASH_SUCCESS;
	}
	else
	{
		/* ==== Clear all data for Mot S format ==== */
		SubR_Fl_Prg_ClearHEXVariables();
	}

	return ret_code;
#endif
}


/******************************************************************************
* [ﾓｼﾞｭｰﾙ名] : SubR_Fl_Prg_WriteData
*	[日本名] : ROMエリアにデータをライトする
*	  [概要] : 〃
*	  [機能] : 〃
*	  [入力] : なし
*	  [出力] : FLASH_SUCCESS、FLASH_FAILURE
*	  [備考] : なし
*	  [上位] :	SubR_Fl_PrgTrgtArea();
*				SubR_Fl_Prg_ProcessForHEX_data();
*	  [下位] :	SubR_FlashWrite();
*				SubR_Fl_Prg_ClearHEXVariables();
*				memset();
*				memcpy();
******************************************************************************/
static __far uint8_t SubR_Fl_Prg_WriteData( void )
{
#if (swKouteiMode == imDisable)
	uint8_t 	ret_code = FLASH_SUCCESS;
	uint8_t 	ret_tmp;
	uint16_t	cnt;
	uint32_t	tmp_addr_cnt;
	fsl_write_t	wkstFSLwrite;													/* ライブラリ引数用構造体 */
	uint16_t	wkLoop;

	wkstFSLwrite.fsl_data_buffer_p_u08 = (fsl_u08 *)&fl_writing_data.data[ 0 ];	/* 書込むデータの先頭アドレスを指定 */
	wkstFSLwrite.fsl_destination_address_u32 = (fsl_u32)fl_writing_data.addr;	/* 書込み先の先頭アドレス */
	wkstFSLwrite.fsl_word_count_u08 = 64U;										/* 書込むデータ数(64ワード = 256バイト) */

	/* **** Write data to Flash **** */
	ret_tmp = FSL_Write( &wkstFSLwrite );										/* ROM書込み */

	/* ==== Check for errors ==== */
	if( 0 != ret_tmp )
	{
		/* Clear all data for HEX format */
		SubR_Fl_Prg_ClearHEXVariables();
		ret_code = FLASH_FAILURE;
		cnt = 0;
	}
	else
	{
		/* **** If no error, read back written area **** */
		for ( cnt = 0; cnt < 0x0100; cnt++ )
		{
			/* Verify */
			if ( *wkstFSLwrite.fsl_data_buffer_p_u08 != fl_writing_data.data[ cnt ] )
			{
				/* Clear all data for HEX format */
				SubR_Fl_Prg_ClearHEXVariables();
				ret_code = FLASH_FAILURE;
				break;
			}
			wkstFSLwrite.fsl_data_buffer_p_u08++;
		}

		/* ==== Clear base address ==== */
		fl_writing_data.addr = 0;
		
		/* ==== Clear writing data ==== */
		for( wkLoop = 0U; wkLoop < 260U; wkLoop++ )
		{
			fl_writing_data.data[ wkLoop ] = 0xFFU;
		}

		if( 0 != fl_wr_hex_bin_surplus.addr )
		{
			/* **** If there is surplus data, set the data to write buffer */

			/* ==== Set base address ==== */
			fl_writing_data.addr = fl_wr_hex_bin_surplus.addr & 0xFFFFFF00;

			/* ==== Copy data ==== */
			tmp_addr_cnt = fl_wr_hex_bin_surplus.addr - fl_writing_data.addr;
			
			for( wkLoop = 0U; wkLoop < fl_wr_hex_bin_surplus.len; wkLoop++ )
			{
				if( tmp_addr_cnt + wkLoop > 255U )
				{
					/* 余剰(surplus.data)が残っている場合、書き込み実行 */
					fl_wr_hex_bin_surplus.len = fl_wr_hex_bin_surplus.len - wkLoop + 1U;
					vSurplusCnt = wkLoop;
					ret_code = SubR_Fl_Prg_SurplusWriteData();
					break;
				}
				else
				{
					fl_writing_data.data[ tmp_addr_cnt + wkLoop ] = fl_wr_hex_bin_surplus.data[ wkLoop ];
				}
			}
			
			/* ==== Clear HEX format complete flag and buffer ==== */
			fl_hex_data_flag = 0;
			fl_wr_hex_bin_surplus.addr = 0;
		}
	}

	return ret_code;
#endif
}


/******************************************************************************
* [ﾓｼﾞｭｰﾙ名] : SubR_Fl_Prg_MakeWriteData
*	[日本名] : ROMエリアにライトするデータ生成
*	  [概要] : 〃
*	  [機能] : 〃
*	  [入力] : なし
*	  [出力] : FLASH_SUCCESS、FLASH_BUSY
*	  [備考] : なし
*	  [上位] : SubR_Fl_PrgTrgtArea();
*	  [下位] : memcpy();
******************************************************************************/
static __far uint8_t SubR_Fl_Prg_MakeWriteData( void )
{
#if (swKouteiMode == imDisable)
	uint8_t 		ret_code = FLASH_SUCCESS;
	uint32_t		tmp_addr_cnt;
	uint32_t		read_cnt;
	uint32_t		cnt;
	uint16_t		wkLoop;

	/* **** If there is not surplus data, set next data */
	if( 0 == fl_writing_data.addr )
	{
		/* ==== If there is no base address, set base address ==== */
		fl_writing_data.addr = fl_wr_hex_bin.addr & 0xFFFFFF00;
	}

	tmp_addr_cnt = fl_wr_hex_bin.addr - fl_writing_data.addr;
	if( tmp_addr_cnt + fl_wr_hex_bin.len > 0x00000100 )
	{
		/* ==== If receive data more than blank size ==== */

		/* Copy data until write buffer is full */
		for( cnt = 0; tmp_addr_cnt < 0x00000100; cnt++ )
		{
			fl_writing_data.data[ tmp_addr_cnt++ ] = fl_wr_hex_bin.data[ cnt ];
		}
		read_cnt = cnt;

		/* ==== Store the surplus data ==== */
		fl_wr_hex_bin_surplus.addr = fl_wr_hex_bin.addr + read_cnt;
		fl_wr_hex_bin_surplus.len = fl_wr_hex_bin.len - read_cnt;

		for( wkLoop = 0U; wkLoop < fl_wr_hex_bin_surplus.len; wkLoop++ )
		{
			fl_wr_hex_bin_surplus.data[ wkLoop ] = fl_wr_hex_bin.data[ read_cnt + wkLoop ];
		}
	}
	else
	{
		/* ==== If receive data less than blank size ==== */
		/* Store all data */
		for( wkLoop = 0U; wkLoop < fl_wr_hex_bin.len; wkLoop++ )
		{
			fl_writing_data.data[ tmp_addr_cnt + wkLoop ] = fl_wr_hex_bin.data[ wkLoop ];
		}

		/* Clear HEX format complete flag */
		fl_hex_data_flag = 0;
	}

	if ( tmp_addr_cnt < 0x0100 )
	{
		/* **** If writing data buffer is not full, return NG **** */
		ret_code = FLASH_BUSY;
	}

	return ret_code;
#endif
}


/******************************************************************************
* [ﾓｼﾞｭｰﾙ名] : SubR_Fl_Prg_SurplusWriteData
*	[日本名] : ROMエリアに余剰データをライトする
*	  [概要] : 余剰データがsurplus.dataに残っている場合のみ本関数を実行
*			   surplus.data内を空にする為、外付けフラッシュ読み出す前に
*			   連続して書き込む。
*	  [機能] : 〃
*	  [入力] : なし
*	  [出力] : FLASH_SUCCESS、FLASH_FAILURE
*	  [備考] : なし
*	  [上位] :	SubR_Fl_Prg_WriteData();
******************************************************************************/
static uint8_t SubR_Fl_Prg_SurplusWriteData( void )
{
#if (swKouteiMode == imDisable)
	uint8_t 	ret_code = FLASH_SUCCESS;
	uint8_t 	ret_tmp;
	uint16_t	cnt;
	fsl_write_t	wkstFSLwrite;													/* ライブラリ引数用構造体 */
	uint16_t	wkLoop;

	wkstFSLwrite.fsl_data_buffer_p_u08 = (fsl_u08 *)&fl_writing_data.data[ 0 ];	/* 書込むデータの先頭アドレスを指定 */
	wkstFSLwrite.fsl_destination_address_u32 = (fsl_u32)fl_writing_data.addr;	/* 書込み先の先頭アドレス */
	wkstFSLwrite.fsl_word_count_u08 = 64U;										/* 書込むデータ数(64ワード = 256バイト) */

	/* **** Write data to Flash **** */
	ret_tmp = FSL_Write( &wkstFSLwrite );										/* ROM書込み */
	/* ==== Check for errors ==== */
	if( 0 != ret_tmp )
	{
		/* Clear all data for HEX format */
		SubR_Fl_Prg_ClearHEXVariables();
		ret_code = FLASH_FAILURE;
		cnt = 0;
	}
	else
	{
		/* **** If no error, read back written area **** */
		for ( cnt = 0; cnt < 0x0100; cnt++ )
		{
			/* Verify */
			if ( *wkstFSLwrite.fsl_data_buffer_p_u08 != fl_writing_data.data[ cnt ] )
			{
				/* Clear all data for HEX format */
				SubR_Fl_Prg_ClearHEXVariables();
				ret_code = FLASH_FAILURE;
				break;
			}
			wkstFSLwrite.fsl_data_buffer_p_u08++;
		}

		/* ==== Clear base address ==== */
		fl_writing_data.addr = 0;
		/* ==== Clear writing data ==== */
		for( wkLoop = 0U; wkLoop < 260U; wkLoop++ )
		{
			fl_writing_data.data[ wkLoop ] = 0xFFU;
		}

		if( 0 != fl_wr_hex_bin_surplus.addr )
		{
			/* **** If there is surplus data, set the data to write buffer */

			/* ==== Set base address ==== */
			fl_writing_data.addr = fl_wr_hex_bin_surplus.addr & 0xFFFFFF00;
			fl_writing_data.addr += 0x00000100U;								/* 次回のベースアドレスに更新 */

			for( wkLoop = 0U; wkLoop < fl_wr_hex_bin_surplus.len; wkLoop++ )
			{
				fl_writing_data.data[ wkLoop ] = fl_wr_hex_bin_surplus.data[ vSurplusCnt + wkLoop ];
			}
			/* ==== Clear HEX format complete flag and buffer ==== */
			fl_hex_data_flag = 0;
			fl_wr_hex_bin_surplus.addr = 0;
			vSurplusCnt = 0U;
		}
	}
	return ret_code;
#endif
}

#pragma section 
