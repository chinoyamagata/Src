/*
 *******************************************************************************
 *	File name	:	Flash.c
 *
 *	[内容]
 *		内蔵フラッシュ処理
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2019
 *******************************************************************************
 */
#include "cpu_sfr.h"							/* SFR定義ヘッダー */
#include "typedef.h"							/* データ型定義 */
#include "UserMacro.h"							/* ユーザー作成マクロ定義 */
#include "immediate.h"							/* immediate定数定義 */
#include "switch.h"								/* コンパイルスイッチ定義 */
#include "enum.h"								/* 列挙型の定義 */
#include "struct.h"								/* 構造体定義 */
#include "func.h"								/* 関数の外部参照(extern)定義 */
#include "glLabel.h"							/* グローバル変数・定数(const)ラベル定義 */
#include "InFlash.h"
#include "RF_Immediate.h"

#include "pfdl.h"								/* Flash Data Library T04 */
#include "pfdl_types.h"


/*
 *==============================================================================
 *	define定義
 *==============================================================================
 */

/* 内蔵Flashファーストアクセス値 */
#define	imFirstAcsChkVal	0xA5U

/* ブロックNo. */
#define	imBlock0			0U
#define	imBlock1			1U
#define	imBlock2			2U
#define	imBlock3			3U

/* メモリアドレス */
#define	imAddrStartBlk0		0x0000
#define	imAddrStartBlk1		0x0400
#define	imAddrStartBlk2		0x0800
#define	imAddrStartBlk3		0x0C00

/* アドレスオフセット */
#define	imAddrProcessOfs	0x0000
#define	imAddrParamOfs		0x0080
#define	imAddrDataOfs		0x0190

/* ブロックサイズ */
#define	imBlkSize			0x0400


/*
 *==============================================================================
 *	列挙型定義
 *==============================================================================
 */
/* リクエスト状態 */
typedef enum ET_InFlashReq
{
	ecInFlashReq_Init = 0,				/* 初期 */
	ecInFlashReq_Wr,					/* 書き込みリクエスト */
	ecInFlashReq_Rd,					/* 読み込みリクエスト */

	ecInFlashReqMax						/* enum最大値 */
} ET_InFlashReq_t;

/* 内蔵フラッシュ状態 */
typedef enum ET_InFlashSts
{
	ecInFlashSts_Init = 0,				/* 初期 */
	ecInFlashSts_BlankChkPre,			/* ブランクチェック準備 */
	ecInFlashSts_BlankChk,				/* ブランクチェック中 */
	ecInFlashSts_ErasePre,				/* ブロック消去準備 */
	ecInFlashSts_Erase,					/* ブロック消去中 */
	ecInFlashSts_Write,					/* 書き込み中 */
	ecInFlashSts_Verify,				/* ベリファイ */
	ecInFlashSts_ReadCheck,				/* リードチェック */
	ecInFlashSts_Close,					/* クローズ */

	ecInFlashStsMax						/* enum最大値 */
} ET_InFlashSts_t;

/* 設定値リスト */
typedef enum ET_SetParamList
{
	ecSetParamList_UUDevi = 0,			/* 逸脱値 */
	ecSetParamList_UDevi,
	ecSetParamList_LDevi,
	ecSetParamList_LLDevi,
	
	ecSetParamList_UUDelay,				/* 遅延値 */
	ecSetParamList_UDelay,
	ecSetParamList_LDelay,
	ecSetParamList_LLDelay,
	
	ecSetParamList_DeviEnaLv,			/* 逸脱許容時間選択閾値 */
	ecSetParamList_DeviEnaTime,			/* 逸脱許容時間 */
	
	ecSetParamList_Offset,				/* 計測値オフセット */
	ecSetParamList_Slope,				/* 計測値一次傾き補正 */
	
	ecSetParamList_BatType,				/* 電池種 */
	ecSetParamList_KeyLock,				/* キーロック */
	
	ecSetParamList_RealTimeCh,			/* リアルタイム待受ch */
	
	ecSetParamList_LogCyc1,				/* 収録周期1 */
	ecSetParamList_LogCyc2,				/* 収録周期2 */
	
	ecSetParamList_LoggerName,			/* ロガー名称 */
	
	ecSetParamList_FstConnCh,			/* 高速通信待受ch */
	ecSetParamList_FstConnBoot,			/* 高速通信起動のログデータ閾値 */
	
	ecSetParamList_AlertType,			/* 警報監視演算設定(演算種) */
	ecSetParamList_AlertParm,			/* 警報監視演算設定(パラメータ) */
	ecSetParamList_AlertFlag,			/* 警報監視演算設定(フラグ記録) */
	
	ecSetParamList_EventKeyFlg,			/* イベントボタン制御 */
	
	ecSetParamList_RealTimeDisConn,		/* リアルタイム通信切断判定時間 */
	ecSetParamList_RealTimeSndPtn,		/* リアルタイム通信転送始点データ */
	
	ecSetParamList_DeviClrPoint,		/* 逸脱許容時間のクリア契機 */
	ecSetParamList_DeviClrTime,			/* 逸脱許容時間のクリア時刻 */
	
	ecSetParamList_CalDate,				/* 校正日 */
	
	ecSetParamList_ScalDecimal,			/* スケーリング小数点位置 */
	ecSetParamList_ScalYInt,			/* スケーリング切片 */
	ecSetParamList_ScalGrad,			/* スケーリング傾き */
	
	ecSetParamList_FlashSelect,			/* フラッシュメモリ切替え */
	
	ecSetParamList_CnctEnaGwId,			/* 接続許容GW */
	
	ecSetParamList_GrId,				/* グループID */
	
	ecSetParamList_MAX
} ET_SetParamList_t;


/*
 *==============================================================================
 *	構造体定義
 *==============================================================================
 */
 
/* 内蔵フラッシュ処理パラメータ */
typedef struct ST_InFlashPrm
{
	ET_InFlashReq_t	mReqSts;			/* リクエスト状態 */
	ET_InFlashSts_t	mSts;				/* 状態 */
} ST_InFlashPrm_t;

/* 領域種データ */
typedef struct ST_InFlashRegionData
{
	uint32_t		mWrNum;				/* 書き込み件数 */
	uint32_t		mWrAddrEnd;			/* 書き込み終端アドレス */
	uint32_t		mWrAddrFin;			/* 書き込み最終アドレス */
} ST_InFlashRegionData_t;

/* 設定値制限リスト用 */
typedef struct ST_SetValWriteLimit
{
	ET_SetParamList_t	mParam;			/* 設定値名 */
	uint16_t			mMin;			/* 設定最小値 */
	uint16_t			mMax;			/* 設定最大値 */
	uint16_t			mKeep;			/* 設定値継続 */
	ET_ActLogItm_t		mLogItm;		/* 履歴No. */
} ST_SetValWriteLimit_t;

/* 設定値制限リスト用 */
typedef struct ST_AlmSetCh
{
	ET_SensType_t		mSensr;			/* センサ種 */
	uint8_t				mCh;			/* 警報設定CH */
} ST_AlmSetCh_t;

/*
 *==============================================================================
 *	変数定義
 *==============================================================================
 */

static ST_InFlashPrm_t	vInFlashPrm = { ecInFlashReq_Init, ecInFlashSts_Init };		/* 内蔵フラッシュ処理パラメータ */

/*
 *==============================================================================
 *	定数定義
 *==============================================================================
 */
/* 設定値制限リスト */
const ST_SetValWriteLimit_t cSetValWriteLimitTbl[ 33U ] =
{
	/* mParam,							mMin,	mMax,	mKeep,	mLogItm */
	{ ecSetParamList_UUDevi,			0,		8191,	1,		ecActLogItm_UUDevi1			},
	{ ecSetParamList_UDevi,				0,		8191,	1,		ecActLogItm_UDevi1			},
	{ ecSetParamList_LDevi,				0,		8191,	1,		ecActLogItm_LDevi1			},
	{ ecSetParamList_LLDevi,			0,		8191,	1,		ecActLogItm_LLDevi1			},
	{ ecSetParamList_UUDelay,			0,		60,		63,		ecActLogItm_UUDelay1		},
	{ ecSetParamList_UDelay,			0,		60,		63,		ecActLogItm_UDelay1			},
	{ ecSetParamList_LDelay,			0,		60,		63,		ecActLogItm_LDelay1			},
	{ ecSetParamList_LLDelay,			0,		60,		63,		ecActLogItm_LLDelay1		},
	{ ecSetParamList_DeviEnaLv,			0,		15,		255,	ecActLogItm_DeviEnaLv1		},
	{ ecSetParamList_DeviEnaTime,		1,		4000,	0,		ecActLogItm_DeviEnaTime1	},
	{ ecSetParamList_Offset,			0,		250,	255,	ecActLogItm_Offset1			},
	{ ecSetParamList_Slope,				0,		60,		63,		ecActLogItm_Slope1			},
	{ ecSetParamList_BatType,			1,		3,		0,		ecActLogItm_BatType			},
	{ ecSetParamList_KeyLock,			0,		15,		255,	ecActLogItm_KeyLock			},
	{ ecSetParamList_RealTimeCh,		0,		25,		255,	ecActLogItm_RealTimeCh		},
	{ ecSetParamList_LogCyc1,			0,		9,		15,		ecActLogItm_LogCyc1			},
	{ ecSetParamList_LogCyc2,			0,		8,		15,		ecActLogItm_LogCyc2			},
	{ ecSetParamList_LoggerName,		0,		255,	255,	ecActLogItm_LoggerName1		},
	{ ecSetParamList_FstConnCh,			0,		4,		15,		ecActLogItm_FstConnCh		},
	{ ecSetParamList_FstConnBoot,		0,		53000,	65535,	ecActLogItm_FstConnBoot		},
	{ ecSetParamList_AlertType,			0,		4,		7,		ecActLogItm_AlertType		},
	{ ecSetParamList_AlertParm,			0,		30,		31,		ecActLogItm_AlertParm		},
	{ ecSetParamList_AlertFlag,			0,		1,		3,		ecActLogItm_AlertFlag		},
	{ ecSetParamList_EventKeyFlg,		0,		1,		3,		ecActLogItm_EventKeyFlg		},
//	{ ecSetParamList_RealTimeDisConn,	0,		50,		63		},
//	{ ecSetParamList_RealTimeSndPtn,	0,		500,	511		},
	{ ecSetParamList_DeviClrPoint,		0,		1,		3,		ecActLogItm_DeviClrPoint	},
	{ ecSetParamList_DeviClrTime,		0,		1439,	2047,	ecActLogItm_DeviClrTime		},
	{ ecSetParamList_CalDate,			0,		32767,	65535,	ecActLogItm_CalDate			},
	{ ecSetParamList_ScalDecimal,		0,		3,		15,		ecActLogItm_ScalDecimal		},
	{ ecSetParamList_ScalYInt,			0,		60000,	65535,	ecActLogItm_ScalYInt		},
	{ ecSetParamList_ScalGrad,			0,		60000,	65535,	ecActLogItm_ScalGrad		},
	{ ecSetParamList_FlashSelect,		0,		1,		15,		ecActLogItm_FlashSelect		},
	{ ecSetParamList_CnctEnaGwId,		0,		255,	255,	ecActLogItm_CnctEnaGwId1	},
	{ ecSetParamList_GrId,				1,		255,	0,		ecActLogItm_GrId			}
};

/* 警報関連の設定CH数 */
#if (swSensorCom == imEnable)
const ST_AlmSetCh_t cAlmSetChTbl[ 6U ] =
{
	{ ecSensType_InTh,		1U },
	{ ecSensType_ExTh,		1U },
	{ ecSensType_HumTmp,	2U },
	{ ecSensType_Pt,		1U },
	{ ecSensType_V,			1U },
	{ ecSensType_Pulse,		1U }
};
#else
const ST_AlmSetCh_t cAlmSetChTbl[ 2U ] =
{
	{ ecSensType_InTh,		1U },
	{ ecSensType_ExTh,		1U }
};
#endif
/*
 *==============================================================================
 *	プロトタイプ宣言
 *==============================================================================
 */
void SubInFlash_Write( ET_FlashKind_t arKind, ET_InFlashWrSts_t arSelect, uint16_t arAddr );
static uint16_t SubInFlash_SetValWrite_ValChk( ET_SetParamList_t arParam, uint16_t arSetVal, uint16_t arNowVal, uint8_t arUser, uint8_t arLoop );
static sint16_t SubInFlash_AlmVal_CnttoVal( uint16_t arAlmValCnt );
static sint8_t SubInFlash_OftVal_CnttoVal( uint8_t arOftValCnt );
static sint16_t SubInFlash_ScaleVal_CnttoVal( uint16_t arScaleValCnt );
static sint16_t SubInFlash_UpdateAlmVal( ET_SetParamList_t arParmList, uint8_t *parSetVal, sint16_t arNowVal, uint8_t arUserID, uint8_t arLoop );
static uint8_t SubInFlash_UpdateAlmDelay( ET_SetParamList_t arParmList, uint8_t *parSetVal, uint8_t arNowVal, uint8_t arUserID, uint8_t arLoop );
void SubInFlash_SetLoggerName( uint8_t *ptrLoggerName );
void SubInFlash_SetCommGwID( uint8_t (*ptrCommGwID)[3] );
void SubInFlash_SetScale( uint8_t *ptrOffset, uint8_t *ptrZerospan );

static void SubInFlash_SetValTime( void );


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	内蔵フラッシュ処理 起動時読み出し処理
 *
 *	[内容]
 *		起動時、内蔵フラッシュから設定値を読み出し、RAMに展開する。
 *******************************************************************************
 */
void ApiInFlash_PwrOnRead( void )
{
	uint8_t			wkRdData;					/* リードデータ */
	uint8_t			*pwkData;					/* RAM格納アドレス */
	uint8_t			wkLoop;
	uint8_t			wkLoopTbl;
	uint8_t			wkRetryCnt;
	uint16_t		wkStrtAddr;					/* 開始アドレス */
	uint16_t		wkEndAddr;					/* 終了アドレス */
	
	uint8_t			wkAcsChk[ 2U ] = { 0xFFU, 0xFFU };
	
	
	/* 内蔵フラッシュドライバ開始 */
	if( PFDL_OK == ApiInFlashDrv_Open() )
	{
		/* 内蔵Flashファーストアクセスチェック */
		if( PFDL_OK == ApiInFlashDrv_RdCmdSnd( imAddrStartBlk0, 1U, &wkRdData ) )
		{
			wkAcsChk[ 0U ] = wkRdData;
		}
		if( PFDL_OK == ApiInFlashDrv_RdCmdSnd( imAddrStartBlk3, 1U, &wkRdData ) )
		{
			wkAcsChk[ 1U ] = wkRdData;
		}
		
		/* ファーストアクセスあり */
		if( wkAcsChk[ 0U ] == imFirstAcsChkVal || wkAcsChk[ 1U ] == imFirstAcsChkVal )
		{
			/* 内蔵FlashからデータリードしRAMに格納 */
			for( wkLoop = 0U ; wkLoop < 2U ; wkLoop++ )
			{
				for( wkLoopTbl = 0U ; wkLoopTbl < 3U ; wkLoopTbl++ )
				{
					if( wkLoop == 0U )
					{
						wkStrtAddr = imAddrStartBlk0;
					}
					else
					{
						wkStrtAddr = imAddrStartBlk3;
					}
					
					switch( wkLoopTbl )
					{
						case 0U:
							wkStrtAddr += imAddrProcessOfs;
							wkEndAddr = sizeof( gvInFlash.mProcess ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mProcess;
							break;
						case 1U:
							wkStrtAddr += imAddrParamOfs;
							wkEndAddr = sizeof( gvInFlash.mParam ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mParam;
							break;
						case 2U:
							wkStrtAddr += imAddrDataOfs;
							wkEndAddr = sizeof( gvInFlash.mData ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mData;
							break;
					}
					
					for( ; wkStrtAddr < wkEndAddr; wkStrtAddr++, pwkData++ )
					{
						for( wkRetryCnt = 0U ; wkRetryCnt < 3U ; wkRetryCnt++ )
						{
							if( PFDL_OK == ApiInFlashDrv_RdCmdSnd( wkStrtAddr, 1U, &wkRdData ) )
							{
								if( wkLoop == 0U )
								{
									*pwkData = wkRdData;							/* RAMに格納 */
									break;
								}
								else
								{
									/* 値比較 */
									if( *pwkData == wkRdData )
									{
										break;
									}
								}
							}
						}
					}
				}
			}
			ApiInFlashDrv_Close();
		}
		/* ファーストアクセスなし */
		else
		{
			ApiInFlashDrv_Close();
			
			/* 工程プロセス、設定値を初期値でライト */
			ApiInFlash_ParmWrite( ecInFlashWrSts_Process, imAddrStartBlk0 );
		}
	}
}


/*
 *******************************************************************************
 *	内蔵Flashにパラメータをライト
 *
 *	[引数]
 *		ET_InFlashWrSts_t	arSelect	：設定値初期化、工程+設定値初期化、現時点RAM
 *	[内容]
 *		内蔵Flashにパラメータをライトする
 *******************************************************************************
 */
void ApiInFlash_ParmWrite( ET_InFlashWrSts_t arSelect, uint16_t arAddr )
{
	ET_FlashKind_t	wkKind;
	
	switch( arSelect )
	{
		/* Key or Modbusによる初期化 */
		case ecInFlashWrSts_ParmInit:
			gvInFlash.mParam = cInFlashParmTbl;
			gvInFlash.mData = cInFlashDataTbl;
			gvInFlash.mProcess.mModelCode = 0xFFU;
			gvrfStsEventFlag.mReset = RFIC_INI;
			break;
			
		/* 一番はじめの電源投入 */
		case ecInFlashWrSts_Process:
		case ecInFlashWrSts_Process2:
			
			/* ecInFlashWrSts_Process2: 工程用テーブルは上書きしない */
			if( arSelect != ecInFlashWrSts_Process2 )
			{
				gvInFlash.mProcess = cInFlashProcessTbl;
				gvInFlash.mProcess.mUpVer[ 0U ] = cRomVer[ 0U ];
				gvInFlash.mProcess.mUpVer[ 1U ] = cRomVer[ 1U ] * 10U + cRomVer[ 2U ];
				gvInFlash.mProcess.mUpVer[ 2U ] = cRomVer[ 3U ];
			}
			
			gvInFlash.mParam = cInFlashParmTbl;
			gvInFlash.mData = cInFlashDataTbl;
			gvInFlash.mParam.mLogCyc1 = ecRecKind1_1min;		/* 工程では1分周期(ResetKey工場出荷初期化は5分周期) */
			gvInFlash.mParam.mLogCyc2 = ecRecKind2_1min;
			gvInFlash.mParam.mAppID[ 0U ] = 0x00U;				/* 工程ではアプリID「1」 */
			gvInFlash.mParam.mAppID[ 1U ] = 0x01U;
			gvInFlash.mParam.mOnCertLmt = CENTRAL;
			break;
			
		/* 現在のRAM変数を内蔵Flashにライトするとき */
		case ecInFlashWrSts_ParmNow:
		default:
			break;
	}
	
	/* 内蔵フラッシュドライバ開始 */
	if( PFDL_OK == ApiInFlashDrv_Open() )
	{
		/* 内蔵Flash(primary, secondary)へ初期値をライト */
		for( wkKind = ecFlashKind_Prim ; wkKind < 2U ; wkKind++ )
		{
			while( 1 )
			{
				SubInFlash_Write( wkKind, arSelect, arAddr );
				if( vInFlashPrm.mSts == ecInFlashSts_Init )
				{
					NOP();
					break;
				}
			}
		}
	}
	
	/* 内蔵フラッシュドライバ終了 */
	ApiInFlashDrv_Close();
	
	/* 基板検査用としてFlashにメモリせずにアプリIDを「0」にする */
	if( arSelect == ecInFlashWrSts_Process )
	{
		gvInFlash.mParam.mAppID[ 1U ] = 0x00U;
	}
}


/*
 *******************************************************************************
 *	内蔵フラッシュ処理 書込み処理
 *
 *	[内容]
 *		内蔵フラッシュ処理 書込み処理
 *******************************************************************************
 */
void SubInFlash_Write( ET_FlashKind_t arKind, ET_InFlashWrSts_t arSelect, uint16_t arAddr )
{
	pfdl_status_t	wkRet;					/* エラー戻り値 */
	uint8_t			wkRdData;
	uint8_t			*pwkData;
	uint8_t			wkLoopTbl;
	uint8_t			wkRetryCnt;
	uint16_t		wkStrtAddr;				/* 開始アドレス */
	uint16_t		wkEndAddr;				/* 終了アドレス */
	uint16_t		wkSize;					/* サイズ */
	
	if( vInFlashPrm.mReqSts != ecInFlashReq_Rd )
	{
		switch( vInFlashPrm.mSts )
		{
			/* 内蔵Flashオープン */
			case ecInFlashSts_Init:
				vInFlashPrm.mReqSts = ecInFlashReq_Wr;
				vInFlashPrm.mSts = ecInFlashSts_ErasePre;
				break;
				
			/* ブロック消去準備 */
			case ecInFlashSts_ErasePre:
				
				if( arKind == ecFlashKind_Prim )
				{
					/* ブロック消去コマンド送信 */
					wkRet = ApiInFlashDrv_BlkErsCmdSnd( imBlock0 );
				}
				else
				{
					/* ブロック消去コマンド送信 */
					wkRet = ApiInFlashDrv_BlkErsCmdSnd( imBlock3 );
				}
				vInFlashPrm.mSts = ecInFlashSts_Erase;
				break;
				
			/* ブロック消去中 */
			case ecInFlashSts_Erase:
				for( wkRetryCnt = 0U ; wkRetryCnt < 3U ; wkRetryCnt++ )
				{
					wkRet = ApiInFlashDrv_HandlerCmdSnd();
					if( wkRet == PFDL_OK )
					{
						vInFlashPrm.mSts = ecInFlashSts_BlankChkPre;
						break;
					}
					else if( wkRet == PFDL_BUSY )
					{
						/* コマンド実行中 */
						NOP();
						break;
					}
					else
					{
						if( wkRetryCnt == 2U )
						{
							ApiAbn_AbnStsSet( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );	/* 内蔵フラッシュアクセスエラー */
							vInFlashPrm.mSts = ecInFlashSts_Close;
						}
					}
				}
				break;
				
			/* ブランクチェック準備 */
			case ecInFlashSts_BlankChkPre:
				if( arKind == ecFlashKind_Prim )
				{
					wkStrtAddr = imAddrStartBlk0;
				}
				else
				{
					wkStrtAddr = imAddrStartBlk3;
				}
				
				/* ブランクチェックコマンド送信 */
				wkRet = ApiInFlashDrv_BlnkChkCmdSnd( wkStrtAddr, imBlkSize );
				vInFlashPrm.mSts = ecInFlashSts_BlankChk;
				break;
				
			/* ブランクチェック中 */
			case ecInFlashSts_BlankChk:
				for( wkRetryCnt = 0U ; wkRetryCnt < 3U ; wkRetryCnt++ )
				{
					wkRet = ApiInFlashDrv_HandlerCmdSnd();
					if( wkRet == PFDL_OK )
					{
						vInFlashPrm.mSts = ecInFlashSts_Write;
						break;
					}
					else if( wkRet == PFDL_ERR_MARGIN )
					{
						/* ブランク状態でない */
						vInFlashPrm.mSts = ecInFlashSts_ErasePre;
						break;
					}
					else if( wkRet == PFDL_BUSY )
					{
						/* コマンド実行中 */
						NOP();
						break;
					}
					else
					{
						if( wkRetryCnt == 2U )
						{
							ApiAbn_AbnStsSet( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );	/* 内蔵フラッシュアクセスエラー */
							vInFlashPrm.mSts = ecInFlashSts_Close;
						}
					}
				}
				break;
				
			/* 書き込み、ベリファイ */
			case ecInFlashSts_Write:
			case ecInFlashSts_Verify:
				for( wkLoopTbl = 0U ; wkLoopTbl < 3U ; wkLoopTbl++ )
				{
					if( arKind == ecFlashKind_Prim )
					{
						wkStrtAddr = imAddrStartBlk0;
					}
					else
					{
						wkStrtAddr = imAddrStartBlk3;
					}
					
					switch( wkLoopTbl )
					{
						case 0U:
							if( vInFlashPrm.mSts == ecInFlashSts_Write )
							{
								pwkData = (uint8_t *)&gvInFlash.mProcess;
							}
							wkSize = sizeof( gvInFlash.mProcess );
							wkStrtAddr += imAddrProcessOfs;
							break;
						case 1U:
							if( vInFlashPrm.mSts == ecInFlashSts_Write )
							{
								pwkData = (uint8_t *)&gvInFlash.mParam;
							}
							wkSize = sizeof( gvInFlash.mParam );
							wkStrtAddr += imAddrParamOfs;
							break;
						case 2U:
							if( vInFlashPrm.mSts == ecInFlashSts_Write )
							{
								pwkData = (uint8_t *)&gvInFlash.mData;
							}
							wkSize = sizeof( gvInFlash.mData );
							wkStrtAddr += imAddrDataOfs;
							break;
					}
					
					if( vInFlashPrm.mSts == ecInFlashSts_Write )
					{
						/* 書込みコマンド送信 */
						wkRet = ApiInFlashDrv_WrCmdSnd( wkStrtAddr, wkSize, pwkData );
					}
					else
					{
						/* ベリファイコマンド送信 */
						wkRet = ApiInFlashDrv_VerifyCmdSnd( wkStrtAddr, wkSize );
					}
					
					wkRetryCnt = 0U;
					while( 1 )
					{
						wkRet = ApiInFlashDrv_HandlerCmdSnd();
						if( wkRet == PFDL_OK )
						{
							break;
						}
						else if( wkRet == PFDL_BUSY )
						{
							/* コマンド実行中 */
							NOP();
						}
						else
						{
							if( wkRetryCnt == 2U )
							{
								ApiAbn_AbnStsSet( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );			/* 内蔵フラッシュアクセスエラー */
								vInFlashPrm.mSts = ecInFlashSts_Close;
								break;
							}
							wkRetryCnt ++;
						}
					}
				}
				
				if( vInFlashPrm.mSts != ecInFlashSts_Close )
				{
					if( vInFlashPrm.mSts == ecInFlashSts_Write )
					{
						vInFlashPrm.mSts = ecInFlashSts_Verify;
					}
					else
					{
						vInFlashPrm.mSts = ecInFlashSts_ReadCheck;
					}
				}
				break;
				
			/* リードチェック */
			case ecInFlashSts_ReadCheck:
				for( wkLoopTbl = 0U ; wkLoopTbl < 3U ; wkLoopTbl++ )
				{
					if( arKind == ecFlashKind_Prim )
					{
						wkStrtAddr = imAddrStartBlk0;
					}
					else
					{
						wkStrtAddr = imAddrStartBlk3;
					}
					
					switch( wkLoopTbl )
					{
						case 0U:
							wkStrtAddr += imAddrProcessOfs;
							wkEndAddr= sizeof( gvInFlash.mProcess ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mProcess;
							break;
						case 1U:
							wkStrtAddr += imAddrParamOfs;
							wkEndAddr = sizeof( gvInFlash.mParam ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mParam;
							break;
						case 2U:
							wkStrtAddr += imAddrDataOfs;
							wkEndAddr = sizeof( gvInFlash.mData ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mData;
							break;
					}
					
					for( ; wkStrtAddr < wkEndAddr; wkStrtAddr++,pwkData++ )
					{
						for( wkRetryCnt = 0U ; wkRetryCnt < 3U ; wkRetryCnt++ )
						{
							/* 読み込みコマンド送信 */
							wkRet = ApiInFlashDrv_RdCmdSnd( wkStrtAddr, 1U, &wkRdData );
							if( *pwkData != wkRdData )
							{
								if( wkRetryCnt == 2U )
								{
									ApiAbn_AbnStsSet( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );		/* 内蔵フラッシュアクセスエラー */
								}
							}
							else
							{
#if 0	/* 機器異常復帰させない */
								ApiAbn_AbnStsClr( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );			/* 内蔵Flasエラー解除 */
#endif
								break;
							}
						}
					}
				}
				vInFlashPrm.mSts = ecInFlashSts_Close;
				break;
				
			/* 内蔵Flashクローズ */
			case ecInFlashSts_Close:
				vInFlashPrm.mSts = ecInFlashSts_Init;							/* 内蔵フラッシュ状態初期 */
				vInFlashPrm.mReqSts = ecInFlashReq_Init;						/* リクエスト状態初期 */
				break;
		}
	}
}


/*
 *******************************************************************************
 *	内蔵Flashからパラメータをリード
 *
 *	[引数]
 *		uint16_t arAddr：リード先アドレス
 *	[戻値]
 *		uint8_t	wkRdDataPrim: リード値
 *	[内容]
 *		内蔵Flashからアドレス指定で値をリードする
 *******************************************************************************
 */
uint8_t ApiInFlash_Read( uint16_t arAddr )
{
	uint16_t		wkStrtAddr;
	uint8_t			wkRdDataPrim;
	uint8_t			wkRdDataSecond;
	uint8_t			wkRetryCnt;
	
	/* 内蔵フラッシュドライバ開始 */
	if( PFDL_OK == ApiInFlashDrv_Open() )
	{
		for( wkRetryCnt = 0U ; wkRetryCnt < 3U ; wkRetryCnt++ )
		{
			wkStrtAddr = arAddr;
			ApiInFlashDrv_RdCmdSnd( wkStrtAddr, 1U, &wkRdDataPrim );
			
			if( wkStrtAddr < imAddrStartBlk1 )
			{
				wkStrtAddr += 0xC00U;
			}
			else
			{
				wkStrtAddr += 0x400U;
			}
			ApiInFlashDrv_RdCmdSnd( wkStrtAddr, 1U, &wkRdDataSecond );
			
			/* ミラー領域と値比較 */
			if( wkRdDataPrim != wkRdDataSecond )
			{
				if( wkRetryCnt == 2U )
				{
					ApiAbn_AbnStsSet( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );		/* 内蔵フラッシュエラー */
				}
			}
			else
			{
#if 0	/* 機器異常復帰させない */
				ApiAbn_AbnStsClr( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );			/* 内蔵Flasエラー解除 */
#endif
				break;
			}
		}
	}
	
	ApiInFlashDrv_Close();
	
	return wkRdDataPrim;
}


/*
 *******************************************************************************
 *	電源投入時のフラグによる各処理
 *
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *	[内容]
 *		内蔵Flashからリードしたフラグを確認し、フラグONなら各処理を実行する
 *******************************************************************************
 */
void ApiInFlash_ParmInitFlgChk( void )
{
	uint8_t wkLoop;
	
	/* 工場出荷初期化フラグあり */
	if( M_TSTBIT( gvInFlash.mParam.mParmFlg, imFactoryInitFlg ) )
	{
		/* 内蔵Flash初期化 */
		ApiInFlash_ParmWrite( ecInFlashWrSts_ParmInit, 0U );
		
		/* 外付けFlash電源ON */
		ApiFlash_FlashPowerCtl( imON, imON );
		
		/* 外付けFlashイレース */
		ApiFlashDrv_ChipErase( ecFlashKind_Prim );
		ApiFlashDrv_ChipErase( ecFlashKind_Second );
	}
	
	/* 収録間隔2は収録間隔1以下の間隔であること */
	if( gvInFlash.mParam.mLogCyc1 < gvInFlash.mParam.mLogCyc2 )
	{
		gvInFlash.mParam.mLogCyc2 = gvInFlash.mParam.mLogCyc1;
	}
	
	/* 警報発生中に電源OFFされたとき、過去警報フラグをONにする */
	for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop++ )
	{
		if( gvInFlash.mData.mAlmPast[ wkLoop ] == ecAlmPastSts_Pre )
		{
			gvInFlash.mData.mAlmPast[ wkLoop ] = ecAlmPastSts_Set;
		}
	}
	
	/* 無線通信間隔初期化 */
	ApiRfDrv_RtStbyOnOff( 2U );
	
	gvFstConnBootCnt = gvInFlash.mData.mMeas1_Num;
}

/*
 *******************************************************************************
 *	設定値書込み
 *
 *	[内容]
 *		高速通信モードで受信したロガー設定値を書き込む
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiInFlash_SetValWrite( ST_RF_Logger_SettingWt_t *arRfBuff )
{
	uint8_t		wkLoop;
	uint8_t		wkU8;
	uint8_t		wkSetVal;
	uint16_t	wkSetData;
	uint16_t	wkNowDataCnt;
	uint16_t	wkNowData;
	
	gvActQue.mNum = 0U;						/* 格納数カウンタクリア */
	gvActQue.mUserID = arRfBuff->mUserID;	/* 設定変更者のID */
	
	for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop++ )
	{
		/* 警報関連の設定CH数制限 */
		if( wkLoop >= cAlmSetChTbl[ gvInFlash.mProcess.mModelCode ].mCh )
		{
			break;
		}
		
		/* 上上限警報値 */
		wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mChSetting[ wkLoop ].mUUAlarmValue[ 0U ] );
		wkNowDataCnt = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviUU[ wkLoop ] );
		gvInFlash.mParam.mDeviUU[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_UUDevi, wkSetData, wkNowDataCnt, gvActQue.mUserID, wkLoop );
		
		/* 上限警報値 */
		wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mChSetting[ wkLoop ].mUAlarmValue[ 0U ] );
		wkNowDataCnt = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviU[ wkLoop ] );
		gvInFlash.mParam.mDeviU[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_UDevi, wkSetData, wkNowDataCnt, gvActQue.mUserID, wkLoop );
		
		/* 下限警報値 */
		wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mChSetting[ wkLoop ].mLAlarmValue[ 0U ] );
		wkNowDataCnt = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviL[ wkLoop ] );
		gvInFlash.mParam.mDeviL[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_LDevi, wkSetData, wkNowDataCnt, gvActQue.mUserID, wkLoop );
		
		/* 下下限警報値 */
		wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mChSetting[ wkLoop ].mLLAlarmValue[ 0U ] );
		wkNowDataCnt = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviLL[ wkLoop ] );
		gvInFlash.mParam.mDeviLL[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_LLDevi, wkSetData, wkNowDataCnt, gvActQue.mUserID, wkLoop );
		
		/* 上上限遅延値 */
		gvInFlash.mParam.mDelayUU[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_UUDelay, arRfBuff->mChSetting[ wkLoop ].mUUAlarmDelay, gvInFlash.mParam.mDelayUU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 上限遅延値 */
		gvInFlash.mParam.mDelayU[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_UDelay, arRfBuff->mChSetting[ wkLoop ].mUAlarmDelay, gvInFlash.mParam.mDelayU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 下限遅延値 */
		gvInFlash.mParam.mDelayL[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LDelay, arRfBuff->mChSetting[ wkLoop ].mLAlarmDelay, gvInFlash.mParam.mDelayL[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 下下限遅延値 */
		gvInFlash.mParam.mDelayLL[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LLDelay, arRfBuff->mChSetting[ wkLoop ].mLLAlarmDelay, gvInFlash.mParam.mDelayLL[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 逸脱許容時間選択閾値 */
		gvInFlash.mParam.mDeviEnaLv[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviEnaLv, arRfBuff->mChSetting[ wkLoop ].mToleranceLevel, gvInFlash.mParam.mDeviEnaLv[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 逸脱許容時間 */
		wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mChSetting[ wkLoop ].mToleranceTime[ 0U ] );
		gvInFlash.mParam.mDeviEnaTime[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviEnaTime, wkSetData, gvInFlash.mParam.mDeviEnaTime[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 計測値オフセット */
		wkNowDataCnt = ApiInFlash_OftVal_ValtoCnt( gvInFlash.mParam.mOffset[ wkLoop ] );			/* 現在値→カウント変換 */
		gvInFlash.mParam.mOffset[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Offset, arRfBuff->mChSetting[ wkLoop ].mMeasureOffset, wkNowDataCnt, gvActQue.mUserID, wkLoop );
		
		/* 計測値一次傾き補正 */
		gvInFlash.mParam.mSlope[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Slope, arRfBuff->mChSetting[ wkLoop ].mMeasureZeroSpan, (gvInFlash.mParam.mSlope[ wkLoop ] - 70), gvActQue.mUserID, wkLoop );
	}
	
	/* 電池種 */
	gvInFlash.mParam.mBatType = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_BatType, arRfBuff->mBattCode, gvInFlash.mParam.mBatType, gvActQue.mUserID, 0U );
	
	/* キーロック */
	gvInFlash.mParam.mKeyLock = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_KeyLock, arRfBuff->mKeylock, gvInFlash.mParam.mKeyLock, gvActQue.mUserID, 0U );
	
	/* 長距離通信無線CH(接続用) */
	gvInFlash.mParam.mrfLoraChGrupeCnt = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_RealTimeCh, arRfBuff->mRtModeCh, gvInFlash.mParam.mrfLoraChGrupeCnt, gvActQue.mUserID, 0U ) - 1U;
	
	/* 収録周期1 */
	wkU8 = gvInFlash.mParam.mLogCyc1;
	gvInFlash.mParam.mLogCyc1 = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LogCyc1, arRfBuff->mRecordIntMain, gvInFlash.mParam.mLogCyc1, gvActQue.mUserID, 0U );
	
	/* 収録周期2(警報監視周期) */
	wkSetVal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LogCyc2, arRfBuff->mRecordIntAlarm, gvInFlash.mParam.mLogCyc2, gvActQue.mUserID, 0U );
	
	/* 収録周期1以下であること、収録周期1と同じ設定なら一緒に変化すること */
	if( gvInFlash.mParam.mLogCyc1 < wkSetVal || (arRfBuff->mRecordIntAlarm == 15 && wkU8 == gvInFlash.mParam.mLogCyc2) )
	{
		wkSetVal = gvInFlash.mParam.mLogCyc1;
	}
	ApiFlash_QueActHist( ecActLogItm_LogCyc2, gvInFlash.mParam.mLogCyc2, wkSetVal, gvActQue.mUserID );								/* 設定変更履歴 */
	gvInFlash.mParam.mLogCyc2 = wkSetVal;
	
	/* ロガー名称 */
	SubInFlash_SetLoggerName( arRfBuff->mLoggerName );
//	for( wkLoop = 0U ; wkLoop < imLoggerNameNum ; wkLoop++ )
//	{
		/* 設定値へ更新 */
//		if( SubInFlash_SetValWrite_ValChk( ecSetParamList_LoggerName, arRfBuff->mLoggerName[ wkLoop ], gvInFlash.mParam.mLoggerName[ wkLoop ], gvActQue.mUserID, 0U ) == 1U )
//		{
//			for( wkLoop = 0U, wkLogItmNo = ecActLogItm_LoggerName1 ; wkLoop < imLoggerNameNum ; wkLoop += 2U )
//			{
//				wkU16 = (gvInFlash.mParam.mLoggerName[ wkLoop ] << 8U) | (gvInFlash.mParam.mLoggerName[ wkLoop + 1U ]);
//				wkSetData = (arRfBuff->mLoggerName[ wkLoop ] << 8U) | (arRfBuff->mLoggerName[ wkLoop  + 1U ]);
				
//				ApiFlash_QueActHist( wkLogItmNo, wkU16, wkSetData, gvActQue.mUserID );												/* 設定変更履歴 */
//				wkLogItmNo++;
				
//				gvInFlash.mParam.mLoggerName[ wkLoop ] = arRfBuff->mLoggerName[ wkLoop ];
//				gvInFlash.mParam.mLoggerName[ wkLoop + 1U ] = arRfBuff->mLoggerName[ wkLoop + 1U ];
//			}
//			break;
//		}
//	}
	
	/* 高速通信接続CH */
	gvInFlash.mParam.mrfHsCh  = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_FstConnCh, arRfBuff->mHsModeCh, gvInFlash.mParam.mrfHsCh, gvActQue.mUserID, 0U ) - 1U;
	
	/* 高速通信起動のログデータ閾値 */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mHSmodeWakeUpLogValue[ 0U ] );
	gvInFlash.mParam.mFstConnBoot = SubInFlash_SetValWrite_ValChk( ecSetParamList_FstConnBoot, wkSetData, gvInFlash.mParam.mFstConnBoot, gvActQue.mUserID, 0U );
	
	/* 警報監視演算種 */
	gvInFlash.mParam.mAlertType = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertType, arRfBuff->mAlarmCulcCode, gvInFlash.mParam.mAlertType, gvActQue.mUserID, 0U );
	
	/* 警報監視演算パラメータ、フラグ記録 */
	/* パラメータ */
	gvInFlash.mParam.mAlertParmFlag = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertParm, (arRfBuff->mAlarmCulcPrmFlag & 0x1FU), (gvInFlash.mParam.mAlertParmFlag & 0x1FU), gvActQue.mUserID, 0U );
	
	/* フラグ */
	wkSetVal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertFlag, ((arRfBuff->mAlarmCulcPrmFlag & 0x20U) >> 5U), ((gvInFlash.mParam.mAlertParmFlag & 0x20U) >> 5U), gvActQue.mUserID, 0U );
	gvInFlash.mParam.mAlertParmFlag += wkSetVal << 5U;
	
	/* イベントボタン制御 */
	wkU8 = gvInFlash.mParam.mEventKeyFlg;
	gvInFlash.mParam.mEventKeyFlg = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_EventKeyFlg, arRfBuff->mEventCtrl, gvInFlash.mParam.mEventKeyFlg, gvActQue.mUserID, 0U );
	if( gvInFlash.mParam.mEventKeyFlg != wkU8 )
	{
		gvModuleSts.mKeyInt = ecKeyIntModuleSts_EventMode;		/* Eventキー1s長押し相当の動作を実行 */
	}
	
	/* 逸脱許容時間クリア手段 */
	gvInFlash.mParam.mDeviClrPoint = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviClrPoint, arRfBuff->mToleranceClarMeth, gvInFlash.mParam.mDeviClrPoint, gvActQue.mUserID, 0U );
	
	/* 逸脱許容時間クリア時刻 */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mToleranceClarTime[ 0U ] );
	wkNowData = (gvInFlash.mParam.mDeviClrTimeHour * 60U) + gvInFlash.mParam.mDeviClrTimeMin;
	wkSetData = SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviClrTime, wkSetData, wkNowData, gvActQue.mUserID, 0U );
	gvInFlash.mParam.mDeviClrTimeHour = wkSetData / 60U;
	gvInFlash.mParam.mDeviClrTimeMin = wkSetData % 60U;
	
	/* 校正日 */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mCalDate[ 0U ] );
	gvInFlash.mParam.mCalDate = SubInFlash_SetValWrite_ValChk( ecSetParamList_CalDate, wkSetData, gvInFlash.mParam.mCalDate, gvActQue.mUserID, 0U );
	
	/* スケーリング変換小数点位置 */
	gvInFlash.mParam.mScalDecimal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_ScalDecimal, arRfBuff->mScalDecimal, gvInFlash.mParam.mScalDecimal, gvActQue.mUserID, 0U );
	
	/* スケーリング変換切片、傾き */
	SubInFlash_SetScale( arRfBuff->mScalYInt, arRfBuff->mScalGrad );
	
	/* プライマリフラッシュメモリ切り替え */
	gvInFlash.mParam.mFlashSelect = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_FlashSelect, arRfBuff->mMemorySelect, gvInFlash.mParam.mFlashSelect, gvActQue.mUserID, 0U );
	
	/* 接続許可ゲートウェイID */
	SubInFlash_SetCommGwID( arRfBuff->mCommGwID );
//	wkLogItmNo = ecActLogItm_CnctEnaGwId1;
//	for( wkLoop = 0U ; wkLoop < 10U ; wkLoop++, wkLogItmNo++ )
//	{
//		for( wkLoop2 = 0U ; wkLoop2 < 3U ; wkLoop2++ )
//		{
			/* 設定値へ更新 */
//			if( SubInFlash_SetValWrite_ValChk( ecSetParamList_CnctEnaGwId, arRfBuff->mCommGwID[ wkLoop ][ wkLoop2 ], gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ], gvActQue.mUserID, 0U ) == 1U )
//			{
//				for( wkLoop2 = 0U, wkCnctId_now = 0U, wkCnctId_set = 0U ; wkLoop2 < 3U ; wkLoop2++ )
//				{
//					wkCnctId_now += gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
//					wkCnctId_set += arRfBuff->mCommGwID[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
					
//					gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] = arRfBuff->mCommGwID[ wkLoop ][ wkLoop2 ];
//				}
//				wkCnctId_now >>= 4U;
//				wkCnctId_set >>= 4U;
				
//				ApiFlash_QueActHist( wkLogItmNo, wkCnctId_now, wkCnctId_set, gvActQue.mUserID );										/* 設定変更履歴 */
//				break;
//			}
//		}
//	}
	
	/* グループID */
	gvInFlash.mParam.mGroupID = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_GrId, arRfBuff->mGroupID, gvInFlash.mParam.mGroupID, gvActQue.mUserID, 0U );
	ApiRfDrv_GrID_Chg();
	
	/* 設定変更時刻を記録 */
	SubInFlash_SetValTime();
	
	gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
}

/*
 *******************************************************************************
 *	長距離通信設定値書込み1
 *
 *	[内容]
 *		長距離通信モードで受信したロガー設定値を書き込む
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiInFlash_RtSetValWrite1( ST_RF_RT_ChangeSet_Prm1_t *arRfBuff )
{
	uint8_t wkKeySet = 0xFFU;
	
	gvActQue.mNum = 0U;						/* 格納数カウンタクリア */
	gvActQue.mUserID = arRfBuff->mUserID;	/* 設定変更者のID */
	
	/* 接続許可ゲートウェイID */
	SubInFlash_SetCommGwID( arRfBuff->mPermitGwID );
//	wkLogItmNo = ecActLogItm_CnctEnaGwId1;
//	for( wkLoop = 0U ; wkLoop < 10U ; wkLoop++, wkLogItmNo++ )
//	{
//		for( wkLoop2 = 0U ; wkLoop2 < 3U ; wkLoop2++ )
//		{
			/* 設定値へ更新 */
//			if( SubInFlash_SetValWrite_ValChk( ecSetParamList_CnctEnaGwId, arRfBuff->mPermitGwID[ wkLoop ][ wkLoop2 ], gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ], gvActQue.mUserID, 0U ) == 1U )
//			{
//				for( wkLoop2 = 0U, wkCnctId_now = 0U, wkCnctId_set = 0U ; wkLoop2 < 3U ; wkLoop2++ )
//				{
//					wkCnctId_now += gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
//					wkCnctId_set += arRfBuff->mPermitGwID[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
					
//					gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] = arRfBuff->mPermitGwID[ wkLoop ][ wkLoop2 ];
//				}
//				wkCnctId_now >>= 4U;
//				wkCnctId_set >>= 4U;
				
//				ApiFlash_QueActHist( wkLogItmNo, wkCnctId_now, wkCnctId_set, gvActQue.mUserID );									/* 設定変更履歴 */
//				break;
//			}
//		}
//	}
	
	/* グループID */
	gvInFlash.mParam.mGroupID = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_GrId, arRfBuff->mGroupID, gvInFlash.mParam.mGroupID, gvActQue.mUserID, 0U );
	ApiRfDrv_GrID_Chg();
	
	/* 電池種 */
	gvInFlash.mParam.mBatType = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_BatType, (arRfBuff->mOnline_Batt_Key & 0xE0U) >> 5U, gvInFlash.mParam.mBatType, gvActQue.mUserID, 0U );
	
	/* キーロック */
	if( !(arRfBuff->mOnline_Batt_Key & 0x10U) )
	{
		wkKeySet = arRfBuff->mOnline_Batt_Key & 0x1FU;
	}
	gvInFlash.mParam.mKeyLock = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_KeyLock, wkKeySet, gvInFlash.mParam.mKeyLock, gvActQue.mUserID, 0U );
}

/*
 *******************************************************************************
 *	長距離通信設定値書込み2
 *
 *	[内容]
 *		長距離通信モードで受信したロガー設定値を書き込む
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiInFlash_RtSetValWrite2( ST_RF_RT_ChangeSet_Prm2_t *arRfBuff )
{
	uint8_t		wkLoop;
	
	/* 長距離通信無線CH(接続用) */
	gvInFlash.mParam.mrfLoraChGrupeCnt = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_RealTimeCh, arRfBuff->mRtModeCh, gvInFlash.mParam.mrfLoraChGrupeCnt, gvActQue.mUserID, 0U ) - 1U;
	
	for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop++ )
	{
		/* 警報関連の設定CH数制限 */
		if( wkLoop >= cAlmSetChTbl[ gvInFlash.mProcess.mModelCode ].mCh )
		{
			break;
		}
		
		/* 上上限警報値 */
		gvInFlash.mParam.mDeviUU[ wkLoop ] = SubInFlash_UpdateAlmVal( ecSetParamList_UUDevi, &arRfBuff->mUUAlarmValue[ 0U ], gvInFlash.mParam.mDeviUU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 上限警報値 */
		gvInFlash.mParam.mDeviU[ wkLoop ] = SubInFlash_UpdateAlmVal( ecSetParamList_UDevi, &arRfBuff->mUAlarmValue[ 0U ], gvInFlash.mParam.mDeviU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 下限警報値 */
		gvInFlash.mParam.mDeviL[ wkLoop ] = SubInFlash_UpdateAlmVal( ecSetParamList_LDevi, &arRfBuff->mLAlarmValue[ 0U ], gvInFlash.mParam.mDeviL[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 下下限警報値 */
		gvInFlash.mParam.mDeviLL[ wkLoop ] = SubInFlash_UpdateAlmVal( ecSetParamList_LLDevi, &arRfBuff->mLLAlarmValue[ 0U ], gvInFlash.mParam.mDeviLL[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 上上限遅延値 */
		gvInFlash.mParam.mDelayUU[ wkLoop ] = SubInFlash_UpdateAlmDelay( ecSetParamList_UUDelay, &arRfBuff->mUUAlarmDelay[ 0U ], gvInFlash.mParam.mDelayUU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 上限遅延値 */
		gvInFlash.mParam.mDelayU[ wkLoop ] = SubInFlash_UpdateAlmDelay( ecSetParamList_UDelay, &arRfBuff->mUAlarmDelay[ 0U ], gvInFlash.mParam.mDelayU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 下限遅延値 */
		gvInFlash.mParam.mDelayL[ wkLoop ] = SubInFlash_UpdateAlmDelay( ecSetParamList_LDelay, &arRfBuff->mLAlarmDelay[ 0U ], gvInFlash.mParam.mDelayL[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* 下下限遅延値 */
		gvInFlash.mParam.mDelayLL[ wkLoop ] = SubInFlash_UpdateAlmDelay( ecSetParamList_LLDelay, &arRfBuff->mLLAlarmDelay[ 0U ], gvInFlash.mParam.mDelayLL[ wkLoop ], gvActQue.mUserID, wkLoop );
	}
}



/*
 *******************************************************************************
 *	長距離通信設定値書込み3
 *
 *	[内容]
 *		長距離通信モードで受信したロガー設定値を書き込む
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiInFlash_RtSetValWrite3( ST_RF_RT_ChangeSet_Prm3_t *arRfBuff )
{
	uint8_t		wkLoop;
	uint8_t		wkU8;
	uint8_t		wkSetVal;
	uint16_t	wkSetData;
	uint16_t	wkNowData;
	uint16_t	wkNowDataCnt;
	
	for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop++ )
	{
		/* 設定CH数制限 */
		if( wkLoop >= cAlmSetChTbl[ gvInFlash.mProcess.mModelCode ].mCh )
		{
			break;
		}
		
		/* 逸脱許容時間選択閾値 */
		wkSetData = (arRfBuff->mToleranceTime[ 2U ] << 4U) + ((arRfBuff->mToleranceTime[ 1U ] & 0xF0U) >> 4U);
		gvInFlash.mParam.mDeviEnaLv[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviEnaLv, wkSetData, gvInFlash.mParam.mDeviEnaLv[ wkLoop ], gvActQue.mUserID, 0U );
		
		/* 逸脱許容時間 */
		wkSetData = ((arRfBuff->mToleranceTime[ 1U ] & 0x0FU) << 8U) + arRfBuff->mToleranceTime[ 0U ];
		gvInFlash.mParam.mDeviEnaTime[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviEnaTime, wkSetData, gvInFlash.mParam.mDeviEnaTime[ wkLoop ], gvActQue.mUserID, 0U );
		
		/* 計測値オフセット */
		wkNowDataCnt = ApiInFlash_OftVal_ValtoCnt( gvInFlash.mParam.mOffset[ wkLoop ] );
		gvInFlash.mParam.mOffset[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Offset, arRfBuff->mMeasureOffset[ wkLoop ], wkNowDataCnt, gvActQue.mUserID, 0U );
		
		/* 計測値一次傾き補正 */
		gvInFlash.mParam.mSlope[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Slope, arRfBuff->mMeasureZeroSpan[ wkLoop ], (gvInFlash.mParam.mSlope[ wkLoop ] - 70), gvActQue.mUserID, 0U );
	}
	
	/* 逸脱許容時間クリア手段_0:イベントボタン、1：設定時刻 */
	wkSetData = 1U;
	if( arRfBuff->mToleranceTime[ 3U ] == 0xFFU )
	{
		wkSetData = 0U;
	}
	gvInFlash.mParam.mDeviClrPoint = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviClrPoint, wkSetData, gvInFlash.mParam.mDeviClrPoint, gvActQue.mUserID, 0U );
	
	/* 逸脱許容時間クリア時刻 */
	wkSetData = ((arRfBuff->mToleranceTime[ 3U ] & 0xF8U) >> 3U) * 60U + (arRfBuff->mToleranceTime[ 3U ] & 0x07U);
	wkNowData = (gvInFlash.mParam.mDeviClrTimeHour * 60U) + gvInFlash.mParam.mDeviClrTimeMin;
	wkSetData = SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviClrTime, wkSetData, wkNowData, gvActQue.mUserID, 0U );
	gvInFlash.mParam.mDeviClrTimeHour = wkSetData / 60U;
	gvInFlash.mParam.mDeviClrTimeMin = wkSetData % 60U;
	
	/* 収録周期1 */
	wkU8 = gvInFlash.mParam.mLogCyc1;
	gvInFlash.mParam.mLogCyc1 = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LogCyc1, arRfBuff->mRecordInt & 0x0FU, gvInFlash.mParam.mLogCyc1, gvActQue.mUserID, 0U );
	
	/* 収録周期2(警報監視周期) */
	wkSetVal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LogCyc2, arRfBuff->mRecordInt >> 4U, gvInFlash.mParam.mLogCyc2, gvActQue.mUserID, 0U );
	
	/* 収録周期1以下であること、収録周期1と同じ設定なら一緒に変化すること */
	if( gvInFlash.mParam.mLogCyc1 < wkSetVal || (arRfBuff->mRecordInt == 0xF0U && wkU8 == gvInFlash.mParam.mLogCyc2) )
	{
		wkSetVal = gvInFlash.mParam.mLogCyc1;
	}
	ApiFlash_QueActHist( ecActLogItm_LogCyc2, gvInFlash.mParam.mLogCyc2, wkSetVal, gvActQue.mUserID );
	gvInFlash.mParam.mLogCyc2 = wkSetVal;
	
//	for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop++ )
//	{
		/* 設定CH数制限 */
//		if( wkLoop >= cAlmSetChTbl[ gvInFlash.mProcess.mModelCode ].mCh )
//		{
//			break;
//		}
		
		/* 計測値オフセット */
//		wkNowDataCnt = ApiInFlash_OftVal_ValtoCnt( gvInFlash.mParam.mOffset[ wkLoop ] );										/* 現在値→カウント変換 */
//		gvInFlash.mParam.mOffset[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Offset, arRfBuff->mMeasureOffset[ wkLoop ], wkNowDataCnt, gvActQue.mUserID, 0U );
		
		/* 計測値一次傾き補正 */
//		gvInFlash.mParam.mSlope[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Slope, arRfBuff->mMeasureZeroSpan[ wkLoop ], (gvInFlash.mParam.mSlope[ wkLoop ] - 70), gvActQue.mUserID, 0U );
//	}
	
	/* ロガー名称 */
	SubInFlash_SetLoggerName( arRfBuff->mLoggerName );
//	for( wkLoop = 0U ; wkLoop < imLoggerNameNum ; wkLoop++ )
//	{
		/* 設定値へ更新 */
//		if( SubInFlash_SetValWrite_ValChk( ecSetParamList_LoggerName, arRfBuff->mLoggerName[ wkLoop ], gvInFlash.mParam.mLoggerName[ wkLoop ], gvActQue.mUserID, 0U ) == 1U )
//		{
//			for( wkLoop = 0U, wkLogItmNo = ecActLogItm_LoggerName1 ; wkLoop < imLoggerNameNum ; wkLoop++ )
//			{
//				wkU16 = (gvInFlash.mParam.mLoggerName[ wkLoop ] << 8U) | (gvInFlash.mParam.mLoggerName[ wkLoop + 1U ]);
//				wkSetData = (arRfBuff->mLoggerName[ wkLoop ] << 8U) | (arRfBuff->mLoggerName[ wkLoop + 1U ]);
				
//				ApiFlash_QueActHist( wkLogItmNo, wkU16, wkSetData, gvActQue.mUserID );												/* 設定変更履歴 */
//				wkLogItmNo++;
				
//				gvInFlash.mParam.mLoggerName[ wkLoop ] = arRfBuff->mLoggerName[ wkLoop ];
//				gvInFlash.mParam.mLoggerName[ wkLoop + 1U ] = arRfBuff->mLoggerName[ wkLoop + 1U ];
//			}
//			break;
//		}
//	}
	
	/* 高速通信待ち受けch */
	gvInFlash.mParam.mrfHsCh = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_FstConnCh, arRfBuff->mHSmodeCh & 0x0FU, gvInFlash.mParam.mrfHsCh, gvActQue.mUserID, 0U ) - 1U;
	
	/* スケーリング変換小数点位置 */
	gvInFlash.mParam.mScalDecimal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_ScalDecimal, ((arRfBuff->mHSmodeCh & 0xF0U) >> 4U), gvInFlash.mParam.mScalDecimal, gvActQue.mUserID, 0U );
	
	/* 高速通信起動のログデータ閾値 */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mHSmodeWakeUpLogValue[ 0U ] );
	gvInFlash.mParam.mFstConnBoot = SubInFlash_SetValWrite_ValChk( ecSetParamList_FstConnBoot, wkSetData, gvInFlash.mParam.mFstConnBoot, gvActQue.mUserID, 0U );
	
	/* 警報監視演算パラメータ */
	gvInFlash.mParam.mAlertParmFlag = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertParm, (arRfBuff->mAlarmCulcSet[ 0U ]), (gvInFlash.mParam.mAlertParmFlag & 0x1FU), gvActQue.mUserID, 0U );
	
	/* 警報監視演算種 */
	gvInFlash.mParam.mAlertType = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertType, arRfBuff->mAlarmCulcSet[ 1U ], gvInFlash.mParam.mAlertType, gvActQue.mUserID, 0U );
	
	/* 警報監視演算フラグ記録 */
	wkSetVal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertFlag, (arRfBuff->mAlCulcFlag_EventCont_MemorySet & 0x02U), ((gvInFlash.mParam.mAlertParmFlag & 0x20U) >> 5U), gvActQue.mUserID, 0U );
	gvInFlash.mParam.mAlertParmFlag += wkSetVal << 5U;
	
	/* イベントボタン制御 */
	wkU8 = gvInFlash.mParam.mEventKeyFlg;
	gvInFlash.mParam.mEventKeyFlg = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_EventKeyFlg, ((arRfBuff->mAlCulcFlag_EventCont_MemorySet & 0x0CU) >> 2U), gvInFlash.mParam.mEventKeyFlg, gvActQue.mUserID, 0U );
	if( gvInFlash.mParam.mEventKeyFlg != wkU8 )
	{
		gvModuleSts.mKeyInt = ecKeyIntModuleSts_EventMode;		/* Eventキー1s長押し相当の動作を実行 */
	}
	
	/* プライマリフラッシュメモリ切り替え */
	wkSetVal = (arRfBuff->mAlCulcFlag_EventCont_MemorySet & 0x30U) >> 4U;
	/* 長距離通信での値継続3 → 高速通信での値継続15に変換 */
	if( wkSetVal == 3U )
	{
		wkSetVal = 15U;
	}
	gvInFlash.mParam.mFlashSelect = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_FlashSelect, wkSetVal, gvInFlash.mParam.mFlashSelect, gvActQue.mUserID, 0U );
	
	/* スケーリング変換切片、傾き */
	SubInFlash_SetScale( arRfBuff->mCalOffset, arRfBuff->mCalZeroSpan );
	
	/* 設定変更時刻を記録 */
	SubInFlash_SetValTime();
	
	gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
}


/*
 *******************************************************************************
 *	設定値書込み(逸脱値設定のサブ関数)
 *
 *	[内容]
 *		無線バッファから逸脱値をパースして設定値への変換、履歴記録する
 *	[引数]
 *		
 *	[戻値]
 *		
 *******************************************************************************
 */
static sint16_t SubInFlash_UpdateAlmVal( ET_SetParamList_t arParmList, uint8_t *parSetVal, sint16_t arNowVal, uint8_t arUserID, uint8_t arLoop )
{
	sint16_t	wkRet;
	uint16_t 	wkSetVal;
	
	switch( arLoop )
	{
		default:
		case 0U:
			/* 上上限逸脱値1CH */
			wkSetVal = ((parSetVal[ 1U ] & 0x1FU) << 8U) + parSetVal[ 0U ];
			break;
		case 1U:
			/* 上上限逸脱値2CH */
			wkSetVal = ((parSetVal[ 3U ] & 0x03U) << 11U) + (parSetVal[ 2U ] << 3U) + (parSetVal[ 1U ] >> 5U);
			break;
		case 2U:
			/* 下限逸脱値3CH */
			wkSetVal = ((parSetVal[ 4U ] & 0x7FU) << 6U) + (parSetVal[ 3U ] >> 2U);
			break;
		case 3U:
			/* 下下限逸脱値4CH */
			wkSetVal = ((parSetVal[ 6U ] & 0x0FU) << 9U) + (parSetVal[ 5U ] << 1U) + (parSetVal[ 4U ] >> 7U);
			break;
	}
	
	arNowVal = ApiInFlash_AlmVal_ValtoCnt( arNowVal );
	wkRet = (sint16_t)SubInFlash_SetValWrite_ValChk( arParmList, wkSetVal, arNowVal, arUserID, arLoop );
	
	return wkRet;
}

/*
 *******************************************************************************
 *	設定値書込み(逸脱遅延回数設定のサブ関数)
 *
 *	[内容]
 *		無線バッファから逸脱遅延回数をパースして設定値への変換、履歴記録する
 *	[引数]
 *		
 *	[戻値]
 *		sint16_t	wkS16: 採用値
 *******************************************************************************
 */
static uint8_t SubInFlash_UpdateAlmDelay( ET_SetParamList_t arParmList, uint8_t *parSetVal, uint8_t arNowVal, uint8_t arUserID, uint8_t arLoop )
{
	uint8_t wkSetVal;
	
	switch( arLoop )
	{
		case 0U:
			/* 上上限遅延回数1CH */
			wkSetVal = parSetVal[ 0U ] & 0x3FU;
			break;
		case 1U:
			/* 上上限遅延回数2CH */
			wkSetVal = ((parSetVal[ 1U ] & 0x3CU) << 2U) + (parSetVal[ 0U ] >> 6U);
			break;
		case 2U:
			/* 下限遅延回数3CH */
			wkSetVal = ((parSetVal[ 2U ] & 0x03U) << 4U) + (parSetVal[ 1U ] >> 4U);
			break;
		case 3U:
			/* 下下限遅延回数4CH */
			wkSetVal = (parSetVal[ 2U ] & 0x0FU) >> 2U;
			break;
	}
	
	wkSetVal = (uint8_t)SubInFlash_SetValWrite_ValChk( arParmList, wkSetVal, arNowVal, arUserID, arLoop );
	
	return wkSetVal;
}


/*
 *******************************************************************************
 *	設定値書込み(現在値と無線設定値を比較する)
 *
 *	[内容]
 *		設定値を確認し、設定値又は現在値を返し、履歴記載
 *		※制限範囲外、かつ設定値継続でない場合、現在値を返す
 *	[引数]
 *		ET_SetParamList_t	arParmList: 設定変更のパラメータ
 *		uint16_t			arSetVal: 設定値
 *		uint16_t			arNowVal: 現在の設定値
 *	[戻値]
 *		uint16_t	wkVal: 採用値
 *******************************************************************************
 */
static uint16_t SubInFlash_SetValWrite_ValChk( ET_SetParamList_t arParmList, uint16_t arSetVal, uint16_t arNowVal, uint8_t arUser, uint8_t arLoop )
{
	uint8_t		wkLoop;
	sint16_t	wkS16;
	sint16_t	wkUpVal;
	sint16_t	wkDwVal;
	uint16_t	wkVal = 0U;
	
	ET_ActLogItm_t wkLogItmNo;
	
	/* 設定値制限リストから設定変更する値を確定する */
	for( wkLoop = 0U ; wkLoop < M_ArrayElement( cSetValWriteLimitTbl ) ; wkLoop++ )
	{
		if( cSetValWriteLimitTbl[ wkLoop ].mParam == arParmList )
		{
			break;
		}
	}
	
	/* 警報履歴No. */
	wkLogItmNo = cSetValWriteLimitTbl[ wkLoop ].mLogItm;
	
	switch( arParmList )
	{
		/* ロガー名称/接続許可GW */
		case ecSetParamList_LoggerName:
		case ecSetParamList_CnctEnaGwId:
			/* 全領域中、継続値でないのが含まれていたら設定値へ更新 */
			if( arSetVal != cSetValWriteLimitTbl[ wkLoop ].mKeep )
			{
				wkVal = 1U;			/* 設定値へ更新 */
			}
			break;
			
		default:
			/* 現在値を一時的に格納 */
			wkVal = arNowVal;
			
			/* 設定値制限 */
			if( arSetVal >= cSetValWriteLimitTbl[ wkLoop ].mMin && arSetVal <= cSetValWriteLimitTbl[ wkLoop ].mMax )
			{
				wkVal = arSetVal;
			}
			
			/* 現在の設定値を継続 */
			if( arSetVal == cSetValWriteLimitTbl[ wkLoop ].mKeep )
			{
				wkVal = arNowVal;
			}
			break;
	}
	
	switch( arParmList )
	{
		case ecSetParamList_UUDevi:
		case ecSetParamList_UDevi:
		case ecSetParamList_LDevi:
		case ecSetParamList_LLDevi:
			/* カウント→実数変換 */
			wkS16 = SubInFlash_AlmVal_CnttoVal( wkVal );
			arNowVal = SubInFlash_AlmVal_CnttoVal( arNowVal );
			
			/* モデルごとの逸脱値制限 */
			switch( gvInFlash.mProcess.mModelCode )
			{
				/* 内蔵サーミスタ */
				case ecSensType_InTh:
					wkUpVal = imAlmValUp_InThModel;
					wkDwVal = imAlmValDw_InThModel;
					break;
				/* 外付けサーミスタ */
				case ecSensType_ExTh:
					wkUpVal = imAlmValUp_ExThModel;
					wkDwVal = imAlmValDw_ExThModel;
					break;
#if (swSensorCom == imEnable)
				/* 温湿度 */
				case ecSensType_HumTmp:
					if( arLoop == 0U )
					{
						wkUpVal = imAlmValUp_HumTmpModel_Tmp;
						wkDwVal = imAlmValDw_HumTmpModel_Tmp;
					}
					else
					{
						wkUpVal = imAlmValUp_HumTmpModel_Hum;
						wkDwVal = imAlmValDw_HumTmpModel_Hum;
					}
					break;
				/* Pt */
				case ecSensType_Pt:
					wkUpVal = imAlmValUp_PtModel;
					wkDwVal = imAlmValDw_PtModel;
					break;
				/* 電圧 */
				case ecSensType_V:
					wkUpVal = imAlmValUp_VolModel;
					wkDwVal = imAlmValDw_VolModel;
					break;
				/* パルス */
				case ecSensType_Pulse:
					wkUpVal = imAlmValUp_PulseModel;
					wkDwVal = imAlmValDw_PulseModel;
					break;
#endif
			}
			
			if( wkS16 != M_SINT16_MIN )
			{
				if( wkS16 < wkDwVal )
				{
					wkS16 = wkDwVal;
				}
				else if( wkS16 > wkUpVal )
				{
					wkS16 = wkUpVal;
				}
			}
			wkVal = (uint16_t)wkS16;
			
			wkLogItmNo += (12U * arLoop);
			break;
			
		case ecSetParamList_UUDelay:
		case ecSetParamList_UDelay:
		case ecSetParamList_LDelay:
		case ecSetParamList_LLDelay:
		case ecSetParamList_DeviEnaLv:
		case ecSetParamList_DeviEnaTime:
		case ecSetParamList_Offset:
		case ecSetParamList_Slope:
			wkLogItmNo += (12U * arLoop);
			if( arParmList == ecSetParamList_Offset )
			{
				wkVal = SubInFlash_OftVal_CnttoVal( wkVal );	/* カウント→実数変換 */
				arNowVal = SubInFlash_OftVal_CnttoVal( arNowVal );
			}
			else if( arParmList == ecSetParamList_Slope )
			{
				wkVal += 70;
				arNowVal += 70;
			}
			break;
			
		case ecSetParamList_RealTimeCh:
		case ecSetParamList_FstConnCh:
			wkVal += 1U;
			arNowVal += 1U;
			break;
			
		case ecSetParamList_ScalYInt:
		case ecSetParamList_ScalGrad:
			wkVal = SubInFlash_ScaleVal_CnttoVal( wkVal );		/* カウント→実数変換 */
			if( arParmList == ecSetParamList_ScalYInt )
			{
				arNowVal = gvInFlash.mParam.mScalYInt;
			}
			else
			{
				arNowVal = gvInFlash.mParam.mScalGrad;
			}
			break;
			
		/* 履歴記録はこの関数を抜けてから行う(収録周期2、ロガー名称、接続許可受信器) */
		case ecSetParamList_LogCyc2:
		case ecSetParamList_LoggerName:
		case ecSetParamList_CnctEnaGwId:
			return wkVal;
	}
	
	/* 設定変更履歴 */
	ApiFlash_QueActHist( wkLogItmNo, (uint32_t)arNowVal, (uint32_t)wkVal, arUser );
	
	return wkVal;
}


/*
 *******************************************************************************
 *	ロガー名称設定
 *
 *	[内容]
 *		ロガー名称設定
 *	[引数]
 *		uint8_t *ptrLoggerName: 無線で受信したロガー名称
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void SubInFlash_SetLoggerName( uint8_t *ptrLoggerName )
{
	uint8_t		wkLoop;
	uint16_t	wkU16;
	uint16_t	wkSetData;
	ET_ActLogItm_t	wkLogItmNo;

	/* ロガー名称 */
	for( wkLoop = 0U ; wkLoop < imLoggerNameNum ; wkLoop++ )
	{
		/* 設定値へ更新 */
		if( SubInFlash_SetValWrite_ValChk( ecSetParamList_LoggerName, ptrLoggerName[ wkLoop ], gvInFlash.mParam.mLoggerName[ wkLoop ], gvActQue.mUserID, 0U ) == 1U )
		{
			for( wkLoop = 0U, wkLogItmNo = ecActLogItm_LoggerName1 ; wkLoop < imLoggerNameNum ; wkLoop +=2 )
			{
				wkU16 = (gvInFlash.mParam.mLoggerName[ wkLoop ] << 8U) | (gvInFlash.mParam.mLoggerName[ wkLoop + 1U ]);
				wkSetData = (ptrLoggerName[ wkLoop ] << 8U) | (ptrLoggerName[ wkLoop + 1U ]);
				
				ApiFlash_QueActHist( wkLogItmNo, wkU16, wkSetData, gvActQue.mUserID );												/* 設定変更履歴 */
				wkLogItmNo++;
				
				gvInFlash.mParam.mLoggerName[ wkLoop ] = ptrLoggerName[ wkLoop ];
				gvInFlash.mParam.mLoggerName[ wkLoop + 1U ] = ptrLoggerName[ wkLoop + 1U ];
			}
			break;
		}
	}
}


/*
 *******************************************************************************
 *	接続許可受信器設定
 *
 *	[内容]
 *		接続許可受信器設定
 *	[引数]
 *		uint8_t *ptrCommGwID[3]: 無線で受信した接続許可GWID
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void SubInFlash_SetCommGwID( uint8_t (*ptrCommGwID)[3] )
{
	uint8_t		wkLoop;
	uint8_t		wkLoop2;
	uint32_t	wkCnctId_set;
	uint32_t	wkCnctId_now;
	ET_ActLogItm_t	wkLogItmNo;
	
	/* 接続許可ゲートウェイID */
	wkLogItmNo = ecActLogItm_CnctEnaGwId1;
	for( wkLoop = 0U ; wkLoop < 10U ; wkLoop++, wkLogItmNo++ )
	{
		for( wkLoop2 = 0U ; wkLoop2 < 3U ; wkLoop2++ )
		{
			/* 設定値へ更新 */
			if( SubInFlash_SetValWrite_ValChk( ecSetParamList_CnctEnaGwId, ptrCommGwID[ wkLoop ][ wkLoop2 ], gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ], gvActQue.mUserID, 0U ) == 1U )
			{
				for( wkLoop2 = 0U, wkCnctId_now = 0U, wkCnctId_set = 0U ; wkLoop2 < 3U ; wkLoop2++ )
				{
					wkCnctId_now += (uint32_t)gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
					wkCnctId_set += (uint32_t)ptrCommGwID[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
					
					gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] = ptrCommGwID[ wkLoop ][ wkLoop2 ];
				}
				wkCnctId_now >>= 4U;
				wkCnctId_set >>= 4U;
				
				ApiFlash_QueActHist( wkLogItmNo, wkCnctId_now, wkCnctId_set, gvActQue.mUserID );										/* 設定変更履歴 */
				break;
			}
		}
	}
}


/*
 *******************************************************************************
 *	スケーリング設定
 *
 *	[内容]
 *		スケーリング設定
 *	[引数]
 *		uint8_t *ptrOffset: 無線で受信したスケーリングオフセット
 *		uint8_t *ptrZerospan: ゼロスパン
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void SubInFlash_SetScale( uint8_t *ptrOffset, uint8_t *ptrZerospan )
{
	uint16_t	wkSetData;
	uint16_t	wkNowDataCnt;
	
	/* スケーリング変換切片 */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &ptrOffset[ 0U ] );
	wkNowDataCnt = ApiInFlash_ScaleVal_ValtoCnt( gvInFlash.mParam.mScalYInt );
	gvInFlash.mParam.mScalYInt = SubInFlash_SetValWrite_ValChk( ecSetParamList_ScalYInt, wkSetData, wkNowDataCnt, gvActQue.mUserID, 0U );
	
	/* スケーリング変換傾き */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &ptrZerospan[ 0U ] );
	wkNowDataCnt = ApiInFlash_ScaleVal_ValtoCnt( gvInFlash.mParam.mScalGrad );
	gvInFlash.mParam.mScalGrad = SubInFlash_SetValWrite_ValChk( ecSetParamList_ScalGrad, wkSetData, wkNowDataCnt, gvActQue.mUserID, 0U );
}


/*
 *******************************************************************************
 *	設定変更時刻を記録
 *
 *	[内容]
 *		設定変更時刻を記録
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
static void SubInFlash_SetValTime( void )
{
	uint32_t wkU32;
	
	/* 設定変更時刻を記録 */
	wkU32 = ApiTimeComp_GetLocalTime();
	
	gvInFlash.mParam.mSetvalChgTime[ 0U ] = (uint8_t)((wkU32 >> 24U) & 0x000000FFU);
	gvInFlash.mParam.mSetvalChgTime[ 1U ] = (uint8_t)((wkU32 >> 16U) & 0x000000FFU);
	gvInFlash.mParam.mSetvalChgTime[ 2U ] = (uint8_t)((wkU32 >> 8U) & 0x000000FFU);
	gvInFlash.mParam.mSetvalChgTime[ 3U ] = (uint8_t)(wkU32 & 0x000000FFU);
}

/*
 *******************************************************************************
 *	警報値変換(カウンタ→実数)
 *
 *	[内容]
 *		警報値変換
 *			10〜8191cnt→(-215.0〜603.1℃)
 *			それ以外(-32768：警報なし)
 *	[引数]
 *		uint16_t	arAlmValCnt: 警報値設定カウント
 *	[戻値]
 *		sint16_t	wkAlmVal: 警報値設定値
 *******************************************************************************
 */
static sint16_t SubInFlash_AlmVal_CnttoVal( uint16_t arAlmValCnt )
{
	sint16_t wkAlmVal = M_SINT16_MIN;
	
	/* 10〜8191cnt(-215.0〜603.1℃) */
	if( arAlmValCnt >= 10U && arAlmValCnt <= 8191 )
	{
		if( gvInFlash.mProcess.mModelCode == ecSensType_V ||
			gvInFlash.mProcess.mModelCode == ecSensType_Pulse )
		{
			wkAlmVal = arAlmValCnt - 50;
		}
		else
		{
			wkAlmVal = arAlmValCnt - 2160;
		}
	}
	
	return wkAlmVal;
}

/*
 *******************************************************************************
 *	オフセット値変換(実数→カウンタ)
 *
 *	[内容]
 *		オフセット値変換
 *			-12.5〜12.5℃→(0〜250cnt)
 *	[引数]
 *		sint8_t		arOftVal: オフセット値設定値
 *	[戻値]
 *		uint8_t		wkOftValCnt: オフセット設定カウント
 *******************************************************************************
 */
uint8_t ApiInFlash_OftVal_ValtoCnt( sint8_t arOftVal )
{
	/* -12.5〜12.5℃(0〜250cnt) */
	return (uint8_t)(arOftVal + 125);
}

/*
 *******************************************************************************
 *	オフセット値変換(カウンタ→実数)
 *
 *	[内容]
 *		オフセット値変換
 *			0〜250cnt→(-12.5〜12.5℃)
 *	[引数]
 *		uint8_t		arOftValCnt: オフセット設定カウント
 *	[戻値]
 *		sint8_t		wkOftVal: オフセット値設定値
 *******************************************************************************
 */
static sint8_t SubInFlash_OftVal_CnttoVal( uint8_t arOftValCnt )
{
	/* 0〜250cnt(-12.5〜12.5℃) */
	return (sint8_t)(arOftValCnt - 125);
}

/*
 *******************************************************************************
 *	スケーリング値変換(カウンタ→実数)
 *
 *	[内容]
 *		スケーリング値変換
 *			0〜60000cnt→(-30000〜30000)
 *	[引数]
 *		uint16_t	arScaleValCnt: スケーリング設定カウント
 *	[戻値]
 *		sint16_t	wkScaleVal: スケーリング値設定値
 *******************************************************************************
 */
static sint16_t SubInFlash_ScaleVal_CnttoVal( uint16_t arScaleValCnt )
{
	/* 0〜60000cnt(-30000〜30000) */
	return (sint16_t)(arScaleValCnt - 30000);
}

/*
 *******************************************************************************
 *	警報値変換(実数→カウンタ)
 *
 *	[内容]
 *		警報値変換
 *			-215.0〜603.1℃→(10〜8191cnt)
 *			-32768(警報なし)→0cnt
 *	[引数]
 *		sint16_t	arAlmVal: 警報値設定値
 *	[戻値]
 *		uint16_t	wkAlmValCnt: 警報値設定カウント
 *******************************************************************************
 */
uint16_t ApiInFlash_AlmVal_ValtoCnt( sint16_t arAlmVal )
{
	uint16_t wkAlmValCnt = 0U;
	
	/* -215.0〜603.1℃(10〜8191cnt) */
	if( arAlmVal != M_SINT16_MIN )
	{
		if( gvInFlash.mProcess.mModelCode == ecSensType_V ||
			gvInFlash.mProcess.mModelCode == ecSensType_Pulse )
		{
			wkAlmValCnt = arAlmVal + 50;
		}
		else
		{
			wkAlmValCnt = arAlmVal + 2160;
		}
	}
	
	return wkAlmValCnt;
}

/*
 *******************************************************************************
 *	スケーリング値変換(実数→カウンタ)
 *
 *	[内容]
 *		スケーリング値変換
 *			-30000〜30000→(0〜60000cnt)
 *	[引数]
 *		sint16_t	arScaleVal: スケーリング値設定値
 *	[戻値]
 *		uint16_t	wkScaleValCnt: スケーリング設定カウント
 *******************************************************************************
 */
uint16_t ApiInFlash_ScaleVal_ValtoCnt( sint16_t arScaleVal )
{
	/* -30000〜30000(0〜60000cnt) */
	return (uint16_t)(30000 + arScaleVal);
}

/*
 *******************************************************************************
 *	1byte配列データをBigエンディアンの2byteに変換
 *
 *	[内容]
 *		1byte配列データをBigエンディアンの2byteに変換
 *	[引数]
 *		uint8_t *parRfData		:1バイトデータ先頭アドレス
 *	[戻値]
 *		uint16_t wkRet			:2バイトデータ
 *******************************************************************************
 */
uint16_t ApiInFlash_1ByteToBig2Byte( uint8_t *parRfData )
{
	uint16_t	wkRet;
	
	wkRet = *parRfData << 8U;
	*parRfData++;
	wkRet |= *parRfData;
	
	return wkRet;
}


/*
 *******************************************************************************
 *	1byte配列データをlittleエンディアンの2byteに変換
 *
 *	[内容]
 *		1byte配列データをlittleエンディアンの2byteに変換
 *	[引数]
 *		uint8_t *parRfData		:1バイトデータ先頭アドレス
 *	[戻値]
 *		uint16_t wkRet			:2バイトデータ
 *******************************************************************************
 */
uint16_t ApiInFlash_1ByteToLittle2Byte( uint8_t *parRfData )
{
	uint16_t	wkRet;
	
	wkRet = *parRfData;
	*parRfData++;
	wkRet |= *parRfData << 8U;
	
	return wkRet;
}

#pragma section

