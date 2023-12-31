/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2013, 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_tau_user.c
* Version	   : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for TAU module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_tau.h"
/* Start user code for include. Do not edit comment generated here */
#include "func.h"																/* 関数の外部参照(extern)定義 */
#include "glLabel.h"															/* グローバル変数・定数(const)ラベル定義 */
#include "enum.h"																/* 列挙型の定義 */
#include "cpu_sfr.h"
#include "immediate.h"
#include "UserMacro.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_tau0_channel0_interrupt(vect=INTTM00)
#pragma interrupt r_tau0_channel1_interrupt(vect=INTTM01)
#pragma interrupt r_tau0_channel2_interrupt(vect=INTTM02)
#pragma interrupt r_tau0_channel3_interrupt(vect=INTTM03)
#pragma interrupt r_tau0_channel4_interrupt(vect=INTTM04)
#pragma interrupt r_tau0_channel5_interrupt(vect=INTTM05)
#pragma interrupt r_tau0_channel6_interrupt(vect=INTTM06)
#pragma interrupt r_tau0_channel7_interrupt(vect=INTTM07)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* For TAU0_ch1 pulse measurement */
volatile uint32_t g_tau0_ch1_width = 0UL;
/* Start user code for global. Do not edit comment generated here */
static uint16_t vInitCnt = 0U;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_tau0_channel0_interrupt
* Description  : This function INTTM00 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel0_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	
	vInitCnt ++;
	if( gvBatt_Int == ecBattInt_LowLoop || vInitCnt >= 300U )
	{
		if( gvBatAd.mPhase == ecAdcPhase_Init )
		{
			gvModuleSts.mBat = ecBatModuleSts_Run;					/* 乾電池AD測定 */
		}
		R_TAU0_Channel0_Stop();
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel1_interrupt
* Description  : This function INTTM01 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel1_interrupt(void)
{
	if (1U == (TSR01 & _0001_TAU_OVERFLOW_OCCURS))	  /* overflow occurs */
	{			 
		g_tau0_ch1_width = (uint32_t)(TDR01 + 1UL) + 0x10000UL;
	}
	else
	{
		g_tau0_ch1_width = (uint32_t)(TDR01 + 1UL);
	}
	/* Start user code. Do not edit comment generated here */
	EI();
	
	if( gvCycTestStart == imON )
	{
		gvCycTestStart = imOFF;
	}
	else
	{
	gvModuleSts.mErrChk |= imErrChk_CycChk;
		R_TAU0_Channel1_Stop();
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel2_interrupt
* Description  : This function INTTM02 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel2_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	static uint8_t			vKeyPortSts[ 2U ] = { 0U, 0U };
	static uint16_t			vKeyCnt = 0U;
	uint8_t					wkKeyKind;
	ET_KeyIntModuleSts_t	wkNextSts[ 2U ];
	
	EI();
	
	switch( gvModuleSts.mKeyInt )
	{
		case ecKeyIntModuleSts_ChatEventMode:
			wkKeyKind = KEY_EVENT;
			wkNextSts[ 0U ] = ecKeyIntModuleSts_EventMode;
			wkNextSts[ 1U ] = ecKeyIntModuleSts_Sleep;
			break;
			
		case ecKeyIntModuleSts_ChatDispMode:
			wkKeyKind = KEY_DISP;
			wkNextSts[ 0U ] = ecKeyIntModuleSts_RfMode;
			wkNextSts[ 1U ] = ecKeyIntModuleSts_DispMode;
			break;
			
		case ecKeyIntModuleSts_ChatResetMode:
			wkKeyKind = KEY_RESET;
			wkNextSts[ 0U ] = ecKeyIntModuleSts_ResetMode;
//			wkNextSts[ 0U ] = ecKeyIntModuleSts_RtmSndPtResetDspMode;
			wkNextSts[ 1U ] = ecKeyIntModuleSts_Sleep;
			break;
			
#if 0
		case ecKeyIntModuleSts_RtmSndPtResetDspMode:
			wkKeyKind = KEY_RESET;
			wkNextSts[ 0U ] = ecKeyIntModuleSts_RtmSndPtResetDspMode;
			wkNextSts[ 1U ] = ecKeyIntModuleSts_RtmSndPtResetMode;
			break;
#endif
			
		case ecKeyIntModuleSts_ResetMode:
			return;
		default:
			M_NOP;
			break;
	}
	
	vKeyCnt++;
	
	/* ボタン押下後15ms、20ms後のポート状態 */
	if( vKeyCnt == 2U || vKeyCnt == 3U )
	{
		vKeyPortSts[ vKeyCnt - 2U ] = wkKeyKind;
	}
	
	/* ボタン押下後20ms */
	if( vKeyCnt > imKeyChatCnt )
	{
		/* キーチャタリング問題なし */
		if( vKeyPortSts[ 0U ] == imHigh && vKeyPortSts[ 1U ] == imHigh )
		{
			if( wkKeyKind == imHigh )
			{
				/* 1s長押し */
				if( vKeyCnt > imKey1secPress )
				{
					R_TAU0_Channel2_Stop();
					vKeyCnt = 0U;
					gvModuleSts.mKeyInt = wkNextSts[ 0U ];
					}
			}
			else
			{
				R_TAU0_Channel2_Stop();
				vKeyCnt = 0U;
				gvModuleSts.mKeyInt = wkNextSts[ 1U ];
			}
		}
		else
		{
			R_TAU0_Channel2_Stop();
			vKeyCnt = 0U;
			gvModuleSts.mKeyInt = ecKeyIntModuleSts_Sleep;
		}
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel3_interrupt
* Description  : This function INTTM03 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel3_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
#if (swSensorCom == imEnable)
	/* ユニット起動タイマ計測処理 */
	if( gvModInf.mComSts == ecComSts_UnitPwnOnWait )				/* ユニット起動待ち */
	{
		R_TAU0_Channel3_Init();
		R_TAU0_Channel3_Stop();									/* タイマ0(CH3)停止 */
		gvModInf.mComSts = ecComSts_SndPre;						/* コマンド送信に状態遷移 */
		gvModuleSts.mCom = ecComModuleSts_Run;
	}
	
	/* センサ間通信無応答判定 */
	ApiModbus_NoReplyJudge();
#endif
	
	/* 受信中 */
	if( gvModInf.mComSts == ecComSts_Rcv )
	{
		gvModInf.mRcvTmCnt++;										/* 受信タイマカウンタ更新 */
		if( gvModInf.mRcvTmCnt > 5U )								/* 受信タイムアウト */
		{
			gvModInf.mRcvTmCnt = 0U;								/* 受信タイマカウンタ初期化 */
			R_TAU0_Channel3_Stop();
			gvModInf.mComSts = ecComSts_RcvAnalys;					/* 受信解析へ状態遷移 */
			
			gvModInf.mRcvLen = gvModInf.mRcvPos;					/* 受信データ長更新 */
			gvModInf.mRcvPos = 0U;									/* 受信位置情報初期化 */
			gvModuleSts.mCom = ecComModuleSts_Run;
		}
	}
	else
	{
		gvModInf.mRcvTmCnt = 0U;									/* 受信タイマカウンタ初期化 */
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel4_interrupt
* Description  : This function INTTM04 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel4_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	uint8_t wkLoop;
	
	EI();														/* レベル0,1,2多重割込許可 */
	
	/* 電池ADスタート */
	R_TAU0_Channel4_Stop();										/* タイマチャネル4停止 */
	R_ADC_Sel_DryBattCh();										/* 乾電池用アナログ入力チャンネル選択 */
	R_ADC_Set_OperationOn();									/* A/D変換待機 */
	/* 基準電圧安定待ち(1us) */
	for( wkLoop = 0U; wkLoop < 20U; wkLoop++ )
	{
		M_NOP;
	}
	gvAdConvFinFlg = imOFF;										/* 電池AD完了フラグOFF */
	gvBatAd.mRetryCnt = 0U;										/* 電池用ADリトライ回数初期化 */
	
	R_ADC_Start();												/* A/D変換開始 */
	
	gvBatAd.mPhase = ecAdcPhase_AdExe;
	gvModuleSts.mBat = ecBatModuleSts_Run;						/* 乾電池AD処理 */
	
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel5_interrupt
* Description  : This function INTTM05 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel5_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	R_TAU0_Channel5_Stop();
	ApiSetFlashSts( ecFlashSts_Init );
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel6_interrupt
* Description  : This function INTTM06 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel6_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	if( ApiRFDrv_GetSleepLoop() == 0U )
	{
		RTCMK = 0U;   /* enable INTRTC interrupt */
		gvRfIntFlg.mRadioTimer = 1U;
		gvRfIntFlg.mRadioStsMsk = 0U;
		gvModuleSts.mRf = ecRfModuleSts_Run;
		R_TAU0_Channel6_Stop();
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel7_interrupt
* Description  : This function INTTM07 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel7_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	R_TAU0_Channel7_Stop();
	
	if( gvModuleSts.mRtc == ecRtcModuleSts_Wait )
	{
		gvModuleSts.mRtc = ecRtcModuleSts_Run;
	}
	
#if (swSensorCom == imEnable)
	if( gvModuleSts.mCom == ecComModuleSts_Wait )
	{
		gvModuleSts.mCom = ecComModuleSts_Run;
	}
#else
	if( gvModuleSts.mMea == ecMeaModuleSts_Wait )
	{
		gvModuleSts.mMea = ecMeaModuleSts_Run;
	}
#endif
	
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
#pragma section text MY_APP2
/*
 *******************************************************************************
 * タイマ0チャネル1のカウンタ取得
 *
 *	[内容]
 *		タイマ0チャネル1のカウンタ取得する。
 *	[引数]
 *		なし
 *	[戻値]
 *		uint32_t	g_tau0_ch1_width: タイマ0チャネル1のカウンタ
 *******************************************************************************
 */
uint32_t ApiTau0_GetTau0Ch1Cnt( void )
{
	return g_tau0_ch1_width;
}

/*
 *******************************************************************************
 * タイマ0チャネル0のタイマ取得
 *
 *	[内容]
 *		タイマ0チャネル0のタイマを取得する。
 *	[引数]
 *		なし
 *	[戻値]
 *		uint16_t	vInitCnt: タイマ0チャネル0のタイマ
 *******************************************************************************
 */
uint16_t ApiTau0_GetTau0Ch0Time( void )
{
	return vInitCnt;
}

/*
 *******************************************************************************
 * タイマ0チャネル0のタイマクリア
 *
 *	[内容]
 *		タイマ0チャネル0のタイマをクリアする。
 *	[引数]
 *		なし
 *	[戻値]
 *		uint16_t	vInitCnt: タイマ0チャネル0のタイマ
 *******************************************************************************
 */
void ApiTau0_GetTau0Ch0TimeClr( void )
{
	vInitCnt = 0U;
}

/*
 *******************************************************************************
 * タイマ0チャネル0によるタイマウェイト
 *
 *	[内容]
 *		タイマ0チャネル0でタイマウェイとする
 *	[引数]
 *		uint16_t arCnt:10msec/cnt
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiTau0_WaitTimer( uint16_t arCnt )
{
	R_TAU0_Channel0_Start();
	while( arCnt > ApiTau0_GetTau0Ch0Time() )
	{
		R_WDT_Restart();
	}
	R_TAU0_Channel0_Stop();
}

#pragma section
/* End user code. Do not edit comment generated here */
