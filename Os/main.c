/*
 *******************************************************************************
 *	File name	:	main.c
 *
 *	[内容]
 *		メインルーチン
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2019.
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
#include "RF_Immediate.h"

/*
 *==============================================================================
 *	列挙型定義
 *==============================================================================
 */



/*
 *==============================================================================
 *	変数定義
 *==============================================================================
 */


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	メインルーチン
 *
 *	[内容]
 *		メインルーチン
 *******************************************************************************
 */
void ApiMain( void )
{
	uint8_t		wkRet;
//	uint8_t		wkRet2;
//	uint8_t		wkFlg = 0;
	
#if 0
	uint32_t	wkLoop;
	uint32_t	wkAdr;
	uint8_t		wkRdData[ 36U ];
	uint32_t	wkPreU32;
#endif
	
#if swRfPortDebug == imEnable
	/* Event KeyをTest Portに */
	PFSEG2 &= 0xFEU;
	PM7 &= 0xEFU;
	P7_bit.no4 = 0U;
	P7_bit.no4 = 1U;
#endif
	
	/* 個別パラメータ初期化処理 */
	InitParameter();
	
	/* 割込み許可 */
	EI();
	
	/* 低電圧割込禁止 */
	R_INTC5_Stop();
	
#if (swRESFDisp == imEnable)
	gvResf = RESF;
#endif
	
	/* 起動から乾電池AD測定のウェイト(起動からここまで175msec)+300msec */
	ApiTau0_WaitTimer( 30U );
	
	/* 乾電池AD測定 */
	while( gvModuleSts.mBat == ecBatModuleSts_Run )
	{
		ApiAdc_Main( 0U );
	}
	
	/* 1.8V未満 or 低電圧ポートLo */
	if( gvBatAd.mDryBattAdCnt < imAdCnt_1800mV || !M_TSTBIT(P0, M_BIT1) )
	{
		ApiLcd_UppLow7SegDsp(" LO", imHigh );
		ApiLcd_UppLow7SegDsp("BAT", imLow );
		ApiLcd_Main();
		
		/* LoBat表示 */
		while( 1U )
		{
			R_WDT_Restart();			/* WDTクリア */
		}
	}
	
	/* 設定値展開処理(内蔵フラッシュの設定値をRAMに展開) */
	ApiInFlash_PwrOnRead();
	
	/* フラグ確認し各処理 */
	ApiInFlash_ParmInitFlgChk();
	
	/* 内蔵温度取得 */
	gvInTemp = ApiRfDrv_Temp() + gvInFlash.mProcess.mRfTmpHosei;
	
	
	/* キャパシタ充電のため電池安定時間：3sec */
	ApiTau0_WaitTimer( 300U );
	
#if 0	/* ID等書込み */
//	gvInFlash.mParam.mOnCertLmt = 0x01;

//	gvInFlash.mParam.mGroupID = 0x01;
//	gvInFlash.mProcess.mOpeCompanyID = 0x01;
//	gvInFlash.mParam.mAppID[ 0U ] = 0x0F;
//	gvInFlash.mParam.mAppID[ 1U ] = 0xFF;
	
//	gvInFlash.mProcess.mFirstAcsChk = 0xA5;
	gvInFlash.mProcess.mUniqueID[ 0U ] = 0x00;
	gvInFlash.mProcess.mUniqueID[ 1U ] = 0x00;
	gvInFlash.mProcess.mUniqueID[ 2U ] = 0x03;
	
//	gvInFlash.mParam.mrfHsCh = 2;
//	gvInFlash.mParam.mrfLoraChGrupeCnt = 24;
//	gvInFlash.mParam.mLogCyc1 = ecRecKind1_5sec;

	ApiInFlash_ParmWrite( ecInFlashWrSts_ParmNow, 0U );		/* 内蔵Flashへライト */
#endif
	
	/*** RTCイニシャライズ ***/
	/* RTC(S-35390A) 初期化 */
	ApiRtcDrv_Initial();
	
	/* 外付けRTCの時刻を内蔵RTCに書込む */
	ApiRtcDrv_ExRtcToInRtc();
	
	/* ROM_CRCチェック(時刻取得後) */
	ApiAbn_ChkCRC();
	
	/*** 無線IC初期化 ***/
	while( 1 )
	{
		if( gvModuleSts.mRf == ecRfModuleSts_Run && (ecRFDrvInitSts_InitEnd != ApiRFDrv_GetInitSts()) )
		{
			ApiRFDrv_Initial();
		}
		
		/* RF(無線通信)の初期化完了 */
		if( ecRFDrvInitSts_InitEnd == ApiRFDrv_GetInitSts() )
		{
			break;
		}
	}
	
	/*** 外付けFlashイニシャライズ ***/
	ApiFlash_SrchFlashAdr();		/* Flash書き込みの続きを検索する */
	
#if 0	/* 消費電力テスト */
	PM4_bit.no5 = (uint8_t)imLow;											/* フラッシュ電源制御ポート：出力 */
	P4_bit.no5 = (uint8_t)imHigh;											/* フラッシュ電源制御：電源OFF */
	P12_bit.no5 = (uint8_t)imLow;											/* チップセレクト(プライマリ)＝Low */
	P6_bit.no1 = (uint8_t)imLow;											/* チップセレクト(セカンダリ)＝Low */
	gvModuleSts.mRtc = ecRtcModuleSts_Sleep;
	gvModuleSts.mRtcInt = ecRtcIntSts_Sleep;
	R_RTC_Stop();
	R_IT_Stop();
	
	R_TAU0_Channel0_Stop();			/* 工程用Modbus通信タイマ */
	R_TAU0_Channel1_Stop();			/* 発振異常検査タイマ */
	R_TAU0_Channel2_Stop();			/* キー操作監視タイマ */
	R_TAU0_Channel3_Stop();			/* サーミスタ測定タイマ */
	R_TAU0_Channel4_Stop();			/* 電池電圧ADスタート時のタイマ */
	R_TAU0_Channel5_Stop();			/* 外付けFlash電源ON時のウェイト用タイマ */
	R_TAU0_Channel6_Stop();			/* 無線用タイマ */
	R_TAU0_Channel7_Stop();			/* RTC、測定処理でのウェイト用タイマ */
	
	R_ADC_Stop();
	R_ADC_Set_OperationOff();

//	R_LCD_Stop();																/* LCDコントローラ／ドライバ動作停止 */
//	R_LCD_Voltage_Off();														/* 容量分割回路動作停止 */

//	R_RTC_Sleep();																/* 動作開始後のHALT/STOPモードへの移行処理 */
//	R_RTC_Stop();

	R_COMP0_Stop();
	
	R_INTC0_Stop();
	R_INTC1_Stop();
	R_INTC3_Stop();
	R_INTC4_Stop();
	R_INTC5_Stop();
	R_INTC6_Stop();
	
	R_CSI10_Stop();
	R_IIC00_Stop();
	R_UART2_Stop();
	R_KEY_Stop();
	R_IT_Stop();
	
	M_STOP;																	/* STOPモードへ移行 */
#endif
	
//	gvInFlash.mProcess.mModelCode = ecSensType_HumTmp;
	
#if 0	/* 外付けFlashテスト */
	ApiFlash_FlashPowerCtl( imON, imON );
	wkAdr = 0x04000;
	for( wkLoop = 0 ; wkLoop < 0xFFFFF; )
	{
		R_WDT_Restart();
		ApiFlashDrv_ReadData( wkAdr, &wkRdData[0], 12, ecFlashKind_Prim );
	
		wkU32 = (uint32_t)wkRdData[0] << 24;
		wkU32 += (uint32_t)wkRdData[1] << 16;
		wkU32 += (uint32_t)wkRdData[2] << 8;
		wkU32 += (uint32_t)wkRdData[3];
		wkU32 >>= 2;
		
		if( wkU32 != wkPreU32 + 10 && wkU32 != 0x3fffffff )
		{
			M_NOP;
		}
		wkPreU32 = wkU32;
		
		if( (wkAdr & 0xFFF) == 0xFF0)
		{
			wkAdr += 16;
		}
		else
		{
			wkAdr += 12;
		}
	}
//	ApiFlash_FlashPowerCtl( imOFF, imON );
//	ApiFlashDrv_FirmMemErase( ecFlashKind_Prim );
#endif
	
	/* 最古IndexNo.、時刻リード */
	ApiFlash_ReadQueSetPastTime();
	
	/* 電源投入時、収録されている最新収録データを無線送信用にセットする */
	ApiFlash_SetNewMeasVal();
	
	/* ログ時刻とRTC時刻を比較しRTC時刻を制限 */
	ApiTimeComp_LimitRtcClock();
	
	/* 起動履歴 */
	ApiFlash_WriteActLog( ecActLogItm_Boot, 0U, 4U );
	
	/* ファーム更新履歴 */
	ApiFirmUpdate_History();
	
	/* 電池電圧(空)復帰 */
	ApiAbn_AbnStsClr( imAbnSts_BatEmpty, ecAbnInfKind_AbnSts );
	
	
	/* 乾電池AD測定 */
	gvModuleSts.mBat = ecBatModuleSts_Run;
	while( gvModuleSts.mBat == ecBatModuleSts_Run )
	{
		ApiAdc_Main( 1U );
	}
	gvModuleSts.mBat = ecBatModuleSts_Run;
	
	/* 異常発生、又は異常継続していたらErr表示 */
	ApiAbn_AbnNumSet();
	
	/* 低電圧割込許可 */
	R_INTC5_Start();
	
	while (1)
	{
		/* WDTクリア */
		R_WDT_Restart();
		
		/* RTC定期割込み */
		if( gvModuleSts.mRtcInt == ecRtcIntSts_Run )
		{
			ApiRtcInt();
			
			/* 収録をRAMに記録中 */
			if( gvBatLowSts == ecLowBatFlashSts_Now )
			{
				ApiAdc_LowStopMotionJdg();
			}
			
		}
		
		/* キー入力処理 */
		if( (gvModuleSts.mKeyInt >= ecKeyIntModuleSts_EventMode) &&
			(gvModuleSts.mKeyInt <= ecKeyIntModuleSts_ResetMode) )
		{
			ApiKey();
		}
		
		/* ハードエラー動作チェック */
		if( gvModuleSts.mErrChk != imErrChk_Sleep )
		{
			ApiAbn_Chk();
		}
		
#if (swSensorCom == imEnable)
		/* 上位通信(温湿度要求) */
		if( gvModuleSts.mCom == ecComModuleSts_Run )
		{
			ApiModbus_Main();
		}
#else
		/* 測定 */
		if( gvModuleSts.mMea == ecMeaModuleSts_Run )
		{
			ApiMeas_Main();
		}
#endif
		
		/* 時計更新 */
		if( (gvModuleSts.mRtc == ecRtcModuleSts_UpdateLose) ||
			(gvModuleSts.mRtc == ecRtcModuleSts_UpdateGain) )
		{
			/* 内蔵RTCに時刻ライト */
			ApiRtcDrv_SetInRtc( gvClock );
			gvModuleSts.mLcd = ecLcdModuleSts_Run;
			gvModuleSts.mRtc = ecRtcModuleSts_Sleep;
		}
		
		/* 無線処理 */
		if( gvModuleSts.mRf == ecRfModuleSts_Run )
		{
			ApiRFDrv_Main();
		}
		
		/* 警報処理 */
		if( (gvModuleSts.mExe == ecExeModuleSts_Alm1) ||
			(gvModuleSts.mExe == ecExeModuleSts_Alm2) ||
			(gvModuleSts.mEventKey == ecEventKeyModuleSts_Alm) )
//			(gvAlmClrPoint & (imAlmClrPoint_Time | imAlmClrPoint_FlgCng)) )
		{
			ApiAlarm_Main();
			
			/* 外付けFlashへの書込み */
			if( gvModuleSts.mExe == ecExeModuleSts_Alm1 )
			{
				gvModuleSts.mExe = ecExeModuleSts_ExFlsWr1;				/* 計測値書込み実行 */
			}
			else if( gvModuleSts.mExe == ecExeModuleSts_Alm2 )
			{
				gvModuleSts.mExe = ecExeModuleSts_Sleep;				/* 終了 */
			}
			
			if( gvModuleSts.mEventKey == ecEventKeyModuleSts_Alm )
			{
				gvModuleSts.mEventKey = ecEventKeyModuleSts_ExFlsWr;	/* イベント値書込み実行 */
			}
		}
		
		/* 表示処理 */
		if( gvModuleSts.mLcd != ecLcdModuleSts_Sleep )
		{
#if (swDebugLcd == imEnable)
			ApiLcd_LcdDebug();
#endif
#if (swRfTestLcd == imEnable)
			if( M_TSTFLG(gvRfTestLcdFlg) )
			{
				ApiHmi_Main();
			}
			else
			{
				gvModuleSts.mLcd = ecLcdModuleSts_Sleep;
			}
#else
			ApiHmi_Main();
#endif
		}
		
		/* 内蔵Flashライト */
		if( gvModuleSts.mInFlash == ecInFlashWrExeSts_Run )
		{
			/* 内蔵Flashへ現在のパラメータをライト */
			ApiInFlash_ParmWrite( ecInFlashWrSts_ParmNow, 0U );
			gvModuleSts.mInFlash = ecInFlashWrExeSts_Sleep;
		}
		
		/* 外付けFlashライトデータセット(測定値) */
		if( gvModuleSts.mExe == ecExeModuleSts_ExFlsWr1 )
		{
			ApiFlash_WriteMeasVal1Event( 0U );								/* 測定値1セット */
			gvModuleSts.mExe = ecExeModuleSts_Sleep;
		}
		
		/* 外付けFlashライトデータセット(イベント) */
		if( gvModuleSts.mEventKey == ecEventKeyModuleSts_ExFlsWr )
		{
			ApiFlash_WriteMeasVal1Event( 1U );								/* キー押下時の測定値セット */
			gvModuleSts.mEventKey = ecEventKeyModuleSts_Sleep;
		}
		
		
		/* Flash操作( AD処理初期状態(AD実行中でない) and AD通常動作(電池電圧割込でない) and 収録をRAMに記録中でない) */
		if( (gvBatAd.mPhase == ecAdcPhase_Init) && (gvBatt_Int == ecBattInt_Init) && (gvBatLowSts == ecLowBatFlashSts_Init) )
		{
			wkRet = ApiFlash_GetQueSts();									/* キュー取得 */
			/* キューに貯められていた場合 */
			if( wkRet == imARI )
			{
				/* 電池低電圧ポートLo and AD通常動作(電池電圧割込でない)*/
				if( !M_TSTBIT(P0, M_BIT1) && (gvBatt_Int == ecBattInt_Init) )
				{
					gvBatt_Int = ecBattInt_LowFlashWr;						/* 低電圧LoでFlashライト直前 */
					gvModuleSts.mBat = ecBatModuleSts_Run;					/* 乾電池AD測定 */
				}
				else
				{
					gvModuleSts.mExFlash = ecExFlashModuleSts_Run;				/* 外付けフラッシュRUN状態へ移行 */
					ApiFlash_Main();											/* 外付けフラッシュメイン処理 */
				}
			}
			else
			{
				gvModuleSts.mExFlash = ecExFlashModuleSts_Sleep;			/* スリープ状態へ移行 */
			}
		}
		
		/* 外付けFlashからのリード */
		if( gvModuleSts.mExFlashRd == ecExFlashRdModuleSts_Run )
		{
			ApiFlash_FinReadData();
		}
		
		/* 設定変更内容の動作履歴をFlashライトキューに格納 */
		if( gvModuleSts.mHistory == ecHistoryModuleSts_Run )
		{
			ApiFlash_StoreActHist();
		}
		
		
		/* 乾電池AD測定 */
		if( gvModuleSts.mBat == ecBatModuleSts_Run )
		{
			/* 工程のサブクロック1Hz出力時 and 測定用コンパレータON時は入らない */
			if( ApiRtc1HzCtlRead() != imON && C0ENB == 0U )
			{
				ApiAdc_Main( 1U );
			}
		}
		
		
		/* ファームアップ */
		if( ( gvModuleSts.mFirmup == ecFirmupModuleSts_Run ) && ( ApiGetFlashSts() == ecFlashSts_PowOn) )
		{
			ApiFlash_FlashPowerCtl( imON, imON );					/* 外付けFlashの電源ON */
			ApiFirmUpdateMain();
		}
		
#if (swSensorCom == imDisable)
		/* 上位通信 */
		if( gvModuleSts.mCom == ecComModuleSts_Run )
		{
			ApiModbus_Main();
		}
#endif
#if 0
		if( gvClock.mSec % 10 == 0U )
		{
			if( wkFlg == 0 )
			{
				wkFlg = 1;
				ApiFlash_WriteErrInfo( ecErrInfItm_KeyBehErr, imON );
//				ApiFlash_WriteActLog( ecActLogItm_RfAirplaneOn, 0U, 4U );
//				ApiFlash_WriteSysLog( ecSysLogItm_KeyDisp, 0U );
			}
		}
		else
		{
			wkFlg = 0;
		}
#endif
		
		/* スタンバイモード移行 */
		ApiSleep_Main();
	}
}
#pragma section


