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
* File Name    : r_cg_key.h
* Version      : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for KEY module.
* Creation Date: 
***********************************************************************************************************************/
#ifndef KEY_H
#define KEY_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    Key return control register (KRCTL)
*/
/* Usage of Key Return Flags (KRMD) */
#define _00_KR_RETURN_FLAG_UNUSED       (0x00U) /* does not use key return flags */
#define _80_KR_RETURN_FLAG_USED         (0x80U) /* uses key return flags */
/* Selection of Detection Edge (KREG) */
#define _00_KR_DETECTION_EDGE_FALL      (0x00U) /* falling edge */
#define _01_KR_DETECTION_EDGE_RISE      (0x01U) /* rising edge */
/*
    Key Return Mode Register (KRM0)
*/
/* Key interrupt mode control (KRM00) */
#define _00_KR0_SIGNAL_DETECT_OFF       (0x00U) /* not detect KR0 signal */
#define _01_KR0_SIGNAL_DETECT_ON        (0x01U) /* detect KR0 signal */
/* Key interrupt mode control (KRM04) */
#define _00_KR4_SIGNAL_DETECT_OFF       (0x00U) /* not detect KR4 signal */
#define _10_KR4_SIGNAL_DETECT_ON        (0x10U) /* detect KR4 signal */
/* Key interrupt mode control (KRM05) */
#define _00_KR5_SIGNAL_DETECT_OFF       (0x00U) /* not detect KR5 signal */
#define _20_KR5_SIGNAL_DETECT_ON        (0x20U) /* detect KR5 signal */
/* Key interrupt mode control (KRM06) */
#define _00_KR6_SIGNAL_DETECT_OFF       (0x00U) /* not detect KR6 signal */
#define _40_KR6_SIGNAL_DETECT_ON        (0x40U) /* detect KR6 signal */
/* Key interrupt mode control (KRM07) */
#define _00_KR7_SIGNAL_DETECT_OFF       (0x00U) /* not detect KR7 signal */
#define _80_KR7_SIGNAL_DETECT_ON        (0x80U) /* detect KR7 signal */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define KEY_WAITTIME                    (1U)   /* change the waiting time according to the system */ 

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_KEY_Create(void);
void R_KEY_Start(void);
void R_KEY_Stop(void);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
