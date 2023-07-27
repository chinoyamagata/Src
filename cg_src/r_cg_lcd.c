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
* File Name    : r_cg_lcd.c
* Version      : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for LCD module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_lcd.h"
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
* Function Name: R_LCD_Create
* Description  : This function initializes the LCD module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Create(void)
{
    LCDON = 0U;    /* disable LCD clock operation */
    LCDM0 = _20_LCD_DISPLAY_WAVEFORM_B | _0D_LCD_DISPLAY_MODE1;
    LCDM0 |= _80_LCD_VOLTAGE_MODE_CAPACITOR;
    /* Set CAPL and CAPH pins */
    ISCLCD &= (uint8_t)~_01_LCD_CAPLH_BUFFER_VALID;
    PU12 &= 0x3FU;
    P12 &= 0x3FU;
    PM12 |= 0xC0U;
    /* Set segment pins */
    PU0 &= 0xFAU;
    PU1 &= 0xC0U;
    PU2 &= 0x3FU;
    PU3 &= 0xFEU;
    PU5 &= 0xEFU;
    PU7 &= 0x3EU;
    POM0 &= 0xFEU;
    POM5 &= 0xEFU;
    PFSEG1 |= 0x11U;
    PFSEG2 |= 0x1CU;
    PFSEG4 |= 0xFCU;
    PFSEG5 |= 0x53U;
    P0 &= 0xFAU;
    PM0 &= 0xFAU;
    PMC1 &= 0xF0U;
    P1 &= 0xC0U;
    PM1 &= 0xC0U;
    PMC2 &= 0x3FU;
    P2 &= 0x3FU;
    PM2 &= 0x3FU;
    P3 &= 0xFEU;
    PM3 &= 0xFEU;
    P5 &= 0xEFU;
    PM5 &= 0xEFU;
    P7 &= 0x3EU;
    PM7 &= 0x3EU;
    LCDM1 |= _00_LCD_DISPLAY_PATTERN_A;
    LCDC0 = _06_LCD_CLOCK_FSUB_FIL_7;
}
/***********************************************************************************************************************
* Function Name: R_LCD_Start
* Description  : This function enables the LCD display.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Start(void)
{
    LCDON = 1U;
}
/***********************************************************************************************************************
* Function Name: R_LCD_Stop
* Description  : This function disables the LCD display.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Stop(void)
{
    LCDON = 0U;
}
/***********************************************************************************************************************
* Function Name: R_LCD_Voltage_On
* Description  : This function enables voltage boost circuit or capacitor split circuit.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Voltage_On(void)
{
    volatile uint32_t wt_count;

    LCDM0 |= _80_LCD_VOLTAGE_MODE_CAPACITOR;
    VLCON = 1U;

    /* Change the waiting time according to the system */
    for(wt_count = 0U; wt_count <= LCD_VOLTAGE_WAITTIME; wt_count++)
    {
        NOP();
    }
    
    SCOC = 1U;
}
/***********************************************************************************************************************
* Function Name: R_LCD_Voltage_Off
* Description  : This function disables voltage boost circuit or capacitor split circuit.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_LCD_Voltage_Off(void)
{
    SCOC = 0U;
    VLCON = 0U;
    LCDM0 &= (uint8_t)~_C0_LCD_VOLTAGE_MODE_INITIALVALUE;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
