/*
 *******************************************************************************
 *	File name	:	TimeComp.c
 *
 *	[���e]
 *		�����␳����
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2018.05.15		Softex K.U		�V�K�쐬
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

#define imTimCompLimTime	7200U		/* �����␳�̐����́}1���Ԗ���(0.5�b����\) */

#define imTimCompMaxCnt		16383U		/* 14bit�����␳�̍ő�J�E���g�� */

#define imClock_32bitSize	0U			/* 32bit�����T�C�Y */
#define imClock_14bitSize	1U			/* 14bit�����T�C�Y */



 /*
 *==============================================================================
 *	�񋓌^��`
 *==============================================================================
 */
typedef enum ET_MeaSts
{
	ecMeaSts_Init = 0,														/* ���� */
	ecMeaSts_Meas,															/* ���� */
	ecMeaSts_Log1,															/* ���^1 */
	ecMeaSts_Log2,															/* ���^2 */

	ecMeaStsMax																/* enum�ő�l */
} ET_MeaSts_t;



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
static uint8_t vTimeCompFlg = imOFF;

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
static void SubTimeComp_MeaExeChk( ST_RTC_t arClock );
static uint32_t SubTimeComp_GetEvtRecTime( ST_RTC_t arClock, uint8_t arSel );
static ET_MeaSts_t	SubRtcIntMeas1( ST_RTC_t arClock );
static ET_MeaSts_t	SubRtcIntMeas2( ST_RTC_t arClock );
static void SubTimeComp_Initial( void );
//static void SubTimeComp_Delay( void );									/* ����RTC�x��␳���� */
//static void SubTimeComp_Fast( void );									/* ����RTC�i�ݕ␳���� */
static void SubTimeComp_DelayFast( void );
static uint8_t	SubTimeComp_SurPlusJudgeMeas1( ST_RTC_t arClock );


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	RTC�����ݏ���
 *
 *	[���e]
 *		RTC�̒�������݌�Ƀ��C�����[�`���Ŏ��s�����
 *	[����]
 *		�Ȃ�
 *	[�߂�l]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRtcInt( void )
{
	/* ���v�␳�������s */
#if 0
	switch( gvRtcSts )
	{
		/* ����RTC���x��Ă��� */
		case ecRtcSts_Delay:
			SubTimeComp_Delay();								/* ����RTC�x��␳���� */
			break;
		
		/* ����RTC���i��ł��� */
		case ecRtcSts_Fast:
			SubTimeComp_Fast();									/* ����RTC�i�ݕ␳���� */
			break;
		
		/* �ُ햳�� */
		case ecRtcSts_Non:
		default:
			/* ���^�����Ǝ������r���āA���^1�A���^2�̑���E���^���s�����̔��f */
			SubTimeComp_MeaExeChk( gvClock );
			break;
	}
#else
	if( gvRtcSts == ecRtcSts_Non )
	{
		/* ���^�����Ǝ������r���āA���^1�A���^2�̑���E���^���s�����̔��f */
		SubTimeComp_MeaExeChk( gvClock );
	}
	else
	{
		SubTimeComp_DelayFast();
	}
#endif
	
	
	if( gvClock.mSec == 0U )
	{
		/* 1����1�� */
		gvModuleSts.mErrChk |= imErrChk_KeyChk;			/* �L�[����m�F */
		
		/* 5����1�� */
		if( gvClock.mMin % 5U == 0U )
		{
			ApiAdc_AdExe();								/* �d�rAD���s */
		}
	
		/* 1���Ԃ�1�� */
		if( gvClock.mMin == 0U )
		{
			R_TAU0_Channel1_Start();					/* �T�u�N���b�N���U�m�F */
			gvCycTestStart = imON;
		}
	}
	
	
#if 0	/* ��E���e���Ԃ̋@�\�͑��t�F�[�X�ł͍폜 */
	/* �x���E�N���A�̎w�莞���m�F */
	if( (gvInFlash.mParam.mDeviClrTimeHour == gvClock.mHour) &&
		(gvInFlash.mParam.mDeviClrTimeMin == gvClock.mMin) )
	{
		gvAlmClrPoint |= imAlmClrPoint_Time;
	}
#endif
	
	/* Ver�\��������l�\���J�� */
	if( gvDispMode.mNowDisp == ecDispMode_UpVerDwNon )
	{
		gvDispMode.mNowDisp = ecDispMode_UpVerDwNon2;
	}
	else if( gvDispMode.mNowDisp == ecDispMode_UpVerDwNon2 )
	{
#if (swSensorCom == imEnable)
		switch( gvInFlash.mProcess.mModelCode )
		{
			case ecSensType_HumTmp:
				gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH;
				break;
			case ecSensType_V:										/* �d�� */
					gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH_V;
					break;
			case ecSensType_Pulse:									/* �p���X */
					gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH_P;
					break;
			default:
			case ecSensType_Pt:
				gvDispMode.mNowDisp = ecDispMode_Up1CHDwClk;
				break;
		}
#else
		gvDispMode.mNowDisp = ecDispMode_Up1CHDwClk;
#endif
	}
	
	/* Lock�\��������l�\���J�� */
	if( gvDispMode.mNowDisp == ecDispMode_Lock )
	{
		gvDispMode.mNowDisp = ecDispMode_Lock2;
	}
	else if( gvDispMode.mNowDisp == ecDispMode_Lock2 )
	{
		gvDispMode.mNowDisp = gvDispMode.mPreDisp;
	}
	
	gvModuleSts.mRtcInt = ecRtcIntSts_Sleep;
}


/*
 *******************************************************************************
 *	���^�����Ǝ������r���āA���^1�A���^2�̑���E���^���s�����̔��f
 *
 *	[���e]
 *		���^�����Ǝ������r���āA���^1�A���^2�̑���E���^���s�����̔��f
 *	[����]
 *		ST_RTC_t	arClock:����
 *	[�߂�l]
 *		ET_MeaSts_t	arMeasSts:�m�F����
 *******************************************************************************
 */
static void SubTimeComp_MeaExeChk( ST_RTC_t arClock )
{
	uint32_t		wkRecTime;
	ET_MeaSts_t		wkMeasSts1;			/* ����1���^���s�X�e�[�^�X */
	ET_MeaSts_t		wkMeasSts2;			/* ����2���^���s�X�e�[�^�X */
	
	static uint32_t	vPreLogClock = 0;	/* �O����^���� */
	
	/* ��ԑJ�� */
	if( gvModuleSts.mExe == ecExeModuleSts_Sleep )
	{
		wkMeasSts1 = SubRtcIntMeas1( arClock );		/* ���^1�X�e�[�^�X */
		wkMeasSts2 = SubRtcIntMeas2( arClock );		/* ���^2�X�e�[�^�X */
		
		if( gvFirmupMode == imOFF && (wkMeasSts1 != ecMeaSts_Init || wkMeasSts2 != ecMeaSts_Init) )
		{
			
			wkRecTime = SubTimeComp_GetEvtRecTime( arClock, 0U );
			if( wkRecTime > vPreLogClock )
			{
				/* ���菈�� */
#if (swSensorCom == imEnable)
				/* �ʐM���� */
				if( gvModuleSts.mCom == ecComModuleSts_Sleep )
				{
					gvModuleSts.mCom = ecComModuleSts_Run;
				}
#else
				if( gvModuleSts.mMea == ecMeaModuleSts_Sleep )
				{
					gvModuleSts.mMea = ecMeaModuleSts_Run;
				}
#endif
				/* ���s�X�e�[�^�X�X�V */
				if( wkMeasSts1 == ecMeaSts_Log1 )
				{
					gvModuleSts.mExe = ecExeModuleSts_Log1;
				}
				else
				{
					if( wkMeasSts2 == ecMeaSts_Log2 )
					{
						gvModuleSts.mExe = ecExeModuleSts_Log2;
					}
					else
					{
						gvModuleSts.mExe = ecExeModuleSts_Meas;
					}
				}
				
				gvRecCycRtcTm = wkRecTime;					/* �������(�o�ߕb)���O�t���t���b�V���������ݗp�ɕێ����� */
				
				vPreLogClock = wkRecTime;					/* �O��Ƃ̔�r�p�Ɏ��^������������ */
				
			}
		}
	}
}


/*
 *******************************************************************************
 *	�������(�o�ߕb)���O�t���t���b�V���������ݗp�ɕێ�����
 *
 *	[���e]
 *		�������(�o�ߕb)���O�t���t���b�V���������ݗp�ɕێ�����
 *	[����]
 *		ST_RTC_t	arClock: ����
 *		uint8_t		arSel: ���^�̎�� 0�F���O���^�A1�F�������^
 *	[�߂�l]
 *		uint32_t	wkTime:�����ݎ���
 *******************************************************************************
 */
uint32_t SubTimeComp_GetEvtRecTime( ST_RTC_t arClock, uint8_t arSel )
{
	uint32_t wkTime;
	
	/* ���O���^���F���v�␳(�Ȃ�)�A�������^���F���v�␳(�Ȃ��A�i��) */
	if( (arSel == 0U && gvRtcSts == ecRtcSts_Non) ||
		(arSel != 0U && gvRtcSts != ecRtcSts_Delay) )
	{
		wkTime = ApiRtcDrv_mktime( gvClock );		/* RTC�����̗p */
	}
	/* ���O���^���F���v�␳(�i�݁A�x��)�A�������^���F���v�␳(�x��) */
	else
	{
		/* �]��L */
		if( SubTimeComp_SurPlusJudgeMeas1( arClock ) == imON )
		{
			wkTime = gvLocalTime - 1U;
		}
		/* �]�薳 */
		else
		{
			wkTime = gvLocalTime;
		}
	}
	
	return wkTime;
}


/*
 *******************************************************************************
 *	���^1�^�C�~���O�m�F
 *
 *	[���e]
 *		���^1�̃^�C�~���O���m�F����
 *	[����]
 *		ST_RTC_t	arClock:����
 *	[�߂�l]
 *		ET_MeaSts_t	wkMeasSts1:�m�F����
 *******************************************************************************
 */
static ET_MeaSts_t	SubRtcIntMeas1( ST_RTC_t arClock )
{
	ET_MeaSts_t		wkMeasSts1 = ecMeaSts_Init;			/* ����1���^���s�X�e�[�^�X */
	
	switch( gvInFlash.mParam.mLogCyc1 )
	{
		/* �ݒ薳�� */
		case ecRecKind1_None:
			break;
		case ecRecKind1_5sec:
			/* 5sec�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 5U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 5U == 0U) || (arClock.mSec % 5U == 1U))) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_10sec:
			/* 10sec�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 10U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 10U == 0U) || (arClock.mSec % 10U == 1U))) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_30sec:
			/* 30sec�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 30U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 30U == 0U) || (arClock.mSec % 30U == 1U))) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_1min:
			/* 1min�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U))) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_5min:
			/* 5min�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 5U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 5U == 0U)) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_10min:
			/* 10min�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 10U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 10U == 0U)) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_15min:
			/* 15min�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 15U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 15U == 0U)) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_30min:
			/* 30min�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 30U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 30U == 0U)) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
		case ecRecKind1_60min:
			/* 60min�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin == 0U)) )
			{
				wkMeasSts1 = ecMeaSts_Log1;
			}
			break;
	}
	
	/* �������s�Ȃ���1min�� */
	if( wkMeasSts1 == ecMeaSts_Init && 
		(((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U)) ||
		((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)))) )
	{
		/* LCD�\���̂ݍX�V(���v�X�V�p�Ƃ���) */
		gvModuleSts.mLcd = ecLcdModuleSts_Run;
	}
	
	return wkMeasSts1;
}


/*
 *******************************************************************************
 *	���^2�^�C�~���O�m�F
 *
 *	[���e]
 *		���^1�̃^�C�~���O���m�F����
 *	[����]
 *		ST_RTC_t	arClock:����
 *	[�߂�l]
 *		ET_MeaSts_t	wkMeasSts2:�m�F����
 *******************************************************************************
 */
static ET_MeaSts_t	SubRtcIntMeas2( ST_RTC_t arClock )
{
	ET_MeaSts_t		wkMeasSts2 = ecMeaSts_Init;			/* ����1���^���s�X�e�[�^�X */
	
	switch( gvInFlash.mParam.mLogCyc2 )
	{
		default:
		case ecRecKind2_None:
			break;
		case ecRecKind2_5sec:
			/* 5sec�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 5U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 5U == 0U) || (arClock.mSec % 5U == 1U))) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
		case ecRecKind2_10sec:
			/* 10sec�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 10U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 10U == 0U) || (arClock.mSec % 10U == 1U))) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
		case ecRecKind2_30sec:
			/* 30sec�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec % 30U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec % 30U == 0U) || (arClock.mSec % 30U == 1U))) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
		case ecRecKind2_1min:
			/* 1min�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U))) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
		case ecRecKind2_5min:
			/* 5min�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 5U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 5U == 0U)) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
		case ecRecKind2_10min:
			/* 10min�� */
			if( ((gvRtcSts != ecRtcSts_Delay) && (arClock.mSec == 0U) && (arClock.mMin % 10U == 0U)) ||
				((gvRtcSts == ecRtcSts_Delay) && ((arClock.mSec == 0U) || (arClock.mSec == 1U)) && (arClock.mMin % 10U == 0U)) )
			{
				wkMeasSts2 = ecMeaSts_Log2;
			}
			break;
	}
	return wkMeasSts2;
}


/*
 *******************************************************************************
 *	�����␳���� ����������
 *
 *	[���e]
 *		�����␳���� ����������
 *******************************************************************************
 */
static void SubTimeComp_Initial( void )
{
	gvLocalTime = 0U;							/* ���[�J������������ */
	gvRtcSts = ecRtcSts_Non;					/* �␳�����Ȃ� */
}

#if 0
/*
 *******************************************************************************
 *	����RTC�x��␳����
 *
 *	[���e]
 *		����RTC�x��␳����
 *******************************************************************************
 */
static void SubTimeComp_Delay( void )
{
	uint32_t		wkRTC_TotalSec;						/* �o�ߕb */
	ST_RTC_t		wkClock;
	
	
	/* ���^�����Ǝ������r���āA���^1�A���^2�̑���E���^���s�����̔��f */
	wkClock = ApiRtcDrv_localtime( gvLocalTime );
	SubTimeComp_MeaExeChk( wkClock );
	
	/* ���[�J���������J�E���g�A�b�v */
	gvLocalTime += 2U;
	
	/* ���^�����Ǝ������r���āA���^1�A���^2�̑���E���^���s�����̔��f */
	wkClock = ApiRtcDrv_localtime( gvLocalTime );
	SubTimeComp_MeaExeChk( wkClock );
	
	/* ����RTC�̎����擾 */
	wkRTC_TotalSec = ApiRtcDrv_mktime( gvClock );
	
	/* ���[�J������������RTC�ƈ�v�A���̓��[�J������������RTC���� */
	if( wkRTC_TotalSec <= gvLocalTime )
	{
		SubTimeComp_Initial();						/* �����␳������ */
	}
	/* ���[�J������������RTC���� */
	else
	{
		;
	}
}


/*
 *******************************************************************************
 *	����RTC�i�ݕ␳����
 *
 *	[���e]
 *		����RTC�i�ݕ␳����
 *******************************************************************************
 */
static void SubTimeComp_Fast( void )
{
	uint32_t	wkRTC_TotalSec;						/* �o�ߕb */
	ST_RTC_t	wkClock;
	
	/* ���^�����Ǝ������r���āA���^1�A���^2�̑���E���^���s�����̔��f */
	wkClock = ApiRtcDrv_localtime( gvLocalTime );
	SubTimeComp_MeaExeChk( wkClock );
	
	/* ����RTC�̎����擾 */
	wkRTC_TotalSec = ApiRtcDrv_mktime( gvClock );
	
	/* ����RTC�����[�J�������ȏ� */
	if( wkRTC_TotalSec >= gvLocalTime )
	{
		SubTimeComp_Initial();						/* �����␳������ */
	}
	else
	{
		;											/* ������ */
	}
}
#endif

/*
 *******************************************************************************
 *	����RTC�x��i�ݕ␳����
 *
 *	[���e]
 *		����RTC�x��i�ݕ␳����
 *******************************************************************************
 */
static void SubTimeComp_DelayFast( void )
{
	uint32_t	wkRTC_TotalSec;						/* �o�ߕb */
	ST_RTC_t	wkClock;

	/* ���݂̃��[�J�������Ŏ��^1�A���^2�̑���E���^���s�����̔��f */
	wkClock = ApiRtcDrv_localtime( gvLocalTime );
	SubTimeComp_MeaExeChk( wkClock );
	
	/* ����RTC�̎����擾 */
	wkRTC_TotalSec = ApiRtcDrv_mktime( gvClock );
	if( gvRtcSts == ecRtcSts_Delay )
	{
		/* ���[�J���������J�E���g�A�b�v */
		gvLocalTime += 2U;
		
		/* ���^�����Ǝ������r���āA���^1�A���^2�̑���E���^���s�����̔��f */
		wkClock = ApiRtcDrv_localtime( gvLocalTime );
		SubTimeComp_MeaExeChk( wkClock );
		
		/* ���[�J������������RTC�ƈ�v�A���̓��[�J������������RTC���� */
		if( wkRTC_TotalSec <= gvLocalTime )
		{
			SubTimeComp_Initial();						/* �����␳������ */
		}
	}
	else
	{
		/* ����RTC�����[�J�������ȏ� */
		if( wkRTC_TotalSec >= gvLocalTime )
		{
			SubTimeComp_Initial();						/* �����␳������ */
		}
	}
}


/*
 *******************************************************************************
 * ���������Ɠ���RTC�̎������r
 *
 *	[���e]
 *		���������Ɠ���RTC�̎������r���A�����␳�����̃X�e�[�^�X���X�V
 *		���[�J�������ɓ���RTC������ۑ����A�������������RTC�����ɕۑ��B
 *		���^1�Ǝ��^2�̎��^�����ɏ]���ARTC�����݂̎��Ԃ�ύX
 *	[����]
 *		uint32_t arRfClock : ��������(�o�ߕb�F0.5�b����\)
 *		uint8_t arSize : �����T�C�Y -> 0�F32bit, 1:14bit
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiTimeComp_first( uint32_t arRfClock, uint8_t arSize )
{
	uint8_t				wkDirectFlg;
	
	uint16_t			wkPlusDiff;
	uint16_t			wkMinusDiff;
	
	uint32_t			wkRTC_TotalSec;
	uint32_t			wkRF_TotalSec;
	uint32_t			wkRTC_Val;
	uint32_t			wkLocalTime;
	uint32_t			wkU32;
	
	sint32_t			wkClockHist = 0U;
	ET_RtcModuleSts_t	wkUpdate = ecRtcModuleSts_Init;
	
	/* ����RTC�o�ߕb */
	wkRTC_TotalSec = ApiRtcDrv_mktime( gvClock );
	wkRTC_TotalSec *= 2U;									/* 0.5sec����\ */
	
	
	/* �����T�C�Y:14bit�␳ */
	if( arSize == imClock_14bitSize )
	{
		/* �ڑ����̃t���T�C�Y�̎��v�擾��1���Ԗ����̎��v����M���Ă��� */
		if( vTimeCompFlg == imON )
		{
			/* �@�펞����14bit�ɂ��Ĕ�r���� */
			wkRTC_Val = wkRTC_TotalSec & 0x3FFFU;
			
			/* �␳���� > �@�펞�� */
			if( arRfClock > wkRTC_Val )
			{
				wkU32 = arRfClock - wkRTC_Val;
				wkPlusDiff = (uint16_t)wkU32;
				wkU32 = (imTimCompMaxCnt + 1U + wkRTC_Val) - arRfClock;
				wkMinusDiff = (uint16_t)wkU32;
			}
			/* �␳���� < �@�펞�� */
			else if( arRfClock < wkRTC_Val )
			{
				wkU32 = wkRTC_Val - arRfClock;
				wkMinusDiff = (uint16_t)wkU32;
				wkU32 = (imTimCompMaxCnt + 1U + arRfClock) - wkRTC_Val;
				wkPlusDiff = (uint16_t)wkU32;
			}
			else
			{
				;
			}
			
			if( arRfClock != wkRTC_Val )
			{
				if( wkPlusDiff >= wkMinusDiff )
				{
					/* �����̍����������}�C�i�X�␳���̗p */
					wkU32 = wkMinusDiff;
					wkDirectFlg = 0U;
				}
				else if( wkMinusDiff > wkPlusDiff )
				{
					/* �����̍����������v���X�␳���̗p */
					wkU32 = wkPlusDiff;
					wkDirectFlg = 1U;
				}
				
				/* �}1hr(7200cnt)�ȓ��͕␳���s */
				if( wkU32 <= imTimCompLimTime )
				{
					/* �}�C�i�X�␳ */
					if( wkDirectFlg == 0U )
					{
						/* �@��̑���������}�C�i�X�␳�����������J�E���g�������̎��� */
						wkRF_TotalSec = wkRTC_TotalSec - wkU32;
					}
					else
					{
						/* �@��̑������Ƀv���X�␳���𑫂����J�E���g�������̎��� */
						wkRF_TotalSec = wkRTC_TotalSec + wkU32;
					}
				}
				else
				{
					wkRF_TotalSec = wkRTC_TotalSec;					/* �␳�Ȃ� */
				}
			}
			else
			{
				wkRF_TotalSec = wkRTC_TotalSec;						/* �␳�Ȃ� */
			}
		}
		else
		{
			return;
		}
	}
	/* �����T�C�Y:�t���T�C�Y */
	else
	{
		/* RF�����o�ߕb */
		wkRF_TotalSec = arRfClock;
	}
	
	/* ���[�J������(0.5�b����\) */
	wkLocalTime = gvLocalTime * 2U;
	
	/* �����␳�X�e�[�^�X�X�V */
	/* Delay */
	if( wkRF_TotalSec > wkRTC_TotalSec )
	{
		wkClockHist = wkRF_TotalSec - wkRTC_TotalSec;
		
		if( imTimCompLimTime > wkClockHist )
		{
			/* �����t���T�C�Y�␳ */
			if( arSize == imClock_32bitSize )
			{
				vTimeCompFlg = imON;												/* ���v�␳�L�� */
			}
			
			switch( gvRtcSts )
			{
				/* Non��Delay */
				case ecRtcSts_Non:
					wkUpdate = ecRtcModuleSts_UpdateLose;
					gvRtcSts = ecRtcSts_Delay;										/* ����RTC���x��Ă��� */
					gvLocalTime = wkRTC_TotalSec / 2U;
					break;
					
				/* Delay��Delay */
				case ecRtcSts_Delay:
					wkUpdate = ecRtcModuleSts_UpdateLose;
					break;
					
				/* Fast��Fast�̂܂� or Delay */
				case ecRtcSts_Fast:
					if( wkRF_TotalSec >= wkLocalTime )								/* ��������>=���[�J�������̏ꍇ */
					{ 
						wkUpdate = ecRtcModuleSts_UpdateLose;
						gvRtcSts = ecRtcSts_Delay;									/* Delay�Ɉڍs */
					}
					else															/* ��L�ȊO */
					{
						wkUpdate = ecRtcModuleSts_UpdateGain;
					}
					break;
					
				default:
					break;
			}
		}
	}
	/* Fast */
	else if( wkRF_TotalSec < wkRTC_TotalSec )
	{
		/* �����ŐV�������� */
		if( gvInFlash.mData.mNewestHistTime != 0xFFFFFFFFU )
		{
			if( imTimCompLimTime > (wkRTC_TotalSec - wkRF_TotalSec) )
			{
				/* �␳�����F�����ŐV�������Â������ɂ͕␳���Ȃ� */
				if( wkRF_TotalSec < (gvInFlash.mData.mNewestHistTime * 2U) )
				{
					wkRF_TotalSec = gvInFlash.mData.mNewestHistTime * 2U;
				}
			}
		}
		
		/* �␳�ʂ�1�b�i�݈ȏ゠�� */
		if( wkRF_TotalSec < wkRTC_TotalSec )
		{
			wkClockHist = wkRTC_TotalSec - wkRF_TotalSec;
			
			if( imTimCompLimTime > wkClockHist )
			{
				/* �����t���T�C�Y�␳ */
				if( arSize == imClock_32bitSize )
				{
					vTimeCompFlg = imON;												/* ���v�␳�L�� */
				}
				
				switch( gvRtcSts )
				{
					/* Non��Fast */
					case ecRtcSts_Non:
						wkUpdate = ecRtcModuleSts_UpdateGain;
						gvRtcSts = ecRtcSts_Fast;										/* ����RTC���i��ł��� */
						gvLocalTime = wkRTC_TotalSec / 2U;
						break;
						
					/* Delay��Delay�̂܂� or Fast */
					case ecRtcSts_Delay:
						if( wkRF_TotalSec >= wkLocalTime )								/* ��������>=���[�J�������̏ꍇ */
						{ 
							wkUpdate = ecRtcModuleSts_UpdateLose;
						}
						else															/* ��L�ȊO */
						{
							wkUpdate = ecRtcModuleSts_UpdateGain;
							gvRtcSts = ecRtcSts_Fast;									/* ���[�J���������i��ł��� */
						}
						break;
						
					/* Fast��Fast */
					case ecRtcSts_Fast:
						wkUpdate = ecRtcModuleSts_UpdateGain;
						break;
						
					default:
						break;
				}
			}
			wkClockHist *= -1;
		}
	}
	else
	{
		/* �����t���T�C�Y�␳ */
		if( arSize == imClock_32bitSize )
		{
			vTimeCompFlg = imON;													/* ���v�␳�L�� */
		}
	}
	
	
	if( wkUpdate != ecRtcModuleSts_Init )
	{
		wkRF_TotalSec /= 2U;
		
		/* ���[�J�������ɓ���RTC������ۑ� */
		/** �������������RTC�ɕۑ� **/
		gvClock = ApiRtcDrv_localtime( wkRF_TotalSec );
		
		/* Main�֐��Ŏ��s */
		gvModuleSts.mRtc = wkUpdate;
		
#if 0	/* �f�[�^�����f�o�b�O */
		/* �}1���ȏ�̕␳ */
//		if( wkClockHist >= 6 || wkClockHist <= -6 )
//		{
			/* ���엚��(���v�␳: ���v�␳�ʁA��������) */
			wkClockHist /= 2;													/* ����\0.5�b����1�b�� */
			ApiFlash_WriteSysLog( ecSysLogItm_FastCom, wkClockHist );
//			ApiFlash_WriteSysLog( ecSysLogItm_TimeComp, wkClockHist );
//		}
#else
		/* �}1���ȏ�̕␳ */
		if( wkClockHist >= 120 || wkClockHist <= -120 )
		{
			/* ���엚��(���v�␳: ���v�␳�ʁA��������) */
			wkClockHist /= 2;													/* ����\0.5�b����1�b�� */
			ApiFlash_WriteSysLog( ecSysLogItm_TimeComp, wkClockHist );
		}
#endif
	}
}

/*
 *******************************************************************************
 *	�]�蔻��
 *
 *	[���e]
 *		���^1�̗]�蔻��
 *	[����]
 *		ST_RTC_t	arClock:����
 *	[�߂�l]
 *		uint8_t		wkSurPlus:�]�茋��(imON:�]��L�AimOFF:�]�薳)
 *******************************************************************************
 */
static uint8_t	SubTimeComp_SurPlusJudgeMeas1( ST_RTC_t arClock )
{
	uint8_t		wkSurPlus;
	
	/* �ϐ������� */
	wkSurPlus = imOFF;
	
	switch( gvInFlash.mParam.mLogCyc1 )
	{
		case ecRecKind1_5sec:
			if(arClock.mSec % 5U == 1U)
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_10sec:
			if(arClock.mSec % 10U == 1U)
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_30sec:
			if(arClock.mSec % 30U == 1U)
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_1min:
			if(arClock.mSec == 1U)
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_5min:
			if((arClock.mSec == 1U) && (arClock.mMin % 5U == 0U))
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_10min:
			if((arClock.mSec == 1U) && (arClock.mMin % 10U == 0U))
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_15min:
			if((arClock.mSec == 1U) && (arClock.mMin % 15U == 0U))
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_30min:
			if((arClock.mSec == 1U) && (arClock.mMin % 30U == 0U))
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_60min:
			if((arClock.mSec == 1U) && (arClock.mMin == 0U))
			{
				wkSurPlus = imON;
			}
			break;
		case ecRecKind1_None:
		default :
			break;
	}
	return wkSurPlus;
}


/*
 *******************************************************************************
 *	���^�f�[�^�Ԃ̎����������^�����̐ݒ�ƈ�v���邩�𔻒�
 *
 *	[���e]
 *		���^�f�[�^�Ԃ̎����������^�����̐ݒ�ƈ�v���邩�𔻒肵�A
 *		���茋�ʁA���͐ݒ�l��Ԃ�
 *	[����]
 *		uint32_t	arSec�F�b��
 *	[�߂�l]
 *		uint8_t		wkInt: �ݒ�l:���^���� ET_RecKind1_t
 *******************************************************************************
 */
uint8_t ApiIntMeasExist( uint32_t arSec )
{
	uint8_t wkInt;
	
	switch( arSec )
	{
		case 5U:
			wkInt = (uint8_t)ecRecKind1_5sec;
			break;
		case 10U:
			wkInt = (uint8_t)ecRecKind1_10sec;
			break;
		case 30U:
			wkInt = (uint8_t)ecRecKind1_30sec;
			break;
		case 60U:
			wkInt = (uint8_t)ecRecKind1_1min;
			break;
		case 300U:
			wkInt = (uint8_t)ecRecKind1_5min;
			break;
		case 600U:
			wkInt = (uint8_t)ecRecKind1_10min;
			break;
		case 900U:
			wkInt = (uint8_t)ecRecKind1_15min;
			break;
		case 1800U:
			wkInt = (uint8_t)ecRecKind1_30min;
			break;
		case 3600U:
			wkInt = (uint8_t)ecRecKind1_60min;
			break;
		default:
			wkInt = (uint8_t)ecRecKind1_None;
			break;
	}
	
	return wkInt;
}


/*
 *******************************************************************************
 *	���v�␳�̃t���O����
 *
 *	[���e]
 *		���v�␳�̃t���O�𖳌��ɂ���(GW�Ƃ̖����ؒf���ɃN���A�����)
 *	[����]
 *		�Ȃ�
 *	[�߂�l]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiTimeComp_TimeCompFlgClr( void )
{
	vTimeCompFlg = imOFF;
}


/*
 *******************************************************************************
 *	���O������RTC�������r��RTC�����𐧌�
 *
 *	[���e]
 *		RTC�����̓��O�������O�ɂȂ�Ȃ��悤�ɐ���
 *	[����]
 *		�Ȃ�
 *	[�߂�l]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiTimeComp_LimitRtcClock( void )
{
	uint32_t wkU32;
	uint32_t wkRTC_TotalSec;
	
	/* ���������A�v���l���O�����̂����ꂩ�������l�ł͂Ȃ�(�l�����݂���) */
	if( gvInFlash.mData.mNewestHistTime != 0xFFFFFFFFU || gvInFlash.mData.mNewestLogTime != 0xFFFFFFFFU )
	{
		/* �����l�����݂��� */
		if( gvInFlash.mData.mNewestHistTime != 0xFFFFFFFFU && gvInFlash.mData.mNewestLogTime != 0xFFFFFFFFU )
		{
			if( gvInFlash.mData.mNewestHistTime <= gvInFlash.mData.mNewestLogTime )
			{
				wkU32 = gvInFlash.mData.mNewestLogTime;
			}
			else
			{
				wkU32 = gvInFlash.mData.mNewestHistTime;
			}
		}
		/* ���������̂ݑ��݂��� */
		else if( gvInFlash.mData.mNewestHistTime != 0xFFFFFFFFU )
		{
			wkU32 = gvInFlash.mData.mNewestHistTime;
		}
		/* �v���l���O�����̂ݑ��݂��� */
		else
		{
			wkU32 = gvInFlash.mData.mNewestLogTime;
		}
		
		/* ����RTC�o�ߕb */
		wkRTC_TotalSec = ApiRtcDrv_mktime( gvClock );
		
		/* ������r���A���O�����ȑO�Ȃ玞���␳ */
		if( wkRTC_TotalSec < wkU32 )
		{
			gvClock = ApiRtcDrv_localtime( wkU32 + 1 );
			ApiRtcDrv_SetInRtc( gvClock );
		}
	}
}


/*
 *******************************************************************************
 *	Flash�Ƀ��������郍�[�J���������擾(�������^)
 *
 *	[���e]
 *		Flash�Ƀ��������郍�[�J���������擾
 *	[����]
 *		�Ȃ�
 *	[�߂�l]
 *		�Ȃ�
 *******************************************************************************
 */
uint32_t ApiTimeComp_GetLocalTime( void )
{
	uint32_t wkTimeInfo;
	ST_RTC_t wkClock;
	
	wkClock = ApiRtcDrv_localtime( gvLocalTime );
	wkTimeInfo = SubTimeComp_GetEvtRecTime( wkClock, 1U );
	
	return wkTimeInfo;
}
#pragma section

