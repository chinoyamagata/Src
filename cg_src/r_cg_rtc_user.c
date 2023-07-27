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
* File Name    : r_cg_rtc_user.c
* Version      : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for RTC module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_rtc.h"
/* Start user code for include. Do not edit comment generated here */
#include "immediate.h"															/* immediate定数定義 */
#include "enum.h"																/* 列挙型の定義 */
#include "glLabel.h"															/* グローバル変数・定数(const)ラベル定義 */
#include "r_cg_tau.h"
#include "func.h"
#include "UserMacro.h"															/* ユーザー作成マクロ定義 */

/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/

#pragma interrupt r_rtc_interrupt(vect=INTRTC)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
static uint8_t	vSubClk1HzFlg = imOFF;

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_rtc_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_rtc_interrupt(void)
{
    if (1U == RIFG)
    {
        RTCWEN = 1U;
        RTCC1 &= (uint8_t)~_08_RTC_INTC_GENERATE_FLAG;    /* clear RIFG */
        RTCWEN = 0U;
        r_rtc_callback_constperiod();
    }
}
/***********************************************************************************************************************
* Function Name: r_rtc_callback_constperiod
* Description  : This function is real-time clock constant-period interrupt service handler.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_rtc_callback_constperiod(void)
{
    /* Start user code. Do not edit comment generated here */
	EI();
	/* 無線による時刻補正反映中は読み出さない */
	if( (gvModuleSts.mRtc != ecRtcModuleSts_UpdateLose) &&
		(gvModuleSts.mRtc != ecRtcModuleSts_UpdateGain) )
	{
		ApiRtcDrv_ReadInRtc();						/* 内蔵RTC日時読み出し処理 */
	}
	gvModuleSts.mRtcInt = ecRtcIntSts_Run;
	
	if( vSubClk1HzFlg == 1U )
	{
		if( M_TSTBIT(P2, M_BIT2) )
		{
			M_CLRBIT(P2, M_BIT2);
		}
		else
		{
			M_SETBIT(P2, M_BIT2);
		}
	}
	else
	{
		M_CLRBIT(P2, M_BIT2);
	}
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
#pragma section text MY_APP2
/*
 *******************************************************************************
 *	サブクロックによる1Hz出力制御処理
 *
 *	[引数]
 *		uint8_t arCtl：出力制御 0:なし、1:出力
 *	[戻値]
 *		なし
 *	[内容]
 *		サブクロックによる1Hz出力制御を行う
 *******************************************************************************
 */
void ApiRtc1HzCtl( uint8_t arCtl )
{
	vSubClk1HzFlg = arCtl;
}


/*
 *******************************************************************************
 *	サブクロックによる1Hz出力制御処理の状態リード
 *
 *	[引数]
 *		なし
 *	[戻値]
 *		uint8_t arCtl：出力制御 0:なし、1:出力
 *	[内容]
 *		サブクロックによる1Hz出力制御の状態をリードする
 *******************************************************************************
 */
uint8_t ApiRtc1HzCtlRead( void )
{
	return vSubClk1HzFlg;
}
#pragma section
/* End user code. Do not edit comment generated here */
