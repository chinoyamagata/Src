/*
 *******************************************************************************
 *	File name	:	glRam.h
 *
 *	[���e]
 *		�O���[�o���ϐ���`�t�@�C��
 *	[���L]
 *
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2017.11.08		Softex N.I		�V�K�쐬
 *******************************************************************************
 */

#include	"typedef.h"										/* �f�[�^�^��` */
#include	"enum.h"										/* �񋓌^�̒�` */
#include	"struct.h"										/* �\���̒�` */
#include	"switch.h"

/* ROM�o�[�W���� */
extern const uint8_t	cRomVer[ 4U ];

/* ROM�}�� */
//extern const uint8_t	cRomFigNo;

/* �Z���T�� */
//extern const ET_SensType_t	cSensType;


extern ET_Mode_t	gvMode;									/* ���[�h */

/* �t�@�[���A�b�v���[�h */
extern uint8_t gvFirmupMode;

/* ���� */
extern ST_RTC_t		gvClock;								/* RTC�f�[�^ */
extern uint32_t		gvLocalTime;							/* ���[�J������ */

/* MODBUS�ʐM */
extern ST_ModInf_t	gvModInf;								/* MODBUS�ʐM��� */


/* �O�t���t���b�V�������� */
extern uint8_t		gvCsi10SndEndFlg;						/* CSI10���M�����t���O */

/* ���A���^�C���N���b�N */
extern uint8_t		gvIicSndEndFlg;							/* IIC���M�����t���O */
extern uint8_t		gvIicRcvEndFlg;							/* IIC��M�����t���O */

/* ���菈�� */
//extern ET_MeasPhase_t	gvMeasPhase;						/* ���菈���t�F�[�Y */
//extern ST_MeasTmCnt_t	gvMeasRefTmCnt;						/* ���t�@�����X�p�^�C�}�J�E���g */
//extern ST_MeasTmCnt_t	gvMeasThTmCnt;						/* �T�[�~�X�^�p�^�C�}�J�E���g */
extern ST_MeasPrm_t	gvMeasPrm;								/* ���菈���p�p�����[�^ */
extern uint8_t	gvHsAlmFlag[ 4U ];

/* �x���� */
extern ET_AlmSts_t gvAlmSts[ imChannelNum ][ imAllAlmNum ];	/* �x���� */

/* �x���E�N���A�_�@ */
//extern uint8_t		gvAlmClrPoint;						/* �x���E�N���A�_�@ */

/* Hmi */
extern ST_DispSts_t	gvDispMode;								/* �\�����[�h */
extern uint16_t gvLcdComGwId;								/* �ڑ�GWID�̉�3��(LCD�\���p) */


/* ���荞�� */
extern uint32_t		gvRecCycRtcTm;							/* ���^�������B���̓���RTC�̎��� */

/* �d�r */
extern uint8_t		gvAdConvFinFlg;							/* �d�rAD���芮���t���O */
extern ET_LowBatFlashSts_t		gvBatLowSts;				/* ��d���p��� */
extern ET_BattInt_t				gvBatt_Int;					/* �d�r�ቺ������� */

extern ST_BatAd_t				gvBatAd;					/* �d�r��� */


/* �������|�[�g���荞�݃t���O */
extern ST_RF_INT_t	gvRfIntFlg;								/* ����IC DIO���荞�݃t���O */

/* ���W���[������X�e�[�^�X */
extern	ST_ModuleSts_t	gvModuleSts;

/* �����^�C�~���OTimer Counter */
extern uint16_t gvRfTimingCounterL;
extern uint16_t gvRfTimingCounterH;
extern uint8_t gvRfTimingTempCorrect;						/* gvRfTimingCounter�̉��x�␳ ppm(�}�C�i�X) */
extern uint8_t gvRfTimingCorrectCnt;						/* gvRfTimingCounter�̉��x�␳�����s����b�J�E���^ 3sec���ƂɎ��s */
extern ST_RF_StsFlag_t gvrfStsEventFlag;;
extern uint16_t vrfITSyncCount;
extern uint8_t vrfRTmodeCommFailNum;
extern uint16_t vrfTest;

extern sint8_t		gvInTemp;								/* �}�C�R������̓������x */


/* �������M�f�[�^ */
extern ST_RTMeasData_t gvRfRTMeasData;						/* �v���f�[�^�̃��A���^�C�����M���e */
extern ST_RTMeasAlm2Data_t gvRfRTMeasAlm2Data;				/* �v������2�̒��������M���e */

extern uint16_t gvFstConnBootCnt;							/* �����ʐM�N���̂��߂̎��^�f�[�^�J�E���^ */


/* ��E���e���ԃN���A�p�����[�^ */
extern	uint8_t		gvDeviEnaFlg;

/* ����Flash�i�[�p�����[�^ */
extern ST_InFlashTbl_t			gvInFlash;

extern ST_ActQue_t gvActQue;								/* �ݒ�ύX�����̈ꎟ�i�[�L���[ */

extern uint8_t gvCycTestStart;								/* �N���b�N���U�̎��Ȑf�f�J�n */

extern ET_RtcSts_t 				gvRtcSts;					/* �����␳�X�e�[�^�X */

extern uint16_t	gvMeasAlmNum;								/* �v���x�񔭐�/������ */
extern uint8_t gvHsMeasAlmFlg;								/* �v���x��t���O */

extern uint16_t gvFlashReadIndex[ 2U ];


/* debug */
#if swRfTestLcd == imEnable
extern uint8_t	gvRfTestLcdFlg;
#endif

#if (swRssiLogDebug == imEnable)
extern uint8_t gvDebugRssi;
#endif

#if (swRESFDisp == imEnable)
extern uint8_t gvResf;
#endif

#if (swAdCntLog == imEnable)
extern uint8_t gvAdCnt_RfFlg;
#endif


