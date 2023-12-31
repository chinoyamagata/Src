/*
 *******************************************************************************
 *	File name	:	Flash.c
 *
 *	[内容]
 *		フラッシュ処理
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2020.7
 *******************************************************************************
 */
#include "cpu_sfr.h"						/* SFR定義ヘッダー */
#include "typedef.h"						/* データ型定義 */
#include "UserMacro.h"						/* ユーザー作成マクロ定義 */
#include "immediate.h"						/* immediate定数定義 */
#include "switch.h"							/* コンパイルスイッチ定義 */
#include "enum.h"							/* 列挙型の定義 */
#include "struct.h"							/* 構造体定義 */
#include "func.h"							/* 関数の外部参照(extern)定義 */
#include "glLabel.h"						/* グローバル変数・定数(const)ラベル定義 */
#include "RF_Struct.h"

#include <string.h>


/*
 *==============================================================================
 *	define定義
 *==============================================================================
 */
#define	P_FLASH_POWER		(P4_bit.no5)			/* フラッシュ電源制御 */
#define	P_FLASH_POWER_IO	(PM4_bit.no5)			/* フラッシュ電源制御ポートIO */

#define imRetryMax				5U					/* リトライMAX */
#define imEraseRetryMax			3000U				/* イレースリトライMAX、261cnt:28.8msec=Max:300msec -> Max=2718cnt */


//#define imFlashQue_DatNum		24U					/* データキューのデータ個数 */
#define imFlashQue_DatNum		72U					/* データキューのデータ個数 */
//#define imFlashQueLowBat_DatNum	48U					/* 低電圧時用データキューのデータ個数 */
#define imFlashQue_DatSize		13U					/* データキューのデータサイズ(Byte) */
#define imFlashRdWr_DatSize		13U					/* リードライトデータサイズ(Byte) */
#define imFlashSctrSize			4096UL				/* セクタサイズ(Byte) */

/* 1セクタのサイズ数 */
#define	imFlashErrInfoSctrSize	2500U				/* 異常情報 */
#define	imFlashMeasAlmSctrSize	3500U				/* 計測警報 */
#define	imFlashMeas1SctrSize	4092U				/* 計測値1 */
#define	imFlashMeas2SctrSize	4092U				/* 計測値2 */
#define	imFlashEvtSctrSize		3600U				/* イベント */
#define	imFlashActLogSctrSize	4050U				/* 動作履歴 */
#define	imFlashSysLogSctrSize	4096UL				/* システムログ */
#define	imFlashAdrTblSctrSize	4096UL				/* アドレス特定テーブル */

/* 1セクタのデータ数 */
#define	imDataNumInSctrErrInf	500U				/* 異常情報 */
#define	imDataNumInSctrMeasAlm	500U				/* 計測警報 */
#define	imDataNumInSctrMeas1	341U				/* 計測値1 */
#define	imDataNumInSctrMeas2	341U				/* 計測値2 */
#define	imDataNumInSctrEvt		300U				/* イベント */
#define	imDataNumInSctrActLog	450U				/* 動作履歴 */
#define	imDataNumInSctrSysLog	512U				/* システムログ */
#define	imDataNumInSctrAdrTbl	512U				/* アドレス特定テーブル */


/* 使用セクタのMAX値(予備セクタは含めず) */
#define imSctrNumErrInf			1U					/* 異常情報 */
#define imSctrNumMeasAlm		1U					/* 計測警報 */
#define imSctrNumMeas1			156U				/* 計測値1 */
#define imSctrNumMeas2			3U					/* 計測値2 */
#define imSctrNumEvt			1U					/* イベント */
#define imSctrNumActLog			1U					/* 動作履歴 */
#define imSctrNumSysLog			4U					/* システムログ */
#define	imSctrNumAdrTbl			3U					/* アドレス特定テーブル(予備セクタ含む) */


/* セクタNo. */
#define imSctrNoMinErrInf		0U					/* 異常情報 */
#define imSctrNoMaxErrInf		1U
#define imSctrNoMinMeasAlm		2U					/* 計測警報 */
#define imSctrNoMaxMeasAlm		3U
#define imSctrNoMinMeas1		4U					/* 計測値1 */
#define imSctrNoMaxMeas1		160U
#define imSctrNoMinMeas2		161U				/* 計測値2 */
#define imSctrNoMaxMeas2		164U
#define imSctrNoMinEvt			165U				/* イベント */
#define imSctrNoMaxEvt			166U
#define imSctrNoMinActLog		167U				/* 動作履歴 */
#define imSctrNoMaxActLog		168U
#define imSctrNoMinSysLog		169U				/* システムログ */
#define imSctrNoMaxSysLog		173U
#define	imSctrNoMinMeas1Adr		174U				/* アドレス特定テーブル：計測値1 */
#define	imSctrNoMaxMeas1Adr		176U
#define	imSctrNoMinMeas2Adr		177U				/* アドレス特定テーブル：計測値2 */
#define	imSctrNoMaxMeas2Adr		179U
#define	imSctrNoMinSysLogAdr	180U				/* アドレス特定テーブル：システムログ */
#define	imSctrNoMaxSysLogAdr	182U


/* 第2エリアセクタ開始アドレス */
#define imSctr2ndStAdrErrInf	0x1000				/* 異常情報 */
#define imSctr2ndStAdrMeasAlm	0x3000				/* 計測警報 */
#define imSctr2ndStAdrMeas1		0xA0000				/* 計測値 */
#define imSctr2ndStAdrEvt		0xA6000				/* イベント */
#define imSctr2ndStAdrActLog	0xA8000				/* 動作履歴 */
#define imSctr2ndStAdrSysLog	0xAD000				/* システム履歴 */


/* データサイズ */
#define imDataByteErrInf		5U					/* 異常情報 */
#define imDataByteMeasAlm		7U					/* 計測警報 */
#define imDataByteMeas1			12U					/* 計測値1 */
#define imDataByteMeas2			12U					/* 計測値2 */
#define imDataByteEvt			12U					/* イベント */
#define imDataByteActLog		9U					/* 動作履歴 */
#define imDataByteSysLog		8U					/* システムログ */
#define	imDataByteAdrTbl		8U					/* アドレス特定テーブル */


/* セクタ終端アドレス */
#define imEndAddrErrInf			0x9C3U				/* 異常情報 */
#define imEndAddrMeasAlm		0xDABU				/* 計測警報 */
#define imEndAddrMeas1			0xFFBU				/* 計測値1 */
#define imEndAddrMeas2			0xFFBU				/* 計測値2 */
#define imEndAddrEvt			0xE0FU				/* イベント */
#define imEndAddrActLog			0xFD1U				/* 動作履歴 */
#define imEndAddrSysLog			0xFFFU				/* システムログ */
#define	imEndAddrAdrTbl			0xFFFU				/* アドレス特定テーブル */


/* セクタ終端の未使用サイズ */
#define imRsvSizeErrInf		1596U					/* 異常情報 */
#define imRsvSizeMeasAlm	596U					/* 計測警報 */
#define imRsvSizeMeas1		4U						/* 計測値1 */
#define imRsvSizeMeas2		4U						/* 計測値2 */
#define imRsvSizeEvt		496U					/* イベント */
#define imRsvSizeActLog		46U						/* 動作履歴 */
#define imRsvSizeSysLog		0U						/* システムログ */
#define	imRsvSizeAdrTbl		0U						/* アドレス特定テーブル */

#define imNonRetry			10U						/* リトライ無し */



/*
 *==============================================================================
 *	列挙型定義
 *==============================================================================
 */
typedef enum ET_SrchKindLog
{
	ecSrchKindLog_ErrInfo = 0U,						/* 異常情報 */
	ecSrchKindLog_MeasAlm,							/* 計測警報 */
	ecSrchKindLog_Evt,								/* イベント */
	ecSrchKindLog_ActLog,							/* 動作履歴 */
	ecSrchKindLog_AdrTblMeas1,						/* アドレステーブル：計測値1 */
	ecSrchKindLog_AdrTblSysLog,						/* アドレステーブル：システムログ */
	
	ecSrchKindLogMax								/* enum最大値 */
} ET_SrchKindLog_t;

typedef enum ET_ReadFlash
{
	ecReadFlash_OK = 0U,							/* 正常 */
	ecReadFlash_IndexNoErr,							/* IndexNoエラー */
	ecReadFlash_ReadErr,							/* 読み出しエラー */
	
	ecReadFlashMax									/* enum最大値 */
} ET_ReadFlash_t;


/*
 *==============================================================================
 *	構造体定義
 *==============================================================================
 */
/* データキュー */
typedef struct ST_FlashQue
{
	uint8_t			mData[imFlashQue_DatNum][imFlashQue_DatSize];				/* データ */
	uint8_t			mHead;														/* 格納位置の先頭 */
	uint8_t			mNum;														/* 格納個数 */
} ST_FlashQue_t;

#if 0
/* 低電圧時用データキュー */
typedef struct ST_FlashQueLowBat
{
	uint8_t			mData[imFlashQueLowBat_DatNum][imFlashQue_DatSize];			/* データ */
	uint8_t			mHead;														/* 格納位置の先頭 */
	uint8_t			mNum;														/* 格納個数 */
} ST_FlashQueLowBat_t;
#endif

/* フラッシュリード */
typedef struct ST_FlashRead
{
	uint32_t		mStartTime;
	uint32_t		mEndTime;
	uint16_t		mIndex;
} ST_FlashRead_t;

/* 領域種データ */
typedef struct ST_RegionData
{
	uint32_t		mStartAddr;							/* 開始アドレス */
	uint8_t			mWrData[imFlashRdWr_DatSize];		/* 書き込みデータ */
	ET_RegionKind_t	mRegionKind;						/* 領域種 */
	uint16_t		mWrNum;								/* 書き込み件数 */
	uint16_t		mDataByteNum;						/* データバイト数 */
	uint32_t		mWrAddrEnd;							/* 書き込み終端アドレス */
	uint32_t		mWrAddrFin;							/* 書き込み最終アドレス */
	uint16_t		mIndexNo;							/* IndexNo. */
} ST_RegionData_t;

/* リードリクエスト */
typedef struct ST_RequestReadData
{
	uint8_t			mReq;					/* リード要求フラグ */
	uint16_t		mFin;					/* リード完了フラグ */
} ST_RequestReadData_t;


typedef struct ST_Region
{
	ET_RegionKind_t	mRegionKind;			/* 領域種類 */
	uint16_t		mDataNumMax;			/* 収録データMax数 */
	uint16_t		mDataByte;				/* データサイズ */
	uint16_t		mIndexMax;				/* IndexNo.の最大数 */
	uint16_t		mRsvSize;				/* セクタ終端の未使用サイズ */
	uint16_t		mSctrMin;				/* 最小セクタ */
	uint16_t		mSctrMax;				/* 最大セクタ */
	uint16_t		mSctrNum;				/* 使用セクタ数 */
	uint16_t		mDataNumInSctr;			/* 1セクタのデータ数 */
	uint32_t		m2ndStAdr;				/* 第2エリアセクタ開始アドレス */
} ST_Region_t;


/*
 *==============================================================================
 *	変数定義
 *==============================================================================
 */
static ET_FlashSts_t			stFlashSts = ecFlashSts_PowOn;					/* フラッシュ処理状態 */

static ST_FlashQue_t			stFlashQue;										/* データキュー */

//static ST_FlashQueLowBat_t		stFlashQueLowBat;								/* 低電圧時用データキュー */

/* Writeキューデータ作成用 */
static ST_FlashVal1Evt_t		stFlashWrMeasVal1;								/* 計測値1 */

/* Readキューデータ作成用 */
static ST_FlashVal1Evt_t		stFlashRdMeasVal1[ 9U ];						/* 計測値1 */
static ST_FlashMeasAlm_t		stFlashRdMeasAlmVal[ 6U ];						/* 計測警報履歴 */


static ST_RegionData_t			stRegionData;									/* 領域種データ */
static ST_RequestReadData_t		stFlashReqRd = { 0U, 0U };						/* Flashリードリクエスト */

static uint16_t					vPrimRetryCnt = 0U;								/* プライマリ用リトライカウンタ */
static uint16_t					vSecondRetryCnt = 0U;							/* セカンダリ用リトライカウンタ */

static ET_ReadFlash_t			vReadFlash = ecReadFlash_OK;					/* フラッシュ読み出し結果 */


/*
 *==============================================================================
 *	定数定義
 *==============================================================================
 */
 
const ST_Region_t	cRegionKindInfo[ 6U ] =
{
	{ ecRegionKind_ErrInfo,	imDataNumErrInf,	imDataByteErrInf,	imIndexMaxErrInf,	imRsvSizeErrInf,	imSctrNoMinErrInf,	imSctrNoMaxErrInf,	imSctrNumErrInf + 1U,	imDataNumInSctrErrInf,	imSctr2ndStAdrErrInf	},
	{ ecRegionKind_MeasAlm,	imDataNumMeasAlm,	imDataByteMeasAlm,	imIndexMaxMeasAlm,	imRsvSizeMeasAlm,	imSctrNoMinMeasAlm,	imSctrNoMaxMeasAlm,	imSctrNumMeasAlm + 1U,	imDataNumInSctrMeasAlm,	imSctr2ndStAdrMeasAlm	},
	{ ecRegionKind_Meas1,	imDataNumMeas1,		imDataByteMeas1,	imIndexMaxMeas1,	imRsvSizeMeas1,		imSctrNoMinMeas1,	imSctrNoMaxMeas1,	imSctrNumMeas1 + 1U,	imDataNumInSctrMeas1, 	imSctr2ndStAdrMeas1		},
	{ ecRegionKind_Event,	imDataNumEvt,		imDataByteEvt,		imIndexMaxEvt,		imRsvSizeEvt,		imSctrNoMinEvt,		imSctrNoMaxEvt,		imSctrNumEvt + 1U,		imDataNumInSctrEvt,		imSctr2ndStAdrEvt		},
	{ ecRegionKind_ActLog,	imDataNumActLog,	imDataByteActLog,	imIndexMaxActLog,	imRsvSizeActLog,	imSctrNoMinActLog,	imSctrNoMaxActLog,	imSctrNumActLog + 1U,	imDataNumInSctrActLog,	imSctr2ndStAdrActLog	},
	{ ecRegionKind_SysLog,	imDataNumSysLog,	imDataByteSysLog,	imIndexMaxSysLog,	imRsvSizeSysLog,	imSctrNoMinSysLog,	imSctrNoMaxSysLog,	imSctrNumSysLog + 1U,	imDataNumInSctrSysLog,	imSctr2ndStAdrSysLog	}
};


/*
 *==============================================================================
 *	プロトタイプ宣言
 *==============================================================================
 */
static uint16_t SubFlash_Retry( uint16_t arRetryMax, uint16_t arRetryNum );
static uint16_t SubFlash_ReadRetry( uint16_t arRetryNum, uint8_t arQueueData[] );
static void SubFlash_AbnSet( ET_FlashKind_t arFlashKind );
static void SubFlash_RetryCntUpDate( uint16_t arRetryMax, ET_Error_t arPrimSts, ET_Error_t arSecondSts );
static void SubFlash_NewestHistTimeSet( uint32_t arTime );
static void SubFlash_EnQueue( uint8_t *parData );								/* エンキュー処理 */
static void SubFlash_DeQueue( uint8_t *parData );								/* デキュー処理 */
static uint8_t SubFlash_GetQueue( uint8_t *parData, uint8_t *parNum );			/* キュー取得処理 */
static ET_RegionKind_t SubFlash_QueDataRegionKindGet( const uint8_t arQueData );
static void SubFlash_GetRegionData( ST_RegionData_t *parRegionData, ET_RegionKind_t arRegionKind );
static ET_Error_t SubFlash_ReadData( uint8_t *parQueData, ET_RegionKind_t arRegionData );
static uint16_t SubFlash_SetReadData( uint8_t arReadData[], ET_RegionKind_t arRegionKind, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex, uint8_t arRfMode );
static uint32_t SubFlash_MakeTime4Byte( const uint8_t *parTimeData );
static void SubFlash_MakeWriteData( const uint8_t *parQueData, uint8_t arWrData[], ET_RegionKind_t arRegionKind );
static uint8_t SubFlash_SctrErsJudge( uint32_t arAddrEnd, uint32_t arAddrFin, ET_RegionKind_t arRegionKind );
static void SubFlash_FinAdrSet( uint32_t arAddrEnd, uint32_t arAddrFin, ET_RegionKind_t arRegionKind );
static ET_Error_t SubFlash_Write( uint32_t arAddr, uint8_t *parWrDataBuff, uint32_t arDataSize);
static ET_Error_t SubFlash_Read( uint32_t arAddr, uint8_t *parRdDataBuff, uint32_t arDataSize );
static void SubFlash_WrEndAddrUpdate( uint32_t arWrFinAddr );
static void SubFlash_WrNumIndexUpdate( uint8_t arSctrChg );
static void SubFlash_RfRTMeasDataSet( void );
static uint32_t SubFlash_ReMakeTime( uint8_t arWrData[] );
static void SubFlash_ReadQueSetPastTime( ET_RegionKind_t arKind );


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	フラッシュ処理 初期化処理
 *
 *	[内容]
 *		フラッシュ処理 初期化処理
 *******************************************************************************
 */
void ApiFlash_Initial( void )
{
	/* データ初期化 */
	memset( &stFlashQue, 0, sizeof(stFlashQue) );
	
	/* 計測値1データ初期化 */
	memset( &stFlashWrMeasVal1, 0, sizeof(stFlashWrMeasVal1) );
	
	/* リアルタイム最新データ初期化 */
	memset( &gvRfRTMeasData, 0, sizeof(gvRfRTMeasData) );
	
	P_FLASH_POWER_IO = (uint8_t)imLow;											/* フラッシュ電源制御ポート：出力 */
	/* CS:Lo */
	ApiFlashDrv_CSCtl( ecFlashKind_Prim, imLow );
	ApiFlashDrv_CSCtl( ecFlashKind_Second, imLow );
	ApiFlash_FlashPowerCtl( imOFF, imOFF );
}


/*
 *******************************************************************************
 *	フラッシュ処理 メイン処理
 *
 *	[内容]
 *		フラッシュ処理 メイン処理
 *******************************************************************************
 */
void ApiFlash_Main( void )
{
	uint8_t			wkLoop;
	uint8_t			wkQueNum;													/* キューデータ数 */
	uint8_t			wkStsPrim;													/* プライマリのステータスレジスタ1 */
	uint8_t			wkStsSecond;												/* セカンダリのステータスレジスタ1 */
	uint8_t			wkSctErsInfo;												/* セクタ消去判定 */
	uint8_t			wkDataInfo;													/* データ情報 */
	uint32_t		wkDataSize;													/* データサイズ数(前半セクタ/後半セクタ書き込み用) */
	uint32_t		wkStartAddr;												/* 開始アドレス */
	ET_RegionKind_t	wkRegionKind;												/* 領域種 */
	ET_Error_t		wkError;													/* エラー情報 */
	ET_Error_t		wkErrorSecond;												/* エラー情報 */
	ET_Error_t		wkPrimReg1Ret;												/* プライマリのエラー情報 */
	ET_Error_t		wkSecReg1Ret;												/* セカンダリのエラー情報 */
	
	static uint8_t	wkQueData[ imFlashQue_DatSize ];							/* キューデータ */
	static uint8_t	vRetryNum = 0U;												/* リトライ回数 */
	static uint16_t	vEraseRetryNum = 0U;										/* イレースリトライ回数 */
	static uint8_t	vWrData[ imFlashRdWr_DatSize ];								/* 書き込みデータ */
	
	switch( stFlashSts )
	{
		/* Flash電源ON */
		case ecFlashSts_PowOn:
			vPrimRetryCnt = 0U;
			vSecondRetryCnt = 0U;
			vRetryNum = 0U;
			vEraseRetryNum = 0U;
			
			/* Flash電源ON */
			ApiFlash_FlashPowerCtl( imON, imOFF );
			R_TAU0_Channel5_Start();
			stFlashSts = ecFlashSts_PowOnWait;
			break;
			
		/* Flash電源安定待ち */
		case ecFlashSts_PowOnWait:
			break;
			
		/* 初期 */
		case ecFlashSts_Init:
#if 0	/* 電気的にOFFしているのでしなくてもよい */
			/* Power-downリリース */
			wkError = ApiFlashDrv_ReleasePowerDown( ecFlashKind_Prim );
			wkError = ApiFlashDrv_ReleasePowerDown( ecFlashKind_Second );
#endif
			/* キュー取得 */
			wkDataInfo = SubFlash_GetQueue( &wkQueData[0], &wkQueNum );
			/* キューがある */
			if( wkDataInfo == imARI )
			{
				/* 領域種取得 */
				wkRegionKind = SubFlash_QueDataRegionKindGet( wkQueData[ imFlashQue_DatSize - 1U ] );
				
				/* 領域種データ取得 */
				SubFlash_GetRegionData( &stRegionData, wkRegionKind );
				
				/* 書き込みの場合 */
				if( M_TSTBIT( wkQueData[imFlashQue_DatSize - 1U], M_BIT0 ) )
				{
					/* 開始アドレス更新 */
					if(( stRegionData.mWrAddrEnd % imFlashSctrSize )== 0U )
					{
						stRegionData.mStartAddr = stRegionData.mWrAddrEnd;
					}
					else
					{
						stRegionData.mStartAddr = stRegionData.mWrAddrEnd + 1U;
					}
					
					/* 書き込みデータ初期化 */
					for( wkLoop = 0U; wkLoop < imFlashRdWr_DatSize; wkLoop++ )
					{
						vWrData[ wkLoop ] = 0U;
					}
					/* 書き込みデータ作成処理 */
					SubFlash_MakeWriteData( wkQueData, &vWrData[0], stRegionData.mRegionKind );
					
					/* セクタ消去判定処理 */
					wkSctErsInfo = SubFlash_SctrErsJudge( stRegionData.mWrAddrEnd, stRegionData.mWrAddrFin, wkRegionKind );
					
					/* セクタ消去無し */
					if( wkSctErsInfo == imNASI )
					{
						/* 書き込み処理実行 */
						wkError = SubFlash_Write( stRegionData.mStartAddr, &vWrData[0], (uint32_t)stRegionData.mDataByteNum );
						/* プライマリ、セカンダリどちらか書き込み成功 */
						if( wkError == ecERR_OK )
						{
							/* 領域種＝計測値1の場合 */
							if( stRegionData.mRegionKind == ecRegionKind_Meas1 )
							{
								/* 先ほど書き込んだデータから時刻情報を取得 */
								gvInFlash.mData.mNewestLogTime = SubFlash_ReMakeTime( vWrData );
							}
							SubFlash_WrNumIndexUpdate( imNASI );					/* 書き込み件数、IndexNo.を領域種毎に更新 */
						}
						else
						{
							/* 次アドレスへの書き込み用に各種更新 */
							stRegionData.mWrAddrEnd = (stRegionData.mStartAddr + (uint32_t)stRegionData.mDataByteNum) - 1U;
							SubFlash_WrEndAddrUpdate( stRegionData.mWrAddrEnd );	/* 書き込み終端アドレスを領域種毎に更新 */
						}
						ApiFlash_FlashPowerCtl( imOFF, imOFF );						/* Flash電源OFF */
						SubFlash_DeQueue( wkQueData );								/* デキュー処理 */
						stFlashSts = ecFlashSts_PowOn;
					}
					/* セクタ消去有り */
					else
					{
						/* 書き込み最終アドレスのセット */
						SubFlash_FinAdrSet( stRegionData.mWrAddrEnd, stRegionData.mWrAddrFin, wkRegionKind );
						
						/* セクタ消去処理 */
						/* プライマリのセクタを消去 */
						wkError = ApiFlashDrv_SectorErase( stRegionData.mWrAddrFin, ecFlashKind_Prim );
						
						/* セカンダリのセクタを消去 */
						wkErrorSecond = ApiFlashDrv_SectorErase( stRegionData.mWrAddrFin, ecFlashKind_Second );
						
						if( (wkError == ecERR_OK) && (wkErrorSecond == ecERR_OK) )
						{
							stFlashSts = ecFlashSts_SctrErase;					/* 状態をセクタ消去中へ */
						}
						else
						{
							SubFlash_RetryCntUpDate( imRetryMax, wkError, wkErrorSecond );
							vRetryNum = SubFlash_Retry( imRetryMax, vRetryNum );
																				/* リトライ回数監視 */
							if( vRetryNum >= imRetryMax )						/* 最大リトライ回数以上 */
							{
								stFlashSts = ecFlashSts_SctrErase;				/* 状態をセクタ消去中へ */
							}
						}
					}
				}
				/* 読み出しの場合 */
				else
				{
					vReadFlash = ecReadFlash_OK;								/* 正常 */
					wkError = SubFlash_ReadData( wkQueData, wkRegionKind );
					
					if( wkError == ecERR_OK )
					{
						/* Flash電源OFF */
						ApiFlash_FlashPowerCtl( imOFF, imOFF );
						SubFlash_DeQueue( wkQueData );							/* デキュー処理 */
						stFlashSts = ecFlashSts_PowOn;
					}
					else
					{
						/* 読み出しエラーの場合のみ、リトライ動作を行う */
						if( vReadFlash == ecReadFlash_ReadErr )
						{
							/* リトライ回数監視 */
							vRetryNum = SubFlash_ReadRetry( vRetryNum, wkQueData );
						}
						else if( vReadFlash == ecReadFlash_IndexNoErr ) 
						{														/* IndexNoエラー */
							M_SETBIT( stFlashReqRd.mFin, M_BIT15 );				/* Flashリード失敗 */
							ApiFlash_FlashPowerCtl( imOFF, imOFF );
							SubFlash_DeQueue( wkQueData );						/* デキュー処理 */
							stFlashSts = ecFlashSts_PowOn;
						}
						/* 入ることがない処理だが念の為、初期化を行う */
						else
						{
							ApiFlash_FlashPowerCtl( imOFF, imOFF );
							SubFlash_DeQueue( wkQueData );						/* デキュー処理 */
							stFlashSts = ecFlashSts_PowOn;
						}
					}
				}
			}
			/* データ無し */
			else
			{
				/* Flash電源OFF */
				ApiFlash_FlashPowerCtl( imOFF, imOFF );
				gvModuleSts.mExFlash = ecExFlashModuleSts_Sleep;
				stFlashSts = ecFlashSts_PowOn;
			}
			break;
			
		/* セクタ消去中 */
		case ecFlashSts_SctrErase:
			/* プライマリのステータスレジスタ1読み出し */
			wkPrimReg1Ret = ApiFlashDrv_ReadStatusReg1( &wkStsPrim, ecFlashKind_Prim );
			
			/* セカンダリのステータスレジスタ1読み出し */
			wkSecReg1Ret = ApiFlashDrv_ReadStatusReg1( &wkStsSecond, ecFlashKind_Second );
			
			/* セクタ消去完了 */
			if( (!M_TSTBIT( wkStsPrim, imFlashDrv_Sts1Bit_WEL )) && (wkPrimReg1Ret == ecERR_OK) &&
				(!M_TSTBIT( wkStsSecond, imFlashDrv_Sts1Bit_WEL )) && (wkSecReg1Ret == ecERR_OK) )
			{
				stFlashSts = ecFlashSts_FirstSctrWr;							/* 状態を前半セクタ書き込み中へ */
			}
			/* セクタ消去未完了 */
			else
			{
				/* プライマリ判定 */
				if( (M_TSTBIT( wkStsPrim, imFlashDrv_Sts1Bit_WEL )) || (wkPrimReg1Ret == ecERR_NG) )
				{
					wkError = ecERR_NG;
				}
				else
				{
					wkError = ecERR_OK;
				}
				
				/* セカンダリ判定 */
				if( (M_TSTBIT( wkStsSecond, imFlashDrv_Sts1Bit_WEL )) || (wkSecReg1Ret == ecERR_NG) )
				{
					wkErrorSecond = ecERR_NG;
				}
				else
				{
					wkErrorSecond = ecERR_OK;
				}
				
				SubFlash_RetryCntUpDate( imEraseRetryMax, wkError, wkErrorSecond );
				vEraseRetryNum = SubFlash_Retry( imEraseRetryMax, vEraseRetryNum );
																				/* セクタリトライ回数監視 */
				if( vEraseRetryNum >= imEraseRetryMax )							/* 最大リトライ回数以上 */
				{
					stFlashSts = ecFlashSts_FirstSctrWr;						/* 状態を前半セクタ書き込み中へ */
				}
			}
			break;
			
		/* 前半セクタ書き込み中 */
		case ecFlashSts_FirstSctrWr:
			wkDataSize = stRegionData.mDataByteNum;
			wkStartAddr = stRegionData.mWrAddrFin;
			
			if( wkDataSize == 0U )
			{
				/* Flash電源OFF */
				ApiFlash_FlashPowerCtl( imOFF, imOFF );
				stFlashSts = ecFlashSts_PowOn;
			}
			else
			{
				/* 書き込み処理実行 */
				wkError = SubFlash_Write( wkStartAddr, &vWrData[0], wkDataSize );
				/* プライマリ、セカンダリどちらか書き込み成功 */
				if( wkError == ecERR_OK )
				{
					/* 領域種＝計測値1の場合 */
					if( stRegionData.mRegionKind == ecRegionKind_Meas1 )
					{
						/* 先ほど書き込んだデータから時刻情報を取得 */
						gvInFlash.mData.mNewestLogTime = SubFlash_ReMakeTime( vWrData );
					}
					SubFlash_WrNumIndexUpdate( imARI );						/* 書き込み件数、IndexNo.を領域種毎に更新 */
				}
				else
				{
					/* 次アドレスへの書き込み用に各種更新 */
					stRegionData.mWrAddrEnd = (wkStartAddr + (uint32_t)stRegionData.mDataByteNum) - 1U;
					SubFlash_WrEndAddrUpdate( stRegionData.mWrAddrEnd );	/* 書き込み終端アドレスを領域種毎に更新 */
				}
				ApiFlash_FlashPowerCtl( imOFF, imOFF );						/* Flash電源OFF */
				SubFlash_DeQueue( wkQueData );								/* デキュー処理 */
				stFlashSts = ecFlashSts_PowOn;
			}
			break;
			
		default:
			break;
	}
}


/*
 *******************************************************************************
 * リトライ回数監視
 *
 *	[内容]
 *		リトライ回数監視
 *	[引数]
 *		uint16_t	arRetryMax: リトライ最大回数
 *		uint16_t	arRetryNum: リトライ回数
 *	[戻値]
 *		uint8_t	arRetryNum: リトライ回数
 *******************************************************************************
 */
static uint16_t SubFlash_Retry( uint16_t arRetryMax, uint16_t arRetryNum )
{
	arRetryNum++;																/* リトライ回数更新 */
	
	/* リトライ回数オーバー */
	if( arRetryNum >= arRetryMax )
	{
		/* 外付けフラッシュエラーセット判定 */
		
		/* プライマリがリトライ */
		if( vPrimRetryCnt == M_UINT16_MAX )
		{
			/* プライマリメモリ異常発生 */
			ApiAbn_AbnStsSet( imAbnSts_PMEM, ecAbnInfKind_AbnSts );
		}
		
		/* セカンダリがリトライ */
		if( vSecondRetryCnt == M_UINT16_MAX )
		{
			/* セカンダリメモリ異常発生 */
			ApiAbn_AbnStsSet( imAbnSts_SMEM, ecAbnInfKind_AbnSts );
		}
	}
	else
	{
		;															/* 無処理 */
	}
	
	return arRetryNum;
}


/*
 *******************************************************************************
 * 読み出し用リトライ回数監視
 *
 *	[内容]
 *		外付けフラッシュへの読み出しリトライ回数監視
 *	[引数]
 *		uint16_t	arRetryNum: リトライ回数
 *	[戻値]
 *		uint8_t	arRetryNum: リトライ回数
 *******************************************************************************
 */
static uint16_t SubFlash_ReadRetry( uint16_t arRetryNum, uint8_t arQueueData[] )
{
	arRetryNum++;																/* リトライ回数更新 */
	
	/* リトライ回数オーバー */
	if( arRetryNum >= imRetryMax )
	{
		M_SETBIT( stFlashReqRd.mFin, M_BIT15 );				/* Flashリード失敗 */
		SubFlash_DeQueue( arQueueData );					/* デキュー処理 */
		ApiFlash_FlashPowerCtl( imOFF, imOFF );
		stFlashSts = ecFlashSts_PowOn;
		arRetryNum = 0U;									/* リトライ回数初期化 */
		
		/* 外付けフラッシュエラーセット判定 */
		
		/* プライマリがリトライ */
		if( vPrimRetryCnt == imRetryMax )
		{
			/* プライマリメモリ異常発生 */
			ApiAbn_AbnStsSet( imAbnSts_PMEM, ecAbnInfKind_AbnSts );
		}
		
		/* セカンダリがリトライ */
		if( vSecondRetryCnt == imRetryMax )
		{
			/* セカンダリメモリ異常発生 */
			ApiAbn_AbnStsSet( imAbnSts_SMEM, ecAbnInfKind_AbnSts );
		}
		
		vPrimRetryCnt = 0U;
		vSecondRetryCnt = 0U;
	}
	else
	{
		;													/* 無処理 */
	}
	
	return arRetryNum;
}


/*
 *******************************************************************************
 * プライマリセカンダリ用リトライ回数更新
 *
 *	[内容]
 *		プライマリセカンダリ用リトライ回数更新
 *	[引数]
 *		uint16_t	arRetryMax: リトライ最大回数
 *		ET_Error_t	arPrimSts: プライマリメモリアクセスのエラー情報
 *		ET_Error_t	arSecondSts: セカンダリメモリアクセスのエラー情報
 *	[戻値]
 *		なし
 *******************************************************************************
 */
static void SubFlash_RetryCntUpDate( uint16_t arRetryMax, ET_Error_t arPrimSts, ET_Error_t arSecondSts )
{
	/* プライマリ判定 */
	if( arPrimSts == ecERR_NG )
	{
		vPrimRetryCnt++;											/* プライマリ用リトライカウント更新 */
		if( vPrimRetryCnt >= arRetryMax )
		{
			vPrimRetryCnt = M_UINT16_MAX;
		}
	}
	else
	{
		vPrimRetryCnt = 0U;											/* リトライ無し状態に移行 */
	}
	
	/* セカンダリ判定 */
	if( arSecondSts == ecERR_NG )
	{
		vSecondRetryCnt++;											/* セカンダリ用リトライカウント更新 */
		if( vSecondRetryCnt >= arRetryMax )
		{
			vSecondRetryCnt = M_UINT16_MAX;
		}
	}
	else
	{
		vSecondRetryCnt = 0U;										/* リトライ無し状態に移行 */
	}
}


/*
 *******************************************************************************
 * フラッシュエラーセット
 *
 *	[内容]
 *		フラッシュエラーセット
 *	[引数]
 *		ET_FlashKind_t	arFlashKind: フラッシュIC
 *	[戻値]
 *		なし
 *******************************************************************************
 */
static void SubFlash_AbnSet( ET_FlashKind_t arFlashKind )
{
	if( arFlashKind == ecFlashKind_Prim )
	{
		/* プライマリメモリ異常発生 */
		ApiAbn_AbnStsSet( imAbnSts_PMEM, ecAbnInfKind_AbnSts );
	}
	else
	{
		/* セカンダリメモリ異常発生 */
		ApiAbn_AbnStsSet( imAbnSts_SMEM, ecAbnInfKind_AbnSts );
	}
}


/*
 *******************************************************************************
 *	異常情報の書き込み処理
 *
 *	[内容]
 *		異常情報の書き込み処理
 *	[引数]
 *		ET_ErrInfItm_t	arItem: 警報項目
 *		uint8_t			arAlmFlg: アラームフラグ
 *******************************************************************************
 */
void ApiFlash_WriteErrInfo( ET_ErrInfItm_t arItem, uint8_t arAlmFlg )
{
#if (swKouteiMode == imDisable)
	uint8_t		wkQueueData[ imFlashQue_DatSize ];								/* キューデータ */
	uint32_t			wkTimeInfo;												/* 時刻情報 */
	
	/* 時刻情報作成(30bit) */
	
	/* 計測関連の機器異常のとき */
	if( arItem >= ecErrInfItm_1chOvrRng && arItem <= ecErrInfItm_4chCalErr )
	{
		if( gvModuleSts.mEventKey == ecEventKeyModuleSts_Meas )
		{
			wkTimeInfo = gvMeasPrm.mTimeEvt;									/* 時刻情報(イベント実行時) */
		}
		else
		{
			wkTimeInfo = gvMeasPrm.mTime;										/* 時刻情報 */
		}
		if( wkTimeInfo == 0U )
		{
			wkTimeInfo = ApiRtcDrv_mktime( gvClock );
		}
	}
	else
	{
		wkTimeInfo = ApiTimeComp_GetLocalTime();
	}
	
	SubFlash_NewestHistTimeSet( wkTimeInfo );									/* 全履歴中の最新時刻を更新 */
	wkTimeInfo <<= 2U;
	wkQueueData[ 0U ] = (uint8_t)((wkTimeInfo >> 24U) & 0x000000FFU);			/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkTimeInfo >> 16U) & 0x000000FFU);			/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkTimeInfo >> 8U) & 0x000000FFU);			/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkTimeInfo & 0x000000FCU);					/* 3:11111100 */
	
	/* 警報項目作成(5bit) */
	wkQueueData[ 3U ] |= ((uint8_t)arItem >> 3U) & 0x03U;						/* 3:00000011 */
	wkQueueData[ 4U ] = ((uint8_t)arItem << 5U) & 0xE0U;						/* 4:11100000 */
	
	/* アラームフラグ作成(1bit) */
	wkQueueData[ 4U ] |= (arAlmFlg << 4U) & 0x10U;								/* 4:00010000 */
	
	/* 領域種 */
	wkQueueData[ 12U ] = ((uint8_t)ecRegionKind_ErrInfo << 1U) & 0x1EU;			/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 12:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* データキューにデータを設定 */
	
	SubFlash_ReadQueSetPastTime( ecRegionKind_ErrInfo );						/* 最古時刻リードのキューをセット */
	
#endif
}


/*
 *******************************************************************************
 *	計測警報の書き込み処理
 *
 *	[内容]
 *		計測警報の書き込み処理
 *	[引数]
 *		uint8_t		arAlmFlg: アラームフラグ
 *		uint8_t		arAlmChannel: チャネル
 *		uint8_t		arLevel: レベル
 *		sint16_t	arMeasVal: 計測値
 *		ET_MeasAlmItm_t	arItem: 警報項目
 *******************************************************************************
 */
void ApiFlash_WriteMeasAlm( uint32_t arTime, uint8_t arAlmFlg, uint8_t arAlmChannel, uint8_t arLevel, sint16_t arMeasVal, ET_MeasAlmItm_t arItem )
{
	uint8_t		wkQueueData[ imFlashQue_DatSize ];								/* キューデータ */
	uint32_t			wkTimeInfo;												/* 時刻情報 */
	
	/* 時刻情報作成(30bit) */
	if( arTime == 0U )
	{
		arTime = ApiRtcDrv_mktime( gvClock );
	}
	SubFlash_NewestHistTimeSet( arTime );										/* 全履歴中の最新時刻を更新 */
	wkTimeInfo = arTime << 2U;
	wkQueueData[ 0U ] = (uint8_t)((wkTimeInfo >> 24U) & 0x000000FFU);			/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkTimeInfo >> 16U) & 0x000000FFU);			/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkTimeInfo >> 8U) & 0x000000FFU);			/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkTimeInfo & 0x000000FCU);					/* 3:11111100 */
	
	/* アラームフラグ作成(1bit) */
	wkQueueData[ 3U ] |= (arAlmFlg << 1U) & 0x02U;								/* 3:00000010 */
	
	/* チャネル作成 */
	wkQueueData[ 3U ] |= (arAlmChannel >> 1U) & 0x01U;							/* 3:00000001 */
	wkQueueData[ 4U ] = (arAlmChannel << 7U) & 0x80U;							/* 4:10000000 */
	
	/* レベル作成 */
	wkQueueData[ 4U ] |= (arLevel << 5U) & 0x60U;								/* 4:01100000 */
	
	/* 計測値作成(13bit) */
	wkQueueData[ 4U ] |= (uint8_t)(arMeasVal >> 8U) & 0x1FU;					/* 4:00011111 */
	wkQueueData[ 5U ] = (uint8_t)(arMeasVal & 0x00FF);							/* 5:11111111 */
	
	/* 警報項目作成(5bit) */
	wkQueueData[ 6U ] = ((uint8_t)arItem << 3U) & 0xF8U;						/* 6:11111000 */
	
	/* 論理(1bit) */
	wkQueueData[ 6U ] |= (gvInFlash.mParam.mEventKeyFlg & 0x01) << 2U;			/* 6:00000100 */
	
	/* 領域種 */
	wkQueueData[ 12U ] = ((uint8_t)ecRegionKind_MeasAlm << 1U) & 0x1EU;			/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 13:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* データキューにデータを設定 */
	
	SubFlash_ReadQueSetPastTime( ecRegionKind_MeasAlm );						/* 最古時刻リードのキューをセット */
}


/*
 *******************************************************************************
 *	計測値1/イベントの書き込み処理
 *
 *	[内容]
 *		計測値1、イベントの書き込み処理
 *	[引数]
 *		uint8_t		arSel: 0：計測値1、1：イベント
 *	[戻り値]
 *		uint32_t	wkTime:書込み時刻
 *******************************************************************************
 */
void ApiFlash_WriteMeasVal1Event( uint8_t arSel )
{
	uint8_t				wkLoop;
	uint8_t				wkQueueData[ imFlashQue_DatSize ];						/* キューデータ */
	
	uint16_t			wkMeasVal;
	uint32_t			wkTimeInfo;
	
	ST_FlashVal1Evt_t	wkWtData;
	ET_RegionKind_t		wkKind;
	
	for( wkLoop=0U ; wkLoop<imFlashQue_ChannelNum ; wkLoop++ )
	{
		wkWtData.mMeasVal[ wkLoop ] = gvMeasPrm.mMeasVal[ wkLoop ];				/* 計測値 */
		wkWtData.mDevFlg[ wkLoop ] = gvMeasPrm.mDevFlg[ wkLoop ];				/* 逸脱フラグ */
		wkWtData.mAlmFlg[ wkLoop ] = gvMeasPrm.mAlmFlg[ wkLoop ];				/* 警報フラグ */
	}
	wkWtData.mLogic = gvMeasPrm.mLogic;											/* 論理 */
	wkWtData.m4chDevAlmFlg = gvMeasPrm.mDevFlg[ 3U ];							/* 4ch逸脱警報フラグ */
	wkWtData.m4chDevAlmFlg |= gvMeasPrm.mAlmFlg[ 3U ] << 4U;
	
	/* 計測値1は静的変数に格納 */
	if( arSel == 0U )
	{
		wkWtData.mTimeInfo = gvMeasPrm.mTime;									/* 時刻情報 */
		stFlashWrMeasVal1 = wkWtData;
		wkKind = ecRegionKind_Meas1;
	}
	/* イベント */
	else
	{
		wkWtData.mTimeInfo = gvMeasPrm.mTimeEvt;								/* 時刻情報(イベント実行時) */
		SubFlash_NewestHistTimeSet( wkWtData.mTimeInfo );						/* 全履歴中の最新時刻を更新 */
		wkKind = ecRegionKind_Event;
	}
	
	
	/* 時刻情報(30bit) */
	wkTimeInfo = wkWtData.mTimeInfo << 2U;
	wkQueueData[ 0U ] = (uint8_t)((wkTimeInfo >> 24U) & 0x000000FFU);			/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkTimeInfo >> 16U) & 0x000000FFU);			/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkTimeInfo >> 8U) & 0x000000FFU);			/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkTimeInfo & 0x000000FCU);					/* 3:11111100 */
	
	/* 1ch計測値1(13bit) */
	wkMeasVal = (wkWtData.mMeasVal[0] >> 11U) & 0x0003U;
	wkQueueData[ 3U ] |= (uint8_t)wkMeasVal;									/* 3:00000011 */
	wkMeasVal = (wkWtData.mMeasVal[0] >> 3U) & 0x00FFU;
	wkQueueData[ 4U ] = (uint8_t)wkMeasVal;										/* 4:11111111 */
	wkMeasVal = (wkWtData.mMeasVal[0] & 0x0007U) << 5U;
	wkQueueData[ 5U ] = (uint8_t)wkMeasVal;										/* 5:11100000 */
	
	/* 論理(1bit) */
	wkQueueData[ 5U ] |= (wkWtData.mLogic << 4U) & 0x10U;						/* 5:00010000 */
	
	/* 1ch逸脱フラグ(4bit) */
	wkQueueData[ 5U ] |= wkWtData.mDevFlg[0] & 0x0FU;							/* 5:00001111 */
	
	/* 1ch警報フラグ(4bit) */
	wkQueueData[ 6U ] = (wkWtData.mAlmFlg[0] << 4U) & 0xF0U;					/* 6:11110000 */
	
	/* 2ch計測値1(13bit) */
	wkMeasVal = (wkWtData.mMeasVal[1] >> 9U) & 0x000FU;
	wkQueueData[ 6U ] |= (uint8_t)wkMeasVal;									/* 6:00001111 */
	wkMeasVal = (wkWtData.mMeasVal[1] >> 1U) & 0x00FFU;
	wkQueueData[ 7U ] = (uint8_t)wkMeasVal;										/* 7:11111111 */
	wkMeasVal = (wkWtData.mMeasVal[1] & 0x0001U) << 7U;
	wkQueueData[ 8U ] = (uint8_t)wkMeasVal;										/* 8:10000000 */
	
	/* 4ch逸脱フラグ(1bit) */
	wkQueueData[ 8U ] |= (wkWtData.m4chDevAlmFlg & 0x01U) << 6U;				/* 8:01000000 */
	
	/* 2ch逸脱フラグ(4bit) */
	wkQueueData[ 8U ] |= (wkWtData.mDevFlg[1] << 2U) & 0x3CU;					/* 8:00111100 */
	
	/* 2ch警報フラグ(4bit) */
	wkQueueData[ 8U ] |= (wkWtData.mAlmFlg[1] >> 2U) & 0x03U;					/* 8:00000011 */
	wkQueueData[ 9U ] = (wkWtData.mAlmFlg[1] & 0x03U) << 6U;					/* 9:11000000 */
	
	/* 3ch計測値1(13bit) */
	wkMeasVal = (wkWtData.mMeasVal[2] >> 7U) & 0x003FU;
	wkQueueData[ 9U ] |= (uint8_t)wkMeasVal;									/* 9:00111111 */
	wkMeasVal = (wkWtData.mMeasVal[2] << 1U) & 0x00FEU;
	wkQueueData[ 10U ] = (uint8_t)wkMeasVal;									/* 10:11111110 */
	
	/* 4ch警報フラグ(1bit) */
	wkQueueData[ 10U ] |= wkWtData.m4chDevAlmFlg & 0x01U;						/* 10:00000001 */
	
	/* 3ch逸脱フラグ(4bit) */
	wkQueueData[ 11U ] = (wkWtData.mDevFlg[2] << 4U) & 0xF0U;					/* 11:11110000 */
	
	/* 3ch警報フラグ(4bit) */
	wkQueueData[ 11U ] |= wkWtData.mAlmFlg[2] & 0x0FU;							/* 11:00001111 */
	
	/* 領域種 */
	wkQueueData[ 12U ] = (wkKind << 1U) & 0x1EU;								/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 13:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* データキューにデータを設定 */
	
	SubFlash_ReadQueSetPastTime( wkKind );										/* 最古時刻リードのキューをセット */
}


/*
 *******************************************************************************
 *	動作履歴の書き込み処理
 *
 *	[内容]
 *		動作履歴の書き込み処理
 *******************************************************************************
 */
void ApiFlash_WriteActLog( ET_ActLogItm_t arItem, uint32_t arData, uint16_t arUserId )
{
#if (swKouteiMode == imDisable)
	uint8_t		wkQueueData[ imFlashQue_DatSize ];						/* キューデータ */
	uint32_t			wkTimeInfo;
	uint32_t			wkData;
	
	/* 時刻情報(30bit) */
	wkTimeInfo = ApiTimeComp_GetLocalTime();
	
	SubFlash_NewestHistTimeSet( wkTimeInfo );									/* 全履歴中の最新時刻を更新 */
	wkTimeInfo <<= 2U;
	wkQueueData[ 0U ] = (uint8_t)((wkTimeInfo >> 24U) & 0x000000FFU);			/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkTimeInfo >> 16U) & 0x000000FFU);			/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkTimeInfo >> 8U) & 0x000000FFU);			/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkTimeInfo & 0x000000FCU);					/* 3:11111100 */
	
	/* 履歴項目(10bit) */
	wkQueueData[ 3U ] |= ((uint8_t)arItem >> 8U) & 0x03U;						/* 3:00000011 */
	wkQueueData[ 4U ] = arItem & 0xFFU;											/* 4:11111111*/
	
	/* データ(24bit) */
	wkData = arData;
	wkQueueData[ 5U ] = (uint8_t)((wkData >> 16U) & 0x000000FFU);				/* 5:11111111 */
	wkQueueData[ 6U ] = (uint8_t)((wkData >> 8U) & 0x000000FFU);				/* 6:11111111 */
	wkQueueData[ 7U ] = (uint8_t)(wkData & 0x000000FFU);						/* 7:11111111 */
	
	/* 変更者ID */
	wkQueueData[ 8U ] = arUserId;												/* 8:11111111 */
	
	/* 領域種 */
	wkQueueData[ 12U ] = ((uint8_t)ecRegionKind_ActLog << 1U) & 0x1EU;			/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 12:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* データキューにデータを設定 */
	
	SubFlash_ReadQueSetPastTime( ecRegionKind_ActLog );							/* 最古時刻リードのキューをセット */
#endif
}


/*
 *******************************************************************************
 *	システムログの書き込み処理
 *
 *	[内容]
 *		システムログの書き込み処理
 *******************************************************************************
 */
void ApiFlash_WriteSysLog( ET_SysLogItm_t arItem, uint32_t arData )
{
#if (swKouteiMode == imDisable)
	uint8_t		wkQueueData[ imFlashQue_DatSize ];								/* キューデータ */
	uint32_t			wkTimeInfo;
	uint32_t			wkData;
	
	/* 時刻情報(30bit) */
	wkTimeInfo = ApiTimeComp_GetLocalTime();
	
	SubFlash_NewestHistTimeSet( wkTimeInfo );									/* 全履歴中の最新時刻を更新 */
	wkTimeInfo <<= 2U;
	wkQueueData[ 0U ] = (uint8_t)((wkTimeInfo >> 24U) & 0x000000FFU);			/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkTimeInfo >> 16U) & 0x000000FFU);			/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkTimeInfo >> 8U) & 0x000000FFU);			/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkTimeInfo & 0x000000FCU);					/* 3:11111100 */
	
	/* システムログ項目(4bit) */
	wkQueueData[ 3U ] |= ((uint8_t)arItem >> 2U) & 0x03U;						/* 3:00000011 */
	wkQueueData[ 4U ] = ((uint8_t)arItem << 6U) & 0xC0U;						/* 4:11000000 */
	
	/* データ(25bit入れるが無線送信が最下位から24bitなので最上位bitは無視される) */
	wkData = (arData << 5U) & 0x3FFFFFFFU;
	wkQueueData[ 4U ] |= (uint8_t)((wkData >> 24U) & 0x000000FFU);				/* 4:00111111 */
	wkQueueData[ 5U ] = (uint8_t)((wkData >> 16U) & 0x000000FFU);				/* 5:11111111 */
	wkQueueData[ 6U ] = (uint8_t)((wkData >> 8U) & 0x000000FFU);				/* 6:11111111 */
	wkQueueData[ 7U ] = (uint8_t)(wkData & 0x000000E0U);						/* 7:11100000 */
	
	/* 領域種 */
	wkQueueData[ 12U ] = ((uint8_t)ecRegionKind_SysLog << 1U) & 0x1EU;			/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 12:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* データキューにデータを設定 */
	
	SubFlash_ReadQueSetPastTime( ecRegionKind_SysLog );							/* 最古時刻リードのキューをセット */
#endif
}

/*
 *******************************************************************************
 *	全履歴中の最新時刻を更新
 *
 *	[内容]
 *		全履歴中の最新時刻を更新
 *	[引数]
 *		uint32_t arTime: ログの時刻
 *******************************************************************************
 */
static void SubFlash_NewestHistTimeSet( uint32_t arTime )
{
	if( (gvInFlash.mData.mNewestHistTime == 0xFFFFFFFF) ||
		(arTime > gvInFlash.mData.mNewestHistTime) )
	{
		gvInFlash.mData.mNewestHistTime = arTime;
	}
}

/*
 *******************************************************************************
 *	計測値1、システムログのアドレステーブルの書き込み処理
 *
 *	[内容]
 *		計測値1、システムログのアドレステーブルのの書き込み処理
 *******************************************************************************
 */
void ApiFlash_WriteFlasAdrTbl( uint32_t arEndadr, uint16_t arNum, uint16_t arIndex, uint8_t arKind )
{
	uint8_t			wkQueueData[ imFlashQue_DatSize ];							/* キューデータ */
	uint32_t		wkEndAdr;
	
	/* 終端アドレス(32bit) */
	wkEndAdr = arEndadr;
	wkQueueData[ 0U ] = (uint8_t)((wkEndAdr >> 24U) & 0x000000FFU);				/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkEndAdr >> 16U) & 0x000000FFU);				/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkEndAdr >> 8U) & 0x000000FFU);				/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkEndAdr & 0x000000FFU);						/* 3:11111111 */
	
	/* データ数(16bit) */
	wkQueueData[ 4U ] = (uint8_t)((arNum >> 8U) & 0x00FFU);						/* 4:11111111 */
	wkQueueData[ 5U ] = (uint8_t)(arNum & 0x00FFU);								/* 5:11111111 */
	
	/* IndexNo.(16bit) */
	wkQueueData[ 6U ] = (uint8_t)((arIndex >> 8U) & 0x00FFU);					/* 6:11111111 */
	wkQueueData[ 7U ] = (uint8_t)(arIndex & 0x00FFU);							/* 7:11111111 */
	
	/* 領域種 */
	wkQueueData[ 12U ] = ((uint8_t)arKind << 1U) & 0x1EU;						/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 12:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* データキューにデータを設定 */
}


/*
 *******************************************************************************
 *	Flashリードのキューセット
 *
 *	[引数]
 *		uint32_t		arStartTime		:リード開始時刻
 *		uint32_t		arEndTime		:リード終了時刻
 *		uint16_t		arIndexNum		:リード開始IndexNo.
 *		uint8_t			arKosu			:リード個数
 *		ET_RegionKind_t	arRegionKind	:リード種類
 *		uint8_t			arReadStas		:0:高速通信、1:長距離通信、2:最古時刻
 *	[内容]
 *		Flashからリードするキューをセットする
 *******************************************************************************
 */
ET_Error_t ApiFlash_ReadQueSet( uint32_t arStartTime, uint32_t arEndTime, uint16_t arIndexNum, uint8_t arKosu, ET_RegionKind_t arRegionKind, uint8_t arReadStas )
{
	uint8_t			wkQueueData[ imFlashQue_DatSize ];	/* キューデータ */
	uint32_t		wkStartTime;						/* 開始時刻 */
	uint32_t		wkEndTime;							/* 終了時刻 */
	ET_Error_t		wkRet = ecERR_OK;					/* 戻り値(エラー情報) */
	
	if( stFlashQue.mNum < imFlashQue_DatNum )
	{
		
		/* 開始時刻作成(30bit) */
		wkStartTime = arStartTime << 2U;											/* 最上位ビットまでシフト後、時刻情報格納 */
		wkQueueData[ 0U ] = (uint8_t)((wkStartTime & 0xFF000000U) >> 24U);			/* 0xFF000000＝11111111000000000000000000000000 */
		wkQueueData[ 1U ] = (uint8_t)((wkStartTime & 0x00FF0000U) >> 16U);			/* 0x00FF0000＝00000000111111110000000000000000 */
		wkQueueData[ 2U ] = (uint8_t)((wkStartTime & 0x0000FF00U) >> 8U);			/* 0x0000FF00＝00000000000000001111111100000000 */
		wkQueueData[ 3U ] = (uint8_t)(wkStartTime & 0x000000FCU);					/* 0x000000FC＝00000000000000000000000011111100 */
		
		/* リードステータス */
		wkQueueData[ 3U ] |= (uint8_t)(arReadStas & 0x03U);							/* 0x03＝00000011 */
		
		/* 終了時刻作成(30bit) */
		wkEndTime = arEndTime << 2U;												/* 最上位ビットまでシフト後、時刻情報格納 */
		wkQueueData[ 4U ] = (uint8_t)((wkEndTime & 0xFF000000U) >> 24U);			/* 0xFF000000＝11111111000000000000000000000000 */
		wkQueueData[ 5U ] = (uint8_t)((wkEndTime & 0x00FF0000U) >> 16U);			/* 0x00FF0000＝00000000111111110000000000000000 */
		wkQueueData[ 6U ] = (uint8_t)((wkEndTime & 0x0000FF00U) >> 8U);				/* 0x0000FF00＝00000000000000001111111100000000 */
		wkQueueData[ 7U ] = (uint8_t)(wkEndTime & 0x000000FCU);						/* 0x000000FC＝00000000000000000000000011111100 */
		
		/* インデックスNo.(16bit) */
		wkQueueData[ 8U ] = (uint8_t)((arIndexNum & 0xFF00U) >> 8U);				/* 0xFF00＝1111111100000000 */
		wkQueueData[ 9U ] = (uint8_t)(arIndexNum & 0x00FFU);						/* 0x00FF＝0000000011111111 */
		
		/* 個数(8bit) */
		wkQueueData[ 10U ] = arKosu;												/* 0xFF＝11111111 */
		
		/* 領域種作成(4bit) */
		wkQueueData[ 12U ] = ((uint8_t)arRegionKind << 1U) & 0x1EU;					/* 0x1E＝00011110 */
		
		/* Read情報作成(1bit) */
		wkQueueData[ 12U ] &= 0xFEU;												/* Read情報格納 */
		
		SubFlash_EnQueue( wkQueueData );											/* データキューにデータを設定 */
	}
	else
	{
		wkRet = ecERR_NG;
	}
	return wkRet;
}



/*
 *******************************************************************************
 *	エンキュー処理
 *
 *	[内容]
 *		データキューに指定したデータを設定する。
 *		なお、格納個数が最大値以上の時は、無処理とする。
 *	[注記]
 *		指定するデータの格納領域はimFlashQue_DatSize以上とすること。
 *******************************************************************************
 */
static void SubFlash_EnQueue( uint8_t *parData )
{
	uint8_t		wkIndex;														/* インデックス */
	uint8_t		wkLoop;															/* ループカウンタ */
	uint8_t		*pwkData;														/* 設定データ位置 */
	
	if( parData != M_NULL )														/* アドレスが正常 */
	{
		if( stFlashQue.mNum < imFlashQue_DatNum )							/* 格納個数が最大値未満 */
		{
			wkIndex = stFlashQue.mHead;
			wkIndex += stFlashQue.mNum;
			wkIndex %= imFlashQue_DatNum;
			
			/* データ格納 */
			pwkData = parData;
			for( wkLoop = 0U ; wkLoop < imFlashQue_DatSize ; wkLoop++ )
			{
				stFlashQue.mData[wkIndex][wkLoop] = *pwkData;
				pwkData++;
			}
			
			stFlashQue.mNum++;												/* 格納数 + 1 */
		}
		else
		{
			/*
			 ***************************************
			 *	格納個数が最大値を超えた場合
			 *	バッファへの格納をしない。
			 ***************************************
			 */
			M_NOP;															/* 無処理 */
		}
	}
}


/*
 *******************************************************************************
 *	デキュー処理
 *
 *	[内容]
 *		データキューからデータを取得する。
 *		取得方法は、FIFO方式で取得する。
 *		データキューが空の場合、無処理とする。
 *	[注記]
 *		指定するデータの格納領域はimFlashQue_DatSize以上とすること。
 *******************************************************************************
 */
static void SubFlash_DeQueue( uint8_t *parData )
{
	uint8_t		wkIndex;														/* インデックス */
	
	if( parData != M_NULL )													/* アドレス正常 */
	{
		if( stFlashQue.mNum > 0U )											/* 通常時用キューのチェック */
		{																	/* データ有り */
			wkIndex = stFlashQue.mHead + 1U;
			wkIndex %= imFlashQue_DatNum;
			stFlashQue.mHead = wkIndex;										/* 格納位置の先頭を更新 */
			stFlashQue.mNum--;												/* 格納数 - 1 */
			
		}
		else
		{
			M_NOP;															/* NG */
		}
	}
}


/*
 *******************************************************************************
 *	キュー取得処理
 *
 *	[内容]
 *		データキューの格納状態を取得する。
 *******************************************************************************
 */
static uint8_t SubFlash_GetQueue( uint8_t *parData, uint8_t *parNum )
{
	uint8_t		wkRet;															/* 戻り値(エラー情報) */
	uint8_t		wkLoop;															/* ループカウンタ */
	
	if( (parData != M_NULL) && (parNum != M_NULL) )
	{
		/* データ有り */
		if( stFlashQue.mNum > 0U )											/* 通常のキューのチェック */
		{																	/* データ有り */
			for( wkLoop = 0U ; wkLoop < imFlashQue_DatSize ; wkLoop++ )
			{
				(*parData) = stFlashQue.mData[ stFlashQue.mHead ][ wkLoop ];
				parData++;
			}
			(*parNum) = stFlashQue.mNum;
			wkRet = imARI;													/* 戻り値：データ有り */
		}
		else
		{																	/* どちらのキューも空だった場合 */
			(*parData) = (uint8_t)M_NULL;
			(*parNum) = stFlashQue.mNum;
			wkRet = imNASI;													/* 戻り値：データ無し */
		}
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	キューデータ領域種取得
 *
 *	[内容]
 *		キューデータ内の領域種を取得する
 *******************************************************************************
 */
static ET_RegionKind_t SubFlash_QueDataRegionKindGet( const uint8_t arQueData )
{
	ET_RegionKind_t	wkRet;
	uint8_t			wkData;
	
	wkData = (arQueData >> 1U) & 0x0FU;
	if( wkData < (uint8_t)ecRegionKindMax )
	{
		wkRet = (ET_RegionKind_t)wkData;
	}
	else
	{
		wkRet = ecRegionKindMax;
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	領域種データ取得
 *
 *	[内容]
 *		領域種データの取得を行う。
 *******************************************************************************
 */
static void SubFlash_GetRegionData( ST_RegionData_t *parRegionData, ET_RegionKind_t arRegionKind )
{
	if( parRegionData != M_NULL )
	{
		parRegionData->mRegionKind = arRegionKind;								/* 領域種 */
		
		switch( arRegionKind )
		{
			/* 異常情報 */
			case ecRegionKind_ErrInfo:
				parRegionData->mWrNum = gvInFlash.mData.mErrInf_Num;					/* 書き込み件数 */
				parRegionData->mDataByteNum = imDataByteErrInf;							/* データバイト数 */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mErrInf_AddrEnd;			/* 書き込み終端アドレス */
				parRegionData->mWrAddrFin = gvInFlash.mData.mErrInf_AddrEnd + imDataByteErrInf;	/* 書き込み最終アドレス */
				parRegionData->mIndexNo = gvInFlash.mData.mErrInf_Index;				/* IndexNo. */
				break;
				
			/* 計測警報 */
			case ecRegionKind_MeasAlm:
				parRegionData->mWrNum = gvInFlash.mData.mMeasAlm_Num;					/* 書き込み件数 */
				parRegionData->mDataByteNum = imDataByteMeasAlm;						/* データバイト数 */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mMeasAlm_AddrEnd;			/* 書き込み終端アドレス */
				parRegionData->mWrAddrFin = gvInFlash.mData.mMeasAlm_AddrEnd + imDataByteMeasAlm;	/* 書き込み最終アドレス */
				parRegionData->mIndexNo = gvInFlash.mData.mMeasAlm_Index;				/* IndexNo. */
				break;
				
			/* 計測値1 */
			case ecRegionKind_Meas1:
				parRegionData->mWrNum = gvInFlash.mData.mMeas1_Num;						/* 書き込み件数 */
				parRegionData->mDataByteNum = imDataByteMeas1;							/* データバイト数 */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mMeas1_AddrEnd;				/* 書き込み終端アドレス */
				parRegionData->mWrAddrFin = gvInFlash.mData.mMeas1_AddrEnd + imDataByteMeas1;		/* 書き込み最終アドレス */
				parRegionData->mIndexNo = gvInFlash.mData.mMeas1_Index;					/* IndexNo. */
				break;
				
			/* イベント */
			case ecRegionKind_Event:
				parRegionData->mWrNum = gvInFlash.mData.mEvt_Num;						/* 書き込み件数 */
				parRegionData->mDataByteNum = imDataByteEvt;							/* データバイト数 */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mEvt_AddrEnd;				/* 書き込み終端アドレス */
				parRegionData->mWrAddrFin = gvInFlash.mData.mEvt_AddrEnd + imDataByteEvt;			/* 書き込み最終アドレス */
				parRegionData->mIndexNo = gvInFlash.mData.mEvt_Index;					/* IndexNo. */
				break;
				
			/* 動作履歴 */
			case ecRegionKind_ActLog:
				parRegionData->mWrNum = gvInFlash.mData.mActLog_Num;					/* 書き込み件数 */
				parRegionData->mDataByteNum = imDataByteActLog;							/* データバイト数 */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mActLog_AddrEnd;			/* 書き込み終端アドレス */
				parRegionData->mWrAddrFin = gvInFlash.mData.mActLog_AddrEnd + imDataByteActLog;	/* 書き込み最終アドレス */
				parRegionData->mIndexNo = gvInFlash.mData.mActLog_Index;				/* IndexNo. */
				break;
				
			/* システムログ */
			case ecRegionKind_SysLog:
				parRegionData->mWrNum = gvInFlash.mData.mSysLog_Num;					/* 書き込み件数 */
				parRegionData->mDataByteNum = imDataByteSysLog;							/* データバイト数 */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mSysLog_AddrEnd;			/* 書き込み終端アドレス */
				parRegionData->mWrAddrFin = gvInFlash.mData.mSysLog_AddrEnd + imDataByteSysLog;	/* 書き込み最終アドレス */
				parRegionData->mIndexNo = gvInFlash.mData.mSysLog_Index;				/* IndexNo. */
				break;
			
			/* 計測値1アドレス */
			case ecRegionKind_Meas1Adr:
				parRegionData->mWrNum = gvInFlash.mData.mMeasVal1Adr_Num;				/* 書き込み件数 */
				parRegionData->mDataByteNum = imDataByteAdrTbl;							/* データバイト数 */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mMeasVal1Adr_AddrEnd;		/* 書き込み終端アドレス */
				parRegionData->mWrAddrFin = gvInFlash.mData.mMeasVal1Adr_AddrEnd + imDataByteAdrTbl;	/* 書き込み最終アドレス */
				parRegionData->mIndexNo = 0U;											/* IndexNo. */
				break;
				
			/* システムログアドレス */
			case ecRegionKind_SysLogAdr:
				parRegionData->mWrNum = gvInFlash.mData.mSysLogAdr_Num;					/* 書き込み件数 */
				parRegionData->mDataByteNum = imDataByteAdrTbl;							/* データバイト数 */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mSysLogAdr_AddrEnd;			/* 書き込み終端アドレス */
				parRegionData->mWrAddrFin = gvInFlash.mData.mSysLogAdr_AddrEnd + imDataByteAdrTbl;	/* 書き込み最終アドレス */
				parRegionData->mIndexNo = 0U;											/* IndexNo. */
				break;
				
			default:
				break;
		}
	}
}


/*
 *******************************************************************************
 *	リードキューの解析処理
 *
 *	[引数]
 *		uint8_t			*parQueData		:キューデータ
 *		ET_RegionKind_t	arRegionKind	:リード種類
 *	[内容]
 *		リードキューのIndexNo.からデータをリードする
 *******************************************************************************
 */
static ET_Error_t SubFlash_ReadData( uint8_t *parQueData, ET_RegionKind_t arRegionKind )
{
	uint8_t			wkU16;
	uint8_t			wkLoop;
	uint32_t		wkU32;
	
	uint32_t		wkQueStartTime;			/* 要求開始時刻 */
	uint16_t		wkQueIndex;				/* 検索IndexNo. */
	uint8_t			wkQueRdStatus;			/* 要求通信モード 0:高速通信 1:長距離通信 */
	uint8_t			wkKosu;					/* リード個数 */
	uint32_t		wkRetAdr;				/* 検索IndexNo.のアドレス */
	
	ST_Region_t		wkRegionInfo;			/* 領域種毎の情報 */
	
	uint32_t		wkRngIndexMin;			/* 検索可能範囲Min値 */
	uint32_t		wkRngIndexMax;			/* 検索可能範囲Max値 */
	
	uint32_t		wkNowSctr;				/* 現在IndexNo.相対セクタ */
	uint32_t		wkSrchSctr;				/* 検索IndexNo.相対セクタ */
	
	uint32_t		wkSize1;				/* Index間サイズ計算用 */
	sint32_t		wkSize2;				/* Index間サイズ計算用 */
	
	uint32_t		wkIndexSctrNum;			/* Index間セクタ数計算用 */
	
	sint32_t		wkStartAreaAdr;			/* メモリ先頭アドレス */
	
	sint32_t		wkCal;					/* 検索IndexNo.計算用 */
	
	uint8_t			wkRdData[ imFlashRdWr_DatSize ];		/* Readデータ */
	
	ET_Error_t		wkError = ecERR_NG;
	
	for( wkLoop = 0U ; wkLoop < M_ArrayElement( cRegionKindInfo ) ; wkLoop++ )
	{
		if( arRegionKind == cRegionKindInfo[ wkLoop ].mRegionKind )
		{
			wkRegionInfo = cRegionKindInfo[ wkLoop ];
			break;
		}
	}
	
	/* キューの開始時刻、IndexNo.、個数、無線モードをリード */
	wkQueStartTime = (uint32_t)parQueData[ 0U ] << 24U;
	wkQueStartTime |= (uint32_t)parQueData[ 1U ] << 16U;
	wkQueStartTime |= (uint32_t)parQueData[ 2U ] << 8U;
	wkQueStartTime |= (uint32_t)parQueData[ 3U ];
	wkQueStartTime >>= 2U;
	
	wkQueIndex = (uint16_t)parQueData[ 8U ] << 8U;
	wkQueIndex |= parQueData[ 9U ];
	
	wkKosu = parQueData[ 10U ];
	
	wkQueRdStatus = parQueData[ 3U ] & 0x03U;
	
	/* データがないとき */
	if( stRegionData.mIndexNo == 0U || stRegionData.mWrNum == 0U )
	{
		wkKosu = 1U;
		wkQueIndex = 1U;
	}
	else
	{
		/* 範囲外のIndexNo. */
		if( wkQueIndex > wkRegionInfo.mIndexMax )
		{
			wkQueIndex = 1;
		}
		
		/* リード個数制限 */
		if( stRegionData.mIndexNo >= wkQueIndex )
		{
			wkU32 = wkQueIndex + wkKosu - 1U;
			if( wkU32 > stRegionData.mIndexNo )
			{
				wkKosu = stRegionData.mIndexNo + 1U - wkQueIndex;
			}
		}
		else
		{
			wkU32 = wkRegionInfo.mIndexMax + 1U - wkQueIndex;
			wkU32 += stRegionData.mIndexNo;
			
			if( wkKosu > wkU32 )
			{
				wkKosu = wkU32;
			}
		}
		
		/*** 検索可能範囲の確認 ***/
		/* 現在の収録データ数が領域種の収録最大値未満 */
		if( stRegionData.mWrNum < wkRegionInfo.mDataNumMax )
		{
			/* 1 〜 現在のIndexNo. */
			wkRngIndexMin = 1U;
			wkRngIndexMax = stRegionData.mIndexNo;
			
			if( wkQueIndex < wkRngIndexMin )
			{
				wkQueIndex = 1U;
				wkKosu = 1U;
			}
			else if( wkQueIndex > wkRngIndexMax )
			{
				wkQueIndex = wkRngIndexMax;
				wkKosu = 1U;
			}
		}
		/* 収録データ数が領域種の収録最大値 */
		else
		{
			/* 現在IndexNo. >= 検索IndexNo. */
			if( stRegionData.mIndexNo >= wkQueIndex )
			{
				if( stRegionData.mIndexNo <= wkRegionInfo.mDataNumMax )
				{
					wkRngIndexMin = 1U;
				}
				else
				{
					/* 現在IndexNo. + 1 - データMAX値 〜 現在IndexNo. */
					wkRngIndexMin = (uint32_t)stRegionData.mIndexNo + 1U - (uint32_t)wkRegionInfo.mDataNumMax;
				}
				
				wkRngIndexMax = (uint32_t)stRegionData.mIndexNo;
				
				if( wkQueIndex < wkRngIndexMin )
				{
					wkQueIndex = wkRngIndexMin;
				}
				else if( wkQueIndex > wkRngIndexMax )
				{
					wkQueIndex = wkRngIndexMax + 1U - wkKosu;
				}
			}
			else
			{
				/* 1 〜 現在IndexNo. */
				wkRngIndexMin = 1U;
				wkRngIndexMax = stRegionData.mIndexNo;
				if( (wkQueIndex < wkRngIndexMin) || (wkQueIndex > wkRngIndexMax) )
				{
					/* ((IndexNo.Max値 + 1) - データMax値) + 現在IndexNo.  〜 IndexNo.Max値 */
					wkRngIndexMin = (uint32_t)wkRegionInfo.mIndexMax + 1U - (uint32_t)wkRegionInfo.mDataNumMax;
					wkRngIndexMin += (uint32_t)stRegionData.mIndexNo;
					wkRngIndexMax = (uint32_t)wkRegionInfo.mIndexMax;
					
					if( (uint32_t)wkQueIndex < wkRngIndexMin )
					{
						wkQueIndex = wkRngIndexMin;
					}
					else if( (uint32_t)wkQueIndex > wkRngIndexMax )
					{
						wkQueIndex = wkRngIndexMax + 1U - wkKosu;
					}
				}
			}
		}
	}
	
	
	/* 現在のIndexNo.相対セクタ計算: (現在IndexNo. - 1) / データMAX値 */
	wkNowSctr = stRegionData.mIndexNo - 1U;
	wkNowSctr /= wkRegionInfo.mDataNumInSctr;
	
	for( wkLoop = 0U ; wkLoop < wkKosu ; wkLoop++, wkQueIndex++ )
	{
		/* キューのIndexNo.が領域種の最大値超過 */
		if( wkQueIndex > wkRegionInfo.mIndexMax )
		{
			wkQueIndex = 1U;
		}
		
		/* 検索のIndexNo.相対セクタ計算: (検索IndexNo. - 1) / データMAX値 */
		wkSrchSctr = wkQueIndex - 1U;
		wkSrchSctr /= wkRegionInfo.mDataNumInSctr;
		
		
		/*** 現在IndexNo.と検索IndexNo.間のサイズ演算 ***/
		
		/* 現在IndexNo. >= 検索IndexNo. */
		if( stRegionData.mIndexNo >= wkQueIndex )
		{
			/* (現在IndexNo. + 1 - 検索IndexNo.) * size */
			wkSize1 = (stRegionData.mIndexNo + 1U) - wkQueIndex;
		}
		else
		{
			/* (IndexNo.最大数 + 1 - 検索IndexNo. + 現在IndexNo.) * size */
			wkSize1 = (wkRegionInfo.mIndexMax + 1U) - wkQueIndex;
			wkSize1 += stRegionData.mIndexNo;
		}
		wkSize1 *= wkRegionInfo.mDataByte;
		
		
		/*** アドレス演算 ***/
		switch( arRegionKind )
		{
			case ecRegionKind_ErrInfo:
			case ecRegionKind_MeasAlm:
			case ecRegionKind_Event:
			case ecRegionKind_ActLog:
				
				/*** 検索IndexNo.のアドレス演算 ***/
				if( stRegionData.mIndexNo == 0U || stRegionData.mWrNum == 0U )
				{
					wkRetAdr = stRegionData.mWrAddrEnd;
				}
				else
				{
				/* 検索IndexNo.は同じセクタ内 */
				if( wkNowSctr == wkSrchSctr )
				{
					/* 検索アドレス = 現在の終端アドレス + 1 - サイズ */
					wkRetAdr = (stRegionData.mWrAddrEnd + 1U) - wkSize1;
				}
				else
				{
					/* 現在の終端アドレスと現在の先頭アドレス間のサイズ */
						wkSize2 = stRegionData.mWrAddrEnd / imFlashSctrSize;
						wkSize2 *= -1;
						wkSize2 *= imFlashSctrSize;
						wkSize2 += stRegionData.mWrAddrEnd + 1;
					
					/* 現在セクタが第1エリア */
					if( stRegionData.mWrAddrEnd < wkRegionInfo.m2ndStAdr )
					{
						/* 第2セクタの最終アドレス */
						wkRetAdr = (wkRegionInfo.mSctrMax * imFlashSctrSize) + imFlashSctrSize;
					}
					else
					{
						/* 前セクタの最終アドレス */
						wkRetAdr = (wkRegionInfo.mSctrMin * imFlashSctrSize) + imFlashSctrSize;
					}
					/* 前セクタの最終アドレス + 現在セクタで使用しているサイズ - Index間のサイズ - 未使用サイズ */
					wkRetAdr = wkRetAdr + wkSize2 - wkSize1 - wkRegionInfo.mRsvSize;
				}
				}
				break;
				
			case ecRegionKind_Meas1:
			case ecRegionKind_SysLog:
				
				if( stRegionData.mIndexNo == 0U || stRegionData.mWrNum == 0U )
				{
					wkRetAdr = stRegionData.mWrAddrEnd;
				}
				else
				{
				/*** 現在Indexセクタと検索Indexセクタ間の数 */
				
				/* 同じセクタ */
				if( wkNowSctr == wkSrchSctr )
				{
					/* 現在IndexNo. < 検索IndexNo. */
					if( stRegionData.mIndexNo < wkQueIndex )
					{
						/* 1周しているので、現在のセクタから後ろに1周させ、現在のセクタの次のセクタにある */
						wkIndexSctrNum = wkRegionInfo.mSctrNum - 1U;
					}
					else
					{
						wkIndexSctrNum = 0U;
					}
				}
				/* 現在セクタ > 検索セクタ */
				else if( wkNowSctr > wkSrchSctr )
				{
					/* 現在セクタ - 検索セクタ */
					wkIndexSctrNum = wkNowSctr - wkSrchSctr;
				}
				/* 現在セクタ < 検索セクタ */
				else
				{
					/* セクタ最大数  - (検索セクタ + 1U - 現在セクタ) */
					wkIndexSctrNum = wkRegionInfo.mSctrNum - ( wkSrchSctr + 1U - wkNowSctr );
				}
				/* 使用セクタ数 * 未使用サイズ */
				wkIndexSctrNum *= wkRegionInfo.mRsvSize;
				
				/*** 検索IndexNo.のアドレス演算 ***/
				
				/* 終端アドレス + 1 - セクタ間サイズ - 未使用領域サイズ */
				wkCal = stRegionData.mWrAddrEnd + 1U - wkSize1;
				wkCal -= wkIndexSctrNum;
				
				wkStartAreaAdr = wkRegionInfo.mSctrMin * imFlashSctrSize;
				/* 使用セクタ開始アドレス以上 */
				if( wkCal >= wkStartAreaAdr )
				{
					wkRetAdr = (uint32_t)wkCal;
				}
				/* 使用セクタ開始アドレス未満、0以上 */
				else if( wkCal >= 0U )
				{
					wkRetAdr = (wkRegionInfo.mSctrMax + 1U) * imFlashSctrSize;
					wkRetAdr -= (wkStartAreaAdr - wkCal);
				}
				/* 0未満 */
				else
				{
					wkCal *= -1;
					wkRetAdr = (wkRegionInfo.mSctrMax + 1U) * imFlashSctrSize;
					wkRetAdr -= (wkStartAreaAdr + wkCal);
				}
				}
				break;
				
			default:
				return wkError;
		}
		
		/*** データリード ***/
		wkError = SubFlash_Read( wkRetAdr, &wkRdData[ 0U ], wkRegionInfo.mDataByte );
		if( wkError == ecERR_OK )
		{
			/* 高速通信、かつ開始時刻を指定 */
			if( wkQueRdStatus == 0U && wkQueStartTime != 0U  )
			{
				/* リードした時刻と比較し不一致、又はリード個数が1以下 */
				if( wkQueStartTime != SubFlash_MakeTime4Byte( wkRdData ) || wkKosu <= 1U )
				{
					/* 全データリードのキュー作成 */
					ApiRfDrv_MakeReadQue( arRegionKind, 0U, 0U );
					break;
				}
				else
				{
					wkQueIndex ++;
					if( wkQueIndex > wkRegionInfo.mIndexMax )
					{
						wkQueIndex = 1U;
					}
					ApiRfDrv_MakeReadQue( arRegionKind, 0U, wkQueIndex );
					break;
				}
			}
			/* 最古時刻 */
			else if( wkQueRdStatus == 2U )
			{
				wkU16 = SubFlash_SetReadData( wkRdData, arRegionKind, wkLoop, wkKosu, wkQueIndex, wkQueRdStatus );
			}
			/* 長距離通信と高速通信の時刻指定なし */
			else
			{
				wkU16 = SubFlash_SetReadData( wkRdData, arRegionKind, wkLoop, wkKosu, wkQueIndex, wkQueRdStatus );
			if( wkU16 != 0U )
			{
				M_SETBIT( stFlashReqRd.mFin, wkU16 );							/* Flashリード完了 */
			}
		}
		}
		else
		{
			vReadFlash = ecReadFlash_ReadErr;									/* 読み出しエラー */
			break;
		}
	}
	
	return wkError;
}


/*
 *******************************************************************************
 *	読み出しデータ格納
 *
 *	[引数]
 *		uint8_t			arReadData[]	:リードデータ
 *		ET_RegionKind_t	arRegionKind	:リード種類
 *		uint8_t			arLoop			:リード順番号
 *		uint8_t			arKosu			:リード個数
 *		uint16_t		arIndex			:リード先頭IndexNo.
 *		uint8_t			arRdStatus		:0:高速通信、1:長距離通信、2:最古時刻のためのデータリード
 *	[戻り値]
 *		uint8_t			wkRet			:Flashリード完了した種類を示すフラグ
 *	[内容]
 *		プライマリから読み出したデータを格納する。
 *******************************************************************************
 */
static uint16_t SubFlash_SetReadData( uint8_t arReadData[], ET_RegionKind_t arRegionKind, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex, uint8_t arRdStatus )
{
	uint16_t	wkRet = 0;					/* 戻り値 */
	uint32_t	wkTimeInfo;					/* 時刻情報 */
	ST_FlashVal1Evt_t	wkRdMeas;			/* 計測値 */
	ST_FlashMeasAlm_t	wkRdMeasAlm;		/* 計測警報 */
	ST_FlashErrInfo_t	wkRdErrInfo;		/* 機器異常警報 */
	ST_FlashActLog_t	wkRdActLog;			/* 動作履歴 */
	ST_FlashSysLog_t	wkRdSysLog;			/* システムログ */
	
	/* 時刻情報の作成 */
	wkTimeInfo = SubFlash_MakeTime4Byte(arReadData);
	
	if( arRdStatus == 2U && wkTimeInfo == 0x3FFFFFFFU )
	{
		arIndex = 0U;
		wkTimeInfo = 0U;
	}
	
	switch( arRegionKind )
	{
		/* 異常情報 */
		case ecRegionKind_ErrInfo:
			
			/* 最古情報セット */
			if( arRdStatus == 2U )
			{
				gvInFlash.mData.mErrInf_PastIndex = arIndex;							/* 最古IndexNo. */
				gvInFlash.mData.mErrInf_PastTime = wkTimeInfo;							/* 最古時刻 */
			}
			else
			{
				wkRdErrInfo.mTimeInfo = wkTimeInfo;									/* 時刻情報 */
				
				wkRdErrInfo.mItem = (ET_ErrInfItm_t)((arReadData[3] & 0x03U) << 3U);	/* 警報項目 */
				wkRdErrInfo.mItem |= (arReadData[4] & 0xE0U) >> 5U;
				
				wkRdErrInfo.mAlmFlg = (arReadData[4] & 0x10U) >> 4U;					/* アラームフラグ */
				
				/* リードした異常履歴を無線バッファに格納 */
				ApiRfDrv_MeasErr_StorageRfBuff( &wkRdErrInfo, arLoop, arKosu, arIndex );
				/* リード個数分を格納完了 */
				if( arLoop == arKosu - 1U )
				{
					wkRet = M_BIT0;
				}
			}
			break;
			
		/* 計測警報 */
		case ecRegionKind_MeasAlm:
			
			/* 最古情報セット */
			if( arRdStatus == 2U )
			{
				gvInFlash.mData.mMeasAlm_PastIndex = arIndex;							/* 最古IndexNo. */
				gvInFlash.mData.mMeasAlm_PastTime = wkTimeInfo;							/* 最古時刻 */
			}
			else
			{
				wkRdMeasAlm.mTimeInfo = wkTimeInfo;									/* 時刻情報 */
			
				wkRdMeasAlm.mAlmFlg = (arReadData[3] & 0x02U) >> 1U;					/* アラームフラグ */
				
				wkRdMeasAlm.mAlmChannel = (arReadData[3] & 0x01U) << 1U;				/* チャネル */
				wkRdMeasAlm.mAlmChannel |= (arReadData[4] & 0x80U) >> 7U;
				
				wkRdMeasAlm.mLevel = (arReadData[4] & 0x60U) >> 5U;						/* レベル */
				
				wkRdMeasAlm.mMeasVal = ((uint16_t)arReadData[4] & 0x001FU) << 8U;		/* 計測値 */
				wkRdMeasAlm.mMeasVal |= ((uint16_t)arReadData[5] & 0x00FFU);
				
				wkRdMeasAlm.mItem = (ET_MeasAlmItm_t)((arReadData[6] & 0xF8U) >> 3U);	/* 警報項目 */
				
				wkRdMeasAlm.mEvent = (arReadData[6] & 0x04U) >> 2U;						/* 論理 */
				
				/* 高速通信時、リードした計測値を無線バッファに格納 */
				if( arRdStatus == 0U )
				{
					/* リードした警報履歴を無線バッファに格納 */
					ApiRfDrv_MeasAlm_StorageRfBuff( &wkRdMeasAlm, arLoop, arKosu, arIndex );
					/* リード個数分を格納完了 */
					if( arLoop == arKosu - 1U )
					{
						wkRet = M_BIT1;
					}
				}
				/* リアルタイム通信時、リードした計測警報履歴を専用変数に格納 */
				else
				{
					stFlashRdMeasAlmVal[ arLoop ] = wkRdMeasAlm;
					wkRet = M_BIT1;
				}
			}
			break;
			
		/* 計測値1 */
		/* イベント */
		case ecRegionKind_Meas1:
		case ecRegionKind_Event:
			
			/* 最古情報セット */
			if( arRdStatus == 2U )
			{
				if( arRegionKind == ecRegionKind_Meas1 )
				{
					gvInFlash.mData.mMeas1_PastIndex = arIndex;								/* 最古IndexNo. */
					gvInFlash.mData.mMeas1_PastTime = wkTimeInfo;							/* 最古時刻 */
				}
				else
				{
					gvInFlash.mData.mEvt_PastIndex = arIndex;								/* 最古IndexNo. */
					gvInFlash.mData.mEvt_PastTime = wkTimeInfo;								/* 最古時刻 */
				}
			}
			else
			{
				wkRdMeas.mTimeInfo = wkTimeInfo;											/* 時刻情報 */
				
				wkRdMeas.mMeasVal[ 0U ] = ((uint16_t)arReadData[ 3U ] & 0x0003U) << 11U;	/* 1ch計測値 */
				wkRdMeas.mMeasVal[ 0U ] |= ((uint16_t)arReadData[ 4U ] & 0x00FFU) << 3U;
				wkRdMeas.mMeasVal[ 0U ] |= ((uint16_t)arReadData[ 5U ] & 0x00E0U) >> 5U;
				
				wkRdMeas.mLogic = (arReadData[ 5U ] & 0x10U) >> 4U;							/* 論理 */
				
				wkRdMeas.mDevFlg[ 0U ] = arReadData[ 5U ] & 0x0FU;							/* 1ch逸脱フラグ */
				
				wkRdMeas.mAlmFlg[ 0U ] = (arReadData[ 6U ] & 0xF0U) >> 4U;					/* 1ch警報フラグ */
				
				wkRdMeas.mMeasVal[ 1U ] = ((uint16_t)arReadData[ 6U ] & 0x000FU) << 9U;		/* 2ch計測値 */
				wkRdMeas.mMeasVal[ 1U ] |= ((uint16_t)arReadData[ 7U ] & 0x00FFU) << 1U;
				wkRdMeas.mMeasVal[ 1U ] |= ((uint16_t)arReadData[ 8U ] & 0x0080U) >> 7U;
				
				wkRdMeas.m4chDevAlmFlg = (arReadData[ 8U ] & 0x40) >> 6U;					/* 4ch逸脱フラグ */
				
				wkRdMeas.mDevFlg[ 1U ] = (arReadData[ 8U ] & 0x3CU) >> 2U;					/* 2ch逸脱フラグ */
				
				wkRdMeas.mAlmFlg[ 1U ] = (arReadData[ 8U ] & 0x03U) << 2U;					/* 2ch警報フラグ */
				wkRdMeas.mAlmFlg[ 1U ] |= (arReadData[ 9U ] & 0xC0U) >> 6U;
				
				wkRdMeas.mMeasVal[ 2U ] = ((uint16_t)arReadData[ 9U ] & 0x003FU) << 7U;		/* 3ch計測値 */
				wkRdMeas.mMeasVal[ 2U ] |= ((uint16_t)arReadData[ 10U ] & 0x00FE) >> 1U;
				
				wkRdMeas.m4chDevAlmFlg = (arReadData[ 10U ] & 0x01U) << 4U;					/* 4ch警報フラグ */
				
				wkRdMeas.mDevFlg[ 2U ] = (arReadData[ 11U ]) & 0xF0 >> 4U;					/* 3ch逸脱フラグ */
				
				wkRdMeas.mAlmFlg[ 2U ] = arReadData[ 11U ] & 0x0FU;							/* 3ch警報フラグ */
				
				if( arRegionKind == ecRegionKind_Meas1 )
				{
					/* 高速通信時、リードした計測値を無線バッファに格納 */
					if( arRdStatus == 0U )
					{
						ApiRfDrv_MeasData_StorageRfBuff( &wkRdMeas, arLoop, arKosu, arIndex );
						/* リード個数分を格納完了 */
						if( arLoop == arKosu - 1U )
						{
							wkRet = M_BIT2;
						}
					}
					/* リアルタイム通信時、リードした計測値を専用変数に格納 */
					else
					{
						stFlashRdMeasVal1[ arLoop ] = wkRdMeas;
						wkRet = M_BIT2;
					}
				}
				else
				{
					ApiRfDrv_MeasEvt_StorageRfBuff( &wkRdMeas, arLoop, arKosu, arIndex );
					/* リード個数分を格納完了 */
					if( arLoop == arKosu - 1U )
					{
						wkRet = M_BIT4;
					}
				}
			}
			break;
			
		/* 動作履歴 */
		case ecRegionKind_ActLog:
			
			/* 最古情報セット */
			if( arRdStatus == 2U )
			{
				gvInFlash.mData.mActLog_PastIndex = arIndex;						/* 最古IndexNo. */
				gvInFlash.mData.mActLog_PastTime = wkTimeInfo;						/* 最古時刻 */
			}
			else
			{
				wkRdActLog.mTimeInfo = wkTimeInfo;									/* 時刻情報 */
				
				wkRdActLog.mItem = (ET_ActLogItm_t)((arReadData[3] & 0x03U) << 8U);	/* 履歴項目 */
				wkRdActLog.mItem |= arReadData[4];
				
				wkRdActLog.mData = (uint32_t)arReadData[5] << 16U;					/* データ */
				wkRdActLog.mData |= (uint32_t)arReadData[6] << 8U;
				wkRdActLog.mData |= (uint32_t)arReadData[7];
				
				wkRdActLog.mUserId = arReadData[8];									/* 変更者ID */
				
				/* リードした動作履歴を無線バッファに格納 */
				ApiRfDrv_ActLog_StorageRfBuff( &wkRdActLog, arLoop, arKosu, arIndex );
				/* リード個数分を格納完了 */
				if( arLoop == arKosu - 1U )
				{
					wkRet = M_BIT5;
				}
			}
			break;
			
		/* システムログ */
		case ecRegionKind_SysLog:
			
			/* 最古情報セット */
			if( arRdStatus == 2U )
			{
				gvInFlash.mData.mSysLog_PastIndex = arIndex;						/* 最古IndexNo. */
				gvInFlash.mData.mSysLog_PastTime = wkTimeInfo;						/* 最古時刻 */
			}
			else
			{
				wkRdSysLog.mTimeInfo = wkTimeInfo;									/* 時刻情報 */
				
				wkRdSysLog.mItem = (ET_SysLogItm_t)((arReadData[3] & 0x03U) << 2U);	/* 履歴項目 */
				wkRdSysLog.mItem |= (arReadData[4] & 0xE0U) >> 6U;
				
				wkRdSysLog.mData = ((uint32_t)arReadData[4] & 0x1FU) << 19U;		/* データ */
				wkRdSysLog.mData |= ((uint32_t)arReadData[5] & 0xFFU) << 11U;
				wkRdSysLog.mData |= ((uint32_t)arReadData[6] & 0xFFU) << 3U;
				wkRdSysLog.mData |= ((uint32_t)arReadData[7] & 0xE0U) >> 5U;
				
				/* リードしたシステム履歴を無線バッファに格納 */
				ApiRfDrv_SysLog_StorageRfBuff( &wkRdSysLog, arLoop, arKosu, arIndex );
				/* リード個数分を格納完了 */
				if( arLoop == arKosu - 1U )
				{
					wkRet = M_BIT6;
				}
			}
			break;
			
		default:
			break;
	}
	return wkRet;
}


/*
 *******************************************************************************
 *	1byte→4byteデータ変換処理
 *
 *	[内容]
 *		1byte毎に持つ時間データを4byteデータに変換する。
 *******************************************************************************
 */
static uint32_t SubFlash_MakeTime4Byte( const uint8_t *parTimeData )
{
	uint32_t	wkRet;
	
	wkRet = (uint32_t)parTimeData[ 0U ] << 24U;
	wkRet |= (uint32_t)parTimeData[ 1U ] << 16U;
	wkRet |= (uint32_t)parTimeData[ 2U ] << 8U;
	wkRet |= (uint32_t)parTimeData[ 3U ];
	wkRet >>= 2U;
	
	return wkRet;
}


/*
 *******************************************************************************
 *	書き込みデータ作成処理
 *
 *	[内容]
 *		データキュー内のデータを書き込み用バッファに置き換える。
 *******************************************************************************
 */
static void SubFlash_MakeWriteData( const uint8_t *parQueData, uint8_t arWrData[], ET_RegionKind_t arRegionKind )
{
	uint8_t		wkLoop;
	uint8_t		wkDataByte;
	
	/* 書き込みデータバッファ内初期化 */
	for( wkLoop = 0U ; wkLoop < M_ArrayElement(arWrData) ; wkLoop++ )
	{
		arWrData[wkLoop] = 0U;
	}
	
	switch( arRegionKind )
	{
		/* 異常情報 */
		case ecRegionKind_ErrInfo:
			wkDataByte = imDataByteErrInf;
			break;
			
		/* 計測警報 */
		case ecRegionKind_MeasAlm:
			wkDataByte = imDataByteMeasAlm;
			break;
			
		/* 計測値1 */
		case ecRegionKind_Meas1:
			wkDataByte = imDataByteMeas1;
			break;
			
		/* イベント */
		case ecRegionKind_Event:
			wkDataByte = imDataByteEvt;
			break;
			
		/* 動作履歴 */
		case ecRegionKind_ActLog:
			wkDataByte = imDataByteActLog;
			break;
			
		/* システムログ */
		case ecRegionKind_SysLog:
			wkDataByte = imDataByteSysLog;
			break;
			
		/* 計測値1、システムログアドレス */
		case ecRegionKind_Meas1Adr:
		case ecRegionKind_SysLogAdr:
			wkDataByte = imDataByteAdrTbl;
			break;
			
		default:
			return;
	}
	
	for( wkLoop = 0U; wkLoop < wkDataByte; wkLoop++ )
	{
		arWrData[wkLoop] = parQueData[wkLoop];
	}
}


/*
 *******************************************************************************
 *	セクタ消去判定処理
 *
 *	[内容]
 *		セクタ消去判定を行う。
 *		戻り値：0＝セクタ消去無し。 1＝セクタ消去有り。
 *******************************************************************************
 */
static uint8_t SubFlash_SctrErsJudge( uint32_t arAddrEnd, uint32_t arAddrFin, ET_RegionKind_t arRegionKind )
{
	uint8_t		wkRet = imNASI;
	uint32_t	wkWrEndSctrNum;					/* 書き込み終端セクタ番号 */
	uint32_t	wkWrFinSctrNum;					/* 書き込み最終セクタ番号 */
	uint32_t	wkSctrSize;						/* セクタサイズ */
	uint32_t	wkStartSctrAddr;
	uint32_t	wkSctrNum;
	
	switch( arRegionKind )
	{
		/* 異常情報 */
		case ecRegionKind_ErrInfo:
			wkSctrSize = imFlashErrInfoSctrSize;
			break;
			
		/* 計測警報 */
		case ecRegionKind_MeasAlm:
			wkSctrSize = imFlashMeasAlmSctrSize;
			break;
			
		/* 計測値1 */
		case ecRegionKind_Meas1:
			wkSctrSize = imFlashMeas1SctrSize;
			break;
			
		/* イベント */
		case ecRegionKind_Event:
			wkSctrSize = imFlashEvtSctrSize;
			break;
			
		/* 動作履歴 */
		case ecRegionKind_ActLog:
			wkSctrSize = imFlashActLogSctrSize;
			break;
			
		/* システムログ */
		case ecRegionKind_SysLog:
			wkSctrSize = imFlashSysLogSctrSize;
			break;
			
		/* 計測値1、システムログアドレス */
		case ecRegionKind_Meas1Adr:
		case ecRegionKind_SysLogAdr:
			wkSctrSize = imFlashAdrTblSctrSize;
			break;
			
		default:
			break;
	}
	wkSctrNum = arAddrEnd / imFlashSctrSize;
	wkStartSctrAddr = wkSctrNum * imFlashSctrSize;
	
	wkWrEndSctrNum = (arAddrEnd - wkStartSctrAddr) / wkSctrSize;		/* 書き込み終端セクタ番号 */
	wkWrFinSctrNum = (arAddrFin - wkStartSctrAddr) / wkSctrSize;		/* 書き込み最終セクタ番号 */
	
	if( wkWrEndSctrNum != wkWrFinSctrNum )
	{
		wkRet = imARI;													/* セクタ消去有り */
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	書込み最終アドレスセット
 *
 *	[内容]
 *		データ種類でセクタ間をまたぐものかを判定し、
 *		書込み最終アドレスの値をセットする
 *******************************************************************************
 */
static void SubFlash_FinAdrSet( uint32_t arAddrEnd, uint32_t arAddrFin, ET_RegionKind_t arRegionKind )
{
	uint32_t	wkNextAreaAddrHead;
	uint32_t	wkAreaAddrEnd;
	uint32_t	wkSctrNum;
	
	wkSctrNum = arAddrEnd / imFlashSctrSize;
	
	switch( arRegionKind )
	{
		/* 異常情報 */
		case ecRegionKind_ErrInfo:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrErrInf;
			if( wkSctrNum == imSctrNoMaxErrInf )
			{
				wkNextAreaAddrHead = imSctrNoMinErrInf * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* 計測警報 */
		case ecRegionKind_MeasAlm:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrMeasAlm;
			if( wkSctrNum == imSctrNoMaxMeasAlm )
			{
				wkNextAreaAddrHead = imSctrNoMinMeasAlm * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* 計測値1 */
		case ecRegionKind_Meas1:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrMeas1;
			if( wkSctrNum == imSctrNoMaxMeas1 )
			{
				wkNextAreaAddrHead = imSctrNoMinMeas1 * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* イベント */
		case ecRegionKind_Event:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrEvt;
			if( wkSctrNum == imSctrNoMaxEvt )
			{
				wkNextAreaAddrHead = imSctrNoMinEvt * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* 動作履歴 */
		case ecRegionKind_ActLog:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrActLog;
			if( wkSctrNum == imSctrNoMaxActLog )
			{
				wkNextAreaAddrHead = imSctrNoMinActLog * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* システムログ */
		case ecRegionKind_SysLog:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrSysLog;
			if( wkSctrNum == imSctrNoMaxSysLog )
			{
				wkNextAreaAddrHead = imSctrNoMinSysLog * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* 計測値1アドレス */
		case ecRegionKind_Meas1Adr:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrAdrTbl;
			if( wkSctrNum == imSctrNoMaxMeas1Adr )
			{
				wkNextAreaAddrHead = imSctrNoMinMeas1Adr * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* システムログアドレス */
		case ecRegionKind_SysLogAdr:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrAdrTbl;
			if( wkSctrNum == imSctrNoMaxSysLogAdr )
			{
				wkNextAreaAddrHead = imSctrNoMinSysLogAdr * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		default:
			return;
	}
	
	/* 書き込み最終アドレスがエリア終端アドレス以上の場合 */
	if( stRegionData.mWrAddrFin > wkAreaAddrEnd )
	{
		stRegionData.mWrAddrFin = wkNextAreaAddrHead;
	}
}


/*
 *******************************************************************************
 *	書き込み処理
 *
 *	[内容]
 *		書き込み処理を行う。
 *******************************************************************************
 */
static ET_Error_t SubFlash_Write( uint32_t arAddr, uint8_t *parWrDataBuff, uint32_t arDataSize )
{
	ET_Error_t		wkFlashRet[ 2U ] = {ecERR_OK, ecERR_OK};
	ET_Error_t		wkRet = ecERR_NG;
	uint8_t			wkRdData[ imFlashRdWr_DatSize ];
	int16_t			wkCmpRet;
	uint8_t			wkRetryCnt;
	
	
	if( arDataSize != 0U )
	{
		/* プライマリ書き込み実行 */
		for( wkRetryCnt = 0U ; wkRetryCnt < imRetryMax ; wkRetryCnt++ )
		{
			wkFlashRet[ ecFlashKind_Prim ] = ApiFlashDrv_WriteMemory( arAddr, &parWrDataBuff[0], (uint16_t)arDataSize, (ET_FlashKind_t)ecFlashKind_Prim );
			if( wkFlashRet[ ecFlashKind_Prim ] == ecERR_OK )
			{
				wkFlashRet[ ecFlashKind_Prim ] = ApiFlashDrv_ReadData( arAddr, &wkRdData[0], (uint16_t)arDataSize, (ET_FlashKind_t)ecFlashKind_Prim );
				if( wkFlashRet[ ecFlashKind_Prim ] == ecERR_OK )
				{
					wkCmpRet = memcmp( &parWrDataBuff[0], &wkRdData[0], arDataSize );
					if( wkCmpRet == 0U )
					{															/* 書き込み正常終了 */
						wkFlashRet[ ecFlashKind_Prim ] = ecERR_OK;				/* OK */
						break;
					}
					else
					{
						wkFlashRet[ ecFlashKind_Prim ] = ecERR_NG;				/* NG */
					}
				}
			}
		}
		if( wkFlashRet[ ecFlashKind_Prim ] != ecERR_OK )
		{
			SubFlash_AbnSet( ecFlashKind_Prim );
		}
		
		/* セカンダリ書き込み実行 */
		for( wkRetryCnt = 0U ; wkRetryCnt < imRetryMax ; wkRetryCnt++ )
		{
			wkFlashRet[ ecFlashKind_Second ] = ApiFlashDrv_WriteMemory( arAddr, &parWrDataBuff[0], (uint16_t)arDataSize, (ET_FlashKind_t)ecFlashKind_Second );
			if( wkFlashRet[ ecFlashKind_Second ] == ecERR_OK )
			{
				wkFlashRet[ ecFlashKind_Second ] = ApiFlashDrv_ReadData( arAddr, &wkRdData[0], (uint16_t)arDataSize, (ET_FlashKind_t)ecFlashKind_Second );
				if( wkFlashRet[ ecFlashKind_Second ] == ecERR_OK )
				{
					wkCmpRet = memcmp( &parWrDataBuff[0], &wkRdData[0], arDataSize );
					/* 書き込み正常終了 */
					if( wkCmpRet == 0U )
					{
						wkFlashRet[ ecFlashKind_Second ] = ecERR_OK;			/* OK */
						break;
					}
					else
					{
						wkFlashRet[ ecFlashKind_Second ] = ecERR_NG;			/* NG */
					}
				}
			}
		}
		if( wkFlashRet[ ecFlashKind_Second ] != ecERR_OK )
		{
			SubFlash_AbnSet( ecFlashKind_Second );
		}
		
		/* プライマリ、セカンダリどちらか成功 */
		if(( wkFlashRet[ ecFlashKind_Prim ] == ecERR_OK )						/* プライマリ書き込み成功 */
		|| ( wkFlashRet[ ecFlashKind_Second ] == ecERR_OK ))					/* セカンダリ書き込み成功 */
		{
			stRegionData.mWrAddrEnd = (arAddr + arDataSize) - 1U;				/* 書き込み最終アドレス更新 */
			SubFlash_WrEndAddrUpdate( stRegionData.mWrAddrEnd );				/* 書き込み終端アドレスを領域種毎に更新 */
			wkRet = ecERR_OK;													/* 戻り値：OK */
		}
		
#if 0	/* 機器異常復帰させない */
		/* プライマリ書き込み成功 */
		if( wkFlashRet[ ecFlashKind_Prim ] == ecERR_OK )
		{
			/* プライマリメモリ異常復帰 */
			ApiAbn_AbnStsClr( imAbnSts_PMEM, ecAbnInfKind_AbnSts );
		}
		
		/* セカンダリ書き込み成功 */
		if( wkFlashRet[ ecFlashKind_Second ] == ecERR_OK )
		{
			/* セカンダリメモリ異常復帰 */
			ApiAbn_AbnStsClr( imAbnSts_SMEM, ecAbnInfKind_AbnSts );
		}
#endif
	}
	else
	{
		wkRet = ecERR_OK;
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	読込処理
 *
 *	[内容]
 *		選択Flashから値を読み込む
 *	[引数]
 *		uint32_t	arAddr: アドレス
 *		uint8_t		*parRdDataBuff: リードバッファ
 *		uint32_t	arDataSize: リードサイズ
 *	[戻値]
 *		ET_Error_t	:エラー情報
 *******************************************************************************
 */
static ET_Error_t SubFlash_Read( uint32_t arAddr, uint8_t *parRdDataBuff, uint32_t arDataSize )
{
	ET_Error_t		wkRet = ecERR_NG;
	ET_FlashKind_t	wkSel;
#if 0	/* 機器異常復帰させない */
	uint16_t		wkAbnSts;
#endif
	
	if( arDataSize != 0U )
	{
#if 0	/* 機器異常復帰させない */
		if( gvInFlash.mParam.mFlashSelect == (uint8_t)ecFlashKind_Prim )
		{
			wkSel = ecFlashKind_Prim;
			wkAbnSts = imAbnSts_PMEM;
		}
		else
		{
			wkSel = ecFlashKind_Second;
			wkAbnSts = imAbnSts_SMEM;
		}
#else
		if( gvInFlash.mParam.mFlashSelect == (uint8_t)ecFlashKind_Prim )
		{
			wkSel = ecFlashKind_Prim;
		}
		else
		{
			wkSel = ecFlashKind_Second;
		}
#endif
		wkRet = ApiFlashDrv_ReadData( arAddr, &parRdDataBuff[0], (uint16_t)arDataSize, wkSel );
		
#if 0	/* 機器異常復帰させない */
		/* フラッシュエラー処理 */
		if( wkRet == ecERR_OK )
		{
			/* プライマリorセカンダリメモリ異常復帰 */
			ApiAbn_AbnStsClr( wkAbnSts, ecAbnInfKind_AbnSts );
			}
		else
		{
			/* セカンダリ用のリトライ回数更新 */
			if( wkSel == ecFlashKind_Prim )
			{																	/* プライマリ選択時 */
				SubFlash_RetryCntUpDate( imRetryMax, wkRet, ecERR_OK );			/* リトライカウンタ更新 */
		}
		else
			{																	/* セカンダリ選択時 */
				SubFlash_RetryCntUpDate( imRetryMax, ecERR_OK, wkRet );			/* リトライカウンタ更新 */
			}
		}
#else
		/* フラッシュエラー処理 */
		if( wkRet != ecERR_OK )
		{
			/* セカンダリ用のリトライ回数更新 */
			if( wkSel == ecFlashKind_Prim )
			{																	/* プライマリ選択時 */
				SubFlash_RetryCntUpDate( imRetryMax, wkRet, ecERR_OK );			/* リトライカウンタ更新 */
			}
			else
			{																	/* セカンダリ選択時 */
				SubFlash_RetryCntUpDate( imRetryMax, ecERR_OK, wkRet );			/* リトライカウンタ更新 */
			}
		}
#endif
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	書き込み件数、IndexNo.更新
 *
 *	[内容]
 *		書き込み件数、IndexNo.の更新処理を行う。
 *	[引数]
 *		uint8_t		arSctrChg: セクタ変更有無
 *	[戻値]
 *		なし
 *******************************************************************************
 */
static void SubFlash_WrNumIndexUpdate( uint8_t arSctrChg )
{
	uint16_t	wkNum;
	uint16_t	wkPastPt_Min;
	
	switch( stRegionData.mRegionKind )
	{
		/* 異常情報 */
		case ecRegionKind_ErrInfo:
			if( gvInFlash.mData.mErrInf_Num < imDataNumErrInf )
			{
				gvInFlash.mData.mErrInf_Num++;						/* 書き込み件数更新 */
			}
			
			gvInFlash.mData.mErrInf_Index++;						/* IndexNo.更新 */
			if( gvInFlash.mData.mErrInf_Index > imIndexMaxErrInf )
			{
				gvInFlash.mData.mErrInf_Index = 1U;
			}
			break;
			
		/* 計測警報 */
		case ecRegionKind_MeasAlm:
			if( gvInFlash.mData.mMeasAlm_Num < imDataNumMeasAlm )
			{
				gvInFlash.mData.mMeasAlm_Num++;						/* 書き込み件数更新 */
			}
			
			gvInFlash.mData.mMeasAlm_Index++;						/* IndexNo.更新 */
			if( gvInFlash.mData.mMeasAlm_Index > imIndexMaxMeasAlm )
			{
				gvInFlash.mData.mMeasAlm_Index = 1U;
			}
			break;
			
		/* 計測値1 */
		case ecRegionKind_Meas1:
			if( gvInFlash.mData.mMeas1_Num < imDataNumMeas1 )
			{
				gvInFlash.mData.mMeas1_Num++;						/* 書き込み件数更新 */
			}
			
			gvInFlash.mData.mMeas1_Index++;							/* IndexNo.更新 */
			if( gvInFlash.mData.mMeas1_Index > imIndexMaxMeas1 )
			{
				gvInFlash.mData.mMeas1_Index = 1U;
				gvInFlash.mData.mMeas1_IndexNextLap = imON;			/* IndexNo.が1周完了 */
				gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;		/* 内蔵Flash書込み更新 */
			}
			
			/* 収録がリアルタイム送信ポインタに追いついたときはポインタ更新 */
			if( gvInFlash.mData.mMeas1_Num == imDataNumMeas1 )
			{
//				wkPastPt_Min = imIndexMaxMeas1 + 1U - imDataNumMeas1 + gvInFlash.mData.mMeas1_Index;
				wkPastPt_Min = 197U + gvInFlash.mData.mMeas1_Index;				/* (最大IndexNo. + 1 - 最大個数 -> 197) + 現在のIndexNo. */
				
				if( wkPastPt_Min <= imIndexMaxMeas1 )
				{
					if( gvInFlash.mData.mMeas1_OldPtrIndex >= gvInFlash.mData.mMeas1_Index && gvInFlash.mData.mMeas1_OldPtrIndex <= wkPastPt_Min )
					{
						gvInFlash.mData.mMeas1_OldPtrIndex = wkPastPt_Min;
						
						/* イベント有効から送信 */
						if( gvInFlash.mParam.mRealTimeSndPt == 510U )
						{
							/* イベント有効ポインタに過去送信ポインタを代入 */
							gvInFlash.mData.mMeas1_EvtPtrIndex = gvInFlash.mData.mMeas1_OldPtrIndex;
						}
					}
				}
				else
				{
					wkPastPt_Min -= imIndexMaxMeas1;
					if( (gvInFlash.mData.mMeas1_OldPtrIndex >= gvInFlash.mData.mMeas1_Index && gvInFlash.mData.mMeas1_OldPtrIndex <= imIndexMaxMeas1) ||
						(gvInFlash.mData.mMeas1_OldPtrIndex <= wkPastPt_Min) )
					{
						gvInFlash.mData.mMeas1_OldPtrIndex = wkPastPt_Min;
						
						/* イベント有効から送信 */
						if( gvInFlash.mParam.mRealTimeSndPt == 510U )
						{
							/* イベント有効ポインタに過去送信ポインタを代入 */
							gvInFlash.mData.mMeas1_EvtPtrIndex = gvInFlash.mData.mMeas1_OldPtrIndex;
						}
					}
				}
			}
			
			/* Flashの最新測定データを無線送信用にRAMに格納 */
			SubFlash_RfRTMeasDataSet();								/* 計測データのリアルタイム送信用データ生成 */
			
			/* セクタ変更あり */
			if( arSctrChg == imARI )
			{
				wkNum = gvInFlash.mData.mMeasVal1Adr_Num;
				if( wkNum < imDataNumAdrTbl )
				{
					wkNum++;
				}
				else
				{
					wkNum = 1U;
				}
				
				/* 測定値1アドレス更新 */
				ApiFlash_WriteFlasAdrTbl( gvInFlash.mData.mMeas1_AddrEnd, wkNum, gvInFlash.mData.mMeas1_Index, ecRegionKind_Meas1Adr );
			}
			
			/* 高速通信起動(設定データ数が溜まったら高速通信待受け) */
			if( gvFstConnBootCnt < imDataNumMeas1 )
			{
				gvFstConnBootCnt++;									/* 高速通信起動データ数更新 */
			}
			
#if 0
			/* 100データ毎RAMを保存 */
			if( gvInFlash.mData.mMeas1_Index % 100U == 1U )
			{
				gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;			/* 内蔵Flash書込み更新 */
			}
#endif
			break;
			
		/* イベント */
		case ecRegionKind_Event:
			if( gvInFlash.mData.mEvt_Num < imDataNumEvt )
			{
				gvInFlash.mData.mEvt_Num++;							/* 書き込み件数更新 */
			}
			
			gvInFlash.mData.mEvt_Index++;							/* IndexNo.更新 */
			if( gvInFlash.mData.mEvt_Index > imIndexMaxEvt )
			{
				gvInFlash.mData.mEvt_Index = 1U;
			}
			break;
			
		/* 動作履歴 */
		case ecRegionKind_ActLog:
			if( gvInFlash.mData.mActLog_Num < imDataNumActLog )
			{
				gvInFlash.mData.mActLog_Num++;						/* 書き込み件数更新 */
			}
			
			gvInFlash.mData.mActLog_Index++;						/* IndexNo.更新 */
			if( gvInFlash.mData.mActLog_Index > imIndexMaxActLog )
			{
				gvInFlash.mData.mActLog_Index = 1U;
			}
			break;
			
		/* システムログ */
		case ecRegionKind_SysLog:
			if( gvInFlash.mData.mSysLog_Num < imDataNumSysLog )
			{
				gvInFlash.mData.mSysLog_Num++;						/* システムログの書き込み件数更新 */
			}
			
			gvInFlash.mData.mSysLog_Index++;						/* IndexNo.更新 */
			if( gvInFlash.mData.mSysLog_Index > imIndexMaxSysLog )
			{
				gvInFlash.mData.mSysLog_Index = 1U;
			}
			
			/* セクタ変更あり */
			if( arSctrChg == imARI )
			{
				wkNum = gvInFlash.mData.mSysLogAdr_Num;
				if( gvInFlash.mData.mSysLogAdr_Num < imDataNumAdrTbl )
				{
					wkNum++;
				}
				else
				{
					wkNum = 1U;
				}
				
				/* システムログアドレス更新 */
				ApiFlash_WriteFlasAdrTbl( gvInFlash.mData.mSysLog_AddrEnd, wkNum, gvInFlash.mData.mSysLog_Index, ecRegionKind_SysLogAdr );
			}
			break;
			
		/* 計測値1アドレス */
		case ecRegionKind_Meas1Adr:
			if( gvInFlash.mData.mMeasVal1Adr_Num < imDataNumAdrTbl )
			{
				gvInFlash.mData.mMeasVal1Adr_Num++;
			}
			else
			{
				gvInFlash.mData.mMeasVal1Adr_Num = 1U;
			}
			break;
			
		/* システムログアドレス */
		case ecRegionKind_SysLogAdr:
			if( gvInFlash.mData.mSysLogAdr_Num < imDataNumAdrTbl )
			{
				gvInFlash.mData.mSysLogAdr_Num++;
			}
			else
			{
				gvInFlash.mData.mSysLogAdr_Num = 1U;
			}
			break;
			
		default:
			break;
	}
	
	/* セクタ変更あり */
	if( arSctrChg == imARI )
	{
		gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;			/* 内蔵Flash書込み更新 */
	}
}


/*
 *******************************************************************************
 *	書き込み終端アドレス更新
 *
 *	[内容]
 *		書き込み終端アドレスの更新処理を行う。
 *	[引数]
 *		uint32_t		arWrEndAddr: 書き込み終端アドレス
 *	[戻値]
 *		なし
 *******************************************************************************
 */
static void SubFlash_WrEndAddrUpdate( uint32_t arWrEndAddr )
{
	switch(stRegionData.mRegionKind)
	{
		/* 異常情報 */
		case ecRegionKind_ErrInfo:
			gvInFlash.mData.mErrInf_AddrEnd = arWrEndAddr;				/* 書き込み終端アドレス */
			break;
			
		/* 計測警報 */
		case ecRegionKind_MeasAlm:
			gvInFlash.mData.mMeasAlm_AddrEnd = arWrEndAddr;				/* 書き込み終端アドレス */
			break;
			
		/* 計測値1 */
		case ecRegionKind_Meas1:
			gvInFlash.mData.mMeas1_AddrEnd = arWrEndAddr;				/* 書き込み終端アドレス */
			break;
			
		/* イベント */
		case ecRegionKind_Event:
			gvInFlash.mData.mEvt_AddrEnd = arWrEndAddr;					/* 書き込み終端アドレス */
			break;
			
		/* 動作履歴 */
		case ecRegionKind_ActLog:
			gvInFlash.mData.mActLog_AddrEnd = arWrEndAddr;				/* 書き込み終端アドレス */
			break;
			
		/* システムログ */
		case ecRegionKind_SysLog:
			gvInFlash.mData.mSysLog_AddrEnd = arWrEndAddr;				/* 書き込み終端アドレス */
			break;
			
		/* 計測値1アドレス */
		case ecRegionKind_Meas1Adr:
			gvInFlash.mData.mMeasVal1Adr_AddrEnd = arWrEndAddr;			/* 書き込み終端アドレス */
			break;
			
		/* システムログアドレス */
		case ecRegionKind_SysLogAdr:
			gvInFlash.mData.mSysLogAdr_AddrEnd = arWrEndAddr;			/* 書き込み終端アドレス */
			break;
			
		default:
			break;
	}
}


/*
 *******************************************************************************
 *	外付けフラッシュ状態取得処理
 *
 *	[内容]
 *	外付けフラッシュ状態を返す。
 *******************************************************************************
 */
ET_FlashSts_t ApiGetFlashSts( void )
{
	return stFlashSts;
}

/*
 *******************************************************************************
 *	外付けフラッシュ状態セット
 *
 *	[内容]
 *	外付けフラッシュ状態をセットする。
 *******************************************************************************
 */
void ApiSetFlashSts( ET_FlashSts_t arSts )
{
	stFlashSts = arSts;
}


/*
 *******************************************************************************
 *	データキューのデータ有無を取得
 *
 *	[内容]
 *		データキューにデータがあるかどうかを取得する。
 *******************************************************************************
 */
uint8_t ApiFlash_GetQueSts( void )
{
	uint8_t wkRet = imNASI;
	
	if( stFlashQue.mNum > 0U )
	{
		wkRet = imARI;				/* データ有り */
	}
	
	return wkRet;
}

#if 0
/*
 *******************************************************************************
 *	低電圧用のデータキューのデータ有無を取得
 *
 *	[内容]
 *		低電圧用のデータキューにデータがあるかどうかを取得する。
 *******************************************************************************
 */
uint8_t ApiFlash_GetQueLowBatSts( void )
{
	uint8_t wkRet = imNASI;
	
	if( stFlashQueLowBat.mNum > 0U )
	{
		wkRet = imARI;				/* データ有り */
	}
	
	return wkRet;
}
#endif

/*
 *******************************************************************************
 *	外付けフラッシュのリード要求又は完了状態のリード
 *
 *	[内容]
 *		外付けフラッシュのリード要求又は完了状態のリード
 *	[引数]
 *		uint8_t			arSelect: リード:0、要求:1(完了フラグクリアも)
 *		ET_RegionKind_t	arRegion: 領域種類
 *	[戻値]
 *		なし
 *******************************************************************************
 */
uint8_t ApiSetFlash_ReqRdFlg( uint8_t arSelect, ET_RegionKind_t arRegion )
{
	uint8_t	wkBit;
	uint8_t	wkRet = imNASI;
	
	switch( arRegion )
	{
		/* 異常情報 */
		case ecRegionKind_ErrInfo:
			wkBit = M_BIT0;
			break;
			
		/* 計測警報 */
		case ecRegionKind_MeasAlm:
			wkBit = M_BIT1;
			break;
			
		/* 測定値1 */
		case ecRegionKind_Meas1:
			wkBit = M_BIT2;
			break;
			
		/* イベント */
		case ecRegionKind_Event:
			wkBit = M_BIT4;
			break;
			
		/* 動作履歴 */
		case ecRegionKind_ActLog:
			wkBit = M_BIT5;
			break;
			
		/* システムログ */
		case ecRegionKind_SysLog:
			wkBit = M_BIT6;
			break;
			
		default:
			break;
	}
	
	switch( arSelect )
	{
		case 0U:
			if( M_TSTBIT( stFlashReqRd.mFin, wkBit) )
			{
				wkRet = imARI;
				M_CLRBIT( stFlashReqRd.mFin, wkBit );			/* Flashリード完了クリア */
			}
			break;
		case 1U:
			M_CLRBIT( stFlashReqRd.mFin, wkBit );				/* Flashリード完了クリア */
			M_SETBIT( stFlashReqRd.mReq, wkBit );				/* Flashリード要求セット */
		gvModuleSts.mExFlash = ecExFlashModuleSts_Run;
			break;
		default:
			break;
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	Flashからリードが完了したかを判断する
 *
 *	[内容]
 *	Flashからリードが完了したかを判断する
 *******************************************************************************
 */
void ApiFlash_FinReadData( void )
{
	/* 異常情報 */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT0) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT0) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT0 );			/* Flashリード要求クリア */
		}
	}
	
	/* 計測警報 */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT1) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT1) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT1 );			/* Flashリード要求クリア */
		}
	}
	
	/* 測定値1 */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT2) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT2) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT2 );			/* Flashリード要求クリア */
		}
	}
	
	/* 測定値2 */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT3) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT3) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT3 );			/* Flashリード要求クリア */
		}
	}
	
	/* イベント */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT4) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT4) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT4 );			/* Flashリード要求クリア */
		}
	}
	
	/* 動作履歴 */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT5) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT5) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT5 );			/* Flashリード要求クリア */
		}
	}
	
	/* システムログ */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT6) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT6) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT6 );			/* Flashリード要求クリア */
		}
	}
	
	/* 全てのリード要求がなくなったらスリープ */
	if( stFlashReqRd.mReq == 0U )
	{
		gvModuleSts.mExFlashRd = ecExFlashRdModuleSts_Sleep;
	}
}


/*
 *******************************************************************************
 *	Flash書き込みの続きを検索する
 *
 *	[内容]
 *		どこまでFlashで書込みされているかを検索し、
 *		終端アドレス、データ数、インデックスNo.にセットする。
 *******************************************************************************
 */
void ApiFlash_SrchFlashAdr( void )
{
	ET_SrchKindLog_t	wkKindLoop;
	ET_FlashKind_t		wkSel;
	
	uint8_t				vPrimRdData[ imFlashRdWr_DatSize ];		/* Flashリードデータ */
	uint8_t				wkSctrLoop;
	uint8_t				wkDataByte;			/* 書込みバイト数 */
	uint8_t				wkSctrNoMinAdr;		/* アドレス特定テーブルの最小セクタNo. */
	uint8_t				wkSctrNoMin;		/* 書込み領域最小セクタNo. */
	uint8_t				wkSctrNoMax;		/* 書込み領域最大セクタNo. */
	uint8_t				wkRsvSize;			/* セクタ間の未使用サイズ */
	uint8_t				wkSctrNumFlash;		/* Flashで検索して得られたセクタNo. */
	uint8_t				wkSctrNumGlobal;	/* global変数で持っているセクタNo. */
	
	uint16_t			wkNum;				/* アドレス検索テーブルの件数 */
	uint16_t			wkNumPre;
	uint16_t			wkIndexMax;			/* 書込みIndexNo. */
	uint16_t			wkDataNum;			/* 書込み件数 */
	uint16_t			wkSrchNum;			/* 検索された件数 */
	uint16_t			wkSrchIndex;		/* 検索されたIndexNo. */
	
	uint32_t			wkAdrLoop;
	uint32_t			wkAdr;
	uint32_t			wkSctrDiff;
	uint32_t			wkSrchAddrEnd;		/* 検索されたエンドアドレス */
	uint32_t			wkSrchAdr;
	uint32_t			wkSrchAdrPre;
	uint32_t			wkClock;			/* Flashリード時刻データ */
	uint32_t			wkPreClock;
	uint32_t			wkU32;
	
	/* Flash電源ON */
	ApiFlash_FlashPowerCtl( imON, imON );
	
	if( gvInFlash.mParam.mFlashSelect == (uint8_t)ecFlashKind_Prim )			/* プライマリ選択時 */
	{
		wkSel = ecFlashKind_Prim;
	}
	else																		/* セカンダリ選択時 */
	{
		wkSel = ecFlashKind_Second;
	}
	
	/*** 異常情報、計測警報、イベント、動作履歴 ***/
	for( wkKindLoop = ecSrchKindLog_ErrInfo ; wkKindLoop <= ecSrchKindLog_ActLog ; wkKindLoop++ )
	{
		switch( wkKindLoop )
		{
			/* 異常情報 */
			case ecSrchKindLog_ErrInfo:
				wkDataNum = imDataNumInSctrErrInf;
				wkDataByte = imDataByteErrInf;
				wkSctrNoMin = imSctrNoMinErrInf;
				wkSctrNoMax = imSctrNoMaxErrInf;
				wkIndexMax = imIndexMaxErrInf;
				wkSrchAddrEnd = gvInFlash.mData.mErrInf_AddrEnd;
				wkSrchNum = gvInFlash.mData.mErrInf_Num;
				wkSrchIndex = gvInFlash.mData.mErrInf_Index;
				break;
				
			/* 計測警報 */
			case ecSrchKindLog_MeasAlm:
				wkDataNum = imDataNumInSctrMeasAlm;
				wkDataByte = imDataByteMeasAlm;
				wkSctrNoMin = imSctrNoMinMeasAlm;
				wkSctrNoMax = imSctrNoMaxMeasAlm;
				wkIndexMax = imIndexMaxMeasAlm;
				wkSrchAddrEnd = gvInFlash.mData.mMeasAlm_AddrEnd;
				wkSrchNum = gvInFlash.mData.mMeasAlm_Num;
				wkSrchIndex = gvInFlash.mData.mMeasAlm_Index;
				break;
				
			/* イベント */
			case ecSrchKindLog_Evt:
				wkDataNum = imDataNumInSctrEvt;
				wkDataByte = imDataByteEvt;
				wkSctrNoMin = imSctrNoMinEvt;
				wkSctrNoMax = imSctrNoMaxEvt;
				wkIndexMax = imIndexMaxEvt;
				wkSrchAddrEnd = gvInFlash.mData.mEvt_AddrEnd;
				wkSrchNum = gvInFlash.mData.mEvt_Num;
				wkSrchIndex = gvInFlash.mData.mEvt_Index;
				break;
				
			/* 動作履歴 */
			case ecSrchKindLog_ActLog:
				wkDataNum = imDataNumInSctrActLog;
				wkDataByte = imDataByteActLog;
				wkSctrNoMin = imSctrNoMinActLog;
				wkSctrNoMax = imSctrNoMaxActLog;
				wkIndexMax = imIndexMaxActLog;
				wkSrchAddrEnd = gvInFlash.mData.mActLog_AddrEnd;
				wkSrchNum = gvInFlash.mData.mActLog_Num;
				wkSrchIndex = gvInFlash.mData.mActLog_Index;
				break;
				
			default:
				break;
		}
		
		wkClock = 0U;
		wkPreClock = 0U;
		wkSrchAdr = wkSctrNoMin * imFlashSctrSize;
		for( wkSctrLoop = 0U ; wkSctrLoop < 2U ; wkSctrLoop++ )
		{
			wkPreClock = wkClock;
			wkAdr = (wkSctrNoMin * imFlashSctrSize) + (wkSctrLoop * imFlashSctrSize);
			for( wkAdrLoop = 0U ; wkAdrLoop < wkDataNum ; wkAdrLoop++ )
			{
				ApiFlashDrv_ReadData( wkAdr, &vPrimRdData[ 0 ], wkDataByte, wkSel );
				
				wkClock = (uint32_t)vPrimRdData[ 0U ] << 24U;
				wkClock |= (uint32_t)vPrimRdData[ 1U ] << 16U;
				wkClock |= (uint32_t)vPrimRdData[ 2U ] << 8U;
				wkClock |= vPrimRdData[ 3U ];
				wkClock >>= 2U;
				
				if( wkClock == 0x3FFFFFFF )
				{
					if( wkAdrLoop == 0U )										/* 最初のアドレス検索 */
					{
						if( wkSctrLoop != 0U )									/* 後セクタ検索中の場合 */
						{
							wkSrchAdr = (wkSctrNoMin * imFlashSctrSize) + (wkDataByte * wkDataNum);
																				/* 前セクタが全アドレスにデータ書き込み済み。前セクタの終了アドレス(データ無し)を格納 */
						}
					}
					else														/* 最初以外のアドレス検索 */
					{
						wkSrchAdr = wkAdr;										/* 最終アドレス(データ有り)を格納 */
					}
					wkSctrLoop = 2U;
					break;
				}
				if(( wkAdrLoop == 0U )											/* 最初のアドレス検索 */
				&& ( wkSctrLoop != 0U )											/* 後セクタ検索中 */
				)
				{
					if( wkPreClock > wkClock )									/* 前セクタの最終データの時刻情報 ＞ 後セクタの最初データの時刻情報 */
					{															/* 最新のデータが前セクタの最終データの時刻情報だった場合 */
						wkSrchAdr = (wkSctrNoMin * imFlashSctrSize) + (wkDataByte * wkDataNum);
						break;
					}
				}
				wkAdr += wkDataByte;
			}
		}
		
		/* 全セクタにデータが入っている場合 */
		if( wkAdr >= ((wkSctrNoMax * imFlashSctrSize) + (wkDataByte * wkDataNum)) )
		{
			wkSrchAdr = (wkSctrNoMax * imFlashSctrSize) + (wkDataByte * wkDataNum) - wkDataByte;
		}
		/* 書込み件数、IndexNo.算出 */
		wkSctrNumFlash = wkSrchAdr / imFlashSctrSize;
		wkSctrNumGlobal = wkSrchAddrEnd / imFlashSctrSize;
		
		/* Flashからリードしたグローバル変数のアドレスと検索した最終アドレスの差を算出 */
		if( wkSctrNumFlash == wkSctrNumGlobal )
		{
			if( wkSrchAdr >= wkSrchAddrEnd )
			{
				wkU32 = wkSrchAdr - wkSrchAddrEnd + 1U;
			}
			else
			{
				/* error */
				wkU32 = 0U;
			}
		}
		else
		{
			wkU32 = (wkSctrNumGlobal * imFlashSctrSize) + (wkDataNum * wkDataByte) - wkSrchAddrEnd;
			wkU32 += wkSrchAdr - (wkSctrNumFlash * imFlashSctrSize) + 1U;
		}
		wkU32 /= wkDataByte;
		
		/* 書き込み件数 */
		wkSrchNum += (uint16_t)wkU32;
		if( wkSrchNum > wkDataNum )
		{
			wkSrchNum = wkDataNum;
		}
		
		/* 書き込みIndexNo. */
		wkSrchIndex += (uint16_t)wkU32;
		if( wkSrchIndex > wkIndexMax )
		{
			wkSrchIndex -= wkIndexMax;
		}
		
		/* 書き込み終端アドレス */
		if( wkSrchAdr != wkSrchAddrEnd )
		{
			wkSrchAddrEnd = wkSrchAdr - 1U;
		}
		
		switch( wkKindLoop )
		{
			/* 異常情報 */
			case ecSrchKindLog_ErrInfo:
				gvInFlash.mData.mErrInf_Num = wkSrchNum;
				gvInFlash.mData.mErrInf_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mErrInf_Index = wkSrchIndex;
				break;
				
			/* 計測警報 */
			case ecSrchKindLog_MeasAlm:
				gvInFlash.mData.mMeasAlm_Num = wkSrchNum;
				gvInFlash.mData.mMeasAlm_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mMeasAlm_Index = wkSrchIndex;
				break;
				
			/* イベント */
			case ecSrchKindLog_Evt:
				gvInFlash.mData.mEvt_Num = wkSrchNum;
				gvInFlash.mData.mEvt_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mEvt_Index = wkSrchIndex;
				break;
				
			/* 動作履歴 */
			case ecSrchKindLog_ActLog:
				gvInFlash.mData.mActLog_Num = wkSrchNum;
				gvInFlash.mData.mActLog_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mActLog_Index = wkSrchIndex;
				break;
				
			default:
				break;
		}
	}
	
	
	/*** 計測値1、システムログ ***/
	for( wkKindLoop = ecSrchKindLog_AdrTblMeas1 ; wkKindLoop <= ecSrchKindLog_AdrTblSysLog ; wkKindLoop++ )
	{
		/* アドレステーブル検索 */
		switch( wkKindLoop )
		{
			/* アドレステーブル：計測値1 */
			case ecSrchKindLog_AdrTblMeas1:
				wkDataNum = imDataNumMeas1;
				wkDataByte = imDataByteAdrTbl;
				wkSctrNoMinAdr = imSctrNoMinMeas1Adr;
				wkSctrNoMin = imSctrNoMinMeas1;
				wkIndexMax = imIndexMaxMeas1;
				break;
				
			/* アドレステーブル：システムログ */
			case ecSrchKindLog_AdrTblSysLog:
				wkDataNum = imDataNumSysLog;
				wkDataByte = imDataByteAdrTbl;
				wkSctrNoMinAdr = imSctrNoMinSysLogAdr;
				wkSctrNoMin = imSctrNoMinSysLog;
				wkIndexMax = imIndexMaxSysLog;
				break;
			default:
				break;
		}
		
		wkNumPre = 0U;
		wkAdr = wkSctrNoMinAdr * imFlashSctrSize;
		for( wkAdrLoop = wkAdr ; wkAdrLoop < (wkAdr + 0x3000) ; wkAdrLoop += wkDataByte )
		{
			ApiFlashDrv_ReadData( wkAdrLoop, &vPrimRdData[ 0 ], wkDataByte, wkSel );
			
			wkSrchAdr = (uint32_t)vPrimRdData[ 0U ] << 24U;
			wkSrchAdr |= (uint32_t)vPrimRdData[ 1U ] << 16U;
			wkSrchAdr |= (uint32_t)vPrimRdData[ 2U ] << 8U;
			wkSrchAdr |= vPrimRdData[ 3U ];
			
			wkNum = (uint16_t)vPrimRdData[ 4U ] << 8U;
			wkNum |= vPrimRdData[ 5U ];
			
			if( (wkNum == 0xFFFF) ||
				( (wkNum != wkNumPre + 1U) && !(wkNumPre == imDataNumAdrTbl) ) ||
				( (wkNum != 1U) && (wkNumPre == imDataNumAdrTbl) ) )
			{
				/* 先頭セクタの先頭アドレスの場合 */
				if( (wkAdrLoop == wkAdr) && (wkSrchAdr == 0xFFFFFFFFU) )
				{
					/* 最後のアドレスにデータがあるかを確認 */
					ApiFlashDrv_ReadData( wkAdr + 0x2FF8U, &vPrimRdData[ 0 ], wkDataByte, wkSel );
					
					wkSrchAdr = (uint32_t)vPrimRdData[ 0U ] << 24U;
					wkSrchAdr |= (uint32_t)vPrimRdData[ 1U ] << 16U;
					wkSrchAdr |= (uint32_t)vPrimRdData[ 2U ] << 8U;
					wkSrchAdr |= vPrimRdData[ 3U ];
					
					wkNum = (uint16_t)vPrimRdData[ 4U ] << 8U;
					wkNum |= vPrimRdData[ 5U ];
					
					/* 最後のアドレスにもデータがないときはアドレステーブルにデータなし */
					if( wkNum == 0xFFFF )
					{
						wkSrchAdr = wkSctrNoMin * imFlashSctrSize;
						wkNum = 0U;
					}
					/* 最後のアドレスにデータがあるときはリードした内容を採用 */
					else
					{
						wkAdr += 0x2FF8U;
					}
				}
				else
				{
					wkAdr = wkAdrLoop - 1U;
					wkSrchAdr = wkSrchAdrPre;
					wkNum = wkNumPre;
				}
				break;
			}
			wkNumPre = wkNum;
			wkSrchAdrPre = wkSrchAdr;
		}
		
		if( wkAdrLoop >= ((wkSctrNoMinAdr * imFlashSctrSize) + (imSctrNumAdrTbl * imFlashSctrSize)) )
		{																		/* 全セクタにデータが入っている場合 */
			wkAdr = wkAdrLoop - 1U;
		}
		
		/* アドレステーブルをもとに、計測値1、システムログ検索 */
		switch( wkKindLoop )
		{
			/* アドレステーブル：計測値1 */
			case ecSrchKindLog_AdrTblMeas1:
				gvInFlash.mData.mMeasVal1Adr_Num = wkNum;
				gvInFlash.mData.mMeasVal1Adr_AddrEnd = wkAdr;
				wkDataByte = imDataByteMeas1;
				wkSrchAddrEnd = gvInFlash.mData.mMeas1_AddrEnd;
				wkSrchNum = gvInFlash.mData.mMeas1_Num;
				wkSrchIndex = gvInFlash.mData.mMeas1_Index;
				wkRsvSize = imRsvSizeMeas1;
				break;
				
			/* アドレステーブル：システムログ */
			case ecSrchKindLog_AdrTblSysLog:
				gvInFlash.mData.mSysLogAdr_Num = wkNum;
				gvInFlash.mData.mSysLogAdr_AddrEnd = wkAdr;
				wkDataByte = imDataByteSysLog;
				wkSrchAddrEnd = gvInFlash.mData.mSysLog_AddrEnd;
				wkSrchNum = gvInFlash.mData.mSysLog_Num;
				wkSrchIndex = gvInFlash.mData.mSysLog_Index;
				wkRsvSize = imRsvSizeSysLog;
				break;
				
			default:
				break;
		}
		
		wkAdr = wkSrchAdr / imFlashSctrSize;
		M_NOP;			/* 最適化防止 */
		wkAdr *= imFlashSctrSize;
		wkSrchAdr = wkAdr;
		for( wkAdrLoop = wkAdr ; wkAdrLoop < (wkAdr + 0x1000) ; wkAdrLoop += wkDataByte )
		{
			ApiFlashDrv_ReadData( wkAdrLoop, &vPrimRdData[ 0 ], wkDataByte, wkSel );
			
			wkClock = (uint32_t)vPrimRdData[ 0U ] << 24U;
			wkClock |= (uint32_t)vPrimRdData[ 1U ] << 16U;
			wkClock |= (uint32_t)vPrimRdData[ 2U ] << 8U;
			wkClock |= vPrimRdData[ 3U ];
			wkClock >>= 2U;
			
			if( wkClock == 0x3FFFFFFF )
			{
				if( wkAdrLoop == wkAdr )
				{
					wkSrchAdr = wkAdrLoop;
				}
				else
				{
					wkSrchAdr = wkAdrLoop - 1U;
				}
				break;
			}
			else
			{
				if(( wkAdrLoop + wkDataByte ) >= ( wkAdr + 0x1000 ))
				{																/* 検索した結果、全てのアドレスにデータが書き込まれていた場合 */
					wkSrchAdr = wkAdrLoop + wkDataByte - 1U;
					break;
				}
			}
		}
		
		/* 書込み件数、IndexNo.算出 */
		if( wkSrchAdr >= wkSrchAddrEnd )
		{
			/* セクタ間の未使用サイズ */
			wkSctrDiff = (wkSrchAdr / imFlashSctrSize) - (wkSrchAddrEnd / imFlashSctrSize);
			wkSctrDiff *= wkRsvSize;
			/* 件数、IndexNo.算出 */
			wkU32 = wkSrchAdr - wkSrchAddrEnd - wkSctrDiff + 1U;
			wkU32 /= wkDataByte;
		}
		else
		{
			/* error */
			wkU32 = 0U;
		}
		
		/* 書き込み件数 */
		wkSrchNum += (uint16_t)wkU32;
		if( wkSrchNum >= wkDataNum )
		{
			wkSrchNum = wkDataNum;
		}
		
		/* 書き込みIndexNo. */
		wkSrchIndex += (uint16_t)wkU32;
		
		if( wkSrchIndex > wkIndexMax )
		{
			wkSrchIndex -= wkIndexMax;
		}
		
		/* 書き込み終端アドレス */
		wkSrchAddrEnd = wkSrchAdr;
		
		
		switch( wkKindLoop )
		{
			/* アドレステーブル：計測値1 */
			case ecSrchKindLog_AdrTblMeas1:
				gvInFlash.mData.mMeas1_Num = wkSrchNum;
				gvInFlash.mData.mMeas1_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mMeas1_Index = wkSrchIndex;
				break;
				
			/* アドレステーブル：システムログ */
			case ecSrchKindLog_AdrTblSysLog:
				gvInFlash.mData.mSysLog_Num = wkSrchNum;
				gvInFlash.mData.mSysLog_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mSysLog_Index = wkSrchIndex;
				break;
				
			default:
				break;
		}
	}
	/* Flash電源OFF */
	ApiFlash_FlashPowerCtl( imOFF, imOFF );
}


/*
 *******************************************************************************
 * 計測データのリアルタイム送信用データ生成
 *
 *	[内容]
 *		計測データのリアルタイム送信用データ生成
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
static void SubFlash_RfRTMeasDataSet( void )
{
	uint8_t			wkLoop;
	uint32_t		wkU32;
	int16_t			wkS16Array[ 4U ];
	
	/* 変数初期化 */
	memset( &gvRfRTMeasData, 0, sizeof(gvRfRTMeasData) );
	
	/* 異常ステータス、電池残量 */
	ApiFlash_RfRTAbnStatusSet();
	
	/* 最新計測値 */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		wkS16Array[ wkLoop ] = stFlashWrMeasVal1.mMeasVal[ wkLoop ];
		wkS16Array[ wkLoop ] += gvInFlash.mProcess.mModelOffset;		/* 測定値を送信用にオフセット */
	}
	/* 無線用プロトコルに計測値を書き込む */
	ApiRfDrv_SetMeaAlmValArray( &wkS16Array[ 0U ], &gvRfRTMeasData.mRTMeasData_MeasVal[ 0U ] );
	
	/* 最新データ時刻 */
	for( wkLoop = 0U ; wkLoop < 4U; wkLoop++ )
	{
		wkU32 = stFlashWrMeasVal1.mTimeInfo >> (24U - wkLoop * 8U);
		gvRfRTMeasData.mRTMeasData_MeasTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* 最新計測値データ時系列ID */
	gvRfRTMeasData.mRTMeasData_MeasID[ 0U ] = (uint8_t)(gvInFlash.mData.mMeas1_Index & 0x00FF);
	gvRfRTMeasData.mRTMeasData_MeasID[ 1U ] = (uint8_t)((gvInFlash.mData.mMeas1_Index >> 8U) & 0x00FF);
	
	
	/* 論理 */
	gvRfRTMeasData.mRTMeasData_MeasVal[ 4U ] &= 0x7F;
	gvRfRTMeasData.mRTMeasData_MeasVal[ 4U ] |= (stFlashWrMeasVal1.mLogic << 7U);
	
	
	/* 最新警報フラグ */
	ApiFlash_RfRTAlarmFlagSet( gvMeasPrm.mAlmFlg, gvMeasPrm.mDevFlg, gvRfRTMeasData.mRTMeasData_AlarmFlag );
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		gvHsAlmFlag[ wkLoop ] = gvMeasPrm.mDevFlg[ wkLoop ];
		gvHsAlmFlag[ wkLoop ] |= gvMeasPrm.mAlmFlg[ wkLoop ] << 4U;
	}
	
	/* 電源起動時に最新測定値として採用するため、内蔵Flashメモリ領域に格納 */
	gvInFlash.mData.mNewTimeInfo = stFlashWrMeasVal1.mTimeInfo;				/* 最新時刻 */
	gvInFlash.mData.mNewMeasVal[ 0U ] = stFlashWrMeasVal1.mMeasVal[ 0U ];	/* 最新測定値 */
	gvInFlash.mData.mNewMeasVal[ 1U ] = stFlashWrMeasVal1.mMeasVal[ 1U ];
	gvInFlash.mData.mNewMeasVal[ 2U ] = stFlashWrMeasVal1.mMeasVal[ 2U ];
	gvInFlash.mData.mNewLogic = stFlashWrMeasVal1.mLogic;					/* 最新論理 */
}


/*
 *******************************************************************************
 * Flashからリードした計測値1をセットして返す
 *
 *	[内容]
 *		Flashからリードした計測値1をセットして返す
 *	[引数]
 *		ST_FlashVal1Evt_t	arRfRtMeasData[]: リアルタイムモード計測値1
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiFlash_RfRTMeasDataGet( ST_FlashVal1Evt_t arRfRtMeasData[] )
{
	uint8_t wkLoop;
	
	for( wkLoop = 0U ; wkLoop < 9U ; wkLoop++ )
	{
		arRfRtMeasData[ wkLoop ] = stFlashRdMeasVal1[ wkLoop ];
	}
}

/*
 *******************************************************************************
 * Flashからリードした計測警報履歴をセットして返す
 *
 *	[内容]
 *		Flashからリードした計測警報履歴をセットして返す
 *	[引数]
 *		ST_FlashMeasAlm_t	arRfRtMeasAlmData[]: 計測警報履歴(長距離通信用)
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiFlash_RfRTMeasAlmDataGet( ST_FlashMeasAlm_t arRfRtMeasAlmData[] )
{
	uint8_t wkLoop;
	
	for( wkLoop = 0U ; wkLoop < 6U ; wkLoop++ )
	{
		arRfRtMeasAlmData[ wkLoop ] = stFlashRdMeasAlmVal[ wkLoop ];
	}
}


/*
 *******************************************************************************
 * Flash電源ポートON/OFF
 *
 *	[内容]
 *		Flash電源ポートをON/OFFする
 *	[引数]
 *		uint8_t	arSel:	imOFF:OFF,imON:ON
 *		uint8_t	arWait:	imOFF:Wait他処理で実施、imON:Wait今回実施
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiFlash_FlashPowerCtl( uint8_t arSel, uint8_t arWait )
{
	uint32_t wkLoop;
	
	if( arSel == imON )
	{
		P_FLASH_POWER = (uint8_t)imLow;
		if( arWait == imON )
		{
			for( wkLoop = 0U ; wkLoop < 18400U; wkLoop++ )
			{
				M_NOP;						/* 10ステート(=10/(18.4*10^6)=0.54us, 0.54us * 18400 = 10ms) */
			}
		}
		/* CS:Hi */
		ApiFlashDrv_CSCtl( ecFlashKind_Prim, imHigh );
		ApiFlashDrv_CSCtl( ecFlashKind_Second, imHigh );
	}
	else
	{
		P_FLASH_POWER = (uint8_t)imHigh;
	}
}


/*
 *******************************************************************************
 * 時刻情報の再形成
 *
 *	[内容]
 *		外付けフラッシュに書き込んだデータ情報から、時刻情報のみを再形成する。
 *	[引数]
 *		なし
 *	[戻値]
 *		外付けフラッシュに書き込んだ最新の時刻情報
 *******************************************************************************
 */
static uint32_t SubFlash_ReMakeTime( uint8_t arWrData[] )
{
	uint32_t wkRet;
	
	wkRet = ((uint32_t)arWrData[ 0U ] << 24U) & 0xFF000000U;
	wkRet |= ((uint32_t)arWrData[ 1U ] << 16U) & 0x00FF0000U;
	wkRet |= ((uint32_t)arWrData[ 2U ] << 8U) & 0x0000FF00U;
	wkRet |= arWrData[ 3U ];
	wkRet >>= 2U;
	
	return wkRet;
}


/*
 *******************************************************************************
 * 空きデータキュー判定
 *
 *	[内容]
 *		データキューに空きがあるか判定する。
 *	[引数]
 *		なし
 *	[戻値]
 *		空き有り：ecERR_OK
 *		空き無し：ecERR_NG
 *******************************************************************************
 */
ET_Error_t ApiFlash_GetEmptyQueueJdg( void )
{
	ET_Error_t wkRet = ecERR_NG;
	
	/* 空き有り */
	if( stFlashQue.mNum < imFlashQue_DatNum )
	{
		wkRet = ecERR_OK;
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	設定変更内容をキューにためる
 *
 *	[内容]
 *		設定変更があった時、その内容を設定変更履歴のキューにためる
 *	[引数]
 *		ET_ActLogItm_t	arItem: 設定項目
 *		uint32_t		arNowVal: 現在の設定値
 *		uint32_t		arSetVal: 設定値
 *		uint8_t			arUser: ユーザーID
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiFlash_QueActHist( ET_ActLogItm_t arItem, uint32_t arNowVal, uint32_t arSetVal, uint8_t arUser )
{
	if( arNowVal != arSetVal )
	{
		if( gvActQue.mNum < 90U )
		{
			gvActQue.mUserID = arUser;
			gvActQue.mItem[ gvActQue.mNum ] = (uint8_t)arItem;
			gvActQue.mSetVal[ gvActQue.mNum ][ 0U ] = (uint8_t)((arSetVal >> 16U) & 0x000000FFU);
			gvActQue.mSetVal[ gvActQue.mNum ][ 1U ] = (uint8_t)((arSetVal >> 8U) & 0x000000FFU);
			gvActQue.mSetVal[ gvActQue.mNum ][ 2U ] = (uint8_t)(arSetVal & 0x000000FFU);
			gvActQue.mNum ++;
			gvModuleSts.mHistory = ecHistoryModuleSts_Run;
		}
	}
}


/*
 *******************************************************************************
 *	Flashライトキューに設定変更内容を動作履歴としてセットする
 *
 *	[内容]
 *		Flashライトキューに設定変更内容を動作履歴としてセットする
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiFlash_StoreActHist( void )
{
	uint32_t wkVal;
	static uint8_t wkNum = 0U;
	
	/* 格納個数が最大値未満 */
	if( stFlashQue.mNum < imFlashQue_DatNum )
	{
		wkVal = (uint32_t)gvActQue.mSetVal[ wkNum ][ 0U ] << 16U;
		wkVal += (uint32_t)gvActQue.mSetVal[ wkNum ][ 1U ] << 8U;
		wkVal += gvActQue.mSetVal[ wkNum ][ 2U ];
		
		ApiFlash_WriteActLog( (ET_ActLogItm_t)gvActQue.mItem[ wkNum ], wkVal, gvActQue.mUserID );
		
		wkNum++;
		if( wkNum >= gvActQue.mNum )
		{
			wkNum = 0U;
			gvActQue.mNum = 0U;
			
			/* 完了 */
			gvModuleSts.mHistory = ecHistoryModuleSts_Sleep;
		}
	}
}

/*
 *******************************************************************************
 *	異常ステータス、電池残量を長距離通信の変数に格納する
 *
 *	[内容]
 *		異常ステータス、電池残量を長距離通信の変数に格納する
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiFlash_RfRTAbnStatusSet( void )
{
	uint16_t		wkU16;
	
	/* 異常ステータス */
	gvRfRTMeasData.mRTMeasData_AbnStatus = 0U;
	
	/* 電池残量 */
	switch( gvBatAd.mBattSts )
	{
		case ecBattSts_Init:
		case ecBattSts_Non:
		case ecBattSts_Empty:
		default:
			gvRfRTMeasData.mRTMeasData_AbnStatus |= 0x00;
			break;
		case ecBattSts_L:
			gvRfRTMeasData.mRTMeasData_AbnStatus |= 0x01;
			break;
		case ecBattSts_M:
			gvRfRTMeasData.mRTMeasData_AbnStatus |= 0x02;
			break;
		case ecBattSts_H:
			gvRfRTMeasData.mRTMeasData_AbnStatus |= 0x03;
			break;
	}
	
	/* センサ異常 */
	wkU16 = ApiAbn_AbnStsGet( ecAbnInfKind_MeasSts );
#if (swSensorCom == imEnable)
	if( wkU16 & imAbnSts_OvUdBurnShortErrCh1 )
#else
	if( wkU16 & imAbnSts_OvUdBurnShortCh1 )
#endif
	{
		gvRfRTMeasData.mRTMeasData_AbnStatus |= M_BIT3;
	}
	if( wkU16 & imAbnSts_OvUdBurnShortCh2 )
	{
		gvRfRTMeasData.mRTMeasData_AbnStatus |= M_BIT4;
	}
	if( wkU16 & imAbnSts_OvUdBurnShortCh3 )
	{
		gvRfRTMeasData.mRTMeasData_AbnStatus |= M_BIT5;
	}
	
	/* メモリ異常 */
	wkU16 = ApiAbn_AbnStsGet( ecAbnInfKind_AbnSts );
	if( wkU16 & 0x001C )
	{
		gvRfRTMeasData.mRTMeasData_AbnStatus |= M_BIT6;
	}
	/* その他異常(電池残量0、低下を除く) */
	if( wkU16 & 0x0FE2 )
	{
		gvRfRTMeasData.mRTMeasData_AbnStatus |= M_BIT7;
	}
}


/*
 *******************************************************************************
 *	最新警報フラグを長距離通信の変数に格納する
 *
 *	[内容]
 *		最新警報フラグを長距離通信の変数に格納する
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiFlash_RfRTAlarmFlagSet( uint8_t arAlmFlg[], uint8_t arDevFlg[], uint8_t arSetFlg[] )
{
	uint8_t			wkLoop;
	uint8_t			wkBitShift;
	uint8_t			wkNum;
	

	arSetFlg[ 0U ] = 0x00U;
	arSetFlg[ 1U ] = 0x00U;
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		switch( wkLoop )
		{
			case 0U:
				wkNum = 0U;
				wkBitShift = 0U;
				break;
			case 1U:
				wkNum = 0U;
				wkBitShift = 4U;
				break;
			case 2U:
				wkNum = 1U;
				wkBitShift = 0U;
				break;
		}
		
		if( arAlmFlg[ wkLoop ] & imAlmFlgHH )
		{
			if( arAlmFlg[ wkLoop ] & imAlmFlgH )
			{
				/* 上限警報上上限警報 */
				arSetFlg[ wkNum ] |= (ecAlmFlg_AlmHAlmHH << wkBitShift);
			}
			else if( arDevFlg[ wkLoop ] & imAlmFlgH )
			{
				/* 上限逸脱上上限警報 */
				arSetFlg[ wkNum ] |= (ecAlmFlg_DevHAlmHH << wkBitShift);
			}
		}
		else if( arDevFlg[ wkLoop ] & imAlmFlgHH )
		{
			if( arAlmFlg[ wkLoop ] & imAlmFlgH )
			{
				/* 上限警報上上限逸脱 */
				arSetFlg[ wkNum ] |= (ecAlmFlg_AlmHDevHH << wkBitShift);
			}
			else if( arDevFlg[ wkLoop ] & imAlmFlgH )
			{
				/* 上限逸脱上上限逸脱 */
				arSetFlg[ wkNum ] |= (ecAlmFlg_DevHDevHH << wkBitShift);
			}
		}
		else if( arAlmFlg[ wkLoop ] & imAlmFlgH )
		{
			/* 上限警報 */
			arSetFlg[ wkNum ] |= (ecAlmFlg_AlmH << wkBitShift);
		}
		else if( arDevFlg[ wkLoop ] & imAlmFlgH )
		{
			/* 上限逸脱 */
			arSetFlg[ wkNum ] |= (ecAlmFlg_DevH << wkBitShift);
		}
		
		else if( arAlmFlg[ wkLoop ] & imAlmFlgLL )
		{
			if( arAlmFlg[ wkLoop ] & imAlmFlgL )
			{
				/* 下限警報下下限警報 */
				arSetFlg[ wkNum ] |= (ecAlmFlg_AlmLAlmLL << wkBitShift);
			}
			else if( arDevFlg[ wkLoop ] & imAlmFlgL )
			{
				/* 下限逸脱下下限警報 */
				arSetFlg[ wkNum ] |= (ecAlmFlg_DevLAlmLL << wkBitShift);
			}
		}
		else if( arDevFlg[ wkLoop ] & imAlmFlgLL )
		{
			if( arAlmFlg[ wkLoop ] & imAlmFlgL )
			{
				/* 下限警報下下限逸脱 */
				arSetFlg[ wkNum ] |= (ecAlmFlg_AlmLDevLL << wkBitShift);
			}
			else if( arDevFlg[ wkLoop ] & imAlmFlgL )
			{
				/* 下限逸脱下下限逸脱 */
				arSetFlg[ wkNum ] |= (ecAlmFlg_DevLDevLL << wkBitShift);
			}
		}
		else if( arAlmFlg[ wkLoop ] & imAlmFlgL )
		{
			/* 下限警報 */
			arSetFlg[ wkNum ] |= (ecAlmFlg_AlmL << wkBitShift);
		}
		else if( arDevFlg[ wkLoop ] & imAlmFlgL )
		{
			/* 下限逸脱 */
			arSetFlg[ wkNum ] |= (ecAlmFlg_DevL << wkBitShift);
		}
	}
}


/*
 *******************************************************************************
 *	収録データの最古収録時刻リードのキュー(初回用)をセット
 *
 *	[内容]
 *		収録データの最古収録時刻リードのキュー(初回用)をセット
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiFlash_ReadQueSetPastTime( void )
{
	SubFlash_ReadQueSetPastTime( ecRegionKind_ErrInfo );
	SubFlash_ReadQueSetPastTime( ecRegionKind_MeasAlm );
	SubFlash_ReadQueSetPastTime( ecRegionKind_Meas1 );
	SubFlash_ReadQueSetPastTime( ecRegionKind_Event );
	SubFlash_ReadQueSetPastTime( ecRegionKind_ActLog );
	SubFlash_ReadQueSetPastTime( ecRegionKind_SysLog );
}


/*
 *******************************************************************************
 *	収録データの最古収録時刻リードのキューをセット
 *
 *	[内容]
 *		収録データの最古収録時刻リードのキューをセット
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
static void SubFlash_ReadQueSetPastTime( ET_RegionKind_t arKind )
{
	uint16_t	wkPastIndex;		/* 算出した最古IndexNo. */
	
	uint16_t	wkNumMax;
	uint16_t	wkIndexMax;
	
	uint16_t	wkNowNum;
	uint16_t	wkNowIndex;
	
	ET_Error_t	wkError;
	
	switch( arKind )
	{
		/* 異常情報 */
		case ecRegionKind_ErrInfo:
			wkNumMax = imDataNumErrInf;
			wkIndexMax = imIndexMaxErrInf;
			wkNowNum = gvInFlash.mData.mErrInf_Num;
			wkNowIndex = gvInFlash.mData.mErrInf_Index;
			break;
			
		/* 計測警報 */
		case ecRegionKind_MeasAlm:
			wkNumMax = imDataNumMeasAlm;
			wkIndexMax = imIndexMaxMeasAlm;
			wkNowNum = gvInFlash.mData.mMeasAlm_Num;
			wkNowIndex = gvInFlash.mData.mMeasAlm_Index;
			break;
			
		/* 測定値1 */
		case ecRegionKind_Meas1:
			wkNumMax = imDataNumMeas1;
			wkIndexMax = imIndexMaxMeas1;
			wkNowNum = gvInFlash.mData.mMeas1_Num;
			wkNowIndex = gvInFlash.mData.mMeas1_Index;
			break;
			
		/* イベント */
		case ecRegionKind_Event:
			wkNumMax = imDataNumEvt;
			wkIndexMax = imIndexMaxEvt;
			wkNowNum = gvInFlash.mData.mEvt_Num;
			wkNowIndex = gvInFlash.mData.mEvt_Index;
			break;
			
		/* 動作履歴 */
		case ecRegionKind_ActLog:
			wkNumMax = imDataNumActLog;
			wkIndexMax = imIndexMaxActLog;
			wkNowNum = gvInFlash.mData.mActLog_Num;
			wkNowIndex = gvInFlash.mData.mActLog_Index;
			break;
			
		/* システムログ */
		case ecRegionKind_SysLog:
			wkNumMax = imDataNumSysLog;
			wkIndexMax = imIndexMaxSysLog;
			wkNowNum = gvInFlash.mData.mSysLog_Num;
			wkNowIndex = gvInFlash.mData.mSysLog_Index;
			break;
	}
	
	
	/* 最古IndexNo.算出 */
	/* データ数:1〜最大値-1 */
	if( wkNowNum < wkNumMax )
	{
		wkPastIndex = 1U;
	}
	/* データ数：最大値 */
	else
	{
		if( wkNowIndex >= wkNumMax )
		{
			wkPastIndex = wkNowIndex - wkNumMax + 1;
		}
		else
		{
			/* IndexNo.リングバッファまたぎ計算 */
			wkPastIndex = wkIndexMax + 1 - (wkNumMax - wkNowIndex);
		}
	}
	
	/* Flashにリードキューをセット */
	wkError = ApiFlash_ReadQueSet( 0U, 0U, wkPastIndex, 1U, arKind, 2U );
	if( wkError != ecERR_OK )
	{
		/* キューがいっぱい:debug */
		M_NOP;
	}
}

/*
 *******************************************************************************
 *	電源投入時、収録されている最新収録データを無線送信用にセットする
 *
 *	[内容]
 *		電源投入時、収録されている最新収録データを無線送信用にセットする
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiFlash_SetNewMeasVal( void )
{
	/* 内蔵Flashにメモリしていた最新データをセット */
	if( gvInFlash.mData.mNewTimeInfo != 0xFFFFFFFF )
	{
		stFlashWrMeasVal1.mTimeInfo = gvInFlash.mData.mNewTimeInfo;				/* 最新時刻 */
		memcpy( stFlashWrMeasVal1.mMeasVal, gvInFlash.mData.mNewMeasVal, 3U );
//		stFlashWrMeasVal1.mMeasVal[ 0U ] = gvInFlash.mData.mNewMeasVal[ 0U ];	/* 最新測定値 */
//		stFlashWrMeasVal1.mMeasVal[ 1U ] = gvInFlash.mData.mNewMeasVal[ 1U ];
//		stFlashWrMeasVal1.mMeasVal[ 2U ] = gvInFlash.mData.mNewMeasVal[ 2U ];
		stFlashWrMeasVal1.mLogic = gvInFlash.mData.mNewLogic;					/* 最新論理 */
	}
	
	/* Flashの最新測定データを無線送信用にRAMに格納 */
	SubFlash_RfRTMeasDataSet();
}
#pragma section



