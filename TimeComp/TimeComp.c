/*
 *******************************************************************************
 *	File name	:	TimeComp.c
 *
 *	[内容]
 *		時刻補正処理
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2018.05.15		Softex K.U		新規作成
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



/*
 *==============================================================================
 *	define定義
 *==============================================================================
 */

#define imTimCompLimTime	7200U		/* 時刻補正の制限は±1時間未満(0.5秒分解能) */

#define imTimCompMaxCnt		16383U		/* 14bit時刻補正の最大カウント数 */

#define imClock_32bitSize	0U			/* 32bit時刻サイズ */
#define imClock_14bitSize	1U			/* 14bit時刻サイズ */



 /*
 *==============================================================================
 *	列挙型定義
 *==============================================================================
 */
typedef enum ET_MeaSts
{
	ecMeaSts_Init = 0,														/* 初期 */
	ecMeaSts_Meas,															/* 測定 */
	ecMeaSts_Log1,															/* 収録1 */
	ecMeaSts_Log2,															/* 収録2 */

	ecMeaStsMax																/* enum最大値 */
} ET_MeaSts_t;



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
static uint8_t vTimeCompFlg = imOFF;

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
static void SubTimeComp_MeaExeChk( ST_RTC_t arClock );
static uint32_t SubTimeComp_GetEvtRecTime( ST_RTC_t arClock, uint8_t arSel );
static ET_MeaSts_t	SubRtcIntMeas1( ST_RTC_t arClock );
static ET_MeaSts_t	SubRtcIntMeas2( ST_RTC_t arClock );
static void SubTimeComp_Initial( void );
//static void SubTimeComp_Delay( void );									/* 内蔵RTC遅れ補正処理 */
//static void SubTimeComp_Fast( void );									/* 内蔵RTC進み補正処理 */
static void SubTimeComp_DelayFast( void );
static uint8_t	SubTimeComp_SurPlusJudgeMeas1( ST_RTC_t arClock );


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	RTC割込み処理
 *
 *	[内容]
 *		RTCの定期割込み後にメインルーチンで実行される
 *	[引数]
 *		なし
 *	[戻り値]
 *		なし
 *******************************************************************************
 */
void ApiRtcInt( void )
{
	/* 時計補正処理実行 */
#if 0
	switch( gvRtcSts )
	{
		/* 内蔵RTCが遅れている */
		case ecRtcSts_Delay:
			SubTimeComp_Delay();								/* 内蔵RTC遅れ補正処理 */
			break;
		
		/* 内蔵RTCが進んでいる */
		case ecRtcSts_Fast:
			SubTimeComp_Fast();									/* 内蔵RTC進み補正処理 */
			break;
		
		/* 異常無し */
		case ecRtcSts_Non:
		default:
			/* 収録周期と時刻を比較して、収録1、収録2の測定・収録を行うかの判断 */
			SubTimeComp_MeaExeChk( gvClock );
			break;
	}
#else
	if( gvRtcSts == ecRtcSts_Non )
	{
		/* 収録周期と時刻を比較して、収録1、収録2の測定・収録を行うかの判断 */
		SubTimeComp_MeaExeChk( gvClock );
	}
	else
	{
		SubTimeComp_DelayFast();
	}
#endif
	
	
	if( gvClock.mSec == 0U )
	{
		/* 1分に1回 */
		gvModuleSts.mErrChk |= imErrChk_KeyChk;			/* キー動作確認 */
		
		/* 5分に1回 */
		if( gvClock.mMin % 5U == 0U )
		{
			ApiAdc_AdExe();								/* 電池AD実行 */
		}
	
		/* 1時間に1回 */
		if( gvClock.mMin == 0U )
		{
			R_TAU0_Channel1_Start();					/* サブクロック発振確認 */
			gvCycTestStart = imON;
		}
	}
	
	
#if 0	/* 逸脱許容時間の機能は第一フェースでは削除 */
	/* 警報逸脱クリアの指定時刻確認 */
	if( (gvInFlash.mParam.mDeviClrTimeHour == gvClock.mHour) &&
		(gvInFlash.mParam.mDeviClrTimeMin == gvClock.mMin) )
	{
		gvAlmClrPoint |= imAlmClrPoint_Time;
	}
#endif
	
	/* Ver表示→測定値表示遷移 */
	if( gvDispMode.mNowDisp == ecDispMode_UpVerDwNon )
	{
		gvDispMode.mNowDisp = ecDispMode_UpVerDwNon2;
	}
	else if( gvDispMode.mNowDisp == ecDispMode_UpVerDwNon2 )
	{
#if (swSensorCom == imEnable)
		switch( gvInFlash.mProcess.mModelCode )
		{
			case ecSensType_HumTmp:
				gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH;
				break;
			case ecSensType_V:										/* 電圧 */
					gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH_V;
					break;
			case ecSensType_Pulse:									/* パルス */
					gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH_P;
					break;
			default:
			case ecSensType_Pt:
				gvDispMode.mNowDisp = ecDispMode_Up1CHDwClk;
				break;
		}
#else
		gvDispMode.mNowDisp = ecDispMode_Up1CHDwClk;
#endif
	}
	
	/* Lock表示→測定値表示遷移 */
	if( gvDispMode.mNowDisp == ecDispMode_Lock )
	{
		gvDispMode.mNowDisp = ecDispMode_Lock2;
	}
	else if( gvDispMode.mNowDisp == ecDispMode_Lock2 )
	{
		gvDispMode.mNowDisp = gvDispMode.mPreDisp;
	}
	
	gvModuleSts.mRtcInt = ecRtcIntSts_Sleep;
}


/*
 *******************************************************************************
 *	収録周期と時刻を比較して、収録1、収録2の測定・収録を行うかの判断
 *
 *	[内容]
 *		収録周期と時刻を比較して、収録1、収録2の測定・収録を行うかの判断
 *	[引数]
 *		ST_RTC_t	arClock:時刻
 *	[戻り値]
 *		ET_MeaSts_t	arMeasSts:確認結果
 *******************************************************************************
 */
static void SubTimeComp_MeaExeChk( ST_RTC_t arClock )
{
	uint32_t		wkRecTime;
	ET_MeaSts_t		wkMeasSts1;			/* 測定1収録実行ステータス */
	ET_MeaSts_t		wkMeasSts2;			/* 測定2収録実行ステータス */
	
	static uint32_t	vPreLogClock = 0;	/* 前回収録時刻 */
	
	/* 状態遷移 */
	if( gvModuleSts.mExe == ecExeModuleSts_Sleep )
	{
		wkMeasSts1 = SubRtcIntMeas1( arClock );		/* 収録1ステータス */
		wkMeasSts2 = SubRtcIntMeas2( arClock );		/* 収録2ステータス */
		
		if( gvFirmupMode == imOFF && (wkMeasSts1 != ecMeaSts_Init || wkMeasSts2 != ecMeaSts_Init) )
		{
			
			wkRecTime = SubTimeComp_GetEvtRecTime( arClock, 0U );
			if( wkRecTime > vPreLogClock )
			{
				/* 測定処理 */
#if (swSensorCom == imEnable)
				/* 通信処理 */
				if( gvModuleSts.mCom == ecComModuleSts_Sleep )
				{
					gvModuleSts.mCom = ecComModuleSts_Run;
				}
#else
				if( gvModuleSts.mMea == ecMeaModuleSts_Sleep )
				{
					gvModuleSts.mMea = ecMeaModuleSts_Run;
				}
#endif
				/* 実行ステータス更新 */
				if( wkMeasSts1 == ecMeaSts_Log1 )
				{
					gvModuleSts.mExe = ecExeModuleSts_Log1;
				}
				else
				{
					if( wkMeasSts2 == ecMeaSts_Log2 )
					{
						gvModuleSts.mExe = ecExeModuleSts_Log2;
					}
					else
					{
						gvModuleSts.mExe = ecExeModuleSts_Meas;
					}
				}
				
				gvRecCycRtcTm = wkRecTime;					/* 時刻情報(経過秒)を外付けフラッシュ書き込み用に保持する */
				
				vPreLogClock = wkRecTime;					/* 前回との比較用に収録時刻をメモリ */
				
			}
		}
	}
}


/*
 *******************************************************************************
 *	時刻情報(経過秒)を外付けフラッシュ書き込み用に保持する
 *
 *	[内容]
 *		時刻情報(経過秒)を外付けフラッシュ書き込み用に保持する
 *	[引数]
 *		ST_RTC_t	arClock: 時刻
 *		uint8_t		arSel: 収録の種類 0：ログ収録、1：履歴収録
 *	[戻り値]
 *		uint32_t	wkTime:書込み時刻
 *******************************************************************************
 */
uint32_t SubTimeComp_GetEvtRecTime( ST_RTC_t arClock, uint8_t arSel )
{
	uint32_t wkTime;
	
	/* ログ収録時：時計補正(なし)、履歴収録時：時計補正(なし、進み) */
	if( (arSel == 0U && gvRtcSts == ecRtcSts_Non) ||
		(arSel != 0U && gvRtcSts != ecRtcSts_Delay) )
	{
		wkTime = ApiRtcDrv_mktime( gvClock );		/* RTC時刻採用 */
	}
	/* ログ収録時：時計補正(進み、遅れ)、履歴収録時：時計補正(遅れ) */
	else
	{
		/* 余り有 */
		if( SubTimeComp_SurPlusJudgeMeas1( arClock ) == imON )
		{
			wkTime = gvLocalTime - 1U;
		}
		/* 余り無 */
		else
		{
			wkTime = gvLocalTime;
		}
	}
	
	return wkTime;
}


/*
 *******************************************************************************
 *	収録1タイミング確認
 *
 *	[内容]
 *		収録1のタイミングを確認する
 *	[引数]
 *		ST_RTC_t	arClock:時刻
 *	[戻り値]
 *		ET_MeaSts_t	wkMeasSts1:確認結果
 *******************************************************************************
 */
static ET_MeaSts_t	SubRtcIntMeas1( ST_RTC_t arClock )
{
	ET_MeaSts_t		wkMeasSts1 = ecMeaSts_Init;			/* 測定1収録実行ステータス */
	
	switch( gvInFlash.mParam.mLogCyc1 )
	{
		/* 設定無し */
		case ecRecKind1_None:
			break;
		case ecRecKind1_5sec:
			/* 5sec毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 5U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 5U == 0U) || (arClock.mSec % 5U == 1U))) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_10sec:
			/* 10sec毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 10U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 10U == 0U) || (arClock.mSec % 10U == 1U))) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_30sec:
			/* 30sec毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 30U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 30U == 0U) || (arClock.mSec % 30U == 1U))) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_1min:
			/* 1min毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U))) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_5min:
			/* 5min毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 5U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 5U == 0U)) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_10min:
			/* 10min毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 10U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 10U == 0U)) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_15min:
			/* 15min毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 15U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 15U == 0U)) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_30min:
			/* 30min毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 30U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 30U == 0U)) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_60min:
			/* 60min毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin == 0U)) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
	}
	
	/* 何も実行なしの1min毎 */
	if( wkMeasSts1 == ecMeaSts_Init && 
		(((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U)) ||
		((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)))) )
	{
		/* LCD表示のみ更新(時計更新用として) */
		gvModuleSts.mLcd = ecLcdModuleSts_Run;
	}
	
	return wkMeasSts1;
}


/*
 *******************************************************************************
 *	収録2タイミング確認
 *
 *	[内容]
 *		収録1のタイミングを確認する
 *	[引数]
 *		ST_RTC_t	arClock:時刻
 *	[戻り値]
 *		ET_MeaSts_t	wkMeasSts2:確認結果
 *******************************************************************************
 */
static ET_MeaSts_t	SubRtcIntMeas2( ST_RTC_t arClock )
{
	ET_MeaSts_t		wkMeasSts2 = ecMeaSts_Init;			/* 測定1収録実行ステータス */
	
	switch( gvInFlash.mParam.mLogCyc2 )
	{
		default:
		case ecRecKind2_None:
			break;
		case ecRecKind2_5sec:
			/* 5sec毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 5U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 5U == 0U) || (arClock.mSec % 5U == 1U))) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
		case ecRecKind2_10sec:
			/* 10sec毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 10U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 10U == 0U) || (arClock.mSec % 10U == 1U))) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
		case ecRecKind2_30sec:
			/* 30sec毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 30U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 30U == 0U) || (arClock.mSec % 30U == 1U))) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
		case ecRecKind2_1min:
			/* 1min毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U))) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
		case ecRecKind2_5min:
			/* 5min毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 5U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 5U == 0U)) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
		case ecRecKind2_10min:
			/* 10min毎 */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 10U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 10U == 0U)) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
	}
	return wkMeasSts2;
}


/*
 *******************************************************************************
 *	時刻補正処理 初期化処理
 *
 *	[内容]
 *		時刻補正処理 初期化処理
 *******************************************************************************
 */
static void SubTimeComp_Initial( void )
{
	gvLocalTime = 0U;							/* ローカル時刻初期化 */
	gvRtcSts = ecRtcSts_Non;					/* 補正処理なし */
}

#if 0
/*
 *******************************************************************************
 *	内蔵RTC遅れ補正処理
 *
 *	[内容]
 *		内蔵RTC遅れ補正処理
 *******************************************************************************
 */
static void SubTimeComp_Delay( void )
{
	uint32_t		wkRTC_TotalSec;						/* 経過秒 */
	ST_RTC_t		wkClock;
	
	
	/* 収録周期と時刻を比較して、収録1、収録2の測定・収録を行うかの判断 */
	wkClock = ApiRtcDrv_localtime( gvLocalTime );
	SubTimeComp_MeaExeChk( wkClock );
	
	/* ローカル時刻をカウントアップ */
	gvLocalTime += 2U;
	
	/* 収録周期と時刻を比較して、収録1、収録2の測定・収録を行うかの判断 */
	wkClock = ApiRtcDrv_localtime( gvLocalTime );
	SubTimeComp_MeaExeChk( wkClock );
	
	/* 内蔵RTCの時刻取得 */
	wkRTC_TotalSec = ApiRtcDrv_mktime( gvClock );
	
	/* ローカル時刻が内蔵RTCと一致、又はローカル時刻が内蔵RTC超過 */
	if( wkRTC_TotalSec <= gvLocalTime )
	{
		SubTimeComp_Initial();						/* 時刻補正初期化 */
	}
	/* ローカル時刻が内蔵RTC未満 */
	else
	{
		;
	}
}


/*
 *******************************************************************************
 *	内蔵RTC進み補正処理
 *
 *	[内容]
 *		内蔵RTC進み補正処理
 *******************************************************************************
 */
static void SubTimeComp_Fast( void )
{
	uint32_t	wkRTC_TotalSec;						/* 経過秒 */
	ST_RTC_t	wkClock;
	
	/* 収録周期と時刻を比較して、収録1、収録2の測定・収録を行うかの判断 */
	wkClock = ApiRtcDrv_localtime( gvLocalTime );
	SubTimeComp_MeaExeChk( wkClock );
	
	/* 内蔵RTCの時刻取得 */
	wkRTC_TotalSec = ApiRtcDrv_mktime( gvClock );
	
	/* 内蔵RTCがローカル時刻以上 */
	if( wkRTC_TotalSec >= gvLocalTime )
	{
		SubTimeComp_Initial();						/* 時刻補正初期化 */
	}
	else
	{
		;											/* 無処理 */
	}
}
#endif

/*
 *******************************************************************************
 *	内蔵RTC遅れ進み補正処理
 *
 *	[内容]
 *		内蔵RTC遅れ進み補正処理
 *******************************************************************************
 */
static void SubTimeComp_DelayFast( void )
{
	uint32_t	wkRTC_TotalSec;						/* 経過秒 */
	ST_RTC_t	wkClock;

	/* 現在のローカル時刻で収録1、収録2の測定・収録を行うかの判断 */
	wkClock = ApiRtcDrv_localtime( gvLocalTime );
	SubTimeComp_MeaExeChk( wkClock );
	
	/* 内蔵RTCの時刻取得 */
	wkRTC_TotalSec = ApiRtcDrv_mktime( gvClock );
	if( gvRtcSts == ecRtcSts_Delay )
	{
		/* ローカル時刻をカウントアップ */
		gvLocalTime += 2U;
		
		/* 収録周期と時刻を比較して、収録1、収録2の測定・収録を行うかの判断 */
		wkClock = ApiRtcDrv_localtime( gvLocalTime );
		SubTimeComp_MeaExeChk( wkClock );
		
		/* ローカル時刻が内蔵RTCと一致、又はローカル時刻が内蔵RTC超過 */
		if( wkRTC_TotalSec <= gvLocalTime )
		{
			SubTimeComp_Initial();						/* 時刻補正初期化 */
		}
	}
	else
	{
		/* 内蔵RTCがローカル時刻以上 */
		if( wkRTC_TotalSec >= gvLocalTime )
		{
			SubTimeComp_Initial();						/* 時刻補正初期化 */
		}
	}
}


/*
 *******************************************************************************
 * 無線時刻と内蔵RTCの時刻を比較
 *
 *	[内容]
 *		無線時刻と内蔵RTCの時刻を比較し、時刻補正処理のステータスを更新
 *		ローカル時刻に内蔵RTC時刻を保存し、無線時刻を内蔵RTC時刻に保存。
 *		収録1と収録2の収録周期に従い、RTC割込みの時間を変更
 *	[引数]
 *		uint32_t arRfClock : 無線時刻(経過秒：0.5秒分解能)
 *		uint8_t arSize : 時刻サイズ -> 0：32bit, 1:14bit
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiTimeComp_first( uint32_t arRfClock, uint8_t arSize )
{
	uint8_t				wkDirectFlg;
	
	uint16_t			wkPlusDiff;
	uint16_t			wkMinusDiff;
	
	uint32_t			wkRTC_TotalSec;
	uint32_t			wkRF_TotalSec;
	uint32_t			wkRTC_Val;
	uint32_t			wkLocalTime;
	uint32_t			wkU32;
	
	sint32_t			wkClockHist = 0U;
	ET_RtcModuleSts_t	wkUpdate = ecRtcModuleSts_Init;
	
	/* 内蔵RTC経過秒 */
	wkRTC_TotalSec = ApiRtcDrv_mktime( gvClock );
	wkRTC_TotalSec *= 2U;									/* 0.5sec分解能 */
	
	
	/* 時刻サイズ:14bit補正 */
	if( arSize == imClock_14bitSize )
	{
		/* 接続時のフルサイズの時計取得で1時間未満の時計を受信している */
		if( vTimeCompFlg == imON )
		{
			/* 機器時刻を14bitにして比較する */
			wkRTC_Val = wkRTC_TotalSec & 0x3FFFU;
			
			/* 補正時刻 > 機器時刻 */
			if( arRfClock > wkRTC_Val )
			{
				wkU32 = arRfClock - wkRTC_Val;
				wkPlusDiff = (uint16_t)wkU32;
				wkU32 = (imTimCompMaxCnt + 1U + wkRTC_Val) - arRfClock;
				wkMinusDiff = (uint16_t)wkU32;
			}
			/* 補正時刻 < 機器時刻 */
			else if( arRfClock < wkRTC_Val )
			{
				wkU32 = wkRTC_Val - arRfClock;
				wkMinusDiff = (uint16_t)wkU32;
				wkU32 = (imTimCompMaxCnt + 1U + arRfClock) - wkRTC_Val;
				wkPlusDiff = (uint16_t)wkU32;
			}
			else
			{
				;
			}
			
			if( arRfClock != wkRTC_Val )
			{
				if( wkPlusDiff >= wkMinusDiff )
				{
					/* 時刻の差が小さいマイナス補正を採用 */
					wkU32 = wkMinusDiff;
					wkDirectFlg = 0U;
				}
				else if( wkMinusDiff > wkPlusDiff )
				{
					/* 時刻の差が小さいプラス補正を採用 */
					wkU32 = wkPlusDiff;
					wkDirectFlg = 1U;
				}
				
				/* ±1hr(7200cnt)以内は補正実行 */
				if( wkU32 <= imTimCompLimTime )
				{
					/* マイナス補正 */
					if( wkDirectFlg == 0U )
					{
						/* 機器の総時刻からマイナス補正分を引いたカウントが無線の時刻 */
						wkRF_TotalSec = wkRTC_TotalSec - wkU32;
					}
					else
					{
						/* 機器の総時刻にプラス補正分を足したカウントが無線の時刻 */
						wkRF_TotalSec = wkRTC_TotalSec + wkU32;
					}
				}
				else
				{
					wkRF_TotalSec = wkRTC_TotalSec;					/* 補正なし */
				}
			}
			else
			{
				wkRF_TotalSec = wkRTC_TotalSec;						/* 補正なし */
			}
		}
		else
		{
			return;
		}
	}
	/* 時刻サイズ:フルサイズ */
	else
	{
		/* RF時刻経過秒 */
		wkRF_TotalSec = arRfClock;
	}
	
	/* ローカル時刻(0.5秒分解能) */
	wkLocalTime = gvLocalTime * 2U;
	
	/* 時刻補正ステータス更新 */
	/* Delay */
	if( wkRF_TotalSec > wkRTC_TotalSec )
	{
		wkClockHist = wkRF_TotalSec - wkRTC_TotalSec;
		
		if( imTimCompLimTime > wkClockHist )
		{
			/* 時刻フルサイズ補正 */
			if( arSize == imClock_32bitSize )
			{
				vTimeCompFlg = imON;												/* 時計補正有効 */
			}
			
			switch( gvRtcSts )
			{
				/* Non→Delay */
				case ecRtcSts_Non:
					wkUpdate = ecRtcModuleSts_UpdateLose;
					gvRtcSts = ecRtcSts_Delay;										/* 内蔵RTCが遅れている */
					gvLocalTime = wkRTC_TotalSec / 2U;
					break;
					
				/* Delay→Delay */
				case ecRtcSts_Delay:
					wkUpdate = ecRtcModuleSts_UpdateLose;
					break;
					
				/* Fast→Fastのまま or Delay */
				case ecRtcSts_Fast:
					if( wkRF_TotalSec >= wkLocalTime )								/* 無線時刻>=ローカル時刻の場合 */
					{ 
						wkUpdate = ecRtcModuleSts_UpdateLose;
						gvRtcSts = ecRtcSts_Delay;									/* Delayに移行 */
					}
					else															/* 上記以外 */
					{
						wkUpdate = ecRtcModuleSts_UpdateGain;
					}
					break;
					
				default:
					break;
			}
		}
	}
	/* Fast */
	else if( wkRF_TotalSec < wkRTC_TotalSec )
	{
		/* 履歴最新時刻あり */
		if( gvInFlash.mData.mNewestHistTime != 0xFFFFFFFFU )
		{
			if( imTimCompLimTime > (wkRTC_TotalSec - wkRF_TotalSec) )
			{
				/* 補正制限：履歴最新時刻より古い時刻には補正しない */
				if( wkRF_TotalSec < (gvInFlash.mData.mNewestHistTime * 2U) )
				{
					wkRF_TotalSec = gvInFlash.mData.mNewestHistTime * 2U;
				}
			}
		}
		
		/* 補正量が1秒進み以上ある */
		if( wkRF_TotalSec < wkRTC_TotalSec )
		{
			wkClockHist = wkRTC_TotalSec - wkRF_TotalSec;
			
			if( imTimCompLimTime > wkClockHist )
			{
				/* 時刻フルサイズ補正 */
				if( arSize == imClock_32bitSize )
				{
					vTimeCompFlg = imON;												/* 時計補正有効 */
				}
				
				switch( gvRtcSts )
				{
					/* Non→Fast */
					case ecRtcSts_Non:
						wkUpdate = ecRtcModuleSts_UpdateGain;
						gvRtcSts = ecRtcSts_Fast;										/* 内蔵RTCが進んでいる */
						gvLocalTime = wkRTC_TotalSec / 2U;
						break;
						
					/* Delay→Delayのまま or Fast */
					case ecRtcSts_Delay:
						if( wkRF_TotalSec >= wkLocalTime )								/* 無線時刻>=ローカル時刻の場合 */
						{ 
							wkUpdate = ecRtcModuleSts_UpdateLose;
						}
						else															/* 上記以外 */
						{
							wkUpdate = ecRtcModuleSts_UpdateGain;
							gvRtcSts = ecRtcSts_Fast;									/* ローカル時刻が進んでいる */
						}
						break;
						
					/* Fast→Fast */
					case ecRtcSts_Fast:
						wkUpdate = ecRtcModuleSts_UpdateGain;
						break;
						
					default:
						break;
				}
			}
			wkClockHist *= -1;
		}
	}
	else
	{
		/* 時刻フルサイズ補正 */
		if( arSize == imClock_32bitSize )
		{
			vTimeCompFlg = imON;													/* 時計補正有効 */
		}
	}
	
	
	if( wkUpdate != ecRtcModuleSts_Init )
	{
		wkRF_TotalSec /= 2U;
		
		/* ローカル時刻に内蔵RTC時刻を保存 */
		/** 無線時刻を内蔵RTCに保存 **/
		gvClock = ApiRtcDrv_localtime( wkRF_TotalSec );
		
		/* Main関数で実行 */
		gvModuleSts.mRtc = wkUpdate;
		
#if 0	/* データ抜けデバッグ */
		/* ±1分以上の補正 */
//		if( wkClockHist >= 6 || wkClockHist <= -6 )
//		{
			/* 操作履歴(時計補正: 時計補正量、自動操作) */
			wkClockHist /= 2;													/* 分解能0.5秒から1秒へ */
			ApiFlash_WriteSysLog( ecSysLogItm_FastCom, wkClockHist );
//			ApiFlash_WriteSysLog( ecSysLogItm_TimeComp, wkClockHist );
//		}
#else
		/* ±1分以上の補正 */
		if( wkClockHist >= 120 || wkClockHist <= -120 )
		{
			/* 操作履歴(時計補正: 時計補正量、自動操作) */
			wkClockHist /= 2;													/* 分解能0.5秒から1秒へ */
			ApiFlash_WriteSysLog( ecSysLogItm_TimeComp, wkClockHist );
		}
#endif
	}
}

/*
 *******************************************************************************
 *	余り判定
 *
 *	[内容]
 *		収録1の余り判定
 *	[引数]
 *		ST_RTC_t	arClock:時刻
 *	[戻り値]
 *		uint8_t		wkSurPlus:余り結果(imON:余り有、imOFF:余り無)
 *******************************************************************************
 */
static uint8_t	SubTimeComp_SurPlusJudgeMeas1( ST_RTC_t arClock )
{
	uint8_t		wkSurPlus;
	
	/* 変数初期化 */
	wkSurPlus = imOFF;
	
	switch( gvInFlash.mParam.mLogCyc1 )
	{
		case ecRecKind1_5sec:
			if(arClock.mSec % 5U == 1U)
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_10sec:
			if(arClock.mSec % 10U == 1U)
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_30sec:
			if(arClock.mSec % 30U == 1U)
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_1min:
			if(arClock.mSec == 1U)
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_5min:
			if((arClock.mSec == 1U) && (arClock.mMin % 5U == 0U))
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_10min:
			if((arClock.mSec == 1U) && (arClock.mMin % 10U == 0U))
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_15min:
			if((arClock.mSec == 1U) && (arClock.mMin % 15U == 0U))
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_30min:
			if((arClock.mSec == 1U) && (arClock.mMin % 30U == 0U))
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_60min:
			if((arClock.mSec == 1U) && (arClock.mMin == 0U))
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_None:
		default :
			break;
	}
	return wkSurPlus;
}


/*
 *******************************************************************************
 *	収録データ間の時刻差が収録周期の設定と一致するかを判定
 *
 *	[内容]
 *		収録データ間の時刻差が収録周期の設定と一致するかを判定し、
 *		判定結果、又は設定値を返す
 *	[引数]
 *		uint32_t	arSec：秒数
 *	[戻り値]
 *		uint8_t		wkInt: 設定値:収録周期 ET_RecKind1_t
 *******************************************************************************
 */
uint8_t ApiIntMeasExist( uint32_t arSec )
{
	uint8_t wkInt;
	
	switch( arSec )
	{
		case 5U:
			wkInt = (uint8_t)ecRecKind1_5sec;
			break;
		case 10U:
			wkInt = (uint8_t)ecRecKind1_10sec;
			break;
		case 30U:
			wkInt = (uint8_t)ecRecKind1_30sec;
			break;
		case 60U:
			wkInt = (uint8_t)ecRecKind1_1min;
			break;
		case 300U:
			wkInt = (uint8_t)ecRecKind1_5min;
			break;
		case 600U:
			wkInt = (uint8_t)ecRecKind1_10min;
			break;
		case 900U:
			wkInt = (uint8_t)ecRecKind1_15min;
			break;
		case 1800U:
			wkInt = (uint8_t)ecRecKind1_30min;
			break;
		case 3600U:
			wkInt = (uint8_t)ecRecKind1_60min;
			break;
		default:
			wkInt = (uint8_t)ecRecKind1_None;
			break;
	}
	
	return wkInt;
}


/*
 *******************************************************************************
 *	時計補正のフラグ無効
 *
 *	[内容]
 *		時計補正のフラグを無効にする(GWとの無線切断時にクリアされる)
 *	[引数]
 *		なし
 *	[戻り値]
 *		なし
 *******************************************************************************
 */
void ApiTimeComp_TimeCompFlgClr( void )
{
	vTimeCompFlg = imOFF;
}


/*
 *******************************************************************************
 *	ログ時刻とRTC時刻を比較しRTC時刻を制限
 *
 *	[内容]
 *		RTC時刻はログ時刻より前にならないように制限
 *	[引数]
 *		なし
 *	[戻り値]
 *		なし
 *******************************************************************************
 */
void ApiTimeComp_LimitRtcClock( void )
{
	uint32_t wkU32;
	uint32_t wkRTC_TotalSec;
	
	/* 履歴時刻、計測値ログ時刻のいずれかが初期値ではない(値が存在する) */
	if( gvInFlash.mData.mNewestHistTime != 0xFFFFFFFFU || gvInFlash.mData.mNewestLogTime != 0xFFFFFFFFU )
	{
		/* 両方値が存在する */
		if( gvInFlash.mData.mNewestHistTime != 0xFFFFFFFFU && gvInFlash.mData.mNewestLogTime != 0xFFFFFFFFU )
		{
			if( gvInFlash.mData.mNewestHistTime <= gvInFlash.mData.mNewestLogTime )
			{
				wkU32 = gvInFlash.mData.mNewestLogTime;
			}
			else
			{
				wkU32 = gvInFlash.mData.mNewestHistTime;
			}
		}
		/* 履歴時刻のみ存在する */
		else if( gvInFlash.mData.mNewestHistTime != 0xFFFFFFFFU )
		{
			wkU32 = gvInFlash.mData.mNewestHistTime;
		}
		/* 計測値ログ時刻のみ存在する */
		else
		{
			wkU32 = gvInFlash.mData.mNewestLogTime;
		}
		
		/* 内蔵RTC経過秒 */
		wkRTC_TotalSec = ApiRtcDrv_mktime( gvClock );
		
		/* 時刻比較し、ログ時刻以前なら時刻補正 */
		if( wkRTC_TotalSec < wkU32 )
		{
			gvClock = ApiRtcDrv_localtime( wkU32 + 1 );
			ApiRtcDrv_SetInRtc( gvClock );
		}
	}
}


/*
 *******************************************************************************
 *	Flashにメモリするローカル時刻を取得(履歴収録)
 *
 *	[内容]
 *		Flashにメモリするローカル時刻を取得
 *	[引数]
 *		なし
 *	[戻り値]
 *		なし
 *******************************************************************************
 */
uint32_t ApiTimeComp_GetLocalTime( void )
{
	uint32_t wkTimeInfo;
	ST_RTC_t wkClock;
	
	wkClock = ApiRtcDrv_localtime( gvLocalTime );
	wkTimeInfo = SubTimeComp_GetEvtRecTime( wkClock, 1U );
	
	return wkTimeInfo;
}
#pragma section

