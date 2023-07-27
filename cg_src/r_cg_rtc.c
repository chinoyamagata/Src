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
* File Name    : r_cg_rtc.c
* Version	   : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
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
#include "UserMacro.h"															/* ユーザー作成マクロ定義 */
#include "enum.h"																/* 列挙型の定義 */
#include "glLabel.h"															/* グローバル変数・定数(const)ラベル定義 */
#include "func.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_RTC_Create
* Description  : This function initializes the real-time clock module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Create(void)
{
	RTCWEN = 1U;   /* enables input clock supply */
	RTCE = 0U;	   /* disable RTC clock operation */
	RTCMK = 1U;    /* disable INTRTC interrupt */
	RTCIF = 0U;    /* clear INTRTC interrupt flag */
	RTITMK = 1U;   /* disable INTRTIT interrupt */
	RTITIF = 0U;   /* clear INTRTIT interrupt flag */
	/* Set INTRTC level 2 priority */
	RTCPR1 = 1U;
	RTCPR0 = 0U;
	RTCC0 = _00_RTC_RTC1HZ_DISABLE | _08_RTC_24HOUR_SYSTEM | _02_RTC_INTRTC_CLOCK_1;
	RTCWEN = 0U;	/* stops input clock supply */
}
/***********************************************************************************************************************
* Function Name: R_RTC_Start
* Description  : This function enables the real-time clock.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Start(void)
{
	volatile uint16_t w_count;

	RTCWEN = 1U;   /* enables input clock supply */
	RTCIF = 0U;    /* clear INTRTC interrupt flag */
	RTCMK = 0U;    /* enable INTRTC interrupt */
	RTCE = 1U;	   /* enable RTC clock operation */

	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count < RTC_WAITTIME_2FRTC; w_count++)
	{
		NOP();
	}

	RTCWEN = 0U;   /* stops input clock supply */
}
/***********************************************************************************************************************
* Function Name: R_RTC_Stop
* Description  : This function disables the real-time clock.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Stop(void)
{
	volatile uint16_t w_count;

	RTCWEN = 1U;  /* enables input clock supply */
	RTCE = 0U;	  /* disable RTC clock operation */
	RTCMK = 1U;   /* disable INTRTC interrupt */
	RTCIF = 0U;   /* clear INTRTC interrupt flag */

	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count < RTC_WAITTIME_2FRTC; w_count++)
	{
		NOP();
	}

	RTCWEN = 0U;  /* stops input clock supply */
}
/***********************************************************************************************************************
* Function Name: R_RTC_Get_CounterValue
* Description  : This function reads the results of real-time clock and store them in the variables.
* Arguments    : counter_read_val -
*					 the current real-time clock value(BCD code)
* Return Value : status -
*					 MD_OK, MD_BUSY1 or MD_BUSY2
***********************************************************************************************************************/
MD_STATUS R_RTC_Get_CounterValue(rtc_counter_value_t * const counter_read_val)
{
	MD_STATUS status = MD_OK;
	volatile uint16_t  w_count;
	
	RTCWEN = 1U;	/* enables input clock supply */
	RTCC1 |= _01_RTC_COUNTER_PAUSE;

	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count < RTC_WAITTIME; w_count++)
	{
		NOP();
	}

	if (0U == RWST)
	{
		status = MD_BUSY1;
	}
	else
	{
		counter_read_val->sec = SEC;
		counter_read_val->min = MIN;
		counter_read_val->hour = HOUR;
		counter_read_val->week = WEEK;
		counter_read_val->day = DAY;
		counter_read_val->month = MONTH;
		counter_read_val->year = YEAR;

		RTCC1 &= (uint8_t)~_01_RTC_COUNTER_PAUSE;

		/* Change the waiting time according to the system */
		for (w_count = 0U; w_count < RTC_WAITTIME; w_count++)
		{
			NOP();
		}

		if (1U == RWST)
		{
			status = MD_BUSY2;
		}
	}

	RTCWEN = 0U;	/* stops input clock supply */

	return (status);
}
/***********************************************************************************************************************
* Function Name: R_RTC_Set_CounterValue
* Description  : This function changes the real-time clock value.
* Arguments    : counter_write_val -
*					 the expected real-time clock value(BCD code)
* Return Value : status -
*					 MD_OK, MD_BUSY1 or MD_BUSY2
***********************************************************************************************************************/
MD_STATUS R_RTC_Set_CounterValue(rtc_counter_value_t counter_write_val)
{
	MD_STATUS status = MD_OK;
	volatile uint16_t  w_count;
	
	RTCWEN = 1U;	/* enables input clock supply */
	RTCC1 |= _01_RTC_COUNTER_PAUSE;

	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count < RTC_WAITTIME; w_count++)
	{
		NOP();
	}

	if (0U == RWST)
	{
		status = MD_BUSY1;
	}
	else
	{
		SEC = counter_write_val.sec;
		MIN = counter_write_val.min;
		HOUR = counter_write_val.hour;
		WEEK = counter_write_val.week;
		DAY = counter_write_val.day;
		MONTH = counter_write_val.month;
		YEAR = counter_write_val.year;
		RTCC1 &= (uint8_t)~_01_RTC_COUNTER_PAUSE;

		/* Change the waiting time according to the system */
		for (w_count = 0U; w_count < RTC_WAITTIME; w_count++)
		{
			NOP();
		}

		if (1U == RWST)
		{
			status = MD_BUSY2;
		}
	}

	RTCWEN = 0U;	/* stops input clock supply */

	return (status);
}
/***********************************************************************************************************************
* Function Name: R_RTC_Set_ConstPeriodInterruptOn
* Description  : This function enables constant-period interrupt.
* Arguments    : period -
*					 the constant period of INTRTC
* Return Value : status -
*					 MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_RTC_Set_ConstPeriodInterruptOn(rtc_int_period_t period)
{
	MD_STATUS status = MD_OK;

	if ((period < HALFSEC) || (period > ONEMONTH))
	{
		status = MD_ARGERROR;
	}
	else
	{
		RTCWEN = 1U;   /* enables input clock supply */
		RTCMK = 1U;    /* disable INTRTC */
		RTCC0 = (RTCC0 & _F8_RTC_INTRTC_CLEAR) | period;
		RTCC1 &= (uint8_t)~_08_RTC_INTC_GENERATE_FLAG;
		RTCIF = 0U;    /* clear INTRTC interrupt flag */
		RTCMK = 0U;    /* enable INTRTC interrupt */
		RTCWEN = 0U;   /* stops input clock supply */
	}

	return (status);
}
/***********************************************************************************************************************
* Function Name: R_RTC_Set_ConstPeriodInterruptOff
* Description  : This function disables constant-period interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_Set_ConstPeriodInterruptOff(void)
{
	RTCWEN = 1U;	   /* enables input clock supply */
	RTCC0 &= _F8_RTC_INTRTC_CLEAR;
	RTCC1 &= (uint8_t)~_08_RTC_INTC_GENERATE_FLAG;
	RTCIF = 0U; 	   /* clear INTRTC interrupt flag */
	RTCWEN = 0U;	   /* stops input clock supply */
}

/* Start user code for adding. Do not edit comment generated here */
#pragma section text MY_APP2


#if 0
/*
 *******************************************************************************
 *	動作開始後のHALT/STOPモードへの移行処理
 *
 *	[内容]
 *		RTCE＝1に設定直後にHALT/STOPモードへ移行する場合は、本処理を実行した後
 *		HALT/STOPモードへ移行すること。
 *	[注記]
 *		RTCE＝1に設定後、1回目INTRTC割り込みの発生以降にSTOPモードへ移行する
 *		場合は、本処理を行う必要無し。
 *******************************************************************************
 */
void R_RTC_Sleep( void )
{
	RTCWEN = 1U;																/* リアルタイム・クロック2の入力クロック供給の制御(1＝入力クロック供給) */
	RTCE = 1U;																	/* リアルタイム・クロック2の動作制御(1＝カウンタ動作開始) */
	RWAIT = 1U;																	/* リアルタイム・クロック2のウエイト制御(1＝SEC～YEARカウンタ停止設定) */

	while (RWST == 1U)															/* リアルタイム・クロック2のウエイト状態フラグ(1＝カウンタ値読み出し，書き込みモード中) */
	{
		break;
	}

	RWAIT = 0U;																	/* リアルタイム・クロック2のウエイト制御(0＝カウンタ動作設定) */

	while (RWST == 0U)															/* リアルタイム・クロック2のウエイト状態フラグ(0＝カウンタ動作) */
	{
		break;
	}

	RTCWEN = 0U;																/* リアルタイム・クロック2の入力クロック供給の制御(0＝入力クロック供給停止) */
}
#endif

#if 0
/***********************************************************************************************************************
* Function Name: R_RTC_InvSw
* Description  : 収録周期に合わせてRTC割込み時間を変更する。
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_InvSw( void )
{
	uint8_t		wkRtcInt;
	
	wkRtcInt = RTCC0;
	if( gvInFlash.mParam.mLogCyc2 == ecRecKind2_None )
	{
		switch( gvInFlash.mParam.mLogCyc1 )
		{
			case ecRecKind1_5sec:							/* 5秒 */
			case ecRecKind1_10sec:							/* 10秒 */
			case ecRecKind1_30sec:							/* 30秒 */
				wkRtcInt = _02_RTC_INTRTC_CLOCK_1;
				break;
			
			default:
			case ecRecKind1_None:							/* なし */
			case ecRecKind1_1min:							/* 1分 */
			case ecRecKind1_5min:							/* 5分 */
			case ecRecKind1_10min:							/* 10分 */
			case ecRecKind1_15min:							/* 15分 */
			case ecRecKind1_30min:							/* 30分 */
			case ecRecKind1_60min:							/* 60分 */
				wkRtcInt = _03_RTC_INTRTC_CLOCK_2;
				break;
		}
	}
	else
	{
		switch( gvInFlash.mParam.mLogCyc2 )
		{
			case ecRecKind2_5sec:
			case ecRecKind2_10sec:
			case ecRecKind2_30sec:
				wkRtcInt = _02_RTC_INTRTC_CLOCK_1;
				break;
			
			default:
			case ecRecKind2_1min:
			case ecRecKind2_5min:
			case ecRecKind2_10min:
				wkRtcInt = _03_RTC_INTRTC_CLOCK_2;
				break;
		}
	}
	
	/* 時計が遅れているときは秒割込み */
	/* Ver,Lock表示時は秒割込み */
	if( (gvRtcSts == ecRtcSts_Delay) ||
		(gvDispMode.mNowDisp == ecDispMode_UpVerDwNon) || (gvDispMode.mNowDisp == ecDispMode_UpVerDwNon2) || (gvDispMode.mNowDisp == ecDispMode_Lock) )
	{
		wkRtcInt = _02_RTC_INTRTC_CLOCK_1;
	}
	
	R_RTC_InvSwManual( wkRtcInt );							/* RTC割込みレジスタ変更 */
}
#endif


#if 0
/***********************************************************************************************************************
* Function Name: R_RTC_InvSw1Sec
* Description  : RTC割込み時間を変更する。
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_RTC_InvSwManual( uint8_t	arSelect )
{
	uint8_t wkRet;
	
	RTCWEN = 1U;																/* RTC2のSFRへのライト許可 */
	RTCMK = 1U;																	/* INTRTC割込禁止 */
	
	/* 工程の1Hz出力時に変更されないように */
	wkRet = ApiRtc1HzCtlRead();
	if( gvMode != ecMode_Normal && wkRet == 1U )
	{
		arSelect = _01_RTC_INTRTC_CLOCK_0;
	}
	
	RTCC0 = (RTCC0 & 0xF8) | arSelect;
	
	RIFG = 0U;																	/* 定周期割込発生ステータスフラグクリア */
	RTCIF = 0U;																	/* INTRTC割込フラグクリア */
	RTCMK = 0U;																	/* INTRTC割込許可 */
	RTCWEN = 0U;																/* RTC2のSFRへのライト禁止 */
}
#endif
#pragma section
/* End user code. Do not edit comment generated here */
