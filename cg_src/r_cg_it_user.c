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
* File Name    : r_cg_it_user.c
* Version      : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for IT module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_it.h"
/* Start user code for include. Do not edit comment generated here */
#include "enum.h"						/* 列挙型の定義 */
#include "func.h"						/* 関数の外部参照(extern)定義 */
#include "glRam.h"						/* グローバル変数定義 */
#include "switch.h"
#include "UserMacro.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_it_interrupt(vect=INTIT)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_it_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_it_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
	EI();
	
#if swLoggerBord == imEnable
	if( gvrfStsEventFlag.mTimingCorrect == 1U )
	{
		R_IT_Stop();
		R_IT_Create_Custom( 0xFFF );				/* gvrfStsEventFlag.mTimerSync値に関わらず0xFFFになる */
		gvrfStsEventFlag.mTimingCorrect = 0U;
		gvRfTimingCorrectCnt = 0U;					/* 3secごとにクリア */
	}
	if( gvRfTimingCounterL < 7U )
	{
		if( !gvrfStsEventFlag.mTimerSync )
		{
			gvRfTimingCounterL++;
#if (swRfPortDebug == imEnable)
			M_SETBIT(P6, M_BIT0);
#endif
		}
		else
		{
			gvrfStsEventFlag.mTimerSync = 0U;
			R_IT_Stop();
			R_IT_Create_Custom(0xFFF);
		}
	}
#else
	if( gvrfStsEventFlag.mTimingCorrect == 1U )
	{
		R_IT_Stop();
		R_IT_Create_Custom( 0x0F );
		gvrfStsEventFlag.mTimingCorrect = 0U;
	}
	if( gvRfTimingCounterL < 0x7FF )
//	if( gvRfTimingCounterL < 0x800 )
	{
#if (swRfPortDebug == imEnable)
		P7_bit.no5 = 0U;
		P7_bit.no4 = 0U;
#endif
		gvRfTimingCounterL++;
	}
#endif
	else
	{
		/* 1sec */
		gvRfTimingCounterL = 0U;
		gvRfTimingCounterH++;
		/* Temp Correct */
		gvRfTimingCorrectCnt++;
#if swLoggerBord == imEnable
		if( gvRfTimingCorrectCnt > 2U )
		{
			R_IT_Stop();
			R_IT_Create_Custom( 0xFFF - gvRfTimingTempCorrect );
			gvrfStsEventFlag.mTimingCorrect = 1U;
			gvRfTimingCorrectCnt = 0U;					/* 3secごとにクリア */
//			P7_bit.no4 = !P7_bit.no4;
		}
#else
		if( gvRfTimingCorrectCnt > 2U )
		{
			R_IT_Stop();
			R_IT_Create_Custom( 0x0F - gvRfTimingTempCorrect );
			gvrfStsEventFlag.mTimingCorrect = 1U;
			gvRfTimingCorrectCnt = 0U;					/* 3secごとにクリア */
		}
		if( vrfTxLimitTime > 0U)
		{
			vrfTxLimitTime--;
		}
#endif
		if( gvRfTimingCounterH >= 7200U )
		{
			/* 2時間でクリア　及び　RFICリセット処理 */
			gvRfTimingCounterH = 0U;
			gvrfStsEventFlag.mReset = 0U;		/* RFIC_INI => 0U */
		}
#if (swRfPortDebug == imEnable)
		M_CLRBIT(P6, M_BIT0);
#endif
		if( !gvRfIntFlg.mRadioStsMsk )
		{
			gvModuleSts.mRf = ecRfModuleSts_Run;
		}
		else
		{
			/* 1sec以上はマスクしない */
			gvRfIntFlg.mRadioStsMsk = 0U;
		}
	}
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */

/*
 *******************************************************************************
 *	インターバルタイマ時間セットしてスタート
 *
 *	[内容]
 *		無線処理のウェイト時間に使用するインターバルタイマをスタート
 *	[引数]
 *		uint16_t：インターバルタイマ設定値
 *	[戻り値]
 *		なし
 *******************************************************************************
 */
void R_IT_Create_Custom( uint16_t ITMCP_value )
{
	TMKAMK = 1U;							/* disable INTIT interrupt */
	TMKAIF = 0U;							/* clear INTIT interrupt flag */
	ITMC = ITMCP_value;						/* カウンタセット */
	ITMC |= _8000_IT_OPERATION_ENABLE;		/* enable IT operation */
	TMKAMK = 0U;							/* enable INTIT interrupt */
	ITMCP_value = 200U;
	while( !(ITMC & 0x8000) && (ITMCP_value > 0U) )
	{
		ITMCP_value --;
	}
	NOP();
}


/* End user code. Do not edit comment generated here */
