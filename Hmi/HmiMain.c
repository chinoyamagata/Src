/*
 *******************************************************************************
 *	File name	:	HmiMain.c
 *
 *	[���e]
 *		���[�U�C���^�[�t�F�[�X����
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2017.02.27		Softex K.U		�V�K�쐬
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



/*
 *==============================================================================
 *	define��`
 *==============================================================================
 */


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
#if (swSensorCom == imEnable)
static uint8_t	vBellMarkSetFlg[ 2U ] = { imOFF, imOFF };						/* �x���}�[�N�t���O */
#else
static uint8_t	vBellMarkSetFlg;												/* �x���}�[�N�t���O */
#endif

/*
 *==============================================================================
 *	�萔��`
 *==============================================================================
 */
static const char_t cHexVal[ 16U ] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
static const uint8_t cRssi[ 4U ] = { 115, 110, 105, 0 };
static const ET_DispMode_t validModes_UpAlm[ 5U ] = {
	ecDispMode_Up1CHDwClk,
	ecDispMode_Up1CHDw2CH,
	ecDispMode_Up1CHDw2CH_V,
	ecDispMode_Up1CHDw2CH_P,
	ecDispMode_Up1CHDw3CH
};

static const ET_DispMode_t validModes_DwAlm[ 6U ] = {
	ecDispMode_UpNonDw2CH,
	ecDispMode_Up1CHDw2CH,
	ecDispMode_Up1CHDw2CH_V,
	ecDispMode_Up1CHDw2CH_P,
	ecDispMode_UpNonDw3CH,
	ecDispMode_Up1CHDw3CH
};

static const ET_DispMode_t validModes_Bell[ 4U ] = {
	ecDispMode_Up1CHDwClk,
	ecDispMode_Up1CHDw2CH,
	ecDispMode_Up1CHDw2CH_V,
	ecDispMode_Up1CHDw2CH_P
};

/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
*/
static void SubHmi_Disp( void );

static void SubHmi_UpValDsp( void );
//static void SubHmi_UpValDsp_Vol( void );			/* �d�� */
//static void SubHmi_UpValDsp_Pulse( void );			/* �p���X */
//static void SubHmi_UpVerDsp( void );
#if (swSensorCom == imEnable)
static void SubHmi_DwValDsp( void );
#endif
static void SubHmi_DwHexDsp( uint16_t arVal );
//static void SubHmi_DwValDsp_Vol( void );			/* �d�� */
//static void SubHmi_DwValDsp_Pulse( void );			/* �p���X */
//static void SubHmi_AppGrDsp( void );
static void SubHmi_DwTimeDsp( void );
static void SubHmi_StsDsp( void );
static void SubHmi_AlmStsDsp( void );
static void SubHmi_AlmStsDsp( void );
static void SubHmi_BellMarkSetDsp( void );


#pragma section text MY_APP2
#if 0
/*
 *******************************************************************************
 *	���[�U�C���^�[�t�F�[�X ����������
 *
 *	[���e]
 *		���[�U�C���^�[�t�F�[�X�̏������������s���B�i�d��ON���̏����������j
 *******************************************************************************
 */
void ApiHmi_initial( void )
{
	gvDispMode.mNowDisp = ecDispMode_UpVerDwNon;
	gvDispMode.mPreDisp = ecDispMode_UpVerDwNon;
}
#endif

/*
 *******************************************************************************
 *	���[�U�C���^�[�t�F�[�X�������C��
 *
 *	[���e]
 *		���[�U�C���^�[�t�F�[�X�������C��
 *******************************************************************************
 */
void ApiHmi_Main( void )
{
	SubHmi_Disp();				/* �f�B�X�v���C�\�� */
	
	/* KeyLock�\������߂邽�߂̕\����Ԃ������� */
	if( (gvDispMode.mNowDisp != ecDispMode_Lock) &&
		(gvDispMode.mNowDisp != ecDispMode_Lock2) &&
		(gvDispMode.mNowDisp != ecDispMode_AbnChkErr) )
	{
		gvDispMode.mPreDisp = gvDispMode.mNowDisp;
	}
	
	if( (gvDispMode.mNowDisp == ecDispMode_UpVerDwNon) ||
		(gvDispMode.mNowDisp == ecDispMode_UpVerDwNon2) ||
		(gvDispMode.mNowDisp == ecDispMode_Lock) ||
		(gvDispMode.mNowDisp == ecDispMode_Lock2) )
	{
		gvModuleSts.mLcd = ecLcdModuleSts_Run;
	}
	else
	{
		gvModuleSts.mLcd = ecLcdModuleSts_Sleep;
	}
	
	ApiLcd_Main();				/* LCD�h���C�o�[�֕\���f�[�^�𑗐M */
	
	if( gvDispMode.mNowDisp == ecDispMode_UpErrDwRtc )
	{
		while( 1U )
		{
			M_STOP;
		}
	}
}


/*
 *******************************************************************************
 *	�f�B�X�v���C�\������
 *
 *	[���e]
 *		�f�B�X�v���C�֏o�͂���ׂ̕\���������s���B
 *******************************************************************************
 */
static void SubHmi_Disp( void )
{
	uint8_t		wkRssi;
	uint8_t		wkLoop;
	uint8_t		wkU8;
	uint16_t wkU16;
	
	ApiLcd_SegSelectOnOff( ecDispSegSelect_SegAllOff );
	
	switch( gvDispMode.mNowDisp )
	{
		/* ��i�FVer�A���i�FVerNo. */
		case ecDispMode_UpVerDwNon:
		case ecDispMode_UpVerDwNon2:
		case ecDispMode_Ver:
			wkU16 = cRomVer[ 0U ] * 100U;
			wkU16 += cRomVer[ 1U ] * 10U + (sint16_t)cRomVer[ 2U ];
			
			ApiLcd_UppLow7SegDsp( "Ver", imHigh );
			ApiLcd_Low7SegNumDsp( wkU16, imOFF );
			
			if( gvDispMode.mNowDisp == ecDispMode_Ver )
			{
				SubHmi_StsDsp();								/* �X�e�[�^�X�\�� */
			}
			break;
			
		/* ��i:1ch�A���i:���� */
		case ecDispMode_Up1CHDwClk:
#if (swSensorCom == imEnable)
			if( gvInFlash.mProcess.mModelCode == 0xFFU )
			{
				ApiLcd_UppLow7SegDsp(" No", imHigh);
				ApiLcd_UppLow7SegDsp("SEN", imLow);
			}
			else
			{
				SubHmi_DwTimeDsp();
				
				/* ���莞�̈�u�������ɑ��̏����œ_����Ԃɂ��Ȃ����߂̏��� */
				if( gvModInf.mComSts == ecComSts_NullPre )
				{
					SubHmi_UpValDsp();
				}
			}
#else
			SubHmi_DwTimeDsp();
			
			/* ���莞�̈�u�������ɑ��̏����œ_����Ԃɂ��Ȃ����߂̏��� */
			if( gvMeasPhase == ecMeasPhase_Init )
			{
				SubHmi_UpValDsp();
			}
#endif
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			SubHmi_AlmStsDsp();									/* �x��X�e�[�^�X�\�� */
			break;
			
#if (swSensorCom == imEnable)
		/* ��i:1ch�A���i:2ch */
		case ecDispMode_Up1CHDw2CH:
		/* �d�� ��i:��̈ʁ`�����_���ʁA���i:�����_���`�O�� */
		case ecDispMode_Up1CHDw2CH_V:
		/* �p���X ��i:�ő�3���A���i:��̈ʂ�����Ƃ��͈�̈ʂ�1���{cn */
		case ecDispMode_Up1CHDw2CH_P:
			/* ���莞�̈�u�������ɑ��̏����œ_����Ԃɂ��Ȃ����߂̏��� */
			if( gvModInf.mComSts == ecComSts_NullPre || gvModInf.mComSts == ecComSts_RcvWait )
			{
				SubHmi_UpValDsp();
				SubHmi_DwValDsp();
			}
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			SubHmi_AlmStsDsp();									/* �x��X�e�[�^�X�\�� */
			break;
			
		/* ��i�F�Ȃ��A���i�F�ړ_��� */
		case ecDispMode_UpNonDwCon:
			
			switch( gvMeasPrm.mMeasVal[ 1U ] )
			{
				case imON:
					ApiLcd_UppLow7SegDsp(" ON", imLow);
					break;
				case imOFF:
					ApiLcd_UppLow7SegDsp("OFF", imLow);
					break;
				default:
					ApiLcd_UppLow7SegDsp("---", imLow);
			}
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
			
		/* ��i:�Ȃ��A���i:���� */
		case ecDispMode_UpNonDwClk:
			ApiLcd_UppLow7SegDsp("   ", imHigh);
			SubHmi_DwTimeDsp();
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
#endif
		/* ��i:Err�A���i:Rtc */
		case ecDispMode_UpErrDwRtc:
			ApiLcd_UppLow7SegDsp("Err", imHigh);
			ApiLcd_UppLow7SegDsp("RTC", imLow);
			break;
			
		/* ��i:Lock�A���i:�Ȃ� */
		case ecDispMode_Lock:
		case ecDispMode_Lock2:
			ApiLcd_UppLow7SegDsp("LCK", imHigh);
			ApiLcd_UppLow7SegDsp("   ", imLow);
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
			
#if (swRssiLogDebug == imEnable)
		/* ��i�FCommTargetID�A���i�FRSSI */
		case ecDispMode_UpComDwRssi:
			ApiLcd_Upp7SegNumDsp( gvLcdComGwId, imOFF, imOFF, imOFF);
			ApiLcd_Low7SegNumDsp( gvDebugRssi, imOFF );
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
#endif
			
		/* ��i:'ch'�A���i:�����ʐMch(1,2,3) */
		case ecDispMode_HsCh:
			ApiLcd_UppLow7SegDsp(" ch", imHigh);
			ApiLcd_Low7SegNumDsp(gvInFlash.mParam.mrfHsCh+1, imOFF);
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
			
		/* ��i�F' id�f�A���i�F���Ɖ��ID */
		case ecDispMode_Appid:
			ApiLcd_UppLow7SegDsp(" id", imHigh);
			
			wkU16 = gvInFlash.mParam.mAppID[ 0U ] << 8U;
			wkU16 |= gvInFlash.mParam.mAppID[ 1U ];
			wkU16 &= 0x0FFFU;
			
			if( wkU16 == 0x0FFFU )
			{
				ApiLcd_UppLow7SegDsp("---", imLow);
			}
			else if( wkU16 == 0x0FFEU )
			{
				ApiLcd_UppLow7SegDsp("LCL", imLow);
			}
			else
			{
			SubHmi_DwHexDsp( wkU16 );
			}
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
		
		/* ��i�F'LEV�f�A���i�FRSSI */
		case ecDispMode_Rssi:
			ApiLcd_UppLow7SegDsp("LEV", imHigh);
			wkRssi = ApiRfDrv_GetRssi();
			
			/* �ʐM���s�� */
			if( wkRssi < 40U )
			{
				ApiLcd_UppLow7SegDsp("---", imLow);
			}
			/* �ʏ� */
			else
			{
				for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
				{
					if( wkRssi >= cRssi[ wkLoop ] )
					{
						ApiLcd_Low7SegNumDsp( wkLoop + 1, imOFF );
						break;
					}
				}
			}
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
			
		/* ��i�F'con�f�A���i�FGW ID */
		case ecDispMode_Gwid:
			ApiLcd_UppLow7SegDsp("con", imHigh);
			if( gvLcdComGwId == 0U )
			{
				ApiLcd_UppLow7SegDsp("---", imLow);
			}
			else
			{
				SubHmi_DwHexDsp( gvLcdComGwId & 0x0FFFU );
			}
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
			
		/* ��i�F'Err�f�A���i�F�G���[No. */
		case ecDispMode_AbnChkErr:
			ApiLcd_UppLow7SegDsp( "Err", imHigh );
			
			wkU8 = ApiAbn_AbnNumGet();							/* ���߂̋@��ُ�No.�擾 */
			ApiLcd_Low7SegNumDsp( wkU8, imOFF );
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
			
#if 0
		/* ��i:�A�v��ID�A���i:�O���[�vID */
		case ecDispMode_UpAppDwGr:
			SubHmi_AppGrDsp();
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
#endif			
#if (swKouteiMode == imEnable)
		/* �S�Z�O�����g�_�� */
		case ecDispMode_AllSegOn:
			ApiLcd_SegSelectOnOff( ecDispSegSelect_SegAllOn );
			break;
			
		/* �S�Z�O�����g���� */
		case ecDispMode_AllSegOff:
			ApiLcd_SegSelectOnOff( ecDispSegSelect_SegAllOff );
			break;
#endif
#if (swKouteiMode == imEnable)
		/* ��Z�O�����g�_�� */
		case ecDispMode_OddSeg:
			ApiLcd_SegSelectOnOff( ecDispSegSelect_SegOddOn );
			break;
			
		/* �����Z�O�����g�_�� */
		case ecDispMode_EvenSeg:
			ApiLcd_SegSelectOnOff( ecDispSegSelect_SegEvenOn );
			break;
#endif
#if 0
		/* ��i:Pt�A���i:�Ȃ� */
		case ecDispMode_UpPtDwNon:
			ApiLcd_UppLow7SegDsp(" Pt", imHigh);
			ApiLcd_UppLow7SegDsp("   ", imLow);
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
#endif
#if (swRESFDisp == imEnable)
		/* ��i:'rst'�A���i:RESF */
		case ecDispMode_Resf:
			ApiLcd_UppLow7SegDsp("rst", imHigh);
			ApiLcd_Low7SegNumDsp(gvResf, imOFF);
			SubHmi_StsDsp();									/* �X�e�[�^�X�\�� */
			break;
#endif
		default:
			/* ������ */
			break;
	}
}


/*
 *******************************************************************************
 *	��i����l�Z�b�g
 *
 *	[���e]
 *		�t����i�ɑ���l���Z�b�g����
 *******************************************************************************
 */
static void SubHmi_UpValDsp( void )
{
	sint16_t	wkMeasVal;						/* ����l */
	sint16_t	wkMeasValA;						/* ����l���T�C�Y�O�ۑ�(�d���}�C�i�X����p) */
	uint8_t		wkErr;							/* ����G���[ */
	
	/* ����l�\��(��i) */
	switch( gvDispMode.mNowDisp )
	{
		default:
		case ecDispMode_Up1CHDw2CH:
			wkMeasVal = gvMeasPrm.mMeasVal[ 0U ];
			wkErr = gvMeasPrm.mMeasError[ 0U ];
			break;
			
		/* �d�����f�� */
		case ecDispMode_Up1CHDw2CH_V:
			wkMeasVal = gvMeasPrm.mMeasVal[ 0U ];
//			wkMeasVal = 0x2134; 		/* �t���\���e�X�g -500(-0.500) FE0C | -1(-0.001) FFFF */
			wkMeasValA = wkMeasVal;
			wkMeasVal /= 100;							/* ��̈ʁ`�����_���ʂ܂Ŏ擾 */
			wkErr = gvMeasPrm.mMeasError[ 0U ];
			break;
			
		/* �p���X���f�� */
		case ecDispMode_Up1CHDw2CH_P:
			wkMeasVal = gvMeasPrm.mMeasVal[ 0U ];
//			wkMeasVal = 0x0; 		/* �t���\���e�X�g */
			if( wkMeasVal >= 1000)				/* 3��(�\�`��̈�)�擾 */
			{
				wkMeasVal /= 10;
			}
			/* �����Ȃ��F3��(��`�S�̈�)�擾 */
			wkErr = gvMeasPrm.mMeasError[ 0U ];
			break;
	}
	
	if( wkErr & imBurnShort )
	{
		ApiLcd_Upp7SegOvrUdrDsp( imSnsrErr );	/* ��i���^�񒆃o�[�\�� */
	}
	else if( wkErr & imTmpUnder )
	{
		ApiLcd_Upp7SegOvrUdrDsp( imUnder );		/* ��i���A���_�[�t���[�����\�� */
	}
	else if( wkErr & imTmpOver )
	{
		ApiLcd_Upp7SegOvrUdrDsp( imOver );		/* ��i���I�[�o�[�t���[�����\�� */
	}
	else if( wkErr & imSnsError )				/* �Z���T�@��ُ� */
	{
		ApiLcd_Upp7SegOvrUdrDsp( imSnsrErr );	/* ���i���^�񒆃o�[�\�� */
	}
#if 1	/* �t���\���e�X�g���͖��������� */
	else if( wkErr & imComTmpError )			/* �Z���T�ԒʐM�ُ� */
	{
		ApiLcd_Upp7SegOvrUdrDsp( imSnsrErr );	/* ��i���^�񒆃o�[�\�� */
	}
#endif
	else
	{
		switch( gvDispMode.mNowDisp )
		{
			default:
			case ecDispMode_Up1CHDw2CH:
				if( wkMeasVal >= -2050 && wkMeasVal <= -2000 )
				{
					ApiLcd_Upp7SegOvrUdrDsp( imUnder );		/* ��i���A���_�[�t���[�����\�� */
				}
				else if( wkMeasVal >= 2000 && wkMeasVal <= 2050 )
				{
					ApiLcd_Upp7SegOvrUdrDsp( imOver );		/* ��i���I�[�o�[�t���[�����\�� */
				}
				else
				{
					ApiLcd_Upp7SegNumDsp( wkMeasVal, imOFF, imON, imOFF);
				}
				break;
				
			case ecDispMode_Up1CHDw2CH_V:
				if( wkMeasValA < 0 )
				{
					ApiLcd_Upp7SegNumDsp( wkMeasVal, imOFF, imON , imON );
				}
				else
				{
					ApiLcd_Upp7SegNumDsp( wkMeasVal, imOFF, imON , imOFF );
				}
				break;
				
			case ecDispMode_Up1CHDw2CH_P:
				ApiLcd_Upp7SegNumDsp( wkMeasVal, imOFF, imOFF, imOFF);
				break;
		}
	}
	
	if( gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH )
	{
		ApiLcd_SegDsp(ecLcdSeg_S13, imON);		/* ���_�� */
	}
}

#if 0
/*
 *******************************************************************************
 *	��i����l�Z�b�g�i�d���j
 *
 *	[���e]
 *		�t����i�ɑ���l�i��̈ʁ`�����_���ʂ܂Łj���Z�b�g����
 *******************************************************************************
 */
static void SubHmi_UpValDsp_Vol( void )
{
	sint16_t	wkMeasVal;						/* ����l */
	sint16_t	wkMeasValA;						/* ����l�ϊ��p */
	uint8_t		wkErr;							/* ����G���[ */
	
	/* ����l�\��(��i) */
//	wkMeasVal = 0x2134; 		/* �t���\���e�X�g -500(-0.500) FE0C | -1(-0.001) FFFF */
	wkMeasVal = gvMeasPrm.mMeasVal[ 0U ];
	wkMeasValA = wkMeasVal;
	wkMeasValA /= 100;							/* ��̈ʁ`�����_���ʂ܂Ŏ擾 */
	wkErr = gvMeasPrm.mMeasError[ 0U ];
	
	if( wkErr & imBurnShort )
	{
		ApiLcd_Upp7SegOvrUdrDsp( imSnsrErr );	/* ��i���^�񒆃o�[�\�� */
	}
	else if( wkErr & imTmpUnder )
	{
		ApiLcd_Upp7SegOvrUdrDsp( imUnder );		/* ��i���A���_�[�t���[�����\�� */
	}
	else if( wkErr & imTmpOver )
	{
		ApiLcd_Upp7SegOvrUdrDsp( imOver );		/* ��i���I�[�o�[�t���[�����\�� */
	}
	else if( wkErr & imSnsError )				/* �Z���T�@��ُ� */
	{
		ApiLcd_Upp7SegOvrUdrDsp( imSnsrErr );	/* ���i���^�񒆃o�[�\�� */
	}
#if 1 /* �d���t���\���e�X�g���͖��������� */
	else if( wkErr & imComTmpError )			/* �Z���T�ԒʐM�ُ� */
	{
		ApiLcd_Upp7SegOvrUdrDsp( imSnsrErr );	/* ��i���^�񒆃o�[�\�� */
	}
#endif
	else if(wkMeasVal < 0)
	{
		ApiLcd_Upp7SegNumDsp( wkMeasValA, imOFF, imON , imON );
	}
	else
	{
		ApiLcd_Upp7SegNumDsp( wkMeasValA, imOFF, imON , imOFF );
	}
	
	//	ApiLcd_SegDsp(ecLcdSeg_S13, imON);		/* ���_�� */
}


/*
 *******************************************************************************
 *	��i����l�Z�b�g�i�p���X�j
 *
 *	[���e]
 *		�t����i�ɑ���l�i3���j���Z�b�g����
 *******************************************************************************
 */
static void SubHmi_UpValDsp_Pulse( void )
{
	sint16_t	wkMeasVal;						/* ����l */
	sint16_t	wkMeasValA;						/* ����l�ϊ��p */
	uint8_t		wkErr;							/* ����G���[ */
	
	/* ����l�\��(��i) */
//	wkMeasVal = 0x0; 		/* �t���\���e�X�g */
	wkMeasVal = gvMeasPrm.mMeasVal[ 0U ];
	wkMeasValA = wkMeasVal;
	if( wkMeasValA >= 1000)				/* 3��(�\�`��̈�)�擾 */
	{
		wkMeasValA /= 10;
	}										
	/* �����Ȃ��F3��(��`�S�̈�)�擾 */
	wkErr = gvMeasPrm.mMeasError[ 0U ];
	
	if( wkErr & imBurnShort )
	{
		ApiLcd_Upp7SegOvrUdrDsp( imSnsrErr );	/* ��i���^�񒆃o�[�\�� */
	}
	else if( wkErr & imTmpUnder )
	{
		ApiLcd_Upp7SegOvrUdrDsp( imUnder );		/* ��i���A���_�[�t���[�����\�� */
	}
	else if( wkErr & imTmpOver )
	{
		ApiLcd_Upp7SegOvrUdrDsp( imOver );		/* ��i���I�[�o�[�t���[�����\�� */
	}
	else if( wkErr & imSnsError )				/* �Z���T�@��ُ� */
	{
		ApiLcd_Upp7SegOvrUdrDsp( imSnsrErr );	/* ���i���^�񒆃o�[�\�� */
	}
#if 1	/* �p���X�t���\���e�X�g���͖��������� */
	else if( wkErr & imComTmpError )			/* �Z���T�ԒʐM�ُ� */
	{
		ApiLcd_Upp7SegOvrUdrDsp( imSnsrErr );	/* ��i���^�񒆃o�[�\�� */
	}
#endif
	else
	{
		ApiLcd_Upp7SegNumDsp( wkMeasValA, imOFF, imOFF, imOFF);
	}
	
	//	ApiLcd_SegDsp(ecLcdSeg_S13, imON);		/* ���_�� */
}
#endif

#if 0
/*
 *******************************************************************************
 *	��iVer.�Z�b�g
 *
 *	[���e]
 *		�t����i��Ver.���Z�b�g����
 *******************************************************************************
 */
static void SubHmi_UpVerDsp( void )
{
	sint16_t	wkVer;
	
	/* Ver.�\��(��i) */
	wkVer = (sint16_t)cRomVer[ 0U ] * 100U;
	wkVer += (sint16_t)cRomVer[ 1U ] * 10U + (sint16_t)cRomVer[ 2U ];
//	wkVer += (sint16_t)cRomVer[ 2U ];
	
	ApiLcd_UppLow7SegDsp("Ver", imLow);
	ApiLcd_Upp7SegNumDsp( wkVer, imON, imOFF );

}
#endif

#if (swSensorCom == imEnable)
/*
 *******************************************************************************
 *	���i����l�Z�b�g
 *
 *	[���e]
 *		�t�����i�ɑ���l���Z�b�g����
 *	[����]
 *		uint8_t arSel: 0:�f�t�H���g�A1�F�d�����f���A2�F�p���X���f��
 *******************************************************************************
 */
static void SubHmi_DwValDsp( void )
{
	sint16_t	wkMeasVal;						/* ����l */
	uint8_t		wkErr;							/* ����G���[ */
	
	/* ����l�\��(���i) */
	switch( gvDispMode.mNowDisp )
	{
		default:
		case ecDispMode_Up1CHDw2CH:
			wkMeasVal = gvMeasPrm.mMeasVal[ 1U ];
			wkErr = gvMeasPrm.mMeasError[ 1U ];
			break;
			
		/* �d�����f�� */
		case ecDispMode_Up1CHDw2CH_V:
			wkMeasVal = gvMeasPrm.mMeasVal[ 0U ];
//			wkMeasVal = 0x2134; 		/* �t���\���e�X�g -500(-0.500) FE0C | -1(-0.001) FFFF */
			wkMeasVal %= 100;							/* �����_���`�O��(2��)���擾 */
			wkMeasVal *= 10;							/* 1���J��グ�� */
			wkErr = gvMeasPrm.mMeasError[ 0U ];
			break;
			
		/* �p���X���f�� */
		case ecDispMode_Up1CHDw2CH_P:
			wkMeasVal = gvMeasPrm.mMeasVal[ 0U ];
//			wkMeasVal = 0x0; 			/* �t���\���e�X�g */
			
			if(	wkMeasVal > 999 )	/* �p���X��1000�ȏ�̂Ƃ��u���i��3���ځv�Ɂu1�̈ʁv�̒l��\�� */
			{
				wkMeasVal %= 10;							/* ��̈ʂ��擾 */
				wkMeasVal *= 100;							/* 3���ڂɉ����グ�� */ 
			}
			else
			{
				wkMeasVal = 0;								/* ����l��1000�����̂Ƃ��A���i�ɐ��l�͔�\�� */
			}
			wkErr = gvMeasPrm.mMeasError[ 0U ];
			break;
	}
	
	if( wkErr & imBurnShort )
	{
		ApiLcd_Low7SegOvrUdrDsp( imSnsrErr );	/* ���i���^�񒆃o�[�\�� */
	}
	else if( wkErr & imTmpUnder )
	{
		ApiLcd_Low7SegOvrUdrDsp( imUnder );		/* ���i���A���_�[�t���[�����\�� */
	}
	else if( wkErr & imTmpOver )
	{
		ApiLcd_Low7SegOvrUdrDsp( imOver );		/* ���i���I�[�o�[�t���[�����\�� */
	}
	else if( wkErr & imSnsError )				/* �Z���T�@��ُ� */
	{
		ApiLcd_Low7SegOvrUdrDsp( imSnsrErr );	/* ���i���^�񒆃o�[�\�� */
	}
#if 1 /* �t���\���e�X�g���͖��������� */
	else if( wkErr & imComTmpError )			/* �Z���T�ԒʐM�ُ� */
	{
		ApiLcd_Low7SegOvrUdrDsp( imSnsrErr );	/* ���i���^�񒆃o�[�\�� */
	}
#endif
	else
	{
		switch( gvDispMode.mNowDisp )
		{
			default:
			case ecDispMode_Up1CHDw2CH:
				ApiLcd_Low7SegNumDsp( wkMeasVal, imON );
				break;
				
			case ecDispMode_Up1CHDw2CH_V:
				ApiLcd_Low7SegVPDsp( wkMeasVal, imOFF );
				break;
				
			case ecDispMode_Up1CHDw2CH_P:
				if( wkMeasVal / 1000 > 0 )
				{
					ApiLcd_Low7SegVPDsp( wkMeasVal, imON );
				}
				else
				{
					ApiLcd_Low7SegVPDsp( wkMeasVal, imOFF );
				}
				break;
		}
	}
	
	if( gvInFlash.mProcess.mModelCode == ecSensType_HumTmp &&
		gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH )
	{
		ApiLcd_SegDsp(ecLcdSeg_S22, imON);		/* %�_�� */
	}
}
#endif

#if 0
/*
 *******************************************************************************
 *	���i����l�Z�b�g(�d��)
 *
 *	[���e]
 *		�t�����i�ɑ���l�i�����_���`�O�ʁj��2�����Z�b�g����
 *******************************************************************************
 */
static void SubHmi_DwValDsp_Vol( void )
{
	sint16_t	wkMeasVal;						/* ����l */
	sint16_t	wkMeasValA;						/* ����l�ϊ��p */
	uint8_t		wkErr;							/* ����G���[ */
	
	/* ����l�\��(���i) */
//	wkMeasVal = 0x2134; 		/* �t���\���e�X�g -500(-0.500) FE0C | -1(-0.001) FFFF */
	wkMeasVal = gvMeasPrm.mMeasVal[ 0U ];
	wkMeasValA = wkMeasVal;
	wkMeasValA %= 100;							/* �����_���`�O��(2��)���擾 */
	wkMeasValA *= 10;							/* 1���J��グ�� */
	wkErr = gvMeasPrm.mMeasError[ 0U ];
	
	if( wkErr & imBurnShort )
	{
		ApiLcd_Low7SegOvrUdrDsp( imSnsrErr );	/* ��i���^�񒆃o�[�\�� */
	}
	else if( wkErr & imTmpUnder )
	{
		ApiLcd_Low7SegOvrUdrDsp( imUnder );		/* ��i���A���_�[�t���[�����\�� */
	}
	else if( wkErr & imTmpOver )
	{
		ApiLcd_Low7SegOvrUdrDsp( imOver );		/* ��i���I�[�o�[�t���[�����\�� */
	}
	else if( wkErr & imSnsError )				/* �Z���T�@��ُ� */
	{
		ApiLcd_Low7SegOvrUdrDsp( imSnsrErr );	/* ���i���^�񒆃o�[�\�� */
	}
#if 1 /* �d���t���\���e�X�g���͖��������� */
	else if( wkErr & imComTmpError )			/* �Z���T�ԒʐM�ُ� */
	{
		ApiLcd_Low7SegOvrUdrDsp( imSnsrErr );	/* ��i���^�񒆃o�[�\�� */
	}
#endif
	else
	{
		ApiLcd_Low7SegVPDsp( wkMeasValA, imOFF );
	}
}

/*
 *******************************************************************************
 *	���i����l�Z�b�g(�p���X)
 *
 *	[���e]
 *		�t�����i�Ɂu��̈ʂ�0�ł͂Ȃ��Ƃ��v�A����l�i��̈ʁj���Z�b�g����
 *******************************************************************************
 */
static void SubHmi_DwValDsp_Pulse( void )
{
	sint16_t	wkMeasVal;						/* ����l */
	sint16_t	wkMeasValA;						/* ����l�ϊ��p */
	uint8_t		wkErr;							/* ����G���[ */
	
	/* ����l�\��(���i) */
//	wkMeasVal = 0x0; 		/* �t���\���e�X�g */
	wkMeasVal = gvMeasPrm.mMeasVal[ 0U ];
	wkMeasValA = wkMeasVal;

	if(	wkMeasValA > 999 )	/* �p���X��1000�ȏ�̂Ƃ��u���i��3���ځv�Ɂu1�̈ʁv�̒l��\�� */
	{
		wkMeasValA %= 10;							/* ��̈ʂ��擾 */
		wkMeasValA *= 100;							/* 3���ڂɉ����グ�� */ 
	}
	else
	{
		wkMeasValA = 0;								/* ����l��1000�����̂Ƃ��A���i�ɐ��l�͔�\�� */
	}
	wkErr = gvMeasPrm.mMeasError[ 0U ];
	
	if( wkErr & imBurnShort )
	{
		ApiLcd_Low7SegOvrUdrDsp( imSnsrErr );	/* ��i���^�񒆃o�[�\�� */
	}
	else if( wkErr & imTmpUnder )
	{
		ApiLcd_Low7SegOvrUdrDsp( imUnder );		/* ��i���A���_�[�t���[�����\�� */
	}
	else if( wkErr & imTmpOver )
	{
		ApiLcd_Low7SegOvrUdrDsp( imOver );		/* ��i���I�[�o�[�t���[�����\�� */
	}
	else if( wkErr & imSnsError )				/* �Z���T�@��ُ� */
	{
		ApiLcd_Low7SegOvrUdrDsp( imSnsrErr );	/* ���i���^�񒆃o�[�\�� */
	}
#if 1 /* �p���X�t���\���e�X�g���͖��������� */
	else if( wkErr & imComTmpError )			/* �Z���T�ԒʐM�ُ� */
	{
		ApiLcd_Low7SegOvrUdrDsp( imSnsrErr );	/* ��i���^�񒆃o�[�\�� */
	}
#endif
	else if( wkMeasVal / 1000 > 0 )
	{
		ApiLcd_Low7SegVPDsp( wkMeasValA, imON );
	}
	else
	{
		ApiLcd_Low7SegVPDsp( wkMeasValA, imOFF );
	}
}
#endif

/*
 *******************************************************************************
 *	���iHex�f�[�^�Z�b�g
 *
 *	[���e]
 *		�t�����i��Hex�f�[�^���Z�b�g����
 *******************************************************************************
 */
static void SubHmi_DwHexDsp( uint16_t arVal )
{
	uint8_t		wkLoop;
	char_t		wkHex[ 3U ];
	uint16_t	wkU16;
	
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		wkU16 = arVal >> (8U - wkLoop * 4U);
		wkU16 &= 0x000FU;
		wkHex[ wkLoop ] = cHexVal[ wkU16 ];
	}
	ApiLcd_UppLow7SegDsp( wkHex, imLow );
}


#if 0
/*
 *******************************************************************************
 *	��i�A�v��ID�A���i�O���[�vID�Z�b�g
 *
 *	[���e]
 *		�t����i�ɃA�v��ID(16�i��)�A���i�ɃO���[�vID(10�i��)���Z�b�g����
 *******************************************************************************
 */
static void SubHmi_AppGrDsp( void )
{
	uint16_t	wkAppId;		/* �A�v��ID */
	sint16_t	wkGrId;			/* �O���[�vID */
	uint16_t	wkVal;
	char_t		wkHex[ 3U ];
	uint8_t		wkLoop;
	
	/* �A�v��ID�\��(��i) */
	wkAppId = gvInFlash.mParam.mAppID[ 0U ] << 8U;
	wkAppId |= gvInFlash.mParam.mAppID[ 1U ];
	wkAppId &= 0x0FFFU;
	
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		wkVal = wkAppId >> (8U - wkLoop * 4U);
		wkVal &= 0x000FU;
		wkHex[ wkLoop ] = cHexVal[ wkVal ];
	}
	
	ApiLcd_UppLow7SegDsp( wkHex, imHigh );
	
	
	/* �O���[�vID�\��(���i) */
	wkGrId = (sint16_t)gvInFlash.mParam.mGroupID;
	ApiLcd_Low7SegNumDsp( wkGrId, imOFF );

}
#endif


/*
 *******************************************************************************
 *	���i�����Z�b�g
 *
 *	[���e]
 *		�t�����i�Ɏ������Z�b�g����
 *******************************************************************************
 */
static void SubHmi_DwTimeDsp( void )
{
	ST_RTC_t	wkClock;
	
	switch( gvRtcSts )
	{
		/* ����RTC���x��Ă���:���X�ɐi�ގ�����\�� */
		case ecRtcSts_Delay:
		/* ����RTC���i��ł���:�i��ł���������\�� */
		case ecRtcSts_Fast:
			wkClock = ApiRtcDrv_localtime( gvLocalTime );
			break;
			
		/* �ʏ� */
		case ecRtcSts_Non:
		default:
			wkClock = gvClock;
			break;
	}
	ApiLcd_TimeDsp( wkClock.mHour, wkClock.mMin );			/* ���i�����v�\�� */
}


/*
 *******************************************************************************
 *	�X�e�[�^�X�Z�b�g
 *
 *	[���e]
 *		�X�e�[�^�X���Z�b�g����
 *******************************************************************************
 */
static void SubHmi_StsDsp( void )
{
	/* �d�r�c�ʕ\�� */
	switch( gvBatAd.mBattSts )
	{
		case ecBattSts_H:
			ApiLcd_SegDsp(ecLcdSeg_T10, imON);
		case ecBattSts_M:
			ApiLcd_SegDsp(ecLcdSeg_T9, imON);
		case ecBattSts_L:
			ApiLcd_SegDsp(ecLcdSeg_T8, imON);
		case ecBattSts_Empty:
		case ecBattSts_Non:
		default:
			ApiLcd_SegDsp(ecLcdSeg_T11, imON);
			break;
		case ecBattSts_Init:
			break;
	}
	
	/* EVENT */
	if( gvInFlash.mParam.mEventKeyFlg == imON )
	{
		ApiLcd_SegDsp(ecLcdSeg_S4, imON);
	}
	else
	{
		ApiLcd_SegDsp(ecLcdSeg_S4, imOFF);
	}
	
	/* ONLINE */
	if( ApiRfDrv_GetRtConInfo() )
	{
		ApiLcd_SegDsp(ecLcdSeg_S2, imON);
	}
	else
	{
		ApiLcd_SegDsp(ecLcdSeg_S2, imOFF);
	}
	
	/* COLLECT */ 
	if( ApiRfDrv_HsStsDisp( 3U ) )
	{
		ApiLcd_SegDsp(ecLcdSeg_S3, imON);
	}
	else
	{
		ApiLcd_SegDsp(ecLcdSeg_S3, imOFF);
	}
	
	/* RF OFF */
	if( ApiRfDrv_RtStbyOnOff( 0U ) == 0xFFFF )
	{
		ApiLcd_SegDsp(ecLcdSeg_S1, imON);
	}
	else
	{
		ApiLcd_SegDsp(ecLcdSeg_S1, imOFF);
	}
	
	SubHmi_BellMarkSetDsp();							/* �x���}�[�N�\�� */
}





/*
 *******************************************************************************
 *	�x��X�e�[�^�X�Z�b�g
 *
 *	[���e]
 *		�x��X�e�[�^�X���Z�b�g����
 *******************************************************************************
 */
static void SubHmi_AlmStsDsp( void )
{
	uint8_t			wkLoop;
	uint8_t			wkChannel;
	ET_LcdSeg_t		wkSegH_L;
	ET_LcdSeg_t		wkSegH_R;
	ET_LcdSeg_t		wkSegL_L;
	ET_LcdSeg_t		wkSegL_R;
	ET_LcdSeg_t		wkSegBell = ecLcdSeg_S1;
	
	
	for( wkChannel = 0U ; wkChannel < imChannelNum ; wkChannel++ )
	{
		if( wkChannel == 0U )
		{
			/* ��i�x��X�e�[�^�X */
			for( wkLoop = 0U ; wkLoop < sizeof(validModes_UpAlm) ; wkLoop++ )
			{
				if( gvDispMode.mNowDisp == validModes_UpAlm[ wkLoop ] )
				{
					wkSegBell = ecLcdSeg_S7;
					wkSegH_L = ecLcdSeg_S8;
					wkSegH_R = ecLcdSeg_S9;
					wkSegL_L = ecLcdSeg_S10;
					wkSegL_R = ecLcdSeg_S11;
				}
			}
//			if( (gvDispMode.mNowDisp == ecDispMode_Up1CHDwClk) ||
//				(gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH) ||
//				(gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH_V) ||			/* �d�� */
//				(gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH_P) ||			/* �p���X */
//				(gvDispMode.mNowDisp == ecDispMode_Up1CHDw3CH) )
//			{
//				wkSegBell = ecLcdSeg_S7;
//				wkSegH_L = ecLcdSeg_S8;
//				wkSegH_R = ecLcdSeg_S9;
//				wkSegL_L = ecLcdSeg_S10;
//				wkSegL_R = ecLcdSeg_S11;
//			}
		}
		else
		{
			/* ���i�x��X�e�[�^�X */
			for( wkLoop = 0U ; wkLoop < sizeof(validModes_DwAlm) ; wkLoop++ )
			{
				if( gvDispMode.mNowDisp == validModes_DwAlm[ wkLoop ] )
				{
					wkSegBell = ecLcdSeg_S17;
					wkSegH_L = ecLcdSeg_S18;
					wkSegH_R = ecLcdSeg_S19;
					wkSegL_L = ecLcdSeg_S20;
					wkSegL_R = ecLcdSeg_S21;
				}
			}
//			if( (gvDispMode.mNowDisp == ecDispMode_UpNonDw2CH) ||
//				(gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH) ||
//				(gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH_V) ||			/* �d�� */
//				(gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH_P) ||			/* �p���X */
//				(gvDispMode.mNowDisp == ecDispMode_UpNonDw3CH) ||
//				(gvDispMode.mNowDisp == ecDispMode_Up1CHDw3CH) )
//			{
//				wkSegBell = ecLcdSeg_S17;
//				wkSegH_L = ecLcdSeg_S18;
//				wkSegH_R = ecLcdSeg_S19;
//				wkSegL_L = ecLcdSeg_S20;
//				wkSegL_R = ecLcdSeg_S21;
//			}
		}
		
		if( wkSegBell != 0U )
		{
			/* LCD�A���[���X�e�[�^�X�Z�b�g */
			
			/* �����x�� */
			if( ecAlmSts_ActDelayCnt == gvAlmSts[ wkChannel ][ ecAlmKind_HH ] )
//					ecAlmSts_ActDevTime == gvAlmSts[ wkChannel ][ ecAlmKind_HH ]
			{
				ApiLcd_SegDsp( wkSegH_L, imON );
				ApiLcd_SegDsp( wkSegH_R, imON );
				ApiLcd_SegDsp( wkSegBell, imON );
			}
			/* ����x�� */
			else if( ecAlmSts_ActDelayCnt == gvAlmSts[ wkChannel ][ ecAlmKind_H ] )
//					ecAlmSts_ActDevTime == gvAlmSts[ wkChannel ][ ecAlmKind_H ]
			{
				ApiLcd_SegDsp( wkSegH_R, imON );
				ApiLcd_SegDsp( wkSegBell, imON );
			}
			
			
			/* �������x�� */
			if( ecAlmSts_ActDelayCnt == gvAlmSts[ wkChannel ][ ecAlmKind_LL ] )
//					ecAlmSts_ActDevTime == gvAlmSts[ wkChannel ][ ecAlmKind_LL ]
			{
				ApiLcd_SegDsp( wkSegL_L, imON );
				ApiLcd_SegDsp( wkSegL_R, imON );
				ApiLcd_SegDsp( wkSegBell, imON );
			}
			/* �����x�� */
			else if( ecAlmSts_ActDelayCnt == gvAlmSts[ wkChannel ][ ecAlmKind_L ] )
//				ecAlmSts_ActDevTime == gvAlmSts[ wkChannel ][ ecAlmKind_L ]
			{
				ApiLcd_SegDsp( wkSegL_R, imON );
				ApiLcd_SegDsp( wkSegBell, imON );
			}
		}
	}
}


/*
 *******************************************************************************
 *	�x���}�[�N�Z�b�g
 *
 *	[���e]
 *		�@��ُ�A�v���x��(�o�[���A�I�[�o�[�܂�)
 *		���A���Ă��_����Ԃ��p��
 *		DISP�L�[�����ŁA�Y���̃A���[���S�Ă����A��ԂȂ����
 *******************************************************************************
 */
static void SubHmi_BellMarkSetDsp( void )
{
#if (swSensorCom == imEnable)
	uint8_t		wkLoop;
	uint8_t		wkAlmNum;
	uint8_t		wkCh;
	
	
	if( gvInFlash.mProcess.mModelCode == ecSensType_HumTmp )
	{
		wkCh = 2U;
	}
	else
	{
		wkCh = 1U;
	}
	
	for( wkLoop = 0U ; wkLoop < wkCh ; wkLoop++ )
	{
		/* �x���}�[�N�t���O��OFF�̏ꍇ */
		if( vBellMarkSetFlg[ wkLoop ] == imOFF )
		{
			/* �@��ُ�(�d�r��:bit0�A�d�r�c�ʒቺ:bit12����) */
			if( gvInFlash.mData.mAbnInf_Sts & 0x0FFE )
			{
				vBellMarkSetFlg[ 0U ] = imON;				/* �x���}�[�N�t���O�Z�b�g */
			}
			
			/* ����G���[(�o�[���A�V���[�g�A�I�[�o�[�A�A���_�[�A�Z���T�G���[) */
			if( gvMeasPrm.mMeasError[ wkLoop ] & imOvUnBrnShrtComSns )
			{
				vBellMarkSetFlg[ wkLoop ] = imON;			/* �x���}�[�N�t���O�Z�b�g */
			}
			
			/* �v���x��(UU/U/L/LL) */
			for( wkAlmNum = 0U ; wkAlmNum < imAllAlmNum ; wkAlmNum++ )
			{
				if( gvAlmSts[ wkLoop ][ wkAlmNum ] == ecAlmSts_ActDelayCnt )
				{
					vBellMarkSetFlg[ wkLoop ] = imON;		/* �x���}�[�N�t���O�Z�b�g */
				}
			}
		}
	}
	
	/* ����l�\���̂Ƃ� */
	for( wkLoop = 0U ; wkLoop < sizeof(validModes_Bell) ; wkLoop++ )
	{
		if( gvDispMode.mNowDisp == validModes_Bell[ wkLoop ] )
		{
			if( vBellMarkSetFlg[ 0U ] == imON )
			{
				ApiLcd_SegDsp(ecLcdSeg_S7, imON);				/* �x���}�[�N�_�� */
			}
			if( vBellMarkSetFlg[ 1U ] == imON )
			{
				ApiLcd_SegDsp(ecLcdSeg_S17, imON);				/* �x���}�[�N�_�� */
			}
		}
	}
//	if( gvDispMode.mNowDisp == ecDispMode_Up1CHDwClk ||
//		gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH ||
//		gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH_V ||		/* �d�� */
//		gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH_P )		/* �p���X */
//	{
//		if( vBellMarkSetFlg[ 0U ] == imON )
//		{
//			ApiLcd_SegDsp(ecLcdSeg_S7, imON);				/* �x���}�[�N�_�� */
//		}
//		if( vBellMarkSetFlg[ 1U ] == imON )
//		{
//			ApiLcd_SegDsp(ecLcdSeg_S17, imON);				/* �x���}�[�N�_�� */
//		}
//	}
#else
	uint8_t		wkLoop;
	uint8_t		wkAlmNum;
	
	if( vBellMarkSetFlg == imOFF )					/* �x���}�[�N�t���O��OFF�̏ꍇ */
	{
		/* �@��ُ�(�d�r��:bit0�A�d�r�c�ʒቺ:bit12����) */
		if( gvInFlash.mData.mAbnInf_Sts & 0x0FFE )
		{
			vBellMarkSetFlg = imON;					/* �x���}�[�N�t���O�Z�b�g */
		}
		
		/* ����G���[(�o�[���A�V���[�g�A�I�[�o�[�A�A���_�[) */
		for( wkLoop = 0U ; wkLoop < imMeasChNum ; wkLoop++ )
		{
			if( gvMeasPrm.mMeasError[ wkLoop ] & imOvUnBrnShrt )
			{
				vBellMarkSetFlg = imON;				/* �x���}�[�N�t���O�Z�b�g */
			}
		}
		
		/* �v���x�� */
		for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop++ )
		{
			for( wkAlmNum = 0U ; wkAlmNum < imAllAlmNum ; wkAlmNum++ )
			{
				if( gvAlmSts[ wkLoop ][ wkAlmNum ] == ecAlmSts_ActDelayCnt )
				{
					vBellMarkSetFlg = imON;			/* �x���}�[�N�t���O�Z�b�g */
				}
			}
		}
	}
	
	if( vBellMarkSetFlg == imON )
	{
		if( gvDispMode.mNowDisp == ecDispMode_Up1CHDwClk )
		{
			ApiLcd_SegDsp(ecLcdSeg_S7, imON);		/* �x���}�[�N�_�� */
		}
	}
#endif
}


/*
 *******************************************************************************
 *	�x���}�[�N�t���O�N���A
 *
 *	[���e]
 *		�x���}�[�N�t���O���N���A����
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		uint8_t arRet: imON:�x���}�[�N�N���A����AimOFF�F�x���}�[�N�N���A�Ȃ�
 *******************************************************************************
 */
uint8_t ApiHmi_BellMarkClrDsp( void )
{
	uint8_t arRet = imOFF;
	
#if (swSensorCom == imEnable)
	if( vBellMarkSetFlg[ 0U ] == imON || vBellMarkSetFlg[ 1U ] == imON )
	{
		vBellMarkSetFlg[ 0U ] = imOFF;				/* �x���}�[�N�t���O�N���A */
		vBellMarkSetFlg[ 1U ] = imOFF;				/* �x���}�[�N�t���O�N���A */
		
		SubHmi_BellMarkSetDsp();					/* �x���}�[�N�X�V */
		
		if( vBellMarkSetFlg[ 0U ] == imOFF && vBellMarkSetFlg[ 1U ] == imOFF )
		{
			arRet = imON;
		}
	}
#else
	if( vBellMarkSetFlg == imON )
	{
		vBellMarkSetFlg = imOFF;							/* �x���}�[�N�t���O�N���A */
		
		SubHmi_BellMarkSetDsp();							/* �x���}�[�N�X�V */
		
		if( vBellMarkSetFlg == imOFF )
		{
			arRet = imON;
		}
	}
#endif	
	return arRet;
}


#pragma section