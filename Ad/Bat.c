/*
 *******************************************************************************
 *	File name	:	Bat.c
 *
 *	[���e]
 *		�o�b�e���[��ԕω����̏���
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2019.09
 *******************************************************************************
 */
#if 0
#include "cpu_sfr.h"															/* SFR��`�w�b�_�[ */

#include "typedef.h"															/* �f�[�^�^��` */
#include "UserMacro.h"															/* ���[�U�[�쐬�}�N����` */
#include "immediate.h"															/* immediate�萔��` */
#include "switch.h"																/* �R���p�C���X�C�b�`��` */
#include "enum.h"																/* �񋓌^�̒�` */
#include "struct.h"																/* �\���̒�` */
#include "func.h"																/* �֐��̊O���Q��(extern)��` */
#include "glLabel.h"															/* �O���[�o���ϐ��E�萔(const)���x����` */
#endif


/*
 *==============================================================================
 *	define��`
 *==============================================================================
 */
//#define imBatLowTime_1min		60U											/* ��d�����ԁF60�b(1��) */


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
#if 0
static uint16_t vOldAllTime;								/* ��d���������̕b��(���{�b) */
#endif


/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
 */


#pragma section text MY_APP2
#if 0
/*
 *******************************************************************************
 *	�o�b�e������ ����������
 *
 *	[���e]
 *		�o�b�e������ ����������
 ****************************************************** */
void ApiBat_Initial( void )
{
	gvBatLowSts = ecLowBatFlashSts_Init;
}
#endif

#if 0
/*
 *******************************************************************************
 *	�o�b�e���ω����̏���
 *
 *	[���e]
 *		�o�b�e���ω����̏���
 *******************************************************************************
 */
void ApiBatChgMain( void )
{
	/* �d�r�F�Ȃ������� */
	if( gvModuleSts.mBatChg == ecBatChgModuleSts_BatOn )
	{
		if( imON == ApiAbn_AbnStsClr( imAbnSts_BatEmpty, ecAbnInfKind_AbnSts ) )
			{
			ApiFlash_WriteErrInfo( ecErrInfItm_BatEmpty, imOFF );			/* �@��ُ헚���F�d�r�d��(��)���A */
			ApiFlash_WriteActLog( ecActLogItm_BattIn, 0U, 4U );				/* ���엚���F�d�r���� */
		}
		
		gvBatLowSts = ecLowBatFlashSts_Ret;									/* ���^��RAM�ɋL�^����̕��A */
	}
	/* �d�r�F���聨�Ȃ� */
	else
	{
		ApiRtcDrv_InRtcToExRtc();						/* ����RTC�̎������O�t��RTC�ɏ����� */
		
		ApiInFlash_ParmWrite( ecInFlashWrSts_ParmNow, 0U );					/* �����_�̕ϐ��A���^�A�h���X�e�[�u�������Flash�փ��C�g */
		
		if( imON == ApiAbn_AbnStsSet( imAbnSts_BatEmpty, ecAbnInfKind_AbnSts ) )
		{
			ApiFlash_WriteErrInfo( ecErrInfItm_BatEmpty, imON );			/* �@��ُ헚���F�d�r�d��(��)���� */
			ApiFlash_WriteActLog( ecActLogItm_BattBackup, 0U, 4U );			/* ���엚���F�d�r�����o�b�N�A�b�v��� */
		}
		vOldAllTime = ( gvClock.mMin * 60U ) + gvClock.mSec;
			
		gvBatLowSts = ecLowBatFlashSts_Now;									/* ���^��RAM�ɋL�^�� */
		}
	
		gvModuleSts.mBatChg = ecBatChgModuleSts_Sleep;
	}
#endif

#if 0
/*
 *******************************************************************************
 *	��d�����̓����~���菈��
 *
 *	[���e]
 *		��d�����̏ꍇ�A1���Ԃ̌v�����s���B�o�ߎ��͉�ʂ̏������s���B
 *		�܂��A��d���p�̃L���[���t����ԂƂȂ����ꍇ�ɂ���ʂ̏������s���B
 *******************************************************************************
 */
void ApiBatLowStopMotionJdg( void )
{
	uint16_t	wkDiff;
	ET_Error_t	wkRet;
	
	/* �S�b�ɕϊ� */
	wkDiff = ((gvClock.mMin * 60U) + gvClock.mSec) - vOldAllTime;
	
	/* ��d���p�L���[�ɋ󂫂����邩�擾 */
	wkRet = ApiFlash_GetEmptyQueueJdg();
	
	
	/* H�d�� & ��d�������݋��� */
	if( M_TSTBIT(P0, M_BIT1) && PMK5 == 0U )
	{
		R_INTC5_Stop();											/* ��d�������֎~ */
		gvBatt_Int = ecBattInt_High;							/* �d�r�d��High���o�����ݔ��� */
		
		/* �d�r�������A�d�r�}�[�N���� */
		if( gvBatAd.mBattSts == ecBattSts_Non || gvBatAd.mBattSts == ecBattSts_Empty )
		{
			gvBatAd.mBattSts = ecBattSts_Init;
			gvModuleSts.mLcd = ecLcdModuleSts_Run;				/* ��ʍX�V */
		}
		
		ApiTau0_GetTau0Ch0TimeClr();							/* ���d�rAD����(3secWait��) */
		R_TAU0_Channel0_Start();
		
	}
	
	
	/* ��d���������ԂƔ�r */
	if( (wkDiff >= imBatLowTime_1min) ||						/* 60�b�o�߂����ꍇ */
		(wkRet == ecERR_NG) )									/* ��d���p�̃L���[�ɋ󂫂��Ȃ��ꍇ */
	{
		/* �S�������� */
		ApiLcd_LcdStop();														/* LCD�h���C�o��~���� */
		
		while( 1U )
		{
			/* �d�r������������ */
			if( gvBatt_Int == ecBattInt_High )
			{
				break;
			}
			M_HALT;
	}
		WDTE = 0xFFU;											/* �������Z�b�g */
}
}
#endif

#pragma section
