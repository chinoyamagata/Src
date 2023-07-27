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
* File Name    : r_cg_tau_user.c
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
#include "func.h"																/* �֐��̊O���Q��(extern)��` */
#include "glLabel.h"															/* �O���[�o���ϐ��E�萔(const)���x����` */
#include "enum.h"																/* �񋓌^�̒�` */
#include "cpu_sfr.h"
#include "immediate.h"
#include "UserMacro.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_tau0_channel0_interrupt(vect=INTTM00)
#pragma interrupt r_tau0_channel1_interrupt(vect=INTTM01)
#pragma interrupt r_tau0_channel2_interrupt(vect=INTTM02)
#pragma interrupt r_tau0_channel3_interrupt(vect=INTTM03)
#pragma interrupt r_tau0_channel4_interrupt(vect=INTTM04)
#pragma interrupt r_tau0_channel5_interrupt(vect=INTTM05)
#pragma interrupt r_tau0_channel6_interrupt(vect=INTTM06)
#pragma interrupt r_tau0_channel7_interrupt(vect=INTTM07)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* For TAU0_ch1 pulse measurement */
volatile uint32_t g_tau0_ch1_width = 0UL;
/* Start user code for global. Do not edit comment generated here */
static uint16_t vInitCnt = 0U;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_tau0_channel0_interrupt
* Description  : This function INTTM00 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel0_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	
	vInitCnt ++;
	if( gvBatt_Int == ecBattInt_LowLoop || vInitCnt >= 300U )
	{
		if( gvBatAd.mPhase == ecAdcPhase_Init )
		{
			gvModuleSts.mBat = ecBatModuleSts_Run;					/* ���d�rAD���� */
		}
		R_TAU0_Channel0_Stop();
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel1_interrupt
* Description  : This function INTTM01 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel1_interrupt(void)
{
	if (1U == (TSR01 & _0001_TAU_OVERFLOW_OCCURS))	  /* overflow occurs */
	{			 
		g_tau0_ch1_width = (uint32_t)(TDR01 + 1UL) + 0x10000UL;
	}
	else
	{
		g_tau0_ch1_width = (uint32_t)(TDR01 + 1UL);
	}
	/* Start user code. Do not edit comment generated here */
	EI();
	
	if( gvCycTestStart == imON )
	{
		gvCycTestStart = imOFF;
	}
	else
	{
	gvModuleSts.mErrChk |= imErrChk_CycChk;
		R_TAU0_Channel1_Stop();
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel2_interrupt
* Description  : This function INTTM02 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel2_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	static uint8_t			vKeyPortSts[ 2U ] = { 0U, 0U };
	static uint16_t			vKeyCnt = 0U;
	uint8_t					wkKeyKind;
	ET_KeyIntModuleSts_t	wkNextSts[ 2U ];
	
	EI();
	
	switch( gvModuleSts.mKeyInt )
	{
		case ecKeyIntModuleSts_ChatEventMode:
			wkKeyKind = KEY_EVENT;
			wkNextSts[ 0U ] = ecKeyIntModuleSts_EventMode;
			wkNextSts[ 1U ] = ecKeyIntModuleSts_Sleep;
			break;
			
		case ecKeyIntModuleSts_ChatDispMode:
			wkKeyKind = KEY_DISP;
			wkNextSts[ 0U ] = ecKeyIntModuleSts_RfMode;
			wkNextSts[ 1U ] = ecKeyIntModuleSts_DispMode;
			break;
			
		case ecKeyIntModuleSts_ChatResetMode:
			wkKeyKind = KEY_RESET;
			wkNextSts[ 0U ] = ecKeyIntModuleSts_ResetMode;
//			wkNextSts[ 0U ] = ecKeyIntModuleSts_RtmSndPtResetDspMode;
			wkNextSts[ 1U ] = ecKeyIntModuleSts_Sleep;
			break;
			
#if 0
		case ecKeyIntModuleSts_RtmSndPtResetDspMode:
			wkKeyKind = KEY_RESET;
			wkNextSts[ 0U ] = ecKeyIntModuleSts_RtmSndPtResetDspMode;
			wkNextSts[ 1U ] = ecKeyIntModuleSts_RtmSndPtResetMode;
			break;
#endif
			
		case ecKeyIntModuleSts_ResetMode:
			return;
		default:
			M_NOP;
			break;
	}
	
	vKeyCnt++;
	
	/* �{�^��������15ms�A20ms��̃|�[�g��� */
	if( vKeyCnt == 2U || vKeyCnt == 3U )
	{
		vKeyPortSts[ vKeyCnt - 2U ] = wkKeyKind;
	}
	
	/* �{�^��������20ms */
	if( vKeyCnt > imKeyChatCnt )
	{
		/* �L�[�`���^�����O���Ȃ� */
		if( vKeyPortSts[ 0U ] == imHigh && vKeyPortSts[ 1U ] == imHigh )
		{
			if( wkKeyKind == imHigh )
			{
				/* 1s������ */
				if( vKeyCnt > imKey1secPress )
				{
					R_TAU0_Channel2_Stop();
					vKeyCnt = 0U;
					gvModuleSts.mKeyInt = wkNextSts[ 0U ];
					}
			}
			else
			{
				R_TAU0_Channel2_Stop();
				vKeyCnt = 0U;
				gvModuleSts.mKeyInt = wkNextSts[ 1U ];
			}
		}
		else
		{
			R_TAU0_Channel2_Stop();
			vKeyCnt = 0U;
			gvModuleSts.mKeyInt = ecKeyIntModuleSts_Sleep;
		}
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel3_interrupt
* Description  : This function INTTM03 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel3_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
#if (swSensorCom == imEnable)
	/* ���j�b�g�N���^�C�}�v������ */
	if( gvModInf.mComSts == ecComSts_UnitPwnOnWait )				/* ���j�b�g�N���҂� */
	{
		R_TAU0_Channel3_Init();
		R_TAU0_Channel3_Stop();									/* �^�C�}0(CH3)��~ */
		gvModInf.mComSts = ecComSts_SndPre;						/* �R�}���h���M�ɏ�ԑJ�� */
		gvModuleSts.mCom = ecComModuleSts_Run;
	}
	
	/* �Z���T�ԒʐM���������� */
	ApiModbus_NoReplyJudge();
#endif
	
	/* ��M�� */
	if( gvModInf.mComSts == ecComSts_Rcv )
	{
		gvModInf.mRcvTmCnt++;										/* ��M�^�C�}�J�E���^�X�V */
		if( gvModInf.mRcvTmCnt > 5U )								/* ��M�^�C���A�E�g */
		{
			gvModInf.mRcvTmCnt = 0U;								/* ��M�^�C�}�J�E���^������ */
			R_TAU0_Channel3_Stop();
			gvModInf.mComSts = ecComSts_RcvAnalys;					/* ��M��͂֏�ԑJ�� */
			
			gvModInf.mRcvLen = gvModInf.mRcvPos;					/* ��M�f�[�^���X�V */
			gvModInf.mRcvPos = 0U;									/* ��M�ʒu��񏉊��� */
			gvModuleSts.mCom = ecComModuleSts_Run;
		}
	}
	else
	{
		gvModInf.mRcvTmCnt = 0U;									/* ��M�^�C�}�J�E���^������ */
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel4_interrupt
* Description  : This function INTTM04 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel4_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	uint8_t wkLoop;
	
	EI();														/* ���x��0,1,2���d�������� */
	
	/* �d�rAD�X�^�[�g */
	R_TAU0_Channel4_Stop();										/* �^�C�}�`���l��4��~ */
	R_ADC_Sel_DryBattCh();										/* ���d�r�p�A�i���O���̓`�����l���I�� */
	R_ADC_Set_OperationOn();									/* A/D�ϊ��ҋ@ */
	/* ��d������҂�(1us) */
	for( wkLoop = 0U; wkLoop < 20U; wkLoop++ )
	{
		M_NOP;
	}
	gvAdConvFinFlg = imOFF;										/* �d�rAD�����t���OOFF */
	gvBatAd.mRetryCnt = 0U;										/* �d�r�pAD���g���C�񐔏����� */
	
	R_ADC_Start();												/* A/D�ϊ��J�n */
	
	gvBatAd.mPhase = ecAdcPhase_AdExe;
	gvModuleSts.mBat = ecBatModuleSts_Run;						/* ���d�rAD���� */
	
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel5_interrupt
* Description  : This function INTTM05 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel5_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	R_TAU0_Channel5_Stop();
	ApiSetFlashSts( ecFlashSts_Init );
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel6_interrupt
* Description  : This function INTTM06 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel6_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	if( ApiRFDrv_GetSleepLoop() == 0U )
	{
		RTCMK = 0U;   /* enable INTRTC interrupt */
		gvRfIntFlg.mRadioTimer = 1U;
		gvRfIntFlg.mRadioStsMsk = 0U;
		gvModuleSts.mRf = ecRfModuleSts_Run;
		R_TAU0_Channel6_Stop();
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_tau0_channel7_interrupt
* Description  : This function INTTM07 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_tau0_channel7_interrupt(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	R_TAU0_Channel7_Stop();
	
	if( gvModuleSts.mRtc == ecRtcModuleSts_Wait )
	{
		gvModuleSts.mRtc = ecRtcModuleSts_Run;
	}
	
#if (swSensorCom == imEnable)
	if( gvModuleSts.mCom == ecComModuleSts_Wait )
	{
		gvModuleSts.mCom = ecComModuleSts_Run;
	}
#else
	if( gvModuleSts.mMea == ecMeaModuleSts_Wait )
	{
		gvModuleSts.mMea = ecMeaModuleSts_Run;
	}
#endif
	
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
#pragma section text MY_APP2
/*
 *******************************************************************************
 * �^�C�}0�`���l��1�̃J�E���^�擾
 *
 *	[���e]
 *		�^�C�}0�`���l��1�̃J�E���^�擾����B
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		uint32_t	g_tau0_ch1_width: �^�C�}0�`���l��1�̃J�E���^
 *******************************************************************************
 */
uint32_t ApiTau0_GetTau0Ch1Cnt( void )
{
	return g_tau0_ch1_width;
}

/*
 *******************************************************************************
 * �^�C�}0�`���l��0�̃^�C�}�擾
 *
 *	[���e]
 *		�^�C�}0�`���l��0�̃^�C�}���擾����B
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		uint16_t	vInitCnt: �^�C�}0�`���l��0�̃^�C�}
 *******************************************************************************
 */
uint16_t ApiTau0_GetTau0Ch0Time( void )
{
	return vInitCnt;
}

/*
 *******************************************************************************
 * �^�C�}0�`���l��0�̃^�C�}�N���A
 *
 *	[���e]
 *		�^�C�}0�`���l��0�̃^�C�}���N���A����B
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		uint16_t	vInitCnt: �^�C�}0�`���l��0�̃^�C�}
 *******************************************************************************
 */
void ApiTau0_GetTau0Ch0TimeClr( void )
{
	vInitCnt = 0U;
}

/*
 *******************************************************************************
 * �^�C�}0�`���l��0�ɂ��^�C�}�E�F�C�g
 *
 *	[���e]
 *		�^�C�}0�`���l��0�Ń^�C�}�E�F�C�Ƃ���
 *	[����]
 *		uint16_t arCnt:10msec/cnt
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiTau0_WaitTimer( uint16_t arCnt )
{
	R_TAU0_Channel0_Start();
	while( arCnt > ApiTau0_GetTau0Ch0Time() )
	{
		R_WDT_Restart();
	}
	R_TAU0_Channel0_Stop();
}

#pragma section
/* End user code. Do not edit comment generated here */