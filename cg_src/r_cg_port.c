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
* File Name    : r_cg_port.c
* Version      : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for Port module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
/* Start user code for include. Do not edit comment generated here */
#include "UserMacro.h"															/* ユーザー作成マクロ定義 */
#include "immediate.h"															/* immediate定数定義 */
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
* Function Name: R_PORT_Create
* Description  : This function initializes the Port I/O.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_PORT_Create(void)
{
    PFSEG0 = _00_PFSEG07_PORT | _40_PFSEG06_SEG;
    PFSEG1 = _10_PFSEG12_SEG | _08_PFSEG11_SEG | _01_PFSEG08_SEG;
    PFSEG2 = _80_PFSEG23_SEG | _00_PFSEG22_PORT | _20_PFSEG21_SEG | _10_PFSEG20_SEG | _08_PFSEG19_SEG | 
             _04_PFSEG18_SEG | _02_PFSEG17_SEG | _01_PFSEG16_SEG;
    PFSEG3 = _40_PFSEG29_SEG | _00_PFDEG_PORT;
    PFSEG4 = _80_PFSEG38_SEG | _40_PFSEG37_SEG | _20_PFSEG36_SEG | _10_PFSEG35_SEG | _08_PFSEG34_SEG | _04_PFSEG33_SEG;
    PFSEG5 = _80_PFSEG46_SEG | _40_PFSEG45_SEG | _20_PFSEG44_SEG | _10_PFSEG43_SEG | _08_PFSEG42_SEG | 
             _04_PFSEG41_SEG | _02_PFSEG40_SEG | _01_PFSEG39_SEG;
    PFSEG6 = _08_PFSEG50_SEG | _04_PFSEG49_SEG | _02_PFSEG48_SEG | _01_PFSEG47_SEG;
    ISCLCD = _02_ISCVL3_VALID;
    P2 = _01_Pn0_OUTPUT_1 | _02_Pn1_OUTPUT_1;
    P3 = _04_Pn2_OUTPUT_1;
    P4 = _04_Pn2_OUTPUT_1 | _00_Pn3_OUTPUT_0 | _00_Pn5_OUTPUT_0;
    P5 = _00_Pn3_OUTPUT_0;
    P6 = _00_Pn0_OUTPUT_0 | _02_Pn1_OUTPUT_1;
    P12 = _20_Pn5_OUTPUT_1;
    ADPC = _01_ADPC_DI_ON;
    PM2 = _00_PMn0_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _04_PMn2_NOT_USE | _40_PMn6_NOT_USE | _80_PMn7_NOT_USE | 
          _38_PM2_DEFAULT;
    PM3 = _01_PMn0_NOT_USE | _02_PMn1_NOT_USE | _00_PMn2_MODE_OUTPUT | _08_PMn3_NOT_USE | _F0_PM3_DEFAULT;
    PM4 = _01_PMn0_NOT_USE | _00_PMn2_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT | _10_PMn4_NOT_USE | _00_PMn5_MODE_OUTPUT | 
          _C2_PM4_DEFAULT;
    PM5 = _04_PMn2_NOT_USE | _00_PMn3_MODE_OUTPUT | _10_PMn4_NOT_USE | _80_PMn7_NOT_USE | _63_PM5_DEFAULT;
    PM6 = _00_PMn0_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _FC_PM6_DEFAULT;
    PM12 = _00_PMn5_MODE_OUTPUT | _40_PMn6_NOT_USE | _80_PMn7_NOT_USE | _1F_PM12_DEFAULT;
}

/* Start user code for adding. Do not edit comment generated here */
#pragma section text MY_APP2
/*
 *******************************************************************************
 *	乾電池用A/Dコントロール用ポートON/OFF
 *
 *	[内容]
 *		乾電池用A/Dコントロール用ポートのON/OFFを行う
 *******************************************************************************
 */
void R_PORT_BattCtrlOnOff( uint8_t arOnOff )
{
	if( arOnOff == imOFF )
	{
		M_SETBIT(P6, M_BIT0);
	}
	else
	{
		M_CLRBIT(P6, M_BIT0);
	}
}
#pragma section
/* End user code. Do not edit comment generated here */
