/*
 *******************************************************************************
 *	File name	:	Sleep.c
 *
 *	[内容]
 *		スリープ処理
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2019.10
 *******************************************************************************
 */
#include "cpu_sfr.h"															/* SFR定義ヘッダー */

#include "typedef.h"															/* データ型定義 */
#include "UserMacro.h"															/* ユーザ作成マクロ定義 */
#include "immediate.h"															/* immediate定数定義 */
#include "switch.h"																/* コンパイルスイッチ定義 */
#include "enum.h"																/* 列挙型の定義 */
#include "struct.h"																/* 構造体定義 */
#include "func.h"																/* 関数の外部参照(extern)定義 */
#include "glLabel.h"															/* グローバル変数・定数(const)ラベル定義 */

#if (swSensorCom == imEnable)
#include "r_cg_sau.h"															/* シリアルアレイユニットヘッダー定義 */
#endif
/*
 *==============================================================================
 *	define定義
 *==============================================================================
 */
#define imSTOP_OK	1U
#define imSTOP_NG	0U

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


/*
 *==============================================================================
 *	プロトタイプ宣言
 *==============================================================================
 */
static uint8_t SubSleep_StopPeripheral( void );									/* 周辺機能の停止処理 */
//static void SubSleep_RestartPeripheral( void );									/* 周辺機能の再開処理 */


#pragma section text MY_APP2
#if 0
/*
 *******************************************************************************
 *	スリープ処理 初期化処理
 *
 *	[内容]
 *		スリープ処理の初期化処理を行う。
 *******************************************************************************
 */
void ApiSleep_Initial( void )
{
	gvModuleSts.mRtcInt = ecRtcIntSts_Sleep;
	gvModuleSts.mExe = ecExeModuleSts_Meas;
#if (swSensorCom == imEnable)
	gvModuleSts.mMea = ecMeaModuleSts_Sleep;
#else
	gvModuleSts.mMea = ecMeaModuleSts_Run;
#endif
	gvModuleSts.mKeyInt = ecKeyIntModuleSts_Sleep;
	gvModuleSts.mEventKey = ecEventKeyModuleSts_Sleep;
	gvModuleSts.mBat = ecBatModuleSts_Run;
#if (swSensorCom == imEnable)
	gvModuleSts.mCom = ecComModuleSts_Run;
#else
	gvModuleSts.mCom = ecComModuleSts_Sleep;
#endif
	gvModuleSts.mRf = ecRfModuleSts_Run;
	gvModuleSts.mRtc = ecRtcModuleSts_Sleep;
	gvModuleSts.mExFlash = ecExFlashModuleSts_Sleep;
	gvModuleSts.mLcd = ecLcdModuleSts_Run;
	gvModuleSts.mErrChk = imErrChk_Sleep;
	gvModuleSts.mExFlashRd = ecExFlashRdModuleSts_Sleep;
	gvModuleSts.mInFlash = ecInFlashWrExeSts_Sleep;
	gvModuleSts.mHistory = ecHistoryModuleSts_Sleep;
	gvModuleSts.mFirmup = ecFirmupModuleSts_Sleep;
}
#endif

/*
 *******************************************************************************
 *	スリープ処理メイン
 *
 *	[内容]
 *		スリープ処理メイン
 *******************************************************************************
 */
void ApiSleep_Main( void )
{
	uint8_t		wkRet;
	
	if( (gvModuleSts.mRtcInt == ecRtcIntSts_Sleep)				&&
		(gvModuleSts.mExe == ecExeModuleSts_Sleep)				&&
//		(gvModuleSts.mMea == ecMeaModuleSts_Sleep)				&&
//		(gvModuleSts.mKeyInt == ecKeyIntModuleSts_Sleep)		&&
		(gvModuleSts.mEventKey == ecEventKeyModuleSts_Sleep)	&&
		(gvModuleSts.mBat == ecBatModuleSts_Sleep)				&&
//		(gvModuleSts.mCom == ecComModuleSts_Sleep)				&&
		(gvModuleSts.mRf == ecRfModuleSts_Sleep)				&&
		(gvModuleSts.mRtc == ecRtcModuleSts_Sleep)				&&
		(gvModuleSts.mExFlash == ecExFlashModuleSts_Sleep)		&&
		(gvModuleSts.mLcd == ecLcdModuleSts_Sleep)				&&
		(gvModuleSts.mErrChk == imErrChk_Sleep)					&&
		(gvModuleSts.mExFlashRd == ecExFlashRdModuleSts_Sleep)	&&
		(gvModuleSts.mHistory == ecHistoryModuleSts_Sleep)			)
	{
		if( gvRfIntFlg.mRadioStsMsk )
		{
			M_HALT;
		}
		else
		{
			/* 無線用タイマ6動作中ならHALT */
			/* キー押下タイマ2動作中ならHALT */
			/* 電池投入後ADまでのウェイトタイマ0動作中ならHALT */
			/* 振動子診断タイマ1動作中ならHALT */
			if( TMMK00 == 0U || TMMK01 == 0U || TMMK02 == 0U || TMMK03 == 0U || TMMK04 == 0U || TMMK05 == 0U || TMMK06 == 0U || TMMK07 == 0U )
			{
				M_HALT;
			}
			else
			{
				wkRet = SubSleep_StopPeripheral();			/* 周辺機能の停止 */
				if( wkRet == imSTOP_OK )
				{
					M_STOP;
				}
				else
				{
					M_HALT;
				}
			}
		}
	}
}


/*
 *******************************************************************************
 *	周辺機能の停止処理
 *
 *	[内容]
 *		スリープに移行する前に行う周辺機能の停止処理
 *******************************************************************************
 */
static uint8_t SubSleep_StopPeripheral( void )
{
	uint8_t wkRet = imSTOP_NG;
	
	DI();							/* 割込み禁止 */
									/* 次の処理が実行される直前に割込発生して周辺機能がON状態になり、次の処理で周辺機能が停止される場面を除くため */
	
	/* タイマ・アレイ・ユニット */
	if( TMMK00 != 0U )
	{
		R_TAU0_Channel0_Stop();			/* 電池投入ウェイト/工程用Modbus通信タイマ */
		
		if( TMMK01 != 0U )
		{
			R_TAU0_Channel1_Stop();			/* 発振異常検査タイマ */
			
			if( TMMK02 != 0U )
			{
				R_TAU0_Channel2_Stop();			/* キー操作監視タイマ */
				
				if( TMMK03 != 0U )
				{
					R_TAU0_Channel3_Stop();			/* ユニット間通信タイマ */
					
					if( TMMK04 != 0U )
					{
						R_TAU0_Channel4_Stop();			/* 電池電圧ADスタート時のタイマ */
						
						if( TMMK05 != 0U )
						{
							R_TAU0_Channel5_Stop();			/* 外付けFlash電源ON時のウェイト用タイマ */
							
							if( TMMK06 != 0U )
							{
								R_TAU0_Channel6_Stop();			/* 無線用タイマ */
								
								if( TMMK07 != 0U )
								{
									R_TAU0_Channel7_Stop();			/* RTC、測定処理でのウェイト用タイマ */

									/* A/Dコンバータ */
									R_ADC_Stop();
									R_ADC_Set_OperationOff();

									/* LCDコントローラ／ドライバ */
//									R_LCD_Stop();							/* LCDコントローラ／ドライバ動作停止 */
//									R_LCD_Voltage_Off();					/* 容量分割回路動作停止 */

									/* リアルタイムクロック */
//									R_RTC_Sleep();							/* 動作開始後のHALT/STOPモードへの移行処理 */
//									R_RTC_Stop();

									/* 割り込み */
//									R_INTC0_Stop();
//									R_INTC1_Stop();
//									R_INTC3_Stop();
//									R_INTC4_Stop();
//									R_INTC5_Stop();
//									R_INTC6_Stop();

//									R_CSI10_Stop();
//									R_IIC00_Stop();
//									R_UART2_Stop();
//									R_KEY_Stop();
//									R_IT_Stop();
									
									wkRet = imSTOP_OK;
								}
							}
						}
					}
				}
			}
		}
	}
	
	EI();

	return wkRet;
}

#if 0
/*
 *******************************************************************************
 *	周辺機能の再開処理
 *
 *	[内容]
 *		スリープに移行後に行う周辺機能の再開処理
 *******************************************************************************
 */
static void SubSleep_RestartPeripheral( void )
{
	/*
	 ***************************************
	 *	LCDコントローラ／ドライバ(使用せず)
	 ***************************************
	 */
//	R_LCD_Start();							/* LCDコントローラ／ドライバ動作開始 */
//	R_LCD_Voltage_On();						/* 容量分割回路動作許可 */

	/*
	 ***************************************
	 *	リアルタイムクロック(使用せず)
	 ***************************************
	 */
//	R_RTC_Start();
}
#endif
#pragma section


