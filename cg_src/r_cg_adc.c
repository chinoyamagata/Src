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
* File Name    : r_cg_adc.c
* Version      : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for ADC module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_adc.h"
/* Start user code for include. Do not edit comment generated here */
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
* Function Name: R_ADC_Create
* Description  : This function initializes the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Create(void)
{
    ADCEN = 1U;  /* supply AD clock */
    ADM0 = _00_AD_ADM0_INITIALVALUE;  /* disable AD conversion and clear ADM0 register */
    ADMK = 1U;  /* disable INTAD interrupt */
    ADIF = 0U;  /* clear INTAD interrupt flag */
    /* Set INTAD low priority */
    ADPR1 = 1U;
    ADPR0 = 1U;
    /* The reset status of ADPC is analog input, so it's unnecessary to set. */
    /* Set ANI16 pin */
    PFSEG3 |= 0x40U;
    PMC2 |= 0x04U;
    PM2 |= 0x04U;
    ADM0 = _08_AD_CONVERSION_CLOCK_32 | _00_AD_TIME_MODE_NORMAL_1;
    ADM1 = _00_AD_TRIGGER_SOFTWARE | _20_AD_CONVMODE_ONESELECT;
    ADM2 = _00_AD_POSITIVE_VDD | _00_AD_NEGATIVE_VSS | _00_AD_AREA_MODE_1 | _00_AD_RESOLUTION_10BIT;
    ADUL = _FF_AD_ADUL_VALUE;
    ADLL = _00_AD_ADLL_VALUE;
    ADS = _10_AD_INPUT_CHANNEL_16;
}
/***********************************************************************************************************************
* Function Name: R_ADC_Start
* Description  : This function starts the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Start(void)
{
    ADIF = 0U;  /* clear INTAD interrupt flag */
    ADMK = 0U;  /* enable INTAD interrupt */
    ADCS = 1U;  /* enable AD conversion */
}
/***********************************************************************************************************************
* Function Name: R_ADC_Stop
* Description  : This function stops the AD converter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Stop(void)
{
    ADCS = 0U;  /* disable AD conversion */
    ADMK = 1U;  /* disable INTAD interrupt */
    ADIF = 0U;  /* clear INTAD interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_ADC_Set_OperationOn
* Description  : This function enables comparator operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Set_OperationOn(void)
{
    ADCE = 1U;  /* enable AD comparator */
}
/***********************************************************************************************************************
* Function Name: R_ADC_Set_OperationOff
* Description  : This function stops comparator operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Set_OperationOff(void)
{
    ADCE = 0U;  /* disable AD comparator */
}
/***********************************************************************************************************************
* Function Name: R_ADC_Get_Result
* Description  : This function returns the conversion result in the buffer.
* Arguments    : buffer -
*                    the address where to write the conversion result
* Return Value : None
***********************************************************************************************************************/
void R_ADC_Get_Result(uint16_t * const buffer)
{
    *buffer = (uint16_t)(ADCR >> 6U);
}

/* Start user code for adding. Do not edit comment generated here */
#pragma section text MY_APP2
/*
 *******************************************************************************
 *	乾電池用アナログ入力チャンネル選択
 *
 *	[内容]
 *		乾電池用アナログ入力チャンネルを選択する。
 *******************************************************************************
 */
void R_ADC_Sel_DryBattCh( void )
{
    ADS = _10_AD_INPUT_CHANNEL_16;
}
#pragma section
/* End user code. Do not edit comment generated here */
