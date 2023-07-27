/*
 *******************************************************************************
 *	File name	:	Abnormal.c
 *
 *	[���e]
 *		���菈��
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2019.02.22		N.Yat			�V�K�쐬
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

#define imROMCRCADDRESS		(*(volatile __far unsigned short *)0x17DFE)
#define imCRCStartAdrS1		0x1C0U
#define imCRCEndAdrS1		0xFFFU
#define imCRCStatAdrS2		0x10CEU
#define imCRCEndAdrS2		0x17DFDU

#define	imKeyChk_ErrCnt		30U					/* �L�[����ُ�J�E���^ */

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
typedef struct ST_AbnInf
{
	uint8_t			mAbnNum;					/* ���߂Ŕ��������@��ُ�No. */
	uint16_t		mMeasSts;					/* ����n�ُ��� */
} ST_AbnInf_t;


typedef struct ST_MeasAbnHist
{
	uint16_t			mAbnSts;
	uint8_t				mCh;
	ET_MeasAlmItm_t		mAlmItm;
	ET_ErrInfItm_t		mItm;
} ST_MeasAbnHist_t;

/*
 *==============================================================================
 *	�ϐ���`
 *==============================================================================
 */
static ST_AbnInf_t		stAbnInf = { 0U, 0U };	/* �@��ُ��� */

/*
 *==============================================================================
 *	�萔��`
 *==============================================================================
 */
#if (swSensorCom == imEnable)
const ST_MeasAbnHist_t cMeasAbnHistType[ 14U ] =
{
	/* mAbnSts,				mCh		mAlmItm					mItm */
	{ imAbnSts_OvRngCh1,	0U,		ecMeasAlmItm_OvRng,		ecErrInfItm_1chOvrRng },
	{ imAbnSts_OvRngCh2,	1U,		ecMeasAlmItm_OvRng,		ecErrInfItm_2chOvrRng },
	{ imAbnSts_OvRngCh3,	2U,		ecMeasAlmItm_OvRng,		ecErrInfItm_3chOvrRng },
	{ imAbnSts_UdRngCh1,	0U,		ecMeasAlmItm_UdRng,		ecErrInfItm_1chUdrRng },
	{ imAbnSts_UdRngCh2,	1U,		ecMeasAlmItm_UdRng,		ecErrInfItm_2chUdrRng },
	{ imAbnSts_UdRngCh3,	2U,		ecMeasAlmItm_UdRng,		ecErrInfItm_3chUdrRng },
	{ imAbnSts_BurnCh1,		0U,		ecMeasAlmItm_Burn,		ecErrInfItm_1chBrnOut },
	{ imAbnSts_BurnCh2,		1U,		ecMeasAlmItm_Burn,		ecErrInfItm_2chBrnOut },
	{ imAbnSts_BurnCh3,		2U,		ecMeasAlmItm_Burn,		ecErrInfItm_3chBrnOut },
	{ imAbnSts_ShortCh1,	0U,		ecMeasAlmItm_Short,		ecErrInfItm_1chSnsShrt },
	{ imAbnSts_ShortCh2,	1U,		ecMeasAlmItm_Short,		ecErrInfItm_2chSnsShrt },
	{ imAbnSts_ShortCh3,	2U,		ecMeasAlmItm_Short,		ecErrInfItm_3chSnsShrt },
	{ imAbnSts_SnsrComErr,	0U,		ecMeasAlmItm_Init,		ecErrInfItm_SnsComErr },
	{ imAbnSts_SnsrErr,		0U,		ecMeasAlmItm_Init,		ecErrInfItm_SnsErr },
};
#else
const ST_MeasAbnHist_t cMeasAbnHistType[ 12U ] =
{
	/* mAbnSts,				mCh		mAlmItm					mItm */
	{ imAbnSts_OvRngCh1,	0U,		ecMeasAlmItm_OvRng,		ecErrInfItm_1chOvrRng },
	{ imAbnSts_OvRngCh2,	1U,		ecMeasAlmItm_OvRng,		ecErrInfItm_2chOvrRng },
	{ imAbnSts_OvRngCh3,	2U,		ecMeasAlmItm_OvRng,		ecErrInfItm_3chOvrRng },
	{ imAbnSts_UdRngCh1,	0U,		ecMeasAlmItm_UdRng,		ecErrInfItm_1chUdrRng },
	{ imAbnSts_UdRngCh2,	1U,		ecMeasAlmItm_UdRng,		ecErrInfItm_2chUdrRng },
	{ imAbnSts_UdRngCh3,	2U,		ecMeasAlmItm_UdRng,		ecErrInfItm_3chUdrRng },
	{ imAbnSts_BurnCh1,		0U,		ecMeasAlmItm_Burn,		ecErrInfItm_1chBrnOut },
	{ imAbnSts_BurnCh2,		1U,		ecMeasAlmItm_Burn,		ecErrInfItm_2chBrnOut },
	{ imAbnSts_BurnCh3,		2U,		ecMeasAlmItm_Burn,		ecErrInfItm_3chBrnOut },
	{ imAbnSts_ShortCh1,	0U,		ecMeasAlmItm_Short,		ecErrInfItm_1chSnsShrt },
	{ imAbnSts_ShortCh2,	1U,		ecMeasAlmItm_Short,		ecErrInfItm_2chSnsShrt },
	{ imAbnSts_ShortCh3,	2U,		ecMeasAlmItm_Short,		ecErrInfItm_3chSnsShrt },
};
#endif

/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
 */
static void SubAbn_AbnHistSet( uint16_t arAbnSts, uint8_t arOnOff );
static void SubAbn_MeasAbnHistSet( uint16_t arAbnSts, uint8_t arOnOff );


#pragma section text MY_APP2

/*
 *******************************************************************************
 * �@��ُ��Ԏ擾
 *
 *	[���e]
 *		�@��ُ��Ԃ��擾����B
 *	[����]
 *		ST_AbnInfKind_t	arKind: �@��ُ� or ����n�ُ�
 *	[�ߒl]
 *		uint16_t	stAbnInf
 *******************************************************************************
 */
uint16_t ApiAbn_AbnStsGet( ST_AbnInfKind_t arKind )
{
	if( arKind == ecAbnInfKind_AbnSts )
	{
		return gvInFlash.mData.mAbnInf_Sts;
	}
	else
	{
		return stAbnInf.mMeasSts;
	}
}

/*
 *******************************************************************************
 * ���߂Ŕ��������@��ُ�No.���擾
 *
 *	[���e]
 *		���߂Ŕ��������@��ُ�No.���擾(Err�\���̔ԍ��Ɏg�p)
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		uint8_t	stAbnInf.mAbnNum
 *******************************************************************************
 */
uint8_t ApiAbn_AbnNumGet( void )
{
	return stAbnInf.mAbnNum;
}

/*
 *******************************************************************************
 * �@��ُ�Z�b�g
 *
 *	[���e]
 *		�@��ُ���Z�b�g����B
 *	[����]
 *		uint16_t		arAbnSts: �ُ�X�e�[�^�X
 *		ST_AbnInfKind_t	arKind: �@��ُ� or ����n�ُ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiAbn_AbnStsSet( uint16_t arAbnSts, ST_AbnInfKind_t arKind )
{
	uint8_t		wkLoop;
	uint16_t	wkSts;
	
	if( arKind == ecAbnInfKind_AbnSts )
	{
		if( !(gvInFlash.mData.mAbnInf_Sts & arAbnSts) )
		{
			gvInFlash.mData.mAbnInf_Sts |= arAbnSts;
			
			/* ���߂Ŕ��������@��ُ�No.���Z�b�g(�d�r�c�ʒቺ�͏���) */
			if( arAbnSts != imAbnSts_BatLow && arAbnSts != imAbnSts_BatEmpty )
			{
				/* �@��ُ�ߋ��x��t���O */
				if( gvInFlash.mData.mAlmPast[ 4U ] == ecAlmPastSts_Non )
				{
					gvInFlash.mData.mAlmPast[ 4U ] = ecAlmPastSts_Pre;
				}
				
				for( wkLoop = 0U, wkSts = arAbnSts ; wkLoop < 16U ; wkLoop++ )
				{
					if( M_TSTBIT( wkSts, M_BIT0 ) )
					{
						stAbnInf.mAbnNum = wkLoop;
						
						if( gvDispMode.mNowDisp != ecDispMode_UpErrDwRtc )
						{
							if( gvDispMode.mNowDisp != ecDispMode_AbnChkErr )
							{
								gvDispMode.mPreDisp = gvDispMode.mNowDisp;
							}
							gvDispMode.mNowDisp = ecDispMode_AbnChkErr;
							gvModuleSts.mLcd = ecLcdModuleSts_Run;
						}
						break;
					}
					else
					{
						wkSts >>= 1U;
					}
				}
			}
			
			/* �������L�^���� */
			SubAbn_AbnHistSet( arAbnSts, imON );
		}
	}
	else
	{
		if( !(stAbnInf.mMeasSts & arAbnSts) )
		{
			stAbnInf.mMeasSts |= arAbnSts;
			
			/* �@��ُ�ߋ��x��t���O */
			if( gvInFlash.mData.mAlmPast[ 4U ] == ecAlmPastSts_Non )
			{
				gvInFlash.mData.mAlmPast[ 4U ] = ecAlmPastSts_Pre;
			}
			
			/* �v���ُ헚�����L�^���� */
			SubAbn_MeasAbnHistSet( arAbnSts, imON );
			
		}
	}
	
}


/*
 *******************************************************************************
 * �@��ُ�N���A
 *
 *	[���e]
 *		�@��ُ���N���A����B
 *	[����]
 *		uint16_t		arAbnSts: �ُ�X�e�[�^�X
 *		ST_AbnInfKind_t	arKind: �@��ُ� or ����n�ُ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiAbn_AbnStsClr( uint16_t arAbnSts, ST_AbnInfKind_t arKind )
{
	if( arKind == ecAbnInfKind_AbnSts )
	{
		if( gvInFlash.mData.mAbnInf_Sts & arAbnSts )
		{
			gvInFlash.mData.mAbnInf_Sts &= ~arAbnSts;
			
			/* �@��ُ�ߋ��x��t���O */
			if( gvInFlash.mData.mAlmPast[ 4U ] == ecAlmPastSts_Pre )
			{
				gvInFlash.mData.mAlmPast[ 4U ] = ecAlmPastSts_Set;
			}
			/* �������L�^���� */
			SubAbn_AbnHistSet( arAbnSts, imOFF );
			
		}
	}
	else
	{
		if( stAbnInf.mMeasSts & arAbnSts )
		{
			stAbnInf.mMeasSts &= ~arAbnSts;
			
			/* �@��ُ�ߋ��x��t���O */
			if( gvInFlash.mData.mAlmPast[ 4U ] == ecAlmPastSts_Pre )
			{
				gvInFlash.mData.mAlmPast[ 4U ] = ecAlmPastSts_Set;
			}
			/* �v���ُ헚�����L�^���� */
			SubAbn_MeasAbnHistSet( arAbnSts, imOFF );
	
		}
	}
}


/*
 *******************************************************************************
 * �ُ헚�����L�^����
 *
 *	[���e]
 *		�ُ헚�����L�^����
 *	[����]
 *		uint16_t		arAbnSts: �ُ�X�e�[�^�X
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubAbn_AbnHistSet( uint16_t arAbnSts, uint8_t arOnOff )
{
	ET_ErrInfItm_t wkItm;
	
	switch( arAbnSts )
	{
		/* �d�r�c�ʋ� */
		case imAbnSts_BatEmpty:
			wkItm = ecErrInfItm_BatEmpty;
			if( arOnOff == imOFF )
			{
				ApiFlash_WriteActLog( ecActLogItm_BattIn, 0U, 4U );				/* ���엚���F�d�r���� */
			}
			break;
		/* ����IC�ُ� */
		case imAbnSts_RF:
			return;
		/* �v���C�}��Flash�������ُ� */
		case imAbnSts_PMEM:
			wkItm = ecErrInfItm_PrimMemErr;
			break;
		/* �Z�J���_��Flash�������ُ� */
		case imAbnSts_SMEM:
			wkItm = ecErrInfItm_SecondMemErr;
			break;
		/* �}�C�R��ROM�ُ� */
		case imAbnSts_ROM:
			wkItm = ecErrInfItm_MiComRomErr;
			break;
		/* debug */
		case imAbnSts_Dummy3:
			return;
		/* ���U�q�ُ� */
		case imAbnSts_CLK:
			wkItm = ecErrInfItm_ClkErr;
			break;
		/* �L�[�ُ� */
		case imAbnSts_KEY:
			wkItm = ecErrInfItm_KeyBehErr;
			break;
		/* �O�t��RTC�ُ� */
		case imAbnSts_RTC:
			wkItm = ecErrInfItm_ExRtcErr;
			break;
		/* ����Flash�ُ� */
		case imAbnSts_INFLSH:
			return;
		/* ����RTC�ُ� */
		case imAbnSts_INRTC:
			wkItm = ecErrInfItm_InRtcErr;
			break;
		/* �d�r�c�ʒቺ */
		case imAbnSts_BatLow:
			if( arOnOff == imOFF )
			{
				return;
			}
			wkItm = ecErrInfItm_BatLow;
			break;
		default:
			return;
	}
	
	ApiFlash_WriteErrInfo( wkItm, arOnOff );
}

/*
 *******************************************************************************
 * �v���ُ헚�����L�^����
 *
 *	[���e]
 *		�v���ُ헚�����L�^����
 *	[����]
 *		uint16_t		arAbnSts: �ُ�X�e�[�^�X
 *		uint8_t			arOnOFF: ����/����
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubAbn_MeasAbnHistSet( uint16_t arAbnSts, uint8_t arOnOff )
{
	uint8_t wkLoop;
	uint32_t wkHistTime;
	
	for( wkLoop = 0U ; wkLoop < M_ArrayElement(cMeasAbnHistType) ; wkLoop++ )
	{
		if( arAbnSts == cMeasAbnHistType[ wkLoop ].mAbnSts )
		{
			break;
		}
	}
	if( wkLoop < M_ArrayElement(cMeasAbnHistType) )
	{
		/* �@��ُ헚�� */
		ApiFlash_WriteErrInfo( cMeasAbnHistType[ wkLoop ].mItm, arOnOff );
		
		/* �v���x�񗚗� */
		if( cMeasAbnHistType[ wkLoop ].mAlmItm != ecMeasAlmItm_Init )
		{
			if( gvModuleSts.mEventKey == ecEventKeyModuleSts_Meas )
			{
				wkHistTime = gvMeasPrm.mTimeEvt;			/* �������(�C�x���g���s��) */
			}
			else
			{
				wkHistTime = gvMeasPrm.mTime;				/* ������� */
			}
			ApiFlash_WriteMeasAlm( wkHistTime, arOnOff, cMeasAbnHistType[ wkLoop ].mCh, 0U, gvMeasPrm.mMeasVal[ cMeasAbnHistType[ wkLoop ].mCh ], cMeasAbnHistType[ wkLoop ].mAlmItm );
		}
		
		gvMeasAlmNum ++;									/* �v���x�񔭐�/�����񐔃C���N�������g */
	}
//	gvHsMeasAlmFlg = imON;
}


/*
 *******************************************************************************
 * �n�[�h�G���[�̊m�F
 *
 *	[���e]
 *		�n�[�h�G���[�̊m�F����B
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiAbn_Chk( void )
{
	uint8_t			wkLoop;
	uint8_t	wkRet;
	uint32_t		wkTau0Ch1Cnt;
	
	static uint8_t	vKeyCnt[ 3U ] = { 0U, 0U, 0U };
	static uint8_t	vClkNgCnt = 0U;
	
	/* �L�[����m�F */
	if( gvModuleSts.mErrChk & imErrChk_KeyChk )
	{
		wkRet = imOFF;
		
		if( KEY_EVENT == imHigh )
		{
			vKeyCnt[ 0U ] ++;
		}
		else
		{
			vKeyCnt[ 0U ] = 0U;
		}
		
		if( KEY_DISP == imHigh )
		{
			vKeyCnt[ 1U ] ++;
		}
		else
		{
			vKeyCnt[ 1U ] = 0U;
		}
		
		if( KEY_RESET == imHigh )
		{
			vKeyCnt[ 2U ] ++;
		}
		else
		{
			vKeyCnt[ 2U ] = 0U;
		}
		
		for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
		{
			/* 30��A��(30min)��ON��� */
			if( vKeyCnt[ wkLoop ] > imKeyChk_ErrCnt )
			{
				wkRet = imON;
			}
		}
		
		/* �L�[�A��ON */
		if( wkRet == imON )
		{
			/* �L�[�ُ픭�� */
			ApiAbn_AbnStsSet( imAbnSts_KEY, ecAbnInfKind_AbnSts );
		}
#if 0	/* �@��ُ한�A�����Ȃ� */
		else
		{
			/* �L�[�ُ���� */
			ApiAbn_AbnStsClr( imAbnSts_KEY, ecAbnInfKind_AbnSts );
	}
#endif
		gvModuleSts.mErrChk &= ~imErrChk_KeyChk;
	}
	
	/* ���U�q����m�F */
	if( gvModuleSts.mErrChk & imErrChk_CycChk )
	{
		wkTau0Ch1Cnt = ApiTau0_GetTau0Ch1Cnt();						/* �^�C�}0�`���l��1�̃J�E���^���擾 */
		
		/* ���U���͈͊O */
		if( wkTau0Ch1Cnt < 724 || wkTau0Ch1Cnt > 739 )
		{
			vClkNgCnt ++;
			/* 10��A������ */
			if( vClkNgCnt > 10U )
			{
				/* ���U�q�ُ픭�� */
				ApiAbn_AbnStsSet( imAbnSts_CLK, ecAbnInfKind_AbnSts );
			}
		}
		else
		{
			vClkNgCnt = 0U;
			
#if 0	/* �@��ُ한�A�����Ȃ� */
			/* ���U�q�ُ���� */
			ApiAbn_AbnStsClr( imAbnSts_CLK, ecAbnInfKind_AbnSts );
#endif
		}
		gvModuleSts.mErrChk &= ~imErrChk_CycChk;
	}
}


/*
 *******************************************************************************
 * Err�\���p�̔ԍ��Z�b�g
 *
 *	[���e]
 *		Err�\���p�̔ԍ��Z�b�g���A�\���J��
 *		�d�r��:bit0�A�d�r�c�ʒቺ:bit12������
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiAbn_AbnNumSet( void )
{
	uint8_t		wkLoop;
	uint16_t	wkBit;
	
	
	if( gvInFlash.mData.mAbnInf_Sts & 0x0FFE )
	{
		for( wkLoop = 0U, wkBit = 1U ; wkLoop < 16U ; wkLoop++, wkBit <<= 1U )
		{
			if( gvInFlash.mData.mAbnInf_Sts & wkBit )
			{
				/* �d�r��:bit0�A�d�r�c�ʒቺ:bit12������ */
				if( wkBit != M_BIT0 && wkBit != M_BIT12 )
				{
					/* Err�ԍ��Z�b�g */
					stAbnInf.mAbnNum = wkLoop;
					
					/* �\���X�V */
					if( gvDispMode.mNowDisp != ecDispMode_UpErrDwRtc )
					{
						if( gvDispMode.mNowDisp != ecDispMode_AbnChkErr )
						{
							gvDispMode.mPreDisp = gvDispMode.mNowDisp;
						}
						gvDispMode.mNowDisp = ecDispMode_AbnChkErr;
						gvModuleSts.mLcd = ecLcdModuleSts_Run;
					}
					break;
				}
			}
		}
	}
}
#pragma section


#pragma section text MY_APP
/*
 *******************************************************************************
 *	ROM CRC���Z����
 *
 *	[���e]
 *		ROM CRC���Z����
 *******************************************************************************
 */
void ApiAbn_ChkCRC( void )
{
	uint8_t __far* __far	wkAdr;
	uint32_t				wkAddr;
	uint8_t					wkVal;
	uint16_t				wkRomCrc;
	
	wkRomCrc = imROMCRCADDRESS;
	
	CRCD = 0x0000U;
	
	/* �v���O�����̈�1 */
	for( wkAddr = imCRCStartAdrS1 ; wkAddr <= imCRCEndAdrS1 ; wkAddr++ )
	{
		wkAdr = (uint8_t __far*)wkAddr;
		
		wkVal = (uint8_t)*wkAdr;
		CRCIN = wkVal;
		NOP();
	}
	
	/* �v���O�����̈�2 */
	for( wkAddr = imCRCStatAdrS2 ; wkAddr <= imCRCEndAdrS2 ; wkAddr++ )
	{
		wkAdr = (uint8_t __far*)wkAddr;
		
		wkVal = (uint8_t)*wkAdr;
		CRCIN = wkVal;
		NOP();
	}
	
	if( wkRomCrc != CRCD )
	{
		/* �}�C�R��ROM�ُ픭�� */
		ApiAbn_AbnStsSet( imAbnSts_ROM, ecAbnInfKind_AbnSts );
	}
#if 0	/* �@��ُ한�A�����Ȃ� */
	else
	{
		/* �}�C�R��ROM�ُ���� */
		ApiAbn_AbnStsClr( imAbnSts_ROM, ecAbnInfKind_AbnSts );
	}
#endif
}
#pragma section
