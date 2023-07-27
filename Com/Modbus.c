/*
 *******************************************************************************
 *	File name	:	Modbus.c
 *
 *	[���e]
 *		MODBUS�ʐM����
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2017.11.13		Softex N.I		�V�K�쐬
 *	2020.12.01		Softex K.U		�Z���T�ԒʐM�V�[�P���X�ɕύX
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
 *	�񋓌^��`
 *==============================================================================
 */
/* �ʐM������� */
typedef enum ET_ComType
{
	ecComType_Non = 0,															/* �Ȃ� */
	ecComType_Vid,																/* �ϐ� */
	ecComType_Ext,																/* �O���֐����� */

	ecComType_Max																/* enum�ő�l */
} ET_ComType_t;

typedef enum ET_ParamError
{
	ecParamError_OK = 0,														/* OK */
	ecParamError_RngNG,															/* �͈͊ONG */
	ecParamError_CodeSet,														/* �ݒ�s�� */
	
	ecParamErrorMax																/* enum�ő�l */
} ET_ParamError_t;

typedef enum ET_ModMeasErrChkKind
{
	ecModMeasErrChkKind_Burn = 0,												/* �o�[���A�E�g���� */
	ecModMeasErrChkKind_Under,													/* �A���_�[�����W���� */
	ecModMeasErrChkKind_Over,													/* �I�[�o�[�����W���� */
	ecModMeasErrChkKind_SnsErr,													/* �Z���T�@��ُ헚�� */
	ecModMeasErrChkKind_ComErr,													/* �Z���T�ԒʐM�G���[���� */

	ecModMeasErrChkKindMax														/* enum�ő�l */
} ET_ModMeasErrChkKind_t;


/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
*/
static void SubModbus_CrcMake( const uint8_t *parDatBuf, uint8_t *parCrcBuf, uint16_t arDatBufLen );
																				/* CRC�쐬 */


/*
 *==============================================================================
 *	define��`
 *==============================================================================
 */
#define imMod_ComFinTim			5U								/* 115200bps��230bit�ȏ�̊Ԋu�ő��M�I���̎d�l�Ŗ�2msec�����A�}�[�W���������5msec�Ƃ��� */
#define imAnaInCmdRecLen		7U								/* �A�i���O���̓f�[�^��M�f�[�^�� 7Byte */

#define	imRCV_SRV_ADR_POS		0								/* �X���[�u�A�h���X�ʒu */
#define	imRCV_FUNC_POS			1								/* �t�@���N�V�����R�[�h�ʒu */
#define	imRCV_DATA_NUM_POS		2								/* �f�[�^���ʒu */

/* �����x�f�[�^�ʒu */
#define	imRCV_TMPHUM_HUM_H_POS	3								/* �����x�F���x */
#define	imRCV_TMPHUM_HUM_L_POS	4
#define	imRCV_TMPHUM_TMP_H_POS	5								/* �����x�F���x */
#define	imRCV_TMPHUM_TMP_L_POS	6

/* �Z���T�f�[�^�ʒu */
#define	imRCV_MEAS_H_POS		3								/* 1CH�Z���T����l */
#define	imRCV_MEAS_L_POS		4

/* Pt�f�[�^�ʒu */
#define	imRCV_PT_TMP_H_POS		3								/* Pt���x */
#define	imRCV_PT_TMP_L_POS		4
#define imRCV_PT_CON_H_POS		5								/* Pt�ړ_��� */
#define imRCV_PT_CON_L_POS		6
#define imRCV_PT_DUMMY_H_POS	7								/* Pt���x�Z���O */
#define imRCV_PT_DUMMY_L_POS	8

#define imRCV_SENSOR_H_POS		9								/* �Z���T��ވʒu */
#define imRCV_SENSOR_L_POS		10

/* �d���f�[�^�ʒu */
#define	imRCV_Voltage_H_POS	3									/* �d�� */
#define	imRCV_Voltage_L_POS	4

/* �p���X�f�[�^�ʒu */
#define	imRCV_Pulse_H_POS 3										/* �p���X */
#define	imRCV_Pulse_L_POS 4

#define	imOK					0								/* OK */
#define	imNG					1								/* NG */

#define	imSensType_Max			5								/* Modbus�Z���T��̍ő�l */


/*
 *==============================================================================
 *	�\���̒�`
 *==============================================================================
 */
typedef struct ST_SensType
{
	ET_SensType_t	mSensorCode;
	uint8_t			( *mpSensorFunc )( void );
} ST_SensType_t;

/*
 *==============================================================================
 *	�ϐ���`
 *==============================================================================
 */


/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
 */
static void SubModbus_ErrChkTxtMake( uint8_t *parSndBuf, uint16_t *parSndLen, uint8_t *parSndBgnPtr );
																				/* �G���[�`�F�b�N�i�[����(CRC) */
static void SubModbus_CrcMake( const uint8_t *parInBuf, uint8_t *parOutBuf, uint16_t arLen );
																				/* CRC�Z�o���� */
static void SubModbus_Snd( void );												/* �R�}���h���M */
static uint8_t SubModbus_RcvSens( void );
//static uint8_t SubModbus_RcvPt( void );
static uint8_t SubModbus_RcvHumTmp( void );
static sint16_t SubModbus_Rcv1ChMeasParse( sint16_t arVal );
static sint16_t SubModbus_ErrValChg( sint16_t arVal );
//static uint8_t SubModbus_RcvDummy( void );
//static uint8_t SubModbus_RcvVol( void );										/* �d�� */
//static uint8_t SubModbus_RcvPulse( void );										/* �p���X */
static sint16_t SubModbus_Rcv8bitToS16bit( uint8_t arNo );
static void SubModbus_RcvAnalys( void );										/* �R�}���h��M��͏��� */
static void SubModbus_StateTrans( void );										/* �X�e�[�^�X��ԑJ�� */
static void SubModbus_MeasErrChk( void );										/* ����G���[���� */


/*
 *==============================================================================
 *	�萔��`
 *==============================================================================
 */
const ST_SensType_t cSensorType[ 6U ] =
{
	/* mSensorCode,			mpSensorFunc */						/* Modbus�Z���T��No. */
	{ ecSensType_HumTmp,	&SubModbus_RcvHumTmp },				/* 0�F�����x */
	{ ecSensType_Pt,		&SubModbus_RcvSens },				/* 1�F������R�� */
	{ ecSensType_V,			&SubModbus_RcvSens },				/* 2�F�d�� */
	{ ecSensType_Pulse,		&SubModbus_RcvSens },				/* 3�F�p���X */
	{ ecSensType_KTc,		&SubModbus_RcvSens },				/* 4�FK�M�d�� */
	{ ecSensType_TTc,		&SubModbus_RcvSens },				/* 5�FT�M�d�� */
};

#pragma section text MY_APP2
/*
 *******************************************************************************
 *	MODBUS�ʐM ����������
 *
 *	[���e]
 *		MODBUS�ʐM ����������
 *******************************************************************************
 */
void ApiModbus_Initial( void )
{
	uint16_t	wkLoop;											/* ���[�v�J�E���^ */
	
	for( wkLoop = 0U; wkLoop < imChannelNum ; wkLoop++ )
	{
		gvMeasPrm.mMeasVal[ wkLoop ] = imSndSnsErr;
		gvMeasPrm.mMeasError[ wkLoop ] = 0U;
	}
	
	for( wkLoop = 0U; wkLoop < M_ArrayElement(gvModInf.mSndBuf); wkLoop++ )
	{
		gvModInf.mSndBuf[ wkLoop ] = 0U;						/* ���M�o�b�t�@������ */
	}
	for( wkLoop = 0U; wkLoop < M_ArrayElement(gvModInf.mRcvBuf); wkLoop++ )
	{
		gvModInf.mRcvBuf[ wkLoop ] = 0U;						/* ��M�o�b�t�@������ */
	}
	gvModInf.mSndLen = 0U;										/* ���M�f�[�^�������� */
	gvModInf.mRcvLen = 0U;										/* ��M�f�[�^�������� */
	gvModInf.mRcvPos = 0U;										/* ��M�f�[�^�ʒu������ */
	gvModInf.mComSts = ecComSts_NullPre;						/* �ʐM��ԏ����� */
	gvModInf.mRcvTmCnt = 0U;									/* ��M�^�C�}�J�E���^ */
	gvModInf.mUnitPwrOnTmCnt = 0U;								/* ���j�b�g�N�����ԃ^�C�}�J�E���^ */
	gvModInf.mRetryCnt = 0U;									/* ���j�b�g�ʐM���g���C�J�E���^ */
	R_UART2_Start();											/* UART2�J�n */
}


/*
 *******************************************************************************
 *	MODBUS�ʐM���C��
 *
 *	[���e]
 *		MODBUS�ʐM�������s���B
 *******************************************************************************
 */
void ApiModbus_Main( void )
{
	uint32_t	wkLoop;

	switch( gvModInf.mComSts )
	{
		case ecComSts_NullPre:													/* NULL���M���� */
			M_CLRBIT(PM0, M_BIT3);												/* �����|�[�g���W�X�^�F�o�͂ɐݒ� */
			M_CLRBIT(P0, M_BIT3);												/* P0_3����0���o�� */
			for( wkLoop = 0U; wkLoop < 250U; wkLoop++)
			{
				M_NOP;
			}
			M_SETBIT(P0, M_BIT3);												/* P0_3����1���o�� */
			M_SETBIT(PM0, M_BIT3);												/* �����|�[�g���W�X�^�F���͂ɐݒ� */
			
			ApiComDrv_SwitchTx();												/* ���M�؂�ւ����� */
			gvModInf.mComSts = ecComSts_UnitPwnOnWait;							/* ���j�b�g�N���҂��ɏ�ԑJ�� */
			R_TAU0_Channel3_Custom(1875);										/* 20msec��Ƀ^�C�}���荞�� */
			gvModuleSts.mCom = ecComModuleSts_Wait;								/* �^�C�}�����ݑ҂���ԂɈڍs */
			break;
			
		case ecComSts_NullSnd:													/* NULL���M�� */
			;
			/*
			 ***************************************
			 *	NULL���M�����҂�
			 ***************************************
			 */
			break;
			
		case ecComSts_UnitPwnOnWait:											/* ���j�b�g�N���҂� */
			;
			/*
			 ***************************************
			 *	�^�C�}�[0������(1ms)��20ms���v��
			 ***************************************
			 */
			break;
			
		case ecComSts_SndPre:													/* �R�}���h���M���� */
			SubModbus_Snd();													/* �R�}���h���M���� */
			gvModInf.mComSts = ecComSts_Snd;									/* �R�}���h���M���ɏ�ԑJ�� */
			break;
			
		case ecComSts_Snd:														/* �R�}���h���M�� */
			;
			/*
			 ***************************************
			 *	UART2���M�����҂�
			 ***************************************
			 */
			break;
			
		case ecComSts_RcvWait:													/* �R�}���h��M�҂� */
			;
			/*
			 ***************************************
			 *	UART2�̎�M�����ݔ����܂őҋ@
			 *	�܂��́A1�b�o��(����������)�܂őҋ@
			 ***************************************
			 */
			break;
			
		case ecComSts_Rcv:														/* �R�}���h��M�� */
			;
			/*
			 ***************************************
			 *	�S�f�[�^�o�C�g��M�܂őҋ@
			 ***************************************
			 */
			break;
			
		case ecComSts_RcvAnalys:												/* �R�}���h��M��� */
			SubModbus_RcvAnalys();												/* �R�}���h��M��͏��� */
			break;
			
		case ecComSts_Init:
			/* ����l��u���� */
			if( gvDispMode.mNowDisp == ecDispMode_Up1CHDwClk )
			{
				ApiLcd_UppLow7SegDsp("   ", imHigh );
				ApiLcd_UppLow7SegDsp("   ", imLow );
				ApiLcd_Main();
			}
			gvModInf.mComSts = ecComSts_Lcdoff;									/* ����O��LED��u�����ɏ�ԑJ�� */
			R_TAU0_Channel7_Custom( 0xFFFF );									/* �^�C�}0(CH7)�J�n */
			gvModuleSts.mCom = ecComModuleSts_Wait;								/* �^�C�}�����ݑ҂���ԂɈڍs */
			break;
			
		/* ����O��LED��u�����̏��� */
		case ecComSts_Lcdoff:
			SubModbus_StateTrans();												/* �X�e�[�^�X��ԑJ�� */
			gvModInf.mComSts = ecComSts_NullPre;								/* NULL���M�����Ɉȍ~ */
			gvModuleSts.mCom = ecComModuleSts_Sleep;							/* �ʐM�X���[�v��ԂɈڍs */
			break;
			
		default:
			break;
	}
}


/*
 *******************************************************************************
 *	�G���[�`�F�b�N�i�[����(CRC)
 *
 *	[���e]
 *		�G���[�`�F�b�N�쐬���A�ʐM�d���Ɋi�[����B
 *******************************************************************************
 */
static void SubModbus_ErrChkTxtMake( uint8_t *parSndBuf, uint16_t *parSndLen, uint8_t *parSndBgnPtr )
{
	uint8_t wkCrcBuf[ 2 ];														/* CRC�p�o�b�t�@ */
	
	if( (parSndLen != M_NULL) && (parSndBgnPtr != M_NULL) )
	{
		*parSndLen = (uint16_t)(parSndBuf - parSndBgnPtr);						/* �f�[�^���擾 */
		
		SubModbus_CrcMake(parSndBgnPtr, wkCrcBuf, *parSndLen);					/* CRC�쐬 */
		*parSndBuf = wkCrcBuf[ 0 ];												/* CRC LO�i�[ */
		parSndBuf++;
		*parSndBuf = wkCrcBuf[ 1 ];												/* CRC HI�i�[ */
		parSndBuf++;
		
		*parSndLen += 2U;														/* ���M�o�b�t�@�T�C�Y�i�[ */
	}
}


/*
 *******************************************************************************
 *	CRC�Z�o����
 *
 *	[���e]
 *		CRC�Z�o���ʂ������o�b�t�@�Ɋi�[����B
 *******************************************************************************
 */
static void SubModbus_CrcMake( const uint8_t *parInBuf, uint8_t *parOutBuf, uint16_t arLen )
{
	uint16_t	wkLoop;															/* ���[�v�J�E���^ */
	uint16_t	wkLoop2;														/* ���[�v�J�E���^2 */
	uint16_t	wkCrcBuf;														/* CRC-16�f�[�^ */
	
	
	if( (parInBuf != M_NULL) && (parOutBuf != M_NULL) )
	{
		wkCrcBuf = 0xFFFFU;
		/*
		 ***************************************
		 *	 CRC-16�f�[�^�̏o�͌��ʂ�������
		 ***************************************
		 */
		parOutBuf[ 0 ] = 0x00U;
		parOutBuf[ 1 ] = 0x00U;
		
		for( wkLoop = 0U ; wkLoop < arLen ; wkLoop++ )
		{
			wkCrcBuf = parInBuf[ wkLoop ] ^ wkCrcBuf;							/* CRC�o�͌��ʂƓ��͂����f�[�^�̔r�����Ƃ� */
			/*
			 ***************************************
			 *	CRC�Z�o���Z
			 ***************************************
			 */
			for( wkLoop2 = 0U ; wkLoop2 < 8U ; wkLoop2++ )
			{																	/* 8�r�b�g�V�t�g����܂ŌJ��Ԃ� */
				if( (wkCrcBuf & 0x0001U) != 0U )								/* �L�����[�̗L���𒲂ׂ� */
				{
					wkCrcBuf >>= 1U;											/* CRC�o�͌��ʂ��E��1�r�b�g�V�t�g */
					wkCrcBuf ^= 0xA001U;										/* A001H�Ƃ̔r�����Ƃ� */
				}
				else
				{
					wkCrcBuf >>= 1U;											/* CRC�o�͌��ʂ��E��1�r�b�g�V�t�g */
				}
			}
		}
		parOutBuf[ 0 ] = (uint8_t)(wkCrcBuf & 0xFFU);							/* �Z�o���ʂ��i�[(L) */
		parOutBuf[ 1 ] = (uint8_t)((wkCrcBuf >> 8U) & 0xFFU);					/* �Z�o���ʂ��i�[(H) */
	}
}


/*
 *******************************************************************************
 *	�R�}���h���M
 *
 *	[���e]
 *		���x�Ǐo���R�}���h�𑗐M����B
 *******************************************************************************
 */
static void SubModbus_Snd( void )
{
	uint8_t		*pwkSndBgnPtr;													/* ���M�o�b�t�@�J�n�A�h���X */
	uint16_t	wkLoop;															/* ���[�v�J�E���^ */
	
	/* �ϐ������� */
	for( wkLoop = 0U; wkLoop < M_ArrayElement(gvModInf.mSndBuf); wkLoop++ )
	{
		gvModInf.mSndBuf[ wkLoop ] = 0U;										/* ���M�o�b�t�@������ */
	}
	
	pwkSndBgnPtr = gvModInf.mSndBuf;											/* ���M�o�b�t�@�J�n�A�h���X */
	
	gvModInf.mSndBuf[ 0U ] = 0x01U;												/* �X���[�u�A�h���X�i�[ */
	gvModInf.mSndBuf[ 1U ] = 0x04U;												/* �t�@���N�V�����R�[�h�i�[ */
	gvModInf.mSndBuf[ 2U ] = 0x13U;												/* ���Δԍ��i�[ */
	gvModInf.mSndBuf[ 3U ] = 0x87U;												/* ���Δԍ��i�[ */
	gvModInf.mSndBuf[ 4U ] = 0x00U;												/* �f�[�^���i�[ */
	gvModInf.mSndBuf[ 5U ] = 0x04U;												/* �f�[�^���i�[ */
	
	gvModInf.mSndLen = 6U;														/* ���M�f�[�^�������� */
	
	/* CRC�쐬 */
	SubModbus_ErrChkTxtMake(&gvModInf.mSndBuf[ 6U ], &gvModInf.mSndLen, pwkSndBgnPtr);
	
	ApiComDrv_Snd();															/* ���M���� */
	R_TAU0_Channel3_Custom(0xFFFF);												/* 700msec��Ƀ^�C�}���荞�� */
}


/*
 *******************************************************************************
 *	�R�}���h��M��͏���
 *
 *	[���e]
 *		���j�b�g���瑗�M���ꂽ�R�}���h����͂���B
 *******************************************************************************
 */
static void SubModbus_RcvAnalys( void )
{
	uint8_t			wkCrcPosLo;													/* CRC�i�[�ʒu(L) */
	uint8_t			wkCrcPosHi;													/* CRC�i�[�ʒu(H) */
	uint8_t			wkCrcBuf[2];												/* CRC�i�[�� */
	uint16_t		wkCrcLen;													/* CRC�p�o�b�t�@�� */
	uint16_t		wkDataU16;													/* int16_t�^�f�[�^ */
	uint16_t		wkLoop;														/* ���[�v�J�E���^ */
	
	uint8_t			wkChkRes = imNG;											/* �`�F�b�N���� */
	
	
	if( gvModInf.mRcvLen >= imAnaInCmdRecLen )									/* ��M�f�[�^������ */
	{
		/*
		 ***************************************
		 *	CRC�̃`�F�b�N
		 ***************************************
		 */
		wkCrcLen   = gvModInf.mRcvLen - 2U;										/* CRC�p�o�b�t�@���擾 */
		wkCrcPosLo = (uint8_t)(gvModInf.mRcvLen - 2U);							/* CRC�i�[�ʒu(L)�擾 */
		wkCrcPosHi = (uint8_t)(gvModInf.mRcvLen - 1U);							/* CRC�i�[�ʒu(H)�擾 */
		SubModbus_CrcMake(gvModInf.mRcvBuf, wkCrcBuf, wkCrcLen);				/* CRC�쐬 */
		
		/* CRC�A�X���[�u�A�h���X�A�t�@���N�V�����R�[�h���� */
		if( (wkCrcBuf[0] == gvModInf.mRcvBuf[wkCrcPosLo]) &&
			(wkCrcBuf[1] == gvModInf.mRcvBuf[wkCrcPosHi]) )
		{
			if( (gvModInf.mRcvBuf[imRCV_SRV_ADR_POS] == 0x01) &&				/* �X���[�u�A�h���X�`�F�b�N(1�Œ�) */
				(gvModInf.mRcvBuf[imRCV_FUNC_POS] == 0x04) )					/* �t�@���N�V�����R�[�h�`�F�b�N(4�Œ�) */
			{
				wkChkRes = imOK;												/* �ǂݏo������ */
			}
		}
	}
	
	if( wkChkRes == imOK )														/* �ǂݏo�������� */
	{
		/* �Z���T��擾 */
		wkDataU16 = SubModbus_Rcv8bitToS16bit( imRCV_SENSOR_H_POS );
		
		/* ���߂ăZ���T��擾�̂Ƃ� */
		if( gvInFlash.mProcess.mModelCode == 0xFFU )
		{
#if 1
			if( wkDataU16 <= imSensType_Max )
			{
				gvInFlash.mProcess.mModelCode = cSensorType[ wkDataU16 ].mSensorCode;
				
				if( gvInFlash.mProcess.mModelCode == ecSensType_V || gvInFlash.mProcess.mModelCode == ecSensType_Pulse )
				{
					gvInFlash.mProcess.mModelOffset = imSndOffset_100;
				}
				else
				{
					gvInFlash.mProcess.mModelOffset = imSndOffset_2100;
				}
				
				if( gvInFlash.mProcess.mModelCode == ecSensType_HumTmp && gvDispMode.mNowDisp == ecDispMode_Up1CHDwClk )
				{
					gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH;
				}
				else if( gvInFlash.mProcess.mModelCode == ecSensType_V && ( gvDispMode.mNowDisp == ecDispMode_Up1CHDwClk || gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH || gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH_P ) )
				{
					gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH_V;
				}
				else if( gvInFlash.mProcess.mModelCode == ecSensType_Pulse && ( gvDispMode.mNowDisp == ecDispMode_Up1CHDwClk || gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH || gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH_V ) )
				{
					gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH_P;
				}
			}
			
#else		/* �t���\���e�X�g */
			if( wkDataU16 <= imSensType_Max )
			{
			#if 1
				gvInFlash.mProcess.mModelCode = ecSensType_V;		 /* �d���\���e�X�g�p */
				gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH_V;
			#else
				gvInFlash.mProcess.mModelCode = ecSensType_Pulse; 	 /* �p���X�\���e�X�g�p */
				gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH_P;
			#endif
			}
			
#endif
			gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
		}
		
		/* �e�Z���T�̑���l�擾 */
		wkChkRes = imNG;
		if( wkDataU16 <= imSensType_Max )
		{
			if( gvInFlash.mProcess.mModelCode == cSensorType[ wkDataU16 ].mSensorCode )
			{
				/* �I�[�o�t���[/�A���_�[�t���[/�Z���T�ԒʐM�G���[���� */
				gvMeasPrm.mMeasError[ 0U ] &= ~(imTmpOver | imTmpUnder | imPtBurnError | imComTmpError | imSnsError);
				gvMeasPrm.mMeasError[ 1U ] &= ~(imTmpOver | imTmpUnder | imPtBurnError | imComTmpError | imSnsError);
				
				wkChkRes = cSensorType[ wkDataU16 ].mpSensorFunc();
			}
		}
	}
	
	
	if( wkChkRes == imNG )
	{
		gvModInf.mRetryCnt ++;
		if( gvBatAd.mBattSts != ecBattSts_Non && gvModInf.mRetryCnt <= 3U )
		{
			/* �ʐM���g���C */
			gvModInf.mComSts = ecComSts_NullPre;								/* NULL���M������ԂɑJ�� */
		}
		else
		{
			/* �ʐM���s�m��F����l�X�V */
//			gvMeasPrm.mMeasVal[ 0U ] = imSndSnsErr;								/* �Z���T�ԒʐM�G���[�l�ݒ� */
//			gvMeasPrm.mMeasVal[ 1U ] = imSndSnsErr;								/* �Z���T�ԒʐM�G���[�l�ݒ� */
			gvMeasPrm.mMeasVal[ 0U ] = SubModbus_ErrValChg( imSndSnsErr );
			gvMeasPrm.mMeasVal[ 1U ] = SubModbus_ErrValChg( imSndSnsErr );
			gvMeasPrm.mMeasError[ 0U ] |= imComTmpError;						/* �Z���T�ԒʐM�G���[�ݒ� */
			gvMeasPrm.mMeasError[ 1U ] |= imComTmpError;						/* �Z���T�ԒʐM�G���[�ݒ� */
			
			wkChkRes = imOK;
		}
	}
	
	/* ����l�X�V */
	if( wkChkRes == imOK )
	{
		gvModInf.mRetryCnt = 0U;
		gvMeasPrm.mTime = gvRecCycRtcTm;										/* �������i�[ */
		
		gvMeasPrm.mTimeEvt = ApiTimeComp_GetLocalTime();						/* �C�x���g���莞�� */
		gvMeasPrm.mLogic = gvInFlash.mParam.mEventKeyFlg;						/* Event�L�[���i�[ */
		
		SubModbus_MeasErrChk();													/* ����G���[���� */
		gvModInf.mComSts = ecComSts_Init;										/* �t����uOFF�ɑJ�� */
	}
	
	
	gvModuleSts.mCom = ecComModuleSts_Run;										/* �ʐM���W���[���̓A�N�e�B�u */
	
	/* �ʐM�ϐ������� */
	for( wkLoop = 0U; wkLoop < M_ArrayElement(gvModInf.mRcvBuf); wkLoop++ )
	{
		gvModInf.mRcvBuf[ wkLoop ] = 0U;										/* ��M�o�b�t�@������ */
	}
	gvModInf.mRcvLen = 0U;														/* ��M�f�[�^�������� */
}


/*
 *******************************************************************************
 *	�Z���T���j�b�g�̑���l���o
 *
 *	[���e]
 *		����l���o
 *******************************************************************************
 */
static uint8_t SubModbus_RcvSens( void )
{
	sint16_t wkDataS16;
	
	/* ����l�擾 */
	wkDataS16 = (sint16_t)SubModbus_Rcv8bitToS16bit( imRCV_MEAS_H_POS );
	
	/* �v���[�u���ŃI�[�o/�A���_�[�t���[���Ă��� */
	if( wkDataS16 == imComUp_Sensor )
	{
//		gvMeasPrm.mMeasVal[ 0U ] = imSndTmpOver;							/* �I�[�o�[�t���[�l��ݒ� */
		gvMeasPrm.mMeasVal[ 0U ] = SubModbus_ErrValChg( imSndTmpOver );
		gvMeasPrm.mMeasError[ 0U ] |= imTmpOver;							/* �I�[�o�[�t���[�G���[����ݒ� */
	}
	else if( wkDataS16 == imComDw_Sensor )
	{
//		gvMeasPrm.mMeasVal[ 0U ] = imSndTmpUnder;							/* �A���_�[�t���[�l��ݒ� */
		gvMeasPrm.mMeasVal[ 0U ] = SubModbus_ErrValChg( imSndTmpUnder );
		gvMeasPrm.mMeasError[ 0U ] |= imTmpUnder;							/* �A���_�[�t���[�G���[����ݒ� */
	}
	else if( wkDataS16 == imComBurn_Sensor )
	{
//		gvMeasPrm.mMeasVal[ 0U ] = imSndTmpBurn;							/* �o�[���A�E�g�l��ݒ� */
		gvMeasPrm.mMeasVal[ 0U ] = SubModbus_ErrValChg( imSndTmpBurn );
		gvMeasPrm.mMeasError[ 0U ] |= imPtBurnError;						/* �o�[���A�E�g�G���[����ݒ� */
	}
	else if( wkDataS16 == imComHard_Sensor )
	{
//		gvMeasPrm.mMeasVal[ 0U ] = imSndSnsErr;								/* �n�[�h�G���[�l��ݒ� */
		gvMeasPrm.mMeasVal[ 0U ] = SubModbus_ErrValChg( imSndSnsErr );
		gvMeasPrm.mMeasError[ 0U ] |= imSnsError;							/* �n�[�h�G���[����ݒ� */
	}
	else
	{
		gvMeasPrm.mMeasVal[ 0U ] = SubModbus_Rcv1ChMeasParse( wkDataS16 );
	}
	
	if( gvInFlash.mProcess.mModelCode == ecSensType_Pt )
	{
		/* �ړ_���擾 */
		gvMeasPrm.mMeasVal[ 1U ] = (sint16_t)SubModbus_Rcv8bitToS16bit( imRCV_PT_CON_H_POS );
	}
	else
	{
//		gvMeasPrm.mMeasVal[ 1U ] = imSndTmpInit;		/* CH2�������� */
		gvMeasPrm.mMeasVal[ 1U ] = SubModbus_ErrValChg( imSndTmpInit );
		gvMeasPrm.mMeasError[ 1U ] = imNonError;		/* CH2�������� */
	}
	
	return imOK;
}

/*
 *******************************************************************************
 *	�����x�̑���l���o
 *
 *	[���e]
 *		�����x�̑���l���o
 *******************************************************************************
 */
static uint8_t SubModbus_RcvHumTmp( void )
{
	sint16_t	wkDataS16;
	sint32_t	wkDataS32;
	uint8_t		wkChkRes = imOK;
	
	static uint8_t	vNoiseRetryCnt = 0U;
	
	gvModInf.mRetryCnt = 0U;
	
	/* 1ch:���x�擾 */
	wkDataS16 = (sint16_t)SubModbus_Rcv8bitToS16bit( imRCV_TMPHUM_TMP_H_POS );
	
	/* �v���[�u���ŃI�[�o/�A���_�[�t���[���Ă��� */
	if( wkDataS16 == imComUp_HumTmpModel )
	{
//		gvMeasPrm.mMeasVal[ 0U ] = imSndTmpOver;							/* �I�[�o�[�t���[�l��ݒ� */
		gvMeasPrm.mMeasVal[ 0U ] = SubModbus_ErrValChg( imSndTmpOver );
		gvMeasPrm.mMeasError[ 0U ] |= imTmpOver;							/* �I�[�o�[�t���[�G���[����ݒ� */
	}
	else if( wkDataS16 == imComDw_HumTmpModel )
	{
//		gvMeasPrm.mMeasVal[ 0U ] = imSndTmpUnder;							/* �A���_�[�t���[�l��ݒ� */
		gvMeasPrm.mMeasVal[ 0U ] = SubModbus_ErrValChg( imSndTmpUnder );
		gvMeasPrm.mMeasError[ 0U ] |= imTmpUnder;							/* �A���_�[�t���[�G���[����ݒ� */
	}
	else
	{
		/* �����_�ꌅ(�l�̌ܓ�) */
		if( wkDataS16 >= 0 )
		{
			wkDataS16 += 5U;
		}
		else
		{
			wkDataS16 -= 5U;
		}
		wkDataS16 /= 10;
		
		gvMeasPrm.mMeasVal[ 0U ] = SubModbus_Rcv1ChMeasParse( wkDataS16 );
		
	}
	
	
	/* 2ch:���x�擾 */
	wkDataS16 = (sint16_t)SubModbus_Rcv8bitToS16bit( imRCV_TMPHUM_HUM_H_POS );
	
	/* �v���[�u���Ŏ��x�e�[�u����������Ă��� */
	if( wkDataS16 == imComDummy_HumTmpModel )
	{
//		wkDataS16 = imSndTmpUnder;
		wkDataS16 = SubModbus_ErrValChg( imSndTmpUnder );
		gvMeasPrm.mMeasError[ 1U ] |= imTmpUnder;							/* �A���_�[�t���[�G���[����ݒ� */
	}
	else
	{
		/* �����_�ꌅ(�l�̌ܓ�) */
		wkDataS16 += 5U;
		wkDataS16 /= 10;
		
		/* �v���l�␳ */
		wkDataS32 = (((sint32_t)wkDataS16 * gvInFlash.mParam.mSlope[ 1U ]) / 100) + gvInFlash.mParam.mOffset[ 1U ];
		wkDataS16 = (sint16_t)wkDataS32;
		
		/* �I�[�o/�A���_�[�t���[���� */
		if( wkDataS16 > imHUMUp_HumTmpModel )
		{
			wkDataS16 = imHUMUp_HumTmpModel;
		}
		else if( wkDataS16 < imHUMDw_HumTmpModel )
		{
			wkDataS16 = imHUMDw_HumTmpModel;
		}
		
		/* ���x0%�F3�񃊃g���C */
		if( wkDataS16 == 0 )
		{
			vNoiseRetryCnt ++;
			if( vNoiseRetryCnt <= 3 )
			{
				wkChkRes = imNG;
			}
		}
	}
	
	if( wkChkRes == imOK )
	{
		vNoiseRetryCnt = 0U;
		gvMeasPrm.mMeasVal[ 1U ] = wkDataS16;
	}
	
	return wkChkRes;
}


/*
 *******************************************************************************
 * 1CH�̑���l�p�[�X
 *
 *	[���e]
 *		1CH�̑���l���p�[�X����
 *	[����]
 *		sint16_t arVal�F����l
 *	[�ߒl]
 *		sint16_t�F�p�[�X��̑���l
 *******************************************************************************
 */
static sint16_t SubModbus_Rcv1ChMeasParse( sint16_t arVal )
{
	sint16_t wkUpVal;
	sint16_t wkDwVal;
	sint16_t wkDataS16;
	sint32_t wkDataS32;
	
	/* �v���l�␳ */
	wkDataS32 = (((sint32_t)arVal * gvInFlash.mParam.mSlope[ 0U ]) / 100) + gvInFlash.mParam.mOffset[ 0U ];
	wkDataS16 = (sint16_t)wkDataS32;
	
	switch( gvInFlash.mProcess.mModelCode )
	{
		case ecSensType_HumTmp:
			wkUpVal = imTMPUp_HumTmpModel;
			wkDwVal = imTMPDw_HumTmpModel;
			break;
			
		case ecSensType_Pt:
			wkUpVal = imTMPUp_PtModel;
			wkDwVal = imTMPDw_PtModel;
			break;
			
		case ecSensType_V:
			wkUpVal = imTMPUp_VolModel;
			wkDwVal = imTMPDw_VolModel;
			break;
			
		case ecSensType_Pulse:
			wkUpVal = imTMPUp_PulseModel;
			wkDwVal = imTMPDw_PulseModel;
			break;
	}
	
	/* �I�[�o/�A���_�[�t���[���� */
	if( wkDataS16 > wkUpVal )
	{
//		wkDataS16 = imSndTmpOver;										/* �I�[�o�[�t���[�l��ݒ� */
		wkDataS16 = SubModbus_ErrValChg( imSndTmpOver );
		gvMeasPrm.mMeasError[ 0U ] |= imTmpOver;						/* �I�[�o�[�t���[�G���[����ݒ� */
	}
	else if( wkDataS16 < wkDwVal )
	{
//		wkDataS16 = imSndTmpUnder;										/* �A���_�[�t���[�l��ݒ� */
		wkDataS16 = (sint32_t)SubModbus_ErrValChg( imSndTmpUnder );
		gvMeasPrm.mMeasError[ 0U ] |= imTmpUnder;						/* �A���_�[�t���[�G���[����ݒ� */
	}
	
	return wkDataS16;
}

/*
 *******************************************************************************
 * ����l�ُ�̒l���Z���T��ɍ��킹�ĕϊ�
 *
 *	[���e]
 *		����l�ُ�̒l���Z���T��ɍ��킹�ĕϊ�
 *	[����]
 *		sint16_t arVal�F����l�ُ�l
 *	[�ߒl]
 *		sint16_t�F�ϊ���̒l
 *******************************************************************************
 */
sint16_t SubModbus_ErrValChg( sint16_t arVal )
{
	if( gvInFlash.mProcess.mModelCode == ecSensType_V ||
		gvInFlash.mProcess.mModelCode == ecSensType_Pulse )
	{
		arVal += 2000;
	}
	
	return arVal;
}


#if 0
/*
 *******************************************************************************
 *	
 *
 *	[���e]
 *		
 *******************************************************************************
 */
static uint8_t SubModbus_RcvDummy( void )
{
	return imOK;
}
#endif

#if 0
/*
 *******************************************************************************
 *	�d���̑���l���o
 *
 *	[���e]
 *		�d���̑���l���o
 *******************************************************************************
 */
static uint8_t SubModbus_RcvVol( void )
{
	sint16_t wkDataS16;
	sint32_t wkDataS32;
	
	/* �d���擾 */
	wkDataS16 = (sint16_t)SubModbus_Rcv8bitToS16bit( imRCV_Voltage_H_POS );
	
	if( wkDataS16 == imComUp_VolModel )
	{
		wkDataS32 = imSndTmpOver;											/* �I�[�o�[�t���[�l��ݒ� */
		gvMeasPrm.mMeasError[ 0U ] |= imTmpOver;							/* �I�[�o�[�t���[�G���[����ݒ� */
	}
	else if( wkDataS16 == imComDw_VolModel )
	{
		wkDataS32 = imSndTmpUnder;											/* �A���_�[�t���[�l��ݒ� */
		gvMeasPrm.mMeasError[ 0U ] |= imTmpUnder;							/* �A���_�[�t���[�G���[����ݒ� */
	}
	else if( wkDataS16 == imComHard_VolModel )
	{
		wkDataS32 = imSndSnsErr;											/* �n�[�h�G���[�l��ݒ� */
		gvMeasPrm.mMeasError[ 0U ] |= imSnsError;							/* �n�[�h�G���[����ݒ� */
	}
	else
	{
		/* �v���l�␳ */
		wkDataS32 = wkDataS16;
		wkDataS32 *= gvInFlash.mParam.mSlope[ 0U ];
		wkDataS32 /= 100;
		wkDataS32 += gvInFlash.mParam.mOffset[ 0U ];
		
		/* �I�[�o/�A���_�[�t���[���� */
		if( wkDataS32 > imTMPUp_VolModel )
		{
			wkDataS32 = imSndTmpOver;										/* �I�[�o�[�t���[�l��ݒ� */
			gvMeasPrm.mMeasError[ 0U ] |= imTmpOver;						/* �I�[�o�[�t���[�G���[����ݒ� */
		}
		else if( wkDataS32 < imTMPDw_VolModel )
		{
			wkDataS32 = imSndTmpUnder;										/* �A���_�[�t���[�l��ݒ� */
			gvMeasPrm.mMeasError[ 0U ] |= imTmpUnder;						/* �A���_�[�t���[�G���[����ݒ� */
		}
	}
	
	gvMeasPrm.mMeasVal[ 0U ] = (sint16_t)wkDataS32;
	gvMeasPrm.mMeasVal[ 1U ] = imSndTmpInit;		/* CH2�������� */
	gvMeasPrm.mMeasError[ 1U ] = imNonError;		/* CH2�������� */

	return imOK;
}

/*
 *******************************************************************************
 *	�p���X�̑���l���o
 *
 *	[���e]
 *		�p���X�̑���l���o
 *******************************************************************************
 */
static uint8_t SubModbus_RcvPulse( void )
{
	uint16_t wkDataU16;
	sint16_t wkDataS16;
	sint32_t wkDataS32;
	
	/* �p���X�擾 */
	wkDataS16 = (sint16_t)SubModbus_Rcv8bitToS16bit( imRCV_Pulse_H_POS );
	
	if( wkDataS16 == imComUp_PulseModel )
	{
		wkDataS32 = imSndTmpOver;											/* �I�[�o�[�t���[�l��ݒ� */
		gvMeasPrm.mMeasError[ 0U ] |= imTmpOver;							/* �I�[�o�[�t���[�G���[����ݒ� */
	}
	else if( wkDataS16 == imComDw_PulseModel )
	{
		wkDataS32 = imSndTmpUnder;											/* �A���_�[�t���[�l��ݒ� */
		gvMeasPrm.mMeasError[ 0U ] |= imTmpUnder;							/* �A���_�[�t���[�G���[����ݒ� */
	}
	else if( wkDataS16 == imComHard_PulseModel )
	{
		wkDataS32 = imSndSnsErr;											/* �n�[�h�G���[�l��ݒ� */
		gvMeasPrm.mMeasError[ 0U ] |= imSnsError;							/* �n�[�h�G���[����ݒ� */
	}
	else
	{
		/* �v���l�␳ */
		wkDataS32 = wkDataS16;
		wkDataS32 *= gvInFlash.mParam.mSlope[ 0U ];
		wkDataS32 /= 100;
		wkDataS32 += gvInFlash.mParam.mOffset[ 0U ];
		
		/* �I�[�o/�A���_�[�t���[���� */
		if( wkDataS32 > imTMPUp_PulseModel )
		{
			wkDataS32 = imSndTmpOver;										/* �I�[�o�[�t���[�l��ݒ� */
			gvMeasPrm.mMeasError[ 0U ] |= imTmpOver;						/* �I�[�o�[�t���[�G���[����ݒ� */
		}
		else if( wkDataS32 < imTMPDw_PulseModel )
		{
			wkDataS32 = imSndTmpUnder;										/* �A���_�[�t���[�l��ݒ� */
			gvMeasPrm.mMeasError[ 0U ] |= imTmpUnder;						/* �A���_�[�t���[�G���[����ݒ� */
		}
	}
	
	gvMeasPrm.mMeasVal[ 0U ] = (sint16_t)wkDataS32;
	gvMeasPrm.mMeasVal[ 1U ] = imSndTmpInit;		/* CH2�������� */
	gvMeasPrm.mMeasError[ 1U ] = imNonError;		/* CH2�������� */

	return imOK;
}
#endif

/*
 *******************************************************************************
 *	Modbus��M�o�b�t�@����l���`
 *
 *	[���e]
 *		Modbus��M�o�b�t�@����16bit�̒l(���x�l�A���x�l�Ȃ�)�𐬌`
 *******************************************************************************
 */
static sint16_t SubModbus_Rcv8bitToS16bit( uint8_t arNo )
{
	uint16_t wkDataU16;
	
	wkDataU16 = ((uint16_t)gvModInf.mRcvBuf[ arNo ] << 8U) + (uint16_t)gvModInf.mRcvBuf[ arNo + 1U ];
	
	return wkDataU16;
}

/*
 *******************************************************************************
 *	�X�e�[�^�X�̏�ԑJ��
 *
 *	[���e]
 *		�X�e�[�^�X�̏�ԑJ��
 *******************************************************************************
 */
static void SubModbus_StateTrans( void )
{
	switch( gvModuleSts.mExe )
	{
		/* ����̂݁�Sleep */
		default:
		case ecExeModuleSts_Meas:
			gvModuleSts.mExe = ecExeModuleSts_Sleep;
			break;
		/* ���^1���x�񔻒�1 */
		case ecExeModuleSts_Log1:
			gvModuleSts.mExe = ecExeModuleSts_Alm1;
			break;
		/* ���^2���x�񔻒�2 */
		case ecExeModuleSts_Log2:
			gvModuleSts.mExe = ecExeModuleSts_Alm2;
			break;
	}
	
	/* EventKey���x�񔻒� */
	if( gvModuleSts.mEventKey == ecEventKeyModuleSts_Meas )
	{
		gvModuleSts.mEventKey = ecEventKeyModuleSts_Alm;
	}
	gvModuleSts.mLcd = ecLcdModuleSts_Run;										/* LCD�X�V */
}


/*
 *******************************************************************************
 *	�Z���T�ԒʐM����������
 *
 *	[���e]
 *		�Z���T�ɃR�}���h���M���Ă���800msec���������������ꍇ�A
 *		�ʐM�ُ�G���[�Ƃ���B
 *******************************************************************************
 */
void ApiModbus_NoReplyJudge( void )
{
	uint16_t		wkLoop;													/* ���[�v�J�E���^ */
	
	if( gvModInf.mComSts == ecComSts_RcvWait )								/* ��M�҂��� */
	{
		R_TAU0_Channel3_Stop();												/* �^�C�}0(CH3)��~ */
		
		gvModInf.mRetryCnt ++;
		if( gvBatAd.mBattSts != ecBattSts_Non && gvModInf.mRetryCnt <= 3U || gvInFlash.mProcess.mModelCode == 0xFFU )
		{
			if( gvInFlash.mProcess.mModelCode == 0xFFU )
			{
				gvModInf.mRetryCnt = 0U;
			}
			/* �ʐM���g���C */
			gvModInf.mComSts = ecComSts_NullPre;							/* NULL���M������ԂɑJ�� */
		}
		else
		{
			/* �ʐM���s�m�� */
			gvModInf.mRetryCnt = 0U;
//			gvMeasPrm.mMeasVal[ 0U ] = imSndSnsErr;							/* �Z���T�ԒʐM�G���[�l�ݒ� */
//			gvMeasPrm.mMeasVal[ 1U ] = imSndSnsErr;							/* �Z���T�ԒʐM�G���[�l�ݒ� */
			gvMeasPrm.mMeasVal[ 0U ] = SubModbus_ErrValChg( imSndSnsErr );
			gvMeasPrm.mMeasVal[ 1U ] = SubModbus_ErrValChg( imSndSnsErr );
			gvMeasPrm.mMeasError[ 0U ] |= imComTmpError;					/* �Z���T�ԒʐM�G���[�ݒ� */
			gvMeasPrm.mMeasError[ 1U ] |= imComTmpError;					/* �Z���T�ԒʐM�G���[�ݒ� */
			
			gvMeasPrm.mTime = gvRecCycRtcTm;								/* �������i�[ */
			
			gvMeasPrm.mTimeEvt = ApiTimeComp_GetLocalTime();				/* �C�x���g���莞�� */
			gvMeasPrm.mLogic = gvInFlash.mParam.mEventKeyFlg;				/* Event�L�[���i�[ */
			
			SubModbus_MeasErrChk();											/* ����G���[���� */
			gvModInf.mComSts = ecComSts_Init;								/* �t����uOFF�ɑJ�� */
		}
		gvModuleSts.mCom = ecComModuleSts_Run;								/* �ʐM���W���[���̓A�N�e�B�u */
		
		/* �ϐ������� */
		for( wkLoop = 0U; wkLoop < M_ArrayElement(gvModInf.mRcvBuf); wkLoop++ )
		{
			gvModInf.mRcvBuf[ wkLoop ] = 0U;								/* ��M�o�b�t�@������ */
		}
		gvModInf.mRcvLen = 0U;												/* ��M�f�[�^�������� */
	}
}


/*
 *******************************************************************************
 * ����G���[����
 *
 *	[���e]
 *		�������Ă���G���[�̃t���O�𗧂āAFlash�Ɉُ헚���Ƃ��ċL�^����B
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubModbus_MeasErrChk( void )
{
	uint8_t					wkChannel;
	ET_ModMeasErrChkKind_t	wkAlmKind;
	uint16_t				wkAbnSts;
	uint16_t				wkMeasError;
	uint16_t				wkMeasErrFlg;
	
	for( wkChannel = 0U ; wkChannel < imMeasChNum ; wkChannel++ )
	{
		/* �����������̒ʕ�D�� */
		wkMeasError = gvMeasPrm.mMeasError[ wkChannel ];
		
		if( wkMeasError & imTmpUnder )
		{
			wkMeasError &= imTmpUnder;
		}
		if( wkMeasError & imTmpOver )
		{
			wkMeasError &= imTmpOver;
		}
		if( wkMeasError & imPtBurnError )
		{
			wkMeasError &= imPtBurnError;
		}
		if( wkMeasError & imSnsError )
		{
			wkMeasError &= imSnsError;
		}
		
		if( wkMeasError & imComTmpError )
		{
			wkMeasError &= imComTmpError;
		}
		
		for( wkAlmKind = ecModMeasErrChkKind_Burn ; wkAlmKind < ecModMeasErrChkKindMax ; wkAlmKind++ )
		{
			switch( wkAlmKind )
			{
				/* �o�[���A�E�g���� */
				case ecModMeasErrChkKind_Burn:
					wkAbnSts = imAbnSts_BurnCh1;
					wkMeasErrFlg = imRefBurnError | imThBurnError;
					break;
					
				/* �A���_�[�����W���� */
				case ecModMeasErrChkKind_Under:
					wkAbnSts = imAbnSts_UdRngCh1;
					wkMeasErrFlg = imTmpUnder;
					break;
					
				/* �I�[�o�[�����W���� */
				case ecModMeasErrChkKind_Over:
					wkAbnSts = imAbnSts_OvRngCh1;
					wkMeasErrFlg = imTmpOver;
					break;
					
				/* �Z���T�@��ُ헚�� */
				case ecModMeasErrChkKind_SnsErr:
					wkAbnSts = imAbnSts_SnsrErr;
					wkMeasErrFlg = imSnsError;
					break;
					
				/* �Z���T�ԒʐM�G���[���� */
				case ecModMeasErrChkKind_ComErr:
					wkAbnSts = imAbnSts_SnsrComErr;
					wkMeasErrFlg = imComTmpError;
					break;
			}
			wkAbnSts <<= wkChannel;
			
			if( wkMeasError & wkMeasErrFlg )
			{
				/* ����Z�b�g */
				ApiAbn_AbnStsSet( wkAbnSts, ecAbnInfKind_MeasSts );
			}
			else
			{
				/* ����Z�b�g */
				ApiAbn_AbnStsClr( wkAbnSts, ecAbnInfKind_MeasSts );
			}
		}
	}
}

#pragma section