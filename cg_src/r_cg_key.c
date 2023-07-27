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
* File Name    : r_cg_key.c
* Version      : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for KEY module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_key.h"
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
* Function Name: R_KEY_Create
* Description  : This function initializes the key return module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_KEY_Create(void)
{
    volatile uint8_t w_count;

    /* Set KR4 pin */
    PFSEG2 &= 0xFEU;
    PU7 |= 0x10U;
    PM7 |= 0x10U;
    /* Set KR5 pin */
    PFSEG2 &= 0xFDU;
    PU7 |= 0x20U;
    PM7 |= 0x20U;
    KRMK = 1U;  /* disable INTKR operation */
    KRIF = 0U;  /* clear INTKR interrupt flag */
    /* Set INTKR low priority */
    KRPR1 = 1U;
    KRPR0 = 1U;
    KRCTL = _80_KR_RETURN_FLAG_USED | _01_KR_DETECTION_EDGE_RISE;
    KRM0 = _00_KR0_SIGNAL_DETECT_OFF | _10_KR4_SIGNAL_DETECT_ON | _20_KR5_SIGNAL_DETECT_ON | 
           _00_KR6_SIGNAL_DETECT_OFF | _00_KR7_SIGNAL_DETECT_OFF;

    /* Change the waiting time according to the system  */
    for (w_count = 0U; w_count <= KEY_WAITTIME; w_count++)
    {   
        NOP();
    }
}
/***********************************************************************************************************************
* Function Name: R_KEY_Start
* Description  : This function clears INTKR interrupt flag and enables interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_KEY_Start(void)
{
    KRF = 0U;
    KRIF = 0U;  /* clear INTKR interrupt flag */
    KRMK = 0U;  /* enable INTKR operation */
}
/***********************************************************************************************************************
* Function Name: R_KEY_Stop
* Description  : This function disables INTKR interrupt and clears interrupt flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_KEY_Stop(void)
{
    KRMK = 1U;  /* disable INTKR operation */
    KRIF = 0U;  /* clear INTKR interrupt flag */
    KRF = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
