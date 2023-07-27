/*
 *******************************************************************************
 *	File name	:	main.c
 *
 *	[���e]
 *		���C�����[�`��
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2019.
 *******************************************************************************
 */
#include "cpu_sfr.h"															/* SFR��`�w�b�_�[ */
#include "typedef.h"															/* �f�[�^�^��` */
#include "UserMacro.h"															/* ���[�U�[�쐬�}�N����` */
#include "immediate.h"															/* immediate�萔��` */
#include "switch.h"																/* �R���p�C���X�C�b�`��` */
#include "enum.h"																/* �񋓌^�̒�` */
#include "struct.h"																/* �\���̒�` */
#include "func.h"																/* �֐��̊O���Q��(extern)��` */
#include "glLabel.h"															/* �O���[�o���ϐ��E�萔(const)���x����` */
#include "RF_Immediate.h"

/*
 *==============================================================================
 *	�񋓌^��`
 *==============================================================================
 */



/*
 *==============================================================================
 *	�ϐ���`
 *==============================================================================
 */


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	���C�����[�`��
 *
 *	[���e]
 *		���C�����[�`��
 *******************************************************************************
 */
void ApiMain( void )
{
	uint8_t		wkRet;
//	uint8_t		wkRet2;
//	uint8_t		wkFlg = 0;
	
#if 0
	uint32_t	wkLoop;
	uint32_t	wkAdr;
	uint8_t		wkRdData[ 36U ];
	uint32_t	wkPreU32;
#endif
	
#if swRfPortDebug == imEnable
	/* Event Key��Test Port�� */
	PFSEG2 &= 0xFEU;
	PM7 &= 0xEFU;
	P7_bit.no4 = 0U;
	P7_bit.no4 = 1U;
#endif
	
	/* �ʃp�����[�^���������� */
	InitParameter();
	
	/* �����݋��� */
	EI();
	
	/* ��d�������֎~ */
	R_INTC5_Stop();
	
#if (swRESFDisp == imEnable)
	gvResf = RESF;
#endif
	
	/* �N�����犣�d�rAD����̃E�F�C�g(�N�����炱���܂�175msec)+300msec */
	ApiTau0_WaitTimer( 30U );
	
	/* ���d�rAD���� */
	while( gvModuleSts.mBat == ecBatModuleSts_Run )
	{
		ApiAdc_Main( 0U );
	}
	
	/* 1.8V���� or ��d���|�[�gLo */
	if( gvBatAd.mDryBattAdCnt < imAdCnt_1800mV || !M_TSTBIT(P0, M_BIT1) )
	{
		ApiLcd_UppLow7SegDsp(" LO", imHigh );
		ApiLcd_UppLow7SegDsp("BAT", imLow );
		ApiLcd_Main();
		
		/* LoBat�\�� */
		while( 1U )
		{
			R_WDT_Restart();			/* WDT�N���A */
		}
	}
	
	/* �ݒ�l�W�J����(�����t���b�V���̐ݒ�l��RAM�ɓW�J) */
	ApiInFlash_PwrOnRead();
	
	/* �t���O�m�F���e���� */
	ApiInFlash_ParmInitFlgChk();
	
	/* �������x�擾 */
	gvInTemp = ApiRfDrv_Temp() + gvInFlash.mProcess.mRfTmpHosei;
	
	
	/* �L���p�V�^�[�d�̂��ߓd�r���莞�ԁF3sec */
	ApiTau0_WaitTimer( 300U );
	
#if 0	/* ID�������� */
//	gvInFlash.mParam.mOnCertLmt = 0x01;

//	gvInFlash.mParam.mGroupID = 0x01;
//	gvInFlash.mProcess.mOpeCompanyID = 0x01;
//	gvInFlash.mParam.mAppID[ 0U ] = 0x0F;
//	gvInFlash.mParam.mAppID[ 1U ] = 0xFF;
	
//	gvInFlash.mProcess.mFirstAcsChk = 0xA5;
	gvInFlash.mProcess.mUniqueID[ 0U ] = 0x00;
	gvInFlash.mProcess.mUniqueID[ 1U ] = 0x00;
	gvInFlash.mProcess.mUniqueID[ 2U ] = 0x03;
	
//	gvInFlash.mParam.mrfHsCh = 2;
//	gvInFlash.mParam.mrfLoraChGrupeCnt = 24;
//	gvInFlash.mParam.mLogCyc1 = ecRecKind1_5sec;

	ApiInFlash_ParmWrite( ecInFlashWrSts_ParmNow, 0U );		/* ����Flash�փ��C�g */
#endif
	
	/*** RTC�C�j�V�����C�Y ***/
	/* RTC(S-35390A) ������ */
	ApiRtcDrv_Initial();
	
	/* �O�t��RTC�̎��������RTC�ɏ����� */
	ApiRtcDrv_ExRtcToInRtc();
	
	/* ROM_CRC�`�F�b�N(�����擾��) */
	ApiAbn_ChkCRC();
	
	/*** ����IC������ ***/
	while( 1 )
	{
		if( gvModuleSts.mRf == ecRfModuleSts_Run && (ecRFDrvInitSts_InitEnd != ApiRFDrv_GetInitSts()) )
		{
			ApiRFDrv_Initial();
		}
		
		/* RF(�����ʐM)�̏��������� */
		if( ecRFDrvInitSts_InitEnd == ApiRFDrv_GetInitSts() )
		{
			break;
		}
	}
	
	/*** �O�t��Flash�C�j�V�����C�Y ***/
	ApiFlash_SrchFlashAdr();		/* Flash�������݂̑������������� */
	
#if 0	/* ����d�̓e�X�g */
	PM4_bit.no5 = (uint8_t)imLow;											/* �t���b�V���d������|�[�g�F�o�� */
	P4_bit.no5 = (uint8_t)imHigh;											/* �t���b�V���d������F�d��OFF */
	P12_bit.no5 = (uint8_t)imLow;											/* �`�b�v�Z���N�g(�v���C�}��)��Low */
	P6_bit.no1 = (uint8_t)imLow;											/* �`�b�v�Z���N�g(�Z�J���_��)��Low */
	gvModuleSts.mRtc = ecRtcModuleSts_Sleep;
	gvModuleSts.mRtcInt = ecRtcIntSts_Sleep;
	R_RTC_Stop();
	R_IT_Stop();
	
	R_TAU0_Channel0_Stop();			/* �H���pModbus�ʐM�^�C�} */
	R_TAU0_Channel1_Stop();			/* ���U�ُ팟���^�C�} */
	R_TAU0_Channel2_Stop();			/* �L�[����Ď��^�C�} */
	R_TAU0_Channel3_Stop();			/* �T�[�~�X�^����^�C�} */
	R_TAU0_Channel4_Stop();			/* �d�r�d��AD�X�^�[�g���̃^�C�} */
	R_TAU0_Channel5_Stop();			/* �O�t��Flash�d��ON���̃E�F�C�g�p�^�C�} */
	R_TAU0_Channel6_Stop();			/* �����p�^�C�} */
	R_TAU0_Channel7_Stop();			/* RTC�A���菈���ł̃E�F�C�g�p�^�C�} */
	
	R_ADC_Stop();
	R_ADC_Set_OperationOff();

//	R_LCD_Stop();																/* LCD�R���g���[���^�h���C�o�����~ */
//	R_LCD_Voltage_Off();														/* �e�ʕ�����H�����~ */

//	R_RTC_Sleep();																/* ����J�n���HALT/STOP���[�h�ւ̈ڍs���� */
//	R_RTC_Stop();

	R_COMP0_Stop();
	
	R_INTC0_Stop();
	R_INTC1_Stop();
	R_INTC3_Stop();
	R_INTC4_Stop();
	R_INTC5_Stop();
	R_INTC6_Stop();
	
	R_CSI10_Stop();
	R_IIC00_Stop();
	R_UART2_Stop();
	R_KEY_Stop();
	R_IT_Stop();
	
	M_STOP;																	/* STOP���[�h�ֈڍs */
#endif
	
//	gvInFlash.mProcess.mModelCode = ecSensType_HumTmp;
	
#if 0	/* �O�t��Flash�e�X�g */
	ApiFlash_FlashPowerCtl( imON, imON );
	wkAdr = 0x04000;
	for( wkLoop = 0 ; wkLoop < 0xFFFFF; )
	{
		R_WDT_Restart();
		ApiFlashDrv_ReadData( wkAdr, &wkRdData[0], 12, ecFlashKind_Prim );
	
		wkU32 = (uint32_t)wkRdData[0] << 24;
		wkU32 += (uint32_t)wkRdData[1] << 16;
		wkU32 += (uint32_t)wkRdData[2] << 8;
		wkU32 += (uint32_t)wkRdData[3];
		wkU32 >>= 2;
		
		if( wkU32 != wkPreU32 + 10 && wkU32 != 0x3fffffff )
		{
			M_NOP;
		}
		wkPreU32 = wkU32;
		
		if( (wkAdr & 0xFFF) == 0xFF0)
		{
			wkAdr += 16;
		}
		else
		{
			wkAdr += 12;
		}
	}
//	ApiFlash_FlashPowerCtl( imOFF, imON );
//	ApiFlashDrv_FirmMemErase( ecFlashKind_Prim );
#endif
	
	/* �Ō�IndexNo.�A�������[�h */
	ApiFlash_ReadQueSetPastTime();
	
	/* �d���������A���^����Ă���ŐV���^�f�[�^�𖳐����M�p�ɃZ�b�g���� */
	ApiFlash_SetNewMeasVal();
	
	/* ���O������RTC�������r��RTC�����𐧌� */
	ApiTimeComp_LimitRtcClock();
	
	/* �N������ */
	ApiFlash_WriteActLog( ecActLogItm_Boot, 0U, 4U );
	
	/* �t�@�[���X�V���� */
	ApiFirmUpdate_History();
	
	/* �d�r�d��(��)���A */
	ApiAbn_AbnStsClr( imAbnSts_BatEmpty, ecAbnInfKind_AbnSts );
	
	
	/* ���d�rAD���� */
	gvModuleSts.mBat = ecBatModuleSts_Run;
	while( gvModuleSts.mBat == ecBatModuleSts_Run )
	{
		ApiAdc_Main( 1U );
	}
	gvModuleSts.mBat = ecBatModuleSts_Run;
	
	/* �ُ픭���A���ُ͈�p�����Ă�����Err�\�� */
	ApiAbn_AbnNumSet();
	
	/* ��d���������� */
	R_INTC5_Start();
	
	while (1)
	{
		/* WDT�N���A */
		R_WDT_Restart();
		
		/* RTC��������� */
		if( gvModuleSts.mRtcInt == ecRtcIntSts_Run )
		{
			ApiRtcInt();
			
			/* ���^��RAM�ɋL�^�� */
			if( gvBatLowSts == ecLowBatFlashSts_Now )
			{
				ApiAdc_LowStopMotionJdg();
			}
			
		}
		
		/* �L�[���͏��� */
		if( (gvModuleSts.mKeyInt >= ecKeyIntModuleSts_EventMode) &&
			(gvModuleSts.mKeyInt <= ecKeyIntModuleSts_ResetMode) )
		{
			ApiKey();
		}
		
		/* �n�[�h�G���[����`�F�b�N */
		if( gvModuleSts.mErrChk != imErrChk_Sleep )
		{
			ApiAbn_Chk();
		}
		
#if (swSensorCom == imEnable)
		/* ��ʒʐM(�����x�v��) */
		if( gvModuleSts.mCom == ecComModuleSts_Run )
		{
			ApiModbus_Main();
		}
#else
		/* ���� */
		if( gvModuleSts.mMea == ecMeaModuleSts_Run )
		{
			ApiMeas_Main();
		}
#endif
		
		/* ���v�X�V */
		if( (gvModuleSts.mRtc == ecRtcModuleSts_UpdateLose) ||
			(gvModuleSts.mRtc == ecRtcModuleSts_UpdateGain) )
		{
			/* ����RTC�Ɏ������C�g */
			ApiRtcDrv_SetInRtc( gvClock );
			gvModuleSts.mLcd = ecLcdModuleSts_Run;
			gvModuleSts.mRtc = ecRtcModuleSts_Sleep;
		}
		
		/* �������� */
		if( gvModuleSts.mRf == ecRfModuleSts_Run )
		{
			ApiRFDrv_Main();
		}
		
		/* �x�񏈗� */
		if( (gvModuleSts.mExe == ecExeModuleSts_Alm1) ||
			(gvModuleSts.mExe == ecExeModuleSts_Alm2) ||
			(gvModuleSts.mEventKey == ecEventKeyModuleSts_Alm) )
//			(gvAlmClrPoint & (imAlmClrPoint_Time | imAlmClrPoint_FlgCng)) )
		{
			ApiAlarm_Main();
			
			/* �O�t��Flash�ւ̏����� */
			if( gvModuleSts.mExe == ecExeModuleSts_Alm1 )
			{
				gvModuleSts.mExe = ecExeModuleSts_ExFlsWr1;				/* �v���l�����ݎ��s */
			}
			else if( gvModuleSts.mExe == ecExeModuleSts_Alm2 )
			{
				gvModuleSts.mExe = ecExeModuleSts_Sleep;				/* �I�� */
			}
			
			if( gvModuleSts.mEventKey == ecEventKeyModuleSts_Alm )
			{
				gvModuleSts.mEventKey = ecEventKeyModuleSts_ExFlsWr;	/* �C�x���g�l�����ݎ��s */
			}
		}
		
		/* �\������ */
		if( gvModuleSts.mLcd != ecLcdModuleSts_Sleep )
		{
#if (swDebugLcd == imEnable)
			ApiLcd_LcdDebug();
#endif
#if (swRfTestLcd == imEnable)
			if( M_TSTFLG(gvRfTestLcdFlg) )
			{
				ApiHmi_Main();
			}
			else
			{
				gvModuleSts.mLcd = ecLcdModuleSts_Sleep;
			}
#else
			ApiHmi_Main();
#endif
		}
		
		/* ����Flash���C�g */
		if( gvModuleSts.mInFlash == ecInFlashWrExeSts_Run )
		{
			/* ����Flash�֌��݂̃p�����[�^�����C�g */
			ApiInFlash_ParmWrite( ecInFlashWrSts_ParmNow, 0U );
			gvModuleSts.mInFlash = ecInFlashWrExeSts_Sleep;
		}
		
		/* �O�t��Flash���C�g�f�[�^�Z�b�g(����l) */
		if( gvModuleSts.mExe == ecExeModuleSts_ExFlsWr1 )
		{
			ApiFlash_WriteMeasVal1Event( 0U );								/* ����l1�Z�b�g */
			gvModuleSts.mExe = ecExeModuleSts_Sleep;
		}
		
		/* �O�t��Flash���C�g�f�[�^�Z�b�g(�C�x���g) */
		if( gvModuleSts.mEventKey == ecEventKeyModuleSts_ExFlsWr )
		{
			ApiFlash_WriteMeasVal1Event( 1U );								/* �L�[�������̑���l�Z�b�g */
			gvModuleSts.mEventKey = ecEventKeyModuleSts_Sleep;
		}
		
		
		/* Flash����( AD�����������(AD���s���łȂ�) and AD�ʏ퓮��(�d�r�d�������łȂ�) and ���^��RAM�ɋL�^���łȂ�) */
		if( (gvBatAd.mPhase == ecAdcPhase_Init) && (gvBatt_Int == ecBattInt_Init) && (gvBatLowSts == ecLowBatFlashSts_Init) )
		{
			wkRet = ApiFlash_GetQueSts();									/* �L���[�擾 */
			/* �L���[�ɒ��߂��Ă����ꍇ */
			if( wkRet == imARI )
			{
				/* �d�r��d���|�[�gLo and AD�ʏ퓮��(�d�r�d�������łȂ�)*/
				if( !M_TSTBIT(P0, M_BIT1) && (gvBatt_Int == ecBattInt_Init) )
				{
					gvBatt_Int = ecBattInt_LowFlashWr;						/* ��d��Lo��Flash���C�g���O */
					gvModuleSts.mBat = ecBatModuleSts_Run;					/* ���d�rAD���� */
				}
				else
				{
					gvModuleSts.mExFlash = ecExFlashModuleSts_Run;				/* �O�t���t���b�V��RUN��Ԃֈڍs */
					ApiFlash_Main();											/* �O�t���t���b�V�����C������ */
				}
			}
			else
			{
				gvModuleSts.mExFlash = ecExFlashModuleSts_Sleep;			/* �X���[�v��Ԃֈڍs */
			}
		}
		
		/* �O�t��Flash����̃��[�h */
		if( gvModuleSts.mExFlashRd == ecExFlashRdModuleSts_Run )
		{
			ApiFlash_FinReadData();
		}
		
		/* �ݒ�ύX���e�̓��엚����Flash���C�g�L���[�Ɋi�[ */
		if( gvModuleSts.mHistory == ecHistoryModuleSts_Run )
		{
			ApiFlash_StoreActHist();
		}
		
		
		/* ���d�rAD���� */
		if( gvModuleSts.mBat == ecBatModuleSts_Run )
		{
			/* �H���̃T�u�N���b�N1Hz�o�͎� and ����p�R���p���[�^ON���͓���Ȃ� */
			if( ApiRtc1HzCtlRead() != imON && C0ENB == 0U )
			{
				ApiAdc_Main( 1U );
			}
		}
		
		
		/* �t�@�[���A�b�v */
		if( ( gvModuleSts.mFirmup == ecFirmupModuleSts_Run ) && ( ApiGetFlashSts() == ecFlashSts_PowOn) )
		{
			ApiFlash_FlashPowerCtl( imON, imON );					/* �O�t��Flash�̓d��ON */
			ApiFirmUpdateMain();
		}
		
#if (swSensorCom == imDisable)
		/* ��ʒʐM */
		if( gvModuleSts.mCom == ecComModuleSts_Run )
		{
			ApiModbus_Main();
		}
#endif
#if 0
		if( gvClock.mSec % 10 == 0U )
		{
			if( wkFlg == 0 )
			{
				wkFlg = 1;
				ApiFlash_WriteErrInfo( ecErrInfItm_KeyBehErr, imON );
//				ApiFlash_WriteActLog( ecActLogItm_RfAirplaneOn, 0U, 4U );
//				ApiFlash_WriteSysLog( ecSysLogItm_KeyDisp, 0U );
			}
		}
		else
		{
			wkFlg = 0;
		}
#endif
		
		/* �X�^���o�C���[�h�ڍs */
		ApiSleep_Main();
	}
}
#pragma section


