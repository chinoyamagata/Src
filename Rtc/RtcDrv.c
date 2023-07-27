/*
 *******************************************************************************
 *	File name	:	RtcDrv.c
 *
 *	[���e]
 *		RTC�h���C�o�[
 *		SII�Z�~�R���_�N�^�� S-35390A
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2017.12.21		Softex H.M		�V�K�쐬
 *******************************************************************************
 */
#include "cpu_sfr.h"															/* SFR��`�w�b�_�[ */

#include "typedef.h"															/* �f�[�^�^��` */
#include "UserMacro.h"															/* ���[�U�쐬�}�N����` */
#include "immediate.h"															/* immediate�萔��` */
#include "switch.h"																/* �R���p�C���X�C�b�`��` */
#include "glLabel.h"															/* �O���[�o���ϐ��E�萔(const)���x����` */
#include "enum.h"																/* �񋓌^�̒�` */
#include "struct.h"																/* �\���̒�` */
#include "func.h"																/* �֐��̊O���Q��(extern)��` */


/*
 *==============================================================================
 *	define��`
 *==============================================================================
 */
#define		imRtcDrv_StsReg1ReadCmd		0x61U									/* �X�e�[�^�X���W�X�^1�Ǐo���R�}���h(0110 0001b) */
#define		imRtcDrv_StsReg1WriteCmd	0x60U									/* �X�e�[�^�X���W�X�^1�������݃R�}���h(0110 0000b) */
#define		imRtcDrv_StsReg1_POC		M_BIT0									/* �X�e�[�^�X���W�X�^1 POC�r�b�g�ʒu */
#define		imRtcDrv_StsReg1_BLD		M_BIT1									/* �X�e�[�^�X���W�X�^1 BLD�r�b�g�ʒu */
#define		imRtcDrv_StsReg1_12_24		M_BIT6									/* �X�e�[�^�X���W�X�^1 ~12/24�r�b�g�ʒu */
#define		imRtcDrv_StsReg1_RESET		M_BIT7									/* �X�e�[�^�X���W�X�^1 RESET�r�b�g�ʒu */

#define		imRtcDrv_StsReg2ReadCmd		0x63U									/* �X�e�[�^�X���W�X�^2�Ǐo���R�}���h(0110 0011b) */
#define		imRtcDrv_StsReg2WriteCmd	0x62U									/* �X�e�[�^�X���W�X�^2�������݃R�}���h(0110 0010b) */
#define		imRtcDrv_Int2RegReadCmd		0x6BU									/* INT2���W�X�^�Ǐo���R�}���h(���g���ݒ�o��)(0110 1011b) */
#define		imRtcDrv_Int2RegWriteCmd	0x6AU									/* INT2���W�X�^�������݃R�}���h(���g���ݒ�o��)(0110 1010b) */
#define		imRtcDrv_StsReg2_INT2_ME	M_BIT2									/* �X�e�[�^�X���W�X�^2 NT2�[�q�o�̓��[�h�I�� */
#define		imRtcDrv_StsReg2_INT2_FE	M_BIT3									/* �X�e�[�^�X���W�X�^2 NT2�[�q�o�̓��[�h�I�� */
#define		imRtcDrv_Int2Reg_1Hz		M_BIT7									/* INT2���W�X�^ 1Hz�r�b�g�ʒu */


#define		imRtcDrv_RealTime1ReadCmd	0x65U									/* ���A���^�C���f�[�^1�Ǐo���R�}���h(0110 0101b) */
#define		imRtcDrv_RealTime1WriteCmd	0x64U									/* ���A���^�C���f�[�^1�������݃R�}���h(0110 0100b) */

#define		imRtcDrv_RetryCnt			10U										/* ���g���C�� */
#define		imErrJdgTimer				24000U									/* 1ms(1/24MHz=41.6667ns�A1ms/41.667ns=24000) */

#define		imInRtcJdgCnt				10U										/* ����RTC�A�N�Z�X�ő�� */
#define		imExRtcJdgCnt				5U										/* �O�t��RTC�A�N�Z�X�ő�� */


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
/* RTC�h���C�o�[�p�p�����[�^�\���� */
typedef struct ST_RtcDrvPrm
{
	ET_RtcDrvInitSts_t	mInitSts;												/* �������X�e�[�^�X */
	uint8_t				mInitWaitCnt;											/* �C�j�V�����C�Y�҂��J�E���^(10ms/�J�E���g) */
} ST_RtcDrvPrm_t;


/*
 *==============================================================================
 *	�ϐ���`
 *==============================================================================
 */
static ST_RtcDrvPrm_t	vRtcDrvPrm;												/* RTC�h���C�o�[�p�p�����[�^ */


/*
 *==============================================================================
 *	�萔��`
 *==============================================================================
 */
/* �[�N�̌����Ƃ̓��� */
const uint8_t cDayofLeapMonth[ 13U ] =
{
	0U,						/* dummy */
	31U,					/* 1�� */
	29U,					/* 2�� */
	31U,					/* 3�� */
	30U,					/* 4�� */
	31U,					/* 5�� */
	30U,					/* 6�� */
	31U,					/* 7�� */
	31U,					/* 8�� */
	30U,					/* 9�� */
	31U,					/* 10�� */
	30U,					/* 11�� */
	31U						/* 12�� */
};

/* ���N�̌����Ƃ̓��� */
const uint8_t cDayofMonth[ 13U ] = 
{
	0U,						/* dummy */
	31U,					/* 1�� */
	28U,					/* 2�� */
	31U,					/* 3�� */
	30U,					/* 4�� */
	31U,					/* 5�� */
	30U,					/* 6�� */
	31U,					/* 7�� */
	31U,					/* 8�� */
	30U,					/* 9�� */
	31U,					/* 10�� */
	30U,					/* 11�� */
	31U						/* 12�� */
};

/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
 */
static void SubRtcDrv_RegInitial( void );
static ET_Error_t SubRtcDrv_SetRtc( const ST_RTC_t *parRtc );
static ET_Error_t SubRtcDrv_ReadRtc( void );
static uint8_t SubRtcDrv_BinToBcd( uint8_t arBin );
static uint8_t SubRtcDrv_BcdToBin( uint8_t arBcd );
static uint32_t	SubRtcDrv_ReverseBit32( uint32_t arData );
static void SubRtcDrv_InRtcErrSet( uint8_t arLoop );
static ET_Error_t SubRtcDrv_FlgChk( uint8_t* gvFlg );
//static ET_Error_t SubRtcDrv_RcvFlgChk( void );
//static ET_Error_t SubRtcDrv_SndFlgChk( void );


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	RTC(S-35390A) ����������
 *
 *	[���e]
 *		RTC(S-35390A)�̏������������s���B
 *******************************************************************************
 */
void ApiRtcDrv_Initial( void )
{
	uint8_t		wkStsReg1;
//	uint16_t	wkErrJdgCnt = 0U;												/* �ُ픻��J�E���^ */
	
	R_RTC_Start();																/* ����RTC�N�� */
	
	vRtcDrvPrm.mInitWaitCnt = 0U;												/* �C�j�V�����C�Y�҂��J�E���^(10ms/�J�E���g)������ */
	
	/* �X�e�[�^�X���W�X�^1�Ǐo�� */
	gvIicRcvEndFlg = imOFF;														/* ���M�����t���OOFF */
	R_IIC00_Master_Receive(imRtcDrv_StsReg1ReadCmd, &wkStsReg1, 1U);
	
	/* ���M�����t���O��ON�ɂȂ�܂� */
#if 0
	while( gvIicRcvEndFlg == imOFF )
	{
		wkErrJdgCnt++;
		if( wkErrJdgCnt > imErrJdgTimer )										/* 1ms���߂ŊO�t��RTC�ւ̃A�N�Z�X���s */
		{
			/* ���W�X�^���������s */
			vRtcDrvPrm.mInitSts = ecRtcDrvInitSts_RegInit;						/* �������X�e�[�^�X�����W�X�^������ */
			vRtcDrvPrm.mInitWaitCnt = 50U;										/* �C�j�V�����C�Y�҂��J�E���^��500ms�ݒ� */
			R_TAU0_Channel7_Custom( imTimer07_10msec );							/* 10msecWait */
			gvModuleSts.mRtc = ecRtcModuleSts_Wait;
			return;
		}
	}
#else
	if( ecERR_NG == SubRtcDrv_FlgChk( &gvIicRcvEndFlg ) )
	{
		/* ���W�X�^���������s */
		vRtcDrvPrm.mInitSts = ecRtcDrvInitSts_RegInit;						/* �������X�e�[�^�X�����W�X�^������ */
		vRtcDrvPrm.mInitWaitCnt = 50U;										/* �C�j�V�����C�Y�҂��J�E���^��500ms�ݒ� */
		R_TAU0_Channel7_Custom( imTimer07_10msec );							/* 10msecWait */
		gvModuleSts.mRtc = ecRtcModuleSts_Wait;
		return;
	}
#endif
	
	/* POC�r�b�g��1 */
	if( M_TSTBIT( wkStsReg1, imRtcDrv_StsReg1_POC ) )
	{
		/* ���W�X�^���������s */
		vRtcDrvPrm.mInitSts = ecRtcDrvInitSts_RegInit;							/* �������X�e�[�^�X�����W�X�^������ */
		vRtcDrvPrm.mInitWaitCnt = 50U;											/* �C�j�V�����C�Y�҂��J�E���^��500ms�ݒ� */
		R_TAU0_Channel7_Custom( imTimer07_10msec );								/* 10msecWait */
		gvModuleSts.mRtc = ecRtcModuleSts_Wait;
	}
	else
	{
		vRtcDrvPrm.mInitWaitCnt = 0U;											/* �C�j�V�����C�Y�҂��J�E���^(10ms/�J�E���g)������ */
		if( M_TSTBIT( wkStsReg1, imRtcDrv_StsReg1_BLD ) )						/* �d���ቺ���� */
		{
			vRtcDrvPrm.mInitSts = ecRtcDrvInitSts_RegInit;						/* �������X�e�[�^�X�����W�X�^������ */
			R_TAU0_Channel7_Custom( imTimer07_10msec );							/* 10msecWait */
			gvModuleSts.mRtc = ecRtcModuleSts_Wait;
		}
		else
		{
			if( !M_TSTBIT( wkStsReg1, imRtcDrv_StsReg1_12_24 ) )				/* ����:12/24 */
			{
				/* ���炩�̗��R��12/24�������l�ɂȂ��Ă��� */
				vRtcDrvPrm.mInitSts = ecRtcDrvInitSts_24Write;					/* 24���ԊǗ��֐ݒ� */
				R_TAU0_Channel7_Custom( imTimer07_10msec );						/* 10msecWait */
				gvModuleSts.mRtc = ecRtcModuleSts_Wait;
			}
			else
			{
				/* ���A���^�C���f�[�^�ǂݏo�� */
				vRtcDrvPrm.mInitSts = ecRtcDrvInitSts_RealRead;					/* �������X�e�[�^�X�����A���^�C���f�[�^�ǂݏo�� */
				gvModuleSts.mRtc = ecRtcModuleSts_Run;
			}
		}
	}
	
	/* 1Hz�o��OFF */
	/* �f�t�H���g�o��OFF�Ȃ̂ō폜 */
//	ApiRtcDrv_1HzOut( imOFF );
	
	while( 1 )
	{
		if( gvModuleSts.mRtc == ecRtcModuleSts_Run )
		{
			/* RTC(S-35390A) ���W�X�^������ */
			SubRtcDrv_RegInitial();
			if( vRtcDrvPrm.mInitSts == ecRtcDrvInitSts_InitEnd )
			{
				gvModuleSts.mRtc = ecRtcModuleSts_Sleep;
				break;
			}
		}
	}
}


/*
 *******************************************************************************
 *	RTC(S-35390A) ���W�X�^����������
 *
 *	[���e]
 *		�X�e�[�^�X���W�X�^�̏������y�сA���A���^�C���f�[�^�̏����ݒ���s���B
 *		���W�X�^�������ݒ肪�o���Ȃ��ꍇ�́AecERR_NG��Ԃ��B
 *	[���s�^�C�~���O]
 *		10ms�����^�X�N
 *******************************************************************************
 */
static void SubRtcDrv_RegInitial( void )
{
	uint8_t		wkStsReg1;
	uint8_t		wkRetryCnt;														/* ���g���C��(���l�|1) */
	ST_RTC_t	wkRTC;															/* RTC�f�[�^ */
	ET_Error_t	wkRet = ecERR_OK;												/* �߂�l(�G���[���) */
	
	switch( vRtcDrvPrm.mInitSts )
	{
		/* ���W�X�^������ */
		case ecRtcDrvInitSts_RegInit:
			if( vRtcDrvPrm.mInitWaitCnt > 0U )									/* �C�j�V�����C�Y�҂��J�E���^���� */
			{
				vRtcDrvPrm.mInitWaitCnt--;										/* �C�j�V�����C�Y�҂��J�E���^�f�N�������g */
				R_TAU0_Channel7_Custom( imTimer07_10msec );						/* 10msecWait */
				gvModuleSts.mRtc = ecRtcModuleSts_Wait;
			}
			else
			{
				wkRetryCnt = 0U;												/* ���g���C�J�E���^������ */
				/* IC�����̏����� */
				do
				{
					/* �X�e�[�^�X���W�X�^1�������݁i���Z�b�g�j */
					wkStsReg1 = (uint8_t)imRtcDrv_StsReg1_RESET;				/* �X�e�[�^�X���W�X�^1�����Z�b�g */
					gvIicSndEndFlg = imOFF;										/* ���M�����t���OOFF */
					R_IIC00_Master_Send( imRtcDrv_StsReg1WriteCmd, &wkStsReg1, 1U );
//					wkRet = SubRtcDrv_SndFlgChk();								/* ���M�t���O�Ď� */
					wkRet = SubRtcDrv_FlgChk( &gvIicSndEndFlg );
					/* �X�e�[�^�X���W�X�^1�Ǐo�� */
					gvIicRcvEndFlg = imOFF;										/* ���M�����t���OOFF */
					R_IIC00_Master_Receive( imRtcDrv_StsReg1ReadCmd, &wkStsReg1, 1U );
//					wkRet = SubRtcDrv_RcvFlgChk();								/* ��M�t���O�Ď� */
					wkRet = SubRtcDrv_FlgChk( &gvIicRcvEndFlg );
					wkRetryCnt++;												/* ���g���C�J�E���^������ */
					if( wkRetryCnt > imRtcDrv_RetryCnt )
					{
						wkRet = ecERR_NG;										/* RTC�ُ� */
						break;
					}
				} while( M_TSTBIT( wkStsReg1, imRtcDrv_StsReg1_POC ) || M_TSTBIT( wkStsReg1, imRtcDrv_StsReg1_BLD ) );
				
				/* RTC���� */
				if( wkRet == ecERR_OK )
				{
					wkRetryCnt = 0U;											/* ���g���C�J�E���^������ */
					/* 24���ԕ\���ݒ� */
					do
					{
						/* �X�e�[�^�X���W�X�^1�������݁i24���ԕ\���j */
						wkStsReg1 = (uint8_t)imRtcDrv_StsReg1_12_24;			/* �X�e�[�^�X���W�X�^1��24���ԕ\�� */
						gvIicSndEndFlg = imOFF;									/* ���M�����t���OOFF */
						R_IIC00_Master_Send( imRtcDrv_StsReg1WriteCmd, &wkStsReg1, 1U );
//						wkRet = SubRtcDrv_SndFlgChk();							/* ���M�t���O�Ď� */
						wkRet = SubRtcDrv_FlgChk( &gvIicSndEndFlg );
						/* �X�e�[�^�X���W�X�^1�Ǐo�� */
						gvIicRcvEndFlg = imOFF;									/* ���M�����t���OOFF */
						R_IIC00_Master_Receive( imRtcDrv_StsReg1ReadCmd, &wkStsReg1, 1U );
//						wkRet = SubRtcDrv_RcvFlgChk();							/* ��M�t���O�Ď� */
						wkRet = SubRtcDrv_FlgChk( &gvIicRcvEndFlg );
						wkRetryCnt++;											/* ���g���C�J�E���^������ */
						if( wkRetryCnt > imRtcDrv_RetryCnt )
						{
							wkRet = ecERR_NG;									/* RTC�ُ� */
							break;
						}
					} while( !M_TSTBIT( wkStsReg1, imRtcDrv_StsReg1_12_24 ) );
				}
				
				/* RTC���� */
				if( wkRet == ecERR_OK )
				{
					wkRetryCnt = 0U;											/* ���g���C�J�E���^������ */
					/* ���A���^�C���f�[�^1�ݒ�i2020/01/01 00:00:00�j */
					wkRTC.mYear = 20U;											/* �N */
					wkRTC.mMonth = 1U;											/* �� */
					wkRTC.mDay = 1U;											/* �� */
					wkRTC.mWeek = 0U;											/* �j�� */
					wkRTC.mHour = 0U;											/* �� */
					wkRTC.mMin = 0U;											/* �� */
					wkRTC.mSec = 0U;											/* �b */
					do
					{
						/* ���A���^�C���f�[�^1�������� */
						wkRet = SubRtcDrv_SetRtc( &wkRTC );						/* RTC(S-35390A) �����ݒ菈�� */
						if( wkRet == ecERR_OK )
						{
							/* ���A���^�C���f�[�^1�Ǐo�� */
							wkRet = SubRtcDrv_ReadRtc();						/* RTC(S-35390A) �����ǂݏo������ */
						}
						
						wkRetryCnt++;											/* ���g���C�J�E���^������ */
						if( wkRetryCnt > imRtcDrv_RetryCnt )
						{
							wkRet = ecERR_NG;									/* RTC�ُ� */
							break;
						}
					} while( wkRet != ecERR_OK );
					
					/* �O�t��RTC�t�@�[�X�g���C�g */
					if( gvInFlash.mProcess.mRtcFirstWrChk != imRtcWrFirstVal )
					{
						gvInFlash.mProcess.mRtcFirstWrChk = imRtcWrFirstVal;
						gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
					}
					else
					{
						/* �O�t��RTC�t�@�[�X�g�łȂ��̂ɁA���W�X�^���������s���ꂽ */
						gvDispMode.mNowDisp = ecDispMode_UpErrDwRtc;
					}
				}
				
				vRtcDrvPrm.mInitSts = ecRtcDrvInitSts_InitEnd;					/* �������X�e�[�^�X������������ */
			}
			break;
			
		/* 24���ԊǗ��ݒ� */
		case ecRtcDrvInitSts_24Write:
			wkRetryCnt = 0U;													/* ���g���C�J�E���^������ */
			/* 24���ԕ\���ݒ� */
			do
			{
				/* �X�e�[�^�X���W�X�^1�������݁i24���ԕ\���j */
				wkStsReg1 = (uint8_t)imRtcDrv_StsReg1_12_24;					/* �X�e�[�^�X���W�X�^1��24���ԕ\�� */
				gvIicSndEndFlg = imOFF;											/* ���M�����t���OOFF */
				R_IIC00_Master_Send( imRtcDrv_StsReg1WriteCmd, &wkStsReg1, 1U );
//				wkRet = SubRtcDrv_SndFlgChk();									/* ���M�t���O�Ď� */
				wkRet = SubRtcDrv_FlgChk( &gvIicSndEndFlg );
				/* �X�e�[�^�X���W�X�^1�Ǐo�� */
				gvIicRcvEndFlg = imOFF;											/* ���M�����t���OOFF */
				R_IIC00_Master_Receive( imRtcDrv_StsReg1ReadCmd, &wkStsReg1, 1U );
//				wkRet = SubRtcDrv_RcvFlgChk();									/* ��M�t���O�Ď� */
				wkRet = SubRtcDrv_FlgChk( &gvIicRcvEndFlg );
				wkRetryCnt++;													/* ���g���C�J�E���^������ */
				if( wkRetryCnt > imRtcDrv_RetryCnt )
				{
					wkRet = ecERR_NG;											/* RTC�ُ� */
					break;
				}
			} while( !M_TSTBIT( wkStsReg1, imRtcDrv_StsReg1_12_24 ) );
			
			vRtcDrvPrm.mInitSts = ecRtcDrvInitSts_RealRead;
			break;
			
		/* ���A���^�C���f�[�^�ǂݏo�� */
		case ecRtcDrvInitSts_RealRead:
			wkRetryCnt = 0U;													/* ���g���C�J�E���^������ */
			do
			{
				/* ���A���^�C���f�[�^1�Ǐo�� */
				wkRet = SubRtcDrv_ReadRtc();										/* RTC(S-35390A) �����ǂݏo������ */
				
				wkRetryCnt++;													/* ���g���C�J�E���^������ */
				if( wkRetryCnt > imRtcDrv_RetryCnt )
				{
					break;
				}
			} while( wkRet != ecERR_OK );
			
			vRtcDrvPrm.mInitSts = ecRtcDrvInitSts_InitEnd;						/* �������X�e�[�^�X������������ */
			break;
			
		/* ���������� */
		case ecRtcDrvInitSts_InitEnd:
		default:
			break;
	}
	
	if( vRtcDrvPrm.mInitSts == ecRtcDrvInitSts_InitEnd )
	{
		/* 2021�N�ȉ��Ȃ�RTC�G���[ */
		if( gvClock.mYear < 22U )
		{
			wkRet = ecERR_NG;													/* RTC�ُ� */
		}
	}
	
	if( wkRet != ecERR_OK )
	{
		/* �O�t��RTC���s:Err�\�����A���K�[���삳���Ȃ� */
		gvDispMode.mNowDisp = ecDispMode_UpErrDwRtc;
	}
}


/*
 *******************************************************************************
 *	RTC(S-35390A) �����ݒ菈��
 *
 *	[���e]
 *		�����̐ݒ���s���B
 *	[���L]
 *		���A���^�C���f�[�^1�̊e�f�[�^(�N,��,��,�j��,��,��,�b)��BCD�R�[�h�ŁA
 *		LSB�t�@�[�X�g�ő��M����B
 *******************************************************************************
 */
static ET_Error_t SubRtcDrv_SetRtc( const ST_RTC_t *parRtc )
{
	union
	{
		uint8_t		umU8[8];
		uint32_t	umU32[2];
	} wkSndData;
	
	uint8_t		wkStsReg1;
	
	
	ET_Error_t	wkRet = ecERR_OK;
	
	if( parRtc != M_NULL )
	{
		/* �X�e�[�^�X���W�X�^1�Ǐo�� */
		gvIicRcvEndFlg = imOFF;														/* ���M�����t���OOFF */
		R_IIC00_Master_Receive( imRtcDrv_StsReg1ReadCmd, &wkStsReg1, 1U );
//		wkRet = SubRtcDrv_RcvFlgChk();												/* ��M�t���O�Ď� */
		wkRet = SubRtcDrv_FlgChk( &gvIicRcvEndFlg );
		if( !M_TSTBIT( wkStsReg1, imRtcDrv_StsReg1_12_24 ) )						/* ����:12/24 */
		{
			/* �X�e�[�^�X���W�X�^1�������݁i24���ԕ\���j */
			wkStsReg1 = (uint8_t)imRtcDrv_StsReg1_12_24;							/* �X�e�[�^�X���W�X�^1��24���ԕ\�� */
			gvIicSndEndFlg = imOFF;													/* ���M�����t���OOFF */
			R_IIC00_Master_Send( imRtcDrv_StsReg1WriteCmd, &wkStsReg1, 1U );
//			wkRet = SubRtcDrv_SndFlgChk();											/* ���M�t���O�Ď� */
			wkRet = SubRtcDrv_FlgChk( &gvIicSndEndFlg );
		}
		
		/* RTC���� */
		if( wkRet == ecERR_OK )
		{
			/* Bin��BCD�f�[�^�ϊ� */
			wkSndData.umU8[3] = SubRtcDrv_BinToBcd( parRtc->mYear );				/* �N(BCD�f�[�^) */
			wkSndData.umU8[2] = SubRtcDrv_BinToBcd( parRtc->mMonth );				/* ��(BCD�f�[�^) */
			wkSndData.umU8[1] = SubRtcDrv_BinToBcd( parRtc->mDay );					/* ��(BCD�f�[�^) */
			wkSndData.umU8[0] = SubRtcDrv_BinToBcd( parRtc->mWeek );				/* �j��(BCD�f�[�^) */
			
			wkSndData.umU8[7] = SubRtcDrv_BinToBcd( parRtc->mHour );				/* ��(BCD�f�[�^) */
			wkSndData.umU8[6] = SubRtcDrv_BinToBcd( parRtc->mMin );					/* ��(BCD�f�[�^) */
			wkSndData.umU8[5] = SubRtcDrv_BinToBcd( parRtc->mSec );					/* �b(BCD�f�[�^) */
			wkSndData.umU8[4] = 0x00U;												/* �_�~�[(���g�p) */
			
			wkSndData.umU32[0] = SubRtcDrv_ReverseBit32( wkSndData.umU32[0] );		/* MSB�t�@�[�X�g��LSB�t�@�[�X�g�ϊ� */
			wkSndData.umU32[1] = SubRtcDrv_ReverseBit32( wkSndData.umU32[1] );		/* MSB�t�@�[�X�g��LSB�t�@�[�X�g�ϊ� */
			
			/* ���A���^�C���f�[�^1�������� */
			gvIicSndEndFlg = imOFF;													/* ���M�����t���OOFF */
			R_IIC00_Master_Send(imRtcDrv_RealTime1WriteCmd, &wkSndData.umU8[0], 7U);
//			wkRet = SubRtcDrv_SndFlgChk();											/* ���M�t���O�Ď� */
			wkRet = SubRtcDrv_FlgChk( &gvIicSndEndFlg );
		}
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	RTC(S-35390A) �����ǂݏo������
 *
 *	[���e]
 *		RTC���ُ�ȏꍇ�́AecERR_NG��Ԃ��B
 *******************************************************************************
 */
static ET_Error_t SubRtcDrv_ReadRtc( void )
{
	union
	{
		uint8_t		umU8[8];
		uint32_t	umU32[2];
	} wkRcvData;																/* RTC��M�f�[�^ */
	
	ET_Error_t				wkRet = ecERR_OK;									/* �߂�l(�G���[���) */
	
	/* ���A���^�C���f�[�^1�Ǐo�� */
	gvIicRcvEndFlg = imOFF;														/* ���M�����t���OOFF */
	R_IIC00_Master_Receive(imRtcDrv_RealTime1ReadCmd, &wkRcvData.umU8[0], 7U);
	
//	wkRet = SubRtcDrv_RcvFlgChk();												/* ��M�t���O�Ď� */
	wkRet = SubRtcDrv_FlgChk( &gvIicRcvEndFlg );
	if( wkRet == ecERR_OK )														/* RTC���펞 */
	{
		wkRcvData.umU32[0] = SubRtcDrv_ReverseBit32( wkRcvData.umU32[0] );		/* LSB�t�@�[�X�g��MSB�t�@�[�X�g�ϊ� */
		wkRcvData.umU32[1] = SubRtcDrv_ReverseBit32( wkRcvData.umU32[1] );		/* LSB�t�@�[�X�g��MSB�t�@�[�X�g�ϊ� */
		
		/* BCD��Bin�f�[�^�ϊ� */
		gvClock.mYear = SubRtcDrv_BcdToBin( wkRcvData.umU8[3] );				/* �N */
		gvClock.mMonth = SubRtcDrv_BcdToBin( wkRcvData.umU8[2] );				/* �� */
		gvClock.mDay = SubRtcDrv_BcdToBin( wkRcvData.umU8[1] );					/* �� */
		gvClock.mWeek = SubRtcDrv_BcdToBin( wkRcvData.umU8[0] );				/* �j�� */
		gvClock.mHour = SubRtcDrv_BcdToBin( (wkRcvData.umU8[7] & 0x3FU) );		/* �� */
		gvClock.mMin = SubRtcDrv_BcdToBin( wkRcvData.umU8[6] );					/* �� */
		gvClock.mSec = SubRtcDrv_BcdToBin( wkRcvData.umU8[5] );					/* �b */
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	Bin��BCD�f�[�^�ϊ�
 *
 *	[���e]
 *		Bin��BCD�f�[�^�ϊ����s���B
 *******************************************************************************
 */
static uint8_t SubRtcDrv_BinToBcd( uint8_t arBin )
{
	uint8_t		wkU8_H;															/* uint8_t�^���[�NH */
	uint8_t		wkU8_L;															/* uint8_t�^���[�NL */
	uint8_t		wkBcd;															/* BCD�f�[�^ */

	wkU8_H = (arBin / 10U) << 4U;
	wkU8_L = arBin % 10U;

	wkBcd = wkU8_H + wkU8_L;

	return wkBcd;
}


/*
 *******************************************************************************
 *	BCD��Bin�f�[�^�ϊ�
 *
 *	[���e]
 *		BCD��Bin�f�[�^�ϊ����s���B
 *******************************************************************************
 */
static uint8_t SubRtcDrv_BcdToBin( uint8_t arBcd )
{
	uint8_t		wkU8_H;															/* uint8_t�^���[�NH */
	uint8_t		wkU8_L;															/* uint8_t�^���[�NL */
	uint8_t		wkBin;															/* Bin�f�[�^ */

	wkU8_H = (arBcd >> 4U) * 10U;
	wkU8_L = arBcd & 0x0FU;

	wkBin = wkU8_H + wkU8_L;

	return wkBin;
}


/*
 *******************************************************************************
 *	�r�b�g�����בւ�(32bit)
 *
 *	[���e]
 *		�r�b�g�̕��я����t�ɂ���B�iMSB��LSB���t�ɂ���j
 *******************************************************************************
 */
static uint32_t	SubRtcDrv_ReverseBit32( uint32_t arData )
{
	uint32_t	wkRet;															/* �߂�l */
	uint8_t		wkLoop;															/* ���[�v�J�E���^ */
	uint8_t		wkShift;														/* �V�t�g�� */
	uint32_t	wkMask;															/* �}�X�N�f�[�^ */
	
	wkRet = 0U;
	for (wkLoop = 0U; wkLoop < 16U; wkLoop++)
	{
		wkShift = 31U - (wkLoop * 2U);											/* wkShift��31,29,27,�E�E�E,7,5,3,1 */
		
		/*
		 ***************************************
		 *	bit0�`15��bit31�`16�ɓ���ւ�
		 ***************************************
		 */
		wkMask = (uint32_t)M_BIT0 << wkLoop;
		wkRet |= (arData & wkMask) << wkShift;
		
		/*
		 ***************************************
		 *	bit31�`16��bit0�`15�ɓ���ւ�
		 ***************************************
		 */
		wkMask = (uint32_t)M_BIT31 >> wkLoop;
		wkRet |= (arData & wkMask) >> wkShift;
	}
	
	return wkRet;
}

#if 0
/*
 *******************************************************************************
 *	RTC(S-35390A) 1Hz�o�͏���
 *
 *	[����]
 *		uint8_t	arSelect:�o�͂�ON/OFF�ؑ�
 *	[�ߒl]
 *		ET_Error_t wkRet:���䌋��
 *	[���e]
 *		1Hz�̏o�͂�INT2�[�q���s���B
 *******************************************************************************
 */
void ApiRtcDrv_1HzOut( uint8_t arSelect )
{
	uint8_t		wkStsReg2;
	uint8_t		wkInt2Reg;
	ET_Error_t	wkRet = ecERR_OK;												/* �߂�l(�G���[���) */
	
	/* �X�e�[�^�X���W�X�^2�Ǐo������ */
	gvIicRcvEndFlg = imOFF;														/* ���M�����t���OOFF */
	R_IIC00_Master_Receive(imRtcDrv_StsReg2ReadCmd, &wkStsReg2, 1U);
//	wkRet = SubRtcDrv_RcvFlgChk();												/* ��M�t���O�Ď� */
	wkRet = SubRtcDrv_FlgChk( &gvIicRcvEndFlg );
	if( wkRet == ecERR_OK )														/* RTC���� */
	{
		if( arSelect == 1U )
		{
			M_CLRBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_ME );					/* INT2ME��0 */
			M_SETBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_FE );					/* INT2FE��1 */
		}
		else
		{
			M_CLRBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_ME );					/* INT2ME��0 */
			M_CLRBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_FE );					/* INT2FE��0 */
		}
		
		/* �X�e�[�^�X���W�X�^2�������ݏ��� */
		gvIicSndEndFlg = imOFF;													/* ���M�����t���OOFF */
		R_IIC00_Master_Send(imRtcDrv_StsReg2WriteCmd, &wkStsReg2, 1U);
//		wkRet = SubRtcDrv_SndFlgChk();											/* ���M�t���O�Ď� */
		wkRet = SubRtcDrv_FlgChk( &gvIicSndEndFlg );
	}
	
	if( wkRet == ecERR_OK )														/* RTC���� */
	{
		/* �X�e�[�^�X���W�X�^2�Ǐo������ */
		gvIicRcvEndFlg = imOFF;													/* ���M�����t���OOFF */
		R_IIC00_Master_Receive(imRtcDrv_StsReg2ReadCmd, &wkStsReg2, 1U);
//		wkRet = SubRtcDrv_RcvFlgChk();											/* ��M�t���O�Ď� */
		wkRet = SubRtcDrv_FlgChk( &gvIicRcvEndFlg );
		if( arSelect == 1U )
		{
			if( M_TSTBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_ME ) || !M_TSTBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_FE ) )
			{
				wkRet = ecERR_NG;
			}
		}
		else
		{
			if( M_TSTBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_ME ) || M_TSTBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_FE ) )
			{
				wkRet = ecERR_NG;
			}
		}
	}
	
	if( wkRet == ecERR_OK )														/* RTC���� */
	{
		gvIicSndEndFlg = imOFF;													/* ���M�����t���OOFF */
		wkInt2Reg = imRtcDrv_Int2Reg_1Hz;										/* INT2���W�X�^ 1Hz�r�b�g�ʒu */
		R_IIC00_Master_Send(imRtcDrv_Int2RegWriteCmd, &wkInt2Reg, 1U);
//		wkRet = SubRtcDrv_SndFlgChk();											/* ���M�t���O�Ď� */
		wkRet = SubRtcDrv_FlgChk( &gvIicSndEndFlg );
	}
	
	if( wkRet == ecERR_OK )														/* RTC���� */
	{
		/* INTP2���W�X�^�Ǐo������ */
		gvIicRcvEndFlg = imOFF;													/* ���M�����t���OOFF */
		R_IIC00_Master_Receive(imRtcDrv_Int2RegReadCmd, &wkInt2Reg, 1U);
//		wkRet = SubRtcDrv_RcvFlgChk();											/* ��M�t���O�Ď� */
		wkRet = SubRtcDrv_FlgChk( &gvIicRcvEndFlg );
		if( wkInt2Reg != imRtcDrv_Int2Reg_1Hz )
		{
			wkRet = ecERR_NG;
		}
	}
}
#endif

#if (swKouteiMode == imEnable)
/*
 *******************************************************************************
 *	RTC(S-35390A) 1Hz�o�͐����ԃ��[�h
 *
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		uint8_t wkRet: �o�͐�����
 *	[���e]
 *		RTC(S-35390A) 1Hz�o�͐����ԃ��[�h
 *******************************************************************************
 */
uint8_t ApiRtcDrv_1HzOutRead( void )
{
	uint8_t		wkStsReg2;
	uint8_t		wkRet;
	
	/* �X�e�[�^�X���W�X�^2�Ǐo������ */
	gvIicRcvEndFlg = imOFF;													/* ���M�����t���OOFF */
	R_IIC00_Master_Receive(imRtcDrv_StsReg2ReadCmd, &wkStsReg2, 1U);
//	wkRet = SubRtcDrv_RcvFlgChk();											/* ��M�t���O�Ď� */
	wkRet = SubRtcDrv_FlgChk( &gvIicRcvEndFlg );
	if( !M_TSTBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_ME ) && M_TSTBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_FE ) )
	{
		wkRet = 1U;
	}
	else if( !M_TSTBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_ME ) && !M_TSTBIT( wkStsReg2, imRtcDrv_StsReg2_INT2_FE ) )
	{
		wkRet = 0U;
	}
	else
	{
		wkRet = 2U;
	}
	return wkRet;
}
#endif

/*
 *******************************************************************************
 *	�O�t��RTC�ʐM��M�t���O�Ď�
 *
 *	[���e]
 *		�O�t��RTC�Ƃ̒ʐM���̎�M�t���O�Ď�
 *	[����]
 *		�Ȃ�
 *	[�߂�l]
 *		ET_Error_t	wkRet: �G���[���
 *******************************************************************************
 */
static ET_Error_t SubRtcDrv_FlgChk( uint8_t* gvFlg )
{
	uint16_t	wkErrJdgCnt = 0U;
	ET_Error_t	wkRet = ecERR_OK;
	
	while( *gvFlg == imOFF )										/* ���M�����t���O��ON�ɂȂ�܂� */
	{
		wkErrJdgCnt++;
		if( wkErrJdgCnt > imErrJdgTimer )									/* 1ms���߂ŊO�t��RTC�ւ̃A�N�Z�X���s */
		{
			wkRet = ecERR_NG;												/* RTC�ُ� */
			break;
		}
	}
	return wkRet;
}
#if 0
/*
 *******************************************************************************
 *	�O�t��RTC�ʐM��M�t���O�Ď�
 *
 *	[���e]
 *		�O�t��RTC�Ƃ̒ʐM���̎�M�t���O�Ď�
 *	[����]
 *		�Ȃ�
 *	[�߂�l]
 *		ET_Error_t	wkRet: �G���[���
 *******************************************************************************
 */
static ET_Error_t SubRtcDrv_RcvFlgChk( void )
{
	uint16_t	wkErrJdgCnt = 0U;
	ET_Error_t	wkRet = ecERR_OK;
	
	while( gvIicRcvEndFlg == imOFF )										/* ���M�����t���O��ON�ɂȂ�܂� */
	{
		wkErrJdgCnt++;
		if( wkErrJdgCnt > imErrJdgTimer )									/* 1ms���߂ŊO�t��RTC�ւ̃A�N�Z�X���s */
		{
			wkRet = ecERR_NG;												/* RTC�ُ� */
			break;
		}
	}
	return wkRet;
}

/*
 *******************************************************************************
 *	�O�t��RTC�ʐM���M�t���O�Ď�
 *
 *	[���e]
 *		�O�t��RTC�Ƃ̒ʐM���̑��M�t���O�Ď�
 *	[����]
 *		�Ȃ�
 *	[�߂�l]
 *		ET_Error_t	wkRet: �G���[���
 *******************************************************************************
 */
static ET_Error_t SubRtcDrv_SndFlgChk( void )
{
	uint16_t	wkErrJdgCnt = 0U;
	ET_Error_t	wkRet = ecERR_OK;
	
	while( gvIicSndEndFlg == imOFF )										/* ���M�����t���O��ON�ɂȂ�܂� */
	{
		wkErrJdgCnt++;
		if( wkErrJdgCnt > imErrJdgTimer )									/* 1ms���߂ŊO�t��RTC�ւ̃A�N�Z�X���s */
		{
			wkRet = ecERR_NG;												/* RTC�ُ� */
			break;
		}
	}
	return wkRet;
}
#endif

/*
 *******************************************************************************
 *	����RTC�̎������O�t��RTC�ɏ������ޏ���
 *
 *	[���e]
 *		����RTC�̎������O�t��RTC�ɏ������ށB
 *******************************************************************************
 */
void ApiRtcDrv_InRtcToExRtc( void )
{
	rtc_counter_value_t	wkInRTC;												/* ����RTC�f�[�^ */
	ST_RTC_t			wkExRTC;												/* �O�t��RTC�f�[�^ */
	uint8_t				wkLoop;
	uint8_t				wkLoopA;
	
	for( wkLoop = 0U ; wkLoop < imInRtcJdgCnt ; wkLoop++ )
	{
		/* ����RTC�̎����擾 */
		if( MD_OK == R_RTC_Get_CounterValue( &wkInRTC ) )
		{
			/* ����RTC�̎�����BCD��BIN�ϊ� */
			wkExRTC.mYear = SubRtcDrv_BcdToBin( wkInRTC.year );						/* �N(BCD�f�[�^) */
			wkExRTC.mMonth = SubRtcDrv_BcdToBin( wkInRTC.month );					/* ��(BCD�f�[�^) */
			wkExRTC.mDay = SubRtcDrv_BcdToBin( wkInRTC.day );						/* ��(BCD�f�[�^) */
			wkExRTC.mWeek = SubRtcDrv_BcdToBin( wkInRTC.week );						/* �j(BCD�f�[�^) */
			wkExRTC.mHour = SubRtcDrv_BcdToBin( wkInRTC.hour );						/* ��(BCD�f�[�^) */
			wkExRTC.mMin = SubRtcDrv_BcdToBin( wkInRTC.min );						/* ��(BCD�f�[�^) */
			wkExRTC.mSec = SubRtcDrv_BcdToBin( wkInRTC.sec );						/* �b(BCD�f�[�^) */
			
			for( wkLoopA = 0U ; wkLoopA < imExRtcJdgCnt ; wkLoopA++ )
			{
				/* ����RTC�̎������O�t��RTC�ɏ������� */
				if( ecERR_OK == SubRtcDrv_SetRtc( &wkExRTC ) )
				{
#if 0	/* �@��ُ한�A�����Ȃ� */
					/* �O�t��RTC���A */
					ApiAbn_AbnStsClr( imAbnSts_RTC, ecAbnInfKind_AbnSts );
#endif
					break;
				}
			}
			
			if( wkLoopA >= imExRtcJdgCnt )
			{
				/* �O�t��RTC�ُ� */
				ApiAbn_AbnStsSet( imAbnSts_RTC, ecAbnInfKind_AbnSts );
			}
			break;
		}
	}
	
	SubRtcDrv_InRtcErrSet( wkLoop );		/* ����RTC�ُ픻�� */
}


/*
 *******************************************************************************
 *	�O�t��RTC�̎��������RTC�ɏ������ޏ���
 *
 *	[���e]
 *		�O�t��RTC�̎��������RTC�ɏ������ށB
 *******************************************************************************
 */
void ApiRtcDrv_ExRtcToInRtc( void )
{
	rtc_counter_value_t	wkInRTC;					/* ����RTC�f�[�^ */
	uint8_t				wkLoop;
	uint8_t				wkLoopA;
	
	for( wkLoopA = 0U ; wkLoopA < imExRtcJdgCnt ; wkLoopA++ )
	{
		/* �O�t��RTC�̎����擾 */
		if( ecERR_OK == SubRtcDrv_ReadRtc() )
		{
			/* ����RTC�p�p�����[�^�ɒu������ */
			wkInRTC.year = SubRtcDrv_BinToBcd(gvClock.mYear);
			wkInRTC.month = SubRtcDrv_BinToBcd(gvClock.mMonth);
			wkInRTC.day = SubRtcDrv_BinToBcd(gvClock.mDay);
			wkInRTC.week = SubRtcDrv_BinToBcd(gvClock.mWeek);
			wkInRTC.hour = SubRtcDrv_BinToBcd(gvClock.mHour);
			wkInRTC.min = SubRtcDrv_BinToBcd(gvClock.mMin);
			wkInRTC.sec = SubRtcDrv_BinToBcd(gvClock.mSec);
			
			for( wkLoop = 0U ; wkLoop < imInRtcJdgCnt ; wkLoop++ )
			{
				/* �O�t��RTC�̎��������RTC�ɏ������� */
				if( MD_OK == R_RTC_Set_CounterValue( wkInRTC ) )
				{
					break;
				}
			}
			SubRtcDrv_InRtcErrSet( wkLoop );		/* ����RTC�ُ픻�� */
			break;
		}
	}
	
	if( wkLoopA >= imExRtcJdgCnt )
	{
		/* �O�t��RTC���[�h���s:Err�\�����A���K�[���삳���Ȃ� */
		gvDispMode.mNowDisp = ecDispMode_UpErrDwRtc;
	}
}

/*
 *******************************************************************************
 *	����RTC�̎��������[�h����RAM�Ɋi�[���鏈��
 *
 *	[���e]
 *		����RTC�̎��������[�h����RAM�Ɋi�[����B
 *******************************************************************************
 */
void ApiRtcDrv_ReadInRtc( void )
{
	rtc_counter_value_t	wkInRTC;												/* ����RTC�f�[�^ */
	uint8_t				wkLoop;
	
	for( wkLoop = 0U ; wkLoop < imInRtcJdgCnt ; wkLoop++ )
	{
		/* ����RTC�̎����擾 */
		if( MD_OK == R_RTC_Get_CounterValue( &wkInRTC ) )
		{
			/* ����RTC�̎�����BCD��BIN�ϊ� */
			gvClock.mYear = SubRtcDrv_BcdToBin( wkInRTC.year );					/* �N(BCD�f�[�^) */
			gvClock.mMonth = SubRtcDrv_BcdToBin( wkInRTC.month );				/* ��(BCD�f�[�^) */
			gvClock.mDay = SubRtcDrv_BcdToBin( wkInRTC.day );					/* ��(BCD�f�[�^) */
			gvClock.mWeek = SubRtcDrv_BcdToBin( wkInRTC.week );					/* �j(BCD�f�[�^) */
			gvClock.mHour = SubRtcDrv_BcdToBin( wkInRTC.hour );					/* ��(BCD�f�[�^) */
			gvClock.mMin = SubRtcDrv_BcdToBin( wkInRTC.min );					/* ��(BCD�f�[�^) */
			gvClock.mSec = SubRtcDrv_BcdToBin( wkInRTC.sec );					/* �b(BCD�f�[�^) */
			break;
		}
	}
	
	SubRtcDrv_InRtcErrSet( wkLoop );		/* ����RTC�ُ픻�� */
}


/*
 *******************************************************************************
 *	����RTC�Ɏ��������C�g���鏈��
 *
 *	[���e]
 *		����RTC�Ɏ��������C�g����B
 *******************************************************************************
 */
void ApiRtcDrv_SetInRtc( ST_RTC_t arClock )
{
	rtc_counter_value_t	wkInRTC;
	uint8_t				wkLoop;
	
	/* ����RTC�p�p�����[�^�ɒu������ */
	wkInRTC.year = SubRtcDrv_BinToBcd( arClock.mYear );
	wkInRTC.month = SubRtcDrv_BinToBcd( arClock.mMonth );
	wkInRTC.day = SubRtcDrv_BinToBcd( arClock.mDay );
	wkInRTC.week = SubRtcDrv_BinToBcd( arClock.mWeek );
	wkInRTC.hour = SubRtcDrv_BinToBcd( arClock.mHour );
	wkInRTC.min = SubRtcDrv_BinToBcd( arClock.mMin );
	wkInRTC.sec = SubRtcDrv_BinToBcd( arClock.mSec );
	
	for( wkLoop = 0U ; wkLoop < imInRtcJdgCnt ; wkLoop++ )
	{
		/* ����RTC�ɏ������� */
		if( MD_OK == R_RTC_Set_CounterValue( wkInRTC ) )
		{
			break;
		}
	}
	
	SubRtcDrv_InRtcErrSet( wkLoop );		/* ����RTC�ُ픻�� */
}


/*
 *******************************************************************************
 *	����RTC�ُ픻��
 *
 *	[���e]
 *		���������Ƃɓ���RTC�ُ̈픻�肵�ĕ��A�A�ُ���Z�b�g����B
 *******************************************************************************
 */
static void SubRtcDrv_InRtcErrSet( uint8_t arLoop )
{
#if 0	/* �@��ُ한�A�����Ȃ� */
	if( arLoop < imInRtcJdgCnt )
	{
		/* ����RTC���A */
		ApiAbn_AbnStsClr( imAbnSts_INRTC, ecAbnInfKind_AbnSts );
	}
	else
	{
		/* ����RTC�ُ� */
		ApiAbn_AbnStsSet( imAbnSts_INRTC, ecAbnInfKind_AbnSts );
		}
#else
	if( arLoop >= imInRtcJdgCnt )
	{
		/* ����RTC�ُ� */
		ApiAbn_AbnStsSet( imAbnSts_INRTC, ecAbnInfKind_AbnSts );
	}
#endif
}


/*
 *******************************************************************************
 * �o�ߕb���Z�o
 *
 *	[���e]
 *		2020/1/1 00:00:00������������܂ł̌o�ߕb�����Z�o����B
 *	[����]
 *		ST_RTC_t arClock : �����f�[�^
 *	[�ߒl]
 *		uint32_t wkTotalSec : 2020/1/1 00:00:00����̌o�ߕb��
 *******************************************************************************
 */
uint32_t ApiRtcDrv_mktime( ST_RTC_t arClock )
{
	uint16_t	wkYear;
	uint8_t		wkMonth;
	uint8_t		wkDay;
	uint8_t		wkHour;
	uint8_t		wkMin;
	uint8_t		wkSec;
	uint8_t		wkLeap;
	uint16_t	wkLoop;
	sint16_t	wkDay1;
	uint16_t	wkDay2;
	uint32_t	wkU32;
	uint32_t	wkTotalSec;
	ST_RTC_t	wkRTC;
	
	
	wkRTC.mYear = arClock.mYear;
	wkRTC.mMonth = arClock.mMonth;
	wkRTC.mDay = arClock.mDay;
	wkRTC.mWeek = arClock.mWeek;
	wkRTC.mHour = arClock.mHour;
	wkRTC.mMin = arClock.mMin;
	wkRTC.mSec = arClock.mSec;
	
	wkDay1 = 0U;
	wkDay2 = 0U;
	
	/* �N�`�F�b�N */
	if( wkRTC.mYear < 20U )
	{
		wkYear = 20U;
	}
	else if( wkRTC.mYear > 99U )
	{
		wkYear = 99U;
	}
	else
	{
		wkYear = wkRTC.mYear;
	}
	
	/* ���`�F�b�N */
	if( wkRTC.mMonth == 0U )
	{
		wkMonth = 1U;
	}
	else if( wkRTC.mMonth > 12U )
	{
		wkMonth = 12U;
	}
	else
	{
		wkMonth = wkRTC.mMonth;
	}
	
	/* ���`�F�b�N */
	if( wkRTC.mDay == 0U )
	{
		wkDay = 1U;
	}
	else if( wkRTC.mDay > 31U )
	{
		wkDay = 31U;
	}
	else
	{
		wkDay = wkRTC.mDay;
	}
	
	/* ���`�F�b�N */
	if( wkRTC.mHour > 23U )
	{
		wkHour = 23U;
	}
	else
	{
		wkHour = wkRTC.mHour;
	}
	
	/* ���`�F�b�N */
	if( wkRTC.mMin > 59U )
	{
		wkMin = 59U;
	}
	else
	{
		wkMin = wkRTC.mMin;
	}
	
	/* �b�`�F�b�N */
	if( wkRTC.mSec > 59U )
	{
		wkSec = 59U;
	}
	else
	{
		wkSec = wkRTC.mSec;
	}
	
	/*
	 ***************************************
	 *	�w�肵���N�̑O�N�܂ł̓����Z�o
	 ***************************************
	 */
	wkYear += 2000U;
	if( wkYear > 2020U )
	{
		for( wkLoop = 2020U; wkLoop <= wkYear-1U; wkLoop++ )
		{
			wkDay1 += 365U;
			if( (!(wkLoop % 4U) && (wkLoop % 100U)) || !(wkLoop % 400U) )	/* ���邤�N */
			{
				wkDay1++;													/* ������1���� */
			}
			else															/* ���N */
			{
				;															/* ������ */
			}
		}
	}
	wkDay1 -= 1U;
	
	/*
	 ***************************************
	 *	�w�肵���N�����邤�N������
	 ***************************************
	 */
	if( (!(wkYear % 4U) && (wkYear % 100U)) || !(wkYear % 400U) )			/* ���邤�N */
	{
		wkLeap = 1U;
	}
	else																	/* ���N */
	{
		wkLeap = 0U;
	}
	
	/*
	 ***************************************
	 *	�w�肵�����̐挎�܂ł̓����Z�o
	 ***************************************
	 */
	for( wkLoop = 1U; wkLoop <= (wkMonth - 1U); wkLoop++ )
	{
		wkDay2 += cDayofMonth[wkLoop];										/* �����Ƃ̓����擾 */
		if( wkLoop == 2U )													/* 2�� */
		{
			wkDay2 += wkLeap;
		}
	}
	
	/*
	 ***************************************
	 *	�w�肵�����̑O���܂ł̓����Z�o
	 ***************************************
	 */
	wkDay2 += wkDay;
	wkDay1 += wkDay2;
	
	wkTotalSec = wkDay1;													/* �������擾 */
	wkTotalSec *= 86400U;													/* ����b�ɕϊ� */
	wkU32 = (uint32_t)wkHour * 3600U;										/* ����b�ɕϊ� */
	wkTotalSec += wkU32;
	wkU32 = (uint32_t)wkMin * 60U;											/* ����b�ɕϊ� */
	wkTotalSec += wkU32;
	wkTotalSec += wkSec;
	
	
	return wkTotalSec;
}


/*
 *******************************************************************************
 * �o�ߕb�����玞���f�[�^�֕ϊ�
 *
 *	[���e]
 *		2020/1/1 00:00:00����̌o�ߕb���������f�[�^�֕ϊ�����B
 *	[����]
 *		uint32_t arTotalSec : 2020/1/1 00:00:00����̌o�ߕb��
 *	[�ߒl]
 *		ST_RTC_t wkClock : �����f�[�^
 *******************************************************************************
 */
ST_RTC_t ApiRtcDrv_localtime( uint32_t arTotalSec )
{
	ST_RTC_t		wkClock;
	uint32_t		wkYear;
	uint32_t		wkMonth;
	uint32_t		wkDay;
	uint32_t		wkHour;
	uint32_t		wkMin;
	uint32_t		wkSec;
	uint32_t		wkU32;
	
	/* sec */
	wkSec = arTotalSec % 60U;
	
	/* min */
	arTotalSec -= wkSec;
	wkU32 = arTotalSec / 60U;
	wkMin = wkU32 % 60U;
	
	/* hour */
	wkU32 -= wkMin;
	wkU32 /= 60U;
	wkHour = wkU32 % 24U;
	
	/* day */
	wkU32 -= wkHour;
	wkDay = wkU32 / 24U;
	
	wkYear = 2020U;
	while( wkDay >= 365U )
	{
		if( (!(wkYear % 4U) && (wkYear % 100U)) || !(wkYear % 400U) )
		{
			if( wkDay == 365U )
			{
				break;
			}
			wkDay -= 366U;
		}
		else
		{
			wkDay -= 365U;
		}
		wkYear ++;
	}
	wkDay ++;
	
	wkMonth = 1U;
	while( 1U )
	{
		if( (!(wkYear % 4U) && (wkYear % 100U)) || !(wkYear % 400U) )
		{
			if( wkDay <= cDayofLeapMonth[ wkMonth ] )
			{
				break;
			}
			else
			{
				wkDay -= cDayofLeapMonth[ wkMonth ];
				wkMonth ++;
			}
		}
		else
		{
			if( wkDay <= cDayofMonth[ wkMonth ] )
			{
				break;
			}
			else
			{
				wkDay -= cDayofMonth[ wkMonth ];
				wkMonth ++;
			}
		}
	}
	wkYear -= 2000U;
	wkClock.mYear = (uint8_t)wkYear;
	wkClock.mMonth = (uint8_t)wkMonth;
	wkClock.mDay = (uint8_t)wkDay;
	wkClock.mHour = (uint8_t)wkHour;
	wkClock.mMin = (uint8_t)wkMin;
	wkClock.mSec = (uint8_t)wkSec;
	
	return wkClock;
}

#pragma section
