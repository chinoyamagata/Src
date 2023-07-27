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
#include "enum.h"																/* �񋓌^�̒�` */
#include "func.h"																/* �֐��̊O���Q��(extern)��` */
#include "glLabel.h"															/* �O���[�o���ϐ��E�萔(const)���x����` */
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
	/*** Reset�L�[�|�[�g ***/
	if( gvModuleSts.mKeyInt == ecKeyIntModuleSts_Sleep )
	{
		R_TAU0_Channel2_Start();								/* �`���^�����O�v���p�^�C�}�X�^�[�g */
		gvModuleSts.mKeyInt = ecKeyIntModuleSts_ChatResetMode;	/* Reset�L�[ */
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
	/*** �d�r��d�����o�|�[�g ***/
	
	/* �H��o�ׂ�INT�\���ȊO */
	if( gvModuleSts.mKeyInt != ecKeyIntModuleSts_ResetMode )
	{
		R_INTC5_Stop();										/* ��d�������֎~ */
	}
	
	/* L��H�d������ */
	if( M_TSTBIT(P0, M_BIT1) )
	{
		/* �d�r�������A�d�r�}�[�N���� */
		if( gvBatAd.mBattSts == ecBattSts_Non )
		{
			gvBatt_Int = ecBattInt_NonHigh;					/* �d�r�Ȃ�����̓d��H�������� */
			
			gvBatAd.mBattSts = ecBattSts_Init;
			gvModuleSts.mLcd = ecLcdModuleSts_Run;			/* ��ʍX�V */
			
			ApiTau0_GetTau0Ch0TimeClr();					/* ���d�rAD����(3secWait��) */
			R_TAU0_Channel0_Start();
	}
	else
	{
			gvBatt_Int = ecBattInt_High;					/* �d�r�d��High���o�����ݔ��� */
			
			gvBatAd.mPhase = ecAdcPhase_CtlWait;
			R_PORT_BattCtrlOnOff( imON );					/* ���d�r�pA/D�R���g���[���p�|�[�gON */
			R_TAU0_Channel4_Start();						/* �^�C�}�`���l��4�J�n */
		}
	}
	/* H��L�d������ */
	else
	{
		gvBatt_Int = ecBattInt_Low;							/* �d�r�d��Low���o�����ݔ��� */
	
		/* �d�r0�ȊO */
		if( gvBatAd.mBattSts != ecBattSts_Non && gvBatAd.mBattSts != ecBattSts_Empty )
	{
			gvBatAd.mPhase = ecAdcPhase_CtlWait;
			R_PORT_BattCtrlOnOff( imON );					/* ���d�r�pA/D�R���g���[���p�|�[�gON */
			R_TAU0_Channel4_Start();						/* �^�C�}�`���l��4�J�n */
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
