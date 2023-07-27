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
* File Name    : r_cg_comp_user.c
* Version	   : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for COMP module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_comp.h"
/* Start user code for include. Do not edit comment generated here */
#include "r_cg_tau.h"
#include "func.h"																/* 関数の外部参照(extern)定義 */
#include "switch.h"																/* コンパイルスイッチ定義 */
#include "glLabel.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_comp0_interrupt(vect=INTCMP0)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_comp0_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_comp0_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	DI();
	TT0 |= _0008_TAU_CH3_STOP_TRG_ON;								/* タイマチャネル3停止 */
	TMMK03 = 1U;													/* disable INTTM03 interrupt */
	TMIF03 = 0U;													/* clear INTTM03 interrupt flag */
	P2_bit.no0 = imHigh;
	P4_bit.no2 = imHigh;
	
	P4_bit.no5 = (uint8_t)imLow;									/* 充放電コントロールポート：Low */
	PM4_bit.no5 = (uint8_t)imLow;									/* 強制放電制御ポート：出力 */
	P4_bit.no5 = (uint8_t)imLow;									/* 強制放電実行 */
	
	ApiR_COMP0_Stop();													/* コンパレータポート：出力Lo */
	EI();
//	ApiMeas_MeasFin();												/* 測定完了時処理 */
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
