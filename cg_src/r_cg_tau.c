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
* File Name    : r_cg_tau.c
* Version	   : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for TAU module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_tau.h"
/* Start user code for include. Do not edit comment generated here */
#include "cpu_sfr.h"															/* SFR��`�w�b�_�[ */

#include "typedef.h"															/* �f�[�^�^��` */
#include "UserMacro.h"															/* ���[�U�[�쐬�}�N����` */
#include "immediate.h"															/* immediate�萔��` */
#include "switch.h"																/* �R���p�C���X�C�b�`��` */
#include "enum.h"																/* �񋓌^�̒�` */
#include "struct.h"																/* �\���̒�` */
#include "func.h"																/* �֐��̊O���Q��(extern)��` */
#include "glLabel.h"															/* �O���[�o���ϐ��E�萔(const)���x����` */

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
/* For TAU0_ch1 pulse measurement */
extern volatile uint32_t g_tau0_ch1_width;
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_TAU0_Create
* Description  : This function initializes the TAU0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Create(void)
{
	TAU0EN = 1U;	/* supplies input clock */
	TPS0 = _0000_TAU_CKM0_fCLK_0 | _0060_TAU_CKM1_fCLK_6 | _0000_TAU_CKM2_fCLK_1 | _0000_TAU_CKM3_fCLK_8;
	/* Stop all channels */
	TT0 = _0001_TAU_CH0_STOP_TRG_ON | _0002_TAU_CH1_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON | 
		  _0008_TAU_CH3_STOP_TRG_ON | _0010_TAU_CH4_STOP_TRG_ON | _0020_TAU_CH5_STOP_TRG_ON | 
		  _0040_TAU_CH6_STOP_TRG_ON | _0080_TAU_CH7_STOP_TRG_ON | _0200_TAU_CH1_H8_STOP_TRG_ON | 
		  _0800_TAU_CH3_H8_STOP_TRG_ON;
	/* Mask channel 0 interrupt */
	TMMK00 = 1U;	/* disable INTTM00 interrupt */
	TMIF00 = 0U;	/* clear INTTM00 interrupt flag */
	/* Mask channel 1 interrupt */
	TMMK01 = 1U;	/* disable INTTM01 interrupt */
	TMIF01 = 0U;	/* clear INTTM01 interrupt flag */
	/* Mask channel 1 higher 8 bits interrupt */
	TMMK01H = 1U;	 /* disable INTTM01H interrupt */
	TMIF01H = 0U;	 /* clear INTTM01H interrupt flag */
	/* Mask channel 2 interrupt */
	TMMK02 = 1U;	/* disable INTTM02 interrupt */
	TMIF02 = 0U;	/* clear INTTM02 interrupt flag */
	/* Mask channel 3 interrupt */
	TMMK03 = 1U;	/* disable INTTM03 interrupt */
	TMIF03 = 0U;	/* clear INTTM03 interrupt flag */
	/* Mask channel 3 higher 8 bits interrupt */
	TMMK03H = 1U;	 /* disable INTTM03H interrupt */
	TMIF03H = 0U;	 /* clear INTTM03H interrupt flag */
	/* Mask channel 4 interrupt */
	TMMK04 = 1U;	/* disable INTTM04 interrupt */
	TMIF04 = 0U;	/* clear INTTM04 interrupt flag */
	/* Mask channel 5 interrupt */
	TMMK05 = 1U;	/* disable INTTM05 interrupt */
	TMIF05 = 0U;	/* clear INTTM05 interrupt flag */
	/* Mask channel 6 interrupt */
	TMMK06 = 1U;	/* disable INTTM06 interrupt */
	TMIF06 = 0U;	/* clear INTTM06 interrupt flag */
	/* Mask channel 7 interrupt */
	TMMK07 = 1U;	/* disable INTTM07 interrupt */
	TMIF07 = 0U;	/* clear INTTM07 interrupt flag */
	/* Set INTTM00 low priority */
	TMPR100 = 1U;
	TMPR000 = 1U;
	/* Set INTTM01 low priority */
	TMPR101 = 1U;
	TMPR001 = 1U;
	/* Set INTTM02 low priority */
	TMPR102 = 1U;
	TMPR002 = 1U;
	/* Set INTTM03 low priority */
	TMPR103 = 1U;
	TMPR003 = 1U;
	/* Set INTTM04 low priority */
	TMPR104 = 1U;
	TMPR004 = 1U;
	/* Set INTTM05 low priority */
	TMPR105 = 1U;
	TMPR005 = 1U;
	/* Set INTTM06 level 1 priority */
	TMPR106 = 0U;
	TMPR006 = 1U;
	/* Set INTTM07 low priority */
	TMPR107 = 1U;
	TMPR007 = 1U;
	/* Channel 0 used as interval timer */
	TMR00 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
			_0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
	TDR00 = _5DBF_TAU_TDR00_VALUE;
	TO0 &= ~_0001_TAU_CH0_OUTPUT_VALUE_1;
	TOE0 &= ~_0001_TAU_CH0_OUTPUT_ENABLE;
	/* Channel 1 is used to measure input pulse interval */
	TIS0 = _05_TAU_CH1_INPUT_fSUB;
	TMR01 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_16BITS_MODE | _0000_TAU_CLOCK_MODE_CKS | 
			_0100_TAU_TRIGGER_TIMN_VALID | _0000_TAU_TIMN_EDGE_FALLING | _0004_TAU_MODE_CAPTURE | 
			_0000_TAU_START_INT_UNUSED;
	TOM0 &= ~_0002_TAU_CH1_OUTPUT_COMBIN;
	TOL0 &= ~_0002_TAU_CH1_OUTPUT_LEVEL_L;
	TO0 &= ~_0002_TAU_CH1_OUTPUT_VALUE_1;
	TOE0 &= ~_0002_TAU_CH1_OUTPUT_ENABLE;
	/* Channel 2 used as interval timer */
	TMR02 = _8000_TAU_CLOCK_SELECT_CKM1 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
			_0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
	TDR02 = _0752_TAU_TDR02_VALUE;
	TOM0 &= ~_0004_TAU_CH2_OUTPUT_COMBIN;
	TOL0 &= ~_0004_TAU_CH2_OUTPUT_LEVEL_L;
	TO0 &= ~_0004_TAU_CH2_OUTPUT_VALUE_1;
	TOE0 &= ~_0004_TAU_CH2_OUTPUT_ENABLE;
	/* Channel 3 used as interval timer */
	TMR03 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_16BITS_MODE | 
			_0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
	TDR03 = _5DBF_TAU_TDR03_VALUE;
	TOM0 &= ~_0008_TAU_CH3_OUTPUT_COMBIN;
	TOL0 &= ~_0008_TAU_CH3_OUTPUT_LEVEL_L;
	TO0 &= ~_0008_TAU_CH3_OUTPUT_VALUE_1;
	TOE0 &= ~_0008_TAU_CH3_OUTPUT_ENABLE;
	/* Channel 4 used as interval timer */
	TMR04 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
			_0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
	TDR04 = _8C9F_TAU_TDR04_VALUE;
	TOM0 &= ~_0010_TAU_CH4_OUTPUT_COMBIN;
	TOL0 &= ~_0010_TAU_CH4_OUTPUT_LEVEL_L;
	TO0 &= ~_0010_TAU_CH4_OUTPUT_VALUE_1;
	TOE0 &= ~_0010_TAU_CH4_OUTPUT_ENABLE;
	/* Channel 5 used as interval timer */
	TMR05 = _8000_TAU_CLOCK_SELECT_CKM1 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
			_0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
	TDR05 = _080E_TAU_TDR05_VALUE;
	TOM0 &= ~_0020_TAU_CH5_OUTPUT_COMBIN;
	TOL0 &= ~_0020_TAU_CH5_OUTPUT_LEVEL_L;
	TO0 &= ~_0020_TAU_CH5_OUTPUT_VALUE_1;
	TOE0 &= ~_0020_TAU_CH5_OUTPUT_ENABLE;
	/* Channel 6 used as interval timer */
	TMR06 = _8000_TAU_CLOCK_SELECT_CKM1 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
			_0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
	TDR06 = _927B_TAU_TDR06_VALUE;
	TOM0 &= ~_0040_TAU_CH6_OUTPUT_COMBIN;
	TOL0 &= ~_0040_TAU_CH6_OUTPUT_LEVEL_L;
	TO0 &= ~_0040_TAU_CH6_OUTPUT_VALUE_1;
	TOE0 &= ~_0040_TAU_CH6_OUTPUT_ENABLE;
	/* Channel 7 used as interval timer */
	TMR07 = _8000_TAU_CLOCK_SELECT_CKM1 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
			_0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
	TDR07 = _3A97_TAU_TDR07_VALUE;
	TOM0 &= ~_0080_TAU_CH7_OUTPUT_COMBIN;
	TOL0 &= ~_0080_TAU_CH7_OUTPUT_LEVEL_L;
	TO0 &= ~_0080_TAU_CH7_OUTPUT_VALUE_1;
	TOE0 &= ~_0080_TAU_CH7_OUTPUT_ENABLE;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel0_Start
* Description  : This function starts TAU0 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel0_Start(void)
{
	TMIF00 = 0U;	/* clear INTTM00 interrupt flag */
	TMMK00 = 0U;	/* enable INTTM00 interrupt */
	TS0 |= _0001_TAU_CH0_START_TRG_ON;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel0_Stop
* Description  : This function stops TAU0 channel 0 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel0_Stop(void)
{
	TT0 |= _0001_TAU_CH0_STOP_TRG_ON;
	/* Mask channel 0 interrupt */
	TMMK00 = 1U;	/* disable INTTM00 interrupt */
	TMIF00 = 0U;	/* clear INTTM00 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel1_Start
* Description  : This function starts TAU0 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel1_Start(void)
{
	TMIF01 = 0U;	/* clear INTTM01 interrupt flag */
	TMMK01 = 0U;	/* enable INTTM01 interrupt */
	TS0 |= _0002_TAU_CH1_START_TRG_ON;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel1_Stop
* Description  : This function stops TAU0 channel 1 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel1_Stop(void)
{
	TT0 |= _0002_TAU_CH1_STOP_TRG_ON;
	/* Mask channel 1 interrupt */
	TMMK01 = 1U;	/* disable INTTM01 interrupt */
	TMIF01 = 0U;	/* clear INTTM01 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel1_Get_PulseWidth
* Description  : This function measures TAU0 channel 1 input pulse width.
* Arguments    : width -
*					 the address where to write the input pulse width
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel1_Get_PulseWidth(uint32_t * const width)
{
	/* For channel 1 pulse measurement */
	*width = g_tau0_ch1_width;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel2_Start
* Description  : This function starts TAU0 channel 2 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel2_Start(void)
{
	TMIF02 = 0U;	/* clear INTTM02 interrupt flag */
	TMMK02 = 0U;	/* enable INTTM02 interrupt */
	TS0 |= _0004_TAU_CH2_START_TRG_ON;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel2_Stop
* Description  : This function stops TAU0 channel 2 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel2_Stop(void)
{
	TT0 |= _0004_TAU_CH2_STOP_TRG_ON;
	/* Mask channel 2 interrupt */
	TMMK02 = 1U;	/* disable INTTM02 interrupt */
	TMIF02 = 0U;	/* clear INTTM02 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel3_Start
* Description  : This function starts TAU0 channel 3 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel3_Start(void)
{
	TMIF03 = 0U;	/* clear INTTM03 interrupt flag */
	TMMK03 = 0U;	/* enable INTTM03 interrupt */
	TS0 |= _0008_TAU_CH3_START_TRG_ON;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel3_Stop
* Description  : This function stops TAU0 channel 3 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel3_Stop(void)
{
	TT0 |= _0008_TAU_CH3_STOP_TRG_ON;
	/* Mask channel 3 interrupt */
	TMMK03 = 1U;	/* disable INTTM03 interrupt */
	TMIF03 = 0U;	/* clear INTTM03 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel4_Start
* Description  : This function starts TAU0 channel 4 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel4_Start(void)
{
	TMIF04 = 0U;	/* clear INTTM04 interrupt flag */
	TMMK04 = 0U;	/* enable INTTM04 interrupt */
	TS0 |= _0010_TAU_CH4_START_TRG_ON;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel4_Stop
* Description  : This function stops TAU0 channel 4 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel4_Stop(void)
{
	TT0 |= _0010_TAU_CH4_STOP_TRG_ON;
	/* Mask channel 4 interrupt */
	TMMK04 = 1U;	/* disable INTTM04 interrupt */
	TMIF04 = 0U;	/* clear INTTM04 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel5_Start
* Description  : This function starts TAU0 channel 5 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel5_Start(void)
{
	TMIF05 = 0U;	/* clear INTTM05 interrupt flag */
	TMMK05 = 0U;	/* enable INTTM05 interrupt */
	TS0 |= _0020_TAU_CH5_START_TRG_ON;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel5_Stop
* Description  : This function stops TAU0 channel 5 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel5_Stop(void)
{
	TT0 |= _0020_TAU_CH5_STOP_TRG_ON;
	/* Mask channel 5 interrupt */
	TMMK05 = 1U;	/* disable INTTM05 interrupt */
	TMIF05 = 0U;	/* clear INTTM05 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel6_Start
* Description  : This function starts TAU0 channel 6 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel6_Start(void)
{
	TMIF06 = 0U;	/* clear INTTM06 interrupt flag */
	TMMK06 = 0U;	/* enable INTTM06 interrupt */
	TS0 |= _0040_TAU_CH6_START_TRG_ON;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel6_Stop
* Description  : This function stops TAU0 channel 6 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel6_Stop(void)
{
	TT0 |= _0040_TAU_CH6_STOP_TRG_ON;
	/* Mask channel 6 interrupt */
	TMMK06 = 1U;	/* disable INTTM06 interrupt */
	TMIF06 = 0U;	/* clear INTTM06 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel7_Start
* Description  : This function starts TAU0 channel 7 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel7_Start(void)
{
	TMIF07 = 0U;	/* clear INTTM07 interrupt flag */
	TMMK07 = 0U;	/* enable INTTM07 interrupt */
	TS0 |= _0080_TAU_CH7_START_TRG_ON;
}
/***********************************************************************************************************************
* Function Name: R_TAU0_Channel7_Stop
* Description  : This function stops TAU0 channel 7 counter.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_TAU0_Channel7_Stop(void)
{
	TT0 |= _0080_TAU_CH7_STOP_TRG_ON;
	/* Mask channel 7 interrupt */
	TMMK07 = 1U;	/* disable INTTM07 interrupt */
	TMIF07 = 0U;	/* clear INTTM07 interrupt flag */
}

/* Start user code for adding. Do not edit comment generated here */
#pragma section text MY_APP2
/*
 *******************************************************************************
 *	�^�C�}0��Modbus�p�^�C�}�ɃZ�b�g
 *
 *	[���e]
 *		�^�C�}0��Modbus�p�^�C�}�ɃZ�b�g
 *		
 *******************************************************************************
 */
void R_TAU0_Channel0_ModbusTimer( void )
{
	TMMK00 = 1U;	/* disable INTTM00 interrupt */
	TMIF00 = 0U;	/* clear INTTM00 interrupt flag */
	
	/* Channel 0 used as interval timer */
	TMR00 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | 
			_0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
	TDR00 = _5DBF_TAU_TDR00_VALUE;
	TO0 &= ~_0001_TAU_CH0_OUTPUT_VALUE_1;
	TOE0 &= ~_0001_TAU_CH0_OUTPUT_ENABLE;
	
	TMMK00 = 0U;	/* enable INTTM00 interrupt */
	TS0 |= _0001_TAU_CH0_START_TRG_ON;
}

/*
 *******************************************************************************
 *	�^�C�}3�����l
 *
 *	[���e]
 *		�^�C�}3�����l
 *******************************************************************************
 */
void R_TAU0_Channel3_Init( void )
{
	TMR03 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_16BITS_MODE | 
			_0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
	TMMK03 = 1U;							/* disable INTTM03 interrupt */
	TMIF03 = 0U;							/* clear INTTM03 interrupt flag */
	TDR03 = _5DBF_TAU_TDR03_VALUE;
	TMMK03 = 0U;							/* enable INTTM03 interrupt */
	TS0 |= _0008_TAU_CH3_START_TRG_ON;
}

/*
 *******************************************************************************
 *	�^�C�}3�҂����ԃZ�b�g���ăX�^�[�g
 *
 *	[���e]
 *		�^�C�}3�҂����ԃZ�b�g���ăX�^�[�g
 *	  93.8kHz
 *******************************************************************************
 */
void R_TAU0_Channel3_Custom( uint16_t TDR03_val )
{
	TMR03 = _C000_TAU_CLOCK_SELECT_CKM3 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_16BITS_MODE | 
			_0000_TAU_TRIGGER_SOFTWARE | _0000_TAU_MODE_INTERVAL_TIMER | _0000_TAU_START_INT_UNUSED;
	TMMK03 = 1U;							/* disable INTTM03 interrupt */
	TMIF03 = 0U;							/* clear INTTM03 interrupt flag */
	TDR03 = TDR03_val;
	TMMK03 = 0U;							/* enable INTTM03 interrupt */
	TS0 |= _0008_TAU_CH3_START_TRG_ON;
}

/*
 *******************************************************************************
 *	�^�C�}6�҂����ԃZ�b�g���ăX�^�[�g
 *
 *	[���e]
 *		�^�C�}6�҂����ԃZ�b�g���ăX�^�[�g
 *	  375kHz
 *******************************************************************************
 */
void R_TAU0_Channel6_Custom( uint16_t TDR06_val )
{
	TMMK06 = 1U;							/* disable INTTM06 interrupt */
	TMIF06 = 0U;							/* clear INTTM06 interrupt flag */
	TDR06 = TDR06_val;
	TMMK06 = 0U;	  /* enable INTTM06 interrupt */
	TS0 |= _0040_TAU_CH6_START_TRG_ON;
}


/*
 *******************************************************************************
 *	�^�C�}7�҂����ԃZ�b�g���ăX�^�[�g
 *
 *	[���e]
 *		�^�C�}7�҂����ԃZ�b�g���ăX�^�[�g
 *		
 *******************************************************************************
 */
void R_TAU0_Channel7_Custom( uint16_t TDR07_val )
{
	TMMK07 = 1U;							/* disable INTTM07 interrupt */
	TMIF07 = 0U;							/* clear INTTM07 interrupt flag */
	TDR07 = TDR07_val;						/* �J�E���^�Z�b�g */
	TMMK07 = 0U;							/* enable INTTM07 interrupt */
	TS0 |= _0080_TAU_CH7_START_TRG_ON;
}

#pragma section
/* End user code. Do not edit comment generated here */
