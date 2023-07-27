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
* File Name    : r_cg_intp_user.c
* Version      : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for INTP module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_intp.h"
/* Start user code for include. Do not edit comment generated here */
#include "enum.h"																/* 列挙型の定義 */
#include "func.h"																/* 関数の外部参照(extern)定義 */
#include "glLabel.h"															/* グローバル変数・定数(const)ラベル定義 */
#include "cpu_sfr.h"
#include "UserMacro.h"
#include "RF_Immediate.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_intc0_interrupt(vect=INTP0)
#pragma interrupt r_intc1_interrupt(vect=INTP1)
#pragma interrupt r_intc3_interrupt(vect=INTP3)
#pragma interrupt r_intc4_interrupt(vect=INTP4)
#pragma interrupt r_intc5_interrupt(vect=INTP5)
#pragma interrupt r_intc6_interrupt(vect=INTP6)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_intc0_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_intc0_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
	EI();
	/*** Resetキーポート ***/
	if( gvModuleSts.mKeyInt == ecKeyIntModuleSts_Sleep )
	{
		R_TAU0_Channel2_Start();								/* チャタリング計測用タイマスタート */
		gvModuleSts.mKeyInt = ecKeyIntModuleSts_ChatResetMode;	/* Resetキー */
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_intc1_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_intc1_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
	EI();
	/* Preamble detect / RSSI @RX mode */
	gvRfIntFlg.mRadioDio4 = 1U;
	gvModuleSts.mRf = ecRfModuleSts_Run;
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_intc3_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_intc3_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
	EI();
	gvRfIntFlg.mRadioDio2 = 1U;
	gvModuleSts.mRf = ecRfModuleSts_Run;
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_intc4_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_intc4_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
	EI();
	gvRfIntFlg.mRadioDio1 = 1U;
	gvModuleSts.mRf = ecRfModuleSts_Run;
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_intc5_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_intc5_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
	EI();
	/*** 電池低電圧検出ポート ***/
	
	/* 工場出荷のINT表示以外 */
	if( gvModuleSts.mKeyInt != ecKeyIntModuleSts_ResetMode )
	{
		R_INTC5_Stop();										/* 低電圧割込禁止 */
	}
	
	/* L→H電圧割込 */
	if( M_TSTBIT(P0, M_BIT1) )
	{
		/* 電池投入時、電池マーク消灯 */
		if( gvBatAd.mBattSts == ecBattSts_Non )
		{
			gvBatt_Int = ecBattInt_NonHigh;					/* 電池なしからの電圧H割込発生 */
			
			gvBatAd.mBattSts = ecBattSts_Init;
			gvModuleSts.mLcd = ecLcdModuleSts_Run;			/* 画面更新 */
			
			ApiTau0_GetTau0Ch0TimeClr();					/* 乾電池AD測定(3secWait後) */
			R_TAU0_Channel0_Start();
	}
	else
	{
			gvBatt_Int = ecBattInt_High;					/* 電池電圧High検出割込み発生 */
			
			gvBatAd.mPhase = ecAdcPhase_CtlWait;
			R_PORT_BattCtrlOnOff( imON );					/* 乾電池用A/Dコントロール用ポートON */
			R_TAU0_Channel4_Start();						/* タイマチャネル4開始 */
		}
	}
	/* H→L電圧割込 */
	else
	{
		gvBatt_Int = ecBattInt_Low;							/* 電池電圧Low検出割込み発生 */
	
		/* 電池0以外 */
		if( gvBatAd.mBattSts != ecBattSts_Non && gvBatAd.mBattSts != ecBattSts_Empty )
	{
			gvBatAd.mPhase = ecAdcPhase_CtlWait;
			R_PORT_BattCtrlOnOff( imON );					/* 乾電池用A/Dコントロール用ポートON */
			R_TAU0_Channel4_Start();						/* タイマチャネル4開始 */
		}
		else
		{
			R_INTC5_Start();
		}
	}

	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_intc6_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_intc6_interrupt(void)
{
    /* Start user code. Do not edit comment generated here */
	EI();
	gvRfIntFlg.mRadioDio0 = 1U;
	gvModuleSts.mRf = ecRfModuleSts_Run;
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
