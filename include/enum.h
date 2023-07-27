/*
 *******************************************************************************
 *	File name	:	enum.h
 *
 *	[���e]
 *		�񋓌^�̒�`
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]			[�T�v]
 *	2017.11.08		Softex N.I			�V�K�쐬
 *******************************************************************************
 */
#ifndef	INCLUDED_ENUM_H
#define	INCLUDED_ENUM_H


/*
 *------------------------------------------------------------------------------
 *	�G���[��`
 *------------------------------------------------------------------------------
 */
typedef enum ET_Error
{
	ecERR_OK = 0,						/* OK���� */
	ecERR_NG,							/* NG���� */

	ecERRMax							/* enum�ő�l */
} ET_Error_t;


/*
 *------------------------------------------------------------------------------
 *	���[�h
 *------------------------------------------------------------------------------
 */
typedef enum ET_Mode
{
	ecMode_Normal = 0,					/* �ʏ탂�[�h */
	ecMode_Process,						/* �H�����[�h */
	ecMode_RfTxLoRa,					/* �������M���s:LoRa */
	ecMode_RfTxFSK,						/* �������M���s:FSK */
	ecMode_RfTxLoRaStop,
	ecMode_RfTxFSKStop,
	ecMode_RfRxLoRa,					/* ������M���s��:LoRa */
	ecMode_RfRxFSK,						/* ������M���s��:FSK */
	ecMode_RfRxLoRaStop,
	ecMode_RfRxFSKStop,
	ecMode_RfRxLoRaCons,				/* ������M����d�� */

	ecModeMax							/* enum�ő�l */
} ET_Mode_t;


/*
 *------------------------------------------------------------------------------
 *	�Z���T��
 *------------------------------------------------------------------------------
 */
typedef enum ET_SensType
{
	ecSensType_InTh = 0,				/* �����T�[�~�X�^ */
	ecSensType_ExTh,					/* �O�t���T�[�~�X�^ */
	ecSensType_HumTmp,					/* �����x */
	ecSensType_Pt,						/* ������R�� */
	ecSensType_V,						/* �d�� */
	ecSensType_Pulse,					/* �p���X */
	ecSensType_KTc,						/* K�M�d�� */
	ecSensType_TTc,						/* T�M�d�� */
	
	ecSensTypeMax						/* enum�ő�l */
} ET_SensType_t;


/*
 *------------------------------------------------------------------------------
 *	���艷�x�G���[
 *------------------------------------------------------------------------------
 */
typedef enum ET_TempErr
{
	ecTempErr_Non = 0,					/* �ُ햳�� */
	ecTempErr_Ovr,						/* �I�[�o�[�t���[ */
	ecTempErr_Udr,						/* �A���_�[�t���[ */

	ecTempErrMax						/* enum�ő�l */
} ET_TempErr_t;


/*
 *------------------------------------------------------------------------------
 *	����`�����l��
 *------------------------------------------------------------------------------
 */
/* �`�����l�� */
typedef enum ET_Ch
{
	ecCh_1 = 0,							/* 1ch */
	ecCh_2,								/* 2ch */
	ecCh_3,								/* 3ch */
	ecCh_4,

	ecChMax								/* enum�ő�l */
} ET_Ch_t;

/*
 *------------------------------------------------------------------------------
 *	�ʐM��`
 *------------------------------------------------------------------------------
 */
#if (swSensorCom == imEnable)
typedef enum ET_ComSts
{
	ecComSts_Init = 0,
	ecComSts_Lcdoff,
	ecComSts_NullPre,															/* NULL���M���� */
	ecComSts_NullSnd,															/* NULL���M�� */
	ecComSts_UnitPwnOnWait,														/* ���j�b�g�N���҂� */
	ecComSts_SndPre,															/* ���M���� */
	ecComSts_Snd,																/* ���M�� */
	ecComSts_SndEnd,															/* ���M���� */
	ecComSts_RcvWait,															/* ��M�҂� */
	ecComSts_Rcv,																/* ��M�� */
	ecComSts_RcvAnalys,															/* ��M��� */
	
	ecComStsMax																	/* enum�ő�l */
} ET_ComSts_t;
#else
typedef enum ET_ComSts
{
	ecComSts_RcvWait = 0,														/* ��M�҂� */
	ecComSts_RcvStart,															/* ��M�J�n�҂� */
	ecComSts_Rcv,																/* ��M�� */
	ecComSts_RcvEnd,															/* ��M���� */
	ecComSts_SndWait,															/* ���M�҂� */
	ecComSts_Snd,																/* ���M�� */
	ecComSts_SndEnd,															/* ���M���� */
	
	ecComStsMax																	/* enum�ő�l */
} ET_ComSts_t;
#endif


/*
 *------------------------------------------------------------------------------
 *	A/D�R���o�[�^�����X�e�[�^�X
 *------------------------------------------------------------------------------
 */
typedef enum ET_AdcSts
{
	ecAdcSts_Nop = 0,															/* ������ */
	ecAdcSts_DryStart,															/* ���d�r�pA/D�ϊ��J�n */
	ecAdcSts_DryAct,															/* ���d�r�pA/D�ϊ��� */
	ecAdcSts_DryFin,															/* ���d�r�pA/D�ϊ����� */

	ecAdcStsMax																	/* enum�ő�l */
} ET_AdcSts_t;


/*
 *------------------------------------------------------------------------------
 *	�������
 *------------------------------------------------------------------------------
 */
typedef enum ET_DateTime
{
	ecDateTime_Year = 0,														/* �N */
	ecDateTime_Month,															/* �� */
	ecDateTime_Day,																/* �� */
	ecDateTime_Week,															/* �j */
	ecDateTime_Hour,															/* �� */
	ecDateTime_Min,																/* �� */
	ecDateTime_Sec,																/* �b */
	
	ecDateTimeMax																/* enum�ő�l */
} ET_DateTime_t;


/*
 *------------------------------------------------------------------------------
 *	RTC(S-35390A) �������X�e�[�^�X
 *------------------------------------------------------------------------------
 */
typedef enum ET_RtcDrvInitSts
{
	ecRtcDrvInitSts_RegInit = 0,												/* ���W�X�^������ */
	ecRtcDrvInitSts_RealRead,													/* ���A���^�C���f�[�^�ǂݏo�� */
	ecRtcDrvInitSts_InitEnd,													/* ���������� */
	ecRtcDrvInitSts_24Write,

	ecRtcDrvInitStsMax
} ET_RtcDrvInitSts_t;


/*
 *------------------------------------------------------------------------------
 *	����RTC���
 *------------------------------------------------------------------------------
 */
typedef enum ET_RtcSts
{
	ecRtcSts_Non = 0,															/* �ُ햳�� */
	ecRtcSts_Fast,																/* ����RTC�i�� */
	ecRtcSts_Delay,																/* ����RTC�x�� */

	ecRtcStsMax																	/* enum�ő�l */
} ET_RtcSts_t;

/*
 *------------------------------------------------------------------------------
 *	�O�t���t���b�V�����������
 *------------------------------------------------------------------------------
 */
typedef enum ET_FlashKind
{
	ecFlashKind_Prim = 0,														/* �v���C�}�� */
	ecFlashKind_Second,															/* �Z�J���_�� */
	
	ecFlashKindMax																/* enum�ő�l */
} ET_FlashKind_t;


/*
 *------------------------------------------------------------------------------
 *	�O�t���t���b�V�����
 *------------------------------------------------------------------------------
 */
typedef enum ET_FlashSts
{
	ecFlashSts_PowOn = 0,														/* Flash�d��ON */
	ecFlashSts_PowOnWait,														/* Flash�d������҂� */
	ecFlashSts_Init,															/* ������� */
	ecFlashSts_SctrErase,														/* �Z�N�^��������� */
	ecFlashSts_FirstSctrWr,														/* �O���Z�N�^�������ݒ���� */

	ecFlashStsMax																/* enum�ő�l */
} ET_FlashSts_t;


/*
 *------------------------------------------------------------------------------
 *	�O�t��Flash�̈��
 *------------------------------------------------------------------------------
 */
typedef enum ET_RegionKind
{
	ecRegionKind_Non = 0,														/* �����l */
	ecRegionKind_ErrInfo,														/* �ُ��� */
	ecRegionKind_MeasAlm,														/* �v���x�� */
	ecRegionKind_Meas1,															/* ����l1 */
	ecRegionKind_Event,															/* �C�x���g */
	ecRegionKind_ActLog,														/* ���엚�� */
	ecRegionKind_SysLog,														/* �V�X�e�����O */
	ecRegionKind_Meas1Adr,
	ecRegionKind_SysLogAdr,

	ecRegionKindMax																/* enum�ő�l */
} ET_RegionKind_t;


/*
 *------------------------------------------------------------------------------
 *	���菈���t�F�[�Y
 *------------------------------------------------------------------------------
 */
typedef enum ET_MeasPhase
{
	ecMeasPhase_Init = 0,
	ecMeasPhase_Lcdoff,
	ecMeasPhase_InitRef,														/* ���t�@�����X���� */
	ecMeasPhase_ChrgRef,														/* ���t�@�����X�[�d */
	ecMeasPhase_MeasRef,														/* ���t�@�����X�v�� */
	ecMeasPhase_DisChrgRef,														/* ���t�@�����X���d */
	ecMeasPhase_DisChrgRefWait,
	ecMeasPhase_InitTh,															/* �T�[�~�X�^���� */
	ecMeasPhase_ChrgTh,															/* �T�[�~�X�^�[�d */
	ecMeasPhase_MeasTh,															/* �T�[�~�X�^�v�� */
	ecMeasPhase_DisChrgTh,														/* �T�[�~�X�^���d */
	ecMeasPhase_Wait,															/* ����҂� */

	ecMeasPhaseMax																/* enum�ő�l */
} ET_MeasPhase_t;


/*
 *------------------------------------------------------------------------------
 *	���^����1
 *------------------------------------------------------------------------------
 */
typedef enum ET_RecKind1
{
	ecRecKind1_None = 0,
	ecRecKind1_5sec,															/* 5�b */
	ecRecKind1_10sec,															/* 10�b */
	ecRecKind1_30sec,															/* 30�b */
	ecRecKind1_1min,															/* 1�� */
	ecRecKind1_5min,															/* 5�� */
	ecRecKind1_10min,															/* 10�� */
	ecRecKind1_15min,															/* 15�� */
	ecRecKind1_30min,															/* 30�� */
	ecRecKind1_60min,															/* 60�� */

	ecRecKind1Max																/* enum�ő�l */
} ET_RecKind1_t;


/*
 *------------------------------------------------------------------------------
 *	���^����2
 *------------------------------------------------------------------------------
 */
typedef enum ET_RecKind2
{
	ecRecKind2_None = 0,
	ecRecKind2_5sec,															/* 5�b */
	ecRecKind2_10sec,															/* 10�b */
	ecRecKind2_30sec,															/* 30�b */
	ecRecKind2_1min,															/* 1�� */
	ecRecKind2_5min,															/* 5�� */
	ecRecKind2_10min,															/* 10�� */

	ecRecKind2Max																/* enum�ő�l */
} ET_RecKind2_t;


/*
 *------------------------------------------------------------------------------
 *	�����ʐM �������X�e�[�^�X
 *------------------------------------------------------------------------------
 */
typedef enum ET_RFDrvInitSts
{
	ecRFDrvInitSts_Init = 0,													/* �������J�n */
	ecRFDrvInitSts_InitPorWait,													/* ���������s�� */
	ecRfDrvInitSts_InitRstWait,													/* SX1272���Z�b�g���s�E�F�C�g */
	ecRfDrvInitSts_InitRstEndWait,												/* SX1272���Z�b�g��̃E�F�C�g */
	ecRFDrvInitSts_InitEnd,														/* ���������� */

	ecRFDrvInitStsMax
} ET_RFDrvInitSts_t;


/*
 *------------------------------------------------------------------------------
 *	ROM���������X�e�[�^�X
 *------------------------------------------------------------------------------
 */
typedef enum ET_FirmUpSts
{
	ecFirmUpSts_Init = 0,														/* ������ */
	ecFirmUpSts_BlankCheck,														/* �u�����N�`�F�b�N */
	ecFirmUpSts_Erase,															/* ���� */
	ecFirmUpSts_Write,															/* ������ */
	ecFirmUpSts_Verify,															/* �x���t�@�C */
	ecFirmUpSts_Close,															/* �I������ */
	ecFirmUpSts_End,															/* �I�� */

	ecFirmUpStsMax
} ET_FirmUpSts_t;


/*
 *------------------------------------------------------------------------------
 *	�\�����[�h
 *------------------------------------------------------------------------------
 */
typedef enum ET_DispMode
{
	ecDispMode_UpVerDwNon = 0,
	ecDispMode_UpVerDwNon2,		/* ��i�FVer�A���i�F�Ȃ� */
	ecDispMode_Up1CHDwClk,		/* ��i�F1CH�A���i�F���v */
	ecDispMode_Up1CHDw2CH,		/* ��i�F1CH�A���i�F2CH */
	ecDispMode_Up1CHDw3CH,		/* ��i�F1CH�A���i�F3CH */
	ecDispMode_UpNonDw2CH,		/* ��i�F�Ȃ��A���i�F2CH */
	ecDispMode_UpNonDw3CH,		/* ��i�F�Ȃ��A���i�F3CH */
	ecDispMode_UpNonDwClk,		/* ��i�F�Ȃ��A���i�F���v */
	ecDispMode_UpNonDwCon,		/* ��i�F�Ȃ��A���i�F�ړ_��� */
	ecDispMode_UpErrDwRtc,		/* ��i�FErr�A���i�FRTC */
	ecDispMode_AllSegOn,		/* �S�Z�O�����gON */
	ecDispMode_AllSegOff,		/* �S�Z�O�����gOFF */
	ecDispMode_Lock,			/* ��i�FLck�A���i�F�Ȃ� */
	ecDispMode_Lock2,			/* ��i�FLck�A���i�F�Ȃ� */
	ecDispMode_OddSeg,			/* ��Z�O�����g */
	ecDispMode_EvenSeg,			/* �����Z�O�����g */
	ecDispMode_UpComDwRssi,		/* ��iComTarget�A���iRSSI */
//	ecDispMode_UpPtDwNon,		/* ��i�FPt�A���i�F�Ȃ� */
	ecDispMode_UpIntDwNon,		/* ��i�Fint�A���i�F�Ȃ� */
	ecDispMode_HsCh,			/* ��i�F'ch�f�A���i�F�����ʐMch */
	ecDispMode_Resf,			/* ��i�F'rst�f�A���i�FRESF */
	ecDispMode_Appid,			/* ��i�F' id�f�A���i�F���Ɖ��ID */
	ecDispMode_Rssi,			/* ��i�F'LEV�f�A���i�FRSSI */
	ecDispMode_Gwid,			/* ��i�F'con�f�A���i�FGW ID */
	ecDispMode_AbnChkErr,		/* ��i�FErr�A���i�F�G���[No. */
	ecDispMode_Ver,				/* ��i�FVer�A���i�FVerNo. */
	ecDispMode_Up1CHDw2CH_V,	/* ��i�F�������iinteger part�j�A���i�F�������idecimal part�j�d�����f�� */
	ecDispMode_Up1CHDw2CH_P,	/* ��i:��̈ʁA���i:��`�S�̈� �p���X���f�� */
	
	ecDispModeMax
} ET_DispMode_t;


typedef enum ET_DispSegSelect
{
	ecDispSegSelect_SegAllOn = 0,
	ecDispSegSelect_SegAllOff,
	ecDispSegSelect_SegOddOn,
	ecDispSegSelect_SegEvenOn,
	
	ecDispSegSelectMax
} ET_DispSegSelect_t;

/*
 *------------------------------------------------------------------------------
 *	�ُ��񍀖�
 *------------------------------------------------------------------------------
 */
typedef enum ET_ErrInfItm
{
	ecErrInfItm_Init = 0,					/* 0:���� */
	ecErrInfItm_BatEmpty,					/* 1:�d�r�c��(��) */
	
	ecErrInfItm_1chOvrRng,					/* 2:�`���l��1�I�[�o�[�����W */
	ecErrInfItm_2chOvrRng,					/* 3:�`���l��2�I�[�o�[�����W */
	ecErrInfItm_3chOvrRng,					/* 4:�`���l��3�I�[�o�[�����W */
	
	ecErrInfItm_1chUdrRng,					/* 5:�`���l��1�A���_�[�����W */
	ecErrInfItm_2chUdrRng,					/* 6:�`���l��2�A���_�[�����W */
	ecErrInfItm_3chUdrRng,					/* 7:�`���l��3�A���_�[�����W */
	
	ecErrInfItm_1chBrnOut,					/* 8:�`���l��1�o�[���A�E�g */
	ecErrInfItm_2chBrnOut,					/* 9:�`���l��2�o�[���A�E�g */
	ecErrInfItm_3chBrnOut,					/* 10:�`���l��3�o�[���A�E�g */
	
	ecErrInfItm_1chSnsShrt,					/* 11:�`���l��1�Z���T�Z�� */
	ecErrInfItm_2chSnsShrt,					/* 12:�`���l��2�Z���T�Z�� */
	ecErrInfItm_3chSnsShrt,					/* 13:�`���l��3�Z���T�Z�� */
	
	ecErrInfItm_SnsComErr,					/* 14:�Z���T�ʐM�ُ� */
	ecErrInfItm_4chCalErr,					/* 15:�`���l��4���Z�G���[ */
	
	ecErrInfItm_RfIcErr,					/* 16:����IC�ُ� */
	ecErrInfItm_PrimMemErr,					/* 17:�v���C�}���������ُ� */
	ecErrInfItm_SecondMemErr,				/* 18:�Z�J���_���������ُ� */
	ecErrInfItm_MiComRomErr,				/* 19:�}�C�R��ROM�ُ� */
	ecErrInfItm_ClkErr,						/* 20:�N���b�N���U�ُ� */
	ecErrInfItm_KeyBehErr,					/* 21:�L�[����s�� */
	ecErrInfItm_ExRtcErr,					/* 22:�O�t��RTC�ُ� */
	ecErrInfItm_InRtcErr,					/* 23����RTC�ُ� */
	ecErrInfItm_BatLow,						/* 24:�d�r�c�ʒቺ */
	
	ecErrInfItm_SnsErr,						/* 25:�Z���T�@��ُ� */

	ecErrInfItmMax							/* enum�ő�l */
} ET_ErrInfItm_t;


typedef enum ET_AbnInfKind
{
	ecAbnInfKind_AbnSts = 0,				/* �@��ُ��� */
	ecAbnInfKind_MeasSts,					/* ����֘A�ُ̈��� */
	
	ecAbnInfKindMax							/* enum�ő�l */
} ST_AbnInfKind_t;


/*
 *------------------------------------------------------------------------------
 *	�v���x�񍀖�
 *------------------------------------------------------------------------------
 */
typedef enum ET_MeasAlmItm
{
	ecMeasAlmItm_Init = 0,			/* ���� */
	ecMeasAlmItm_MeasDev,			/* �v���l��E */
	ecMeasAlmItm_MeasAlmDelayCnt,	/* �v���l�x��(�x���񐔌o��) */
	ecMeasAlmItm_MeasAlmDevTime,	/* �v���l�x��(��E���e���Ԍo��) */

	ecMeasAlmItm_OvRng,
	ecMeasAlmItm_UdRng,
	ecMeasAlmItm_Burn,
	ecMeasAlmItm_Short,

	ecMeasAlmItmMax					/* enum�ő�l */
} ET_MeasAlmItm_t;


/* �x���� */
typedef enum ET_AlmKind
{
	ecAlmKind_HH = 0,				/* ���� */
	ecAlmKind_H,					/* ��� */
	ecAlmKind_L,					/* ���� */
	ecAlmKind_LL,					/* ������ */
	
	ecAlmKind_Max
} ET_AlmKind_t;

/* �x��X�e�[�^�X */
typedef enum ET_AlmSts
{
	ecAlmSts_Non = 0,				/* ������ */
	ecAlmSts_DelayCnt,				/* �x���񐔌v���� */
	ecAlmSts_ActDelayCnt,			/* �x�񔭐���(�x���񐔌o��) */
	ecAlmSts_ActDevTime,			/* �x�񔭐���(��E���e���Ԍo��) */

	ecAlmStsMax
} ET_AlmSts_t;

/* �ߋ��x��t���O */
typedef enum ET_AlmPastSts
{
	ecAlmPastSts_Non = 0,			/* �ߋ��x�񖢔��� */
	ecAlmPastSts_Pre,				/* �ߋ��x�񏀔� */
	ecAlmPastSts_Set,				/* �ߋ��x�񔭐� */
	
	ecAlmPastStsMax
} ET_AlmPastSts_t;

/* �������M�p�x��t���O */
typedef enum ET_AlmFlg
{
	ecAlmFlg_Non = 0,				/* �Ȃ� */
	ecAlmFlg_DevH,					/* �����E */
	ecAlmFlg_AlmH,					/* ����x�� */
	ecAlmFlg_DevHDevHH,				/* �����E������E */
	ecAlmFlg_AlmHDevHH,				/* ����x�������E */
	ecAlmFlg_DevHAlmHH,				/* �����E�����x�� */
	ecAlmFlg_AlmHAlmHH,				/* ����x������x�� */
	ecAlmFlg_DevL,					/* ������E */
	ecAlmFlg_AlmL,					/* �����x�� */
	ecAlmFlg_DevLDevLL,				/* ������E��������E */
	ecAlmFlg_AlmLDevLL,				/* �����x�񉺉�����E */
	ecAlmFlg_DevLAlmLL,				/* ������E�������x�� */
	ecAlmFlg_AlmLAlmLL,				/* �����x�񉺉����x�� */
	
	ecAlmFlgMax
} ET_AlmFlg_t;


/*
 *------------------------------------------------------------------------------
 *	���엚������
 *------------------------------------------------------------------------------
 */
typedef enum ET_ActLogItm
{
	ecActLogItm_Init = 0,				/* ���� */
	ecActLogItm_Reserve1,				/* 1:���g�p */
	ecActLogItm_Boot,					/* 2:�N�� */
	ecActLogItm_BattIn,					/* 3:�d�r���� */
	ecActLogItm_BattBackup,				/* 4:�d�r�����o�b�N�A�b�v��� */
	ecActLogItm_RfStopBatEmpty,			/* 5:������~(�d�r�c�ʋ�) */
	ecActLogItm_RfAirplaneOn,			/* 6:�@�����[�hON */
	ecActLogItm_RfAirplaneOff,			/* 7:�@�����[�hOFF */
	ecActLogItm_RfStopBatLow,			/* 8:������~(�d�r�c��0�{) */
	ecActLogItm_Reserve9,
	ecActLogItm_FrmUpdate,				/* 10:�t�@�[���E�F�A�X�V */
	ecActLogItm_DisSetReq,				/* 11:�s���ݒ�v�� */
	ecActLogItm_Reserve12,
	ecActLogItm_Reserve13,
	ecActLogItm_Reserve14,
	ecActLogItm_Reserve15,
	ecActLogItm_Reserve16,
	ecActLogItm_Reserve17,
	ecActLogItm_Reserve18,
	ecActLogItm_Reserve19,
	
	ecActLogItm_UUDevi1,				/* 20:�����x��l(1ch) */
	ecActLogItm_UDevi1,					/* 21:����x��l(1ch) */
	ecActLogItm_LDevi1,					/* 22:�����x��l(1ch) */
	ecActLogItm_LLDevi1,				/* 23:�������x��l(1ch) */
	ecActLogItm_UUDelay1,				/* 24:�����x��x���l(1ch) */
	ecActLogItm_UDelay1,				/* 25:����x��x���l(1ch) */
	ecActLogItm_LDelay1,				/* 26:�����x��x���l(1ch) */
	ecActLogItm_LLDelay1,				/* 27:�������x��x���l(1ch) */
	ecActLogItm_DeviEnaLv1,				/* 28:��E���e���ԑI��臒l(1ch) */
	ecActLogItm_DeviEnaTime1,			/* 29:��E���e����(1ch) */
	ecActLogItm_Offset1,				/* 30:�v���l�I�t�Z�b�g(1ch) */
	ecActLogItm_Slope1,					/* 31:�v���l�ꎟ�X���␳(1ch) */
	
	ecActLogItm_UUDevi2,				/* 32:�����x��l(2ch) */
	ecActLogItm_UDevi2,					/* 33:����x��l(2ch) */
	ecActLogItm_LDevi2,					/* 34:�����x��l(2ch) */
	ecActLogItm_LLDevi2,				/* 35:�������x��l(2ch) */
	ecActLogItm_UUDelay2,				/* 36:�����x��x���l(2ch) */
	ecActLogItm_UDelay2,				/* 37:����x��x���l(2ch) */
	ecActLogItm_LDelay2,				/* 38:�����x��x���l(2ch) */
	ecActLogItm_LLDelay2,				/* 39:�������x��x���l(2ch) */
	ecActLogItm_DeviEnaLv2,				/* 40:��E���e���ԑI��臒l(2ch) */
	ecActLogItm_DeviEnaTime2,			/* 41:��E���e����(2ch) */
	ecActLogItm_Offset2,				/* 42:�v���l�I�t�Z�b�g(2ch) */
	ecActLogItm_Slope2,					/* 43:�v���l�ꎟ�X���␳(2ch) */
	
	ecActLogItm_UUDevi3,				/* 44:�����x��l(3ch) */
	ecActLogItm_UDevi3,					/* 45:����x��l(3ch) */
	ecActLogItm_LDevi3,					/* 46:�����x��l(3ch) */
	ecActLogItm_LLDevi3,				/* 47:�������x��l(3ch) */
	ecActLogItm_UUDelay3,				/* 48:�����x��x���l(3ch) */
	ecActLogItm_UDelay3,				/* 49:����x��x���l(3ch) */
	ecActLogItm_LDelay3,				/* 50:�����x��x���l(3ch) */
	ecActLogItm_LLDelay3,				/* 51:�������x��x���l(3ch) */
	ecActLogItm_DeviEnaLv3,				/* 52:��E���e���ԑI��臒l(3ch) */
	ecActLogItm_DeviEnaTime3,			/* 53:��E���e����(3ch) */
	ecActLogItm_Offset3,				/* 54:�v���l�I�t�Z�b�g(3ch) */
	ecActLogItm_Slope3,					/* 55:�v���l�ꎟ�X���␳(3ch) */
	
	ecActLogItm_UUDevi4,				/* 56:�����x��l(4ch) */
	ecActLogItm_UDevi4,					/* 57:����x��l(4ch) */
	ecActLogItm_LDevi4,					/* 58:�����x��l(4ch) */
	ecActLogItm_LLDevi4,				/* 59:�������x��l(4ch) */
	ecActLogItm_UUDelay4,				/* 60:�����x��x���l(4ch) */
	ecActLogItm_UDelay4,				/* 61:����x��x���l(4ch) */
	ecActLogItm_LDelay4,				/* 62:�����x��x���l(4ch) */
	ecActLogItm_LLDelay4,				/* 63:�������x��x���l(4ch) */
	ecActLogItm_DeviEnaLv4,				/* 64:��E���e���ԑI��臒l(4ch) */
	ecActLogItm_DeviEnaTime4,			/* 65:��E���e����(4ch) */
	ecActLogItm_Offset4,				/* 66:�v���l�I�t�Z�b�g(4ch) */
	ecActLogItm_Slope4,					/* 67:�v���l�ꎟ�X���␳(4ch) */
	
	ecActLogItm_BatType,				/* 68:�d�r�� */
	ecActLogItm_KeyLock,				/* 69:�L�[���b�N */
	ecActLogItm_RealTimeCh,				/* 70:���A���^�C���ʐM�Ҏ�ch */
	ecActLogItm_LogCyc1,				/* 71:���^����1 */
	ecActLogItm_LogCyc2,				/* 72:���^����2 */
	ecActLogItm_LoggerName1,			/* 73:���K�[����1/10 */
	ecActLogItm_LoggerName2,			/* 74:���K�[����2/10 */
	ecActLogItm_LoggerName3,			/* 75:���K�[����3/10 */
	ecActLogItm_LoggerName4,			/* 76:���K�[����4/10 */
	ecActLogItm_LoggerName5,			/* 77:���K�[����5/10 */
	ecActLogItm_LoggerName6,			/* 78:���K�[����6/10 */
	ecActLogItm_LoggerName7,			/* 79:���K�[����7/10 */
	ecActLogItm_LoggerName8,			/* 80:���K�[����8/10 */
	ecActLogItm_LoggerName9,			/* 81:���K�[����9/10 */
	ecActLogItm_LoggerName10,			/* 82:���K�[����10/10 */
	ecActLogItm_FstConnCh,				/* 83:�����ʐM�Ҏ�ch */
	ecActLogItm_FstConnBoot,			/* 84:�����ʐM�N���̃��O�f�[�^臒l */
	ecActLogItm_AlertType,				/* 85:�x��Ď����Z�� */
	ecActLogItm_AlertParm,				/* 86:�x��Ď����Z�p�����[�^ */
	ecActLogItm_AlertFlag,				/* 87:�x��Ď����Z�t���O�L�^ */
	ecActLogItm_EventKeyFlg,			/* 88:�C�x���g�{�^������ */
	ecActLogItm_DeviClrPoint,			/* 89:��E�����ԃN���A��i */
	ecActLogItm_DeviClrTime,			/* 90:��E���e���ԃN���A���� */
	ecActLogItm_CalDate,				/* 91:�Z���� */
	ecActLogItm_ScalDecimal,			/* 92:�X�P�[�����O�ϊ� �����_�ʒu */
	ecActLogItm_ScalYInt,				/* 93:�X�P�[�����O�ϊ� �ؕ� */
	ecActLogItm_ScalGrad,				/* 94:�X�P�[�����O�ϊ� �X�� */
	ecActLogItm_FlashSelect,			/* 95:�v���C�}���t���b�V���������؂�ւ� */
	ecActLogItm_CnctEnaGwId1,			/* 96:�ڑ����Q�[�g�E�F�CID1 */
	ecActLogItm_CnctEnaGwId2,			/* 97:�ڑ����Q�[�g�E�F�CID2 */
	ecActLogItm_CnctEnaGwId3,			/* 98:�ڑ����Q�[�g�E�F�CID3 */
	ecActLogItm_CnctEnaGwId4,			/* 99:�ڑ����Q�[�g�E�F�CID4 */
	ecActLogItm_CnctEnaGwId5,			/* 100:�ڑ����Q�[�g�E�F�CID5 */
	ecActLogItm_CnctEnaGwId6,			/* 101:�ڑ����Q�[�g�E�F�CID6 */
	ecActLogItm_CnctEnaGwId7,			/* 102:�ڑ����Q�[�g�E�F�CID7 */
	ecActLogItm_CnctEnaGwId8,			/* 103:�ڑ����Q�[�g�E�F�CID8 */
	ecActLogItm_CnctEnaGwId9,			/* 104:�ڑ����Q�[�g�E�F�CID9 */
	ecActLogItm_CnctEnaGwId10,			/* 105:�ڑ����Q�[�g�E�F�CID10 */
	ecActLogItm_GrId,					/* 106:�O���[�vID */
	
	
	ecActLogItmMax						/* enum�ő�l */
} ET_ActLogItm_t;


/*
 *------------------------------------------------------------------------------
 *	�V�X�e�����O����
 *------------------------------------------------------------------------------
 */
typedef enum ET_SysLogItm
{
	ecSysLogItm_Init = 0U,				/* ���� */
	ecSysLogItm_FastCom,				/* 1:�����ʐM */
	ecSysLogItm_KeyDisp,				/* 2:DISP�L�[���� */
	ecSysLogItm_EleWvTstMd,				/* 3:�d�g�e�X�g���[�h */
	ecSysLogItm_RealTmDisCom,			/* 4:�������ʐM�ؒf */
	ecSysLogItm_FlashIcErr,				/* 5:�t���b�V��IC�G���[ */
	ecSysLogItm_RealTmCom,				/* 6:�������ʐM�ڑ� */
	ecSysLogItm_TimeComp,				/* 7:�����␳ */
	ecSysLogItm_IdDiff,					/* 7:ID��������� */

	ecSysLogItmMax						/* enum�ő�l */
} ET_SysLogItm_t;


/*
 *------------------------------------------------------------------------------
 *	�d�r
 *------------------------------------------------------------------------------
 */
/* �d�r�X�e�[�^�X */
typedef enum ET_BattSts
{
	ecBattSts_Init = 0U,			/* �d�r���� */
	ecBattSts_H,					/* �d�r�� */
	ecBattSts_M,					/* �d�r�� */
	ecBattSts_L,					/* �d�r�� */
	ecBattSts_Empty,				/* �d�r�� */
	ecBattSts_Non,					/* �d�r�Ȃ� */
	
	ecBattStsMax
} ET_BattSts_t;

/* �d�r��� */
typedef enum ET_BatType
{
	ecBatType_Init = 0U,
	ecBatType_Alk,					/* �A���J���d�r */
	ecBatType_Li,					/* ���`�E���d�r */
	ecBatType_NiHM,					/* �j�b�P�����f�d�r */
	ecBatType_Large,				/* ��e�ʃo�b�e�� */
	
	ecBatTypeMax
} ET_BatType_t;

/* �d�r������� */
typedef enum ET_BattInt
{
	ecBattInt_Init = 0U,
	ecBattInt_Low,					/* ��d���������� */
	ecBattInt_LowLoop,				/* ��d�������̌J�Ԃ�AD */
	ecBattInt_High,					/* �d��L��H�������� */
	ecBattInt_NonHigh,				/* �d�r�Ȃ�����̓d��H�������� */
	ecBattInt_LowFlashWr,			/* ��d��Lo��Flash���C�g���O */
	ecBattInt_LowFlashWrFin,		/* ��d��Lo��Flash���C�g���O��AD���� */
	
	ecBattIntMax
} ET_BattInt_t;

typedef enum ET_AdcPhase
{
	ecAdcPhase_Init = 0,
	ecAdcPhase_CtlWait,
	ecAdcPhase_AdExe,

	ecAdcPhaseMax
} ET_AdcPhase_t;

/*
 *------------------------------------------------------------------------------
 *	�H���p�X�e�[�^�X
 *------------------------------------------------------------------------------
 */
/* �����`���l���I�� */
typedef enum ET_SelRfCh
{
	ecSelRfCh_CurrentCh_LoRa = 0,
	ecSelRfCh_CurrentCh_FSK,
	ecSelRfCh_HsCh,
	ecSelRfCh_LoraChGrupe,

	ecSelRfChMax
} ET_SelRfCh_t;

/*
 *------------------------------------------------------------------------------
 *	�e�@�\�̓����ԑJ��
 *------------------------------------------------------------------------------
 */

/* ������ */
typedef enum ET_MeaModuleSts
{
	ecMeaModuleSts_Init = 0,
	ecMeaModuleSts_Run,
	ecMeaModuleSts_Wait,
	ecMeaModuleSts_Sleep,

	ecMeaModuleStsMax
} ET_MeaModuleSts_t;

/* RTC��� */
typedef enum ET_RtcIntSts
{
	ecRtcIntSts_Init = 0,
	ecRtcIntSts_Run,
	ecRtcIntSts_Sleep,
	
	ecRtcIntStsMax
} ET_RtcIntSts_t;


/* ����Flash�����ݎ��s��� */
typedef enum ET_InFlashWrExeSts
{
	ecInFlashWrExeSts_Init = 0,
	ecInFlashWrExeSts_Run,
	ecInFlashWrExeSts_Sleep,
	
	ecInFlashWrExeStsMax
} ET_InFlashStsWrExe_t;


/* �����ԑJ�� */
typedef enum ET_ExeModuleSts
{
	ecExeModuleSts_Init = 0,				/* ���� */
	ecExeModuleSts_Meas,					/* ���� */
	ecExeModuleSts_Log1,					/* ���^1 */
	ecExeModuleSts_Log2,					/* ���^2 */
	ecExeModuleSts_Alm1,					/* �x��1 */
	ecExeModuleSts_Alm2,					/* �x��2 */
	ecExeModuleSts_ExFlsWr1,				/* �O�t���t���b�V����������1 */
	ecExeModuleSts_Sleep,					/* �X���[�v */

	ecExeModuleStsMax
} ET_ExeModuleSts_t;

/* �L�[�����ݏ�� */
typedef enum ET_KeyIntModuleSts
{
	ecKeyIntModuleSts_Init = 0,				/* ���� */
	ecKeyIntModuleSts_ChatEventMode,		/* Event�L�[���͂ɂ��`���^�����O���蒆 */
	ecKeyIntModuleSts_ChatDispMode,			/* Display�L�[���͂ɂ��`���^�����O���蒆 */
	ecKeyIntModuleSts_ChatResetMode,		/* Reset�L�[���͂ɂ��`���^�����O���蒆 */
	ecKeyIntModuleSts_EventMode,			/* Event�L�[1�b�������ɂ��C�x���g���s */
	ecKeyIntModuleSts_DispMode,				/* Disp�L�[�P�����ɂ��A�\���؂�ւ����s */
	ecKeyIntModuleSts_RfMode,				/* Disp�L�[1�b�������ɂ�閳�����[�h�؂�ւ����s */
	ecKeyIntModuleSts_ResetMode,			/* Reset�L�[1�b�������ɂ��o�׏�ԏ��������s */
//	ecKeyIntModuleSts_RtmSndPtResetDspMode,	/* Reset�L�[2�b�������ɂ�郊�A���^�C���ʐM�|�C���^�������\�����s */
//	ecKeyIntModuleSts_RtmSndPtResetMode,	/* Reset�L�[2�b�������ɂ�郊�A���^�C���ʐM�|�C���^���������s */
	ecKeyIntModuleSts_Sleep,
	
	ecKeyModuleStsMax
} ET_KeyIntModuleSts_t;


/* Event�L�[��� */
typedef enum ET_EventKeyModuleSts
{
	ecEventKeyModuleSts_Init = 0,			/* ���� */
	ecEventKeyModuleSts_Meas,
	ecEventKeyModuleSts_Alm,
	ecEventKeyModuleSts_ExFlsWr,
	ecEventKeyModuleSts_Sleep,
	
	ecEventKeyModuleStsMax
} ET_EventKeyModuleSts_t;


/* �d�rAD��� */
typedef enum ET_BatModuleSts
{
	ecBatModuleSts_Init = 0,
	ecBatModuleSts_Run,
	ecBatModuleSts_Wait,
	ecBatModuleSts_Sleep,

	ecBatModuleStsMax
} ET_BatModuleSts_t;


/* UART�ʐM��� */
typedef enum ET_ComModuleSts
{
	ecComModuleSts_Init = 0,
	ecComModuleSts_Run,
	ecComModuleSts_Wait,
	ecComModuleSts_Sleep,

	ecComModuleStsMax
} ET_ComModuleSts_t;



/* RF�ʐM��� */
typedef enum ET_RfModuleSts
{
	ecRfModuleSts_Init = 0,
	ecRfModuleSts_Run,
	ecRfModuleSts_Sleep,

	ecRfModuleStsMax
} ET_RfModuleSts_t;


/* RTC���䊄�荞�ݏ�� */
typedef enum ET_RtcModuleSts
{
	ecRtcModuleSts_Init = 0,
	ecRtcModuleSts_Run,
	ecRtcModuleSts_Wait,
	ecRtcModuleSts_Update,
	ecRtcModuleSts_UpdateGain,
	ecRtcModuleSts_UpdateLose,
	ecRtcModuleSts_Sleep,

	ecRtcModuleStsMax
} ET_RtcModuleSts_t;


/* �O�t��Flash��� */
typedef enum ET_ExFlashModuleSts
{
	ecExFlashModuleSts_Init = 0,
	ecExFlashModuleSts_Run,
	ecExFlashModuleSts_Sleep,

	ecExFlashModuleStsMax
} ET_ExFlashModuleSts_t;


/* �t�������� */
typedef enum ET_LcdModuleSts
{
	ecLcdModuleSts_Init = 0,
	ecLcdModuleSts_Run,
	ecLcdModuleSts_Sleep,

	ecLcdModuleStsMax
} ET_LcdModuleSts_t;



/* �O�t��Flash���[�h��� */
typedef enum ET_ExFlashRdModuleSts
{
	ecExFlashRdModuleSts_Init = 0,
	ecExFlashRdModuleSts_Run,
	ecExFlashRdModuleSts_Sleep,
	
	ecExFlashRdModuleStsMax
} ET_ExFlashRdModuleSts_t;






/* �����t���b�V�������݃X�e�[�^�X */
typedef enum ET_InFlashWrSts
{
	ecInFlashWrSts_ParmInit = 0,		/* �ݒ�l�̂ݏ����l�Ń��C�g */
	ecInFlashWrSts_Process,				/* �H���v���Z�X�A�ݒ�l�������l�Ń��C�g */
	ecInFlashWrSts_Process2,			/* �H���v���Z�X�A�ݒ�l�������l�Ń��C�g */
	ecInFlashWrSts_ParmNow,				/* �����_RAM�ϐ������C�g */

	ecInFlashWrStsMax
} ET_InFlashWrSts_t;


typedef enum ET_LowBatFlashSts
{
	ecLowBatFlashSts_Init = 0U,			/* �ʏ� */
	ecLowBatFlashSts_Now,														/* ��d���� */

	ecLowBatFlashStsMax															/* enum�ő�l */
} ET_LowBatFlashSts_t;


/* �����i�[��� */
typedef enum ET_HistorySts
{
	ecHistoryModuleSts_Init = 0,
	ecHistoryModuleSts_Run,
	ecHistoryModuleSts_Sleep,

	ecHistoryModuleStsMax
} ET_HistorySts_t;

/* �t�@�[���A�b�v��� */
typedef enum ET_FirmupSts
{
	ecFirmupModuleSts_Init = 0,
	ecFirmupModuleSts_Pre,
	ecFirmupModuleSts_Run,
	ecFirmupModuleSts_Sleep,

	ecFirmupModuleStsMax
} ET_FirmupSts_t;



#endif																			/* INCLUDED_ENUM_H */
