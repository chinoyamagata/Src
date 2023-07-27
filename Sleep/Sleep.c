/*
 *******************************************************************************
 *	File name	:	Sleep.c
 *
 *	[���e]
 *		�X���[�v����
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2019.10
 *******************************************************************************
 */
#include "cpu_sfr.h"															/* SFR��`�w�b�_�[ */

#include "typedef.h"															/* �f�[�^�^��` */
#include "UserMacro.h"															/* ���[�U�쐬�}�N����` */
#include "immediate.h"															/* immediate�萔��` */
#include "switch.h"																/* �R���p�C���X�C�b�`��` */
#include "enum.h"																/* �񋓌^�̒�` */
#include "struct.h"																/* �\���̒�` */
#include "func.h"																/* �֐��̊O���Q��(extern)��` */
#include "glLabel.h"															/* �O���[�o���ϐ��E�萔(const)���x����` */

#if (swSensorCom == imEnable)
#include "r_cg_sau.h"															/* �V���A���A���C���j�b�g�w�b�_�[��` */
#endif
/*
 *==============================================================================
 *	define��`
 *==============================================================================
 */
#define imSTOP_OK	1U
#define imSTOP_NG	0U

/*
 *==============================================================================
 *	�񋓌^��`
 *==============================================================================
 */


/*
 *==============================================================================
 *	�\���̒�`
 *==============================================================================
 */



/*
 *==============================================================================
 *	�ϐ���`
 *==============================================================================
 */


/*
 *==============================================================================
 *	�萔��`
 *==============================================================================
 */


/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
 */
static uint8_t SubSleep_StopPeripheral( void );									/* ���Ӌ@�\�̒�~���� */
//static void SubSleep_RestartPeripheral( void );									/* ���Ӌ@�\�̍ĊJ���� */


#pragma section text MY_APP2
#if 0
/*
 *******************************************************************************
 *	�X���[�v���� ����������
 *
 *	[���e]
 *		�X���[�v�����̏������������s���B
 *******************************************************************************
 */
void ApiSleep_Initial( void )
{
	gvModuleSts.mRtcInt = ecRtcIntSts_Sleep;
	gvModuleSts.mExe = ecExeModuleSts_Meas;
#if (swSensorCom == imEnable)
	gvModuleSts.mMea = ecMeaModuleSts_Sleep;
#else
	gvModuleSts.mMea = ecMeaModuleSts_Run;
#endif
	gvModuleSts.mKeyInt = ecKeyIntModuleSts_Sleep;
	gvModuleSts.mEventKey = ecEventKeyModuleSts_Sleep;
	gvModuleSts.mBat = ecBatModuleSts_Run;
#if (swSensorCom == imEnable)
	gvModuleSts.mCom = ecComModuleSts_Run;
#else
	gvModuleSts.mCom = ecComModuleSts_Sleep;
#endif
	gvModuleSts.mRf = ecRfModuleSts_Run;
	gvModuleSts.mRtc = ecRtcModuleSts_Sleep;
	gvModuleSts.mExFlash = ecExFlashModuleSts_Sleep;
	gvModuleSts.mLcd = ecLcdModuleSts_Run;
	gvModuleSts.mErrChk = imErrChk_Sleep;
	gvModuleSts.mExFlashRd = ecExFlashRdModuleSts_Sleep;
	gvModuleSts.mInFlash = ecInFlashWrExeSts_Sleep;
	gvModuleSts.mHistory = ecHistoryModuleSts_Sleep;
	gvModuleSts.mFirmup = ecFirmupModuleSts_Sleep;
}
#endif

/*
 *******************************************************************************
 *	�X���[�v�������C��
 *
 *	[���e]
 *		�X���[�v�������C��
 *******************************************************************************
 */
void ApiSleep_Main( void )
{
	uint8_t		wkRet;
	
	if( (gvModuleSts.mRtcInt == ecRtcIntSts_Sleep)				&&
		(gvModuleSts.mExe == ecExeModuleSts_Sleep)				&&
//		(gvModuleSts.mMea == ecMeaModuleSts_Sleep)				&&
//		(gvModuleSts.mKeyInt == ecKeyIntModuleSts_Sleep)		&&
		(gvModuleSts.mEventKey == ecEventKeyModuleSts_Sleep)	&&
		(gvModuleSts.mBat == ecBatModuleSts_Sleep)				&&
//		(gvModuleSts.mCom == ecComModuleSts_Sleep)				&&
		(gvModuleSts.mRf == ecRfModuleSts_Sleep)				&&
		(gvModuleSts.mRtc == ecRtcModuleSts_Sleep)				&&
		(gvModuleSts.mExFlash == ecExFlashModuleSts_Sleep)		&&
		(gvModuleSts.mLcd == ecLcdModuleSts_Sleep)				&&
		(gvModuleSts.mErrChk == imErrChk_Sleep)					&&
		(gvModuleSts.mExFlashRd == ecExFlashRdModuleSts_Sleep)	&&
		(gvModuleSts.mHistory == ecHistoryModuleSts_Sleep)			)
	{
		if( gvRfIntFlg.mRadioStsMsk )
		{
			M_HALT;
		}
		else
		{
			/* �����p�^�C�}6���쒆�Ȃ�HALT */
			/* �L�[�����^�C�}2���쒆�Ȃ�HALT */
			/* �d�r������AD�܂ł̃E�F�C�g�^�C�}0���쒆�Ȃ�HALT */
			/* �U���q�f�f�^�C�}1���쒆�Ȃ�HALT */
			if( TMMK00 == 0U || TMMK01 == 0U || TMMK02 == 0U || TMMK03 == 0U || TMMK04 == 0U || TMMK05 == 0U || TMMK06 == 0U || TMMK07 == 0U )
			{
				M_HALT;
			}
			else
			{
				wkRet = SubSleep_StopPeripheral();			/* ���Ӌ@�\�̒�~ */
				if( wkRet == imSTOP_OK )
				{
					M_STOP;
				}
				else
				{
					M_HALT;
				}
			}
		}
	}
}


/*
 *******************************************************************************
 *	���Ӌ@�\�̒�~����
 *
 *	[���e]
 *		�X���[�v�Ɉڍs����O�ɍs�����Ӌ@�\�̒�~����
 *******************************************************************************
 */
static uint8_t SubSleep_StopPeripheral( void )
{
	uint8_t wkRet = imSTOP_NG;
	
	DI();							/* �����݋֎~ */
									/* ���̏��������s����钼�O�Ɋ����������Ď��Ӌ@�\��ON��ԂɂȂ�A���̏����Ŏ��Ӌ@�\����~������ʂ��������� */
	
	/* �^�C�}�E�A���C�E���j�b�g */
	if( TMMK00 != 0U )
	{
		R_TAU0_Channel0_Stop();			/* �d�r�����E�F�C�g/�H���pModbus�ʐM�^�C�} */
		
		if( TMMK01 != 0U )
		{
			R_TAU0_Channel1_Stop();			/* ���U�ُ팟���^�C�} */
			
			if( TMMK02 != 0U )
			{
				R_TAU0_Channel2_Stop();			/* �L�[����Ď��^�C�} */
				
				if( TMMK03 != 0U )
				{
					R_TAU0_Channel3_Stop();			/* ���j�b�g�ԒʐM�^�C�} */
					
					if( TMMK04 != 0U )
					{
						R_TAU0_Channel4_Stop();			/* �d�r�d��AD�X�^�[�g���̃^�C�} */
						
						if( TMMK05 != 0U )
						{
							R_TAU0_Channel5_Stop();			/* �O�t��Flash�d��ON���̃E�F�C�g�p�^�C�} */
							
							if( TMMK06 != 0U )
							{
								R_TAU0_Channel6_Stop();			/* �����p�^�C�} */
								
								if( TMMK07 != 0U )
								{
									R_TAU0_Channel7_Stop();			/* RTC�A���菈���ł̃E�F�C�g�p�^�C�} */

									/* A/D�R���o�[�^ */
									R_ADC_Stop();
									R_ADC_Set_OperationOff();

									/* LCD�R���g���[���^�h���C�o */
//									R_LCD_Stop();							/* LCD�R���g���[���^�h���C�o�����~ */
//									R_LCD_Voltage_Off();					/* �e�ʕ�����H�����~ */

									/* ���A���^�C���N���b�N */
//									R_RTC_Sleep();							/* ����J�n���HALT/STOP���[�h�ւ̈ڍs���� */
//									R_RTC_Stop();

									/* ���荞�� */
//									R_INTC0_Stop();
//									R_INTC1_Stop();
//									R_INTC3_Stop();
//									R_INTC4_Stop();
//									R_INTC5_Stop();
//									R_INTC6_Stop();

//									R_CSI10_Stop();
//									R_IIC00_Stop();
//									R_UART2_Stop();
//									R_KEY_Stop();
//									R_IT_Stop();
									
									wkRet = imSTOP_OK;
								}
							}
						}
					}
				}
			}
		}
	}
	
	EI();

	return wkRet;
}

#if 0
/*
 *******************************************************************************
 *	���Ӌ@�\�̍ĊJ����
 *
 *	[���e]
 *		�X���[�v�Ɉڍs��ɍs�����Ӌ@�\�̍ĊJ����
 *******************************************************************************
 */
static void SubSleep_RestartPeripheral( void )
{
	/*
	 ***************************************
	 *	LCD�R���g���[���^�h���C�o(�g�p����)
	 ***************************************
	 */
//	R_LCD_Start();							/* LCD�R���g���[���^�h���C�o����J�n */
//	R_LCD_Voltage_On();						/* �e�ʕ�����H���싖�� */

	/*
	 ***************************************
	 *	���A���^�C���N���b�N(�g�p����)
	 ***************************************
	 */
//	R_RTC_Start();
}
#endif
#pragma section


