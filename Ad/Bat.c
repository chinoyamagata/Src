/*
 *******************************************************************************
 *	File name	:	Bat.c
 *
 *	[内容]
 *		バッテリー状態変化時の処理
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2019.09
 *******************************************************************************
 */
#if 0
#include "cpu_sfr.h"															/* SFR定義ヘッダー */

#include "typedef.h"															/* データ型定義 */
#include "UserMacro.h"															/* ユーザー作成マクロ定義 */
#include "immediate.h"															/* immediate定数定義 */
#include "switch.h"																/* コンパイルスイッチ定義 */
#include "enum.h"																/* 列挙型の定義 */
#include "struct.h"																/* 構造体定義 */
#include "func.h"																/* 関数の外部参照(extern)定義 */
#include "glLabel.h"															/* グローバル変数・定数(const)ラベル定義 */
#endif


/*
 *==============================================================================
 *	define定義
 *==============================================================================
 */
//#define imBatLowTime_1min		60U											/* 低電圧時間：60秒(1分) */


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


/*
 *==============================================================================
 *	変数定義
 *==============================================================================
 */


/*
 *==============================================================================
 *	定数定義
 *==============================================================================
 */
#if 0
static uint16_t vOldAllTime;								/* 低電圧発生時の秒数(分＋秒) */
#endif


/*
 *==============================================================================
 *	プロトタイプ宣言
 *==============================================================================
 */


#pragma section text MY_APP2
#if 0
/*
 *******************************************************************************
 *	バッテリ処理 初期化処理
 *
 *	[内容]
 *		バッテリ処理 初期化処理
 ****************************************************** */
void ApiBat_Initial( void )
{
	gvBatLowSts = ecLowBatFlashSts_Init;
}
#endif

#if 0
/*
 *******************************************************************************
 *	バッテリ変化時の処理
 *
 *	[内容]
 *		バッテリ変化時の処理
 *******************************************************************************
 */
void ApiBatChgMain( void )
{
	/* 電池：なし→あり */
	if( gvModuleSts.mBatChg == ecBatChgModuleSts_BatOn )
	{
		if( imON == ApiAbn_AbnStsClr( imAbnSts_BatEmpty, ecAbnInfKind_AbnSts ) )
			{
			ApiFlash_WriteErrInfo( ecErrInfItm_BatEmpty, imOFF );			/* 機器異常履歴：電池電圧(空)復帰 */
			ApiFlash_WriteActLog( ecActLogItm_BattIn, 0U, 4U );				/* 操作履歴：電池交換 */
		}
		
		gvBatLowSts = ecLowBatFlashSts_Ret;									/* 収録をRAMに記録からの復帰 */
	}
	/* 電池：あり→なし */
	else
	{
		ApiRtcDrv_InRtcToExRtc();						/* 内蔵RTCの時刻を外付けRTCに書込む */
		
		ApiInFlash_ParmWrite( ecInFlashWrSts_ParmNow, 0U );					/* 現時点の変数、収録アドレステーブルを内蔵Flashへライト */
		
		if( imON == ApiAbn_AbnStsSet( imAbnSts_BatEmpty, ecAbnInfKind_AbnSts ) )
		{
			ApiFlash_WriteErrInfo( ecErrInfItm_BatEmpty, imON );			/* 機器異常履歴：電池電圧(空)発生 */
			ApiFlash_WriteActLog( ecActLogItm_BattBackup, 0U, 4U );			/* 操作履歴：電池交換バックアップ状態 */
		}
		vOldAllTime = ( gvClock.mMin * 60U ) + gvClock.mSec;
			
		gvBatLowSts = ecLowBatFlashSts_Now;									/* 収録をRAMに記録中 */
		}
	
		gvModuleSts.mBatChg = ecBatChgModuleSts_Sleep;
	}
#endif

#if 0
/*
 *******************************************************************************
 *	低電圧時の動作停止判定処理
 *
 *	[内容]
 *		低電圧中の場合、1分間の計測を行う。経過時は画面の消灯を行う。
 *		また、低電圧用のキューがフル状態となった場合にも画面の消灯を行う。
 *******************************************************************************
 */
void ApiBatLowStopMotionJdg( void )
{
	uint16_t	wkDiff;
	ET_Error_t	wkRet;
	
	/* 全秒に変換 */
	wkDiff = ((gvClock.mMin * 60U) + gvClock.mSec) - vOldAllTime;
	
	/* 低電圧用キューに空きがあるか取得 */
	wkRet = ApiFlash_GetEmptyQueueJdg();
	
	
	/* H電圧 & 低電圧割込み許可 */
	if( M_TSTBIT(P0, M_BIT1) && PMK5 == 0U )
	{
		R_INTC5_Stop();											/* 低電圧割込禁止 */
		gvBatt_Int = ecBattInt_High;							/* 電池電圧High検出割込み発生 */
		
		/* 電池投入時、電池マーク消灯 */
		if( gvBatAd.mBattSts == ecBattSts_Non || gvBatAd.mBattSts == ecBattSts_Empty )
		{
			gvBatAd.mBattSts = ecBattSts_Init;
			gvModuleSts.mLcd = ecLcdModuleSts_Run;				/* 画面更新 */
		}
		
		ApiTau0_GetTau0Ch0TimeClr();							/* 乾電池AD測定(3secWait後) */
		R_TAU0_Channel0_Start();
		
	}
	
	
	/* 低電圧発生時間と比較 */
	if( (wkDiff >= imBatLowTime_1min) ||						/* 60秒経過した場合 */
		(wkRet == ecERR_NG) )									/* 低電圧用のキューに空きがない場合 */
	{
		/* 全消灯処理 */
		ApiLcd_LcdStop();														/* LCDドライバ停止処理 */
		
		while( 1U )
		{
			/* 電池投入割込発生 */
			if( gvBatt_Int == ecBattInt_High )
			{
				break;
			}
			M_HALT;
	}
		WDTE = 0xFFU;											/* 強制リセット */
}
}
#endif

#pragma section
