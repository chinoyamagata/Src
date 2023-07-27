/*
 *******************************************************************************
 *	File name	:	glRam.c
 *
 *	[���e]
 *		�O���[�o���ϐ���`�t�@�C��
 *	[���L]
 *
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2019.11
 *******************************************************************************
 */


#include "typedef.h"							/* �f�[�^�^��` */
#include "enum.h"								/* �񋓌^�̒�` */
#include "struct.h"								/* �\���̒�` */
#include "UserMacro.h"
#include "switch.h"

/* ROM�o�[�W���� */
const uint8_t cRomVer[ 4U ] = { 1U, 3U, 5U, 0U };			/* 0:���W���[Ver�A1:�}�C�i�[Ver�A2:�}�C�i�[Ver�A3:�B��Ver */

/* ROM�}�� */
//const uint8_t cRomFigNo = 1U;

/* �Z���T�� */
//const ET_SensType_t	cSensType = ecSensType_ExTh;

/* ���[�h */
ET_Mode_t gvMode = ecMode_Normal;

/* �t�@�[���A�b�v���[�h */
uint8_t gvFirmupMode = imOFF;								/* �t�@�[���A�b�v���s���(on:���s���Aoff:�����s) */

/* ���� */
ST_RTC_t gvClock = { 20U, 1U, 1U, 0U, 0U, 0U, 0U };			/* RTC�f�[�^ */
uint32_t gvLocalTime = 0U;									/* ���[�J������ */
ET_RtcSts_t gvRtcSts = ecRtcSts_Non;						/* �����␳�X�e�[�^�X */

uint32_t gvRecCycRtcTm;										/* ���^�������B���̓���RTC�̎��� */

/* MODBUS�ʐM */
ST_ModInf_t gvModInf;										/* MODBUS�ʐM��� */

uint8_t gvCsi10SndEndFlg;									/* �O�t��Flash�ʐMCSI10���M�����t���O */
uint8_t gvIicSndEndFlg;										/* �O�t��RTC�ʐMIIC���M�����t���O */
uint8_t gvIicRcvEndFlg;										/* �O�t��RTC�ʐMIIC��M�����t���O */

/* ���菈�� */
//ET_MeasPhase_t gvMeasPhase = ecMeasPhase_Init;			/* ���菈���t�F�[�Y */
//ST_MeasTmCnt_t gvMeasRefTmCnt = { 0xFFFFU, 0xFFFFU };		/* ���t�@�����X�p�^�C�}�J�E���g */
//ST_MeasTmCnt_t gvMeasThTmCnt = { 0xFFFFU, 0xFFFFU };		/* �T�[�~�X�^�p�^�C�}�J�E���g */
ST_MeasPrm_t gvMeasPrm;										/* ���菈���p�p�����[�^ */

/* �x�� */
ET_AlmSts_t gvAlmSts[ imChannelNum ][ imAllAlmNum ] = 
{
	ecAlmSts_Non, ecAlmSts_Non, ecAlmSts_Non, ecAlmSts_Non,
	ecAlmSts_Non, ecAlmSts_Non, ecAlmSts_Non, ecAlmSts_Non,
	ecAlmSts_Non, ecAlmSts_Non, ecAlmSts_Non, ecAlmSts_Non,
	ecAlmSts_Non, ecAlmSts_Non, ecAlmSts_Non, ecAlmSts_Non
};

uint8_t gvHsAlmFlag[ 4U ];									/* �ŐV�x��t���O */
//uint8_t gvAlmClrPoint = 0U;								/* �x���E�N���A�_�@ */
uint16_t gvMeasAlmNum = 0U;									/* �v���x�񔭐�/������ */
uint8_t gvHsMeasAlmFlg = imOFF;								/* �v���x��t���O */

/* �\�� */
ST_DispSts_t gvDispMode = 
{ ecDispMode_UpVerDwNon, ecDispMode_UpVerDwNon };			/* �\�����[�h */

uint16_t gvLcdComGwId;										/* �ڑ�GWID�̉�3��(LCD�\���p) */

/* �d�r */
uint8_t gvAdConvFinFlg;										/* �d�rAD���芮���t���O */
ET_LowBatFlashSts_t gvBatLowSts = ecLowBatFlashSts_Init;	/* ��d���p��� */
ET_BattInt_t gvBatt_Int= ecBattInt_Init;					/* �d�r�ቺ������� */
ST_BatAd_t gvBatAd;											/* �d�r��� */

/* ���� */
ST_RF_INT_t gvRfIntFlg;										/* ����IC DIO���荞�݃t���O */

uint16_t gvRfTimingCounterL;
uint16_t gvRfTimingCounterH;
uint8_t gvRfTimingTempCorrect;								/* gvRfTimingCounter�̉��x�␳ ppm(�}�C�i�X) */
uint8_t gvRfTimingCorrectCnt;								/* gvRfTimingCounter�̉��x�␳�����s����b�J�E���^ 3sec���ƂɎ��s */
ST_RF_StsFlag_t gvrfStsEventFlag;
uint16_t vrfITSyncCount;									/* GW�Ƃ̃^�C�~���O�����p��Tua�J�E���^ */
uint8_t vrfRTmodeCommFailNum;
uint16_t vrfTest;

sint8_t gvInTemp = 25;										/* �}�C�R������̓������x(�����\��) */

/* �������M�f�[�^ */
ST_RTMeasData_t gvRfRTMeasData;								/* �v���f�[�^�̒��������M���e */
ST_RTMeasAlm2Data_t gvRfRTMeasAlm2Data;						/* �v������2�̒��������M���e */

uint16_t gvFstConnBootCnt;									/* �����ʐM�N���̂��߂̎��^�f�[�^�J�E���^ */

/* ���W���[������X�e�[�^�X */
ST_ModuleSts_t gvModuleSts = 
{
	ecRtcIntSts_Sleep,
	ecExeModuleSts_Sleep,
	ecMeaModuleSts_Run,
	ecKeyIntModuleSts_Sleep,
	ecEventKeyModuleSts_Sleep,
	ecBatModuleSts_Run,
	ecComModuleSts_Run,
	ecRfModuleSts_Run,
	ecRtcModuleSts_Sleep,
	ecExFlashModuleSts_Sleep,
	ecLcdModuleSts_Run,
	imErrChk_Sleep,
	ecExFlashRdModuleSts_Sleep,
	ecInFlashWrExeSts_Sleep,
	ecHistoryModuleSts_Sleep,
	ecFirmupModuleSts_Sleep
};


/* ����Flash�i�[�p�����[�^ */
ST_InFlashTbl_t gvInFlash;									/* ����Flash�i�[���ʕϐ� */

ST_ActQue_t gvActQue;										/* �ݒ�ύX�����̈ꎟ�i�[�L���[ */

uint8_t gvCycTestStart;										/* �N���b�N���U�̎��Ȑf�f�J�n */

uint16_t gvFlashReadIndex[ 2U ];							/* Flash���烊�[�h�������^�f�[�^�̐擪IndexNo. */


/* �f�o�b�O�p�ϐ� */
#if swRfTestLcd == imEnable
uint8_t	gvRfTestLcdFlg = 1;
#endif

#if (swRssiLogDebug == imEnable)
uint8_t gvDebugRssi;
#endif

#if (swRESFDisp == imEnable)
uint8_t gvResf;
#endif

#if (swAdCntLog == imEnable)
uint8_t gvAdCnt_RfFlg = imOFF;
#endif

