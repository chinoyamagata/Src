/*
 *******************************************************************************
 *	File name	:	RFDrv.c
 *
 *	[���e]
 *		�����ʐM����
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2020.9.10
 *******************************************************************************
 */
#include <string.h>
#include <stdlib.h>

#include "cpu_sfr.h"															/* SFR��`�w�b�_�[ */
#include "typedef.h"															/* �f�[�^�^��` */
#include "UserMacro.h"															/* ���[�U�[�쐬�}�N����` */
#include "immediate.h"															/* immediate�萔��` */
#include "switch.h"																/* �R���p�C���X�C�b�`��` */
#include "enum.h"																/* �񋓌^�̒�` */
#include "struct.h"																/* �\���̒�` */
#include "func.h"																/* �֐��̊O���Q��(extern)��` */
#include "glLabel.h"															/* �O���[�o���ϐ��E�萔(const)���x����` */

#include "RF_Struct.h"
#include "RF_Enum.h"
#include "RF_Const.h"
#include "RF_Immediate.h"
#include "sx1272regs-Fsk.h"
#include "sx1272regs-LoRa.h"


/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
 */
void SubIntervalTimer_Sleep( uint16_t wkTimerCount, uint16_t wkLoopCount );
void SubIntervalTimer_Stop( void );

/* �������֘A */
void SubSX1272POR( void );
void SubSX1272Reset( uint8_t arRstExe );
void SubSX1272InitReg( void );

/* �����A�v�����C�� */
void ApiRfDrv_TimingCorrect( void );
sint8_t ApiRfDrv_Temp( void );
#if (swLoggerBord == imEnable)
void SubRfDrv_RtConctTiAdd( void );
#else
void SubRfDrv_RtConctCut( void );
#endif

/* �����ʐM���C�� */
/* ���[�h���� */
uint8_t SubRFDrv_CarrierSens_FSK( uint16_t arSensTime );
uint8_t SubRFDrv_CarrierSens_5ch( uint8_t arCh );
uint8_t SubRfDrv_CarrierSens_Rt_RetryDecision( uint16_t arWaitTime, uint8_t arChChange, uint8_t arMode );
void SubRfDrv_PreBCH( uint8_t *wkDataIn, uint16_t *wkDataOut);
void SubRfDrv_MakeBCH(uint8_t *arCulcData, uint8_t *arBchOut, uint8_t arDatatNum);
#if 0
void SubRfDrv_Interleave( uint16_t *wkDataIn, uint8_t *wkDataOut );
#endif
uint8_t SubRfDrv_Coding_HS(uint8_t *arDataIn, uint16_t arLength, uint8_t arBchOption );
void SubRfDrv_Crc_Coding(uint8_t *wkDataIn, uint8_t wkLength);
uint8_t SubRfDrv_Crc_Decoding(uint8_t *wkDataIn, uint8_t wkLength);
//uint8_t SubRfDrv_Decoding_Measure(uint8_t *arCulcData);
uint8_t SubRfDrv_Decoding_Bch(uint8_t *arCulcData, uint16_t arOffset, uint8_t arDataNumber);
void SubRfDrv_PwChange( uint8_t arPwMode );
uint8_t SubRfDrv_GetRssi( void );
void SubRfDrv_RfReset( void );

/* FSK */
//uint8_t SubSX1272CommandSendFSK( PayloadStruct_t *FSK_CommandLength, uint8_t CommandCount, uint8_t CommandSetPointer, uint8_t CommandKind, uint8_t wkCH );
#if (swKouteiMode == imEnable)
void SubRFDrv_PN9FSK( uint8_t wkCH );
#endif
void SubRFDrv_SyncSet( void );
uint8_t SubRfDrv_Crc_Decoding_HS(uint8_t *wkDataIn, uint16_t wkLength, uint8_t arBch );
uint8_t	SubRFDrv_Hsmode_BcnPktCodeCheck(void);

void SubRfDrv_Coding_Bch(uint8_t *arDataIn, uint16_t arBchOffset, uint16_t arDataNumber);

#if (swLoggerBord == imEnable)
uint8_t SubRFDrv_Wakeup_Main( uint8_t *vrfRssi, uint8_t wkCH );
void SubRfDrv_Wakeup_Cycle( void );			/* �������[�h�ďo�M�����m�J��Ԃ����� */
uint8_t SubRFDrv_Wakeup_PreDetect(void);
uint8_t SubRFDrv_Wakeup_SyncDetect( void );
uint16_t SubRfDrv_CallResponseTimeslot( uint8_t *wkTimeSlotGrupe, uint8_t *wkTimeSlotNum);
//void SubRFDrv_CallResponseMake( void );
//void SubRfDrv_Make_Broadcast_Responce( void );
//void SubRfDrv_Make_Measure_Main_Responce( uint8_t arContinue, uint8_t arChNum );
void SubRFDrv_Hsmode_Logger_ResponceDataSet( uint8_t arPacket );
static void SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( uint8_t arNum );
static void SubRfDrv_SetStatusAdd( uint8_t *parBuffer );
//void SubRfDrv_Coding_Measure(uint8_t *arDataIn );
void SubRfDrv_Header_Cording( uint8_t arLength, uint8_t arBchOption );
void SubRfDrv_Payload_Cording( uint8_t arPayloadLen, uint8_t arBchFrameNum, uint8_t arCrcFrameNum, uint8_t arHeaderNum );
void SubRfDrv_Hsmode_Logger_BeaconRxPrcss( void );
static void SubRfDrv_Hsmode_Logger_BeaconRxAnalysis( void );
static void SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Sub( ST_BeaconRxAnalysis_t arAnalysis );
static void SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Continue( void );
static void SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Continue_Sub( uint16_t arKosu, ET_RegionKind_t arRegion );
static void SubRfDrv_Hsmode_Logger_SndPtSet( void );
//void SubRFDrv_Hsmode_Logger_ResPcktPointerSet( void );
static void SubRFDrv_Hsmode_PcktPointerSet( void );
void SubRFDrv_Hsmode_Lg_To_Gw_AckDataSet( void );
void SubRFDrv_Hsmode_End(void);
ET_RfDrv_ResInfo_t SubRFDrv_Filter_Gwid( uint8_t *arGwid );
//static void SubRFDrv_Hsmode_SleepFin( void );
static ET_RfDrv_ResInfo_t SubRfDrv_FirmStore( ST_RF_Gw_Hex_Tx_t *arRfBuff );
static void SubRfDrv_Hsmode_RptTx( void );
void SubRfDrv_Rtmode_CnctEnd( void );
#else
void SubRfDrv_Hsmode_ResRx_AfterWait( void );
void SubRFDrv_Hsmode_Logger_BcnAckDataSet( void );
ET_RfDrv_ResInfo_t SubRfDrv_Header_Decryption( uint8_t arLength );
ET_RfDrv_ResInfo_t SubRfDrv_Payload_Decryption( uint8_t arPayloadLen, uint8_t arBchFrameNum, uint8_t arCrcFrameNum, uint8_t arHeaderNum );
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_MeasHist( uint8_t arLoop );
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_SubModDatSet( void );
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_SetRead( uint8_t arLoop );
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_SetWrite( uint8_t arLoop );
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_ReqFirm( uint8_t arLoop );
//uint8_t SubRFDrv_CallResponseRecieve( uint16_t wkRxLength, uint8_t *vrfRssi, uint8_t wkCH );
//uint8_t SubRFDrv_Call_FSK_Main( uint8_t *wkCallID, uint8_t wkCommand, ET_RfDrvMainGwSts_t arSts, uint16_t wkCH, uint16_t wkResCH, uint8_t *arResSlotCnt );
#endif

/* LoRa */
#if (swKouteiMode == imEnable)
void SubRFDrv_PN9LoRa( uint8_t wkCH );
#endif
void SubRFDrv_LoRa_Tx_Setting(uint8_t arPayLoadLength, uint8_t arPreambleLength, uint8_t arTxMode);
void SubRFDrv_LoRa_Rx_Start(uint8_t arPayLoadLength, uint8_t arPreambleLength, uint8_t arRxMode);
void SubRFDrv_LoRa_Rx_Read(uint8_t *arRxLength, uint8_t *arRssi);

/* SX1272���� */
/* ���[�h���� */
void SubSX1272Sleep( void );
void SubSX1272Stdby( void );
void SubSX1272Tx(uint16_t Length);

void SubSX1272Write( uint8_t addr, uint8_t tx_data );
uint8_t SubSX1272Read( uint8_t addr );
void SubSX1272ReadBuffer( uint8_t addr, uint8_t *rx_buffer, uint8_t size );
void SubSX1272WriteBuffer( uint8_t addr, uint8_t *tx_buffer, uint8_t size );
void SubSX1272WriteFifo( uint8_t *tx_buffer, uint8_t size );
void SubSX1272SetModem( RadioModems_t modem );
#if 0
void SubSX1272RegReadAll( void );
#endif
void SubSX1272FreqSet( uint8_t ChNo, sint16_t Freq_Offset);

/* FSK */
void SubSX1272RxFSK_DIO( void );
void SubSX1272CarrierSenseFSK_DIO( void );
void SubSX1272RxFSK_On( void );
void SubSX1272FSK_Length( uint16_t wkDataLength );
void SubSX1272RxFSK( uint16_t wkDataLength );
void SubSX1272TxFSK_DIO_240kbps(void);
void SubSX1272FSK_Sync( uint8_t *wkSyncWord);
void SubSX1272FSK_Preamble( uint16_t wkPreambleLength );
uint8_t SubSX1272TxFSK( uint16_t arDataLength );

/* LoRa */
void SubSX1272TxLoRa_DIO( void );
void SubSX1272RxLoRa_DIO( void );
void SubSX1272RxLoRa_Continu( void );
void SubSX1272CadLoRa_DIO( void );
void SubSX1272CadLoRa( void );


#if (swKouteiMode == imEnable)
/* �H���p������M���s */
static void SubRfDrv_TestRx( void );
static void SubRfDrv_TestCurrentRx( void );
#endif

#if (swLoggerBord == imEnable)
/* �����p�v���g�R���Ɍx��x���l���������� */
static void SubRfDrv_SetAlmDelayArray( uint8_t *arFromAlmParmArray, uint8_t *arToAlmParmArray );
/* �����p�v���g�R���Ɉ�E���e���Ԃ��������� */
static void SubRfDrv_SetDevi( uint8_t *arToAlmParmArray );
/* ���A���^�C���ڑ����Z�b�g */
void SubRfDrv_SetRtConInfo( uint8_t arSel );
/* ������M�v���f�[�^��Flash���烊�[�h */
static void SubRfDrv_RtmodeMeasDataFlashRead( uint8_t arSel );
static void SubRfDrv_ChgRealTimeSndPt( void );
/* ������M�v���f�[�^��Flash���烊�[�h���v�Z */
static void SubRfDrv_RtmodeDataFlashReadKosu( void );
/* ������M�v���f�[�^�Z�b�g�F���A���^�C�����[�h */
static void SubRfDrv_RTmodeMeasDataSet( uint8_t arSel );
//static void SubRfDrv_RTmodeAuditDataSet( void );

/* �v���l1�̉ߋ����M�|�C���^���X�V */
static void SubRfDrv_RtmodeMeas1OldPtrUpdate( void );
static void SubRfDrv_RtmodeMeasAlmOldPtrUpdate( uint16_t arKosu );

#endif

#if (swRssiLogDebug == imEnable)

void SubRfDrv_RssiLogDebug( void );
static void SubRFDrv_CalFstConnBootCnt( void );
#endif



/*
 *==============================================================================
 *	�萔��`
 *==============================================================================
 */
#if (swLoggerBord == imEnable)
const ST_BeaconRxAnalysis_ContinueTbl_t cBeaconRxAnalysis_ContinueTbl[ 12U ] =
{
	/* mReqCmdCode,						mKosu,	mRegionKind */
	{ ecRfDrvGw_Hs_ReqCmd_Status,		0U,		ecRegionKind_Non },
	{ ecRfDrvGw_Hs_ReqCmd_Toroku,		0U,		ecRegionKind_Non },
	{ ecRfDrvGw_Hs_ReqCmd_Measure,		90U,	ecRegionKind_Meas1 },
	{ ecRfDrvGw_Hs_ReqCmd_AlartHis,		100U,	ecRegionKind_MeasAlm },
	{ ecRfDrvGw_Hs_ReqCmd_AbnormalHis,	100U,	ecRegionKind_ErrInfo },
	{ ecRfDrvGw_Hs_ReqCmd_EventHis,		60U,	ecRegionKind_Event },
	{ ecRfDrvGw_Hs_ReqCmd_OpeHis,		90U,	ecRegionKind_ActLog },
	{ ecRfDrvGw_Hs_ReqCmd_SysHis,		90U,	ecRegionKind_SysLog },
	{ ecRfDrvGw_Hs_ReqCmd_SetRead,		0U,		ecRegionKind_Non },
	{ ecRfDrvGw_Hs_ReqCmd_SetWrite,		0U,		ecRegionKind_Non },
	{ ecRfDrvGw_Hs_ReqCmd_ReqFirm,		0U,		ecRegionKind_Non },
	{ ecRfDrvGw_Hs_FirmHex,				0U,		ecRegionKind_Non }
};

const uint8_t wkRadioTable[ 10U ] = { 37U, 38U, 45U, 53U, 60U, 63U, 70U, 78U, 85U, 88U };

#else
const ST_BcnAckDataSetTbl_t cBcnAckDataSetTbl[ 9U ] =
{
	/* mReqCmdCode,						mPacketCode,	mFnc		*/
	{ ecRfDrvGw_Hs_ReqCmd_Measure,		0x0E,			&SubRFDrv_Hsmode_Logger_BcnAckDataSet_MeasHist	},
	{ ecRfDrvGw_Hs_ReqCmd_AlartHis,		0x0F,			&SubRFDrv_Hsmode_Logger_BcnAckDataSet_MeasHist	},
	{ ecRfDrvGw_Hs_ReqCmd_AbnormalHis,	0x15,			&SubRFDrv_Hsmode_Logger_BcnAckDataSet_MeasHist	},
	{ ecRfDrvGw_Hs_ReqCmd_EventHis,		0x16,			&SubRFDrv_Hsmode_Logger_BcnAckDataSet_MeasHist	},
	{ ecRfDrvGw_Hs_ReqCmd_OpeHis,		0x17,			&SubRFDrv_Hsmode_Logger_BcnAckDataSet_MeasHist	},
	{ ecRfDrvGw_Hs_ReqCmd_SysHis,		0x18,			&SubRFDrv_Hsmode_Logger_BcnAckDataSet_MeasHist	},
	{ ecRfDrvGw_Hs_ReqCmd_SetRead,		0x00,			&SubRFDrv_Hsmode_Logger_BcnAckDataSet_SetRead	},
	{ ecRfDrvGw_Hs_ReqCmd_SetWrite,		0x0A,			&SubRFDrv_Hsmode_Logger_BcnAckDataSet_SetWrite	},
	{ ecRfDrvGw_Hs_ReqCmd_ReqFirm,		0x13,			&SubRFDrv_Hsmode_Logger_BcnAckDataSet_ReqFirm	}
};
#endif

/* ���A���^�C���ʐM�Ԋu���X�g */
const uint16_t cComIntList[ 9U ] = { 0xFFFF, 10U, 20U, 60U, 300U, 600U, 900U, 1200U, 1800U };

/*
 *==============================================================================
 *	�ϐ���`
 *==============================================================================
 */
/* �H���p�����[�^ */
static sint16_t			rfFreqOffset;
#if (swKouteiMode == imEnable)
static uint16_t			rfCurrentCh_FSK;
static uint16_t			rfCurrentCh_LoRa;					/* �H�����[�h�p */
#endif
//static int8_t			vrfTempCal;							/* �������x�Z���T�␳�l */

/* ���[�U�[�ݒ�p�����[�^ */
//static uint8_t			vrfSysModeLocalCentral;
static ST_RF_SerialID_t	vrfMySerialID;
static uint8_t			vrfLoraChGrupeMes;					/* Lora�`���l���O���[�v�ݒ� �v���ʐM�p */
//static uint8_t			vrfLoraChGrupeCnt;					/* Lora�`���l���O���[�v�ݒ� �ڑ��p */

static uint16_t			vrfHsReqDataNum;					/* �����ʐM�ł̃r�[�R���Ăяo���̗v���f�[�^���� */



#if (swLoggerBord == imEnable)
	static uint16_t	vrfRTmodeCommInt;						/* �����ʐM�Ԋu */
	static uint16_t	vrfPreRTmodeCommInt;					/* �O��܂ł̖����ʐM�Ԋu */
	static uint8_t	vrfRTmodeCommCutTime;					/* �����ʐM���s�ɂ��RTmode�ؒf�̎��s��臒l */
	static uint8_t	vrfHsmodeSleepMode;						/* 0xFF�̏ꍇ�͕ϐ��֒���0�ȂǏ������ނ܂�HSmode�͋N�����Ȃ� */
	static uint8_t	vrfHsWkupDectNum;						/* �����ʐM���[�h 1set�ł̋N���� 3�񃍃K�[���o�p,3��v���ݒ�p */
#if 0
	static uint8_t vrfGwConnectList[ GW_FILTER_MAXNUM ][ 3U ];	/* GW�ɐڑ����̂��郊�X�g ��l�߂Ŕz�u [0U][0U]��0x00 00 00��ALL���� */
#endif
#else
	static uint8_t vrfLoggerSetReserveList[ 5U ][ 3U ];			/* �ݒ�ύX�\�񃊃X�g5�䕪 */
//	static uint8_t vrfHsmodeDuty;								/* HSmode��Beacon��ON/OFF���������duty 0:�A�� , 1:25sec/1min , 2:25sec/3min */
//	static uint8_t vrfLoggerDisconnectCnt;						/* ���K�[�ؒf���f�̐ݒ�J�E���g�� Idle�̐擪��+1 */
#if 0
	static uint8_t vrfTorokuMode;								/* GW�ŐV�K���K�[�̓o�^��t�� */
#endif
#endif

/* �X�e�[�^�X��� */
static uint8_t			vrfRssi;
static uint8_t	vrfCurrentRssi;
//static uint8_t	vrfCurrentRssi_Big;
#if (swLoggerBord == imEnable)
	static uint8_t	vrfConnectSuccess;
	static uint8_t	vrfRTmodeTimeSlot;
	//static uint16_t vrfRTmodeCommFailNum;
	static uint8_t	vrfRtmodeStopCnt;						/* 2021.8.2 */
#else
	static uint8_t 	vrfRtChSelect;							/* RTmode�ł̖����ʐM�`���l���̑I�����[�h�uCH_AUTO�v�uCH_MANU�v */
	static uint8_t	vrfHSLoggerID[ 3U ];					/* �����ʐM���[�h: �v�����K�[ID */
	static uint16_t	vrfHSReqHeadMeaID;						/* �����ʐM���[�h: �擪�v�����n��ID */
	static uint16_t	vrfHSReqHeadMeaTime[ 2U ];				/* �����ʐM���[�h: �擪�v������ */
	static uint8_t vrfLoggerSumNumber;
	static uint8_t vrfLoggerList[ 60U ][ 3U ];				/* RTmode�Őڑ����Ă��郍�K�[ */
	static uint8_t vrfLoggerComCnt[ 60U ];					/* �v���l�ʐM�̃J�E���g �ʐM�����ŃN���A */
	static uint8_t vrfLoggerRssi[ 60U ];
	static uint16_t vrfScanRssiOver;
	static uint16_t vrfScanRssiUnder;
#endif

/* �f�o�b�O�ϐ� */
/* HSmode Debug�ϐ� */
#if swRfMonitor == imEnable
static uint16_t vrfDebug[56U];
//static uint16_t vrfDebug[20U];
#endif

//static uint16_t vrfDebugCntL[50U];
//static uint16_t vrfTest;						/* �O���[�o���錾 */
static uint16_t vrfErrorCorrectCnt;
/*
static uint16_t vrfCorrectSetCnt;
static uint16_t vrfTestPacketCnt;
static uint16_t vrfTestCrcErrorCnt;
static uint16_t vrfTestCorrectTimeout;
static uint16_t vrfTestHeaderCrcError;
static uint8_t vrfTestHeaderTimeOut;
*/
/* RTmode Debug�ϐ� */
#if (swLoggerBord == imEnable)
#if (swRssiLogDebug == imEnable)
	static uint8_t vrfErorrCause;						/* �ʐM�G���[�̌��� */
#endif
	static uint16_t vrfDebugRTmodeCnt;				/* RTmode�f�o�b�O�p �v���l���M�� */
	static uint16_t vrfDebugRTmodeSuccess;			/* RTmode�f�o�b�O�p �v���l�ʐM������ */
	static uint16_t vrfDebugRTmodeRxTimeout;		/* RTmode�f�o�b�O�p �v���l�ʐMAck��M�^�C���A�E�g�� */
	static uint16_t vrfDebugRTmodeCadMiss;			/* RTmode�f�o�b�O�p �v���l�ʐMAck��CAD�~�X */
	static uint16_t vrfDebugRTmodeCrc;
#else
#endif

/* �����p�ϐ� */
static ST_RFPrm_t		vRFPrm;							/* �����ʐM�����p�����[�^ */
static ST_RFStatus_t	rfStatus;						/* ��������ԃp�����[�^ */

static uint16_t			vSleepLoop;
//static uint16_t			vrfDummy;
static uint16_t			vrfPacketSum;					/* ���M���錻�݂̃p�P�b�g */
static uint16_t			vrfPacketLimit;					/* ���M����p�P�b�g���� */
static UT_RF_Data_t		vutRfDataBuffer;
static UT_RF_Hsmode_AckData_t vrfHsmodeAckBuff;			/* Ack��M����vutRfDataBuffer�����p�ł��Ȃ����� */

static ET_RfDrv_CommIntMode_t		vRfDrv_CommIntMode;

static uint8_t		vLoopLoRa;
static uint8_t		vErrorLoopCounter;
static uint8_t 		vCallChLoopLora;
static ET_RfDrv_ResInfo_t		vrfResInfo;			/* HSmode�Ń��K�[����̎�M�f�[�^�ɑ΂���Ack(0U) or Nack(1U)�y��GW��M�����̌��� */
//static uint8_t		vRtmode_DataCh[3U];
//static uint8_t		vHsmode_Ch[3U];
static uint8_t		vrfSetCommTime;
static uint16_t		vrfHsmodePacketLength;			/* �Z�b�g����f�[�^���v�� */
static ET_RfDrv_GwReqCmd_t vrfHsReqCmdCode;
static uint8_t		vrfMesureCommIntTime;			/* 10sec,20sec,60sec GW��Idle�p */
static uint16_t		vrfRtmodeCommTimeTotal;			/* ���M���Ԑ����p ���M���ԃJ�E���g�ϐ� */

#if (swLoggerBord == imDisable)
static uint8_t		vrfRtMeasChMask;				/* �v���ʐM��AppID�Ⴂ�̒ʐM����M�����Ƃ��Ƀ}�X�N����ch */
#endif
//static int8_t		vrfTempCorrect;					/* �����x�ɂ��^�C�~���O�␳ */

/* 2021.7.12 �ǉ� */
static uint8_t		vrfInitCnt;						/* ��莞�Ԗ����ʐM���Ȃ��ꍇ�ɏ����� */
static uint8_t 		vrfRfErr;						/* ����IC�֘A�G���[�� */

#if (swLoggerBord == imEnable)
	static uint16_t			vrfPacketPt[2U];		/* ���M���������p�P�b�g�̃|�C���^(2��) ��񐔂̑��M��[0U]�Ƌ����񐔂̑��M��[1U] */
	static uint8_t	vrfCallID[3U];					/* ����4bit�͋� */
	static uint8_t	vrfCommTargetID[3U];
	static ET_RfDrv_InitState_t vrfInitState;
	static ET_RfDrvMainLoggerSts_t	vRfDrvMainLoggerSts;
	static uint8_t	rfTimeSlotGrupe;
	static uint8_t	rfTimeSlotNum;
	static uint16_t	vRtMode_MeasSndKosu;
	static uint16_t	vRtMode_MeasAlmSndKosu;
	static uint16_t gvrfHsmodePacketPt;				/* Hsmode ���M�p�P�b�g�̃|�C���^ */
	static uint16_t	vMem_ReqIndexNo_St;				/* Flash���烊�[�h����擪��IndexNo */
	static uint16_t vMem_RdIndexNo;					/* Flash���烊�[�h����IndexNo */
	static uint16_t	vFirmPacketNum;
	static uint16_t	vFirmPacketSum;
#else
	static ET_RfDrvMainGwSts_t		vRfDrvMainGwSts;
	static uint16_t vrfHsmodeRandom;
	static uint16_t vrfHsLoginUserId;
	static uint16_t	vrfHsLgDct;					/* ���K�[���m�� */
#endif

#if (swLoggerBord == imEnable)
static ST_Rf_LogSendCtl_t vrfLogSendPacketCtl;
static uint8_t	vRfTestFlg = 0U;
static uint16_t vGusu_Snd_EndId;
static uint16_t vKisu_Snd_EndId;
static uint16_t vSleepPacket_StaIndex;
static uint16_t vPrePacket_EndIndex;
static uint8_t vSleep_Sts;
#endif





#pragma section text MY_APP2
/*
 *******************************************************************************
 *	�����ʐM���� �p�����[�^����������
 *
 *	[���e]
 *		�����ʐM�̃p�����[�^�������������s���B
 *******************************************************************************
 */
void ApiRFDrv_ParamInitial( void )
{
	vRFPrm.mInitSts = ecRFDrvInitSts_Init;				/* �������X�e�[�^�X */
	vRFPrm.mInitWaitCnt = 100U;							/* �C�j�V�����C�Y�҂��J�E���^(10ms/�J�E���g) */
	vSleepLoop = 0U;
	
	gvRfTimingCounterL = 0U;
	gvRfTimingCounterH = 0U;
	vCallChLoopLora = 0U;
#if (swLoggerBord == imDisable)
	vrfRtMeasChMask = RT_MODE_CH_LOOP_NUM + 1U;		/* �ő�l���+1�Ń}�X�N�Ȃ� */
#endif
	vrfLoraChGrupeMes = 0U;				/* Lora�̃`���l���O���[�v�v���ʐM 0-6 */
//	vrfLoraChGrupeCnt = 0U;				/* Lora�̃`���l���O���[�v�ڑ��ʐM 0-1 */
	vRfDrv_CommIntMode = ecRfDrv_LongIntMode;
	vrfMesureCommIntTime = RT_INT_LONG_MODE_TX_INT;
	vrfSetCommTime = RT_INT_LONG_MODE_SET_TIME;
	vrfRtmodeCommTimeTotal = 0U;
//	vrfTempCal = 0U;								/* �������x�Z���T�␳�l */
	gvRfTimingCorrectCnt = 0U;			/* �^�C�~���O�␳��3sec�J�E���^������ */
	gvrfStsEventFlag.mRtmodePwmode = PW_H;
	gvrfStsEventFlag.mPowerInt = BATT_POWER_ON;
	gvrfStsEventFlag.mTimingCorrect = 0U;
	gvrfStsEventFlag.mReset = RFIC_RUN;
	vrfRfErr = 0U;
//	gvInFlash.mParam.mOnCertLmt = LOCAL;
	
#if (swLoggerBord == imDisable)
	/* GW RTmode */
	gvInFlash.mParam.mrfTorokuMode = LOGGER_TOROKU_OFF;
	vrfRtChSelect = CH_AUTO;
	vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWInit;
	R_IT_Create_Custom( 0x0F );		/* 0.488msec */
	gvrfStsEventFlag.mMuskMultiEvent = 0U;
	vrfLoggerSumNumber = 0U;
//	vrfLoggerDisconnectCnt = 3U;			/* �����l3��A���G���[�Őؒf */
	gvrfStsEventFlag.mHsmodeInit = 0U;
	M_CLRBIT(P3,M_BIT0);					/* RF COM LED */
	vrfScanRssiOver = 0U;
	vrfScanRssiUnder = 0U;
//	vrfHsmodeDuty = 0U;					/* FSK���M �A�� */
	vrfHsmodeRandom = 0U;
	vErrorLoopCounter = 0U;
	/* 2021.7.12 �ǉ� */
	vrfInitCnt = 0U;
	/* 2021.7.20 */
	vrfHsLgDct = 0U;
#else
	/* Logger RTmode */
	vrfRTmodeCommInt = 300U;			/* �����l1min�����̒ʐM�Ԋu => Idle�̏������ł���`���Ă��� */
	vrfPreRTmodeCommInt = vrfRTmodeCommInt;
	vrfHsWkupDectNum = 0U;
	
	vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_Init;
	R_IT_Create_Custom( 0xFFF );		/* 125msec */
	gvrfStsEventFlag.mTimerSync = 0U;
	vrfConnectSuccess = 0U;
	vrfRTmodeCommFailNum = 0U;
	vrfRTmodeCommCutTime = 4U;			/* RTmode�ʐM�ؒf���� �����l3�� */
	vrfHsmodeSleepMode = 0U;
	gvrfStsEventFlag.mSetReserve = 0U;
	gvrfStsEventFlag.mHsmodeeResCmdStatus = 0U;
	vrfRtmodeStopCnt = 0U;							/* 2021.8.2 */
	gvrfStsEventFlag.mOnlineLcdOnOff = 0;			/* LCD OFF 2022.9.16 */
#endif
	
	vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;				/* ���K�[�X�e�[�^�X(�u���[�h�L���X�g) */
	
	/* Debug�ϐ������� */
#if (swLoggerBord == imEnable)
	vrfDebugRTmodeCnt = 0U;
	vrfDebugRTmodeSuccess = 0U;
	vrfDebugRTmodeRxTimeout = 0U;
	vrfDebugRTmodeCadMiss = 0U;
	vrfDebugRTmodeCrc = 0U;
#if (swRssiLogDebug == imEnable)
	vrfErorrCause = 0U;
#endif
#endif
}


/*
 *******************************************************************************
 *	�����ʐM���� ����������
 *
 *	[���e]
 *		�����ʐM�̏������������s���B(10ms�����Ŗ{�֐������s)
 *******************************************************************************
*/
void ApiRFDrv_Initial( void )
{
	gvModuleSts.mRf = ecRfModuleSts_Sleep;
	switch( vRFPrm.mInitSts )
	{
		case ecRFDrvInitSts_Init:
			/* �{�����͋N����1�x�̂ݎ��s */
			RADIO_SW = RF_SW_ON;
			SubSX1272POR();														/* POR���� */
			/* 10msec��Wait�K�v �� 10�`11msec��Wait */
			SubIntervalTimer_Sleep( 375U * 12U, 1U );
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			vRFPrm.mInitSts = ecRFDrvInitSts_InitPorWait;
			break;
			
		case ecRFDrvInitSts_InitPorWait:
			/*
			 ***************************************
			 *	���������݋����Ă���1�b��̏���
			 *	(�����ʐM�̏�����/�ݒ蓙)���ȉ��ɋL��
			 *	 ���{������1�x�̂ݎ��s
			 ***************************************
			 */
			ApiSX1272Reset( imON );
			/* 100usec�ȏ�K�v �� Wait 1�`2 ms */
			SubIntervalTimer_Sleep( 75U, 1U );		/* 0.1msec */
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			vRFPrm.mInitSts = ecRfDrvInitSts_InitRstWait;
			break;
			
		case ecRfDrvInitSts_InitRstWait:
			ApiSX1272Reset( imOFF );
			/* 5msec�K�v �� Wait 6�`7 ms */
			SubIntervalTimer_Sleep( 375U * 6U, 1U );		/* 5msec */
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			vRFPrm.mInitSts = ecRfDrvInitSts_InitRstEndWait;
			break;
			
		case ecRfDrvInitSts_InitRstEndWait:
			SubSX1272SetModem( MODEM_FSK );
			/* ������ */
			SubSX1272Sleep();
			SubSX1272InitReg();
			SubSX1272Sleep();
			rfStatus.Modem = MODEM_FSK;
			rfStatus.State = RF_SLEEP;
			rfStatus.Modem = MODEM_LORA;
			
			SubIntervalTimer_Sleep( 375U * 100U, 2U );
			vRFPrm.mInitSts = ecRFDrvInitSts_InitEnd;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRFDrvInitSts_InitEnd:
		default:
			break;
	}
}


/*
 *******************************************************************************
 *	�����ʐM�������C��
 *
 *	[���e]
 *		�����ʐM�������C��(1�b�����Ŗ{�֐������s)
 *******************************************************************************
 */
void ApiRFDrv_Main( void )
{
	uint8_t		wkRadio;
	uint8_t		wkRadio2nd;
	uint16_t	wkRtmodeInterval;
	uint16_t	wkCulc;
	
#if (swLoggerBord == imEnable)
	uint8_t		wkHsmodeInterval;
	uint8_t		wkLoop;
	uint8_t		wkU8Array[ 4U ];
	sint16_t	wkU16Array[ 4U ];
	uint32_t	wkU32;
	static uint8_t vOldPtrFlg;				/* �������ʐM�I���t���O�F0:�v���ʐM�A1:�v���x�񗚗� */
#else
	uint8_t		wkReg;
	uint8_t		wkErrCode;
#endif
	
	
#if 0
	static uint8_t vTestDebugCnt;	/* debug */
#endif

	
#if (swKouteiMode == imEnable)
	/***** �H�����[�h *****/
//	gvMode = ecMode_RfTxFSK;		/* �����I��FSK�e�X�g */
//	rfCurrentCh_FSK = 75U;			/* �����I��FSK�e�X�g */
//	rfCurrentCh_LoRa = 0U;			/* �����I��FSK�e�X�g�̏ꍇ��0 */
//	rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
	
	/* LoRa���M */
	if( gvMode == ecMode_RfTxLoRa )
	{
		SubSX1272Sleep();
		rfStatus.Modem = MODEM_FSK;
		SubSX1272Sleep();
		SubSX1272Stdby();
		SubSX1272FreqSet( rfCurrentCh_LoRa, rfFreqOffset );
		
		SubSX1272TxFSK_DIO_240kbps();
		SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_RESPONSE );
		SubSX1272FSK_Length( 64U );
		SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
		SubSX1272CarrierSenseFSK_DIO();
		
		while( 1U )
		{
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
				SubRFDrv_PN9LoRa( rfCurrentCh_LoRa );
				break;
			}
			R_WDT_Restart();
		}
		
		/*** Logger ***/
#if (swLoggerBord == imEnable)
		while( 1U )
		{
			R_WDT_Restart();
			if( gvMode == ecMode_RfTxLoRaStop )
			{
				vRFPrm.mInitSts = ecRFDrvInitSts_Init;
				gvModuleSts.mRf = ecRfModuleSts_Run;
				while( 1 )
				{
					if( gvModuleSts.mRf == ecRfModuleSts_Run && (ecRFDrvInitSts_InitEnd != ApiRFDrv_GetInitSts()) )
					{
						ApiRFDrv_Initial();
					}
					
					/* RF(�����ʐM)�̏��������� */
					if( ecRFDrvInitSts_InitEnd == ApiRFDrv_GetInitSts() )
					{
						gvModuleSts.mRf = ecRfModuleSts_Run;
						break;
					}
				}
				break;
			}
		}
#else
		/*** GW ***/
		while( P_CTS == imHigh )
		{
			M_NOP;
		}
		if( P_CTS == imLow )
		{
			while( 1U )
			{
				R_WDT_Restart();
				if( P_CTS == imHigh )
				{
					gvutComBuff.umModInf.mRcvPos = 0U;
					break;
				}
			}
			gvMode = ecMode_Process;
			vRFPrm.mInitSts = ecRFDrvInitSts_Init;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			while( 1 )
			{
				R_WDT_Restart();
				if( gvModuleSts.mRf == ecRfModuleSts_Run && (ecRFDrvInitSts_InitEnd != ApiRFDrv_GetInitSts()) )
				{
					ApiRFDrv_Initial();
				}
				
				/* RF(�����ʐM)�̏��������� */
				if( ecRFDrvInitSts_InitEnd == ApiRFDrv_GetInitSts() )
				{
					gvModuleSts.mRf = ecRfModuleSts_Run;
					break;
				}
			}
		}
#endif
	}
	/* FSK���M */
	else if( gvMode == ecMode_RfTxFSK )
	{
		rfStatus.Modem = MODEM_FSK;
		SubRFDrv_PN9FSK( rfCurrentCh_FSK );
		R_WDT_Restart();
		return;
	}
	/* LoRa�AFSK��M */
	else if( gvMode == ecMode_RfRxLoRa || gvMode == ecMode_RfRxFSK )
	{
		SubRfDrv_TestRx();
		return;
	}
	/* LoRa�ڑ� */
	else if( gvMode == ecMode_RfRxLoRaCons )
	{
		SubRfDrv_TestCurrentRx();
		gvMode = ecMode_RfRxLoRa;
		return;
	}
	else if( gvMode == ecMode_Process )
	{
		return;
	}
#else
	if( gvMode == ecMode_Process )
	{
		return;
	}
#endif
	
	/*** �Z�K�E���g�������e�X�g�p ***/
#if (swRfTxTest == imEnable)
	while( 1U )
	{
		/* ���g�������p�ϐ� 61.035[Hz/Count] */
		rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
		rfCurrentCh_LoRa = 39U;
		SubRFDrv_PN9LoRa( rfCurrentCh_LoRa );
		while( 1U )
		{
			R_WDT_Restart();
		}
	}
#endif
	
	
	/*** ��M���x�e�X�g�p ***/
#if (swRfRxTest == imEnable)
	rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
	rfCurrentCh_LoRa = 39U;
	SubSX1272FreqSet( rfCurrentCh_LoRa, rfFreqOffset );
	rfStatus.Modem = MODEM_FSK;
	SubSX1272Sleep();
	SubSX1272CarrierSenseFSK_DIO();
	while( 1U )
	{
		/* Carrire Sence 125usec */
		if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
		{
			wkRadio = SubSX1272Read( REG_RSSIVALUE );
			SubSX1272Sleep();
#if (swLoggerBord == imEnable)
			ApiLcd_Upp7SegNumDsp( (wkRadio / 2U), imOFF, imOFF );
			ApiLcd_Low7SegNumDsp( 0U, imOFF );
			ApiLcd_Main();
#else
			P7_bit.no5 = 1U;
#endif
		}
		else
		{
			wkRadio = SubSX1272Read( REG_RSSIVALUE );
			SubSX1272Sleep();
#if (swLoggerBord == imEnable)
			ApiLcd_Upp7SegNumDsp( (wkRadio / 2U), imOFF, imOFF );
			ApiLcd_Low7SegNumDsp( 1U, imOFF );
			ApiLcd_Main();
#else
			P7_bit.no5 = 0U;
#endif
		}
		SubIntervalTimer_Sleep( 0xFFFF, 2U );
		while( !gvRfIntFlg.mRadioTimer )
		{
			M_HALT;
		}
		R_TAU0_Channel6_Stop();
	}
#endif
	
	
	/*********** �ʏ폈�� ***********/
#if (swLoggerBord == imEnable) && (swRfTxTest == imDisable) && (swRfRxTest == imDisable)
	
	/* ���g�������p�ϐ� 61.035[Hz/Count] */
//	gvInFlash.mParam.mOnCertLmt = CENTRAL;
	rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
	
	gvModuleSts.mRf = ecRfModuleSts_Sleep;
	gvRfIntFlg.mRadioStsMsk = 0U;		/* 125msec ITimer�ɂ��ecRfModuleSts_Run��L�� */
	
	/* �d�r�����̏��� */
#if (swSensorCom == imDisable)
	if( gvrfStsEventFlag.mPowerInt == BATT_POWER_OFF )
#else	
	if( gvrfStsEventFlag.mPowerInt == BATT_POWER_OFF || gvInFlash.mProcess.mModelCode == 0xFFU )
#endif
	{
		ApiRfDrv_ForcedSleep();
		vrfErorrCause = 27U;
#if (swRssiLogDebug == imEnable)
		SubRfDrv_RssiLogDebug();
#endif
	}
	
	switch( vRfDrvMainLoggerSts )
	{
		/* *** ���K�[ FSK���[�h ********************/
		case ecRfDrvMainSts_HSmode_BeaconCsInit:
//			rfFreqOffset = RF_FREQ_OFFSET;
			vrfHsWkupDectNum = 0U;
			
			/* ����I�ȏ��������� */
			if( gvrfStsEventFlag.mReset == RFIC_INI )
			{
				SubRfDrv_RfReset();
				gvrfStsEventFlag.mReset = RFIC_RUN;
			}
			
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272Write( REG_PACKETCONFIG1,
				RF_PACKETCONFIG1_PACKETFORMAT_FIXED | RF_PACKETCONFIG1_DCFREE_OFF
				 | RF_PACKETCONFIG1_CRC_OFF | RF_PACKETCONFIG1_CRCAUTOCLEAR_OFF
				 | RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT  );
			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_CALL );
			SubRFDrv_SyncSet();
			SubSX1272RxFSK_DIO();
			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_1);
			SubSX1272Write( REG_LNA, RF_LNA_GAIN_G1 | RF_LNA_BOOST_OFF );
			SubSX1272FreqSet(RF_HSMODE_CH[ gvInFlash.mParam.mrfHsCh ], rfFreqOffset);
			
			SubSX1272FSK_Length( RF_HSMODE_BEACON_LENGTH );
			SubRfDrv_PwChange( PW_L );
			
			vrfInitState = ecRfDrv_Init_Hsmode;
			vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconPre1stDetect;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconPre1stDetect:
			/* �A��WakeUp�̖h�~ */
#if swRfPortDebug == imEnable
			P7_bit.no4 = 0U;
#endif
			if( vrfHsmodeSleepMode > 0U )
			{
#if 0
				if( vrfHsmodeSleepMode != 0xFF )
				{
					vrfHsmodeSleepMode--;
				}
				else
				{
					SubRFDrv_Hsmode_SleepFin();
				}
				vrfInitState = ecRfDrv_Init_Hsmode;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				break;
#endif
				if( vrfHsWkupDectNum == 0U )
				{
					gvrfStsEventFlag.mHsmodeSyncDetect = 0U;
					vrfInitState = ecRfDrv_Init_Non;
					SubRFDrv_SyncSet();
				}
			}
			else
			{
				/* 2���1��̂݃X�e�[�^�X�Ăяo�������m���� */
				if( gvrfStsEventFlag.mHsmodeeResCmdStatus == 0U )
				{
					gvrfStsEventFlag.mHsmodeeResCmdStatus = 1U;
					gvrfStsEventFlag.mHsmodeSyncDetect = 0U;
					vrfInitState = ecRfDrv_Init_Non;
					SubRFDrv_SyncSet();
				}
				else
				{
					gvrfStsEventFlag.mHsmodeeResCmdStatus = 0U;
				}
			}
			//break�Ȃ�
		case ecRfDrvMainSts_HSmode_BeaconPre1stDetect_Inter:
			if(SubRFDrv_Wakeup_PreDetect() != HS_MODE_PREAMBLE_SUCCESS )
			{
				/* 1.056msec Wait��A�ăT�[�`���� ���������̂�����N������+0.3msec */
				wkRadio = 0U;
				if(!gvrfStsEventFlag.mHsmodeDetectLoop){ wkRadio = 112U; }
				SubIntervalTimer_Sleep( _PREAMBLE_RETRY_WAIT_ITMCP_VALUE - wkRadio, 1U );
				vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconPre2ndDetect;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			else
			{
				vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconSync1stDetect;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
#if swRfPortDebug == imEnable
			P7_bit.no4 = 1U;
#endif
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconPre2ndDetect:
			if( SubRFDrv_Wakeup_PreDetect() != HS_MODE_PREAMBLE_SUCCESS )
			{
				/* miss*/
				SubRfDrv_Wakeup_Cycle();
#if 0
				if( gvrfStsEventFlag.mHsmodeDetectLoop )
				{
					/* ���K�[���o���~����i�v���E�ݒ�̂݁j */
					SubRfDrv_Wakeup_Cycle();
				}
				else
				{
					/* ���g���C���� 250msec Sleep */
					wkRadio = gvRfTimingCounterL;			/* gvRfTimingCounterL��0-7�ŌJ��Ԃ� */
					if( wkRadio < 6U )
					{
						while( gvRfTimingCounterL < wkRadio + 2U )
						{
							/* �R���p���[�^���s����STOP����̋N���ɂ��w���Y���h�~�Ƃ��āFHALT */
							if( CMPMK0 == 0U )
							{
								M_HALT;
							}
							else
							{
								M_STOP;
							}
						}
					}
					else
					{
						while( !(gvRfTimingCounterL == (wkRadio - 6U)) )
						{
							/* �R���p���[�^���s����STOP����̋N���ɂ��w���Y���h�~�Ƃ��āFHALT */
							if( CMPMK0 == 0U )
							{
								M_HALT;
							}
							else
							{
								M_STOP;
							}
						}
					}
					gvrfStsEventFlag.mHsmodeDetectLoop = 1U;
//					SubIntervalTimer_Sleep( _HSMODE_DETECT_RETRY_ITMCP_VALUE, 1U );
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconPre1stDetect;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
#endif
			}
			else
			{
				gvModuleSts.mRf =ecRfModuleSts_Run;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconSync2ndDetect;
			}
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconSync1stDetect:
			if( SubRFDrv_Wakeup_SyncDetect() != HS_MODE_SYNC_SUCCESS )
			{
				/* 0.96msec Wait��A�ăT�[�`���� ����͋N������+0.3msec ?? */
				wkRadio = 0U;
				if( !gvrfStsEventFlag.mHsmodeDetectLoop ){ wkRadio = 112U; }
				SubIntervalTimer_Sleep( _SYNCMISS_RETRY_ITMCP_VALUE - wkRadio, 1U );
				vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconPre2ndDetect;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			else
			{
				vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconSync2ndDetect:
			if( SubRFDrv_Wakeup_SyncDetect() != HS_MODE_SYNC_SUCCESS )
			{
				SubRfDrv_Wakeup_Cycle();
#if 0
				vrfInitState = ecRfDrv_Init_Hsmode;
				if( gvrfStsEventFlag.mHsmodeDetectLoop )
				{
					SubRfDrv_Wakeup_Cycle();
				}
				else
				{
					/* ���g���C���� 250msec Sleep */
					wkRadio = gvRfTimingCounterL;			/* gvRfTimingCounterL��0-7�ŌJ��Ԃ� */
					if( wkRadio < 6U )
					{
						while( gvRfTimingCounterL < wkRadio + 2U )
						{
							/* �R���p���[�^���s����STOP����̋N���ɂ��w���Y���h�~�Ƃ��āFHALT */
							if( CMPMK0 == 0U )
							{
								M_HALT;
							}
							else
							{
								M_STOP;
							}
						}
					}
					else
					{
						while( !(gvRfTimingCounterL == (wkRadio - 6U)) )
						{
							/* �R���p���[�^���s����STOP����̋N���ɂ��w���Y���h�~�Ƃ��āFHALT */
							if( CMPMK0 == 0U )
							{
								M_HALT;
							}
							else
							{
								M_STOP;
							}
						}
					}
					gvrfStsEventFlag.mHsmodeDetectLoop = 1U;
//					SubIntervalTimer_Sleep( _HSMODE_DETECT_RETRY_ITMCP_VALUE, 1U );
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconPre1stDetect;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
#endif
			}
			else
			{
				vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconRxStart:
			/* Syncword���o ��M�J�n */
			SubSX1272RxFSK_DIO();						/* SubRFDrv_Wakeup_SyncDetect()��SubSX1272RxFSK_On()���s */
			SubIntervalTimer_Sleep( 10 * 375U , 1U );
			SubSX1272RxFSK( RF_HSMODE_BEACON_LENGTH );
			SubSX1272Sleep();
			/* CRC�`�F�b�N */
			if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[ 0U ], RF_HSMODE_BEACON_LENGTH, BCH_OFF ) == SUCCESS )
			{
				/* CRC OK */
				wkRadio = FAIL;
				if( (vutRfDataBuffer.umGwQueryBeacon.mPacketCode == 0x00)
					&& (vutRfDataBuffer.umGwQueryBeacon.mCompanyID == vrfMySerialID.mOpeCompanyID) )
				{
					/* ���Ɖ��ID�A�p�P�b�g�R�[�h��v */
					if( ((vutRfDataBuffer.umGwQueryBeacon.mAppID[0U] == vrfMySerialID.mAppID[0U])
						&& (vutRfDataBuffer.umGwQueryBeacon.mAppID[1U] == vrfMySerialID.mAppID[1U])) )
					{
#if 0
						/* ���p���ID��v */
						if( (vrfMySerialID.mAppID[0U] == APP_ID_LOCAL_UP) && (vrfMySerialID.mAppID[1U] == APP_ID_LOCAL_LO) )
						{
							/* ���[�J�����[�h */
							if( SubRFDrv_Filter_Gwid( &vutRfDataBuffer.umGwQueryBeacon.mGwID[0U] ) == ecRfDrv_Success )
							{
								wkRadio = SUCCESS;
							}
						}else
						{
							/* �W���Ď����[�h(�T�[�o�[���[�h)�ł̓t�B���^�����O�Ȃ� */
							wkRadio = SUCCESS;
						}
#else
						/* ���p���ID��v */
						if( SubRFDrv_Filter_Gwid( &vutRfDataBuffer.umGwQueryBeacon.mGwID[0U] ) == ecRfDrv_Success )
						{
							wkRadio = SUCCESS;
							
							wkU32 = (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mGwID[ 0U ] << 12U;
							wkU32 |= (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mGwID[ 1U ] << 4U;
							wkU32 |= (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mGwID[ 2U ] >> 4U;
							
							/* �V�X�e�������F�����ʐM(�Ό�GWID) */
//							ApiFlash_WriteSysLog( ecSysLogItm_FastCom, wkU32 );
							
						}
#endif
					}
					if( (((vrfMySerialID.mAppID[0U] >> 4U) & 0x0F) == APP_ID_LOGGER_INIT_UP )
						&& ( (((vrfMySerialID.mAppID[0U] << 4U) & 0xF0) + ((vrfMySerialID.mAppID[1U] >> 4U) & 0x0F)) == APP_ID_LOGGER_INIT_LO ))
					{
						/* ���p���ID�������l */
						wkRadio = SUCCESS;
					}
				}
				if( wkRadio == SUCCESS )
				{
					/* �V�X�e��ID���� ����AppID���͕K����������(������Sync�̊֌W��GW���o�^���[�h��������M���Ȃ�) */
					wkCulc = SubRfDrv_CallResponseTimeslot( &rfTimeSlotGrupe, &rfTimeSlotNum );
					/* Test */
//					rfTimeSlotGrupe = 3U;
//					rfTimeSlotNum = 0U;
					SubIntervalTimer_Sleep( wkCulc , 1U );
					
					/* ��M�r�[�R���̃f�[�^���� �J�ڐ攻��,�����^�C�~���O�ݒ� */
					SubRfDrv_Hsmode_Logger_BeaconRxPrcss();
				}else
				{
					/* CompanyID,PacketID,AppID�Ⴂ�EGWID�t�B���^�����O���� */
					SubRFDrv_Hsmode_End();
				}
			}
			else
			{
				/* CRC�G���[ */
				SubRFDrv_Hsmode_End();
			}
			break;
			
		/* HSmode beacon Responce */
		case ecRfDrvMainSts_HSmode_BeaconResTxWait:
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxWait;
			if( rfTimeSlotGrupe > 0U)
			{
				if( rfTimeSlotGrupe == 2U )
				{
					SubIntervalTimer_Sleep( _CALL_RESPONCE_SEND_TIMESLOT_GRUPE, 1U );
					/* rfTimeSlotGrupe == 1U�܂łɃt���b�V�����瑗�M�o�b�t�@�փf�[�^���Z�b�g���� */
					
					gvrfStsEventFlag.mHsmodeTxTurn = 0U;				/* ���M�񐔂���ŃZ�b�g(�ŏ���1���) */
					SubRfDrv_Hsmode_Logger_BeaconRxAnalysis();				/* ��M�f�[�^��͂��A�������� */
				}
				else if( rfTimeSlotGrupe == 1U )
				{
					/* ��������(6msec) */
//					SubIntervalTimer_Sleep(_CALL_RESPONCE_SEND_TIMESLOT_GRUPE - 2250U, 1U );
					/* -------���X�|���X�f�[�^�̃Z�b�g(100msec - 2250/375msec)94msec----- */
//					SubIntervalTimer_Sleep(_CALL_RESPONCE_SEND_TIMESLOT_GRUPE - 700U, 1U );	/* -5.5msec */
					SubIntervalTimer_Sleep(_CALL_RESPONCE_SEND_TIMESLOT_GRUPE, 1U );	/* -5.5msec */
					vErrorLoopCounter = 0U;				/* ���g���C�񐔃J�E���g�ϐ� */
					/* �p�P�b�g���Z�b�g�ƃ|�C���^������ */
//					vrfHsReqDataNum = (uint16_t)vutRfDataBuffer.umGwQueryBeacon.mRequestEndMeasureCount[0U] << 8U;
//					vrfHsReqDataNum += vutRfDataBuffer.umGwQueryBeacon.mRequestEndMeasureCount[1U];
					
					vrfPacketSum = 1U;					/* ���K�[�̏ꍇ��vrfPacketSum��0�܂Ō����Ă����AGW�̏ꍇ��vrfPacketLimit�܂ő����Ă��� */
					vrfPacketLimit = 0U;
					vrfPacketPt[0U] = 1U;
					vrfPacketPt[1U] = 2U;
					
					gvrfHsmodePacketPt = vrfPacketPt[1U];		/* ���C�����[�`���Ƀ��^�[������|�C���^ */
					
//					gvrfStsEventFlag.mHsmodeTxTurn = 0U;				/* ���M�񐔂���ŃZ�b�g(�ŏ���1���) */
					
					SubRFDrv_Hsmode_Logger_ResponceDataSet( 0U );		/* Flash���烊�[�h�����f�[�^�𖳐��o�b�t�@�Ɋi�[ */
					
				}
				else
				{
					SubIntervalTimer_Sleep( _CALL_RESPONCE_SEND_TIMESLOT_GRUPE, 1U );
				}
				rfTimeSlotGrupe--;
			}
			else
			{
				if( rfTimeSlotNum > 0U )
				{
					SubIntervalTimer_Sleep( _CALL_RESPONCE_SEND_TIMESLOT_NUM, 1U);
					rfTimeSlotNum--;
				}
				else
				{
					vLoopLoRa = 0U;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconResTxStart:
#if swRfPortDebug == imEnable
			P7_bit.no4 = 0U;
#endif
			/* �J��Ԃ����M��ݒ� */
			if( vrfPacketSum == 0U )
			{
				/* �p�P�b�g���M���� */
				SubRFDrv_Hsmode_End();
				
				break;
			}
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubRFDrv_SyncSet();
			SubSX1272CarrierSenseFSK_DIO();
			
			/* Carrire Sence 125usec */
//			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == SENS_CARRIER )
			if( SubRFDrv_CarrierSens_5ch( RF_HSMODE_CH[ gvInFlash.mParam.mrfHsCh ]) == SENS_CARRIER )
			{
				/* 2021.11.4 �O���ϐ��̋��� */
//				if( vLoopLoRa < 30U )
				if( vLoopLoRa < 50U )
				{
					SubIntervalTimer_Sleep( (3U - 2U) * 375U, 1U );		/* �L�����A�Z���X���v���� 2.2msec */
					vLoopLoRa++;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;

					gvModuleSts.mRf = ecRfModuleSts_Sleep;
					break;
				}
				else
				{
					SubRFDrv_Hsmode_End();
					break;
				}
			}
			
			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_RESPONSE );
/*
			SubSX1272Write( REG_PACKETCONFIG1,
				RF_PACKETCONFIG1_PACKETFORMAT_FIXED | RF_PACKETCONFIG1_DCFREE_OFF
				 | RF_PACKETCONFIG1_CRC_OFF | RF_PACKETCONFIG1_CRCAUTOCLEAR_OFF
				 | RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT  );
*/
			SubSX1272Write( REG_PACKETCONFIG1,
				RF_PACKETCONFIG1_PACKETFORMAT_FIXED | RF_PACKETCONFIG1_DCFREE_WHITENING
				 | RF_PACKETCONFIG1_CRC_OFF | RF_PACKETCONFIG1_CRCAUTOCLEAR_OFF
				 | RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT  );
#if swRfPortDebug == imEnable
			P7_bit.no4 = 1U;
#endif
			if( SubSX1272TxFSK( vrfHsmodePacketLength ) == SUCCESS )
			{
				if( gvHsMeasAlmFlg == imON )
				{
					gvHsMeasAlmFlg = imOFF;
				}
				
				/*	2021.7.12 �ǉ� */
				vrfInitCnt = 0U;
				
				SubSX1272Sleep();
				if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_SetWrite )
				{
					for( wkRadio = 0U; wkRadio < 2U; wkRadio++ )
					{
						if( SubSX1272TxFSK( vrfHsmodePacketLength ) == FAIL )
						{
							SubSX1272Sleep();
							break;
						}
						SubSX1272Sleep();
					}
#if 0
					if( wkRadio == SUCCESS )
					{
						/* ���M�J�n����GW�̃��X�|���X�܂�HALT */
						SubIntervalTimer_Sleep( 38U * 375U, 1U );
				if( !gvRfIntFlg.mRadioTimer ){ M_HALT; }
					}
#endif
				}
#if swRfPortDebug == imEnable
			P7_bit.no4 = 0U;
#endif
//				*/
#if 0
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
#endif
				/*** �J��Ԃ����M��ݒ� ***/
				gvrfStsEventFlag.mHsmodeTxTurn = ~gvrfStsEventFlag.mHsmodeTxTurn;			/* �f�[�^�Z�b�g�̂��ߊ��������ւ� */
//				SubIntervalTimer_Sleep( 95U * 375U, 1U );									/* 50msec�̊ԂɎ��p�P�b�g�̃f�[�^�Z�b�g */
				SubIntervalTimer_Sleep( (45U - 10U) * 375U, 1U );							/* 50msec�̊ԂɎ��p�P�b�g�̃f�[�^�Z�b�g */
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
				
				/* ���d�rAD���� */
				ApiAdc_AdExe();

				/* Ack��M */
				switch ( vrfHsReqCmdCode )
				{
					case ecRfDrvGw_Hs_ReqCmd_Status:
						/* 2021.8.2 �������ʐM���΂� */
						/* 2021.8.27 */
						if( vrfConnectSuccess == 1U )
						{
							vrfRtmodeStopCnt = (180U / vrfMesureCommIntTime);					/* 3min��(+1��) �������ʐM���~����(�^�C�~���O�ɂ�蒼��-���Z) */
						}
						else
						{
							vrfRtmodeStopCnt = 7U;												/* 7 * 35sec = 245sec�x�~ */
						}
						
					case ecRfDrvGw_Hs_ReqCmd_Toroku:
						SubRFDrv_Hsmode_End();
						break;
					case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
					case ecRfDrvGw_Hs_ReqCmd_SetWrite:
						SubIntervalTimer_Stop();
						SubIntervalTimer_Sleep( 40U * 375U, 1U );
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResDataSetWait;
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
						break;
						
					case ecRfDrvGw_Hs_ReqCmd_Measure:
						
						SubRFDrv_CalFstConnBootCnt();
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResDataSetWait;
						gvModuleSts.mRf = ecRfModuleSts_Run;
						break;
					default:
#if 0
						vLoopLoRa = 25U;													/* 25 * 2msec ��M�҂� */
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResDataSetWait;
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
#else
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResDataSetWait;
						gvModuleSts.mRf = ecRfModuleSts_Run;
#endif
						break;
				}
				
				if( vrfPacketSum > 1 )
				{
					/* ������Flash����f�[�^���W�����[�h���邽�߂̃L���[���Z�b�g */
					SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Continue();
				}
			}
			else
			{
#if swRfPortDebug == imEnable
			P7_bit.no4 = 0U;
#endif
				SubIntervalTimer_Stop();
				switch ( vrfHsReqCmdCode )
				{
					case ecRfDrvGw_Hs_ReqCmd_Status:
					case ecRfDrvGw_Hs_ReqCmd_Toroku:
						SubRFDrv_Hsmode_End();
						break;
					case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
					case ecRfDrvGw_Hs_ReqCmd_SetWrite:
						SubIntervalTimer_Sleep( 40U * 375U, 1U );
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResDataSetWait;
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
						break;
					default:
						/* GW����NACK�������\������邽�ߎ��p�P�b�g�̓Z�b�g���Ȃ� */
//						SubIntervalTimer_Sleep( 95U * 375U, 1U );							/* GW�̎�M���������z���đ҂��𒷂����Ă�OK ���������M�x�~50msec�ȉ���NG */
						SubIntervalTimer_Sleep( (45U - 10U) * 375U, 1U );							/* GW�̎�M���������z���đ҂��𒷂����Ă�OK ���������M�x�~50msec�ȉ���NG */
#if 0						
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResAckRxPreDetect;
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
#else
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResDataSetWait;
						gvModuleSts.mRf = ecRfModuleSts_Run;
#endif
						break;
				}
			}
			/* 2021.11.4 �O���ϐ��̋��� */
//			vLoopLoRa = 19U;													/* 25 * 2msec ��M�҂� */
			vLoopLoRa = 39U;
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconResDataSetWait:
			switch( vrfHsReqCmdCode )
			{
				case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_FirmHexRx;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					vFirmPacketNum = 0U;										/* �t�@�[���A�b�v�f�[�^�p�P�b�g�ԍ������� */
					break;
				case ecRfDrvGw_Hs_ReqCmd_SetWrite:
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_SettingRx;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					break;
				default:
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResAckRxPreDetect;
//					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResAckRxStart;
					if( gvRfIntFlg.mRadioTimer )
					{
						/* �Œ�҂�����50msec�I�[�o�[ */
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					else
					{
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
					}
					break;
			}
#if 0
			if( gvRfIntFlg.mRadioTimer )
			{
				/* �Œ�҂�����50msec�I�[�o�[ */
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
#endif
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconResAckRxPreDetect:
			SubSX1272Sleep();
			SubSX1272RxFSK_DIO();										/* SubRFDrv_Wakeup_SyncDetect()��SubSX1272RxFSK_On()���s */
#if swRfPortDebug == imEnable
			P7_bit.no4 = 0U;
#endif
			if( SubRFDrv_Wakeup_PreDetect() != HS_MODE_PREAMBLE_SUCCESS )
			{
				if( vLoopLoRa > 0U)
				{
					/* Ack��M���g���C */
					SubIntervalTimer_Sleep( 1U * 375U - _PREAMBLE_WAIT_ITMCP_VALUE, 1U );							/* Ack��Preamble����4msec */
					vLoopLoRa --;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResAckRxPreDetect;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
//					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
#if swRfPortDebug == imEnable
			P7_bit.no4 = 1U;
#endif
					/* Ack��M�^�C���A�E�g���� */
					if( vErrorLoopCounter < 10U )
					{
						gvrfStsEventFlag.mHsmodeTxTurn = ~gvrfStsEventFlag.mHsmodeTxTurn;			/* �f�[�^�Z�b�g�̂��ߊ��������ւ� */
						/*	�p�P�b�g�đ� */
						vErrorLoopCounter ++;
						/* �|�C���^�ύX�Ȃ� */
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;

						gvModuleSts.mRf = ecRfModuleSts_Run;
					}else
					{
						/* ���g���C�񐔃I�[�o�[ */
						SubRFDrv_Hsmode_End();
					}
				}
			}
			else
			{
#if swRfPortDebug == imEnable
			P7_bit.no4 = 1U;
#endif
				/* �v���A���u�����m �R�}���h�ɂ�蕪�� */
				switch ( vrfHsReqCmdCode )
				{
					case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_FirmHexRx;
						vFirmPacketNum = 0U;									/* �t�@�[���A�b�v�f�[�^�p�P�b�g�ԍ������� */
						break;
					case ecRfDrvGw_Hs_ReqCmd_SetWrite:
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_SettingRx;
						break;
					default:
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResAckRxStart;
						break;
				}
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconResAckRxStart:
#if swRfPortDebug == imEnable
			P7_bit.no4 = 0U;
#endif
			/* Logger �� GW�̃f�[�^�`�������ɂ�����GW �� Logger��Ack Preamble���o ��M�J�n */
			SubSX1272RxFSK_DIO();										/* SubRFDrv_Wakeup_SyncDetect()��SubSX1272RxFSK_On()���s */
//			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_ACK );
			SubSX1272RxFSK_On();
			SubIntervalTimer_Sleep( 10U * 375U, 1U );					/* GW��ACK���M����4.9msec */
			SubSX1272RxFSK( RF_HSMODE_ACK_LENGTH );
//			SubSX1272Stdby();
#if swRfPortDebug == imEnable
			P7_bit.no4 = 1U;
#endif
			wkRadio = SUCCESS;
			if( !gvRfIntFlg.mRadioTimer )
			{
				/* CRC�`�F�b�N */
				if( SubRfDrv_Crc_Decoding_HS( &vrfHsmodeAckBuff.umData[ 0U ], RF_HSMODE_ACK_LENGTH, BCH_OFF ) == SUCCESS )
				{
					SubSX1272Sleep();
					/* Ack or Nack�̔��� */
					/* �ʐM�����GW�łȂ� or ���@���łȂ�:���M�I�� */
					if( memcmp( vrfHsmodeAckBuff.umCategories.mGwID, vrfCallID, 3U ) != 0U ||
						memcmp( vrfHsmodeAckBuff.umCategories.mLoggerID, vrfMySerialID.mUniqueID, 3U ) != 0U )
					{
						wkRadio = FAIL;
					}
					if( wkRadio == SUCCESS )
					{
						wkRadio = FAIL;
						switch( vrfHsReqCmdCode )
						{
							case ecRfDrvGw_Hs_ReqCmd_Measure:
								/* �v���l�ʐM */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x0E )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_AlartHis:
								/* �x�񗚗�ʐM */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x0F )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
								/* �@��ُ헚��ʐM */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x15 )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_EventHis:
								/* �C�x���g����ʐM */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x16 )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_OpeHis:
								/* ���엚��ʐM */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x17 )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_SysHis:
								/* �V�X�e������ʐM */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x18 )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_SetRead:
								/* �ݒ�ǂݍ��� */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x08 )
								{
									wkRadio = SUCCESS;
								}
								break;
							default:
								/* �p�P�b�g�Ⴂ�@�I�� */
								break;
						}
					}
					if( wkRadio == SUCCESS )
					{
						vLoopLoRa = 0U;			/* ���M�����ɂ��L�����A�Z���X���[�v0�N���A */
						
						/* ��M���� */
						if( vrfHsmodeAckBuff.umCategories.mResponce == 0x01 )
						{
							/* NACK��M �p�P�b�g�đ� (���ӁF�i��NACK�������Ɖi�v���[�v����) */
							/* �|�C���^�ύX�Ȃ� */
							SubRfDrv_Hsmode_RptTx();
//							vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;
//							SubIntervalTimer_Sleep( 2U * 375U, 1U );
//							gvModuleSts.mRf = ecRfModuleSts_Sleep;
						}
						else
						{
							/* ACK��M */
							if( vrfPacketSum > 0U )
							{
								vrfPacketSum--;					/* ���̃p�P�b�g */
							}
							vErrorLoopCounter = 0U;				/* ���g���C�񐔃J�E���g�ϐ����Z�b�g */
							
							if( vrfLogSendPacketCtl.mFlashReadFin[ 0U ] == ecLogSndCtl_SndFin )
							{
								vrfPacketPt[1U] += 2U;
							}
							else if( vrfLogSendPacketCtl.mFlashReadFin[ 1U ] == ecLogSndCtl_SndFin )
							{
								vrfPacketPt[0U] += 2U;
							}
							else
							{
								if(gvrfStsEventFlag.mHsmodeTxTurn == 0U )
								{
									/* ���̃f�[�^�Z�b�g����Ă���ꍇ�͋�����̑��M������ */
									vrfPacketPt[1U] += 2U;
								}
								else
								{
									/* ������̃f�[�^�Z�b�g����Ă���ꍇ�͊��̑��M������ */
									vrfPacketPt[0U] += 2U;
								}
							}
							SubRfDrv_Hsmode_RptTx();
//							vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;
//							SubIntervalTimer_Sleep( 2U * 375U, 1U );
//							gvModuleSts.mRf = ecRfModuleSts_Sleep;
						}
					}else
					{
						SubRFDrv_Hsmode_End();
					}
				}else
				{
					SubSX1272Sleep();
					/* CRC�G���[ */
					if( vErrorLoopCounter < 10U )
					{
						/*	�p�P�b�g�đ� */
						vErrorLoopCounter ++;
						/* �|�C���^�ύX�Ȃ� */
						SubRfDrv_Hsmode_RptTx();
//						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;
//						SubIntervalTimer_Sleep( 2U * 375U, 1U );
//						gvModuleSts.mRf = ecRfModuleSts_Sleep;
					}else
					{
						/* ���g���C�񐔃I�[�o�[ ���M�I�� */
						SubRFDrv_Hsmode_End();
					}
				}
			}else
			{
				/* Ack��M�^�C���A�E�g */
				if( vErrorLoopCounter < 10U )
				{
					/*	�p�P�b�g�đ� */
					vErrorLoopCounter ++;
					/* �|�C���^�ύX�Ȃ� */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}else
				{
					/* ���g���C�񐔃I�[�o�[ ���M�I�� */
					SubRFDrv_Hsmode_End();
				}
			}
			
			/* Flash���烊�[�h�����f�[�^�𖳐��o�b�t�@�Ɋi�[ */
			if( vrfPacketSum > 0 )
			{
				SubRFDrv_Hsmode_Logger_ResponceDataSet( 1U );					/* Pointer gvrfHsmodePacketPt���Z�b�g */
			}
			
			if( vRfDrvMainLoggerSts == ecRfDrvMainSts_HSmode_BeaconResTxStart )
			{
				/* ���K�[�����f�[�^�̃p�P�b�g�|�C���^��ݒ� */
				SubRFDrv_Hsmode_PcktPointerSet();
				
				/* ���̑��M�|�C���^�Z�b�g */
				SubRfDrv_Hsmode_Logger_SndPtSet();
			}
			break;
			
		/* �ݒ�ύX��M */
		case ecRfDrvMainSts_HSmode_SettingRx:
#if 0		/* �������݃G���[CRC���� */
			SubSX1272Sleep();
			SubSX1272RxFSK_DIO();										/* SubRFDrv_Wakeup_SyncDetect()��SubSX1272RxFSK_On()���s */
			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_ACK );
			SubSX1272RxFSK_On();
#if swRfPortDebug == imEnable
			P7_bit.no4 = 1U;
#endif
			SubIntervalTimer_Sleep( 60U * 375U, 1U );					/* ��M�f�[�^�� 12msec Ack���MWait 50msec */
			SubSX1272RxFSK( RF_HSMODE_RES_SETWRITE_LENGTH );
			SubSX1272Sleep();
#if swRfPortDebug == imEnable
			P7_bit.no4 = 0U;
#endif
#if 0
			vrfInitState = ecRfDrv_Init_Non;
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
#endif
#else
			SubSX1272RxFSK_DIO();
			SubSX1272RxFSK_On();
			SubIntervalTimer_Sleep( 80U * 375U, 1U );
			SubSX1272RxFSK( RF_HSMODE_RES_SETWRITE_LENGTH );
			SubSX1272Sleep();
#endif
			wkRadio = SUCCESS;
			if( !gvRfIntFlg.mRadioTimer )
			{
				/* CRC�`�F�b�N */
				if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[ 0U ], RF_HSMODE_RES_SETWRITE_LENGTH_HEADER, BCH_OFF ) == SUCCESS )
				{
					/* Ack or Nack�̔��� */
					if( memcmp( vutRfDataBuffer.umGwWriteSet.mGwID, vrfCallID, 3U ) != 0U )
					{
						/* �ʐM�����GW�łȂ� ���M�I�� */
						wkRadio = ERROR;
						
						/* ���엚��(�s���ݒ�v���A��������) */
						wkU32 = (vutRfDataBuffer.umGwWriteSet.mGwID[0U] << 12U) + (vutRfDataBuffer.umGwWriteSet.mGwID[1U] << 4U) + (vutRfDataBuffer.umGwWriteSet.mGwID[2U] >> 4U);
						ApiFlash_WriteActLog( ecActLogItm_DisSetReq, wkU32, 0U );
					}
					else if( memcmp( vutRfDataBuffer.umGwWriteSet.mLoggerID, vrfMySerialID.mUniqueID, 3U ) != 0U )
					{
						/* ���@���łȂ� ���M�I�� */
						wkRadio = ERROR;
					}
					else if( vutRfDataBuffer.umGwWriteSet.mPacketCode != 0x0A )
					{
						/* ��M�p�P�b�g���Ⴄ */
						wkRadio = ERROR;
					}
					
					if( wkRadio == SUCCESS )
					{
						/* CRC�`�F�b�N */
						CRCD = 0x0000U;
						for( wkRadio = RF_HSMODE_RES_SETWRITE_LENGTH_HEADER; wkRadio < (RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_BCH - 2U); wkRadio++ )
						{
							CRCIN = vutRfDataBuffer.umData[wkRadio];
						}
						if((vutRfDataBuffer.umGwWriteSet.mCrc2[1U] == (uint8_t)(CRCD) )
								&& (vutRfDataBuffer.umGwWriteSet.mCrc2[0U] == (uint8_t)(CRCD >> 8U) ) )
						{
							/* �G���[�Ȃ� Ack���� */
//							vrfTest++;
						}
						else
						{
							/* �G���[���� �G���[�������� 20�f�[�^���Ƃɏ���(�[���̓��[�v�O�ŏ���) */
							for( wkRadio = 0U; wkRadio < 7U; wkRadio++ )
							{
								vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( &vutRfDataBuffer.umData[ RF_HSMODE_RES_SETWRITE_LENGTH_HEADER + ( wkRadio * 20U ) ] 
																			,RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_HEADER - RF_HSMODE_RES_SETWRITE_LENGTH_BCH -  ( wkRadio * 10U)
																			,20U );
							}
							vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( &vutRfDataBuffer.umData[ RF_HSMODE_RES_SETWRITE_LENGTH_HEADER + ( wkRadio * 20U ) ]
																		, RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_HEADER - RF_HSMODE_RES_SETWRITE_LENGTH_BCH -  ( wkRadio * 10U)
																		, RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_HEADER - RF_HSMODE_RES_SETWRITE_LENGTH_BCH - ( wkRadio * 20U ) );
							/* CRC�`�F�b�N */
							CRCD = 0x0000U;
							for( wkRadio = RF_HSMODE_RES_SETWRITE_LENGTH_HEADER; wkRadio < (RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_BCH - 2U); wkRadio++ )
							{
								CRCIN = vutRfDataBuffer.umData[wkRadio];
							}
							if((vutRfDataBuffer.umGwWriteSet.mCrc2[1U] == (uint8_t)(CRCD) )
									&& (vutRfDataBuffer.umGwWriteSet.mCrc2[0U] == (uint8_t)(CRCD >> 8U) ) )
							{
								/* �G���[�������� Ack���� */
//								vrfTest++;
//								vrfCorrectSetCnt++;
								wkRadio = SUCCESS;
							}
							else
							{
								/* �ʐM���s �p�P�b�g�j�� ������ */
								vrfResInfo = ecRfDrv_Fail;
//								vrfTestCrcErrorCnt++;
								wkRadio = ERROR;
							}
						}
					}
					else
					{
						/* ��M�p�P�b�g�G���[ */
						wkRadio = ERROR;
					}
				}
				else
				{
					/* �w�b�_�[CRC�G���[ */
					wkRadio = ERROR;
				}
			}
			else
			{
				/* ��M�^�C���A�E�g�G���[ */
				wkRadio = ERROR;
			}
			
#if swRfPortDebug == imEnable
			P7_bit.no4 = 1U;
#endif
			if( wkRadio != ERROR )
			{
				ApiInFlash_SetValWrite( &vutRfDataBuffer.umGwWriteSet );				/* RAM�ɐݒ�l������ */
				
				vLoopLoRa = 30U;
				SubRFDrv_Hsmode_Lg_To_Gw_AckDataSet();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxCs;
//				gvModuleSts.mRf = ecRfModuleSts_Run;
#if 0	/* ���K�[�ݒ菑������NG���� */
				SubIntervalTimer_Sleep( 26U * 375U, 1U );
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
#else
				if( gvRfIntFlg.mRadioTimer )
				{
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}else
				{
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
#endif
			}
			else
			{
				if( vErrorLoopCounter < 10U )
				{
					vErrorLoopCounter++;
					SubIntervalTimer_Sleep( 15U * 375U, 1U );					/* ���g���C�Ԋu93msec wait 10msec + RX 85msec  */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_SettingRx;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					SubRFDrv_Hsmode_End();
				}
			}
			break;
			
		/* �t�@�[��HEX��M */
		case ecRfDrvMainSts_HSmode_FirmHexRx:
			/* Firm Update Preamble���o ��M�J�n */
			SubSX1272RxFSK_DIO();
			SubSX1272RxFSK_On();
			SubIntervalTimer_Sleep( 100U * 375U, 2U );			/* GW���̃f�[�^�������Ԃ��l�� */
			SubSX1272RxFSK( RF_HSMODE_FIRM_HEX_LENGTH );		/* SubRFDrv_Wakeup_SyncDetect()��SubSX1272RxFSK_On()���s */
			SubSX1272Sleep();
			
			vrfInitState = ecRfDrv_Init_Non;
			vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxCs;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			vrfResInfo = ecRfDrv_Fail;
			
			if( !gvRfIntFlg.mRadioTimer )
			{
				//SubIntervalTimer_Stop();
				//SubIntervalTimer_Sleep( 50U * 375U, 1U );					/* GW�̑��M���Ԑ������l����50msec Wait */
				/* CRC�`�F�b�N */
				if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[ 0U ], RF_HSMODE_FIRM_HEX_LENGTH_HEADER, BCH_OFF ) == SUCCESS )
				{
					/* Ack or Nack�̔��� */
					if( memcmp( vutRfDataBuffer.umFirmHex.mGwID, vrfCallID, 3U ) == 0U )
					{
						if( memcmp( vutRfDataBuffer.umFirmHex.mLoggerID, vrfMySerialID.mUniqueID, 3U ) == 0 )
						{
							if( vutRfDataBuffer.umFirmHex.mPacketCode == 0x13 )
							{
								
								gvrfStsEventFlag.mHsmodeRxEnter = 1U;				/* ��M�J�n�t���O */
								vErrorLoopCounter = 0U;								/* ���g���C�J�E���^ ���Z�b�g */
								
								for( wkRadio = 0U; wkRadio < (RF_HSMODE_FIRM_HEX_LENGTH_HEX_CRC / 20U); wkRadio++ )		/* 45�u���b�N(�v900data CRC�܂�) */
								{
									/* �G���[�������� */
									vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( &vutRfDataBuffer.umFirmHex.mPacketNo[wkRadio * 20U] , RF_HSMODE_FIRM_HEX_LENGTH_HEX_CRC - (wkRadio * 10U) + 2U, 20U);
								}
								/* CRC�`�F�b�N */
								CRCD = 0x0000U;
								for( wkCulc = 0U; wkCulc < (RF_HSMODE_FIRM_HEX_LENGTH_HEX_CRC - 10U); wkCulc++ )
								{
									CRCIN = vutRfDataBuffer.umFirmHex.mHex[wkCulc];
								}
								
								if((vutRfDataBuffer.umFirmHex.mCrc2[1U] == (uint8_t)(CRCD) )
									&& (vutRfDataBuffer.umFirmHex.mCrc2[0U] == (uint8_t)(CRCD >> 8U) ) )
								{
									/* �G���[�������� Ack���� */
									vrfResInfo = ecRfDrv_Success;
//									vrfTest++;
//									vrfCorrectSetCnt++;
								}
								else
								{
									M_NOP;
									/* �ʐM���s �p�P�b�g�j�� Nack���� */
//									vrfTestCrcErrorCnt++;
								}
							}
							else
							{
								M_NOP;
								/* �p�P�b�g��ʃR�[�h�̃G���[ ������ */
							}
						}
						else
						{
							M_NOP;
							/* ���@���ł͂Ȃ� ������ */
						}
					}
					else
					{
						M_NOP;
						/* �ʐM��GW�ł͂Ȃ� */
					}
				}
				else
				{
					M_NOP;
					/* CRC�G���[ Nack���� */
//					vrfTestHeaderCrcError++;
				}
			}
			else
			{
				/* ��M�^�C���A�E�g Nack���� */
				//SubIntervalTimer_Sleep( 50U * 375U, 1U );
//				vrfTestHeaderTimeOut++;
			}
			
			SubSX1272Sleep();
			if( vrfResInfo == ecRfDrv_Success )
			{
				vLoopLoRa = 40U;
				//SubRFDrv_Hsmode_Logger_BcnAckDataSet();
				/* �t�@�[���A�b�v�f�[�^�i�[ */
				vrfResInfo = SubRfDrv_FirmStore( &vutRfDataBuffer.umFirmHex );
				
				/* ACK/NACK�f�[�^�Z�b�g */
				SubRFDrv_Hsmode_Lg_To_Gw_AckDataSet();
			}
			
			if( vrfResInfo != ecRfDrv_Success )
			{
				/* ERROR�̏ꍇ�͖��������� �^�C���A�E�g�ȊO�̃G���[�͉i�v���[�v�h�~���K�v����(ERROR�����͍l�����Ȃ���) */
				vErrorLoopCounter++;
				if( vErrorLoopCounter < 20U )
				{
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_FirmHexRx;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
					gvFirmupMode = imOFF;					/* �t�@�[���A�b�v�ʐM���s�������s */
					
					/* ERROR�̃��g���C�񐔂��I�[�o�[ */
					vErrorLoopCounter = 0U;
					SubRFDrv_Hsmode_End();
				}
			}
			else if( gvrfStsEventFlag.mHsmodeRxEnter == 0U )
			{
				gvFirmupMode = imOFF;						/* �t�@�[���A�b�v�ʐM���s�������s */
				/* �p�P�b�g��M�J�n���Ă��Ȃ���� */
				SubRFDrv_Hsmode_End();
			}
			break;
			
		case ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxCs:
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
			RTCMK = 1U;   /* disable INTRTC interrupt */
			/* Carrire Sence usec */
			if( SubRFDrv_CarrierSens_5ch( RF_HSMODE_CH[ gvInFlash.mParam.mrfHsCh ]) == NO_CARRIER )
			{
				vLoopLoRa = 0U;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				RTCMK = 0U;   /* enable INTRTC interrupt */
				SubSX1272Sleep();
				if( vLoopLoRa > 0U )
				{
					vLoopLoRa--;
					SubIntervalTimer_Sleep( 1U * 375U, 1U );
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxCs;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					SubRFDrv_Hsmode_End();
				}
			}
			break;
			
		case ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxStart:
			RTCMK = 0U;   /* enable INTRTC interrupt */
			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_FIRM_HEX );
			
			switch( vrfHsReqCmdCode )
			{
				case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
					if( SubSX1272TxFSK( RF_HSMODE_ACK_LENGTH ) == SUCCESS )
					{
						/* �t�@�[���̃p�P�b�g���������O��GW��������Ȃ���΂Ȃ�Ȃ� */
						if( vFirmPacketNum >= vFirmPacketSum )
						{
							SubRFDrv_Hsmode_End();
							
							if( gvModuleSts.mFirmup == ecFirmupModuleSts_Pre )
							{
								gvModuleSts.mFirmup = ecFirmupModuleSts_Run;
							}
						}
					}
					/* ���M�����������s������M���p������ */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_FirmHexRx;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					break;
				case ecRfDrvGw_Hs_ReqCmd_SetWrite:
					SubSX1272TxFSK( RF_HSMODE_ACK_LENGTH );
					SubSX1272Sleep();
					/* 3��Ack���M */
					vLoopLoRa++;
					if( vLoopLoRa < 3U )
					{
						SubIntervalTimer_Sleep( 1U * 375U, 1U );
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxStart;
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
					}
					else
					{
						/* RAM�ɐݒ�l������(���M�����������s�����ݒ�l�i�[) */
						gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
						SubRFDrv_Hsmode_End();
					}
					break;
				default:
					SubRFDrv_Hsmode_End();
					break;
				}
			break;
			
		/*************** ���K�[ LoRa���[�h ********************/
		case ecRfDrvMainSts_RTmode_Init:
			/* 2022.9.16 �d��������ɂ����ڑ��ʐM */
			gvRfTimingCounterH = 30;
			gvrfStsEventFlag.mReset = RFIC_INI;
#if 0
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
			rfStatus.Modem = MODEM_LORA;
			SubSX1272Sleep();
			/* �H��o�׎� �ݒ�l */
			rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
			vrfMySerialID.mUniqueID[0U] = gvInFlash.mProcess.mUniqueID[ 0U ];
			vrfMySerialID.mUniqueID[1U] = gvInFlash.mProcess.mUniqueID[ 1U ];
			vrfMySerialID.mUniqueID[2U] = gvInFlash.mProcess.mUniqueID[ 2U ];
			vrfMySerialID.mOpeCompanyID = gvInFlash.mProcess.mOpeCompanyID;
			
			/* ���[�U�[ �ݒ�l */
//			gvInFlash.mParam.mOnCertLmt = CENTRAL;
//			gvInFlash.mParam.mOnCertLmt = LOCAL;
//			gvInFlash.mParam.mrfLoraChGrupeCnt = 0;
			vrfMySerialID.mGroupID = gvInFlash.mParam.mGroupID;
			/* AppID�̓W�J �����l,���[�J�����[�h���͌Œ�l */
			if( (gvInFlash.mParam.mAppID[0U] != APP_ID_LOGGER_INIT_UP)
				 || (gvInFlash.mParam.mAppID[1U] != APP_ID_LOGGER_INIT_LO) )
			{
				if( gvInFlash.mParam.mOnCertLmt == CENTRAL )
				{
					vrfMySerialID.mAppID[0U] = ((gvInFlash.mParam.mAppID[ 0U ] << 4U) & 0xF0)
										+ ((gvInFlash.mParam.mAppID[ 1U ] >> 4U) & 0x0F);
					vrfMySerialID.mAppID[1U] = (gvInFlash.mParam.mAppID[ 1U ] << 4U) & 0xF0;
				}else
				{
					vrfMySerialID.mAppID[0U] = APP_ID_LOCAL_UP;
					vrfMySerialID.mAppID[1U] = APP_ID_LOCAL_LO;
				}
			}else
			{
				vrfMySerialID.mAppID[0U] = APP_ID_LOGGER_INIT_UP_TOP_ALIGN;
				vrfMySerialID.mAppID[1U] = APP_ID_LOGGER_INIT_LO_TOP_ALIGN;
			}
#if 0
			vrfGwConnectList[0U][0U] = 0x00;
			vrfGwConnectList[0U][1U] = 0x00;
			vrfGwConnectList[0U][2U] = 0x00;
#endif
			
			/* ���[�U�[�ݒ�l */
//			vrfLoraChGrupeMes = 1U;			/* GW���擾 */
//			vrfLoraChGrupeCnt = 25U;
//			vRtmode_DataCh[ 0U ] = RF_RTMODE_CH_GRUPE[ vrfLoraChGrupe ].mCh1;
//			vRtmode_DataCh[ 1U ] = RF_RTMODE_CH_GRUPE[ vrfLoraChGrupe ].mCh2;
//			vRtmode_DataCh[ 2U ] = RF_RTMODE_CH_GRUPE[ vrfLoraChGrupe ].mCh3;
//			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_CNCT[ vrfLoraChGrupeCnt ].mLoraSync );
			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mLoraSync );
			SubRfDrv_PwChange( gvrfStsEventFlag.mRtmodePwmode );
//			vHsmode_Ch[ 0U ] = 64U;
//			vHsmode_Ch[ 1U ] = 69U;
//			vHsmode_Ch[ 2U ] = 74U;

//			vrfTempCal = (sint16_t)(ApiRfDrv_Temp());
//			vrfTempCal = (sint16_t)((gvMeasPrm.mMeasVal[0U] / 10) - vrfTempCal);
#endif
			/* break�Ȃ� */
			
		case ecRfDrvMainSts_RTmode_BeaconInit:
			/* ����I�ȏ��������� */
			if( gvrfStsEventFlag.mReset == RFIC_INI )
			{
				SubRfDrv_RfReset();
				gvrfStsEventFlag.mReset = RFIC_RUN;
			}
			rfStatus.Modem = MODEM_LORA;
			SubSX1272Sleep();
			SubRFDrv_LoRa_Tx_Setting( RT_MODE_PAYLOAD_LENGTH_1, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7 );
			if( vCallChLoopLora >= RT_MODE_CH_LOOP_NUM ){	vCallChLoopLora = 0U;	}
//			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[ vrfLoraChGrupeCnt ].mCh[vCallChLoopLora], rfFreqOffset );
			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mCh[ vCallChLoopLora ], rfFreqOffset );
//			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_CNCT[ vrfLoraChGrupeCnt ].mLoraSync );
			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mLoraSync );
			//gvrfStsEventFlag.mRtmodePwmode = PW_H;
			SubSX1272Write( REG_LR_LNA , RFLR_LNA_GAIN_G1 | RFLR_LNA_BOOST_ON );
			SubRfDrv_PwChange( PW_H );
			vLoopLoRa = 0U;
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272Write( REG_PACKETCONFIG1,
				RF_PACKETCONFIG1_PACKETFORMAT_FIXED | RF_PACKETCONFIG1_DCFREE_OFF
				 | RF_PACKETCONFIG1_CRC_OFF | RF_PACKETCONFIG1_CRCAUTOCLEAR_OFF
				 | RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT  );
			SubSX1272CarrierSenseFSK_DIO();
			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
			rfStatus.Modem = MODEM_LORA;
			SubSX1272Sleep();
#if (swRssiLogDebug == imEnable)
			SubRfDrv_RssiLogDebug();
#endif
#if 0
#if (swRfTestLcd == imEnable)
			if( !M_TSTFLG(gvRfTestLcdFlg) )
			{
				ApiLcd_Upp7SegNumDsp( vrfMySerialID.mUniqueID[2U], imOFF, imOFF);
//				ApiLcd_Upp7SegNumDsp( ((uint32_t)(vrfDebugRTmodeSuccess) * 100U) / ((uint32_t)(vrfDebugRTmodeCnt) * 100U), imOFF, imOFF);
//				ApiLcd_Low7SegNumDsp( vrfErorrCause, imOFF );
				ApiLcd_Low7SegNumDsp( gvRfTimingCounterH, imOFF );				/* �L�����A�Z���X�ň������������ꍇ��+100U */
				ApiLcd_Main();
			}
#endif
#endif
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconDataSet;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureInit:
			if( vRfDrv_CommIntMode == ecRfDrv_LongIntMode )
			{
				vrfMesureCommIntTime = RT_INT_LONG_MODE_TX_INT;
				vrfSetCommTime = RT_INT_LONG_MODE_SET_TIME;
			}
			else if( vRfDrv_CommIntMode == ecRfDrv_MidIntMode )
			{
				vrfMesureCommIntTime = RT_INT_MID_MODE_TX_INT;
				vrfSetCommTime = RT_INT_MID_MODE_SET_TIME;
			}
			else if( vRfDrv_CommIntMode == ecRfDrv_ShortIntMode )
			{
				vrfMesureCommIntTime = RT_INT_SHORT_MODE_TX_INT;
				vrfSetCommTime = RT_INT_SHORT_MODE_SET_TIME;
			}
			SubRFDrv_LoRa_Tx_Setting( RT_MODE_PAYLOAD_LENGTH_5, 10U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			if( vCallChLoopLora >= RT_MODE_CH_LOOP_NUM ){	vCallChLoopLora = 0U;	}
			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_MEASURE[ vrfLoraChGrupeMes ].mCh[vCallChLoopLora], rfFreqOffset );
			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_MEASURE[ vrfLoraChGrupeMes ].mLoraSync );
			SubSX1272Write( REG_LR_LNA , RFLR_LNA_GAIN_G1 | RFLR_LNA_BOOST_ON );
			SubRfDrv_PwChange( gvrfStsEventFlag.mRtmodePwmode );
			vLoopLoRa = 0U;
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
			rfStatus.Modem = MODEM_LORA;
			SubSX1272Sleep();

#if 0
#if (swRfTestLcd == imEnable)
			ApiLcd_Upp7SegNumDsp( (gvRfTimingCounterH & 0x00FF), imOFF, imOFF);
			ApiLcd_Low7SegNumDsp( vrfRTmodeTimeSlot, imOFF );
//			if( !M_TSTFLG(gvRfTestLcdFlg) )
//			{
				ApiLcd_Main();
//			}
#endif
#endif
#if (swRssiLogDebug == imEnable)
			SubRfDrv_RssiLogDebug();
#endif
			/* �ڑ�GW�̉��O��ID(LCD�\���p) */
			gvLcdComGwId = (vrfCommTargetID[ 1U ] << 4U) + (vrfCommTargetID[ 2U ] >> 4U);
			
			vrfInitState = ecRfDrv_Init_Rtmode_Measure;
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		/****** Logger Idle ******/
		case ecRfDrvMainSts_RTmode_LoggerIdle:
			wkRadio = gvRfTimingCounterH % 35UL;
			wkRadio2nd = gvRfTimingCounterH % vrfMesureCommIntTime;				/* 10,20,60sec������"0" */
//			vrfRTmodeCommInt = 0xFFFF;
			wkRtmodeInterval = gvRfTimingCounterH % vrfRTmodeCommInt;			/* ���������"0" */
//			wkHsmodeInterval = gvRfTimingCounterH % 6U;
			wkHsmodeInterval = gvRfTimingCounterH % 6UL;
			/* �x�񔭐����ɗ��t���O������΁A������`�F�b�N���AwkRadio2nd��0���t���O�Z�b�g����Ă����wkRtmodeInterval��0�ɂ��� */
			
			gvFirmupMode = imOFF;												/* �t�@�[���A�b�v�����s */
			
			if( wkRadio2nd == 0U )
			{
				/* RT mode�v���l�ʐM�C���^�[�o���ł�0sec */
				if( gvRfTimingCounterL == 0U )
				{
#if 0
					/* ���x�ɂ��^�C�~���O�␳ */
					if( vRfDrv_CommIntMode == ecRfDrv_LongIntMode )
					{
						ApiRfDrv_TimingCorrect();					/* �ʐM�����̒����ꍇ�̂ݏC�� */
					}else
					{
						gvRfTimingTempCorrect = 0U;
					}
#endif
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
					gvrfStsEventFlag.mSetReserve = 0U;			/* 1set��Tx�Œ��߂� */
					vrfRtmodeCommTimeTotal = 0U;				/* ���M���Ԑ����p�̐ݒ�ύX�񐔂��N���A */
					gvrfStsEventFlag.mTempMeasure = 0U;
					/* 2021.8.2 */
					if( vrfRtmodeStopCnt > 0U )
					{
						vrfRtmodeStopCnt--;								/* �����ʐM�ł̌��m�ŃZ�b�g */
					}
					
					/* ����x�񔭐�/��������F�����ݒ������v���x��ʐM���{ */
					if( gvMeasAlmNum )
					{
						wkRtmodeInterval = 0U;
						/* 2021.8.2 */
						vrfRtmodeStopCnt = 0U;
					}
					
//					vrfInitState = ecRfDrv_Init_Non;
					if( (vrfRTmodeCommFailNum >= vrfRTmodeCommCutTime) || (vrfRTmodeCommInt == 0xFFFF) )
					{
						/* GW�ڑ��������� */
						/*�f�o�b�O
						if( vrfDebugRTmodeCnt > 0U )
						{
							vrfDebugRTmodeCnt = 0U;
						}*/
						
						SubRfDrv_SetRtConInfo( imOFF );			/* �ڑ����� */
						vrfConnectSuccess = 0U;
						/* 2021.8.27 */
						vrfRtmodeStopCnt = 0U;
						vrfRssi = 0U;
						vrfRTmodeCommFailNum = 0U;				/* ���s�񐔃��Z�b�g */	/* 2021.8.24 �폜 �� 2021.9.22���� */
						SubRfDrv_RtConctTiAdd();				/* ����ڑ��r�[�R���̃^�C�~���O���� */
						vrfCommTargetID[0U] = 0x00;
						vrfCommTargetID[1U] = 0x00;
						vrfCommTargetID[2U] = 0x00;
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconInit;
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
						break;			/* BeaconInit�ɑJ�ڂ��邽�� */
					}
					
					if( (vrfConnectSuccess == 0U) && (gvRfTimingCounterH > 315U) )
					{
						/* �������x�擾 */
						ApiRfDrv_Temp();
						SubRfDrv_RtConctTiAdd();				/* �ڑ��ʐM�̃^�C�~���O�����炷 */
						/* 2021.7.12 �ǉ� */
						vrfInitCnt ++;
						if( vrfInitCnt > RF_INI_CNT )
						{
							vrfInitCnt = 0U;
							gvrfStsEventFlag.mReset = RFIC_INI;
							/* ����IC�G���[ ����or�\�� */
							vRfTestFlg = 1U;

						}
					}
					/* 2021.7.12 �ǉ� */
					if( vrfRfErr > RF_ERR_CNT )
					{
						/* ����IC�G���[ ����or�\�� */
						vRfTestFlg = 2U;
					}

					if( vRfDrv_CommIntMode != ecRfDrv_LongIntMode )
					{
						/* 10,20sec�C���^�[�o������1min���Ƃɉ��x�X�V */
						ApiRfDrv_Temp();
					}
#if 0
					else if( vrfConnectSuccess && (vrfInitState != ecRfDrv_Init_Rtmode_Measure) )
					{
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
						gvModuleSts.mRf = ecRfModuleSts_Run;
						break;			/* MeasureInit�ɑJ�ڂ��邽�� */
					}
#endif
#if (swRssiLogDebug == imEnable)
					vrfErorrCause = 0U;
#endif
				}
			}

#if swRfOffMode == imEnable
			if( vrfRTmodeCommInt == 0xFFFF )
			{
				/* RTmode OFF */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
				vrfErorrCause = 28U;
#if (swRssiLogDebug == imEnable)
				SubRfDrv_RssiLogDebug();
#endif
				break;
			}
#endif
			/* �d�r�����̏��� */	
			if( gvrfStsEventFlag.mPowerInt == BATT_POWER_OFF )
			{
				ApiRfDrv_ForcedSleep();
				vrfErorrCause = 26U;
#if (swRssiLogDebug == imEnable)
				SubRfDrv_RssiLogDebug();
#endif
				break;
			}
			
			if( (wkRadio == 0U) && (vrfConnectSuccess == 0U) && (vrfRtmodeStopCnt == 0U) )			/* 2021.8.2 �����ʐM���ɔ�΂� */
			{
#if swRfOffMode == imEnable
#else
				if( vrfRTmodeCommInt == 0xFFFF )
				{
					/* RTmode OFF */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
					break;
				}
#endif
				/* 35sec���� 0sec ���ڑ���Ԃł̐ڑ��ʐM */
				if( gvRfTimingCounterL < 2U )				/* ���������̃��[�v�l�� */
				{
					/* 35sec���� RTmode BeaconTx */
					if( vrfInitState == ecRfDrv_Init_Rtmode_Beacon )
					{
						/* �����ݒ�ς� */
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconCarrierSens;
					}
					else
					{
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconInit;
					}
					SubIntervalTimer_Sleep( rand() % 20U * 375U, 1U );		/* �ڑ��ʐM�̃^�C�~���O�����炷 */
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					/* �������M�̖h�~(Sleep������Run��ԂőJ�ڂ����ꍇ) */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				break;
			}
//			if( (wkRtmodeInterval < (vrfMesureCommIntTime - vrfSetCommTime)) && (vrfConnectSuccess != 0U) && (vrfRtmodeStopCnt == 0U) )			/* 2021.8.2 �����ʐM���ɔ�΂� */
			if( (wkRtmodeInterval < (vrfMesureCommIntTime - vrfSetCommTime)) && (vrfConnectSuccess != 0U) )			/* 2021.8.2 �����ʐM���ɔ�΂� �ˁ@2021.8.24 ���蕔�����Ɉړ� */
			{
				/* 1-39sec �����d�������K�v */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
				if( vrfRTmodeTimeSlot > 0U )
				{
					/* GW�ڑ��ς� */
					wkCulc = (vrfRTmodeTimeSlot - 1U) * 660U;			/* msec�ɕϊ� max39,600U */
					if( wkRadio2nd == wkCulc / 1000U )			/* �b�P�ʂ̔��� */
					{
						if(vrfInitState == ecRfDrv_Init_Rtmode_Measure)
						{
							/* GW�t�B���^�����O�̃`�F�b�N */
							if( SubRFDrv_Filter_Gwid( &vrfCommTargetID[0U] ) == ecRfDrv_Fail )
							{
								vrfRTmodeCommFailNum = vrfRTmodeCommCutTime;
								break;
							}
							/* 2021.8.24 �����ʐM�͈͓��̂��ߒʐM���f */
							if( vrfRtmodeStopCnt > 0U )
							{
								vrfRTmodeCommFailNum++;
								break;
							}

							/* �ʐM�����̃`�F�b�N */
							
							/* RTmode�ł̑��M�J�n(wait�t��) */
							if( RT_MODE_TIME_SLOT_IT_COUNT[vrfRTmodeTimeSlot - 1U] > 0U )
							{
								/* 0x0�͏��� */
#if 0
								SubIntervalTimer_Sleep( RT_MODE_TIME_SLOT_IT_COUNT[vrfRTmodeTimeSlot - 1U], 12U );			/* 87msec�܂� */
#else
								SubIntervalTimer_Sleep( RT_MODE_TIME_SLOT_IT_COUNT[vrfRTmodeTimeSlot - 1U] + 2812UL, 12U );			/* �^�C�~���O�����p+90msec( 2812 * 12 ) */
#endif
								gvModuleSts.mRf = ecRfModuleSts_Sleep;
							}
							else
							{
#if 0
								gvModuleSts.mRf =ecRfModuleSts_Run;
#else
								SubIntervalTimer_Sleep( 60U * 375U, 1U );			/* �^�C�~���O������l�������M��x�点�� */
								gvModuleSts.mRf = ecRfModuleSts_Sleep;
#endif
							}
							vrfDebugRTmodeCnt++;				/* Debug�p ���M�񐔃J�E���g */
							if( vrfDebugRTmodeCnt >= 0xFFFFU )
							{
								vrfDebugRTmodeCnt = 1U;
							}
							/* �v���l���M(�x�񗚗𑗐M) */
							vLoopLoRa = 0U;
							
							/* �v���x�񔭐�/�����񐔂���F�񐔕�Flash���烊�[�h */
							if( gvMeasAlmNum > 0U )
							{
								/* �v���x�񗚗��̑��M */
								vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasAlmDataSet;
								
								if( gvMeasAlmNum > 6U )
								{
									vRtMode_MeasAlmSndKosu = 6U;
								}
								else
								{
									vRtMode_MeasAlmSndKosu = gvMeasAlmNum;
								}
								
								SubRfDrv_RtmodeMeasAlmOldPtrUpdate( vRtMode_MeasAlmSndKosu );	/* Flash���[�hIndexNo.���Z�b�g */
								SubRfDrv_RtmodeMeasDataFlashRead( MEASALM_DATASET );			/* Flash����������[�h(�x�񗚗�) */
							}
							else
							{
								/* �v���l�̑��M */
								vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureDataSet;
								
								SubRfDrv_RtmodeDataFlashReadKosu();							/* �v���l�ߋ����M�� */
								if( vRtMode_MeasSndKosu != 0U )
								{
									SubRfDrv_RtmodeMeasDataFlashRead( MEASURE_DATASET );	/* Flash����������[�h(����l) */
								}
							}
						}
						else
						{
							/* ���������K�v */
							//vrfDebug[0U]++;
							vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
							gvModuleSts.mRf = ecRfModuleSts_Run;
						}
					}
					else if( wkHsmodeInterval == 0U )
					{
						/* Measure���M�^�C�~���O�ȊO�͍������[�h��M�ҋ@ */
						gvrfStsEventFlag.mHsmodeSyncDetect = 1U;
						if( vrfInitState == ecRfDrv_Init_Hsmode )
						{
							vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconPre1stDetect;
						}
						else
						{
							vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconCsInit;
						}
						gvrfStsEventFlag.mHsmodeDetectLoop = 0U;
						gvModuleSts.mRf =ecRfModuleSts_Run;
					}
				}
				else
				{
					/* GW�ڑ���Ԃ���TimeSlot == 0U ?? */
//					vrfConnectSuccess = 0U;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
					//vrfDebug[1U]++;
					SubRfDrv_SetRtConInfo( imOFF );		/* �ڑ����Ȃ��� */
				}
				break;
			}
			
			/* 40-59sec �ݒ�ύX���ɓ��� �ݒ�ύX�\�񂪂Ȃ��ꍇ�͍������[�h */
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			
			if( gvrfStsEventFlag.mSetReserve && (vrfRtmodeCommTimeTotal < 8U) )				/* ���g���C��8��܂� */
			{
				vrfRtmodeCommTimeTotal++;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetBeaconInit;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			/* ���x�ɂ��^�C�~���O�␳ */
#if 1
			else if( ( wkRadio2nd == vrfMesureCommIntTime - 1U ) 
					&& ( !gvrfStsEventFlag.mTempMeasure )
				&& ( vrfMesureCommIntTime == RT_INT_LONG_MODE_TX_INT )
				&& ( C0ENB == 0U ) )													/* ����p�R���p���[�^ON���͎��̃^�C�~���O */
			{
				ApiRfDrv_TimingCorrect();					/* �ʐM�����̒����ꍇ�̂ݏC�� */
				gvrfStsEventFlag.mTempMeasure = 1U;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconInit;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
#endif
			else if( wkHsmodeInterval == 0U )
			{
				/* �������[�h */
				/* Measure���M�^�C�~���O�ȊO�͍������[�h��M�ҋ@ */
				gvrfStsEventFlag.mHsmodeSyncDetect = 1U;
				if( vrfInitState == ecRfDrv_Init_Hsmode )
				{
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconPre1stDetect;
				}
				else
				{
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconCsInit;
				}
				gvrfStsEventFlag.mHsmodeDetectLoop = 0U;
				gvModuleSts.mRf =ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_BeaconDataSet:
			vutRfDataBuffer.umRT_Logger_Beacon.mCompanyID = gvInFlash.mProcess.mOpeCompanyID;
			vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[0U] = ((gvInFlash.mParam.mAppID[ 0U ] << 4U) & 0xF0)
																 + ((gvInFlash.mParam.mAppID[ 1U ] >> 4U) & 0x0F);
			vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[1U] = (gvInFlash.mParam.mAppID[ 1U ] << 4U) & 0xF0;
			vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[1U] += gvInFlash.mParam.mOnCertLmt << 1U;
			vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[1U] &= 0xFE;
			
			SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_1 - 2U );
			
			vrfInitState = ecRfDrv_Init_Rtmode_Beacon;
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_RTmode_BeaconCarrierSens:
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			if( vCallChLoopLora >= RT_MODE_CH_LOOP_NUM ){	vCallChLoopLora = 0U;	}
//			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[vrfLoraChGrupeCnt].mCh[vCallChLoopLora], rfFreqOffset );
			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mCh[ vCallChLoopLora ], rfFreqOffset );
			SubSX1272CarrierSenseFSK_DIO();
//			RTCMK = 1U;   /* disable INTRTC interrupt */
			
			/* Carrire Sence 125usec */
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconTxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				if( SubRfDrv_CarrierSens_Rt_RetryDecision( 10U * 375U, CH_CHANGE, CS_RT_CONNECT_MODE) == NEXT )
				{
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconCarrierSens;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					vrfInitState = ecRfDrv_Init_Rtmode_Beacon;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
#if (swRssiLogDebug == imEnable)
					vrfErorrCause = 1U;
#endif
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_BeaconTxStart:
			/* ����d�����l�����ŏ��ݒ� �֐����g��Ȃ� */
			SubSX1272Sleep();
			rfStatus.Modem = MODEM_LORA;
			SubSX1272Sleep();
			SubSX1272Stdby();
			SubSX1272TxLoRa_DIO();
			SubSX1272Write( REG_LR_FIFOADDRPTR, 0x00 );
			SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_1);
			SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_1 );
			
			/* Tx Done Wait */
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			SubIntervalTimer_Sleep( 80U * 375U, 1U );			/* 80msec */
			gvrfStsEventFlag.mOnlineLcdOnOff = 1;				/* LCD ON 2022.9.16 */
			gvModuleSts.mLcd = ecLcdModuleSts_Run;
			
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconTxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_BeaconTxWait:
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ���M���Ƀ^�C���A�E�g(399.8msec) */
				/* �K���ȏ�̑��M���� */
				R_INTC6_Stop();
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */	
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 2U;
#endif
			}
			else
			{
				/* ���M���� */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAuthCad;
				SubIntervalTimer_Sleep( 4U + 10U * 375U, 1U );		/* 4msec + 10msec(GW�̃����_��wait�l��) */
				
				/* ���d�rAD���� */
				ApiAdc_AdExe();
				
				vLoopLoRa = 0U;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
//				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAuthCad:
			SubSX1272CadLoRa();
			if( gvRfIntFlg.mRadioDio4 )
			{
				/* CAD���m */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAuthRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else if( gvRfIntFlg.mRadioDio0 )
			{
				vCallChLoopLora++;		/* ����`���l���ύX */
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 3U;
#endif
			}
			else
			{
				/* �N���^�C���A�E�g */
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 4U;
#endif
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAuthRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_2, 22U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait �ݒ� */
			SubIntervalTimer_Sleep( 80U * 375U, 2U );		/* 160msec */
			/* ��M�҂����Ƀf�[�^�Z�b�g */
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAuthRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAuthRxWait:
			/* ��M�����҂� */
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* ��M���� */
				R_INTC6_Stop();
				SubSX1272Stdby();
				SubIntervalTimer_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAuthRxDataRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ��M�^�C���A�E�g */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
//				wkRadio = SubSX1272Read( REG_LR_IRQFLAGS );
				vCallChLoopLora++;			/* ����`���l���ύX */
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 5U;
#endif
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAuthRxDataRead:
			SubRFDrv_LoRa_Rx_Read( &wkRadio, &vrfCurrentRssi);
			SubSX1272Sleep();
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS )
			{
				/* CRC OK */
				wkRadio = FAIL;
				/* ���Ɖ��ID�m�F */
				if( vutRfDataBuffer.umRT_Gw_ConnAuth.mCompanyID == gvInFlash.mProcess.mOpeCompanyID )
				{
					/* ����App ID�̊m�F���� */
					if( (gvInFlash.mParam.mAppID[ 0U ] == APP_ID_LOGGER_INIT_UP) && (gvInFlash.mParam.mAppID[1U] == APP_ID_LOGGER_INIT_LO) )
					{
						wkRadio = SUCCESS;
					}
					/* App ID��v�m�F */
					if( (vutRfDataBuffer.umRT_Gw_ConnAuth.mAppID_GwUniqueID[0U] 
						== (((gvInFlash.mParam.mAppID[0U] << 4U) & 0xF0) + (( gvInFlash.mParam.mAppID[1U] >> 4U) & 0x0F) ))
					 && ((vutRfDataBuffer.umRT_Gw_ConnAuth.mAppID_GwUniqueID[1U] & 0xF0)
						== ((gvInFlash.mParam.mAppID[1U] << 4U) & 0xF0) )	)
					{
						wkRadio = SUCCESS;
					}
//					if( (wkRadio == SUCCESS) && (gvInFlash.mParam.mOnCertLmt == LOCAL) )
					if( wkRadio == SUCCESS )
					{
#if 1
						for( wkRadio2nd = 0U; wkRadio2nd < 10U; wkRadio2nd++ )
						{
							if( (gvInFlash.mParam.mCnctEnaGwId[ wkRadio2nd ][ 0U ] == 0x00) && (gvInFlash.mParam.mCnctEnaGwId[ wkRadio2nd ][ 1U ] == 0x00) && (gvInFlash.mParam.mCnctEnaGwId[ wkRadio2nd ][ 2U ] == 0x00) )
							{
								;
							}
							else
							{
								wkRadio = FAIL;
								break;
							}
						}
						/* �ʐM����GW�̌��� */
						for( wkRadio2nd = 0U; wkRadio2nd < 10U; wkRadio2nd++ )
						{
							if( (gvInFlash.mParam.mCnctEnaGwId[ wkRadio2nd ][ 0U ]
									== ((vutRfDataBuffer.umRT_Gw_ConnAuth.mAppID_GwUniqueID[1U] & 0x0F) << 4U)
									+ ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[0U] & 0xF0) >> 4U))
								&& ( gvInFlash.mParam.mCnctEnaGwId[ wkRadio2nd ][ 1U ]
									== ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[0U] & 0x0F) << 4U)
									+ ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[1U] & 0xF0) >> 4U))
								&& ( gvInFlash.mParam.mCnctEnaGwId[ wkRadio2nd ][ 2U ]
									== ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[1U] & 0x0F) << 4U)))
							{
								/* �ʐM����GW���X�g�ɂ��� */
								wkRadio = SUCCESS;
							}
						}
#else
						wkRadio = FAIL;
						vrfRssi = vrfCurrentRssi;
						/* GW�̋��ʐM���X�g�m�F */
#if 1
						if( (gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 0U ] == 0x00) && (gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 1U ] == 0x00) && (gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 2U ] == 0x00) )
						{
#else
						if( (vrfGwConnectList[0U][0U] == 0x00) && (vrfGwConnectList[0U][1U] == 0x00) && (vrfGwConnectList[0U][2U] == 0x00) )
						{
#endif
							wkRadio = SUCCESS;		/* GW�t�B���^�ݒ薳�� */
						}
						else
						{
							/* �ʐM����GW�̌��� */
							for( wkRadio2nd = 0U; wkRadio2nd < 10U; wkRadio2nd++ )
							{
#if 1
								if( (gvInFlash.mParam.mCnctEnaGwId[ wkRadio2nd ][ 0U ]
										== ((vutRfDataBuffer.umRT_Gw_ConnAuth.mAppID_GwUniqueID[1U] & 0x0F) << 4U)
										+ ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[0U] & 0xF0) >> 4U))
									&& ( gvInFlash.mParam.mCnctEnaGwId[ wkRadio2nd ][ 1U ]
										== ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[0U] & 0x0F) << 4U)
										+ ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[1U] & 0xF0) >> 4U))
									&& ( gvInFlash.mParam.mCnctEnaGwId[ wkRadio2nd ][ 2U ]
										== ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[1U] & 0x0F) << 4U)))
								{
#else
								if( (vrfGwConnectList[wkRadio2nd][0U]
										== ((vutRfDataBuffer.umRT_Gw_ConnAuth.mAppID_GwUniqueID[1U] & 0x0F) << 4U)
										+ ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[0U] & 0xF0) >> 4U))
									&& ( vrfGwConnectList[wkRadio2nd][1U]
										== ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[0U] & 0x0F) << 4U)
										+ ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[1U] & 0xF0) >> 4U))
									&& ( vrfGwConnectList[wkRadio2nd][2U]
										== ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[1U] & 0x0F) << 4U)))
								{
#endif
									/* �ʐM����GW���X�g�ɂ��� */
									wkRadio = SUCCESS;
								}
							}
						}
#endif
					}
				}
				else
				{
					/* ���Ɖ��ID���قȂ� */
				}
				
				if( wkRadio == SUCCESS )
				{
					/* �V�X�e��ID����v ���� �ʐM����GW */
					/*------------GW�^�C�~���O����------------ */
					R_IT_Stop();
					gvRfTimingCounterL = (vutRfDataBuffer.umRT_Gw_ConnAuth.mSyncTimerL[0U] << 8U) & 0xFF00;
					gvRfTimingCounterL += vutRfDataBuffer.umRT_Gw_ConnAuth.mSyncTimerL[1U];
					gvRfTimingCounterH = (vutRfDataBuffer.umRT_Gw_ConnAuth.mSyncTimerH[0U] << 8U) & 0xFF00;
					gvRfTimingCounterH += vutRfDataBuffer.umRT_Gw_ConnAuth.mSyncTimerH[1U];
					/* GW�����181msec�̒x��(TX����144msec/GW��Logger��������7msec/Beacon��M����DataSet30msec) */
					/* gvRfTimingCounterL��~0x07FF�܂� GW��IntTimer��0x000F */
//					gvRfTimingCounterL += 360U;
					gvRfTimingCounterL += 268U;			/* 144msec - 13msec(preamble)�݂̂ɕύX */
					vrfITSyncCount = gvRfTimingCounterL;
					gvRfTimingCounterL = ((gvRfTimingCounterL & 0x0F00) >> 8U) & 0x000F;		/* gvRfTimingCounterL��0-7��������8�܂� */
					vrfITSyncCount = (vrfITSyncCount << 4U) & 0x0FF0;			/* vrfITSyncCount��0-0x0FF0 */
					vrfITSyncCount = 0x0FF0 - (vrfITSyncCount & 0x0FF0) + 8U;		/* �Ō�̌���0x0��0xF�̊� */
					if( gvRfTimingCounterL < 7U )	{	gvRfTimingCounterL++;	}
					else
					{
						gvRfTimingCounterH++;
						gvRfTimingCounterL = gvRfTimingCounterL - 7U;
					}
					
					if( vrfITSyncCount )
					{
						gvrfStsEventFlag.mTimerSync = 1U;
						R_IT_Create_Custom( vrfITSyncCount );
					}
					else
					{
						R_IT_Create_Custom( 0xFFF );
					}
					
					/* TimeSlot */
//					vrfRTmodeTimeSlot = vutRfDataBuffer.umRT_Gw_ConnAuth.mTimeSlot;			/* �ڑ��̍ŏIAck��M���Ɉړ� */
					
					/* �ʐM�Ԋu�̎擾 */
					vrfPreRTmodeCommInt = vrfRTmodeCommInt;									/* ���݂�Rt�Ҏ�������L�� */
					if( vutRfDataBuffer.umRT_Gw_ConnAuth.mComInt <= 8U )
					{
						vrfRTmodeCommInt = cComIntList[ vutRfDataBuffer.umRT_Gw_ConnAuth.mComInt ];
					}
					else
					{
						vrfRTmodeCommInt = 300U;
					}
					
					if( vrfRTmodeCommInt == 0xFFFF )
					{
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconInit;
					}
					else
					{
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerInfoDataSet;
					}
					
					/* ���A���^�C���ʐM�������X�V���ꂽ�ꍇ */
					if( vrfRTmodeCommInt != vrfPreRTmodeCommInt )
					{
						/* �����ʐM�������� �� ���� */
						if( vrfRTmodeCommInt == 0xFFFF )
						{
							/* HS�̂ݑҎ��Ԃ֕ύX */
							M_SETBIT( gvInFlash.mParam.mParmFlg, imRtStbyOffFlg );
							gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;					/* ����Flash�����ݍX�V */
							gvModuleSts.mLcd = ecLcdModuleSts_Run;							/* LCD�\���X�V */
						}
						else
						{
							/* �����ʐM�������� �� ���� */
							if( vrfPreRTmodeCommInt == 0xFFFF )
							{
								/* HS-Rt�Ҏ��Ԃ֕ύX */
								M_CLRBIT( gvInFlash.mParam.mParmFlg, imRtStbyOffFlg );
								gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;				/* ����Flash�����ݍX�V */
								gvModuleSts.mLcd = ecLcdModuleSts_Run;						/* LCD�\���X�V */
							}
						}
					}
					
					if( vrfRTmodeCommInt == 10U )
					{
						vRfDrv_CommIntMode = ecRfDrv_ShortIntMode;
					}
					else if( vrfRTmodeCommInt == 20U )
					{
						vRfDrv_CommIntMode = ecRfDrv_MidIntMode;
					}
					else
					{
						vRfDrv_CommIntMode = ecRfDrv_LongIntMode;
					}
					
					/* GW���� */
					wkU32 = vutRfDataBuffer.umRT_Gw_ConnAuth.mOnline_Time[ 0U ];
					wkU32 |= (uint32_t)(vutRfDataBuffer.umRT_Gw_ConnAuth.mOnline_Time[ 1U ]) << 8U;
					wkU32 |= (uint32_t)(vutRfDataBuffer.umRT_Gw_ConnAuth.mOnline_Time[ 2U ]) << 16U;
					wkU32 |= (uint32_t)(vutRfDataBuffer.umRT_Gw_ConnAuth.mOnline_Time[ 3U ] & 0x7FU) << 24U;
#if 1
					ApiTimeComp_first( wkU32, 0U );
#else
/* debug�p */
					gvClock = ApiRtcDrv_localtime( wkU32/2 );
					/* ����RTC�Ɏ������C�g */
					ApiRtcDrv_SetInRtc( gvClock );
					
					/* ����RTC�̎������O�t��RTC�Ɏ������C�g */
					ApiRtcDrv_InRtcToExRtc();
#endif
					/* �v���ʐM�p�`���l���擾 */
					vrfLoraChGrupeMes = vutRfDataBuffer.umRT_Gw_ConnAuth.mChGrupe[0U];
					
					/* ���A���^�C���ʐM�ؒf���莞�� */
					vrfRTmodeCommCutTime = vutRfDataBuffer.umRT_Gw_ConnAuth.mComOffTime;
					
					/* �������ʐM�]���n�_�f�[�^ */
					gvInFlash.mParam.mRealTimeSndPt = (uint16_t)(vutRfDataBuffer.umRT_Gw_ConnAuth.mRecStartTime[ 0U ] << 8U);
					gvInFlash.mParam.mRealTimeSndPt += vutRfDataBuffer.umRT_Gw_ConnAuth.mRecStartTime[ 1U ];
					SubRfDrv_ChgRealTimeSndPt();											/* ���M�|�C���^�ύX */
					
					/* �x�񔭐�/�����񐔂�0�N���A */
					gvMeasAlmNum = 0U;
					
//					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerInfoDataSet;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
					/* �V�X�e��ID���s��v or GW�t�B���^ */
//					SubIntervalTimer_Sleep( 0xFFF, 16U );		/* 2sec */
					SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
					vrfErorrCause = 6U;
#endif
				}
			}
			else
			{
				/*CRC NG */
//				SubIntervalTimer_Sleep( 0xFFF, 16U );		/* 2sec */
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 7U;
#endif
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerInfoDataSet:
			/* ��������8.6msec */
			/* ��M�f�[�^�̑ޔ� */
			for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_2 - 2U; wkRadio++ )
			{
				vutRfDataBuffer.umData[wkRadio + 256U] = vutRfDataBuffer.umData[wkRadio];
			}
			/* �_�~�[�f�[�^�Z�b�g */
			for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_3 - 2U; wkRadio++ )
			{
				vutRfDataBuffer.umData[wkRadio] = wkRadio;
			}
			
			/* 3:���K�[��� */
			vutRfDataBuffer.umRT_LoggerInfo.mLoggerID[ 0U ] = gvInFlash.mProcess.mUniqueID[ 0U ];					/* 1:���j�[�ND */
			vutRfDataBuffer.umRT_LoggerInfo.mLoggerID[ 1U ] = gvInFlash.mProcess.mUniqueID[ 1U ];
			vutRfDataBuffer.umRT_LoggerInfo.mLoggerID[ 2U ] = gvInFlash.mProcess.mUniqueID[ 2U ];
			for( wkLoop = 0U ; wkLoop < imLoggerNameNum ; wkLoop++ )
			{
				vutRfDataBuffer.umRT_LoggerInfo.mLoggerName[ wkLoop ] = gvInFlash.mParam.mLoggerName[ wkLoop ];	/* 2:���K�[���� */
			}
			vutRfDataBuffer.umRT_LoggerInfo.mGroupID = gvInFlash.mParam.mGroupID;									/* 3:�O���[�vID */
			vutRfDataBuffer.umRT_LoggerInfo.mGwID[ 0U ] = (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxAppID_GwUniqueID[ 1U ] << 4U) & 0xF0;	/* 4:GW�V���A��ID */
			vutRfDataBuffer.umRT_LoggerInfo.mGwID[ 0U ] += (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[ 0U ] >> 4U) & 0x0F;
			vutRfDataBuffer.umRT_LoggerInfo.mGwID[ 1U ] = (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[ 0U ] << 4U) & 0xF0;
			vutRfDataBuffer.umRT_LoggerInfo.mGwID[ 1U ] += (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[ 1U ] >> 4U) & 0x0F;
			vutRfDataBuffer.umRT_LoggerInfo.mGwID[ 2U ] = (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[ 1U ] << 4U) & 0xF0;
//			vutRfDataBuffer.umRT_LoggerInfo.mLoggerSensorType = (uint8_t)cSensType;								/* 5:�Z���T�� */
			vutRfDataBuffer.umRT_LoggerInfo.mLoggerSensorType = (uint8_t)gvInFlash.mProcess.mModelCode;			/* 5:�Z���T�� */
			
			for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
			{
				wkU16Array[ wkLoop ] = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviUU[ wkLoop ] );
				wkU8Array[ wkLoop ] = gvInFlash.mParam.mDelayUU[ wkLoop ];
			}
			ApiRfDrv_SetMeaAlmValArray( &wkU16Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLastUpperLimitAlarm[ 0U ] );	/* 6:�����l */
			SubRfDrv_SetAlmDelayArray( &wkU8Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLastUpperLimitDelay[ 0U ] );		/* 7:�����x���� */
			
			for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
			{
				wkU16Array[ wkLoop ] = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviU[ wkLoop ] );
				wkU8Array[ wkLoop ] = gvInFlash.mParam.mDelayU[ wkLoop ];
			}
			ApiRfDrv_SetMeaAlmValArray( &wkU16Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mUpperLimitAlarm[ 0U ] );		/* 8:����l */
			SubRfDrv_SetAlmDelayArray( &wkU8Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mUpperLimitDelay[ 0U ] );			/* 9:����x���� */
			
			for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
			{
				wkU16Array[ wkLoop ] = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviL[ wkLoop ] );
				wkU8Array[ wkLoop ] = gvInFlash.mParam.mDelayL[ wkLoop ];
			}
			ApiRfDrv_SetMeaAlmValArray( &wkU16Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLowerLimitAlarm[ 0U ] );		/* 10:�����l */
			SubRfDrv_SetAlmDelayArray( &wkU8Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLowerLimitDelay[ 0U ] );			/* 11:�����x���� */
			
			for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
			{
				wkU16Array[ wkLoop ] = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviLL[ wkLoop ] );
				wkU8Array[ wkLoop ] = gvInFlash.mParam.mDelayLL[ wkLoop ];
			}
			ApiRfDrv_SetMeaAlmValArray( &wkU16Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLastLowerLimitAlarm[ 0U ] );	/* 12:�����l */
			SubRfDrv_SetAlmDelayArray( &wkU8Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLastLowerLimitDelay[ 0U ] );		/* 13:�����x���� */
			
			SubRfDrv_SetDevi( &vutRfDataBuffer.umRT_LoggerInfo.mAlarmDeviationTime[ 0U ] );							/* 14:��E���e���� */
			
			vutRfDataBuffer.umRT_LoggerInfo.mRecInterval = gvInFlash.mParam.mLogCyc1 & 0x0F;						/* 15:���^����1 */
			vutRfDataBuffer.umRT_LoggerInfo.mRecInterval |= (gvInFlash.mParam.mLogCyc2 & 0x0F) << 4U;				/* 16:���^����2 */
			
			vutRfDataBuffer.umRT_LoggerInfo.mCalcAlarmKind = gvInFlash.mParam.mAlertType;							/* 17:�x��Ď����Z�� */
			vutRfDataBuffer.umRT_LoggerInfo.mCalcAlarmParameter = gvInFlash.mParam.mAlertParmFlag;					/* 18:�x��Ď����Z�p�����[�^�A�t���O */
			
			vutRfDataBuffer.umRT_LoggerInfo.mMeasOldId[ 0U ] = gvInFlash.mData.mMeas1_PastIndex >> 8U;					/* 19:�Ō�IndexNo.(�v���l) */
			vutRfDataBuffer.umRT_LoggerInfo.mMeasOldId[ 1U ] = gvInFlash.mData.mMeas1_PastIndex & 0x00FFU;
			
			for( wkLoop = 0U ; wkLoop < 4U; wkLoop++ )
			{
				wkU32 = gvInFlash.mData.mMeas1_PastTime >> (24U - wkLoop * 8U);
				vutRfDataBuffer.umRT_LoggerInfo.mMeasOldTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);					/* 20:�ŌÎ���(�v���l) */
			}
			
			vutRfDataBuffer.umRT_LoggerInfo.mAlHistOldId[ 0U ] = gvInFlash.mData.mMeasAlm_PastIndex >> 8U;				/* 21:�Ō�IndexNo.(�v���x��) */
			vutRfDataBuffer.umRT_LoggerInfo.mAlHistOldId[ 1U ] = gvInFlash.mData.mMeasAlm_PastIndex & 0x00FFU;
			
			for( wkLoop = 0U ; wkLoop < 4U; wkLoop++ )
			{
				wkU32 = gvInFlash.mData.mMeasAlm_PastTime >> (24U - wkLoop * 8U);
				vutRfDataBuffer.umRT_LoggerInfo.mAlHistOldTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);				/* 22:�ŌÎ���(�v���x��) */
			}
			
			vutRfDataBuffer.umRT_LoggerInfo.mVer[ 0U ] = cRomVer[ 0U ];
			vutRfDataBuffer.umRT_LoggerInfo.mVer[ 1U ] = cRomVer[ 1U ] * 10 + cRomVer[ 2U ];
			
			SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_3 - 2U );
			
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoogerInfoCarrierSense;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_RTmode_LoogerInfoCarrierSense:
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
//			RTCMK = 1U;   /* disable INTRTC interrupt */
			
			/* Carrire Sence 125usec */
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoogerInfoTxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				if( SubRfDrv_CarrierSens_Rt_RetryDecision( 1U * 375U, CH_FIX, CS_RT_CONNECT_MODE ) == NEXT )
				{
					/* CarrierSens Retry */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoogerInfoCarrierSense;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
					vrfErorrCause = 8U;
#endif
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoogerInfoTxStart:
			SubRFDrv_LoRa_Tx_Setting(RT_MODE_PAYLOAD_LENGTH_3, 6U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_3);
			SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_3 );
			
			/* Tx Done Wait */
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			SubIntervalTimer_Sleep( 80U * 375U, 5U );			/* 400msec */
			
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoogerInfoTxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_LoogerInfoTxWait:
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ���M���Ƀ^�C���A�E�g(399.8msec) */
				/* �K���ȏ�̑��M���� */
				R_INTC6_Stop();
				SubIntervalTimer_Stop();
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 9U;
#endif
			}
			else
			{
				/* ���M���� */
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAckCad;
				SubIntervalTimer_Sleep( 6U * 375U, 1U );		/* 10msec */
				
				/* ���d�rAD���� */
				ApiAdc_AdExe();
				
				vLoopLoRa = 0U;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckCad:
			SubSX1272CadLoRa();
			if( gvRfIntFlg.mRadioDio4 )
			{
				/* CAD���m */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAckRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else if( gvRfIntFlg.mRadioDio0 )
			{
				/* �񌟒m */
				if( vLoopLoRa < 2U)
				{
					/* Retry */
					SubSX1272Sleep();		/* �t���O�N���A�̂��� */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAckCad;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					vLoopLoRa++;
				}
				else
				{
					/* 3 Retry Time out */
					SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
					vrfErorrCause = 10U;
#endif
				}
			}
			else
			{
				/* �N���^�C���A�E�g */
				SubIntervalTimer_Stop();
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 11U;
#endif
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_4, 6U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait �ݒ� */
			SubIntervalTimer_Sleep( 100U * 375U, 1U );			// 100msec
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAckRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckRxWait:
			/* ��M�����҂� */
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* ��M���� */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubSX1272Stdby();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAckRxRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ��M�^�C���A�E�g */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 12U;
#endif
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckRxRead:
			/* ���K�[�ڑ����ʐM(Logger CoonectAuth)��256U�ȍ~�Ɋi�[�ς� */
			SubRFDrv_LoRa_Rx_Read( &wkRadio, &vrfCurrentRssi);
			SubSX1272Sleep();
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS )
			{
				/* CRC OK */
				/* LoggerID�ƃV�[�P���XNo�̃`�F�b�N */
				if( memcmp( vutRfDataBuffer.umRT_Gw_ConnAck.mLoggerID, gvInFlash.mProcess.mUniqueID, 3U ) == 0U )
				{
					/* ���@���̒ʐM �V�[�P���XNo�`�F�b�N*/
					if( (vutRfDataBuffer.umRT_Gw_ConnAck.mGwID_Sequence[2U] & 0x0F) == 0x01 )
					{
						/* 2021.7.12 �ǉ� */
						vrfInitCnt = 0U;

						vrfRssi = vrfCurrentRssi;
						/* ���K�[�ڑ���ACK�ʐM�ł��� */
						/* RTmode�ł̌v���l�ʐM�J�n */
						SubRfDrv_SetRtConInfo( imON );
						/* Auth�ʐM�Ŏ�M�ς݂����A�ēo�^�̏ꍇ�͂����炪�D��(Auth�ʐM���폜���Ă�OK) */
						vrfRTmodeTimeSlot = vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot;
						if( (vRfDrv_CommIntMode == ecRfDrv_ShortIntMode) && (vrfRTmodeTimeSlot > RT_SHORT_MODE_CONNECT_NUM ) )
						{
							/* Short Mode�����ڑ��䐔�I�[�o�[ */
							SubRfDrv_SetRtConInfo( imOFF );
							vrfRTmodeTimeSlot = 0U;
#if (swRssiLogDebug == imEnable)
							vrfErorrCause = 16U;
#endif
						}
						else if( (vRfDrv_CommIntMode == ecRfDrv_MidIntMode) && (vrfRTmodeTimeSlot > RT_MID_MODE_CONNECT_NUM ) )
						{
							/* Mid Mode�����ڑ��䐔�I�[�o�[ */
							SubRfDrv_SetRtConInfo( imOFF );
							vrfRTmodeTimeSlot = 0U;
#if (swRssiLogDebug == imEnable)
							vrfErorrCause = 16U;
#endif
						}
						else
						{
							vrfCommTargetID[0U] = vutRfDataBuffer.umRT_Gw_ConnAck.mGwID_Sequence[0U];
							vrfCommTargetID[1U] = vutRfDataBuffer.umRT_Gw_ConnAck.mGwID_Sequence[1U];
							vrfCommTargetID[2U] = vutRfDataBuffer.umRT_Gw_ConnAck.mGwID_Sequence[2U] & 0xF0;
							
#if swRfDenpaJigu == imEnable
							/* �d�g���x�m�F�W�O */
							//SubModbus_MakeSndData( wkFunCode, wkRefNo, wkNum, wkSetData );
//							vrfDebug[0U] = vrfRssi;
//							vrfDebug[1U] = vrfCommTargetID[0U];
//							vrfDebug[2U] = vrfCommTargetID[1U];
//							vrfDebug[3U] = vrfCommTargetID[2U];
							SubModbus_MakeSndData( imMod_Fnc16, 40011U, 4U, vrfDebug );							/*112byte�܂ő��M��(������56�܂�)*/
							
							gvutComBuff.umModInf.mRsrvFlg = imON;			/* ���M�\��t���OON */
							gvModuleSts.mMod = ecModModuleSts_Run;			/* Modbus���C�����[�`���J�n */
#else
							/* ����AppID�̏ꍇ�̓��K�[�ڑ�����M����GW AppID��o�^ */
							if( (gvInFlash.mParam.mAppID[ 0U ] == APP_ID_LOGGER_INIT_UP) && (gvInFlash.mParam.mAppID[1U] == APP_ID_LOGGER_INIT_LO) )
							{
								gvInFlash.mParam.mAppID[ 0U ] = (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxAppID_GwUniqueID[0U] >> 4U) & 0x0F;
								gvInFlash.mParam.mAppID[ 1U ] = (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxAppID_GwUniqueID[0U] << 4U) & 0xF0;
								gvInFlash.mParam.mAppID[ 1U ] += (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxAppID_GwUniqueID[1U] >> 4U) & 0x0F;
								vrfMySerialID.mAppID[0U] = ((gvInFlash.mParam.mAppID[ 0U ] << 4U) & 0xF0) + ((gvInFlash.mParam.mAppID[ 1U ] >> 4U) & 0x0F);
								vrfMySerialID.mAppID[1U] = (gvInFlash.mParam.mAppID[ 1U ] << 4U) & 0xF0;
								if( (gvInFlash.mParam.mAppID[ 0U ] == 0x0F) && (gvInFlash.mParam.mAppID[ 1U ] == 0xFE ) )
								{
									gvInFlash.mParam.mOnCertLmt = LOCAL;
								}else
								{
									gvInFlash.mParam.mOnCertLmt = CENTRAL;
								}
								gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
							}
#endif
							/* ���[�J�����[�h�̏ꍇ��GW�t�B���^�ݒ肪�Ȃ��ꍇ��GW��o�^ */
							if( (gvInFlash.mParam.mOnCertLmt == LOCAL) && 
								(gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 0U ] == 0x00) &&
								(gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 1U ] == 0x00) &&
								(gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 2U ] == 0x00) )
							{
								gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 0U ] = vrfCommTargetID[0U];
								gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 1U ] = vrfCommTargetID[1U];
								gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 2U ] = vrfCommTargetID[2U];
								gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
							}
						}
						gvrfStsEventFlag.mOnlineLcdOnOff = 0;				/* LCD OFF 2022.9.16 */
						gvModuleSts.mLcd = ecLcdModuleSts_Run;
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
					}
					else
					{
						/* ���@���̈Ⴄ15byte�ʐM */
						SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
						vrfErorrCause = 15U;
#endif
					}
				}
				else
				{
					/* ���@���̒ʐM */
					SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
					vrfErorrCause = 14U;
#endif
				}
			}
			else
			{
				/*CRC NG */
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 13U;
#endif
			}
			break;
			
		/* Logger Measure(�x�񗚗�) ���M--------------------------------------- */
		case ecRfDrvMainSts_RTmode_MeasureDataSet:
		case ecRfDrvMainSts_RTmode_MeasAlmDataSet:
			vrfRssi = 0U;
			if(vLoopLoRa == 0U)
			{
# if 0
				/* �J�ڑO�̉ӏ���sleep�ɕύX */
				SubIntervalTimer_Sleep( 90U * 375U, 1U );			/* �^�C�~���O������l�������M��x�点�� */
#endif
				vLoopLoRa++;
#if 0
#if (swRfTestLcd == imEnable)
				ApiLcd_Upp7SegNumDsp( vrfRTmodeTimeSlot, imOFF, imOFF);
				ApiLcd_Low7SegNumDsp( vrfErorrCause, imOFF );
				ApiLcd_Main();
#endif
#endif
			}
			
			/* �ߋ��f�[�^���M��0�A���͑��M��1�ȏ��Flash���瑗�M�f�[�^�̃��[�h�����̂Ƃ� */
			if( (vRfDrvMainLoggerSts == ecRfDrvMainSts_RTmode_MeasAlmDataSet && imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_MeasAlm )) ||
				(vRfDrvMainLoggerSts == ecRfDrvMainSts_RTmode_MeasureDataSet && (imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_Meas1 ) || vRtMode_MeasSndKosu == 0U)) )
			{
				if( vRfDrvMainLoggerSts == ecRfDrvMainSts_RTmode_MeasureDataSet )
				{
					SubRfDrv_RTmodeMeasDataSet( MEASURE_DATASET );				/* ������M�v���f�[�^�Z�b�g */
					/* �v���l�̑��M */
				SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_5 - (9U - vRtMode_MeasSndKosu) * 7U );
					vOldPtrFlg = MEASURE_DATASET;
				}
				else
				{
					SubRfDrv_RTmodeMeasDataSet( MEASALM_DATASET );				/* �x�񗚗��f�[�^�Z�b�g */
					/* �x�񗚗��̑��M */
					SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_10 - (6U - vRtMode_MeasAlmSndKosu) * 11U );
					vOldPtrFlg = MEASALM_DATASET;
				}
				
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureCarrierSens;
#if 0
				if( vLoopLoRa == 1U )
				{
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
#else
				gvModuleSts.mRf = ecRfModuleSts_Run;
#endif
			}
			else
			{
				/* Flash���[�h�ł��Ȃ��Ƃ��̃G���[���� */
				SubIntervalTimer_Sleep( 10U * 375U, 1U );
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
				vLoopLoRa++;
				if( vLoopLoRa > 2U )
				{
					vRtMode_MeasSndKosu = 0U;
					vrfRTmodeCommFailNum++;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
#if (swRssiLogDebug == imEnable)
					vrfErorrCause = 17U;
#endif
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureCarrierSens:
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
			
//			RTCMK = 1U;   /* disable INTRTC interrupt */
			
			/* Carrire Sence 125usec */
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureTxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				if( SubRfDrv_CarrierSens_Rt_RetryDecision( 1U * 375U, CH_CHANGE, CS_RT_MEASURE_MODE ) == NEXT )
				{
					/* CarrierSens Retry */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureCarrierSens;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					vrfRTmodeCommFailNum++;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
#if (swRssiLogDebug == imEnable)
					vrfErorrCause = 18U;
#endif
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureTxStart:
			/* ONLINE���� */
			ApiLcd_SegDsp(ecLcdSeg_S2, imOFF);
			ApiLcd_Main();
			
			if( (vutRfDataBuffer.umRT_MeasureTx.mDataCntKind & 0x80) == 0x80 )
			{
				/* �x�񗚗𑗐M */
				SubRFDrv_LoRa_Tx_Setting(RT_MODE_PAYLOAD_LENGTH_10 - (6U - vRtMode_MeasAlmSndKosu) * 11U, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
				/* 2022.9.16 ���ʂȂ��Ɣ��f�����ɖ߂� 2022.9.20 */
//				SubRFDrv_LoRa_Tx_Setting(RT_MODE_PAYLOAD_LENGTH_10 - (6U - vRtMode_MeasAlmSndKosu) * 11U, 16U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
				SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_10 - (6U - vRtMode_MeasAlmSndKosu) * 11U);
				SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_10 - (6U - vRtMode_MeasAlmSndKosu) * 11U );
			}
			else
			{
				/* �v���l���M */
				SubRFDrv_LoRa_Tx_Setting(RT_MODE_PAYLOAD_LENGTH_5 - (9U - vRtMode_MeasSndKosu) * 7U, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
				/* 2022.9.16 */
//				SubRFDrv_LoRa_Tx_Setting(RT_MODE_PAYLOAD_LENGTH_5 - (9U - vRtMode_MeasSndKosu) * 7U, 16U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
				SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_5 - (9U - vRtMode_MeasSndKosu) * 7U);
				SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_5 - (9U - vRtMode_MeasSndKosu) * 7U );
			}
#if swRfTestLcd == imEnable
			P7_bit.no5 = 1U;
#endif
			vrfInitState = ecRfDrv_Init_Non;
			
			/* Tx Done Wait */
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
#if 0
			SubIntervalTimer_Sleep( 80U * 375U, 5U );		/* 400msec */
			
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureTxWait;
#else
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureBattAd;

#if 1
			if( vRtMode_MeasSndKosu < 10U )
			{
				wkRadio = wkRadioTable[ vRtMode_MeasSndKosu ];
			}
			else
			{
				wkRadio = 37U;
			}
#else
			switch ( vRtMode_MeasSndKosu )
			{
				case 0U:
					wkRadio = 37U;		/* 140msec */
					break;
				case 1U:
					wkRadio = 38U;		/* 150msec */
					break;
				case 2U:
					wkRadio = 45U;		/* 180msec */
					break;
				case 3U:
					wkRadio = 53U;		/* 210msec */
					break;
				case 4U:
					wkRadio = 60U;		/* 240msec */
					break;
				case 5U:
					wkRadio = 63U;		/* 250msec */
					break;
				case 6U:
					wkRadio = 70U;		/* 280msec */
					break;
				case 7U:
					wkRadio = 78U;		/* 310msec */
					break;
				case 8U:
					wkRadio = 85U;		/* 340msec */
					break;
				case 9U:
					wkRadio = 88U;		/* 350msec */
					break;
				default:
					wkRadio = 37U;		/* 140msec */
					break;
			}
#endif
			/* 2022.9.16 ���ɖ߂� 2022.9.20 */
//			wkRadio += 8;
//			SubIntervalTimer_Sleep( wkRadio * 375, 4U );
#endif
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureBattAd:
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureTxWait;
			
			/* ���d�rAD���� */
			ApiAdc_AdExe();
			
#if (swAdCntLog == imEnable)
			gvAdCnt_RfFlg = imON;
#endif
			
			if( gvRfIntFlg.mRadioDio0 == 1U )
			{
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				SubIntervalTimer_Sleep( 50 * 375, 4U );
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureTxWait:
			/* ONLINE�_�� */
			ApiLcd_SegDsp(ecLcdSeg_S2, imON);
			ApiLcd_Main();
#if swRfTestLcd == imEnable
			P7_bit.no5 = 0U;
#endif
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ���M���Ƀ^�C���A�E�g(399.8msec) */
				/* �K���ȏ�̑��M���� */
				SubSX1272Sleep();
				R_INTC6_Stop();
				SubIntervalTimer_Stop();
				vrfRTmodeCommFailNum++;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;			/*	�����I�� */
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 19U;
#endif
			}
			else
			{
				/* ���M���� */
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureAckCad;
#if 0
				switch (vRtMode_MeasSndKosu)
				{
					case 9U:
						SubIntervalTimer_Sleep( 6U * 375U, 1U );			/* 6msec */
						break;
					case 8U:
						SubIntervalTimer_Sleep( 20U * 375U, 1U );			/* 20msec */
						break;
					case 7U:
						SubIntervalTimer_Sleep( 49U * 375U, 1U );		/* 49msec */
						break;
					case 6U:
						SubIntervalTimer_Sleep( 77U * 375U, 1U );		/* 77msec */
						break;
					case 5U:
						SubIntervalTimer_Sleep( 106U * 375U, 1U );		/* 106msec */
						break;
					case 4U:
						SubIntervalTimer_Sleep( 119U * 375U, 1U );		/* 119msec */
						break;
					case 3U:
						SubIntervalTimer_Sleep( 75U * 375U, 2U );		/* 150msec */
						break;
					case 2U:
						SubIntervalTimer_Sleep( 88U * 375U, 2U );		/* 176msec */
						break;
					case 1U:
						SubIntervalTimer_Sleep( 103U * 375U, 2U );		/* 206msec */
						break;
					case 0U:
						SubIntervalTimer_Sleep( 110U * 375U, 2U );		/* 220msec */
						break;
					default:
						SubIntervalTimer_Sleep( 6U * 375U, 1U );		/* 6msec */
						break;
				}
#else
				if( (vutRfDataBuffer.umRT_MeasureTx.mDataCntKind & 0x80) == 0x80 )
				{
					/* �x�񗚗� */
					SubIntervalTimer_Sleep(cRF_RTMODE_TX_AUDIT_WAIT[vRtMode_MeasAlmSndKosu].mTime * 375U ,cRF_RTMODE_TX_AUDIT_WAIT[vRtMode_MeasAlmSndKosu].mNum);
				}
				else
				{
					/* �v���l */
					SubIntervalTimer_Sleep(cRF_RTMODE_TX_MEAS_WAIT[vRtMode_MeasSndKosu].mTime * 375U ,cRF_RTMODE_TX_MEAS_WAIT[vRtMode_MeasSndKosu].mNum);
				}
#endif
				vLoopLoRa = 0U;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureAckCad:
#if swRfTestLcd == imEnable
			P7_bit.no5 = 1U;
#endif
			SubSX1272CadLoRa();
			if( gvRfIntFlg.mRadioDio4 )
			{
				/* CAD���m */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureAckRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
#if swRfTestLcd == imEnable
				P7_bit.no5 = 0U;
#endif
			}
			else if( gvRfIntFlg.mRadioDio0 )
			{
				/* �񌟒m */
				if( vLoopLoRa < 3U)
				{
					/* Retry */
					SubSX1272Sleep();		/* �t���O�N���A�̂��� */
//					SubSX1272Write( REG_LR_IRQFLAGS, 0xFF );
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureAckCad;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					vLoopLoRa++;
				}
				else
				{
					/* 3 Retry Time out */
					SubSX1272Sleep();
					vCallChLoopLora++;			/* ����`���l���ύX */
					vrfRTmodeCommFailNum++;
					vrfDebugRTmodeCadMiss++;
					gvrfStsEventFlag.mRtmodePwmode = PW_H;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
#if (swRssiLogDebug == imEnable)
					vrfErorrCause = 20U;
#endif
#if swRfTestLcd == imEnable
					P7_bit.no5 = 0U;
#endif
				}
			}
			else
			{
				/* �N���^�C���A�E�g */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				vrfRTmodeCommFailNum++;
				vrfDebugRTmodeCadMiss++;
				gvrfStsEventFlag.mRtmodePwmode = PW_H;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 21U;
#endif
#if swRfTestLcd == imEnable
				P7_bit.no5 = 0U;
#endif
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureAckRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_6, 14U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait �ݒ� */
			SubIntervalTimer_Sleep( 85U * 375U, 1U );			// 85msec
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureAckRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureAckRxWait:
			/* ��M�����҂� */
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* ��M���� */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubSX1272Stdby();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureAckRxRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ��M�^�C���A�E�g */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				vCallChLoopLora++;		/* ����`���l���ύX */
				vrfRTmodeCommFailNum++;
				gvrfStsEventFlag.mRtmodePwmode = PW_H;
				vrfDebugRTmodeRxTimeout++;			/* �f�o�b�O�p */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureAckRxRead:
			/* ���K�[�ڑ����ʐM(Logger CoonectAuth)��256U�ȍ~�Ɋi�[�ς� */
			SubRFDrv_LoRa_Rx_Read( &wkRadio, &vrfCurrentRssi);
			SubSX1272Sleep();
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS )
			{
				/* CRC OK */
				/* LoggerID�ƃV�[�P���XNo�̃`�F�b�N */
				if( memcmp( vutRfDataBuffer.umRT_MeasureAck.mLoggerID, gvInFlash.mProcess.mUniqueID, 3U ) == 0U )
				{
					/* ���@���̒ʐM �V�[�P���XNo�`�F�b�N*/
//					if( (vutRfDataBuffer.umRT_MeasureAck.mSequenceSettingDisConnect & 0xE0) == 0x40 )
//					{
						vrfRssi = vrfCurrentRssi;
						/* �v���f�[�^�ʐM��ACK�ł��� */
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
						vrfRTmodeCommFailNum = 0U;			/* ���s�񐔃N���A */
						/* �^�C�~���O�J�E���^���f */
						R_IT_Stop();
#if 0
						gvRfTimingCounterH =  vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[0U] & 0xF8;
						gvRfTimingCounterH = (gvRfTimingCounterH << 5U) & 0x1F00;
						gvRfTimingCounterH += vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[2U];
#endif
						/* ���� */
						vrfITSyncCount =  vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[0U] & 0xF8;
						vrfITSyncCount = (vrfITSyncCount << 5U) & 0x1F00;
						vrfITSyncCount += vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[2U];
//						if( (((vrfITSyncCount) % 60U - (gvRfTimingCounterH) % 60U) > 1U) | ((gvRfTimingCounterH % 60U)-(vrfITSyncCount % 60U) > 1U ))
//						{
//							NOP();
//						}
						gvRfTimingCounterH = vrfITSyncCount;
						gvRfTimingCounterL = 0x0000;
						gvRfTimingCounterL = vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[0U] & 0x07;
						gvRfTimingCounterL <<= 8U;
						gvRfTimingCounterL = (gvRfTimingCounterL & 0x0700) + vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[1U];
						gvRfTimingCounterL += 170U;			/* 78msec + 5msec(��������) */
						vrfITSyncCount = gvRfTimingCounterL;
						gvRfTimingCounterL = ((gvRfTimingCounterL & 0x0F00) >> 8U) & 0x000F;		/* gvRfTimingCounterL��0-7��������8�܂� */
						vrfITSyncCount = (vrfITSyncCount << 4U) & 0x0FF0;			/* vrfITSyncCount��0-0x0FF0 */
						vrfITSyncCount = 0x0FF0 - (vrfITSyncCount & 0x0FF0) + 8U;		/* �Ō�̌���0x0��0xF�̊� */
						if( gvRfTimingCounterL < 7U )
						{
							gvRfTimingCounterL++;
						}
						else
						{
							gvRfTimingCounterH++;
							gvRfTimingCounterL = gvRfTimingCounterL - 7U;
						}
						if( vrfITSyncCount )
						{
							gvrfStsEventFlag.mTimerSync = 1U;
							R_IT_Create_Custom( vrfITSyncCount );
						}
						else
						{
							R_IT_Create_Custom( 0xFFF );
						}
						
						/* �ؒf�t���O���� */
						if( vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[0U] & 0x40 )
						{
							SubRfDrv_SetRtConInfo( imOFF );
							vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconInit;
						}
						/* �ݒ�ύX�v������ */
						if( vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[0U] & 0x80 )
						{
							gvrfStsEventFlag.mSetReserve = 1U;
						}
						
						/* GW����(14bit) */
						wkU32 = (vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[ 0U ] & 0x3F) << 8U;
						wkU32 |= vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[ 1U ];
						ApiTimeComp_first( wkU32, 1U );
						
						/* ���M�d�͕ύX */
						if( (vrfRssi > PW_TH_L_TO_H) )
						{
							gvrfStsEventFlag.mRtmodePwmode = PW_H;
						}
						else if( vrfRssi < PW_TH_H_TO_L )
						{
							gvrfStsEventFlag.mRtmodePwmode = PW_L;
						}
						else
						{
							;/* ���̃P�[�X�͌���ێ� */
						}
						
						if( vOldPtrFlg == MEASURE_DATASET )
						{
							SubRfDrv_RtmodeMeas1OldPtrUpdate();			/* ����l�̉ߋ����M�|�C���^�X�V */
						}
						else
						{
							/* �v���x�񔭐�/�����񐔂��f�N�������g */
							if( gvMeasAlmNum >= vRtMode_MeasAlmSndKosu )
							{
								gvMeasAlmNum -= vRtMode_MeasAlmSndKosu;
							}
							else
							{
								gvMeasAlmNum = 0U;
							}
						}
						
						gvInFlash.mData.mAlmPast[ 0U ] = ecAlmPastSts_Non;				/* �ߋ��x��t���O�N���A */
						gvInFlash.mData.mAlmPast[ 1U ] = ecAlmPastSts_Non;
						gvInFlash.mData.mAlmPast[ 2U ] = ecAlmPastSts_Non;
						gvInFlash.mData.mAlmPast[ 3U ] = ecAlmPastSts_Non;
						gvInFlash.mData.mAlmPast[ 4U ] = ecAlmPastSts_Non;
						
						vrfDebugRTmodeSuccess++;			/* Debug�p �v���l���M������ */
						
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
						gvModuleSts.mLcd = ecLcdModuleSts_Run;
//					}
//					else
//					{
//						/* ���@���̈Ⴄ15byte�ʐM */
//						vCallChLoopLora++;		/* ����`���l���ύX */
//						vrfRTmodeCommFailNum++;
//						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
//						gvModuleSts.mRf = ecRfModuleSts_Sleep;
//						vrfErorrCause = 25U;
//					}
				}
				else
				{
					/* ���@���̒ʐM */
					vCallChLoopLora++;		/* ����`���l���ύX */
					vrfRTmodeCommFailNum++;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
#if (swRssiLogDebug == imEnable)
					vrfErorrCause = 24U;
#endif
				}
			}
			else
			{
				/*CRC NG */
				vrfRTmodeCommFailNum++;
				vrfDebugRTmodeCrc++;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 23U;
#endif
			}
			break;
			
		/* Logger RTmode Setting */
		case ecRfDrvMainSts_RTmode_ChangeSetBeaconInit:
			vrfRssi = 0U;
			rfStatus.Modem = MODEM_LORA;
			SubSX1272Sleep();
//			SubRFDrv_LoRa_Tx_Setting( RT_MODE_PAYLOAD_LENGTH_1, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7 );
//			if( vCallChLoopLora >= RT_MODE_CH_LOOP_NUM ){	vCallChLoopLora = 0U;	}
//			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[ vrfLoraChGrupeCnt ].mCh[vCallChLoopLora], rfFreqOffset );
//			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mCh[ vCallChLoopLora ], rfFreqOffset );
//			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_CNCT[ vrfLoraChGrupeCnt ].mLoraSync );
			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mLoraSync );
			SubSX1272Write( REG_LR_LNA , RFLR_LNA_GAIN_G1 | RFLR_LNA_BOOST_ON );
			SubRfDrv_PwChange( gvrfStsEventFlag.mRtmodePwmode );
			vLoopLoRa = 0U;
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272Write( REG_PACKETCONFIG1,
				RF_PACKETCONFIG1_PACKETFORMAT_FIXED | RF_PACKETCONFIG1_DCFREE_OFF
				 | RF_PACKETCONFIG1_CRC_OFF | RF_PACKETCONFIG1_CRCAUTOCLEAR_OFF
				 | RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT  );
//			SubSX1272CarrierSenseFSK_DIO();
			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
//			rfStatus.Modem = MODEM_LORA;
//			SubSX1272Sleep();
//			vCallChLoopLora = 0U;
//			vLoopLoRa = 0U;
//			rfStatus.Modem = MODEM_FSK;
//			SubSX1272Sleep();
//			SubSX1272CarrierSenseFSK_DIO();
			vutRfDataBuffer.umRT_ChangeSetBeacon.mLoggerID1[0U] = gvInFlash.mProcess.mUniqueID[ 0U ];
			vutRfDataBuffer.umRT_ChangeSetBeacon.mLoggerID1[1U] = gvInFlash.mProcess.mUniqueID[ 1U ];
			vutRfDataBuffer.umRT_ChangeSetBeacon.mLoggerID2 = gvInFlash.mProcess.mUniqueID[ 2U ];
			vutRfDataBuffer.umRT_ChangeSetBeacon.mOnline_Flag = 0x01;
			vutRfDataBuffer.umRT_ChangeSetBeacon.mOnline_Flag += gvInFlash.mParam.mOnCertLmt << 1U;
			SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_1 - 2U );
			
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetBeaconCarrireSense;
//			vrfInitState = ecRfDrv_Init_Rtmode_ChangeSet;
			vrfInitState = ecRfDrv_Init_Rtmode_Beacon;
			/* �����_��wait */
			srand( gvInFlash.mProcess.mUniqueID[ 2U ] );
			
			SubIntervalTimer_Sleep( ( rand() % 95U) * 375U, 4U );		/* 0-380msec */
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetBeaconCarrireSense:
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			if( vCallChLoopLora >= RT_MODE_CH_LOOP_NUM ){	vCallChLoopLora = 0U;	}
//			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[vrfLoraChGrupeCnt].mCh[vCallChLoopLora], rfFreqOffset );
			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mCh[ vCallChLoopLora ], rfFreqOffset );
			SubSX1272CarrierSenseFSK_DIO();
//			RTCMK = 1U;   /* disable INTRTC interrupt */
			
			/* Carrire Sence 125usec */
#if 0
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetBeaconTxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				SubSX1272Sleep();
				vCallChLoopLora++;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
#else
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) != NO_CARRIER )
			{
				SubSX1272Sleep();
				vCallChLoopLora++;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
				break;
			}
#endif
			
		case ecRfDrvMainSts_RTmode_ChangeSetBeaconTxStart:
			SubRFDrv_LoRa_Tx_Setting( RT_MODE_PAYLOAD_LENGTH_1, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7 );
			SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_1);
			SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_1 );
			/* Tx Done Wait */
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			SubIntervalTimer_Sleep( 100U * 375U, 1U );			/* 100msec */
			
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetBeaconTxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetBeaconTxWait:
			SubSX1272Sleep();
			R_INTC6_Stop();
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ���M���Ƀ^�C���A�E�g(100msec) */
				/* �K���ȏ�̑��M���� */
//				SubSX1272Sleep();
//				R_INTC6_Stop();
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;			/*	�����I�� */
//				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			else
			{
				/* ���M���� */
//				SubSX1272Sleep();
				SubIntervalTimer_Stop();
//				R_INTC6_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx1Cad;
#if swRTmodeSetting == imEnable
				SubIntervalTimer_Sleep( 100U * 375U, 10U );		/* 1sec */
#else
				SubIntervalTimer_Sleep( 2U * 375U, 1U );		/* 2msec */
#endif
				vLoopLoRa = 0U;
			}
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx1Cad:
			SubSX1272CadLoRa();
			if( gvRfIntFlg.mRadioDio4 )
			{
				/* CAD���m */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx1Start;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else if( gvRfIntFlg.mRadioDio0 )
			{
				SubSX1272Sleep();
				/* �񌟒m */
#if swRTmodeSetting == imEnable
				if( vLoopLoRa < 10U)
#else
				if( vLoopLoRa < 3U)
#endif
				{
					/* Retry */
//					SubSX1272Sleep();		/* �t���O�N���A�̂��� */
//					SubSX1272Write( REG_LR_IRQFLAGS, 0xFF );
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx1Cad;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					vLoopLoRa++;
				}
				else
				{
					/* 3 Retry Time out */
//					SubSX1272Sleep();
					vrfInitState = ecRfDrv_Init_Non;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
			}
			else
			{
				/* �N���^�C���A�E�g */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx1Start:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_7_1, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait �ݒ� */
			SubIntervalTimer_Sleep( 100U * 375U, 2U );		/* 200msec */
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx1Wait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx1Wait:
			/* ��M�����҂� */
			R_INTC6_Stop();
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* ��M���� */
//				R_INTC6_Stop();
				SubSX1272Stdby();
				SubIntervalTimer_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx1Read;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ��M�^�C���A�E�g */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
//				R_INTC6_Stop();
				vCallChLoopLora++;
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx1Read:
			SubRFDrv_LoRa_Rx_Read( &wkRadio, &vrfCurrentRssi);
			SubSX1272Sleep();
#if 1
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS &&
				memcmp( vutRfDataBuffer.umRT_ChangeSetPrm1.mLoggerID, gvInFlash.mProcess.mUniqueID, 3U ) == 0U )
			{
				/* CRC OK and ID��v*/
//				vrfRssi = vrfCurrentRssi;
//				for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_7_1; wkRadio++ )
//				{
//					vutRfDataBuffer.umData[wkRadio + 256U] = vutRfDataBuffer.umData[wkRadio];
//				}
				
				ApiInFlash_RtSetValWrite1( &vutRfDataBuffer.umRT_ChangeSetPrm1 );				/* RAM�ɐݒ�l������ */
				
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx2Start;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* CRC�G���[ or ID�s��v */
				vCallChLoopLora++;
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
#else
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS )
			{
				/* CRC OK */
				if( memcmp( vutRfDataBuffer.umRT_ChangeSetPrm1.mLoggerID, gvInFlash.mProcess.mUniqueID, 3U ) == 0U )
				{
					vrfRssi = vrfCurrentRssi;
					for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_7_1; wkRadio++ )
					{
						vutRfDataBuffer.umData[wkRadio + 256U] = vutRfDataBuffer.umData[wkRadio];
					}
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx2Start;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
					/* ID�s��v */
					vCallChLoopLora++;
					vrfInitState = ecRfDrv_Init_Non;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
			}
			else
			{
				/* CRC�G���[ */
				vCallChLoopLora++;
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
#endif
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx2Start:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_7_2, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait �ݒ� */
			SubIntervalTimer_Sleep( 100U * 375U, 2U );		/* 200msec */
			
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx2Wait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx2Wait:
			/* ��M�����҂� */
			R_INTC6_Stop();
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* ��M���� */
//				R_INTC6_Stop();
				SubSX1272Stdby();
				SubIntervalTimer_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx2Read;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ��M�^�C���A�E�g */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
//				R_INTC6_Stop();
				vCallChLoopLora++;
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx2Read:
			SubRFDrv_LoRa_Rx_Read( &wkRadio, &vrfCurrentRssi);
			SubSX1272Sleep();
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS )
			{
				/* CRC OK */
//				vrfRssi = vrfCurrentRssi;
//				for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_7_2; wkRadio++ )
//				{
//					vutRfDataBuffer.umData[wkRadio + 512U] = vutRfDataBuffer.umData[wkRadio];
//				}
				ApiInFlash_RtSetValWrite2( &vutRfDataBuffer.umRT_ChangeSetPrm2 );				/* RAM�ɐݒ�l������ */
				
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx3Start;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* CRC�G���[ */
				vCallChLoopLora++;
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx3Start:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_7_3, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait �ݒ� */
			SubIntervalTimer_Sleep( 100U * 375U, 2U );		/* 200msec */
			
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx3Wait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
				case ecRfDrvMainSts_RTmode_ChangeSetRx3Wait:
			/* ��M�����҂� */
			R_INTC6_Stop();
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* ��M���� */
//				R_INTC6_Stop();
				SubSX1272Stdby();
				SubIntervalTimer_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx3Read;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ��M�^�C���A�E�g */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
//				R_INTC6_Stop();
				vCallChLoopLora++;
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx3Read:
			SubRFDrv_LoRa_Rx_Read( &wkRadio, &vrfCurrentRssi);
			SubSX1272Sleep();
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS )
			{
				/* CRC OK */
//				vrfRssi = vrfCurrentRssi;
//				for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_7_3; wkRadio++ )
//				{
//					vutRfDataBuffer.umData[wkRadio + 768U] = vutRfDataBuffer.umData[wkRadio];
//				}
				ApiInFlash_RtSetValWrite3( &vutRfDataBuffer.umRT_ChangeSetPrm3 );				/* RAM�ɐݒ�l������ */
				
#if 0 /* TH���f���Z���� */
				/* �Z���ݒ�l������ */
				/* GW��ID��00001�̂ݍZ�����������s */
				if( (vutRfDataBuffer.umData[ 256U + 3U ] == 0x00U) &&
					(vutRfDataBuffer.umData[ 256U + 4U ] == 0x00U) &&
					((vutRfDataBuffer.umData[ 256U + 5U ] & 0xF0U) == 0x10U) )
				{
					gvInFlash.mProcess.mOffset[ 0U ] = vutRfDataBuffer.umRT_ChangeSetPrm3.mCalOffset[ 0U ];
					gvInFlash.mProcess.mSlope[ 0U ] = vutRfDataBuffer.umRT_ChangeSetPrm3.mCalZeroSpan[ 0U ];
					ApiInFlash_ParmWrite( ecInFlashWrSts_ParmNow, 0U );
				}
#endif
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetAckCarrierSens;
				gvModuleSts.mRf = ecRfModuleSts_Run;
				gvModuleSts.mLcd = ecLcdModuleSts_Run;
			}
			else
			{
				/* CRC�G���[ */
				vCallChLoopLora++;
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetAckCarrierSens:
			/* �f�[�^�Z�b�g */
			memcpy( &vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[ 0U ], &gvInFlash.mProcess.mUniqueID[ 0U ], 3U );
			memcpy( &vutRfDataBuffer.umRT_ChangeSetAck.mGwID[ 0U ], &vrfCommTargetID[ 0U ], 3U );
			vutRfDataBuffer.umRT_ChangeSetAck.mSequenceNo = 0x03;
//			vutRfDataBuffer.umRT_ChangeSetAck.mReserve[0U] = 0x12;
//			vutRfDataBuffer.umRT_ChangeSetAck.mReserve[1U] = 0x34;
			SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_8 - 2U );
			
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
//			RTCMK = 1U;   /* disable INTRTC interrupt */
			
			/* Carrire Sence 125usec */
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetAckTxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				if( SubRfDrv_CarrierSens_Rt_RetryDecision( 1U * 375U, CH_FIX, CS_RT_CONNECT_MODE ) == NEXT )
				{
					/* CarrierSens Retry */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetAckCarrierSens;
				}
				else
				{
					vrfInitState = ecRfDrv_Init_Non;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				}
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetAckTxStart:
			SubRFDrv_LoRa_Tx_Setting( RT_MODE_PAYLOAD_LENGTH_8, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7 );
			SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_8);
			SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_8 );
			
			/* Tx Done Wait */
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			SubIntervalTimer_Sleep( 90U * 375U, 1U );			/* 90msec */
			
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetAckTxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetAckTxWait:
			SubSX1272Sleep();
			R_INTC6_Stop();
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ���M���Ƀ^�C���A�E�g(399.8msec) */
				/* �K���ȏ�̑��M���� */
//				SubSX1272Sleep();
//				R_INTC6_Stop();
//				vrfInitState = ecRfDrv_Init_Non;
//				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;			/*	�����I�� */
//				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			else
			{
				/* ���M���� */
				gvrfStsEventFlag.mSetReserve = 0U;				/* �ݒ�ύX�t���O�N���A */
//				SubSX1272Sleep();
				SubIntervalTimer_Stop();
//				R_INTC6_Stop();
//				vrfInitState = ecRfDrv_Init_Non;
//				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
//				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		default:
			break;
		}

#endif


/******************************GateWay*****************************************/
#if  (swLoggerBord == imDisable) && (swRfTxTest == imDisable) && (swRfTxTest == imDisable)
	
	switch( vRfDrvMainGwSts )
	{
		/*************** GW FSK���[�h ****************/
		case ecRfDrvMainSts_HSmode_BeaconInit:
			/* ���[�U�[�ݒ� */
//			vrfHSLoggerID[ 0U ] = 0x00;			/* �Ăяo�����K�[ID */
//			vrfHSLoggerID[ 1U ] = 0x00;
//			vrfHSLoggerID[ 2U ] = 0x01;
			
			vrfMySerialID.mGroupID = gvInFlash.mId.mGroupID;
			vrfMySerialID.mAppID[0U] = ((gvInFlash.mId.mAppID[ 0U ] << 4U) & 0xF0) + ((gvInFlash.mId.mAppID[ 1U ] >> 4U) & 0x0F);
			vrfMySerialID.mAppID[1U] = (gvInFlash.mId.mAppID[ 1U ] << 4U) & 0xF0;
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Measure;				/* �v���l�v�� */ 
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_AlartHis;				/* �x�񗚗�v�� */ 
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_ReqFirm;				/* �t�@�[���A�b�v�v�� */
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;				/* ���K�[�X�e�[�^�X(�u���[�h�L���X�g) */
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_SetRead;				/* ���K�[�ݒ�l�ǂݍ��� */
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_SetWrite;				/* ���K�[�ݒ�l�ǂݍ��� */
//			gvInFlash.mParam.mrfTorokuMode = LOGGER_TOROKU_ON;

//			vrfHsReqDataNum = 46000U;									/* �v���f�[�^�� */
			if((gvInFlash.mParam.mrfTorokuMode == LOGGER_TOROKU_ON) && (vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Status) )
			{
				vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Toroku;
			}
			
			rfFreqOffset = RF_FREQ_OFFSET;
			
			/* ����I�ȏ��������� */
			if( gvrfStsEventFlag.mReset == RFIC_INI )
			{
				SubRfDrv_RfReset();
				gvrfStsEventFlag.mReset = RFIC_RUN;
			}

			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272Write( REG_PACKETCONFIG1,
				RF_PACKETCONFIG1_PACKETFORMAT_FIXED | RF_PACKETCONFIG1_DCFREE_OFF
				 | RF_PACKETCONFIG1_CRC_OFF | RF_PACKETCONFIG1_CRCAUTOCLEAR_OFF
				 | RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT  );
			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_CALL );
			SubRFDrv_SyncSet();
			SubSX1272TxFSK_DIO_240kbps();
			SubSX1272FreqSet( RF_HSMODE_CH[ gvInFlash.mParam.mrfHsCh ], rfFreqOffset );
			
			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_1);
			SubSX1272Write( REG_LNA, RF_LNA_GAIN_G1 | RF_LNA_BOOST_OFF );
			gvrfStsEventFlag.mHsmodeInit = 1U;
			
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconDataSet:
			/* ��������1.4msec */
			vrfHsmodePacketLength = 0U;
			for( wkRadio = 0U; wkRadio < 19U; wkRadio++ )
			{
				if( wkRadio > 0U )
				{
					for( wkRadio2nd = 0U; wkRadio2nd < RF_FSK_PREAMBLE_LENGTH_CALL; wkRadio2nd++ )
					{
						vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0xAA;
						vrfHsmodePacketLength++;
					}
					if( gvInFlash.mParam.mrfTorokuMode == LOGGER_TOROKU_ON )
					{
						vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0xF0 | 0x08;
						vrfHsmodePacketLength++;
						vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0xF0 | 0x0E;
						vrfHsmodePacketLength++;
						vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0xF0 | 0x02;
						vrfHsmodePacketLength++;
						vutRfDataBuffer.umData[vrfHsmodePacketLength] = ( vrfMySerialID.mOpeCompanyID & 0xF0 ) | 0x06;
						vrfHsmodePacketLength++;
						vutRfDataBuffer.umData[vrfHsmodePacketLength] = ( (vrfMySerialID.mOpeCompanyID << 4U) & 0xF0 ) | 0x02;
						vrfHsmodePacketLength++;
						vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0x21;
						vrfHsmodePacketLength++;
					}
					else if( gvInFlash.mParam.mOnCertLmt == CENTRAL )
					{
						if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Status )
						{
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mOpeCompanyID & 0xF0 | 0x01;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mOpeCompanyID << 4U) & 0xF0 | 0x09;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mAppID[0U] & 0xF0) | 0x05;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mAppID[0U] << 4U) & 0xF0 | 0x06;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mAppID[1U] & 0xF0 | 0x06;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0x71;
							vrfHsmodePacketLength++;
						}
						else
						{
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mOpeCompanyID & 0xF0 | 0x06;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mOpeCompanyID << 4U) & 0xF0 | 0x01;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mAppID[0U] & 0xF0) | 0x02;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mAppID[0U] << 4U) & 0xF0 | 0x01;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mAppID[1U] & 0xF0 | 0x07;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0x7C;
							vrfHsmodePacketLength++;
						}
					}
					else
					{
						if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Status )
						{
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mOpeCompanyID & 0xF0 | 0x09;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mOpeCompanyID << 4U) & 0xF0 | 0x06;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mAppID[0U] & 0xF0) | 0x04;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mAppID[0U] << 4U) & 0xF0 | 0x05;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mAppID[1U] & 0xF0 | 0x09;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0x17;
							vrfHsmodePacketLength++;
						}
						else
						{
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mOpeCompanyID & 0xF0 | 0x07;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mOpeCompanyID << 4U) & 0xF0 | 0x02;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mAppID[0U] & 0xF0) | 0x01;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = (vrfMySerialID.mAppID[0U] << 4U) & 0xF0 | 0x02;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mAppID[1U] & 0xF0 | 0x0E;
							vrfHsmodePacketLength++;
							vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0xEC;
							vrfHsmodePacketLength++;
						}
					}
				}
				/* GW���j�[�NID */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mUniqueID[0U];
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mUniqueID[1U];
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mUniqueID[2U];
				vrfHsmodePacketLength++;
				
				/* CRC�\�� */
				vrfHsmodePacketLength += 2U;
				
				/* �p�P�b�g��ʃR�[�h */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0x00;
				vrfHsmodePacketLength++;
				
				/* �v���f�[�^��ʃR�[�h */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfHsReqCmdCode;	/* �X�e�[�^�X���擾 */
				vrfHsmodePacketLength++;
				
				/* Wait�J�E���g�_�E���l */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = 20U - wkRadio;
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0xFF;		/* �\�� */
				vrfHsmodePacketLength++;
				
				/* �����f�[�^ */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)(gvElapseSec & 0x000000FF);			/* GW���� */
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((gvElapseSec & 0x0000FF00) >> 8U);
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((gvElapseSec & 0x00FF0000) >> 16U);
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((gvElapseSec & 0x7F000000) >> 24U);
				vrfHsmodePacketLength++;
				
				/* Online��� */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = gvInFlash.mParam.mOnCertLmt;
				vrfHsmodePacketLength++;
				
				/* �Ăяo�����K�[ID */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfHSLoggerID[ 0U ];
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfHSLoggerID[ 1U ];
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfHSLoggerID[ 2U ];
				vrfHsmodePacketLength++;
				
				/* �O���[�vID */
//				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mGroupID;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0xFF;				/* �O���[�v�ւ̃u���[�h�L���X�g�͖����� */
				vrfHsmodePacketLength ++;
				
				/* �v���f�[�^�擪���� */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((vrfHSReqHeadMeaTime[ 0U ] >> 8U) & 0x00FF);
				vrfHsmodePacketLength ++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)(vrfHSReqHeadMeaTime[ 0U ] & 0x00FF );
				vrfHsmodePacketLength ++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((vrfHSReqHeadMeaTime[ 1U ] >> 8U) & 0x00FF);
				vrfHsmodePacketLength ++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)(vrfHSReqHeadMeaTime[ 1U ] & 0x00FF );
				vrfHsmodePacketLength ++;
				
				/* �v���f�[�^�擪���n��ID */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((vrfHSReqHeadMeaID >> 8U) & 0x00FF) ;
				vrfHsmodePacketLength ++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)(vrfHSReqHeadMeaID & 0x00FF );
				vrfHsmodePacketLength ++;
				
				/* �\�� */
//				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((vrfHsReqDataNum >> 8U) & 0x00FF) ;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0U;
				vrfHsmodePacketLength ++;
//				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)(vrfHsReqDataNum & 0x00FF );
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0U;
				vrfHsmodePacketLength ++;
				
				/* AppID */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mAppID[0U];
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mAppID[1U];
				vrfHsmodePacketLength++;
				
				/* ���K�[Sleep��� */
				switch (vrfHsReqCmdCode)
				{
					case ecRfDrvGw_Hs_ReqCmd_Status:
						vutRfDataBuffer.umData[vrfHsmodePacketLength] = HSMODE_SLEEP_MODE_CNTDWN;
						break;
					case ecRfDrvGw_Hs_ReqCmd_AlartHis:
					case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
					case ecRfDrvGw_Hs_ReqCmd_EventHis:
					case ecRfDrvGw_Hs_ReqCmd_OpeHis:
					case ecRfDrvGw_Hs_ReqCmd_SysHis:
					case ecRfDrvGw_Hs_ReqCmd_Toroku:
					case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
					case ecRfDrvGw_Hs_ReqCmd_SetRead:
						vutRfDataBuffer.umData[vrfHsmodePacketLength] = HSMODE_SLEEP_MODE_TRIG;
						break;
					case ecRfDrvGw_Hs_ReqCmd_Measure:
						vutRfDataBuffer.umData[vrfHsmodePacketLength] = HSMODE_SLEEP_MODE_TRIG;
						break;
					case ecRfDrvGw_Hs_ReqCmd_SetWrite:
						vutRfDataBuffer.umData[vrfHsmodePacketLength] = HSMODE_SLEEP_MODE_NON;
						break;
					default:
						break;
				}
				vrfHsmodePacketLength ++;
				
				/* ���Ɖ��ID */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mOpeCompanyID;
				vrfHsmodePacketLength++;
				
				SubRfDrv_Coding_HS( &vutRfDataBuffer.umData[ vrfHsmodePacketLength - RF_HSMODE_BEACON_LENGTH ], RF_HSMODE_BEACON_LENGTH, BCH_OFF );
			}
			vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconTxCs;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconTxCs:
			SubSX1272Sleep();
			SubRFDrv_SyncSet();
			SubSX1272CarrierSenseFSK_DIO();
			
			RTCMK = 1U;   /* disable INTRTC interrupt */
#if swRfCsTest == imEnable
			P7_bit.no5 = 1U;
#endif
			/* Carrire Sence usec */
//			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			if( SubRFDrv_CarrierSens_5ch( RF_HSMODE_CH[ gvInFlash.mParam.mrfHsCh ]) == NO_CARRIER )
			{
#if swRfCsTest == imEnable
				P7_bit.no5 = 0U;
#endif
				vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconTxStart;
			}
			else
			{
				RTCMK = 0U;   /* enable INTRTC interrupt */
#if swRfCsTest == imEnable
				P7_bit.no5 = 0U;
#endif
				SubSX1272Sleep();
				
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
			}
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconTxStart:
			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_CALL );
			if( SubSX1272TxFSK( vrfHsmodePacketLength ) == SUCCESS )
			{
				vErrorLoopCounter = 0U;							/* ��M�^�C���A�E�g���̃��g���C�� */
//				gvrfStsEventFlag.mHsmodeRxRetry = 0U;			/* ��M���g���C�p�̃t���O�N���A */
//				vLoopLoRa = 0U;
				gvrfStsEventFlag.mHsmodeRxEnter = 0U;
				gvrfStsEventFlag.mHsmodeGwToLgDataSet = 0U;
//				gvrfStsEventFlag.mHsmodeRxEnd = 0U;
//				vrfTest = 0U;
//				vrfTestPacketCnt = 0U;
//				vrfErrorCorrectCnt = 0U;
//				vrfTestCrcErrorCnt = 0U;
//				vrfTestCorrectTimeout = 0U;
//				vrfTestHeaderCrcError = 0U;
//				vrfCorrectSetCnt = 0U;
//				vrfTestHeaderTimeOut = 0U;
//				P7_bit.no5 = 0U;
				
				SubSX1272Stdby();
				SubSX1272Sleep();
//				SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_RESPONSE );
//				SubSX1272RxFSK_DIO();
/*
				SubSX1272Write( REG_PACKETCONFIG1,
				RF_PACKETCONFIG1_PACKETFORMAT_FIXED | RF_PACKETCONFIG1_DCFREE_OFF
				 | RF_PACKETCONFIG1_CRC_OFF | RF_PACKETCONFIG1_CRCAUTOCLEAR_OFF
				 | RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT  );
*/
				SubSX1272Write( REG_PACKETCONFIG1,
					RF_PACKETCONFIG1_PACKETFORMAT_FIXED | RF_PACKETCONFIG1_DCFREE_WHITENING
					 | RF_PACKETCONFIG1_CRC_OFF | RF_PACKETCONFIG1_CRCAUTOCLEAR_OFF
					 | RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT  );
				
				vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconResRxStart;
			}
			else
			{
				/* ���M�^�C���A�E�g */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
			}
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconResRxStart:
			SubRFDrv_SyncSet();
			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_RESPONSE );
			SubSX1272RxFSK_DIO();
			SubSX1272Stdby();
			SubSX1272RxFSK_On();
			/* GW HSmode �r�[�R�������̎�M�J�n */
			if( gvrfStsEventFlag.mHsmodeRxEnter )
			{
//				SubIntervalTimer_Sleep( 72U * 375U, 2U );
				/* 2021.11.4 �҂����Ԃ̑���Ɏ�M�҂��ɂ��� */
//				SubIntervalTimer_Sleep( 95UL * 375UL, 1U );	
				SubIntervalTimer_Sleep( 110UL * 375UL, 1U );	
			}
			else
			{
				SubIntervalTimer_Sleep( 47U * 375U, 1U );
			}
			/* break�Ȃ� */
		case ecRfDrvMainSts_HSmode_BeaconRxContinue:
		
			switch( vrfHsReqCmdCode )
			{
				case ecRfDrvGw_Hs_ReqCmd_Status:
				case ecRfDrvGw_Hs_ReqCmd_Toroku:
					wkCulc = RF_HSMODE_RES_STATUS_LENGTH;
					break;
				case ecRfDrvGw_Hs_ReqCmd_Measure:
					wkCulc = RF_HSMODE_RES_MEASURE_LENGTH;
					wkReg = RF_HSMODE_RES_MEASURE_LENGTH_HEADER - 8U;		/* BCH���� */
					break;
				case ecRfDrvGw_Hs_ReqCmd_AlartHis:
					wkCulc = RF_HSMODE_RES_ALHIS_LENGTH;
					wkReg = RF_HSMODE_RES_ALHIS_LENGTH_HEADER;
					break;
				case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
					wkCulc = RF_HSMODE_RES_ABNORMAL_LENGTH;
					wkReg = RF_HSMODE_RES_ABNORMAL_LENGTH_HEADER;
					break;
				case ecRfDrvGw_Hs_ReqCmd_EventHis:
					wkCulc = RF_HSMODE_RES_EVENTHIS_LENGTH;
					wkReg = RF_HSMODE_RES_EVENTHIS_LENGTH_HEADER;
					break;
				case ecRfDrvGw_Hs_ReqCmd_OpeHis:
					wkCulc = RF_HSMODE_RES_OPEHIS_LENGTH;
					wkReg = RF_HSMODE_RES_OPEHIS_LENGTH_HEADER;
					break;
				case ecRfDrvGw_Hs_ReqCmd_SysHis:
					wkCulc = RF_HSMODE_RES_SYSHIS_LENGTH;
					wkReg = RF_HSMODE_RES_SYSHIS_LENGTH_HEADER;
					break;
				case ecRfDrvGw_Hs_ReqCmd_SetRead:
					wkCulc = RF_HSMODE_RES_SETREAD_LENGTH;
					break;
				case ecRfDrvGw_Hs_ReqCmd_SetWrite:
					wkCulc = RF_HSMODE_RES_SETQUERY_LENGTH;
					break;
				case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
					wkCulc = RF_HSMODE_RES_REQ_FIRM_LENGTH;
					break;
				default:
					break;
			}
			SubSX1272RxFSK( wkCulc );					/* ��MTimeOut���瑗�M�J�n�܂�2msec */
			vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconResAckTxCs;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			
			if( !gvRfIntFlg.mRadioTimer )
			{
				SubSX1272Sleep();
				if( gvrfStsEventFlag.mHsmodeRxEnter )
				{
					SubIntervalTimer_Stop();
					SubIntervalTimer_Sleep( 46U * 375U, 1U );
				}
				switch( vrfHsReqCmdCode )
				{
					/* Logger�X�e�[�^�X���� */
					case ecRfDrvGw_Hs_ReqCmd_Status:
					case ecRfDrvGw_Hs_ReqCmd_Toroku:
						vrfResInfo = ecRfDrv_Continue;
						if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[0U], wkCulc, BCH_OFF ) == SUCCESS )
						{
							if( (vutRfDataBuffer.umGwQueryBeacon.mGwID[0U] == vrfMySerialID.mUniqueID[0U])
								&&(vutRfDataBuffer.umGwQueryBeacon.mGwID[1U] == vrfMySerialID.mUniqueID[1U])
								&&(vutRfDataBuffer.umGwQueryBeacon.mGwID[2U] == vrfMySerialID.mUniqueID[2U]))
							{
								/* ���@���̃��X�|���X��M */
								if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[0U], wkCulc, BCH_OFF ) == SUCCESS )
								{
//									gvrfStsEventFlag.mHsmodeRxEnter = 1U;							/* ��M�J�n�t���O */
									
									if( (vutRfDataBuffer.umLoggerResStatus.mPacketCode == 0x01) ||
										(vutRfDataBuffer.umLoggerResStatus.mPacketCode == 0x0C) ||
										(vutRfDataBuffer.umLoggerResStatus.mPacketCode == 0x0D) )
									{
										/* ���K�[�o�^�A���̓X�e�[�^�X�X�V */
										ApiModbus_SndFnc16Ref41101( &vutRfDataBuffer.umLoggerResStatus );
									}
									
									switch( vutRfDataBuffer.umLoggerResStatus.mPacketCode )
									{
										/* �X�e�[�^�X������M���� */
										case 0x01:
											/* 2021.7.20 */
											vrfHsLgDct++;
											/* 2021.9.16 */
											vrfHsLgDct++;
										/* ���K�[�o�^ */
										case 0x0C:
										/* �o�^���[�h�����o�^�ς݂̓���AppID���K�[ */
										case 0x0D:
										/* �p�P�b�g��ʃR�[�h�̃G���[ */
										default:
											vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconRxContinue;
											gvModuleSts.mRf = ecRfModuleSts_Run;
											break;
									}
								}
							}
							else
							{
								/* ���@�ւ̃��X�|���X */
								vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconRxContinue;
								gvModuleSts.mRf = ecRfModuleSts_Run;
							}
						}
						else
						{
							/* CRC�G���[ */
							vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconRxContinue;
							gvModuleSts.mRf = ecRfModuleSts_Run;
						}
						break;
						
					/* Logger�v���l���� */
					case ecRfDrvGw_Hs_ReqCmd_Measure:
					/* Logger�x�񗚗��f�[�^���� */
					case ecRfDrvGw_Hs_ReqCmd_AlartHis:
					/* Logger�@��ُ헚���f�[�^���� */
					case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
					/* Logger�C�x���g�f�[�^���� */
					case ecRfDrvGw_Hs_ReqCmd_EventHis:
					/* Logger���엚���f�[�^���� */
					case ecRfDrvGw_Hs_ReqCmd_OpeHis:
					/* Logger�V�X�e�������f�[�^���� */
					case ecRfDrvGw_Hs_ReqCmd_SysHis:
#if 0
						SubIntervalTimer_Stop();
						SubIntervalTimer_Sleep( 50U * 375U, 1U );				/* Ack�����҂�����(CRC�G���[���ŏ��������������ꍇ) */
//						vrfTestPacketCnt++;										/* ��M�p�P�b�g�������J�E���g���� */
#else
#endif
						if( SubRfDrv_Header_Decryption( wkReg ) == ecRfDrv_Success )
						{
//							gvrfStsEventFlag.mHsmodeRxEnter = 1U;			/* ��M�J�n�t���O */
							vErrorLoopCounter = 0U;								/* ���g���C�J�E���^ ���Z�b�g */
							vrfResInfo = ecRfDrv_Success;
							switch( vrfHsReqCmdCode )
							{
								case ecRfDrvGw_Hs_ReqCmd_Measure:
									vrfResInfo = SubRfDrv_Payload_Decryption( 10U, 10U, 9U, RF_HSMODE_RES_MEASURE_LENGTH_HEADER );
									break;
									
								case ecRfDrvGw_Hs_ReqCmd_AlartHis:
									vrfResInfo = SubRfDrv_Payload_Decryption( 8U, 10U, 10U, RF_HSMODE_RES_ALHIS_LENGTH_HEADER );
									break;
									
								case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
									vrfResInfo = SubRfDrv_Payload_Decryption( 6U, 10U, 10U, RF_HSMODE_RES_ABNORMAL_LENGTH_HEADER );
									break;
									
								case ecRfDrvGw_Hs_ReqCmd_EventHis:
									vrfResInfo = SubRfDrv_Payload_Decryption( 14U, 10U, 6U, RF_HSMODE_RES_EVENTHIS_LENGTH_HEADER );
									break;
									
								case ecRfDrvGw_Hs_ReqCmd_OpeHis:
									vrfResInfo = SubRfDrv_Payload_Decryption( 10U, 10U, 9U, RF_HSMODE_RES_OPEHIS_LENGTH_HEADER );
									break;
									
								case ecRfDrvGw_Hs_ReqCmd_SysHis:
									vrfResInfo = SubRfDrv_Payload_Decryption( 10U, 10U, 9U, RF_HSMODE_RES_SYSHIS_LENGTH_HEADER );
									
								default:
									break;
							}
						}
						else
						{
							vrfResInfo = ecRfDrv_Fail;
						}
						/* 1�p�P�b�g��M���� ACK/NACK�������M�� */
						
						/* �����M����Wait�ǉ� */
						SubRfDrv_Hsmode_ResRx_AfterWait();
						
						break;
						
					/* Logger�ݒ�l�ǂݍ��� */
					case ecRfDrvGw_Hs_ReqCmd_SetRead:
//						SubIntervalTimer_Stop();
//						SubIntervalTimer_Sleep( 50U * 375U, 1U );				/* Ack�����҂�����(CRC�G���[���ŏ��������������ꍇ) */
//						vrfTestPacketCnt++;										/* ��M�p�P�b�g�������J�E���g���� */
						vrfResInfo = ecRfDrv_Success;
						/* �w�b�_�[����CRC�`�F�b�N(BCH����) */
						if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[0U], RF_HSMODE_RES_SETREAD_LENGTH_HEADER, BCH_OFF ) != SUCCESS )
						{
							/* CRC�G���[ Nack���� */
//							vrfTestHeaderCrcError++;
							vrfResInfo = ecRfDrv_End;
						}
						if( memcmp( vutRfDataBuffer.umGwQueryBeacon.mGwID, vrfMySerialID.mUniqueID, 3U ) != 0 && (vrfResInfo == ecRfDrv_Success) )
						{
							/* ���@���ł͂Ȃ� ������ */
							vrfResInfo = ecRfDrv_End;
						}
						if( (SubRFDrv_Hsmode_BcnPktCodeCheck() != SUCCESS) && (vrfResInfo == ecRfDrv_Success) )
						{
							/* �p�P�b�g��ʃR�[�h�̃G���[ ������ */
							vrfResInfo = ecRfDrv_End;
						}
						if( vrfResInfo == ecRfDrv_Success )
						{
							gvrfStsEventFlag.mHsmodeRxEnter = 1U;			/* ��M�J�n�t���O */
							vErrorLoopCounter = 0U;								/* ���g���C�J�E���^ ���Z�b�g */
							/* CRC�`�F�b�N */
							CRCD = 0x0000U;
							for( wkRadio = RF_HSMODE_RES_SETREAD_LENGTH_HEADER; wkRadio < (RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_BCH - 2U); wkRadio++ )
							{
								CRCIN = vutRfDataBuffer.umData[wkRadio];
							}
							if((vutRfDataBuffer.umLoggerResSetting.mCrc2[1U] == (uint8_t)(CRCD) )
									&& (vutRfDataBuffer.umLoggerResSetting.mCrc2[0U] == (uint8_t)(CRCD >> 8U) ) )
							{
								/* �G���[�Ȃ� Ack���� */
//								vrfTest++;
							}
							else
							{
								/* �G���[���� �G���[�������� 20�f�[�^���Ƃɏ���(�[���̓��[�v�O�ŏ���) */
								for( wkRadio = 0U; wkRadio < 7U; wkRadio++ )
								{
									vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( &vutRfDataBuffer.umData[ RF_HSMODE_RES_SETREAD_LENGTH_HEADER + ( wkRadio * 20U ) ] 
																				,RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_HEADER - RF_HSMODE_RES_SETREAD_LENGTH_BCH -  ( wkRadio * 10U)
																				,20U );
								}
								vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( &vutRfDataBuffer.umData[ RF_HSMODE_RES_SETREAD_LENGTH_HEADER + ( wkRadio * 20U ) ]
																			, RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_HEADER - RF_HSMODE_RES_SETREAD_LENGTH_BCH -  ( wkRadio * 10U)
																			, RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_HEADER - RF_HSMODE_RES_SETREAD_LENGTH_BCH - ( wkRadio * 20U ) );
								/* CRC�`�F�b�N */
								CRCD = 0x0000U;
								for( wkRadio = RF_HSMODE_RES_SETREAD_LENGTH_HEADER; wkRadio < (RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_BCH - 2U); wkRadio++ )
								{
									CRCIN = vutRfDataBuffer.umData[wkRadio];
								}
								if((vutRfDataBuffer.umLoggerResSetting.mCrc2[1U] == (uint8_t)(CRCD) )
										&& (vutRfDataBuffer.umLoggerResSetting.mCrc2[0U] == (uint8_t)(CRCD >> 8U) ) )
								{
									/* �G���[�������� Ack���� */
//									vrfTest++;
//									vrfCorrectSetCnt++;
								}else
								{
									/* �ʐM���s �p�P�b�g�j�� ������ */
									vrfResInfo = ecRfDrv_End;
//									vrfTestCrcErrorCnt++;
								}
							}
						}
						SubRfDrv_Hsmode_ResRx_AfterWait();
						
						break;
						
					/* �ݒ�ύX���̑��M */
					case ecRfDrvGw_Hs_ReqCmd_SetWrite:
						SubIntervalTimer_Stop();
						SubIntervalTimer_Sleep( 40U * 375U, 1U );				/* Ack�����҂�����(CRC�G���[���ŏ��������������ꍇ) */
//						vrfTestPacketCnt++;										/* ��M�p�P�b�g�������J�E���g���� */
						vrfResInfo = ecRfDrv_Success;
						/* �w�b�_�[����CRC�`�F�b�N(BCH����) */
						if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[0U], RF_HSMODE_RES_SETQUERY_LENGTH, BCH_OFF ) != SUCCESS )
						{
							/* CRC�G���[ Nack���� */
//							vrfTestHeaderCrcError++;
							vrfResInfo = ecRfDrv_End;
						}
						if( memcmp( vutRfDataBuffer.umGwQueryBeacon.mGwID, vrfMySerialID.mUniqueID, 3U ) != 0 && (vrfResInfo == ecRfDrv_Success) )
						{
							/* ���@���ł͂Ȃ� ������ */
							vrfResInfo = ecRfDrv_End;
						}
						if( (SubRFDrv_Hsmode_BcnPktCodeCheck() != SUCCESS) && (vrfResInfo == ecRfDrv_Success) )
						{
							/* �p�P�b�g��ʃR�[�h�̃G���[ ������ */
							vrfResInfo = ecRfDrv_End;
						}
						if( vrfResInfo == ecRfDrv_Success )
						{
							vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_DataSet;
//							gvModuleSts.mRf = ecRfModuleSts_Run;
							gvModuleSts.mRf = ecRfModuleSts_Sleep;
//							SubRfDrv_Hsmode_ResRx_AfterWait();
							vrfPacketSum = 0U;
							vrfPacketLimit = 1U;
						}
						break;
						
					case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
						vrfResInfo = ecRfDrv_End;
						SubIntervalTimer_Stop();
						SubIntervalTimer_Sleep( 40U * 375U, 1U );				/* Ack�����҂�����(CRC�G���[���ŏ��������������ꍇ) */
						/* CRC�`�F�b�N */
						if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[ 0U ], RF_HSMODE_ACK_LENGTH, BCH_OFF ) == SUCCESS )
						{
							if( (vutRfDataBuffer.umLoggerAckNack.mGwID[0U] == vrfMySerialID.mUniqueID[0U]) &&
								(vutRfDataBuffer.umLoggerAckNack.mGwID[1U] == vrfMySerialID.mUniqueID[1U]) &&
								(vutRfDataBuffer.umLoggerAckNack.mGwID[2U] == vrfMySerialID.mUniqueID[2U]) )
							{
								if( (vutRfDataBuffer.umLoggerAckNack.mLoggerID[0U] == vrfHSLoggerID[0U]) &&
									(vutRfDataBuffer.umLoggerAckNack.mLoggerID[1U] == vrfHSLoggerID[1U]) &&
									(vutRfDataBuffer.umLoggerAckNack.mLoggerID[2U] == vrfHSLoggerID[2U]) )
								{
									/* �t�@�[���A�b�v������OK���`�F�b�N */
									if( vutRfDataBuffer.umLoggerAckNack.mResponce == 0U )
									{
										vrfResInfo = ecRfDrv_Success;
										/* �t�@�[���A�b�vOK */
										
										vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_DataSet;
//										gvModuleSts.mRf = ecRfModuleSts_Run;
										gvModuleSts.mRf = ecRfModuleSts_Sleep;
										vrfPacketSum = 0U;
										vErrorLoopCounter = 0U;
										vrfPacketLimit = gvInFlash.mParam.mFirmPacketNum + 2U;		/* �ŏ��̋�p�P�b�g�ƍŌ��CRC����2�p�P�b�g�𑫂� */
									}
									else
									{
										/* �t�@�[���A�b�vNG */
									}
								}
								else
								{
									/* ���@���łȂ� ���M�I�� */
								}
							}
							else
							{
								/* �ʐM�����GW�łȂ� ���M�I�� */
							}
						}
						else
						{
							/* CRC�G���[ */
						}
						break;
						
					default:
						break;
				}
			}
			else
			{
				/* ��M�^�C���A�E�g Nack���� */
//				vrfTestHeaderTimeOut++;
				vrfResInfo = ecRfDrv_Error;
			}
			
			/*
			***Responce ���ʏ���
			*  END:�I��
			*  ERROR:��M���g���C
			*  FAIL:NACK����
			*  SUCCESS:ACK���� or Responce���M
			*  CONTINUE:Beacon��M�p��
			*/
			SubSX1272Sleep();
			if( vrfResInfo == ecRfDrv_End )
			{
				/* �t�@�[���A�b�v�v���ɑ΂���Ack��NG */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;				/* �Z�K�����l�� */
			}
			else if( vrfResInfo == ecRfDrv_Continue )
			{
				vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconRxContinue;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else if( vrfResInfo != ecRfDrv_Error )
			{
				/* Success or Fail */
				if( vrfResInfo == ecRfDrv_Fail )
				{
					ApiRfDrv_Led( imOFF );
				}
				else
				{
					ApiRfDrv_Led( imON );
				}
				vLoopLoRa = 4U;
				/* 2021.9.16  �����񐔕� �Ăяo��wait */
				vrfHsLgDct ++;
				SubRFDrv_Hsmode_Logger_BcnAckDataSet();
				gvrfStsEventFlag.mHsmodeGwToLgDataSet = 1U;		/* ���C�����[�`���ł̃f�[�^�Z�b�g�ʒm�t���O */
			}
			else if( gvrfStsEventFlag.mHsmodeRxEnter == 0U )
			{
				/* �p�P�b�g��M�J�n���Ă��Ȃ���� */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ERROR�̏ꍇ�͖��������� �^�C���A�E�g�ȊO�̃G���[�͉i�v���[�v�h�~���K�v����(ERROR�����͍l�����Ȃ���) */
				SubIntervalTimer_Stop();
				vErrorLoopCounter++;
				if( vErrorLoopCounter < 10U )
				{
					vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconResRxStart;
					/* 2021.11.4 �҂����Ԃ̑���Ɏ�M�҂��ɂ��� */
					gvModuleSts.mRf = ecRfModuleSts_Run;
//					SubIntervalTimer_Sleep( 15U * 375U, 1U );					/* ���K�[����Ack�҂��^�C���A�E�g���ԂɈˑ� Pre���m�̃��g���C��20��93msec 25��104msec */
//					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					/* ERROR�̃��g���C�񐔂��I�[�o�[ */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					
					/* �����ʐM�A����M���� */
					gvutComBuff.umModInf.mHiPriorityFlg = imOFF;		/* CPU��Modbus�ʐM�̍ŗD�摗�M�t���O(�����ʐM���W�f�[�^����)��OFF */
					gvutComBuff.umModInf.mComSts = ecComSts_SndEnd;		/* Modbus���M�����֑J�� */
					gvModuleSts.mMod = ecModModuleSts_Run;
					
					vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;
				}
			}
			
			if( gvRfIntFlg.mRadioTimer )
			{
				/* ������50msec(��M�J�n����100msec)�ȏォ�������ꍇ�͑�Ack���M */
				gvModuleSts.mRf = ecRfModuleSts_Run;
				/* �f�o�b�O */
				if( vRfDrvMainGwSts != ecRfDrvMainSts_RTmode_GWIdle )
				{
					NOP();
				}
			}
//			P7_bit.no5 = 0U;
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconResAckTxCs:
			SubSX1272Sleep();
			if( !gvRfIntFlg.mRadioTimer )
			{
				NOP();
			}
			SubSX1272CarrierSenseFSK_DIO();
			RTCMK = 1U;   /* disable INTRTC interrupt */
			/* Carrire Sence usec */
			if( SubRFDrv_CarrierSens_5ch( RF_HSMODE_CH[ gvInFlash.mParam.mrfHsCh ]) == NO_CARRIER )
			{
				vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconResAckTxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				RTCMK = 0U;   /* enable INTRTC interrupt */
				SubSX1272Sleep();
				if( vLoopLoRa > 0U )
				{
					vLoopLoRa--;
					SubIntervalTimer_Sleep( 1U * 375U, 1U );		/* (2.2msec + 1msec) * (vLoopLoRa + 1) */
					vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconResAckTxCs;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					vErrorLoopCounter++;
					if( vErrorLoopCounter < 10U )
					{
						vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconResRxStart;
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					else
					{
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
					}
				}
			}
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconResAckTxStart:
			RTCMK = 0U;   /* enable INTRTC interrupt */
			SubSX1272Stdby();
			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_ACK );
			SubRFDrv_SyncSet();
			if( SubSX1272TxFSK( RF_HSMODE_ACK_LENGTH ) != SUCCESS )
			{
				/* ���M���s������M���p������ */
				vErrorLoopCounter++;
				if( vErrorLoopCounter > 10U )
				{
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
					break;
				}
			}
			gvModuleSts.mRf = ecRfModuleSts_Run;
			vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconResRxStart;
			break;
			
		/* �ݒ�ύX�f�[�^���M */
		/* �t�@�[��HEX�f�[�^���M */
		case ecRfDrvMainSts_HSmode_Gw_To_Lg_DataSet:
			SubRFDrv_Hsmode_Logger_BcnAckDataSet();				/* 5.4msec */
			vLoopLoRa = 20U;			/* CS���g���C�񐔃Z�b�g */
			if( gvrfStsEventFlag.mHsmodeRxEnter == 0U )
			{
				/* ���� */
				gvrfStsEventFlag.mHsmodeRxEnter = 1U;
				vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_CS;
				
				if( gvRfIntFlg.mRadioTimer )
				{
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
			}
			else
			{
				vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Lg_To_Gw_DataAckRx;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_HSmode_Gw_To_Lg_CS:
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
			RTCMK = 1U;   /* disable INTRTC interrupt */
			/* Carrire Sence usec */
			if( SubRFDrv_CarrierSens_5ch( RF_HSMODE_CH[ gvInFlash.mParam.mrfHsCh ]) == NO_CARRIER )
			{
				vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_Tx;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				RTCMK = 0U;   /* enable INTRTC interrupt */
				SubSX1272Sleep();
				if( vLoopLoRa > 0U )
				{
					vLoopLoRa--;
					SubIntervalTimer_Sleep( 1U * 375U, 1U );
					vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_CS;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			break;
			
		case ecRfDrvMainSts_HSmode_Gw_To_Lg_Tx:
			RTCMK = 0U;   /* enable INTRTC interrupt */
			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_FIRM_HEX );
			//SubRFDrv_SyncSet();
			
			switch (vrfHsReqCmdCode)
			{
				case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
					switch( vErrorLoopCounter )
					{
						case 0U:
							/* �O��Ack��M �������͍ŏ��̃p�P�b�g */
							vrfPacketSum++;							/* �ʐM�����@���̃p�P�b�g���Z�b�g */
							SubSX1272TxFSK( RF_HSMODE_FIRM_HEX_LENGTH );
							break;
						case 1U:
							/* ���g���C���� */
							vrfPacketSum--;							/* ���g���C���͐�ɃZ�b�g�ς݂̃p�P�b�g��߂� */
							SubIntervalTimer_Sleep( 50U * 375U, 1U );
							if( !gvRfIntFlg.mRadioTimer ){ M_HALT; }
							break;
						default:
							/* ���g���C�� */
							SubSX1272TxFSK( RF_HSMODE_FIRM_HEX_LENGTH );
							break;
					}
					
					SubIntervalTimer_Sleep( 50U * 375U, 3U );	/* 150msec */
					vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_DataSet;
					break;
					
				case ecRfDrvGw_Hs_ReqCmd_SetWrite:
					for( wkRadio = 0U; wkRadio < 3U; wkRadio++ )
					{
						SubSX1272TxFSK( RF_HSMODE_RES_SETWRITE_LENGTH );				/* 12.6msec */
						SubSX1272Sleep();
						SubIntervalTimer_Sleep( 1U * 375U, 1U );
						if( !gvRfIntFlg.mRadioTimer ){ M_HALT; }
					}
					vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Lg_To_Gw_DataAckRx;		/* ����ACK��M */
					break;
					
				default:
					SubSX1272TxFSK( RF_HSMODE_RES_SETWRITE_LENGTH );
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
					break;
			}
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_HSmode_Lg_To_Gw_DataAckRx:
			/* Ack Preamble���o ��M�J�n */
			SubSX1272RxFSK_On();
			SubSX1272RxFSK_DIO();
//			P7_bit.no5 = 1U;
//			SubIntervalTimer_Sleep( 13U * 375U, 1U );					/* GW��ACK���M����4.9msec 10msec��TimeOut,11msec�͎�M�ł��� */
			switch( vrfHsReqCmdCode )
			{
				case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
					SubIntervalTimer_Sleep( 65U * 375U, 3U );					/* GW��ACK���M����4.9msec 10msec��TimeOut,11msec�͎�M�ł��� */
					break;
				case ecRfDrvGw_Hs_ReqCmd_SetWrite:
//					SubIntervalTimer_Sleep( 50U * 375U, 1U );					/* GW��ACK���M����4.9msec 10msec��TimeOut,11msec�͎�M�ł��� */
					/* 2022.8.10 */
					SubIntervalTimer_Sleep( 80U * 375U, 1U );					/* GW��ACK���M����4.9msec 10msec��TimeOut,11msec�͎�M�ł��� */
					break;
				default:
					SubIntervalTimer_Sleep( 50U * 375U, 1U );
					break;
			}
			SubSX1272RxFSK( RF_HSMODE_ACK_LENGTH );
//			P7_bit.no5 = 0U;
			SubSX1272Sleep();

//			vrfInitState = ecRfDrv_Init_Non;
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			if( !gvRfIntFlg.mRadioTimer )
			{
				/* CRC�`�F�b�N */
				if( SubRfDrv_Crc_Decoding_HS( &vrfHsmodeAckBuff.umData[ 0U ], RF_HSMODE_ACK_LENGTH, BCH_OFF ) == SUCCESS )
				{
					wkRadio = SUCCESS;
					if( memcmp( vrfHsmodeAckBuff.umCategories.mGwID, vrfMySerialID.mUniqueID, 3U ) != 0 )
					{
						/* �ʐM�����GW�łȂ� ���M�I�� */
						wkRadio = FAIL;
					}
					
					if( memcmp( vrfHsmodeAckBuff.umCategories.mLoggerID, vrfHSLoggerID, 3U ) != 0 )
					{
						/* ���@���łȂ� ���M�I�� */
						wkRadio = FAIL;
					}
					
					switch (vrfHsReqCmdCode)
					{
						case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
							if( vrfHsmodeAckBuff.umCategories.mPacketCode != 0x14 )
							{
								/* �R�}���h�R�[�h�s��v */
								wkRadio = FAIL;
							}
							break;
						case ecRfDrvGw_Hs_ReqCmd_SetWrite:
							if( vrfHsmodeAckBuff.umCategories.mPacketCode != 0x10 )
							{
								/* �R�}���h�R�[�h�s��v */
								wkRadio = FAIL;
							}
							break;
						default:
							/* �p�P�b�g�Ⴂ�I�� */
							wkRadio = FAIL;
							break;
					}
					
					if( wkRadio == SUCCESS )
					{
						/* ��M���� */
						if( vrfHsmodeAckBuff.umCategories.mResponce == 0x01 )
						{
							/* NACK��M �p�P�b�g�đ� (���ӁF�i��NACK�������Ɖi�v���[�v����) */
							vErrorLoopCounter++;
							
							/* �t�@�[��CRC���Z���ʂ�NACK���� */
							if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_ReqFirm && vrfPacketSum > vrfPacketLimit )
							{
								/* Linux��CRC����(NG)���� */
								ApiModbus_SndRef40047( 0U );
								vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;
							}
							else
							{
								/* �|�C���^�ύX�Ȃ� */
								vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_CS;
							}
						}
						else
						{
							/* ACK��M */
							vErrorLoopCounter = 0U;				/* ���g���C�񐔃J�E���g�ϐ����Z�b�g */
							
							switch( vrfHsReqCmdCode )
							{
								case ecRfDrvGw_Hs_ReqCmd_SetWrite:
									/* �ݒ菑������ */
									ApiModbus_SndFnc16Ref42001( &vrfHsmodeAckBuff.umCategories );
									vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;
									break;
								case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
									
									if( vrfPacketSum <= vrfPacketLimit )
									{
										vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_CS;
									}
									else
									{
										/* Linux��CRC����(OK)���� */
										ApiModbus_SndRef40047( 1U );
										break;
									}
									break;
							}
//							vrfPacketSum++;						/* ���̃p�P�b�g */
						}
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					else
					{
						/* ID,Code�s��v */
						SubIntervalTimer_Sleep( 100U * 375U, 1U );
					}
				}
				else
				{
					/* �w�b�_CRC�G���[ */
					vErrorLoopCounter++;
					vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_CS;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_SetWrite )
				{
					SubIntervalTimer_Sleep( 100U * 375U, 1U );
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
			}
			else
			{
				vErrorLoopCounter++;
				/* ��M�^�C���A�E�g */
				if( vErrorLoopCounter > 20U )
				{
					M_NOP;
					/* �t�@�[����M���s */
				}
				else
				{
					//SubIntervalTimer_Sleep( 72U * 375U, 1U );			/* Ack�҂�20msec + ����������5.4msec + �L�����A�Z���X0.6*5ch msec */
					vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_CS;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			break;
			
		/***************** GW RTmode Lora ************************/
		case ecRfDrvMainSts_RTmode_GWInit:
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
//			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
			rfStatus.Modem = MODEM_LORA;
			SubSX1272Sleep();
			
			/* �H��o�׎��Œ�l */
			rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
			vrfMySerialID.mOpeCompanyID = gvInFlash.mId.mOpeCompanyID;
			vrfMySerialID.mUniqueID[0U] = gvInFlash.mId.mUniqueID[ 0U ];
			vrfMySerialID.mUniqueID[1U] = gvInFlash.mId.mUniqueID[ 1U ];
			vrfMySerialID.mUniqueID[2U] = gvInFlash.mId.mUniqueID[ 2U ];
			
			/* ******���[�U�[�ݒ�****** */
//			gvInFlash.mParam.mrfLoraChGrupeCnt = 0;
//			gvInFlash.mParam.mOnCertLmt = CENTRAL;
//			gvInFlash.mParam.mOnCertLmt = LOCAL;
			
//			vrfRtChSelect = CH_MANU;
//			vrfRtChSelect = CH_AUTO;
			vrfRtChSelect = gvInFlash.mParam.mrfRtChSelect;
			
			vrfMySerialID.mGroupID = gvInFlash.mId.mGroupID;
			vrfMySerialID.mAppID[0U] = ((gvInFlash.mId.mAppID[ 0U ] << 4U) & 0xF0) + ((gvInFlash.mId.mAppID[ 1U ] >> 4U) & 0x0F);
			vrfMySerialID.mAppID[1U] = (gvInFlash.mId.mAppID[ 1U ] << 4U) & 0xF0;
#if (swKouteiCalibration == imEnable)
			vrfRtChSelect = CH_MANU;
			vrfLoraChGrupeMes = 12U;			/* Lora�̃`���l���O���[�v�v���l�ʐM�p 0-6,7-25�͋Z�K�p 33-51ch */
#else
			if(vrfRtChSelect == CH_MANU)
			{
//				vrfLoraChGrupeMes = 0U;			/* Lora�̃`���l���O���[�v�v���l�ʐM�p 0-6,7-25�͋Z�K�p 33-51ch 12U(38ch)�͉��x�����W�O */
				vrfLoraChGrupeMes = gvInFlash.mParam.mrfLoraChGrupeMes;
			}
#endif
//			vrfLoraChGrupeCnt = 24U;			/* �ڑ��ʐM�p */
//			gvInFlash.mParam.mrfRTmodeCommInt = 10U;				/* �v���l�ʐM�Ԋu */
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;
			
//			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_CNCT[vrfLoraChGrupeCnt].mLoraSync );
			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mLoraSync );
			
			if( gvInFlash.mParam.mOnCertLmt == LOCAL )
			{
				vrfMySerialID.mAppID[0U] = 0xFF;
				vrfMySerialID.mAppID[1U] = 0xE0;
			}
			vLoopLoRa = 0U;
			vCallChLoopLora = 0U;
			
			SubSX1272Sleep();
			
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			/*break;�Ȃ�*/
			
		case ecRfDrvMainSts_RTmode_ConnectAuthCadSetting:
			if( gvInFlash.mParam.mrfRTmodeCommInt > 20U )
			{
				vRfDrv_CommIntMode = ecRfDrv_LongIntMode;
				vrfMesureCommIntTime = RT_INT_LONG_MODE_TX_INT;
				vrfSetCommTime = RT_INT_LONG_MODE_SET_TIME;
			}
			else if( gvInFlash.mParam.mrfRTmodeCommInt == 20U )
			{
				vRfDrv_CommIntMode = ecRfDrv_MidIntMode;
				vrfMesureCommIntTime = RT_INT_MID_MODE_TX_INT;
				vrfSetCommTime = RT_INT_MID_MODE_SET_TIME;
			}
			else if( gvInFlash.mParam.mrfRTmodeCommInt == 10U )
			{
				vRfDrv_CommIntMode = ecRfDrv_ShortIntMode;
				vrfMesureCommIntTime = RT_INT_SHORT_MODE_TX_INT;
				vrfSetCommTime = RT_INT_SHORT_MODE_SET_TIME;
			}

			if( gvrfStsEventFlag.mReset == RFIC_INI )
			{
				SubRfDrv_RfReset();
				gvrfStsEventFlag.mReset = RFIC_RUN;
			}
			SubSX1272Sleep();
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
//			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
			rfStatus.Modem = MODEM_LORA;
			SubSX1272Sleep();
//			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_CNCT[vrfLoraChGrupeCnt].mLoraSync );
			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mLoraSync );
			
			SubSX1272Write( REG_LR_PAYLOADLENGTH, RT_MODE_PAYLOAD_LENGTH_1 );
			SubSX1272Write( REG_LR_PREAMBLELSB, 10U);
			SubSX1272Write( REG_LR_FIFORXBASEADDR, 0x00 );
			SubSX1272Write( REG_LR_FIFOADDRPTR, 0x00 );
			SubSX1272Write( REG_LR_IRQFLAGS, 0xFF );
			SubSX1272Write( REG_LR_MODEMCONFIG1, RFLR_MODEMCONFIG1_BW_125_KHZ
				 | RFLR_MODEMCONFIG1_CODINGRATE_4_7 | RFLR_MODEMCONFIG1_IMPLICITHEADER_ON
				 | RFLR_MODEMCONFIG1_RXPAYLOADCRC_OFF | RFLR_MODEMCONFIG1_LOWDATARATEOPTIMIZE_OFF);
			SubSX1272CadLoRa_DIO();
			if( vCallChLoopLora >= RT_MODE_CH_LOOP_NUM ){	vCallChLoopLora = 0U; }
//			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[vrfLoraChGrupeCnt].mCh[vCallChLoopLora], rfFreqOffset);
			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mCh[ vCallChLoopLora ], rfFreqOffset);
			
			SubSX1272Write( REG_LR_LNA , RFLR_LNA_GAIN_G1 | RFLR_LNA_BOOST_ON );
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureCadSetting:
			SubSX1272Sleep();
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
//			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
			rfStatus.Modem = MODEM_LORA;
			SubSX1272Sleep();
			SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_MEASURE[ vrfLoraChGrupeMes ].mLoraSync );
			SubSX1272CadLoRa_DIO();
			SubSX1272Write( REG_LR_FIFORXBASEADDR, 0x00 );				// Full buffer used for Tx
			SubSX1272Write( REG_LR_PAYLOADLENGTH, RT_MODE_PAYLOAD_LENGTH_5 );
			SubSX1272Write( REG_LR_PREAMBLELSB, 10U);
			SubSX1272Write( REG_LR_FIFOADDRPTR, 0x00 );
			SubSX1272Write( REG_LR_MODEMCONFIG1, RFLR_MODEMCONFIG1_BW_125_KHZ
				 | RFLR_MODEMCONFIG1_CODINGRATE_4_7 | RFLR_MODEMCONFIG1_IMPLICITHEADER_ON
				 | RFLR_MODEMCONFIG1_RXPAYLOADCRC_OFF | RFLR_MODEMCONFIG1_LOWDATARATEOPTIMIZE_OFF);
			vLoopLoRa = 0U;
			if( vCallChLoopLora >= RT_MODE_CH_LOOP_NUM ){	vCallChLoopLora = 0U; }
			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_MEASURE[vrfLoraChGrupeMes].mCh[vCallChLoopLora], rfFreqOffset);
			SubSX1272Write( REG_LR_LNA , RFLR_LNA_GAIN_G1 | RFLR_LNA_BOOST_ON );
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		/* GW Idle */
		case ecRfDrvMainSts_RTmode_GWIdle:
			wkRadio = gvRfTimingCounterH % vrfMesureCommIntTime;
				wkRtmodeInterval = gvRfTimingCounterH % gvInFlash.mParam.mrfRTmodeCommInt;
			ApiRfDrv_Led( imOFF );
			
//			wkRtmodeInterval = 1U;
//			vrfLoggerSumNumber = 1U;
//			SubSX1272Sleep();												/* �������K�v */
			
			if( wkRadio > 0U )
			{
				gvrfStsEventFlag.mMuskMultiEvent = 0U;				/* wkRadio == 0U�̘A���C�x���g�̖h�~ */
			}
			if( wkRadio == 0U && !gvrfStsEventFlag.mMuskMultiEvent )
			{
				/* 1min������1��̂� RTmode��M�J�n */
				gvrfStsEventFlag.mMuskMultiEvent = 1U;			/* �A���C�x���g�̖h�~ */
				gvrfStsEventFlag.mMuskCadSetting = 0U;			/* Cad����̂ݐݒ� */
				gvrfStsEventFlag.mTempMeasure = 0U;
				vrfRtmodeCommTimeTotal = 0U;
				gvrfStsEventFlag.mReset = RFIC_INI;
				
				if( wkRtmodeInterval == 0U )
				{
					/* �قȂ�AppID��M�ɂ��ʐM�`���l���}�X�N�����𔻒� */
					for( wkCulc = 0U; wkCulc < vrfLoggerSumNumber; wkCulc++ )
					{
						if( vrfLoggerComCnt[wkCulc] > 0U )
						{
							break;
						}
					}
					if( wkCulc == vrfLoggerSumNumber )
					{
						/* �S���K�[�̒ʐM���� or ���K�[�Ȃ� */
						vrfRtMeasChMask = RT_MODE_CH_LOOP_NUM + 1U;		/* �ő�l���+1�Ń}�X�N�Ȃ� */
					}
					/* �������ˍ��� �ύX���̃��X�g�폜���l�� */
#if 1
//					if( gvModeSelect == RT_MODE )
#else
					if( gvInFlash.mParam.mModeSelect == RT_MODE )
#endif
//					{
						SubRfDrv_RtConctCut();
//					}
					
					/* �ʐM���s�J�E���g���� */
					if( vrfLoggerSumNumber > 0U )
					{
						for( wkCulc = 0U; wkCulc < vrfLoggerSumNumber; wkCulc++ )
						{
							if( vrfLoggerComCnt[wkCulc] < 0xFF )
							{
								vrfLoggerComCnt[wkCulc]++;			/* �ʐM���s�J�E���g+1 */
								
								if( vrfLoggerComCnt[ wkCulc ] == 2U )
								{
									/* �ʐM���s���M */
									ApiModbus_SndRef40025( &vrfLoggerList[ wkCulc ][ 0U ] );
								}
							}
						}
					}
					else if( vrfRtChSelect == CH_AUTO )
					{
						vrfLoraChGrupeMes = rand() % 7U;			/* �����`���l���I�� */
					}
				}
#if 1
				if( gvModeSelect == RT_MODE )
#else
				if( gvInFlash.mParam.mModeSelect == RT_MODE )
#endif
				{
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadSetting;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					break;
				}
			}
			
#if swRfMonitor == imEnable
			vrfLoraChGrupeMes = 2U;
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadScan;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
#endif
//			gvInFlash.mParam.mModeSelect = TEST_MODE;
#if 1
			switch( gvModeSelect )
#else
			switch( gvInFlash.mParam.mModeSelect )
#endif
			{
				case RT_MODE:
					vCallChLoopLora = 0U;
					gvrfStsEventFlag.mHsmodeInit = 0U;
//					if( (wkRtmodeInterval <= (((vrfLoggerSumNumber + 1U) * 660U)/1000U)) && vrfLoggerSumNumber > 0U )
					if( (wkRadio <= (((vrfLoggerSumNumber + 1U) * 660U)/1000U)) && vrfLoggerSumNumber > 0U )
					{
						/* RTmode �ڑ��䐔�ɉ������v���l���M���� */
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadScan;
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					else if( !gvrfStsEventFlag.mMuskCadSetting )
					{
//						P7_bit.no5 = 0U;
						/* �v���l�ʐM������ɂP����s */
						SubRfDrv_RtConctCut();				/* �v���l�ʐM�s�ʃJ�E���g�A�b�v�����A�ؒf���� */
						/* �v���l��M������A�P�񏉊��� */
						gvrfStsEventFlag.mMuskCadSetting = 1U;
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					/* ���x�ɂ��^�C�~���O�␳ */
					else if( ( wkRadio == vrfMesureCommIntTime - 1U ) 
							&& ( !gvrfStsEventFlag.mTempMeasure ) )
					{
						ApiRfDrv_TimingCorrect();					/* �ʐM�����̒����ꍇ�̂ݏC�� */
						gvrfStsEventFlag.mTempMeasure = 1U;
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_BeaconCadScan;
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					else
					{
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_BeaconCadScan;
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					break;
					
				case HS_MODE:
					wkRadio = 0U;
//					switch ( vrfHsmodeDuty )
					if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Status )
					{
						switch( gvInFlash.mParam.mHsmodeDuty )
						{
							default:
							case 0U:
								/* �펞FSK���M */
								wkRadio = 1U;
								break;
							case 1U:
								/* 1min���Ƃ�25sec���M */
								if( ( gvRfTimingCounterH > vrfHsmodeRandom + 10UL ) && ( gvRfTimingCounterH < ( vrfHsmodeRandom + 36UL ) ) )		/* 10-60sec ���� 35-85sec */
								{
									wkRadio = 1U;
								}
									else if( gvRfTimingCounterH > ( vrfHsmodeRandom + 36UL ) )
									{
										gvrfStsEventFlag.mHsmodeRandom = 0U;					/* �v���ʐM�Ȃǂɂ��������߂����P�[�X */
									}
									else if( gvRfTimingCounterH <= (vrfHsmodeRandom + 10UL) )
									{
										if( ((vrfHsmodeRandom + 10UL) - gvRfTimingCounterH) > 60UL )
										{
											gvrfStsEventFlag.mHsmodeRandom = 0U;				/* �v���ʐM�Ȃǂɂ��������߂��A7200sec�����[���I�[�o�[�����P�[�X */
										}
									}
								break;
							case 2U:
								/* 3min���Ƃ�25sec���M */
								if( ( gvRfTimingCounterH > (vrfHsmodeRandom + 130UL) ) && ( gvRfTimingCounterH < ( vrfHsmodeRandom + 156UL ) ) )	/* 130-180sec ���� 155-205sec */
								{
									wkRadio = 1U;
								}
									else if( gvRfTimingCounterH >= ( vrfHsmodeRandom + 156UL ) )
									{
										gvrfStsEventFlag.mHsmodeRandom = 0U;
									}
									else if( gvRfTimingCounterH <= (vrfHsmodeRandom + 130UL) )
									{
										if( ((vrfHsmodeRandom + 130UL) - gvRfTimingCounterH) > 180UL )
										{
											gvrfStsEventFlag.mHsmodeRandom = 0U;
										}
									}
								break;
						}

						/* 2021.7.20 Linux���։ߏ�Ƀ��K�[���m����Ȃ��悤�ɐ��� */
						if( vrfHsLgDct > 20U )
						{
							wkRadio = 0U;
						}
						if( gvRfTimingCounterL == 0U )
						{
							/* 2021.9.16 ���W��̔񌟒m���Ԓ����̂��� */
							if( gvrfStsEventFlag.mHsmodeLgDctDwn == 0U )
							{
								gvrfStsEventFlag.mHsmodeLgDctDwn = 1U;
#if 0
								if( vrfHsLgDct > 0U )
								{
									vrfHsLgDct--;
#else
								if( vrfHsLgDct > 4U )
								{
									vrfHsLgDct -= 5;
#endif
								}
							}
						}
						else
						{
							gvrfStsEventFlag.mHsmodeLgDctDwn = 0U;
						}

				}else
				{
					wkRadio = 1U;
					gvrfStsEventFlag.mHsmodeRandom = 0U;
				}
				
				if( wkRadio == 0U )
				{
					/* ���M�I�� */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
					if( gvrfStsEventFlag.mHsmodeRandom == 0U )
					{
						/* ���M�I��1��� */
						gvrfStsEventFlag.mHsmodeRandom = 1U;
						vrfHsmodeRandom = gvRfTimingCounterH + (rand() % 50U);			/* gvRfTimingCounterH��7200�Ń[���Ƀ��Z�b�g */
						if( vrfHsmodeRandom >= (7200UL - 130UL) )
						{
							vrfHsmodeRandom = 0UL;
							/* 2021.7.20 */
							vrfHsLgDct = 0U;
						}
					}
				}
				else
				{
					/* ���M�� */
					gvrfStsEventFlag.mHsmodeRandom = 0U;

					if( !gvrfStsEventFlag.mHsmodeInit )
					{
						/* ���������Ă��Ȃ� */
						vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconInit;
					}
					else
					{
						gvrfStsEventFlag.mHsmodeInit = 0U;
						vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconDataSet;
					}
				}
				gvModuleSts.mRf = ecRfModuleSts_Run;
				break;

			case SCAN_MODE:
				if( (vrfScanRssiUnder + vrfScanRssiOver) == 2000U )
				{
					/* �����œd�g�g�p���擾 vrfScanRssiOver/20 [%] */
					ApiModbus_SndRef40019( vrfScanRssiOver / 20 );
					/* 1000��X�L���������烊�Z�b�g */
					vrfScanRssiUnder = 0U;
					vrfScanRssiOver = 0U;
				}
				if( !gvrfStsEventFlag.mHsmodeInit )
				{
					/* ���������Ă��Ȃ� */
					vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconInit;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
					gvrfStsEventFlag.mHsmodeInit = 0U;
					SubSX1272Sleep();
					SubSX1272CarrierSenseFSK_DIO();
					if( SubRFDrv_CarrierSens_5ch( RF_HSMODE_CH[ gvInFlash.mParam.mrfHsCh ]) == NO_CARRIER )
					{
						vrfScanRssiUnder++;
					}
					else
					{
						vrfScanRssiOver++;
					}
					SubSX1272Sleep();
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				break;

			case OFF_MODE:
				SubSX1272Sleep();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
				gvModuleSts.mRf = ecRfModuleSts_Run;
				break;

			case TEST_MODE:
				if( vErrorLoopCounter > 200U )
				{
					break;
				}
				vErrorLoopCounter++;
				SubSX1272Stdby();
				SubSX1272Write( REG_LR_SYNCWORD, RF_RTMODE_CH_GRUPE_CNCT[ 10U ].mLoraSync );
				SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[ 10U ].mCh[ 0U ], rfFreqOffset );
				ApiRfDrv_Led( imON );
				SubRFDrv_LoRa_Tx_Setting(6U, 20U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
				vutRfDataBuffer.umData[0U] = vrfMySerialID.mUniqueID[0U];
				vutRfDataBuffer.umData[1U] = vrfMySerialID.mUniqueID[1U];
				vutRfDataBuffer.umData[2U] = vrfMySerialID.mUniqueID[2U];
				vutRfDataBuffer.umData[3U] = 0x21;
				vutRfDataBuffer.umData[4U] = 0x34;
				vutRfDataBuffer.umData[5U] = 0x65;
				SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], 6U);
//				SubRfDrv_PwChange( PW_H );
				SubSX1272Tx( 6U );
				SubIntervalTimer_Sleep( 100U * 375U, 20U );			/* 2000msec */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
				break;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_BeaconCadScan:
			SubSX1272Stdby();
			if( vCallChLoopLora >= RT_MODE_CH_LOOP_NUM ){ vCallChLoopLora = 0U; }
//			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[vrfLoraChGrupeCnt].mCh[vCallChLoopLora], rfFreqOffset );
			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mCh[ vCallChLoopLora ], rfFreqOffset );
			
			SubSX1272CadLoRa();
			
			if( gvRfIntFlg.mRadioDio4 )
			{
				/* CAD���m */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_BeaconRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else if( gvRfIntFlg.mRadioDio0 )
			{
				/* �񌟒m */
				SubSX1272Write( REG_LR_IRQFLAGS, 0xFF );
				vCallChLoopLora++;
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* �N���^�C���A�E�g */
				SubSX1272Stdby();
				SubSX1272Write( REG_LR_IRQFLAGS, 0xFF );
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_BeaconRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_1, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7 );
			
			SubIntervalTimer_Sleep( 70U * 375U, 1U );		/* 70msec */
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_BeaconRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_BeaconRxWait:
			/* ��M�����҂� */
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ��M�^�C���A�E�g */
				SubSX1272Stdby();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ��M���� */
				R_INTC6_Stop();
				SubSX1272Stdby();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_BeaconRxDataRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAuthSetData:
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxCompanyID = vrfMySerialID.mOpeCompanyID;								/* ���Ɖ��ID */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxAppID_GwUniqueID[0U] = vrfMySerialID.mAppID[0U];						/* �V�X�e��ID(�A�v��ID) */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxAppID_GwUniqueID[1U] = vrfMySerialID.mAppID[1U] & 0xF0;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxAppID_GwUniqueID[1U] += (vrfMySerialID.mUniqueID[0U] >> 4U) & 0x0F;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[0U] = (vrfMySerialID.mUniqueID[0U] << 4U) & 0xF0;			/* GW���j�[�NID */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[0U] += (vrfMySerialID.mUniqueID[1U] >> 4U) & 0x0F;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[1U] = (vrfMySerialID.mUniqueID[1U] << 4U) & 0xF0;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[1U] += (vrfMySerialID.mUniqueID[2U] >> 4U) & 0x0F;
			
			/* RTmode �ʐM�Ԋu */
#if 1
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x04;		/* �����l */
			for( wkRadio = 0U ; wkRadio < 9U ; wkRadio ++ )
			{
				if( cComIntList[ wkRadio ] == gvInFlash.mParam.mrfRTmodeCommInt )
				{
					vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = wkRadio;
					break;
				}
			}
#else
			switch ( gvInFlash.mParam.mrfRTmodeCommInt ) {
				case 0xFFFFU:
					/* �Ȃ� */
					vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x00;
					break;
				case 10U:
					/* 10sec */
					vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x01;
					break;
				case 20U:
					/* 20sec */
					vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x02;
					break;
				case 60U:
					/* 1min */
					vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x03;
					break;
				case 300U:
					/* 5min */
					vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x04;
					break;
				case 600U:
					/* 10min */
					vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x05;
					break;
				case 900U:
					/* 15min */
					vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x06;
					break;
				case 1200U:
					/* 20min */
					vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x07;
					break;
				case 1800U:
					/* 30min */
					vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x08;
					break;
				default:
					/* 5min */
					vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x04;
					break;
			}
#endif
			/* ���Z���̃C���N�������g�΍􂪕K�v */
			TMKAMK = 1U;							/* disable INTIT interrupt */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxSyncTimerL[0U] = (uint8_t)((gvRfTimingCounterL & 0xFF00) >> 8U);	/* �����^�C�~���O�J�E���^0.5msec */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxSyncTimerL[1U] = (uint8_t)(gvRfTimingCounterL);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxSyncTimerH[0U] = (uint8_t)((gvRfTimingCounterH & 0xFF00) >> 8U);	/* �����^�C�~���O�J�E���^1sec */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxSyncTimerH[1U] = (uint8_t)(gvRfTimingCounterH);
			TMKAMK = 0U;							/* enable INTIT interrupt */
#if 0
			/* LoggerList�ɋ󂫂����邩���������A�v���ʐM�̏��Ԃ����� */
			for( wkRadio = 0U; wkRadio < vrfLoggerSumNumber; wkRadio++ )
			{
				if( (vrfLoggerList[ wkRadio ][ 0U ] == 0x00) &&
					(vrfLoggerList[ wkRadio ][ 1U ] == 0x00) &&
					(vrfLoggerList[ wkRadio ][ 2U ] == 0x00) )
				{
					NOP();
					break;
				}
			}
			if( wkRadio == vrfLoggerSumNumber )
			{
				wkRadio++;
			}
			
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxTimeSlot = wkRadio;	
#endif
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxTimeSlot = 0U;														/* �����ł͒ʐM�^�C���X���b�g�ݒ薳��(0U�͖��ڑ�����)�폜�� */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxChGrupe[0U] = vrfLoraChGrupeMes;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxChGrupe[1U] = 0x00;													/* ch�O���[�v�d�l�ύX�ɂ��_�~�[�� */
			
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxOnline_Time[ 0U ] = (uint8_t)(gvElapseSec & 0x000000FF);			/* GW���� */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxOnline_Time[ 1U ] = (uint8_t)((gvElapseSec & 0x0000FF00) >> 8U);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxOnline_Time[ 2U ] = (uint8_t)((gvElapseSec & 0x00FF0000) >> 16U);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxOnline_Time[ 3U ] = (uint8_t)((gvElapseSec & 0x7F000000) >> 24U);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxOnline_Time[ 3U ] |= gvInFlash.mParam.mOnCertLmt << 7U;				/* �I�����C���F�،��� */
			
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComOffTime = gvInFlash.mParam.mDelayTimeDiscon;						/* RT�ʐM�ؒf���莞�� */
			
#if 0
			wkU32 = gvElapseSec / 2U;			/* ����\1sec(0.5sec�؎̂�) */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 0U ] = (uint8_t)((wkU32 - 60U) & 0x000000FF);			/* �Ƃ肠����1min�O */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 1U ] = (uint8_t)((wkU32 & 0x0000FF00) >> 8U);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 2U ] = (uint8_t)((wkU32 & 0x00FF0000) >> 16U);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 3U ] = (uint8_t)(wkU32 >> 24U);						/* RT���[�h�ł̌v���f�[�^�J�n���� */
#else
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 0U ] = gvInFlash.mParam.mRealTimeSndPt >> 8U;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 1U ] = gvInFlash.mParam.mRealTimeSndPt & 0x00FFU;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 2U ] = 0U;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 3U ] = 0U;
#endif
			
			for( wkRadio = 0U; wkRadio < 3U; wkRadio++ )
			{
				vutRfDataBuffer.umRT_Gw_ConnAuth.mTxReserve[wkRadio] = wkRadio;
			}
			
			SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[256U], RT_MODE_PAYLOAD_LENGTH_2 - 2U);
			
			/* RSSI�l���Ƃ� wait */
			wkCulc =  vrfCurrentRssi / 10U;
			if( wkCulc > 3U ){ wkCulc -= 3U; }
			else{ wkCulc = 0U; }
			if( wkCulc > 8U ){ wkCulc = 8U; }
			wkCulc += rand() % 3U;
			SubIntervalTimer_Sleep( wkCulc * 375U, 1U );		/* 1-10msec */
			
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCarrierSens;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_BeaconRxDataRead:
			SubRFDrv_LoRa_Rx_Read( &wkRadio, &vrfCurrentRssi);
			SubSX1272Sleep();
			
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS )
			{
				/* CRC OK */
				/* �ڑ��r�[�R�����ݒ�ύX�r�[�R�����̔��� */
				if( vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[1U] & 0x01 )
				{
					/****** �ݒ�ύX ********/
					/* ���M���Ԑ����`�F�b�N */
					switch (vRfDrv_CommIntMode)
					{
						case ecRfDrv_ShortIntMode:
							wkCulc = 900U - 580U;
							break;
						case ecRfDrv_MidIntMode:
							wkCulc = 1740U - 580U;
							break;
						case ecRfDrv_LongIntMode:
							wkCulc = 4700U - 580U;
							break;
						default:
							wkCulc = 990U - 580U;
							break;
					}
					
					if( vrfRtmodeCommTimeTotal > wkCulc )
					{
						/* �������ԃI�[�o�[ */
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					else
					{
//						vrfRssi = vrfCurrentRssi;
						/* �ڑ����K�[���X�g�ɂ��郍�K�[���ǂ�������(�ݒ�ύX���K�[���X�g�̔��肪����Εs�v) */
						for( wkRadio = 0U; wkRadio < vrfLoggerSumNumber; wkRadio++ )
						{
							if( (vrfLoggerList[wkRadio][0U] == vutRfDataBuffer.umRT_ChangeSetBeacon.mLoggerID1[0U]) &&
								(vrfLoggerList[wkRadio][1U] == vutRfDataBuffer.umRT_ChangeSetBeacon.mLoggerID1[1U]) &&
								(vrfLoggerList[wkRadio][2U] == vutRfDataBuffer.umRT_ChangeSetBeacon.mLoggerID2) )
							{
//								vrfLoggerRssi[wkRadio] = vrfCurrentRssi;
								break;
							}
						}
						if( vrfLoggerSumNumber == wkRadio )
						{
							/* Logger�Ȃ� */
							vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
							gvModuleSts.mRf = ecRfModuleSts_Run;
						}else
						{
							/* Logger���� �f�[�^�ޔ� */
							for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_1; wkRadio++ )
							{
								vutRfDataBuffer.umData[wkRadio + 256U] = vutRfDataBuffer.umData[wkRadio];
							}
							/* �ݒ�ύX���K�[���X�g�ɂ��郍�K�[���ǂ�������(������) */
							/* �ݒ�ύX����̏ꍇ */
							vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingData1Set;
							gvModuleSts.mRf = ecRfModuleSts_Run;
							/* �ݒ�ύX�Ȃ��̏ꍇ�͖����� */
						}
						/* ------------------------------------------------------------------------- */
					}
				}else
				{
					/******** �ڑ��r�[�R�� *********/
					/* �V�X�e��ID�̈�v�m�F���K�v */
					if( vrfMySerialID.mOpeCompanyID == vutRfDataBuffer.umRT_Logger_Beacon.mCompanyID )
					{
						/* �ȍ~���Ɖ��ID����v */
						/* �o�^���[�h�m�F */
						wkRadio2nd = FAIL;
						if( gvInFlash.mParam.mrfTorokuMode == LOGGER_TOROKU_ON )
						{
							if( ((vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[0U] >> 4U) & 0x0F) == APP_ID_LOGGER_INIT_UP
								&& ( ((vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[0U] << 4U) & 0xF0) + ((vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[1U] >> 4U) & 0x0F) )
								== APP_ID_LOGGER_INIT_LO )
							{
								/* ���K�[�o�^OK (AppID�����l) */
								wkRadio2nd = SUCCESS;
							}
						}
						if( (vrfMySerialID.mAppID[0U] == vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[0U]) &&
							(vrfMySerialID.mAppID[1U] == (vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[1U] & 0xF0)) &&
							(gvInFlash.mParam.mOnCertLmt == ((vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[1U] & 0x02) >> 1U)) )
						{
							/* ���K�[�o�^OK (AppID��v and ���[�J��/�W���Ǘ����[�h��v) */
							wkRadio2nd = SUCCESS;
						}
						/* ���K�[�󂯓��ꔻ�� */
						if( wkRadio2nd == SUCCESS )
						{
//							vrfRssi = vrfCurrentRssi;
							/* Online�F�؂���v(��������AppID�����l0x0FFF) */
							/* ���M���Ԑ����`�F�b�N */
							switch (vRfDrv_CommIntMode)
							{
								case ecRfDrv_ShortIntMode:
									wkCulc = 900U - 231U;
									break;
								case ecRfDrv_MidIntMode:
									wkCulc = 1740U - 231U;
									break;
								case ecRfDrv_LongIntMode:
									wkCulc = 4700U - 231U;
									break;
								default:
									wkCulc = 900U - 231U;
									break;
							}
							if( vrfRtmodeCommTimeTotal > wkCulc )
							{
								/* ���M���Ԑ��� */
								vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
								gvModuleSts.mRf = ecRfModuleSts_Run;
							}else
							{
								vLoopLoRa = 0U;
								vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthSetData;
								gvModuleSts.mRf = ecRfModuleSts_Run;
							}
						}
						else
						{
							/* ���K�[�o�^���� */
							vCallChLoopLora++;
							vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
							gvModuleSts.mRf = ecRfModuleSts_Run;
						}
					}
					else
					{
						/* ���Ǝ҂̈Ⴄ�V�X�e�� */
						vCallChLoopLora++;
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
				}
			}
			else
			{
				/*CRC NG */
				vCallChLoopLora++;
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAuthCarrierSens:
#if swRfCsTest == imEnable
			P7_bit.no5 = 1U;
#endif
			SubSX1272Sleep();
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
			
//			RTCMK = 1U;   /* disable INTRTC interrupt */

			/* Carrire Sence 125usec */
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
#if swRfCsTest == imEnable
				P7_bit.no5 = 0U;
#endif
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthTxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				if( SubRfDrv_CarrierSens_Rt_RetryDecision( 1U * 375U, CH_FIX, CS_RT_CONNECT_MODE ) == NEXT )
				{
					/* CarrierSens Retry */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCarrierSens;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
#if swRfCsTest == imEnable
					P7_bit.no5 = 0U;
#endif
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAuthTxStart:
			ApiRfDrv_Led( imON );
			SubRFDrv_LoRa_Tx_Setting(RT_MODE_PAYLOAD_LENGTH_2, 22U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[256U], RT_MODE_PAYLOAD_LENGTH_2);
			SubRfDrv_PwChange( PW_H );
			SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_2 );
			vrfRtmodeCommTimeTotal += RT_MODE_TX_TIME_2;					/* 156msec���M */
			
			/* Tx Done Wait */
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			SubIntervalTimer_Sleep( 100U * 375U, 4U );			/* 400msec */

//			RTCMK = 0U;   /* enable INTRTC interrupt */

			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthTxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAuthTxWait:
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ���M���Ƀ^�C���A�E�g(399.8msec) */
				/* �K���ȏ�̑��M���� */
				ApiRfDrv_Led( imOFF );
				SubSX1272Sleep();
				R_INTC6_Stop();
				vCallChLoopLora++;
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/*	�����I�� */
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ���M���� */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerInfoRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerInfoRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_3, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait �ݒ� */
			SubIntervalTimer_Sleep( 80U * 375U, 5U );		/*400msec*/
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerInfoRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerInfoRxWait:
			/* ��M�����҂� */
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* ��M���� */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubSX1272Stdby();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerInfoRxRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ��M�^�C���A�E�g */
				ApiRfDrv_Led( imOFF );
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/*	�����I�� */
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerInfoRxRead:
			SubRFDrv_LoRa_Rx_Read( &wkRadio, &vrfCurrentRssi);
			SubSX1272Sleep();
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS )
			{
				/* CRC OK */
				if( (vutRfDataBuffer.umRT_LoggerInfo.mGwID[0U] == vrfMySerialID.mUniqueID[0U]) &&
					(vutRfDataBuffer.umRT_LoggerInfo.mGwID[1U] == vrfMySerialID.mUniqueID[1U]) &&
					(vutRfDataBuffer.umRT_LoggerInfo.mGwID[2U] == vrfMySerialID.mUniqueID[2U]) )
				{
					vrfRssi = vrfCurrentRssi;
					/* ���@���̒ʐM */
					/* ��M�f�[�^�̑ޔ� */
					for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_3; wkRadio++ )
					{
						vutRfDataBuffer.umData[wkRadio + 256U] = vutRfDataBuffer.umData[wkRadio];
					}
					
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAckDataSet;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
					/* ���@���̒ʐM */
					ApiRfDrv_Led( imOFF );
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			else
			{
				/*CRC NG */
				ApiRfDrv_Led( imOFF );
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckDataSet:
			/* �ڑ��ς�Loggger�̃`�F�b�N */
			wkRadio2nd = 0xFF;
			for( wkRadio = 0U; wkRadio < vrfLoggerSumNumber; wkRadio++ )
			{
				if( (vrfLoggerList[wkRadio][0U] == vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[0U]) &&
					(vrfLoggerList[wkRadio][1U] == vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[1U]) &&
					(vrfLoggerList[wkRadio][2U] == vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[2U]) )
				{
					/* ���X�g�Ƀ��K�[���� */
					vrfLoggerComCnt[wkRadio] = 0U;
					vrfLoggerRssi[wkRadio] = vrfRssi;
					break;
				}
				if( (vrfLoggerList[wkRadio][0U] == 0U) && (vrfLoggerList[wkRadio][1U] == 0U) && (vrfLoggerList[wkRadio][2U] == 0U) )
				{
					/* ���X�g�ɋ󂫂��� */
					wkRadio2nd = wkRadio;
				}
			}
			
			vutRfDataBuffer.umRT_Gw_ConnAck.mLoggerID[0U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[0U];
			vutRfDataBuffer.umRT_Gw_ConnAck.mLoggerID[1U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[1U];
			vutRfDataBuffer.umRT_Gw_ConnAck.mLoggerID[2U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[2U];
			vutRfDataBuffer.umRT_Gw_ConnAck.mGwID_Sequence[0U] = vrfMySerialID.mUniqueID[0U];
			vutRfDataBuffer.umRT_Gw_ConnAck.mGwID_Sequence[1U] = vrfMySerialID.mUniqueID[1U];
			vutRfDataBuffer.umRT_Gw_ConnAck.mGwID_Sequence[2U] = vrfMySerialID.mUniqueID[2U];
			vutRfDataBuffer.umRT_Gw_ConnAck.mGwID_Sequence[2U] += 0x01;
			vutRfDataBuffer.umRT_Gw_ConnAck.mReserve[0U] = 0x12;
			vutRfDataBuffer.umRT_Gw_ConnAck.mReserve[1U] = 0x34;
			
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAckCarrireSens;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			
			if( vrfLoggerSumNumber == wkRadio )
			{
				/* Logger�V�K�o�^ */
				if( wkRadio2nd == 0xFF )
				{
					/* �o�^�䐔��max�łȂ����ǂ��� */
					if( ((vRfDrv_CommIntMode == ecRfDrv_LongIntMode) && (vrfLoggerSumNumber < RT_LONG_MODE_CONNECT_NUM)) ||
						 ((vRfDrv_CommIntMode == ecRfDrv_ShortIntMode) && (vrfLoggerSumNumber < RT_SHORT_MODE_CONNECT_NUM)) ||
						 ((vRfDrv_CommIntMode == ecRfDrv_MidIntMode) && (vrfLoggerSumNumber < RT_MID_MODE_CONNECT_NUM)) )
					{
						vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot = vrfLoggerSumNumber + 1U;		/* ���X�g�󂫂Ȃ� �V�K�ǉ� */
					}else
					{
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/* Logger MAX �o�^������ */
					}
					
				}else
				{
					vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot = wkRadio2nd + 1U;				/* �󂫃��X�g�̒ʐM�� */
				}
			}
			else
			{
				vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot = wkRadio + 1U;						/* Logger�o�^�ς� */
			}
			
			if( vRfDrvMainGwSts == ecRfDrvMainSts_RTmode_ConnectAckCarrireSens )
			{
				SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_4 - 2U );
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckCarrireSens:
#if swRfCsTest == imEnable
			P7_bit.no5 = 1U;
#endif
			SubSX1272Sleep();
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
			
			/* �L�����A�Z���X��RTC�����݋֎~ */
//			RTCMK = 1U;   /* disable INTRTC interrupt */
			
			/* Carrire Sence 125usec */
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
#if swRfCsTest == imEnable
				P7_bit.no5 = 0U;
#endif
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAckTxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				if( SubRfDrv_CarrierSens_Rt_RetryDecision( 1U * 375U, CH_FIX, CS_RT_CONNECT_MODE ) == NEXT )
				{
					/* CarrierSens Retry */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAckCarrireSens;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
#if swRfCsTest == imEnable
					P7_bit.no5 = 0U;
#endif
					ApiRfDrv_Led( imOFF );
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckTxStart:
			SubRFDrv_LoRa_Tx_Setting(RT_MODE_PAYLOAD_LENGTH_4, 10U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_4);
			SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_4 );
			vrfRtmodeCommTimeTotal += RT_MODE_TX_TIME_4;
			RTCMK = 0U;   /* enable INTRTC interrupt */
			
			/* Tx Done Wait */
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			SubIntervalTimer_Sleep( 100U * 375U, 4U );			/* 399.8msec */
			
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAckTxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckTxWait:
			ApiRfDrv_Led( imOFF );
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ���M���Ƀ^�C���A�E�g(399.8msec) */
				/* �K���ȏ�̑��M���� */
				SubSX1272Sleep();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ���M���� */
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				
				ApiModbus_SndFnc16Ref40201( (ST_RF_RT_LoggerInfo_t *)&vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[ 0U ], vrfRssi );
				
				if( vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot == vrfLoggerSumNumber + 1U )
				{
					/* Logger�o�^�̒ǉ� */
					vrfLoggerList[ vrfLoggerSumNumber][0U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[0U];
					vrfLoggerList[ vrfLoggerSumNumber][1U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[1U];
					vrfLoggerList[ vrfLoggerSumNumber][2U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[2U];
					vrfLoggerComCnt[vrfLoggerSumNumber] = 0x00;
					vrfLoggerRssi[vrfLoggerSumNumber] = vrfRssi;		/* �s�v? */
					vrfLoggerSumNumber++;
				}else
				{
					vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot--;
					vrfLoggerList[ vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot][0U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[0U];
					vrfLoggerList[ vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot][1U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[1U];
					vrfLoggerList[ vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot][2U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[2U];
					vrfLoggerComCnt[vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot] = 0x00;
					vrfLoggerRssi[vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot] = vrfRssi;			/* �s�v? */
				}
				
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		/* GW RTmode �v���l��M----------------------------------------------------------- */
		case ecRfDrvMainSts_RTmode_MeasureCadScan:
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			for( vCallChLoopLora = 0U; vCallChLoopLora < RT_MODE_CH_LOOP_NUM; vCallChLoopLora++ )
			{
				if( vCallChLoopLora != vrfRtMeasChMask )
				{
					SubSX1272Stdby();
//					if( vCallChLoopLora >= RT_MODE_CH_LOOP_NUM ){	vCallChLoopLora = 0U; }
					SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_MEASURE[vrfLoraChGrupeMes].mCh[vCallChLoopLora], rfFreqOffset );
					SubSX1272CadLoRa();
					
					if( gvRfIntFlg.mRadioDio4 )
					{
						/* CAD���m */
						M_SETBIT(P3,M_BIT0);
//						SubSX1272RxLoRa_DIO();
//						SubSX1272RxLoRa_Continu();
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureRxStart;
						gvModuleSts.mRf = ecRfModuleSts_Run;
						break;
					}
					else if( gvRfIntFlg.mRadioDio0 )
					{
						/* �񌟒m */
						SubSX1272Write( REG_LR_IRQFLAGS, 0xFF );
					}
					else
					{
						/* �N���^�C���A�E�g */
						SubSX1272Stdby();
						SubSX1272Write( REG_LR_IRQFLAGS, 0xFF );
						break;
					}
				}
				else
				{
					/* �قȂ�AppID��O���M */
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_5, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait �ݒ� */
			SubIntervalTimer_Sleep( 95U * 375U, 4U );		/* 380msec */
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureRxWait:
			/* ��M�����҂� */
			if( SubSX1272Read( REG_LR_RXNBBYTES ) > 0U )
			{
				/* ��M���� */
				R_INTC6_Stop();
				SubIntervalTimer_Stop();
				SubSX1272Stdby();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureRxRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ��M�^�C���A�E�g */
				M_CLRBIT(P3,M_BIT0);
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadSetting;			/*	�����I�� */
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureRxRead:
			SubRFDrv_LoRa_Rx_Read( &wkRadio, (uint8_t *)&wkCulc);
			SubSX1272Sleep();
			wkRadio = FAIL;
			if( (vutRfDataBuffer.umRT_AuditTx.mDataCntKind & 0x80) == 0x80 )
			{
				/* �������ʐM:�v���x�񗚗��̑��M�ߋ��f�[�^���F0�`6�� */
				if( (vutRfDataBuffer.umRT_AuditTx.mDataCntKind & 0x0F) <= 6U )
				{
					if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, RT_MODE_PAYLOAD_LENGTH_10_CRC + ((vutRfDataBuffer.umRT_AuditTx.mDataCntKind & 0x0F) * 11U)) == SUCCESS )
					{
						wkRadio = SUCCESS;
					}
				}
			}
			else
			{
				/* �������ʐM:�v���l�̑��M�ߋ��f�[�^���F0�`9�� */
				if( (vutRfDataBuffer.umRT_MeasureTx.mDataCntKind & 0x0F) <= 9U )
				{
					if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, RT_MODE_PAYLOAD_LENGTH_5_CRC + ((vutRfDataBuffer.umRT_MeasureTx.mDataCntKind & 0x0F) * 7U)) == SUCCESS )
					{
						wkRadio = SUCCESS;
					}
				}
			}
			
			/* �t�@�[���A�b�v(�f�[�^�]���A���̓t�@�[���A�b�v��):��M���s�Ƃ��A���K�[�̑��MPt���X�V�����Ȃ� */
			if( gvFirmupMode == imON )
			{
				wkRadio = FAIL;
			}
			
			if( wkRadio == SUCCESS )
			{
				/* CRC OK */
#if swRfMonitor == imEnable
				wkCulc = (RT_MODE_PAYLOAD_LENGTH_5 / 2U) + (RT_MODE_PAYLOAD_LENGTH_5 % 2U);
				for( wkRadio = 0; wkRadio < wkCulc; wkRadio++ )
				{
					vrfDebug[ wkRadio ] = ((uint16_t)(vutRfDataBuffer.umData[ wkRadio * 2U ]) << 8U) & 0xFF00;
					vrfDebug[ wkRadio ] += vutRfDataBuffer.umData[ wkRadio * 2U + 1U ];
				}
				vrfDebug[wkRadio] = vrfCurrentRssi;
				vrfDebug[wkRadio + 1U] = 0xCCCC;
				vrfDebug[wkRadio + 2U] = 0xCCCC;
				SubModbus_MakeSndData( 0x10, 40011U, wkCulc + 3U, &vrfDebug );				/*112byte�܂ő��M��(������56�܂�)*/
				gvutComBuff.umModInf.mRsrvFlg = imON;			/* ���M�\��t���OON */
				gvModuleSts.mMod = ecModModuleSts_Run;			/* Modbus���C�����[�`���J�n */

				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
				break;
#endif
//				P7_bit.no5 = 0U;
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
//				wkRadio = SUCCESS;
				if( (vutRfDataBuffer.umRT_MeasureTx.mGwID[0U] != vrfMySerialID.mUniqueID[0U])
					|| (vutRfDataBuffer.umRT_MeasureTx.mGwID[1U] != vrfMySerialID.mUniqueID[1U])
					|| ( (vutRfDataBuffer.umRT_MeasureTx.mGwID[2U] & 0xF0U) != (vrfMySerialID.mUniqueID[2U] & 0xF0U)) )
				{
					wkRadio = FAIL;
				}
#if 1
				if( vutRfDataBuffer.umRT_MeasureTx.mCompanyID != vrfMySerialID.mOpeCompanyID )
				{
					wkRadio = FAIL;
				}
				if( (vutRfDataBuffer.umRT_MeasureTx.mAppID != vrfMySerialID.mAppID[0U])
					|| (vutRfDataBuffer.umRT_MeasureTx.mGwID[2U] & 0x0FU) != ((vrfMySerialID.mAppID[1U] >> 4U) & 0x0F) )
				{
					wkRadio = FAIL;
				}
#endif
				if( wkRadio == SUCCESS )
				{
					ApiRfDrv_Led( imON );
					/* ���K�[�����X�g���ɂ��邩��check */
					for( wkRadio = 0U; wkRadio < vrfLoggerSumNumber; wkRadio++ )
					{
						if( (vrfLoggerList[wkRadio][0U] == vutRfDataBuffer.umRT_MeasureTx.mLoggerID[0U]) &&
							(vrfLoggerList[wkRadio][1U] == vutRfDataBuffer.umRT_MeasureTx.mLoggerID[1U]) &&
							(vrfLoggerList[wkRadio][2U] == vutRfDataBuffer.umRT_MeasureTx.mLoggerID[2U]) )
						{
							vrfRssi = vrfCurrentRssi;						/* SubSX1272CadLoRa()�Ŏ擾����RSSI */
							/* ���X�g�Ƀ��K�[���� */
							/* ��M�f�[�^�̑ޔ� */
							if( (vutRfDataBuffer.umRT_AuditTx.mDataCntKind & 0x80) == 0x80 )
							{
								wkCulc = RT_MODE_PAYLOAD_LENGTH_10;
							}
							else
							{
								wkCulc = RT_MODE_PAYLOAD_LENGTH_5;
							}
							for( wkRadio = 0U; wkRadio < wkCulc; wkRadio++ )
							{
								vutRfDataBuffer.umData[wkRadio + 256U] = vutRfDataBuffer.umData[wkRadio];
							}
							/* ���M�d�͕ύX */
							if( (vrfRssi > PW_TH_L_TO_H) )
							{
								gvrfStsEventFlag.mRtmodePwmode = PW_H;
							}
							else if( vrfRssi < PW_TH_H_TO_L )
							{
								gvrfStsEventFlag.mRtmodePwmode = PW_L;
							}
							else
							{
								/* ���̃P�[�X */
								gvrfStsEventFlag.mRtmodePwmode = PW_L;
							}
							vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureAckDataSet;
							gvModuleSts.mRf = ecRfModuleSts_Run;
							break;
						}
					}
				}
				else
				{
					/* ���@���̒ʐM */
					vrfRtMeasChMask = vCallChLoopLora;			/* Ch���}�X�N */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadSetting;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			else
			{
				/*CRC NG */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureAckDataSet:
			/*
			for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_6 - 2U; wkRadio++ )
			{
				vutRfDataBuffer.umData[wkRadio] = wkRadio;
			}
			*/
			vutRfDataBuffer.umRT_MeasureAck.mCompanyID = vrfMySerialID.mOpeCompanyID;
			vutRfDataBuffer.umRT_MeasureAck.mLoggerID[0U] = vutRfDataBuffer.umRT_MeasureTx.mBuffLoggerID[0U];
			vutRfDataBuffer.umRT_MeasureAck.mLoggerID[1U] = vutRfDataBuffer.umRT_MeasureTx.mBuffLoggerID[1U];
			vutRfDataBuffer.umRT_MeasureAck.mLoggerID[2U] = vutRfDataBuffer.umRT_MeasureTx.mBuffLoggerID[2U];
//			vutRfDataBuffer.umRT_MeasureAck.mSequenceSettingDisConnect = 0x40;		/* sequence No2 */
			
			/* ---������LoggerID���Ƃ̏����𔻒f����---------------------------------  */
			/* �ʐM�����̃J�E���g���� */
			for( wkRadio = 0U; wkRadio < vrfLoggerSumNumber; wkRadio++ )
			{
				if( (vrfLoggerList[wkRadio][0U] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[0U]) &&
					(vrfLoggerList[wkRadio][1U] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[1U]) &&
					(vrfLoggerList[wkRadio][2U] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[2U]) )
				{
					vrfLoggerComCnt[wkRadio] = 0U;			/* �o�^�ς�Logger�ʐM���� �N���A���� */
					vrfLoggerRssi[wkRadio] = vrfRssi;
					break;
				}
			}
			if( wkRadio > 0U && wkRadio == vrfLoggerSumNumber )
			{
				wkRadio--;
			}
			
			/* ���K�[�Z���ݒ�l�����݊m�F */
			vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[ 0U ] = 0x00;
#if (swKouteiCalibration == imEnable)
			if( (gvCalVal.mLoggerId[ 0U ] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[ 0U ]) &&
				(gvCalVal.mLoggerId[ 1U ] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[ 1U ]) &&
				(gvCalVal.mLoggerId[ 2U ] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[ 2U ]) )
			{
				/* ���K�[�Z���ݒ肠�� */
				vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[ 0U ] |= 0x80;
			}
#endif
			/* Test�ؒf�t���O */
//			vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[0U] += 0x40;
			/* Test �ݒ�ύX�t���OON */
//			vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[0U] += 0x80;
			
			/* LoggerID�������A�������ɒʐM���s�J�E���g���N���A���� */
			
/******************************* ACK���M��Ɉړ��ł���΃x�X�g *******************************************/
			if( (vutRfDataBuffer.umRT_AuditTx.mBuffDataCntKind & 0x80) == 0x80 )
			{
				/* �x�񗚗�����M */
				wkErrCode = ApiModbus_SndFnc16Ref40401( (ST_RF_RT_Audit_Tx_t *)&vutRfDataBuffer.umRT_AuditTx.mBuffCompanyID, vrfLoggerRssi[ wkRadio ] );
			}
			else
			{
				/* �v���l����M */
				wkErrCode = ApiModbus_SndFnc16Ref40101( (ST_RF_RT_Measure_Tx_t *)&vutRfDataBuffer.umRT_MeasureTx.mBuffCompanyID, vrfLoggerRssi[ wkRadio ] );
			}
			if( wkErrCode != imMod_ErrCodeNon )
			{
				ApiRfDrv_Led( imOFF );
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
				break;
			}
/******************************************************************************************************/
			/* �^�C�~���O�J�E���^H 8-13bit */
			vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[0U] = (uint8_t)(0x00F8 & (gvRfTimingCounterH >> 5U));
			vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[0U] += (uint8_t)(0x0007 & (gvRfTimingCounterL >> 8U));
			vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[1U] = (uint8_t)(gvRfTimingCounterL);
			vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[2U] = (uint8_t)(gvRfTimingCounterH);
			
			/* �����f�[�^(����14bit, 1cnt/0.5sec) */
			vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[ 0U ] |= (uint8_t)((gvElapseSec & 0x00003F00) >> 8U);
			vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[ 1U ] = (uint8_t)(gvElapseSec & 0x000000FF);
			
			SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[ 0U ], RT_MODE_PAYLOAD_LENGTH_6 - 2U );
			
		case ecRfDrvMainSts_RTmode_MeasureAckCarrireSence:
#if swRfCsTest == imEnable
			P7_bit.no5 = 1U;
#endif
			SubSX1272Sleep();
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
			
//			RTCMK = 1U;   /* disable INTRTC interrupt */
			
			/* Carrire Sence 125usec */
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureAckTxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				if( SubRfDrv_CarrierSens_Rt_RetryDecision( 1U * 375U, CH_FIX, CS_RT_MEASURE_MODE ) == NEXT )
				{
					/* CarrierSens Retry */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureAckCarrireSence;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
#if swRfCsTest == imEnable
					P7_bit.no5 = 0U;
#endif
					ApiRfDrv_Led( imOFF );
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadSetting;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureAckTxStart:
			SubRFDrv_LoRa_Tx_Setting( RT_MODE_PAYLOAD_LENGTH_6, 14U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_6);
			SubRfDrv_PwChange( gvrfStsEventFlag.mRtmodePwmode );
//			P7_bit.no5 = 1U;
			SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_6 );
			vrfRtmodeCommTimeTotal += RT_MODE_TX_TIME_6;
			
			/* Tx Done Wait */
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			SubIntervalTimer_Sleep( 100U * 375U, 4U );			/* 399.8msec */
			
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureAckTxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureAckTxWait:
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ���M���Ƀ^�C���A�E�g(399.8msec) */
				/* �K���ȏ�̑��M���� */
				ApiRfDrv_Led( imOFF );
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				gvrfStsEventFlag.mRtmodePwmode = PW_H;
				SubRfDrv_PwChange( gvrfStsEventFlag.mRtmodePwmode );
				ApiRfDrv_Led( imOFF );

				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ���M���� */
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				gvrfStsEventFlag.mRtmodePwmode = PW_H;
				SubRfDrv_PwChange( gvrfStsEventFlag.mRtmodePwmode );
				ApiRfDrv_Led( imOFF );
				
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		/* GW RTmode �ݒ�ύX */
		case ecRfDrvMainSts_RTmode_LoggerSettingData1Set:
			/* �_�~�[�f�[�^�������� */
//			for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_7_1 - 2U; wkRadio++ )
//			{
//				vutRfDataBuffer.umData[wkRadio] = wkRadio;
//			}
			vutRfDataBuffer.umRT_ChangeSetPrm1.mLoggerID[0U] = vutRfDataBuffer.umRT_ChangeSetBeacon.mBuffLoggerID1[0U];
			vutRfDataBuffer.umRT_ChangeSetPrm1.mLoggerID[1U] = vutRfDataBuffer.umRT_ChangeSetBeacon.mBuffLoggerID1[1U];
			vutRfDataBuffer.umRT_ChangeSetPrm1.mLoggerID[2U] = vutRfDataBuffer.umRT_ChangeSetBeacon.mBuffLoggerID2;
			vutRfDataBuffer.umRT_ChangeSetPrm1.mGwID[0U] = vrfMySerialID.mUniqueID[0U];
			vutRfDataBuffer.umRT_ChangeSetPrm1.mGwID[1U] = vrfMySerialID.mUniqueID[1U];
			vutRfDataBuffer.umRT_ChangeSetPrm1.mGwID[2U] = vrfMySerialID.mUniqueID[2U];
			
#if (swKouteiCalibration == imEnable)
			if( (gvCalVal.mLoggerId[ 0U ] == vutRfDataBuffer.umRT_ChangeSetPrm1.mLoggerID[ 0U ]) &&
				(gvCalVal.mLoggerId[ 1U ] == vutRfDataBuffer.umRT_ChangeSetPrm1.mLoggerID[ 1U ]) &&
				(gvCalVal.mLoggerId[ 2U ] == vutRfDataBuffer.umRT_ChangeSetPrm1.mLoggerID[ 2U ]) )
			{
				gvCalVal.mAck = 2U;				/* �Z���ݒ芮���҂� */
			}
#endif
			
			SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_7_1 - 2U );
			
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence1;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence1:
#if swRfCsTest == imEnable
			P7_bit.no5 = 1U;
#endif
			SubSX1272Sleep();
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
			
//			RTCMK = 1U;   /* disable INTRTC interrupt */
			
			/* Carrire Sence 125usec */
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
#if swRfCsTest == imEnable
				P7_bit.no5 = 0U;
#endif
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingTxStart1;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				if( SubRfDrv_CarrierSens_Rt_RetryDecision( 0U, CH_FIX, CS_RT_CONNECT_MODE ) == NEXT )
				{
					/* CarrierSens Retry */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence1;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
#if swRfCsTest == imEnable
					P7_bit.no5 = 0U;
#endif
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingTxStart1:
			ApiRfDrv_Led( imON );
			SubRFDrv_LoRa_Tx_Setting( RT_MODE_PAYLOAD_LENGTH_7_1, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_7_1);
			SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_7_1 );
			vrfRtmodeCommTimeTotal += RT_MODE_TX_TIME_7_1;
			
			/* Tx Done Wait */
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			SubIntervalTimer_Sleep( 100U * 375U, 2U );			/* 200msec */
			/*���M���Ƀf�[�^�Z�b�g*/
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingData2Set;
//			gvModuleSts.mRf = ecRfModuleSts_Run;
//			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingData2Set:
			/* �_�~�[�f�[�^�������� */
//			for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_7_2 - 2U; wkRadio++ )
//			{
//				vutRfDataBuffer.umData[wkRadio] = wkRadio;
//			}
			SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_7_2 - 2U );
			
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingTxWait1;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingTxWait1:
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ���M���Ƀ^�C���A�E�g(200msec) */
				/* �K���ȏ�̑��M���� */
				ApiRfDrv_Led( imOFF );
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ���M���� */
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				SubIntervalTimer_Sleep( 2U * 375U, 1U );			/* �x�~����2msec */
				
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence2;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence2:
#if swRfCsTest == imEnable
			P7_bit.no5 = 1U;
#endif
			SubSX1272Sleep();
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
			
//			RTCMK = 1U;   /* disable INTRTC interrupt */
			
			/* Carrire Sence 125usec */
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
#if swRfCsTest == imEnable
				P7_bit.no5 = 0U;
#endif
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingTxStart2;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				if( SubRfDrv_CarrierSens_Rt_RetryDecision( 0U, CH_FIX, CS_RT_CONNECT_MODE ) == NEXT )
				{
					/* CarrierSens Retry */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence2;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
#if swRfCsTest == imEnable
					P7_bit.no5 = 0U;
#endif
					ApiRfDrv_Led( imOFF );
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingTxStart2:
			SubRFDrv_LoRa_Tx_Setting( RT_MODE_PAYLOAD_LENGTH_7_2, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_7_2);
			SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_7_2 );
			vrfRtmodeCommTimeTotal += RT_MODE_TX_TIME_7_2;
			
			/* Tx Done Wait */
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			SubIntervalTimer_Sleep( 100U * 375U, 2U );			/* 200msec */
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingData3Set;
			//gvModuleSts.mRf = ecRfModuleSts_Sleep;
			//break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingData3Set:
			/* �_�~�[�f�[�^�������� */
//			for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_7_3 - 2U; wkRadio++ )
//			{
//				vutRfDataBuffer.umData[wkRadio] = wkRadio;
//			}
			
#if (swKouteiCalibration == imEnable)
			/* ���K�[�Z���ݒ�l�̗L���m�F */
			/* �ݒ芮���҂����� */
			if( gvCalVal.mAck == 2U )
			{
				/* �Z���l�𑗐M�p�P�b�g�ɋL�� */
				vutRfDataBuffer.umRT_ChangeSetPrm3.mCalZeroSpan[ 0U ] = gvCalVal.mSpan_1ch;
				vutRfDataBuffer.umRT_ChangeSetPrm3.mCalZeroSpan[ 1U ] = gvCalVal.mSpan_2ch;
				vutRfDataBuffer.umRT_ChangeSetPrm3.mCalOffset[ 0U ] = gvCalVal.mOffset_1ch;
				vutRfDataBuffer.umRT_ChangeSetPrm3.mCalOffset[ 1U ] = gvCalVal.mOffset_2ch;
			}
#endif
			
			SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_7_3 - 2U );
			
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingTxWait2;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingTxWait2:
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ���M���Ƀ^�C���A�E�g(200msec) */
				/* �K���ȏ�̑��M���� */
				ApiRfDrv_Led( imOFF );
				SubSX1272Sleep();
				R_INTC6_Stop();
				vCallChLoopLora++;
//				vrfTxLimitTime = 5U;				/* ���M���Ԑ��� 4sec-5sec */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/*	�����I�� */
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ���M���� */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubIntervalTimer_Sleep( 2U * 375U, 1U );			/* �x�~����2msec */
//				vrfTxLimitTime = 5U;				/* ���M���Ԑ��� 4sec-5sec */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence3;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence3:
#if swRfCsTest == imEnable
			P7_bit.no5 = 1U;
#endif
			SubSX1272Sleep();
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272CarrierSenseFSK_DIO();
			
//			RTCMK = 1U;   /* disable INTRTC interrupt */
			
			/* Carrire Sence 125usec */
			if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
			{
#if swRfCsTest == imEnable
				P7_bit.no5 = 0U;
#endif
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingTxStart3;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				if( SubRfDrv_CarrierSens_Rt_RetryDecision( 0U, CH_FIX, CS_RT_CONNECT_MODE ) == NEXT )
				{
					/* CarrierSens Retry */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence3;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
#if swRfCsTest == imEnable
					P7_bit.no5 = 0U;
#endif
					ApiRfDrv_Led( imOFF );
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingTxStart3:
			SubRFDrv_LoRa_Tx_Setting( RT_MODE_PAYLOAD_LENGTH_7_3, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_7_3);
			SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_7_3 );
			vrfRtmodeCommTimeTotal += RT_MODE_TX_TIME_7_3;
			
			/* Tx Done Wait */
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			SubIntervalTimer_Sleep( 100U * 375U, 2U );			/* 200msec */
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingTxWait3;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;

		case ecRfDrvMainSts_RTmode_LoggerSettingTxWait3:
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* ���M���Ƀ^�C���A�E�g(200msec) */
				/* �K���ȏ�̑��M���� */
				ApiRfDrv_Led( imOFF );
				SubSX1272Sleep();
				R_INTC6_Stop();
				vCallChLoopLora++;
//				vrfTxLimitTime = 5U;				/* ���M���Ԑ��� 4sec-5sec */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/*	�����I�� */
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ���M���� */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
//				vrfTxLimitTime = 5U;				/* ���M���Ԑ��� 4sec-5sec */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingAckRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingAckRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_8, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait �ݒ� */
			SubIntervalTimer_Sleep( 100U * 375U, 5U );		/*100msec*/
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingAckRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingAckRxWait:
			/* ��M�����҂� */
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* ��M���� */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubSX1272Stdby();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingAckRxRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ��M�^�C���A�E�g */
				ApiRfDrv_Led( imOFF );
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/*	�����I�� */
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingAckRxRead:
			SubRFDrv_LoRa_Rx_Read( &wkRadio, &vrfCurrentRssi);
			SubSX1272Sleep();
			ApiRfDrv_Led( imOFF );
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS )
			{
				/* CRC OK */
				if( (vutRfDataBuffer.umRT_ChangeSetAck.mGwID[0U] == vrfMySerialID.mUniqueID[0U]) &&
					(vutRfDataBuffer.umRT_ChangeSetAck.mGwID[1U] == vrfMySerialID.mUniqueID[1U]) &&
					(vutRfDataBuffer.umRT_ChangeSetAck.mGwID[2U] == vrfMySerialID.mUniqueID[2U]) )
				{
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					/* ���@���̒ʐM */
					if( vutRfDataBuffer.umRT_ChangeSetAck.mSequenceNo == 0x03 )
					{
						/* �ݒ�ύXACK�ł��� */
						if( (vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[0U] == vutRfDataBuffer.umRT_ChangeSetBeacon.mBuffLoggerID1[0U] )
						  && (vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[1U] == vutRfDataBuffer.umRT_ChangeSetBeacon.mBuffLoggerID1[1U] )
							&& (vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[2U] == vutRfDataBuffer.umRT_ChangeSetBeacon.mBuffLoggerID2 ))
						{
							vrfRssi = vrfCurrentRssi;
							for( wkRadio = 0U; wkRadio < vrfLoggerSumNumber; wkRadio++ )
							{
								if( (vrfLoggerList[wkRadio][0U] == vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[0U]) &&
									(vrfLoggerList[wkRadio][1U] == vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[1U]) &&
									(vrfLoggerList[wkRadio][2U] == vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[2U]) )
								{
									vrfLoggerRssi[wkRadio] = vrfRssi;
									break;
								}
							}
							/* �ݒ�ύX�������K�[�̃`�F�b�NOK */
#if (swKouteiCalibration == imEnable)
							/* �ݒ�ύXOK �\�񃊃X�g���烍�K�[���폜 */
							if( (gvCalVal.mLoggerId[ 0U ] == vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[ 0U ]) &&
								(gvCalVal.mLoggerId[ 1U ] == vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[ 1U ]) &&
								(gvCalVal.mLoggerId[ 2U ] == vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[ 2U ]) )
							{
								/* �ݒ芮���҂����� */
								if( gvCalVal.mAck == 2U )
								{
									gvCalVal.mAck = 1U;				/* �Z���ݒ芮�� */
									ApiModbus_SndFnc16Ref45201( &gvCalVal.mLoggerId[ 0U ] );
									gvCalVal.mLoggerId[ 0U ] = 0xFF;
									gvCalVal.mLoggerId[ 1U ] = 0xFF;
									gvCalVal.mLoggerId[ 2U ] = 0xFF;
									gvCalVal.mSpan_1ch = 0xFF;
									gvCalVal.mOffset_1ch = 0xFF;
									gvCalVal.mSpan_2ch = 0xFF;
									gvCalVal.mOffset_2ch = 0xFF;
									gvCalVal.mAck = 0xFF;
								}
							}
#endif
						}
						else
						{
							/* �ݒ�ύX�������K�[���Ⴄ?? */
						}
					}
					else
					{
						/* �V�[�P���XErr */
					}
				}
				else
				{
					/* ���@���̒ʐM */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
			}
			else
			{
				/*CRC NG */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		default:
			break;
	}
#endif
}


/* ��������֐� */
/*
********************************************************************************
* �C���^�[�o���^�C�}�ɂ��sleep����
* [���e]
*	  �������C���^�[�o���^�C�}�̊��荞�݃J�E���^�ɃZ�b�g��sleep�A�J�E���g��N��
*	  ���v���ł̊��荞�݂ł̋N��������(���荞�݃}�X�N�Ȃ�)
*	  20�񃋁[�v���ɃE�H�b�`�h�b�O�^�C�}�����Z�b�g
*	�J�E���g�\�[�X375kHz
* [����]
*	  wkTimerCount	�C���^�[�o���^�C�}���荞�݃J�E���^
*	  wkLoopCount  �J��Ԃ���
********************************************************************************
*/
void SubIntervalTimer_Sleep( uint16_t wkTimerCount, uint16_t wkLoopCount )
{
#if 0
	R_IT_Create_Custom( wkTimerCount );
	vSleepLoop = wkLoopCount;
#else
//	RTCMK = 1U;		/* disable INTRTC interrupt */
	if( wkLoopCount > 0U )
	{
		gvRfIntFlg.mRadioTimer = 0U;
		R_TAU0_Channel6_Custom( wkTimerCount );
		vSleepLoop = wkLoopCount;
		gvRfIntFlg.mRadioStsMsk = 1U;		/* IT�ɂ��ecRfModuleSts_Run���}�X�N */
	}
#endif
}


void SubIntervalTimer_Stop( void )
{
//	RTCMK = 0U;   /* enable INTRTC interrupt */
	R_TAU0_Channel6_Stop();
	gvRfIntFlg.mRadioTimer = 0U;
	gvRfIntFlg.mRadioStsMsk = 0U;		/* IT�ɂ��ecRfModuleSts_Run�}�X�N���� */
}


/*
********************************************************************************
* SX1272 POR����
*
*[���e]
*	  SX1272��POR�������s��
********************************************************************************
*/
void SubSX1272POR( void )
{
	RADIO_NSS = imHigh;
	/* Reset pin ���̓��[�h */
	RADIO_RESET_IO = imIoIn;
	/* RFIO4,RFIO2,RFIO6,RFIO1 ���̓��[�h */
	RADIO_DIO_2_IO = imIoIn;
	RADIO_DIO_1_IO = imIoIn;
	RADIO_DIO_4_IO = imIoIn;
	RADIO_DIO_0_IO = imIoIn;
}

/*
 *******************************************************************************
 *	SX1272���Z�b�g����
 *
 *	[���e]
 *		SX1272�̃��Z�b�g���s���B
 *******************************************************************************
 */
void ApiSX1272Reset( uint8_t arRstExe )
{
	if( arRstExe == imON )
	{
		/* Set RESET pin to 1 */
		RADIO_RESET = imHigh;
		
		/* RESET Pin Output */
		RADIO_RESET_IO = imIoOut;
		
		/* NSS pin 1 �� H */
		
//		RADIO_NSS = imHigh;
	}
	else
	{
		/* Configure RESET as input ����HiZ */
		/* RESET Pin Input */
		RADIO_RESET_IO = imIoIn;
//		RADIO_RESET = imLow;
	}
}


/*
 *******************************************************************************
 *	SX1272 FSK���[�h������
 *
 *	[���e]
 *		SX1272��FSK���[�h�Ń��W�X�^���������s��
 *******************************************************************************
*/
void SubSX1272InitReg(void)
{
	uint8_t wkRadio;
//	uint8_t wkTest;
	
	for( wkRadio = 0; wkRadio < sizeof( RadioRegsInit ) / sizeof( RadioRegisters_t ); wkRadio++ )
	{
		if( RadioRegsInit[wkRadio].Modem == MODEM_FSK )
		{
			SubSX1272Write( REG_OPMODE, RF_OPMODE_LONGRANGEMODE_OFF |
							RF_OPMODE_MODULATIONTYPE_FSK |
							RF_OPMODE_MODULATIONSHAPING_11 |
							RF_OPMODE_SLEEP );
		}
		else
		{
			SubSX1272Write( REG_LR_OPMODE ,
				RF_OPMODE_LONGRANGEMODE_ON |
				RFLR_OPMODE_ACCESSSHAREDREG_DISABLE |
				RFLR_OPMODE_SLEEP );
		}
		SubSX1272Write( RadioRegsInit[wkRadio].Addr, RadioRegsInit[wkRadio].Value );
//		wkTest = SubSX1272Read( RadioRegsInit[wkRadio].Addr );
	}
}

#if (swLoggerBord == imEnable)
/*
*******************************************************************************
*  ���K�[�����@RTmode�ڑ��f���̍Đڑ��r�[�R���̃^�C�~���O�����֐�
*  �y�T�v�z
*	GW�̃��K�[�ڑ��҂��^�C�~���O�ɐؒf����̃r�[�R��������悤�ɒ���
*******************************************************************************
*/
void SubRfDrv_RtConctTiAdd( void )
{
	srand( vrfCurrentRssi );
	gvRfTimingCounterH = rand() % 34U + 1U;
	gvRfTimingCounterL = rand() % 7U;
	
#if 0
	switch (vRfDrv_CommIntMode)
	{
	case ecRfDrv_ShortIntMode:
		// �␳�Ȃ���OK
		break;
	case ecRfDrv_MidIntMode:
		gvRfTimingCounterH += ( 10U - ((vrfRTmodeTimeSlot * 3) / 2) + (rand() % 8U) );
		gvRfTimingCounterL = rand() % 7U;
		break;
	case ecRfDrv_LongIntMode:
		gvRfTimingCounterH += ( 40U - ((vrfRTmodeTimeSlot * 3) / 2) + (rand() % 18U) );
		gvRfTimingCounterL = rand() % 7U;
		break;
	
	default:
		break;
	}
#endif
}
#endif

#if (swLoggerBord == imDisable)
/*
*******************************************************************************
*  GW�����@RTmode�ڑ���Ԃ̔��f�����ƃ��K�[���X�g�X�V
*  �y�T�v�z
*	�v���l�ʐM�s�ʂ̃J�E���g�A�b�v�����ƘA�����s�񐔂���ؒf���f�C���K�[���X�g�̍X�V
*	20�𒴂���ꍇ�́A20�����Ŋi�[
*******************************************************************************
*/
void SubRfDrv_RtConctCut( void )
{
	uint8_t wkCulc;
	uint8_t wkSize;
	uint8_t wkDelList[ 20U ][ 3U ];
	
	wkSize = 0U;
	wkCulc = vrfLoggerSumNumber;
	if( wkCulc > 0 && wkCulc <= 60U )
	{
		do
		{
			wkCulc--;
//			vrfLoggerComCnt[wkCulc]++;							/* �֐����v���ʐM��Ɉړ����A�J�E���g������Idele�擪�Ɏc�� */
			/* ���K�[�ؒf���� */
//			if( vrfLoggerComCnt[wkCulc] > vrfLoggerDisconnectCnt )
			if( vrfLoggerComCnt[wkCulc] > gvInFlash.mParam.mDelayTimeDiscon )
			{
				if( (vrfLoggerList[ wkCulc ][ 0U ] != 0x00) ||
					(vrfLoggerList[ wkCulc ][ 1U ] != 0x00) ||
					(vrfLoggerList[ wkCulc ][ 2U ] != 0x00) )
				{
					wkDelList[ wkSize ][ 0U ] = vrfLoggerList[ wkCulc ][ 0U ];
					wkDelList[ wkSize ][ 1U ] = vrfLoggerList[ wkCulc ][ 1U ];
					wkDelList[ wkSize ][ 2U ] = vrfLoggerList[ wkCulc ][ 2U ];
					wkSize ++;
					
					/* �ؒf���ꂽ���K�[���X�g��Modbus���M���X�g�ɒǉ� */
					if( wkSize >= 20U )
					{
						ApiModbus_SndFnc16Ref40301( 0U, wkSize, wkDelList );
						wkSize = 0U;
					}
					
#if (swKouteiCalibration == imEnable)
					if( (gvCalVal.mLoggerId[ 0U ] == vrfLoggerList[ wkCulc ][ 0U ]) &&
						(gvCalVal.mLoggerId[ 1U ] == vrfLoggerList[ wkCulc ][ 1U ]) &&
						(gvCalVal.mLoggerId[ 2U ] == vrfLoggerList[ wkCulc ][ 2U ]) )
					{
						gvCalVal.mAck = 3U;				/* �Z�����s */
						ApiModbus_SndFnc16Ref45201( &gvCalVal.mLoggerId[ 0U ] );
						gvCalVal.mLoggerId[ 0U ] = 0xFF;
						gvCalVal.mLoggerId[ 1U ] = 0xFF;
						gvCalVal.mLoggerId[ 2U ] = 0xFF;
						gvCalVal.mSpan_1ch = 0xFF;
						gvCalVal.mOffset_1ch = 0xFF;
						gvCalVal.mSpan_2ch = 0xFF;
						gvCalVal.mOffset_2ch = 0xFF;
						gvCalVal.mAck = 0xFF;
					}
#endif
				}
				vrfLoggerList[wkCulc][0U] = 0x00;
				vrfLoggerList[wkCulc][1U] = 0x00;
				vrfLoggerList[wkCulc][2U] = 0x00;
				if( (vrfLoggerSumNumber - 1U) == wkCulc )
				{
					/* �ڑ��䐔�������̃��K�[�̏ꍇ�͑���-1 */
					vrfLoggerSumNumber--;
					vrfLoggerComCnt[wkCulc] = 0x00;
				}
			}
		}while(wkCulc > 0U);
		
		/* �ؒf���ꂽ���K�[���X�g��Modbus���M���X�g�ɒǉ� */
		if( wkSize != 0U )
		{
			ApiModbus_SndFnc16Ref40301( 0U, wkSize, wkDelList );
		}
	}
}
#endif

/*
*******************************************************************************
*	FSK���[�h�ł̃L�����A�Z���X
*
*	[���e]
*		FSK���[�h/�`���l���ݒ�ς�/DIO04_11,MAP_RSSI/SLEEP or STANDBY���O��
*�@�@�ݒ莞�ԃL�����A�Z���X���s���B
*		���荞�݂̓}�X�N���邱��(�}�X�N��Ԃ̂܂ܔ�����)
*		Sleep��ԂŔ�����
* [����]
*	�@�C���^�[�o���^�C�}�̃J�E���g�l(�L�����A�Z���X����)
* [Return]
*	NO_CARRIER�F�L�����A�Ȃ�
*	SENS_CARRIER : �L�����A����
*******************************************************************************
*/
uint8_t SubRFDrv_CarrierSens_FSK( uint16_t arSensTime )
{
	uint8_t wkRet;
	uint8_t wkLoop;
	
	gvRfIntFlg.mRadioDio4 = 0U;
	// RSSI set time 16usec / RX wake up 63usec / PLL 60usec / Standby 250usec
	// WakeUp���v 381usec + 125usec => 16U (519usec)
	// ����140usec(RSSI set time 16usec / RX wake up 63usec / PLL 60usec)
	R_INTC1_Start();
	SubSX1272RxFSK_On();
	vrfCurrentRssi = SubRfDrv_GetRssi();			/* �̂�RSSI�l */
	wkLoop = 30U;				/* ��������6�`7���[�v���� */
	while( !(SubSX1272Read(REG_IRQFLAGS1) & 0x80) & wkLoop > 0U)
	{
		wkLoop--;
	}
#if 0
	if( !gvRfIntFlg.mRadioDio4 )
	{
		R_IT_Create_Custom( arSensTime );		/* RSSI�J�n����STOP�����܂Ŏ���150usec */
//	R_IT_Start_Custom();				/* R_IT_Create_Custom�֐�����start���Ă��� */
		M_STOP;
	}
	
	SubSX1272Sleep();
	R_INTC1_Stop();
	R_IT_Stop();
#else
	if( !gvRfIntFlg.mRadioDio4 )
	{
		SubIntervalTimer_Sleep( arSensTime, 1U );		/* RSSI�J�n����STOP�����܂Ŏ���150usec */
		/* M_STOP�̑��� */
		while( !(gvRfIntFlg.mRadioDio4 | gvRfIntFlg.mRadioTimer) )
		{
			M_HALT;
			vrfCurrentRssi = SubRfDrv_GetRssi();
		}
	}
	
#if (swRfRxTest == imDisable)
	if( (gvMode == ecMode_RfRxLoRa) || (gvMode == ecMode_RfRxFSK) || (gvMode == ecMode_RfRxLoRaCons) )
	{
		;
	}
	else
	{
		SubSX1272Sleep();
	}
#endif
	
	R_INTC1_Stop();
	R_TAU0_Channel6_Stop();
#endif
	
	/* 2021.7.12 vrfRfErr��ǉ� */
	if( gvRfIntFlg.mRadioDio4 )
	{
		wkRet = SENS_CARRIER;
		vrfRfErr = 0U;
	}
	else
	{
		wkRet = NO_CARRIER;
		vrfRfErr = 0U;
	}
	
	if( wkLoop == 0U )
	{
		/* ����G���[ */
		wkRet = SENS_CARRIER;
		vrfRfErr++;
		gvrfStsEventFlag.mReset = RFIC_INI;
		gvrfStsEventFlag.mHsmodeInit = 0U;
	}
	
	return wkRet;
}

/*
*******************************************************************************
*	FSK���[�h 5ch���ł̃L�����A�Z���X
*
*	[���e]
*		FSK���[�h/DIO04_11,MAP_RSSI/SLEEP or STANDBY���O��
*		�`�����l��100kHz����5��X�L��������(�v500kHz)
*�@�@�@�@��莞�ԃL�����A�Z���X���s���B(�v2.2msec)
*		���荞�݂̓}�X�N���邱��(�}�X�N��Ԃ̂܂ܔ�����)
*		Sleep��ԂŔ�����
* [����]
*	�@�`�����l�����
* [Return]
*	NO_CARRIER�F�L�����A�Ȃ�
*	SENS_CARRIER : �L�����A����
*******************************************************************************
*/
uint8_t SubRFDrv_CarrierSens_5ch( uint8_t arCh )
{
	uint8_t wkRet;
	uint8_t wkRadio;

	wkRet = NO_CARRIER;
//	SubSX1272Sleep();
	SubSX1272Write( REG_RXBW, RF_RXBW_MANT_20 | RF_RXBW_EXP_3 );
	for( wkRadio = 0U; wkRadio < 5U; wkRadio++ )
	{
		SubSX1272FreqSet( arCh - 2U + wkRadio, rfFreqOffset );
		if( SubRFDrv_CarrierSens_FSK( 30U ) == SENS_CARRIER )			/* 80usec */
		{
			wkRet = SENS_CARRIER;
			break;
		}
		else if(vrfCurrentRssi < (RF_RSSITHRESH_THRESHOLD / 2U))
		{
			wkRet = SENS_CARRIER;
			break;
		}
	}
//	SubSX1272Sleep();
	SubSX1272Write( REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_1 );
	SubSX1272FreqSet( arCh, rfFreqOffset);
//	SubSX1272Sleep();
	return wkRet;
}


/*
********************************************************************************
*  �L�����A�Z���X��̃��g���C����ƃ`���l���ύX
*  [�T�v]
*  �L�����A�Z���X�̌��ʂ��烊�g���C�񐔃v���X�Await���Ԑݒ�A
*  ����L�����A�Z���X�̃`���l���ύX���f������(END���͕K���`���l���ύX)
*  vLoopLoRa�͎��O��0�ɃZ�b�g����
*********************************************************************************
*/
uint8_t SubRfDrv_CarrierSens_Rt_RetryDecision( uint16_t arWaitTime, uint8_t arChChenge, uint8_t arMode )
{
	uint8_t wkReturn;

	RTCMK = 0U;   /* enable INTRTC interrupt */
	SubSX1272Sleep();
	if( vLoopLoRa < RT_MODE_CS_RETRY_NUM )
	{
		/* CarrierSens Retry */
		if( arChChenge == CH_CHANGE )
		{
			vCallChLoopLora++;
			if( vCallChLoopLora >= RT_MODE_CH_LOOP_NUM ){	vCallChLoopLora = 0U; }
			if(arMode == CS_RT_CONNECT_MODE)
			{
//				SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[vrfLoraChGrupeCnt].mCh[vCallChLoopLora], rfFreqOffset );
				SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mCh[ vCallChLoopLora ], rfFreqOffset );
			}else
			{
				SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_MEASURE[vrfLoraChGrupeMes].mCh[vCallChLoopLora], rfFreqOffset );
			}
		}
		vLoopLoRa++;
		if( arWaitTime > 0U )
		{
			SubIntervalTimer_Sleep( arWaitTime, 1U );
		}
		wkReturn = NEXT;
	}
	else
	{
//		SubIntervalTimer_Stop();
		vCallChLoopLora++;
		if( vCallChLoopLora >= RT_MODE_CH_LOOP_NUM ){	vCallChLoopLora = 0U; }
		if(arMode == CS_RT_CONNECT_MODE)
		{
//			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[vrfLoraChGrupeCnt].mCh[vCallChLoopLora], rfFreqOffset );
			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_CNCT[ gvInFlash.mParam.mrfLoraChGrupeCnt ].mCh[ vCallChLoopLora ], rfFreqOffset );
		}else
		{
			SubSX1272FreqSet( RF_RTMODE_CH_GRUPE_MEASURE[vrfLoraChGrupeMes].mCh[vCallChLoopLora], rfFreqOffset );
		}
		wkReturn = END;
	}
	return wkReturn;
}

/*
********************************************************************************
*	�p�P�b�g��CRC,BCH��t��
*		[���e]
*		�f�[�^��4,5byte�ڂ�CRC�ǉ�
*		[����]
*		wkLength�FCRC�ABCH�������f�[�^���B11�̔{���ƂȂ�
*		wkLength�FBCH�Ȃ��̏ꍇ�A�S�f�[�^��
********************************************************************************
*/
uint8_t SubRfDrv_Coding_HS(uint8_t *arDataIn, uint16_t arLength, uint8_t arBchOption )
{
	uint16_t		wkDataCount;
//	uint16_t		wkDataBlockCount;
//	uint16_t		wkDataBlockNum;
	uint16_t	wkResult;
	uint8_t wkReturn;
	uint8_t		wkCrc;
//	uint16_t wkBchWork[8U];
//	uint8_t wkTest;

	/* CRC���܂߂��f�[�^���̃`�F�b�N */
	if( !(arLength % 11U) || (arBchOption == BCH_OFF) )
	{
		/* CRC�Z�o */
		CRCD = 0x0000U;
		for( wkDataCount = 0U; wkDataCount < arLength; wkDataCount++ )
		{
			if( wkDataCount == 3U )
			{
				wkDataCount += 2U;
			}
			wkCrc = *(arDataIn + wkDataCount);
			CRCIN = wkCrc;
		}
		wkResult = CRCD;
		
		/* �f�[�^��4,5byte�ڂ�CRC�ǉ� */
		*(arDataIn + 3U) = (uint8_t)( (wkResult >> 8U) & 0x00FF );
		*(arDataIn + 4U) = (uint8_t) wkResult;
#if 0
		if( arBchOption == BCH_ON )
		{
			/*BCH������*/
			wkDataBlockNum = arLength / 11U;
			for( wkDataBlockCount = 0U; wkDataBlockCount < wkDataBlockNum; wkDataBlockCount++ )
			{
				SubRfDrv_PreBCH( arDataIn + (wkDataBlockCount * 11U), &wkBchWork );
				SubRfDrv_MakeBCH( &wkBchWork );
				SubRfDrv_Interleave( &wkBchWork, arDataIn + (arLength + (wkDataBlockCount * 4U)) );		/* BCH�����̈ʒu���w�� */
			}
		}
#endif
		wkReturn = SUCCESS;
	}
	else
	{
		/* �f�[�^����11�̔{���łȂ� */
		wkReturn = ERROR;
	}
	
	return( wkReturn );
}

#if 0
/*
********************************************************************************
*	No22�̌v���l�p�P�b�g��CRC,BCH��t��
*		[���e]
*		HSmode�v���l�̃p�P�b�g��CRC,BCH��t���B9byte�f�[�^,2byte CRC,4byte BCH
*		[����]
*
********************************************************************************
*/
void SubRfDrv_Coding_Measure(uint8_t *arDataIn)
{
	uint16_t		wkDataCount;
//	uint16_t		wkDataBlockCount;
//	uint16_t		wkDataBlockNum;
//	uint16_t	wkResult;
//	uint8_t wkReturn;
//	uint8_t		wkCrc;
//	uint16_t wkBchWork[8U];
	
	for( wkDataCount = 0U; wkDataCount < 5U; wkDataCount++ )	/* 1���[�v��2byte�������� */
	{
		/* BCH��mBch�̏�ʃr�b�g���疄�߂� */
		*( arDataIn + 10U + wkDataCount ) = RF_HSMODE_BCH[ *(arDataIn + wkDataCount * 2U) ] << 4U;
		*( arDataIn + 10U + wkDataCount ) &= 0xF0;
		*( arDataIn + 10U + wkDataCount ) += RF_HSMODE_BCH[ *(arDataIn + wkDataCount * 2U + 1U) ];
	}
	
#if 0
	/* CRC�Z�o */
	CRCD = 0x0000U;
	for( wkDataCount = 0U; wkDataCount < 9U; wkDataCount++ )
	{
		wkCrc = *(arDataIn + wkDataCount);
		CRCIN = wkCrc;
	}
	wkResult = CRCD;
	
	/* �f�[�^��11,12byte�ڂ�CRC�ǉ� */
	*(arDataIn + 10U) = (uint8_t)( (wkResult >> 8U) & 0x00FF );
	*(arDataIn + 11U) = (uint8_t) wkResult;
	/* ��10-18�܂�12byteBCH�t�� */
	SubRfDrv_MakeBCH( arDataIn, arDataIn + 12U, 12U );
	/*BCH������*/
//	SubRfDrv_PreBCH( arDataIn , &wkBchWork );
//	SubRfDrv_MakeBCH( &wkBchWork );
//	SubRfDrv_Interleave( &wkBchWork, arDataIn );		/* BCH�����̈ʒu���w�� */
/*
	*(arDataIn + 11U) = (uint8_t)((*wkBchWork & 0x000F) << 4U) + (uint8_t)(*(wkBchWork + 1U) & 0x000F);
	*(arDataIn + 12U) = (uint8_t)((*(wkBchWork + 2U) & 0x000F) << 4U) + (uint8_t)(*(wkBchWork + 3U) & 0x000F);
	*(arDataIn + 13U) = (uint8_t)((*(wkBchWork + 4U) & 0x000F) << 4U) + (uint8_t)(*(wkBchWork + 5U) & 0x000F);
	*(arDataIn + 14U) = (uint8_t)((*(wkBchWork + 6U) & 0x000F) << 4U) + (uint8_t)(*(wkBchWork + 7U) & 0x000F);
*/
#endif
	NOP();
}
#endif

/*
********************************************************************************
*	�f�[�^��BCH��t��
*		[���e]
*		2byte�P�ʂ̃f�[�^��1byte��BCH������t������
*		[����]
*		arData�F����������f�[�^�̐擪�|�C���^
*		arBchOffset�FBCH�������i�[����擪�f�[�^�ʒu (10�Ȃ�arData�̈ʒu +10~+14�܂�BCH)
*		arDataNumber�F����������f�[�^���@�������ł��邱��
********************************************************************************
*/
void SubRfDrv_Coding_Bch(uint8_t *arDataIn, uint16_t arBchOffset, uint16_t arDataNumber)
{
	uint16_t		wkDataCount;
//	uint16_t	wkResult;
//	uint8_t		wkCrc;
	
	for( wkDataCount = 0U; wkDataCount < (arDataNumber / 2U); wkDataCount++ )	/* 1���[�v��2byte�������� */
	{
		/* BCH��mBch�̏�ʃr�b�g���疄�߂� */
		*( arDataIn + arBchOffset + wkDataCount ) = RF_HSMODE_BCH[ *(arDataIn + wkDataCount * 2U) ] << 4U;
		*( arDataIn + arBchOffset + wkDataCount ) &= 0xF0;
		*( arDataIn + arBchOffset + wkDataCount ) += RF_HSMODE_BCH[ *(arDataIn + wkDataCount * 2U + 1U) ];
	}
}

#if 0
/*
********************************************************************************
*	��M����No22�̌v���l�p�P�b�g��BCH����
*		[���e]
*		HSmode�v���l�̃p�P�b�g�𕜍��B10byte�f�[�^,5byte BCH
*		[Return]
*		�G���[������
********************************************************************************
*/
uint8_t SubRfDrv_Decoding_Measure(uint8_t *arCulcData)
{
	uint16_t wkDataOut;
	uint16_t wkDataIn;
	uint8_t wkShiftReg = 0x00; 		/* �V�t�g���W�X�^�l			*/
	uint8_t wkFFout = 0x00;			/* �t���b�v�t���b�v�o��		*/
	uint8_t wkLoop;
	uint8_t wkDataCnt;
//	uint8_t wkCulc;
	uint8_t wkBch[10U];
	uint8_t wkReturn = 0U;
//	uint16_t	wkResult;
	
	/* BCH������4bit���ƂɓW�J */
	for( wkDataCnt = 0U; wkDataCnt < 5U; wkDataCnt++ )
	{
		/* �ꏊ��vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[].mMeasure[].mBch */
		wkBch[wkDataCnt * 2U] = (*(arCulcData + 10U + wkDataCnt) >> 4U) & 0x0F;
		wkBch[wkDataCnt * 2U + 1U] = *(arCulcData + 10U + wkDataCnt) & 0x0F;
	}
	/* BCH���� 10�f�[�^ */
	for( wkDataCnt = 0U; wkDataCnt < 10U; wkDataCnt++ )
	{
		wkDataIn = ((uint16_t)(*arCulcData) << 4U) & 0x0FF0;
		wkDataIn += wkBch[wkDataCnt];
		wkDataOut = wkDataIn;
		wkFFout = 0U;
		wkShiftReg = 0U;
		
		for( wkLoop = 0U; wkLoop < 15U; wkLoop++ )
		{
			wkFFout = (wkFFout & 0xEF) | ( ((wkShiftReg & 0x08) << 1U) ^ (uint8_t)( (wkDataIn & 0x4000) >> 10U) ); 	/* X4 = R4 xor D11	�����1bit�͖���	*/
			wkFFout = (wkFFout & 0xF7) | ( (wkShiftReg & 0x04) << 1U); 									/* X3 = R3								*/
			wkFFout = (wkFFout & 0xFB) | ( (wkShiftReg & 0x02) << 1U); 									/* X2 = R2								*/
			wkFFout = (wkFFout & 0xFD) | ( ( ((wkShiftReg & 0x01) << 1U) ^ ( (wkFFout & 0x10) >> 3U) ));		/* X1 = X4 xor R1						*/
			wkFFout = (wkFFout & 0xFE) | ( (wkFFout & 0x10) >> 4U);										/* X0 = X4								*/
			wkShiftReg = wkFFout & 0x0F;
			wkDataIn = wkDataIn << 1U;
		}
		
		if(!(wkShiftReg == 0U) )
		{
			/* ���3bitD14-D12��0�Œ�ABCH������D0-D3�͖��� */
			switch( wkShiftReg ){
				case 0x01:		/* D11			*/
					wkDataOut = wkDataOut ^ 0x0800;
					break;
//				case 0x02:		/* D12			*/
//					wkDataOut = wkDataOut ^ 0x1000;
//					break;
//				case 0x03:		/* D0			*/
//					wkDataOut = wkDataOut ^ 0x0001;
//					break;
//				case 0x04:		/* D13			*/
//					wkDataOut = wkDataOut ^ 0x2000;
//					break;
				case 0x05:		/* D4			*/
					wkDataOut = wkDataOut ^ 0x0010;
					break;
//				case 0x06:		/* D1			*/
//					wkDataOut = wkDataOut ^ 0x0002;
//					break;
				case 0x07:		/* D6			*/
					wkDataOut = wkDataOut ^ 0x0040;
					break;
//				case 0x08:		/* D14			*/
//					wkDataOut = wkDataOut ^ 0x4000;
//					break;
				case 0x09:		/* D10			*/
					wkDataOut = wkDataOut ^ 0x0400;
					break;
				case 0x0A:		/* D5			*/
					wkDataOut = wkDataOut ^ 0x0020;
					break;
//				case 0x0B:		/* D3			*/
//					wkDataOut = wkDataOut ^ 0x0008;
//					break;
//				case 0x0C:		/* D2			*/
//					wkDataOut = wkDataOut ^ 0x0004;
//					break;
				case 0x0D:		/* D9			*/
					wkDataOut = wkDataOut ^ 0x0200;
					break;
				case 0x0E:		/* D7			*/
					wkDataOut = wkDataOut ^ 0x0080;
					break;
				case 0x0F:		/* D8			*/
					wkDataOut = wkDataOut ^ 0x0100;
					break;
			}
			*arCulcData = (uint8_t)((wkDataOut >> 4U) & 0x00FF);
			wkReturn++;
		}
		arCulcData++;
	}
	return wkReturn;
}
#endif

/*
********************************************************************************
*	BCH��������
*		[���e]
*		BCH�����t���f�[�^�𕜍��B
*		[����]
*		arCulcData�F�f�[�^�̐擪�A�h���X
*		arOffset�FBCH�����̐擪�A�h���X arCulcData + arOfset
*		arDataNumber�F��������f�[�^���@�������ł��邱�Ɓ@���ő�22�f�[�^�܂�
*		[Return]
*		�G���[������
********************************************************************************
*/
uint8_t SubRfDrv_Decoding_Bch(uint8_t *arCulcData, uint16_t arOffset, uint8_t arDataNumber)
{
	uint16_t wkDataOut;
	uint16_t wkDataIn;
	uint8_t wkShiftReg = 0x00; 		/* �V�t�g���W�X�^�l			*/
	uint8_t wkFFout = 0x00;			/* �t���b�v�t���b�v�o��		*/
	uint8_t wkLoop;
	uint8_t wkDataCnt;
//	uint8_t wkCulc;
	uint8_t wkBch[22U];
	uint8_t wkReturn = 0U;
//	uint16_t	wkResult;
	
	/* BCH������4bit���ƂɓW�J */
	for( wkDataCnt = 0U; wkDataCnt < (arDataNumber / 2U); wkDataCnt++ )
	{
		/* �ꏊ��vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[].mMeasure[].mBch */
		wkBch[wkDataCnt * 2U] = (*(arCulcData + arOffset + wkDataCnt) >> 4U) & 0x0F;
		wkBch[wkDataCnt * 2U + 1U] = *(arCulcData + arOffset + wkDataCnt) & 0x0F;
	}
	/* BCH���� 10�f�[�^ */
	for( wkDataCnt = 0U; wkDataCnt < arDataNumber; wkDataCnt++ )
	{
		wkDataIn = ((uint16_t)(*arCulcData) << 4U) & 0x0FF0;
		wkDataIn += wkBch[wkDataCnt];
		wkDataOut = wkDataIn;
		wkFFout = 0U;
		wkShiftReg = 0U;
		
		for( wkLoop = 0U; wkLoop < 15U; wkLoop++ )
		{
			wkFFout = (wkFFout & 0xEF) | ( ((wkShiftReg & 0x08) << 1U) ^ (uint8_t)( (wkDataIn & 0x4000) >> 10U) ); 	/* X4 = R4 xor D11	�����1bit�͖���	*/
			wkFFout = (wkFFout & 0xF7) | ( (wkShiftReg & 0x04) << 1U); 									/* X3 = R3								*/
			wkFFout = (wkFFout & 0xFB) | ( (wkShiftReg & 0x02) << 1U); 									/* X2 = R2								*/
			wkFFout = (wkFFout & 0xFD) | ( ( ((wkShiftReg & 0x01) << 1U) ^ ( (wkFFout & 0x10) >> 3U) ));		/* X1 = X4 xor R1						*/
			wkFFout = (wkFFout & 0xFE) | ( (wkFFout & 0x10) >> 4U);										/* X0 = X4								*/
			wkShiftReg = wkFFout & 0x0F;
			wkDataIn = wkDataIn << 1U;
		}
		
		if(!(wkShiftReg == 0U) )
		{
			/* ���3bitD14-D12��0�Œ�ABCH������D0-D3�͖��� */
			switch( wkShiftReg ){
				case 0x01:		/* D11			*/
					wkDataOut = wkDataOut ^ 0x0800;
					break;
//				case 0x02:		/* D12			*/
//					wkDataOut = wkDataOut ^ 0x1000;
//					break;
//				case 0x03:		/* D0			*/
//					wkDataOut = wkDataOut ^ 0x0001;
//					break;
//				case 0x04:		/* D13			*/
//					wkDataOut = wkDataOut ^ 0x2000;
//					break;
				case 0x05:		/* D4			*/
					wkDataOut = wkDataOut ^ 0x0010;
					break;
//				case 0x06:		/* D1			*/
//					wkDataOut = wkDataOut ^ 0x0002;
//					break;
				case 0x07:		/* D6			*/
					wkDataOut = wkDataOut ^ 0x0040;
					break;
//				case 0x08:		/* D14			*/
//					wkDataOut = wkDataOut ^ 0x4000;
//					break;
				case 0x09:		/* D10			*/
					wkDataOut = wkDataOut ^ 0x0400;
					break;
				case 0x0A:		/* D5			*/
					wkDataOut = wkDataOut ^ 0x0020;
					break;
//				case 0x0B:		/* D3			*/
//					wkDataOut = wkDataOut ^ 0x0008;
//					break;
//				case 0x0C:		/* D2			*/
//					wkDataOut = wkDataOut ^ 0x0004;
//					break;
				case 0x0D:		/* D9			*/
					wkDataOut = wkDataOut ^ 0x0200;
					break;
				case 0x0E:		/* D7			*/
					wkDataOut = wkDataOut ^ 0x0080;
					break;
				case 0x0F:		/* D8			*/
					wkDataOut = wkDataOut ^ 0x0100;
					break;
			}
			*arCulcData = (uint8_t)((wkDataOut >> 4U) & 0x00FF);
			wkReturn++;
		}
		arCulcData++;
	}
	return wkReturn;
}

/*
********************************************************************************
*	�p�P�b�g��CRC��t��
*		[���e]
*		RT���[�h�̃p�P�b�g��CRC��t��
*		[����]
*		wkLength�FCRC�������f�[�^���B(ecRfDrvMainSts_RTmode_MeasureDataSet�̏ꍇ����)
********************************************************************************
*/
void SubRfDrv_Crc_Coding(uint8_t *wkDataIn, uint8_t wkLength)
{
	uint8_t		wkDataCount;
//	uint8_t		wkDataBlockCount;
	uint16_t	wkResult;
	uint8_t		wkCrc;
	
#if (swLoggerBord == imEnable)
	uint16_t	wkCrcLen;
	
	
	/* CRC�Z�o */
	CRCD = 0x0000U;
	switch (vRfDrvMainLoggerSts)
	{
		case ecRfDrvMainSts_RTmode_MeasureDataSet:
		case ecRfDrvMainSts_RTmode_MeasAlmDataSet:
			
			if( vRfDrvMainLoggerSts == ecRfDrvMainSts_RTmode_MeasureDataSet )
			{
				wkCrcLen = RT_MODE_PAYLOAD_LENGTH_5_CRC;
			}
			else
			{
				wkCrcLen = RT_MODE_PAYLOAD_LENGTH_10_CRC;
			}
			
			/* RTmode�v���l���M���̓f�[�^���� */
			for( wkDataCount = 0U; wkDataCount < wkLength; wkDataCount++ )
			{
				/* CRC��31,32byte�� (30U,31U) */
				if( wkDataCount == wkCrcLen - 2U )
				{
					if( wkLength == (wkCrcLen) )
					{
						/* �v���l���[���̏ꍇ */
						break;
					}
					wkDataCount += 2U;
				}
				wkCrc = *(wkDataIn + wkDataCount);
				CRCIN = wkCrc;
			}
			wkResult = CRCD;
			
			/* �v���l��O��CRC�ǉ� */
			*(wkDataIn + wkCrcLen - 2U) = (uint8_t)( (wkResult >> 8U) & 0x00FF );
			*(wkDataIn + wkCrcLen - 1U) = (uint8_t) wkResult;
			break;
			
		default:
			for( wkDataCount = 0U; wkDataCount < wkLength; wkDataCount++ )
			{
				wkCrc = *(wkDataIn + wkDataCount);
				CRCIN = wkCrc;
			}
			wkResult = CRCD;
			
			/* �f�[�^�̌��(BCH��O)��CRC�ǉ� */
			*(wkDataIn + wkLength) = (uint8_t)( (wkResult >> 8U) & 0x00FF );
			*(wkDataIn + wkLength + 1U) = (uint8_t) wkResult;
			break;
	}
#endif
#if (swLoggerBord == imDisable)
	/* CRC�Z�o */
	CRCD = 0x0000U;
	for( wkDataCount = 0U; wkDataCount < wkLength; wkDataCount++ )
	{
		wkCrc = *(wkDataIn + wkDataCount);
		CRCIN = wkCrc;
	}
	wkResult = CRCD;
	
	/* �f�[�^�̌��(BCH��O)��CRC�ǉ� */
	*(wkDataIn + wkLength) = (uint8_t)( (wkResult >> 8U) & 0x00FF );
	*(wkDataIn + wkLength + 1U) = (uint8_t) wkResult;
#endif
}


/*
********************************************************************************
*	�p�P�b�g��CRC�`�F�b�N
*		[���e]
*		RT���[�h�̃p�P�b�g��CRC�`�F�b�N
*		[����]
*		wkLength�FCRC���܂ރf�[�^���B
********************************************************************************
*/
uint8_t SubRfDrv_Crc_Decoding(uint8_t *wkDataIn, uint8_t wkLength)
{
	uint8_t		wkDataCount;
//	uint8_t		wkDataBlockCount;
//	uint8_t		wkDataBlockNum;
	uint16_t	wkResult;
	uint8_t wkReturn;
	uint8_t		wkCrc;
//	uint16_t wkBchWork[8U];
//	uint8_t wkTest;
	
#if (swLoggerBord == imDisable)
	switch (vRfDrvMainGwSts)
	{
		case ecRfDrvMainSts_RTmode_MeasureRxRead:
			/* RTmode�ł̌v���l��M�͉ϒ��p�P�b�g */
			if( (vutRfDataBuffer.umRT_AuditTx.mDataCntKind & 0x80) == 0x80 )
			{
				wkReturn = RT_MODE_PAYLOAD_LENGTH_10_CRC;
			}
			else
			{
				wkReturn = RT_MODE_PAYLOAD_LENGTH_5_CRC;
			}
			/* CRC�Z�o */
			CRCD = 0x0000U;
			/* CRC�Z�o */
			for( wkDataCount = 0U; wkDataCount < wkLength; wkDataCount++ )
			{
				/* CRC�̉ӏ�(31,32byte��)�����f 30U */
				if( wkDataCount == (wkReturn - 2U) )
				{
					/* CRC�܂ł�32byte */
					if( wkLength == (wkReturn) )
					{
						/* �v���l���[���̏ꍇ */
						break;
					}
					wkDataCount += 2U;
				}
				wkCrc = *(wkDataIn + wkDataCount);
				CRCIN = wkCrc;
			}
			wkResult = CRCD;
			
			if(	(*(wkDataIn + wkReturn - 2U) == (uint8_t)( (wkResult >> 8U) & 0x00FF ))
					&& (*(wkDataIn + (wkReturn - 1U)) == (uint8_t) wkResult) )
			{
				wkReturn = SUCCESS;
			}
			else
			{
				/* BCH�����̏ꍇ�́A�������畜���� */
				wkReturn = ERROR;
			}
			break;
			
		default:
			/* CRC��BCH���������f�[�^���ɕϊ� */
			wkLength -= 2U ;
			/* CRC�Z�o */
			CRCD = 0x0000U;
			/* CRC�Z�o */
			for( wkDataCount = 0U; wkDataCount < wkLength; wkDataCount++ )
			{
				wkCrc = *(wkDataIn + wkDataCount);
				CRCIN = wkCrc;
			}
			wkResult = CRCD;
			
			if(	(*(wkDataIn + wkLength) == (uint8_t)( (wkResult >> 8U) & 0x00FF ))
					&& (*(wkDataIn + (wkLength + 1U)) == (uint8_t) wkResult) )
			{
				wkReturn = SUCCESS;
			}
			else
			{
				/* BCH�����̏ꍇ�́A�������畜���� */
				wkReturn = ERROR;
			}
			break;
	}
	
	return( wkReturn );
	
#endif
#if (swLoggerBord == imEnable)
	/* CRC��BCH���������f�[�^���ɕϊ� */
	wkLength -= 2U ;
	/* CRC�Z�o */
	CRCD = 0x0000U;
	/* CRC�Z�o */
	for( wkDataCount = 0U; wkDataCount < wkLength; wkDataCount++ )
	{
		wkCrc = *(wkDataIn + wkDataCount);
		CRCIN = wkCrc;
	}
	wkResult = CRCD;
	
	if(	(*(wkDataIn + wkLength) == (uint8_t)( (wkResult >> 8U) & 0x00FF ))
			&& (*(wkDataIn + (wkLength + 1U)) == (uint8_t) wkResult) )
	{
		wkReturn = SUCCESS;
	}
	else
	{
		/* BCH�����̏ꍇ�́A�������畜���� */
		wkReturn = ERROR;
	}
	return( wkReturn );
#endif
}


/*
********************************************************************************
*	�p�P�b�g��CRC�`�F�b�N
*		[���e]
*		HS���[�h�̃p�P�b�g��CRC�`�F�b�N�BCRC�ʒu��4,5byte
*		[����]
*		wkLength�FCRC���܂ރf�[�^���BBCH�����Ȃ��̏ꍇ�͑S�f�[�^���B
********************************************************************************
*/
uint8_t SubRfDrv_Crc_Decoding_HS(uint8_t *wkDataIn, uint16_t wkLength, uint8_t arBch )
{
	uint16_t		wkDataCount;
//	uint16_t		wkDataBlockCount;
//	uint16_t		wkDataBlockNum;
	uint16_t	wkResult;
	uint8_t wkReturn;
	uint8_t		wkCrc;
//	uint16_t wkBchWork[8U];
	
	/* CRC��BCH���������f�[�^���ɕϊ� */
	if( arBch == BCH_ON )
	{
		wkCrc =  wkLength / 15U;
		wkLength -= (wkCrc * 4U);
	}
	
	/* CRC�Z�o */
	CRCD = 0x0000U;
	/* CRC�Z�o */
	for( wkDataCount = 0U; wkDataCount < wkLength; wkDataCount++ )
	{
		if(wkDataCount == 3U )
		{
			/* CRC�}���ʒu�͔�΂� */
			wkDataCount += 2U;
		}
		wkCrc = *(wkDataIn + wkDataCount);
		CRCIN = wkCrc;
	}
	wkResult = CRCD;
	
	if(	(*(wkDataIn + 3U) == (uint8_t)( (wkResult >> 8U) & 0x00FF ))
			&& (*(wkDataIn + 4U) == (uint8_t) wkResult) )
	{
		wkReturn = SUCCESS;
	}
	else
	{
		/* BCH�����̏ꍇ�́A�������畜���� */
		wkReturn = ERROR;
	}
	return( wkReturn );
}


/*****************************************************************************
	[Ӽޭ�ٖ�]
	  [���{��]	BCH�������O����
		[�T�v]	�s��ϊ��@uint8_t [11] �� uint16_t [8]
		[�@�\]	�V
		[����]	uint8_t wkDataIn[0-10]
		[�o��]	uint16_t wkDataOut[0-7]
		[���l]	���o�͂̔z�񒷂͌Œ�
		[���]
		[����]
*****************************************************************************/
void SubRfDrv_PreBCH( uint8_t *wkDataIn, uint16_t *wkDataOut)
{
	uint16_t wkBCH;
//	uint8_t wkTest;
//	uint8_t wkMakeDataOutLoop;
//	uint8_t wkBitShiftLoop;
	
	int8_t wkX_Axis;
	int8_t wkY_Axis;
	
	/* ���������� */
	for( wkBCH = 0U; wkBCH < 8U; wkBCH++ )
	{
		*(wkDataOut + wkBCH) = 0x00;
	}
	
	for( wkX_Axis = 0U; wkX_Axis < 8U; wkX_Axis++ )
	{
		for( wkY_Axis = 0U; wkY_Axis < 11U; wkY_Axis++ )
		{
			if( (8U + wkX_Axis) > wkY_Axis )
			{
//				wkBCH = (uint16_t)(*(wkDataIn + wkY_Axis));
//				wkBCH = wkBCH & RF_AXIS8[wkX_Axis];
//				wkBCH = wkBCH << (8U - wkY_Axis + wkX_Axis);
//				*(wkDataOut + wkX_Axis) += wkBCH;
				*(wkDataOut + wkX_Axis) += ((uint16_t)(*(wkDataIn + wkY_Axis) & RF_AXIS8[wkX_Axis]) << (8U - wkY_Axis + wkX_Axis));
			}else
			{
//				wkBCH = (uint16_t)(*(wkDataIn + wkY_Axis));
//				wkBCH = wkBCH & RF_AXIS8[wkX_Axis];
//				wkBCH = wkBCH >> (wkY_Axis - 8U - wkX_Axis);
//				*(wkDataOut + wkX_Axis) += wkBCH;
				*(wkDataOut + wkX_Axis) += (uint16_t)(*(wkDataIn + wkY_Axis) & RF_AXIS8[wkX_Axis]) >> (wkY_Axis - 8U - wkX_Axis);
			}
		}
	}
	
#if 0
	/* ���������� */
	for( wkBCH = 0U; wkBCH < 8U; wkBCH++ )
	{
		*(wkDataOut + wkBCH) = 0x00;
	}

	for(wkMakeDataOutLoop = 0U; wkMakeDataOutLoop < 8U; wkMakeDataOutLoop++)
	{
		/* ���1bit�͋󔒁A����4bit��BCH�����Ŗ��܂� */
		for( wkBitShiftLoop = 0U; wkBitShiftLoop < 11U; wkBitShiftLoop++ )
		{
			/* �}�X�N��1bit���o */
			wkBCH = ((uint16_t)*(wkDataIn + wkBitShiftLoop)) & RF_PRE_BCH[wkMakeDataOutLoop].Mask;
			if( (int8_t) (RF_PRE_BCH[wkMakeDataOutLoop].Shift - wkBitShiftLoop) > 0 )
			{
				/* ���V�t�g */
				wkTest = RF_PRE_BCH[wkMakeDataOutLoop].Shift - wkBitShiftLoop;
				*wkDataOut |= wkBCH << (RF_PRE_BCH[wkMakeDataOutLoop].Shift - wkBitShiftLoop);
			}
			else if( RF_PRE_BCH[wkMakeDataOutLoop].Shift - wkBitShiftLoop == 0U )
			{
				/* �V�t�g�Ȃ� */
				*wkDataOut |= wkBCH;
			}
			else
			{
				/* �E�V�t�g */
				*wkDataOut |= wkBCH >> ( wkBitShiftLoop - RF_PRE_BCH[wkMakeDataOutLoop].Shift );
			}
		}
		/* ���̃f�[�^ */
		wkDataOut++;
	}
#endif
}


/*****************************************************************************
	[Ӽޭ�ٖ�]
	  [���{��]	BCH�����@�t��
		[�T�v]	BCH����������4bit�ɒǉ�����B
		[�@�\]	�V
		[����]	arDatatNum�͋����łȂ���BCH��1byta�P�ʂŖ��܂�Ȃ�
		[�o��]
		[���l]	���͂�8bit�P�ʌŒ�A3bit(0�Œ�)�̂Ăč��v11bit�����B
					wkDataOut�̖���4bit�ɕ����t��
					��Ԗڂ͏o��BCH����(arBchOut)1byte�̏��4bit�A�����Ԗڂ͉���4bit
		[���]
		[����]
*****************************************************************************/
void SubRfDrv_MakeBCH(uint8_t *arCulcData, uint8_t *arBchOut, uint8_t arDatatNum)
{
	uint16_t wkDataOut;
	uint16_t wkDataIn;
	uint8_t wkShiftReg = 0x00; 		/* �V�t�g���W�X�^�l			*/
	uint8_t wkFFout = 0x00;			/* �t���b�v�t���b�v�o��		*/
	uint8_t wkLoop;
	uint8_t wkDataCnt;
//	uint8_t wkCulc;
	
#if 0
	for( wkDataCnt = 0U; wkDataCnt < arDatatNum; wkDataCnt++ )
	{
		wkDataIn = ((uint16_t)(*arCulcData) << 4U) & 0xFFF0;
		
		wkDataOut = wkDataIn;
		for( wkLoop = 0U; wkLoop < 11U; wkLoop++ )
		{
			wkCulc = wkShiftReg << 1U;
			wkFFout = (wkFFout & 0xEF) | ( (wkCulc & 0x10) ^  ((uint8_t)((wkDataIn & 0x4000) >> 10U) )); 	/* X4 = R4 xor D11	�����1bit�͖���	*/
			wkFFout = (wkFFout & 0xF7) | ( wkCulc & 0x08); 									/* X3 = R3 */
			wkFFout = (wkFFout & 0xFB) | ( wkCulc & 0x04); 									/* X2 = R2 */
			wkFFout = (wkFFout & 0xFD) | ( ( (wkCulc & 0x02) ^ ( (wkFFout & 0x10) >> 3U) ));		/* X1 = X4 xor R1	*/
			/* ������1bit */
			wkFFout = (wkFFout & 0xFE) | ( (wkCulc & 0x20) >> 5U);										/* X0 = X4 */
			
			wkShiftReg = wkFFout & 0x0F;
			wkDataIn = wkDataIn << 1U;
		}
		
		wkDataOut |= wkShiftReg;
		if( wkDataCnt & 0x01 )
		{
			*arBchOut = 0x00;
			*arBchOut = (uint8_t)((wkDataOut << 4U) & 0x00F0);
		}else
		{
			/* 1,3,5,7 */
			*arBchOut += (uint8_t)(wkDataOut & 0x000F);
			arBchOut++;
		}
		arCulcData++;
	}
#endif
	
#if 1
	for( wkDataCnt = 0U; wkDataCnt < arDatatNum; wkDataCnt++ )
	{
		wkDataIn = ((uint16_t)(*arCulcData) << 4U) & 0xFFF0;
		
		wkDataOut = wkDataIn;
		for( wkLoop = 0U; wkLoop < 11U; wkLoop++ )
		{
			wkFFout = (wkFFout & 0xEF) | ( ((wkShiftReg & 0x08) << 1U) ^ (uint8_t)( (wkDataIn & 0x4000) >> 10U) ); 	/* X4 = R4 xor D11	�����1bit�͖���	*/
			wkFFout = (wkFFout & 0xF7) | ( (wkShiftReg & 0x04) << 1U); 									/* X3 = R3								*/
			wkFFout = (wkFFout & 0xFB) | ( (wkShiftReg & 0x02) << 1U); 									/* X2 = R2								*/
			wkFFout = (wkFFout & 0xFD) | ( ( ((wkShiftReg & 0x01) << 1U) ^ ( (wkFFout & 0x10) >> 3U) ));		/* X1 = X4 xor R1						*/
			wkFFout = (wkFFout & 0xFE) | ( (wkFFout & 0x10) >> 4U);										/* X0 = X4								*/
			wkShiftReg = wkFFout & 0x0F;
			wkDataIn = wkDataIn << 1U;
		}
		
		wkDataOut |= wkShiftReg;
		if( wkDataCnt % 2U )
		{
			/* 1,3,5,7 */
			*arBchOut += (uint8_t)(wkDataOut & 0x000F);
			arBchOut++;
		}else
		{
			*arBchOut = 0x00;
			*arBchOut = (uint8_t)((wkDataOut << 4U) & 0x00F0);
		}
		arCulcData++;
	}
#endif
	
#if 0
	for( wkDataCnt = 0U; wkDataCnt < 8U; wkDataCnt++ )
	{
		wkDataIn = *wkData & 0xFFF0;
		/* �O������4bit�V�t�g�ς� */
		wkDataOut = wkDataIn;
		for( wkLoop = 0U; wkLoop < 11U; wkLoop++ )
		{
			wkCulc = wkShiftReg << 1U;
			wkFFout = (wkFFout & 0xEF) | ( (wkCulc & 0x10) ^  ((uint8_t)((wkDataIn & 0x4000) >> 10U) )); 	/* X4 = R4 xor D11	�����1bit�͖���	*/
			wkFFout = (wkFFout & 0xF7) | ( wkCulc & 0x08); 									/* X3 = R3 */
			wkFFout = (wkFFout & 0xFB) | ( wkCulc & 0x04); 									/* X2 = R2 */
			wkFFout = (wkFFout & 0xFD) | ( ( (wkCulc & 0x02) ^ ( (wkFFout & 0x10) >> 3U) ));		/* X1 = X4 xor R1	*/
			/* ������1bit */
			wkFFout = (wkFFout & 0xFE) | ( (wkCulc & 0x20) >> 5U);										/* X0 = X4 */
			
			wkShiftReg = wkFFout & 0x0F;
			wkDataIn = wkDataIn << 1U;
		}
		
		wkDataOut |= wkShiftReg;
		*wkData = wkDataOut;
		wkData++;
	}
#endif

}

#if 0
/*****************************************************************************
	[Ӽޭ�ٖ�]
	  [���{��]	�C���^�[���[�u����
		[�T�v]	BCH�������܂߃C���^�[���[�u�����B
		[�@�\]	�V
		[����]	uint16_t[0-7]
		[�o��]	uint8_t[0-14] -> BCH�����݂̂̏����ɂ��邱�Ƃɂ��[0-3]
		[���l]
		[���]
		[����]
*****************************************************************************/
void SubRfDrv_Interleave( uint16_t *wkDataIn, uint8_t *wkDataOut )
{
	uint16_t wkBCH;
//	uint8_t wkTest;
//	uint8_t wkMakeDataOutLoop;
//	uint8_t wkBitShiftLoop;
//	uint16_t wkMask = 0x0008; 	  /* BCH�����̂ݏ��� */
//	int8_t wkBitShiftCnt = -4;	  /* BCH�����݂̂̏��� */
	
	int8_t wkX_Axis;
	int8_t wkY_Axis;
	
	/* ���������� */
	for( wkBCH = 0U; wkBCH < 15U; wkBCH++ )
	{
		*(wkDataOut + wkBCH) = 0x00;
	}
	
	for( wkX_Axis = 0U; wkX_Axis < 15U; wkX_Axis++ )
	{
		for( wkY_Axis = 0U; wkY_Axis < 8U; wkY_Axis++ )
		{
			if( (8U + wkY_Axis) > wkX_Axis )
			{
				wkBCH = *(wkDataIn + wkY_Axis);
				wkBCH = wkBCH & RF_AXIS16[wkX_Axis];
				wkBCH = wkBCH >> (8U + wkY_Axis - wkX_Axis);
				*(wkDataOut + wkX_Axis) += (uint8_t)wkBCH;
//				*(wkDataOut + wkX_Axis) += (uint16_t)(*(wkDataIn + wkY_Axis) & RF_AXIS8[wkX_Axis]) << (8U - wkY_Axis + wkX_Axis);
			}else
			{
				wkBCH = *(wkDataIn + wkY_Axis);
				wkBCH = wkBCH & RF_AXIS16[wkX_Axis];
				wkBCH = wkBCH << (wkX_Axis - 8U - wkY_Axis);
				*(wkDataOut + wkX_Axis) += (uint8_t)wkBCH;
//				*(wkDataOut + wkX_Axis) += (uint16_t)(*(wkDataIn + wkY_Axis) & RF_AXIS8[wkX_Axis]) >> (wkY_Axis - 8U - wkX_Axis);
			}
		}
	}
	
#if 0
	/* BCH�����݂̂̏��� */
	for( wkBCH = 0U; wkBCH < 4U; wkBCH++ )
	{
		*(wkDataOut + wkBCH) = 0x00;
	}
	
	/* wkDataIn�̏��1bit�̓_�~�[�u0�v�̂��ߏȂ� */
	/* BCH����(����4but�̂ݏ���) */
	for(wkMakeDataOutLoop = 0U; wkMakeDataOutLoop < 4U; wkMakeDataOutLoop++)
	{
		/*	*/
		for( wkBitShiftLoop = 0U; wkBitShiftLoop < 8U; wkBitShiftLoop++ )
		{
			/* �}�X�N��1bit���o */
			wkBCH = ((uint16_t)*(wkDataIn + wkBitShiftLoop)) & wkMask;
			if( (int8_t) (wkBitShiftCnt + wkBitShiftLoop) > 0 )
			{
				/* �E�V�t�g */
				wkTest = wkBitShiftCnt + wkBitShiftLoop;
				*wkDataOut |= wkBCH >> (wkBitShiftCnt + wkBitShiftLoop);
			}
			else if( wkBitShiftCnt + wkBitShiftLoop == 0U )
			{
				/* �V�t�g�Ȃ� */
				*wkDataOut |= wkBCH;
			}
			else
			{
				/* ���V�t�g */
				wkTest = (0xFF ^ ( wkBitShiftLoop + wkBitShiftCnt ) ) + 1U;
				*wkDataOut |= wkBCH << ( (0xFF ^ ( wkBitShiftLoop + wkBitShiftCnt ) ) + 1U);
			}
		}
		/* �}�N�X��1bit�V�t�g�A�V�t�g�ʂ�1���炵�A���̃f�[�^ */
		wkMask >>= 1U;
		wkBitShiftCnt--;
		wkDataOut++;
	}
#endif
}
#endif

/*
*******************************************************************************
*  �������M�o�͂̕ύX
*  [�T�v]
*  �����̑��M�o�͂�13dBm or -1dBm�ɂ���B
*  FSK/Lora�̗������ʐݒ�ƂȂ� �� FSK mode����-1dBm�Œ�
*******************************************************************************
*/
void SubRfDrv_PwChange( uint8_t arPwMode )
{
	if( rfStatus.Modem == MODEM_FSK )
	{
		SubSX1272Write( REG_PACONFIG, RF_PACONFIG_PASELECT_RFO | RF_PACONFIG_MIN);
	}else if( arPwMode == PW_L )
	{
		SubSX1272Write( REG_LR_PACONFIG, RF_PACONFIG_PASELECT_RFO | RFLR_PACONFIG_OUTPUTPOWER_MIN);
	}else
	{
		SubSX1272Write( REG_LR_PACONFIG, RF_PACONFIG_PASELECT_RFO | RFLR_PACONFIG_OUTPUTPOWER);
	}
}

#if (swKouteiMode == imEnable)
/*
 *******************************************************************************
 *	PN9���M�֐�
 *	[���e]
 *		2.53msec���M(�v�Z��2.496msec) / 1.04msec�x�~ / �o�[�X�g���� 3.57msec
 *******************************************************************************
 */
void SubRFDrv_PN9FSK( uint8_t wkCH )
{
#if (swLoggerBord == imDisable)
	uint8_t wkRadio;
#endif
	uint8_t wkReturn;
	
	SubSX1272Sleep();
	rfStatus.Modem = MODEM_FSK;
	SubSX1272Sleep();
	SubSX1272Stdby();
	SubSX1272FreqSet( wkCH - 1U, rfFreqOffset );
	SubSX1272TxFSK_DIO_240kbps();
//	SubRFDrv_SyncSet();
	SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_RESPONSE );
	SubSX1272FSK_Length( 64U );
	SubSX1272Write( REG_PACONFIG, RF_PACONFIG_PASELECT_RFO | RF_PACONFIG_MIN);
	SubSX1272Write( REG_RXBW, RF_RXBW_MANT_20 | RF_RXBW_EXP_2 );
/*
	SubSX1272Write( REG_IMAGECAL, RF_IMAGECAL_IMAGECAL_START );
	while( (SubSX1272Read( REG_IMAGECAL ) & RF_IMAGECAL_IMAGECAL_RUNNING) == RF_IMAGECAL_IMAGECAL_RUNNING )
	{
		NOP();
	}
*/
	SubSX1272CarrierSenseFSK_DIO();
	
#if (swLoggerBord == imEnable)
	while( 1U )
	{
		R_WDT_Restart();
		wkReturn = 0U;
/* �Z�K�\���̎��͗L���ɂ���(�L�����A�Z���X�L��) */
#if 0
		SubSX1272Write( REG_RXBW, RF_RXBW_MANT_20 | RF_RXBW_EXP_2 );
		for( wkRadio = 0U; wkRadio < 5U; wkRadio++ )
		{
			SubSX1272FreqSet( wkCH - 2U + wkRadio, rfFreqOffset );
			if(SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == SENS_CARRIER)
			{
				wkReturn = 1U;
				break;
			}
		}
#endif
		
		SubSX1272Write( REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_1 );
		if( wkReturn == 0U )
		{
			SubSX1272FreqSet( wkCH, rfFreqOffset );
			SubSX1272TxFSK_DIO_240kbps();
			SubSX1272WriteFifo( (uint8_t *)PN9, 64U );
//			SubSX1272WriteFifo( &PN9, 64U );
			SubSX1272Tx( 64U );
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			while( !gvRfIntFlg.mRadioDio0 )
			{
				R_WDT_Restart();
			}
			R_INTC6_Stop();
			SubSX1272Stdby();
			
			if( gvMode == ecMode_RfTxFSKStop )
			{
				vRFPrm.mInitSts = ecRFDrvInitSts_Init;
				gvModuleSts.mRf = ecRfModuleSts_Run;
				while( 1 )
				{
					R_WDT_Restart();
					if( gvModuleSts.mRf == ecRfModuleSts_Run && (ecRFDrvInitSts_InitEnd != ApiRFDrv_GetInitSts()) )
					{
						ApiRFDrv_Initial();
					}
					
					/* RF(�����ʐM)�̏��������� */
					if( ecRFDrvInitSts_InitEnd == ApiRFDrv_GetInitSts() )
					{
						gvModuleSts.mRf = ecRfModuleSts_Run;
						break;
					}
				}
				break;
			}
			SubSX1272CarrierSenseFSK_DIO();
		}
	}
}
#else
	while( (P_CTS == imLow) || (rfCurrentCh_LoRa == 0U) )
	{
		R_WDT_Restart();
		wkReturn = 0U;
		SubSX1272Write( REG_RXBW, RF_RXBW_MANT_20 | RF_RXBW_EXP_2 );
		for( wkRadio = 0U; wkRadio < 5U; wkRadio++ )
		{
			SubSX1272FreqSet( wkCH - 2U + wkRadio, rfFreqOffset );
			if(SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == SENS_CARRIER)
			{
				wkReturn = 1U;
				break;
			}
		}
		
		SubSX1272Write( REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_1 );
		if( wkReturn == 0U )
		{
			SubSX1272FreqSet( wkCH, rfFreqOffset );
			SubSX1272TxFSK_DIO_240kbps();
			SubSX1272WriteFifo( (uint8_t *)PN9, 64U );
//			SubSX1272WriteFifo( &PN9, 64U );
			SubSX1272Tx( 64U );
			gvRfIntFlg.mRadioDio0 = 0U;
			R_INTC6_Start();
			while( !gvRfIntFlg.mRadioDio0 )
			{
				R_WDT_Restart();
			}
			R_INTC6_Stop();
			SubSX1272Stdby();
			SubSX1272CarrierSenseFSK_DIO();
		}
		
	}
	gvutComBuff.umModInf.mRcvPos = 0U;
	vRFPrm.mInitSts = ecRFDrvInitSts_Init;
	gvModuleSts.mRf = ecRfModuleSts_Run;
	while( 1 )
	{
		if( gvModuleSts.mRf == ecRfModuleSts_Run && (ecRFDrvInitSts_InitEnd != ApiRFDrv_GetInitSts()) )
		{
			ApiRFDrv_Initial();
		}
		
		/* RF(�����ʐM)�̏��������� */
		if( ecRFDrvInitSts_InitEnd == ApiRFDrv_GetInitSts() )
		{
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
		}
	}
}
#endif
#endif

#if (swRfTxTest == imDisable) && (swRfTxTest == imDisable)
/*
*******************************************************************************
*	Sync�֗��p���ID,�O���[�vID���Z�b�g
*
*	[���e]
*		�I�����C���F�ؗL���ɉ�����Sync�֗��p���ID,�O���[�vID���Z�b�g
*�@�@6byte SX1272�փ��W�X�^��������
* [����]
*	�@wkMyID���g�p
*	�I�����C���F�؏��
*	GW : vRfDrvMainGwSts��� �R���p�C���X�C�b�`�Ő؂�ւ�
*	Logger : vRfDrvMainLoggerSts��� �R���p�C���X�C�b�`�Ő؂�ւ�
*******************************************************************************
*/
void SubRFDrv_SyncSet( void )
{
	uint8_t wkSync[6U];
	uint8_t wkDefAppID[2U];
	
	wkDefAppID[0U] = APP_ID_LOGGER_INIT_UP;
	wkDefAppID[1U] = APP_ID_LOGGER_INIT_LO;
	
#if (swLoggerBord == imEnable)
	switch( vRfDrvMainLoggerSts )
	{
		case ecRfDrvMainSts_HSmode_BeaconCsInit:
		case ecRfDrvMainSts_HSmode_BeaconPre1stDetect:
		case ecRfDrvMainSts_HSmode_BeaconPre2ndDetect:
		case ecRfDrvMainSts_HSmode_BeaconSync2ndDetect:
			if( (vrfMySerialID.mAppID[0U] == ( ((wkDefAppID[0U] << 4U) & 0xF0) + ((wkDefAppID[1U] & 0xF0) >> 4U) ))
				&& (vrfMySerialID.mAppID[1U] == ((wkDefAppID[1U] << 4U) & 0xF0)) )
			{
				/* AppID�������l�̏ꍇ */
				wkSync[0U] = 0x08;
				wkSync[1U] = 0x0E;
				wkSync[2U] = 0x02;
				wkSync[3U] = 0x06;
				wkSync[4U] = 0x02;
				wkSync[5U] = 0x21;
				wkSync[0U] = ( (wkDefAppID[0U] << 4U) & 0xF0) | wkSync[0U];
				wkSync[1U] = ( wkDefAppID[1U] & 0xF0) | wkSync[1U];
				wkSync[2U] = ( (wkDefAppID[1U] << 4U) & 0xF0) | wkSync[2U];
				wkSync[3U] = ( vrfMySerialID.mOpeCompanyID & 0xF0 ) | wkSync[3U];
				wkSync[4U] = ( (vrfMySerialID.mOpeCompanyID << 4U) & 0xF0 ) | wkSync[4U];
			}
			else
			{
				if( gvInFlash.mParam.mOnCertLmt == CENTRAL )
				{
					if( gvrfStsEventFlag.mHsmodeSyncDetect )
					{
						wkSync[0U] = 0x01;
						wkSync[1U] = 0x09;
						wkSync[2U] = 0x05;
						wkSync[3U] = 0x06;
						wkSync[4U] = 0x06;
						wkSync[5U] = 0x71;
					}
					else
					{
						wkSync[0U] = 0x06;
						wkSync[1U] = 0x01;
						wkSync[2U] = 0x02;
						wkSync[3U] = 0x01;
						wkSync[4U] = 0x07;
						wkSync[5U] = 0x7C;
					}
				}
				else
				{
					if( gvrfStsEventFlag.mHsmodeSyncDetect )
					{
						wkSync[0U] = 0x09;
						wkSync[1U] = 0x06;
						wkSync[2U] = 0x04;
						wkSync[3U] = 0x05;
						wkSync[4U] = 0x09;
						wkSync[5U] = 0x17;
					}
					else
					{
						wkSync[0U] = 0x07;
						wkSync[1U] = 0x02;
						wkSync[2U] = 0x01;
						wkSync[3U] = 0x02;
						wkSync[4U] = 0x0E;
						wkSync[5U] = 0xEC;
					}
				}
				wkSync[0U] = (vrfMySerialID.mOpeCompanyID & 0xF0) | wkSync[0U];
				wkSync[1U] = ((vrfMySerialID.mOpeCompanyID << 4U) & 0xF0) | wkSync[1U];
				wkSync[2U] = (vrfMySerialID.mAppID[0U] & 0xF0) | wkSync[2U];
				wkSync[3U] = ((vrfMySerialID.mAppID[0U] << 4U) & 0xF0) | wkSync[3U];
				wkSync[4U] = (vrfMySerialID.mAppID[1U] & 0xF0) | wkSync[4U];
			}
			
			break;
			
//		case ecRfDrvMainSts_HSmode_BeaconResTxWait:
		case ecRfDrvMainSts_HSmode_BeaconResTxStart:
		/* �֐����s��ecRfDrvMainSts_HSmode_BeaconRxStart��case�� */
			if( (vrfMySerialID.mAppID[0U] == ( ((wkDefAppID[0U] << 4U) & 0xF0) + ((wkDefAppID[1U] & 0xF0) >> 4U) ))
				&& (vrfMySerialID.mAppID[1U] == ((wkDefAppID[1U] << 4U) & 0xF0)) )
			{
				/* AppID�������l�̏ꍇ */
				wkSync[0U] = 0x04;
				wkSync[1U] = 0x07;
				wkSync[2U] = 0x04;
				wkSync[3U] = 0x0E;
				wkSync[4U] = 0x02;
				wkSync[5U] = 0xE1;
				wkSync[0U] = ( (wkDefAppID[0U] << 4U) & 0xF0) | wkSync[0U];
				wkSync[1U] = ( wkDefAppID[1U] & 0xF0) | wkSync[1U];
				wkSync[2U] = ( (wkDefAppID[1U] << 4U) & 0xF0) | wkSync[2U];
				wkSync[3U] = ( vrfMySerialID.mOpeCompanyID & 0xF0 ) | wkSync[3U];
				wkSync[4U] = ( (vrfMySerialID.mOpeCompanyID << 4U) & 0xF0 ) | wkSync[4U];
			}
			else
			{
				if( gvInFlash.mParam.mOnCertLmt == CENTRAL )
				{
					wkSync[0U] = 0x01;
					wkSync[1U] = 0x0E;
					wkSync[2U] = 0x02;
					wkSync[3U] = 0x07;
					wkSync[4U] = 0x08;
					wkSync[5U] = 0x71;
				}
				else
				{
					wkSync[0U] = 0x02;
					wkSync[1U] = 0x07;
					wkSync[2U] = 0x01;
					wkSync[3U] = 0x0E;
					wkSync[4U] = 0x04;
					wkSync[5U] = 0x17;
				}
				wkSync[0U] = (vrfCallID[0U] & 0xF0) | wkSync[0U];
				wkSync[1U] = ((vrfCallID[0U] << 4U) & 0xF0) | wkSync[1U];
				wkSync[2U] = (vrfCallID[1U] & 0xF0) | wkSync[2U];
				wkSync[3U] = ((vrfCallID[1U] << 4U) & 0xF0) | wkSync[3U];
				wkSync[4U] = (vrfCallID[2U] & 0xF0) | wkSync[4U];
				/* [3U]�̉���4bit��NULL */
			}
			
			break;
			
		default:
			break;
	}
	
#else
	switch( vRfDrvMainGwSts)
	{
		case ecRfDrvMainSts_HSmode_BeaconInit:
//		case ecRfDrvMainSts_HSmode_BeaconTxStart:
		case ecRfDrvMainSts_HSmode_BeaconTxCs:
			/* ApiRFDrv_LoggerBroadCastFSK�֐� */
/*
#if (swLoggerBord == imEnable)
			if( gvInFlash.mParam.mOnCertLmt == CENTRAL )
			{
				wkSync[0U] = 0x01;
				wkSync[1U] = 0x09;
				wkSync[2U] = 0x05;
				wkSync[3U] = 0x06;
				wkSync[4U] = 0x06;
				wkSync[5U] = 0x71;
			}
			else
			{
				wkSync[0U] = 0x09;
				wkSync[1U] = 0x06;
				wkSync[2U] = 0x04;
				wkSync[3U] = 0x05;
				wkSync[4U] = 0x09;
				wkSync[5U] = 0x17;
			}
			wkSync[0U] = (vrfMySerialID.mOpeCompanyID & 0xF0) | wkSync[0U];
			wkSync[1U] = ((vrfMySerialID.mOpeCompanyID << 4U) & 0xF0) | wkSync[1U];
			wkSync[2U] = (vrfMySerialID.mAppID[0U] & 0xF0) | wkSync[2U];
			wkSync[3U] = ((vrfMySerialID.mAppID[0U] << 4U) & 0xF0) | wkSync[3U];
			wkSync[4U] = (vrfMySerialID.mAppID[1U] & 0xF0) | wkSync[4U];
#else
*/
			if( gvInFlash.mParam.mrfTorokuMode == LOGGER_TOROKU_ON )
			{
				wkSync[0U] = 0x08;
				wkSync[1U] = 0x0E;
				wkSync[2U] = 0x02;
				wkSync[3U] = 0x06;
				wkSync[4U] = 0x02;
				wkSync[5U] = 0x21;
				wkSync[0U] = ( (wkDefAppID[0U] << 4U) & 0xF0) | wkSync[0U];
				wkSync[1U] = ( wkDefAppID[1U] & 0xF0) | wkSync[1U];
				wkSync[2U] = ( (wkDefAppID[1U] << 4U) & 0xF0) | wkSync[2U];
				wkSync[3U] = ( vrfMySerialID.mOpeCompanyID & 0xF0 ) | wkSync[3U];
				wkSync[4U] = ( (vrfMySerialID.mOpeCompanyID << 4U) & 0xF0 ) | wkSync[4U];
			}
			else
			{
				if( gvInFlash.mParam.mOnCertLmt == CENTRAL )
				{
					if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Status )
					{
						wkSync[0U] = 0x01;
						wkSync[1U] = 0x09;
						wkSync[2U] = 0x05;
						wkSync[3U] = 0x06;
						wkSync[4U] = 0x06;
						wkSync[5U] = 0x71;
					}
					else
					{
						wkSync[0U] = 0x06;
						wkSync[1U] = 0x01;
						wkSync[2U] = 0x02;
						wkSync[3U] = 0x01;
						wkSync[4U] = 0x07;
						wkSync[5U] = 0x7C;
					}
				}
				else
				{
					if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Status )
					{
						wkSync[0U] = 0x09;
						wkSync[1U] = 0x06;
						wkSync[2U] = 0x04;
						wkSync[3U] = 0x05;
						wkSync[4U] = 0x09;
						wkSync[5U] = 0x17;
					}
					else
					{
						wkSync[0U] = 0x07;
						wkSync[1U] = 0x02;
						wkSync[2U] = 0x01;
						wkSync[3U] = 0x02;
						wkSync[4U] = 0x0E;
						wkSync[5U] = 0xEC;
					}
				}
				wkSync[0U] = (vrfMySerialID.mOpeCompanyID & 0xF0) | wkSync[0U];
				wkSync[1U] = ((vrfMySerialID.mOpeCompanyID << 4U) & 0xF0) | wkSync[1U];
				wkSync[2U] = (vrfMySerialID.mAppID[0U] & 0xF0) | wkSync[2U];
				wkSync[3U] = ((vrfMySerialID.mAppID[0U] << 4U) & 0xF0) | wkSync[3U];
				wkSync[4U] = (vrfMySerialID.mAppID[1U] & 0xF0) | wkSync[4U];
			}
/*
#endif
*/
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconResRxStart:
		case ecRfDrvMainSts_HSmode_BeaconResAckTxStart:
			/* SubRFDrv_CallResponseRecieve�֐� */
/*
#if (swLoggerBord == imEnable)
			if( gvInFlash.mParam.mOnCertLmt == CENTRAL )
			{
				wkSync[0U] = 0x01;
				wkSync[1U] = 0x0E;
				wkSync[2U] = 0x02;
				wkSync[3U] = 0x07;
				wkSync[4U] = 0x08;
				wkSync[5U] = 0x71;
			}
			else
			{
				wkSync[0U] = 0x02;
				wkSync[1U] = 0x07;
				wkSync[2U] = 0x01;
				wkSync[3U] = 0x0E;
				wkSync[4U] = 0x04;
				wkSync[5U] = 0x17;
			}
			wkSync[0U] = (vrfMySerialID.mUniqueID[0U] & 0xF0) | wkSync[0U];
			wkSync[1U] = ((vrfMySerialID.mUniqueID[0U] << 4U) & 0xF0) | wkSync[1U];
			wkSync[2U] = (vrfMySerialID.mUniqueID[1U] & 0xF0) | wkSync[2U];
			wkSync[3U] = ((vrfMySerialID.mUniqueID[1U] << 4U) & 0xF0) | wkSync[3U];
			wkSync[4U] = (vrfMySerialID.mUniqueID[2U] & 0xF0) | wkSync[4U];
#else
*/
#if 0
			if( gvInFlash.mParam.mrfTorokuMode == LOGGER_TOROKU_ON )
			{
				wkSync[0U] = 0x04;
				wkSync[1U] = 0x07;
				wkSync[2U] = 0x04;
				wkSync[3U] = 0x0E;
				wkSync[4U] = 0x02;
				wkSync[5U] = 0xE1;
				wkSync[0U] = ( (wkDefAppID[0U] << 4U) & 0xF0) | wkSync[0U];
				wkSync[1U] = ( wkDefAppID[1U] & 0xF0) | wkSync[1U];
				wkSync[2U] = ( (wkDefAppID[1U] << 4U) & 0xF0) | wkSync[2U];
				wkSync[3U] = ( vrfMySerialID.mOpeCompanyID & 0xF0 ) | wkSync[3U];
				wkSync[4U] = ( (vrfMySerialID.mOpeCompanyID << 4U) & 0xF0 ) | wkSync[4U];
			}
			else
			{
				if( gvInFlash.mParam.mOnCertLmt == CENTRAL ){
					wkSync[0U] = 0x01;
					wkSync[1U] = 0x0E;
					wkSync[2U] = 0x02;
					wkSync[3U] = 0x07;
					wkSync[4U] = 0x08;
					wkSync[5U] = 0x71;
					wkSync[0U] = (vrfMySerialID.mUniqueID[0U] & 0xF0) | wkSync[0U];
					wkSync[1U] = ((vrfMySerialID.mUniqueID[0U] << 4U) & 0xF0) | wkSync[1U];
					wkSync[2U] = (vrfMySerialID.mUniqueID[1U] & 0xF0) | wkSync[2U];
					wkSync[3U] = ((vrfMySerialID.mUniqueID[1U] << 4U) & 0xF0) | wkSync[3U];
					wkSync[4U] = (vrfMySerialID.mUniqueID[2U] & 0xF0) | wkSync[4U];
					/* [0U]�̏��4bit��NULL */
				}
				else
				{
					wkSync[0U] = 0x02;
					wkSync[1U] = 0x07;
					wkSync[2U] = 0x01;
					wkSync[3U] = 0x0E;
					wkSync[4U] = 0x04;
					wkSync[5U] = 0x17;
					wkSync[0U] = (vrfMySerialID.mUniqueID[0U] & 0xF0) | wkSync[0U];
					wkSync[1U] = ((vrfMySerialID.mUniqueID[0U] << 4U) & 0xF0) | wkSync[1U];
					wkSync[2U] = (vrfMySerialID.mUniqueID[1U] & 0xF0) | wkSync[2U];
					wkSync[3U] = ((vrfMySerialID.mUniqueID[1U] << 4U) & 0xF0) | wkSync[3U];
					wkSync[4U] = (vrfMySerialID.mUniqueID[2U] & 0xF0) | wkSync[4U];
				}
			}
#endif
			if( gvInFlash.mParam.mOnCertLmt == CENTRAL ){
				wkSync[0U] = 0x01;
				wkSync[1U] = 0x0E;
				wkSync[2U] = 0x02;
				wkSync[3U] = 0x07;
				wkSync[4U] = 0x08;
				wkSync[5U] = 0x71;
			}
			else
			{
				wkSync[0U] = 0x02;
				wkSync[1U] = 0x07;
				wkSync[2U] = 0x01;
				wkSync[3U] = 0x0E;
				wkSync[4U] = 0x04;
				wkSync[5U] = 0x17;
			}
				wkSync[0U] = (vrfMySerialID.mUniqueID[0U] & 0xF0) | wkSync[0U];
				wkSync[1U] = ((vrfMySerialID.mUniqueID[0U] << 4U) & 0xF0) | wkSync[1U];
				wkSync[2U] = (vrfMySerialID.mUniqueID[1U] & 0xF0) | wkSync[2U];
				wkSync[3U] = ((vrfMySerialID.mUniqueID[1U] << 4U) & 0xF0) | wkSync[3U];
				wkSync[4U] = (vrfMySerialID.mUniqueID[2U] & 0xF0) | wkSync[4U];
			/* [0U]�̏��4bit��NULL */
			break;
			
		default:
			break;
	}
#endif
	
	SubSX1272FSK_Sync( &wkSync[0U] );
}
#endif


#if (swLoggerBord == imEnable)
/*
********************************************************************************
* CALL�M���̃v���A���u�����o�֐�
*
* [���e]
*  ���m������RX�p��/�����o����SLEEP
* [Return]
*  SUCCESS / FAIL
********************************************************************************
*/
uint8_t SubRFDrv_Wakeup_PreDetect(void)
{
	uint8_t wkReturn = FAIL;
	
	SubSX1272RxFSK_On();
	/* SLEEP���A�|�[�g���荞��or�^�C�}���荞�݂�WAKE UP */
	SubIntervalTimer_Sleep( _PREAMBLE_WAIT_ITMCP_VALUE, 1U );
	gvRfIntFlg.mRadioDio4 = 0U;
	R_INTC1_Start();
	
	while( !gvRfIntFlg.mRadioTimer && !gvRfIntFlg.mRadioDio4 )
	{
		M_HALT;
	}
	
	/* Pre���o�`�F�b�N */
	if( gvRfIntFlg.mRadioTimer ){	SubIntervalTimer_Stop(); }
	R_INTC1_Stop();
	if( gvRfIntFlg.mRadioDio4 )
	{
		/* Preamble���m RSSI�l�擾 */
		wkReturn = HS_MODE_PREAMBLE_SUCCESS;
	}
	else
	{
		SubSX1272Sleep();
		wkReturn = HS_MODE_PREAMBLE_FAIL;
	}
	
	return	wkReturn;
}


/*
********************************************************************************
* CALL�M����Sync���o�֐�
*
* [���e]
*  ���m������RX�p��/�����o����SLEEP
*  RSSI�͕K���擾����
* [Return]
*  SUCCESS / FAIL
********************************************************************************
*/
uint8_t SubRFDrv_Wakeup_SyncDetect( void )
{
//	uint8_t wkFunc;
	uint8_t wkReturn = HS_MODE_SYNC_FAIL;
	/* ------------SyncWord�҂�------------ */
	/* SLEEP���A�|�[�g���荞��or�^�C�}���荞��3.44msec��WAKE UP */
	gvRfIntFlg.mRadioDio2 = 0U;
	gvRfIntFlg.mRadioTimer = 0U;
	R_INTC3_Start();
	SubIntervalTimer_Sleep( _SYNC_WAIT_ITMCP_VALUE, 1U );			/* 1.376msec */
	
	while( !(gvRfIntFlg.mRadioDio2 | gvRfIntFlg.mRadioTimer) )
	{
		M_HALT;
	}
	
	/* Sync���o�`�F�b�N */
	R_INTC3_Stop();
	SubIntervalTimer_Stop();
	if( gvRfIntFlg.mRadioDio2 )
	{
		/* sync���m */
		wkReturn = HS_MODE_SYNC_SUCCESS;
		vrfCurrentRssi = SubRfDrv_GetRssi();
	}
	else
	{
		SubSX1272Sleep();
		wkReturn = HS_MODE_SYNC_FAIL;
	}
	
	return	wkReturn;
}


/*
*******************************************************************************
*	CALL�M���ɑ΂��鉞���^�C�~���O����
*
*	[���e]
*		ApiRFDrv_Wakeup��̉����M���^�C�~���O���Z�o����B
*		���K�[���̏���
*		rfRxData.Countdown�l���͂��K�v(��M���Ɏ擾)
*		rfMyID.UniqueID[2U]���K�v
* [����]
*		wkTimeSlotGrupe�@�F�@1U�`9U / 0msec,100msec,150msec ....
*		wkTimeSlotNum		:	0U�`3U / 0msec,9msec,18msec,27msec....
* [return]
*		��M�J�E���g�l����Z�o���������^�C�~���O
*******************************************************************************
*/
uint16_t SubRfDrv_CallResponseTimeslot( uint8_t *wkTimeSlotGrupe, uint8_t *wkTimeSlotNum)
{
	uint16_t	wkReturn;
/*
#if 0
	wkReturn = rfRxData.Countdown[ 0U ];
	wkReturn = (wkReturn << 8U) & 0xFF00;
	wkReturn = wkReturn + rfRxData.Countdown[ 1U ];
	
	srand( rfMyID.UniqueID[2U] + gvClock.mSec );
	*wkTimeSlotGrupe = (rand() % 9U) + 1U;
	*wkTimeSlotNum = rand() % 4U;
#endif
*/
//	wkReturn = RESPONSE_COUNT[ 20U - vutRfDataBuffer.umGwQueryBeacon.mWaitCount[0U] ];
	wkReturn = RESPONSE_COUNT[ 19U - vutRfDataBuffer.umGwQueryBeacon.mWaitCount[0U] ];
	srand( vrfMySerialID.mUniqueID[2U] + gvClock.mSec );
	*wkTimeSlotGrupe = (rand() % 3U) + 1U;
//	*wkTimeSlotNum = rand() % 6U;
	*wkTimeSlotNum = rand() % 5U;
	
	return wkReturn;
}


/*
*******************************************************************************
*	�������[�h�@���K�[�����f�[�^�̃Z�b�g
*
*	[���e]
*		�������[�h�Ńr�[�R������M���A���̉�����Ԃ��f�[�^���Z�b�g,����������B
*		�t���b�V��������̃f�[�^�Z�b�g�́A�{�֐��̑O��main���[�`���ŃZ�b�g����B
*		�v���l���̘A���Ńf�[�^�𑗐M����ۂ��Ăяo�����
* [����]
*
* [return]
*
*******************************************************************************
*/
void SubRFDrv_Hsmode_Logger_ResponceDataSet( uint8_t arPacket )
{
	uint8_t wkRadio;
//	uint8_t wkRadio2nd;
	uint16_t wkCulc;
//	uint8_t wkCrcLoop;
	uint8_t *pwkFlashData;		/* Flash�ێ��ݒ�l */
	uint8_t *pwkRfData;			/* ��������M�ݒ�l */
	uint8_t *pwkEndAdr;			/* �ݒ�l�G���h�A�h���X */
	uint16_t wkLoop;
	uint16_t wkEndIndex;
	uint16_t wkSleep_index;
	uint32_t wkStaIndex;
	
	switch( vrfHsReqCmdCode )
	{
		case ecRfDrvGw_Hs_ReqCmd_Status:
		case ecRfDrvGw_Hs_ReqCmd_Toroku:
			/* �X�e�[�^�X�������� No21 , �o�^���[�h�ł̃X�e�[�^�X��񉞓� */
			/* �X�e�[�^�X�v�� or �o�^���[�h�ɂ�菈���ύX */
			if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Status )
			{
				vutRfDataBuffer.umLoggerResStatus.mPacketCode = 0x01;
			}
			else if( ( ((vrfMySerialID.mAppID[0U] >> 4U) & 0x0F) == APP_ID_LOGGER_INIT_UP )
						&& ( (((vrfMySerialID.mAppID[0U] << 4U) & 0xF0) + ((vrfMySerialID.mAppID[1U] >> 4U) & 0x0F)) == APP_ID_LOGGER_INIT_LO ) )
			{
				/* GW��AppID��o�^ */
				vutRfDataBuffer.umLoggerResStatus.mPacketCode = 0x0C;
				memcpy( &vrfMySerialID.mAppID[ 0U ], &vutRfDataBuffer.umGwQueryBeacon.mAppID[ 0U ], 2U );
				gvInFlash.mParam.mAppID[ 0U ] = (vutRfDataBuffer.umGwQueryBeacon.mAppID[0U] >> 4U) & 0x0F;
				gvInFlash.mParam.mAppID[ 1U ] = ((vutRfDataBuffer.umGwQueryBeacon.mAppID[0U] << 4U) & 0xF0) + ((vutRfDataBuffer.umGwQueryBeacon.mAppID[1U] >> 4U) & 0x0F);
				vrfHsmodeSleepMode = 0x00;			/* �o�^����̓X���[�v�����ɃX�e�[�^�X��Ԃ� */
				/* ���[�J�����[�h�̏ꍇ�A�ڑ���GW��o�^ */
				if( (vrfMySerialID.mAppID[0U] == APP_ID_LOCAL_UP) && (vrfMySerialID.mAppID[1U] == APP_ID_LOCAL_LO) )
				{
					gvInFlash.mParam.mOnCertLmt = LOCAL;
					memcpy( &gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 0U ], &vrfCallID[ 0U ], 3U );
					gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
				}
				else
				{
					gvInFlash.mParam.mOnCertLmt = CENTRAL;
				}
				gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
			}
			else
			{
				/* GW���o�^���[�h�������ɓo�^�ς� �������o�^���[�h����AppID���قȂ邽�߁A�����ɂ͗��Ȃ� */
				vutRfDataBuffer.umLoggerResStatus.mPacketCode = 0x0D;
				SubRFDrv_Hsmode_End();
				
				ApiFlash_WriteSysLog( ecSysLogItm_IdDiff, 4U );				/* �V�X�e�������FID���������̗����ɑ���� */
				
				break;
			}
			
			vrfHsmodePacketLength = RF_HSMODE_RES_STATUS_LENGTH;
			
			/* ���K�[ID�ƈ���GW ID���Z�b�g */
			memcpy( &vutRfDataBuffer.umLoggerResStatus.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
			
			/* ����GWID */
			memcpy( &vutRfDataBuffer.umLoggerResStatus.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
			
			/* �O���[�vID */
			vutRfDataBuffer.umLoggerResStatus.mGroupID = vrfMySerialID.mGroupID;
			
			/* ���K�[���� */
			memcpy( &vutRfDataBuffer.umLoggerResStatus.mLoggerName[ 0U ], &gvInFlash.mParam.mLoggerName[ 0U ], imLoggerNameNum );
			
			/* �Z���T�^�C�v */
//			vutRfDataBuffer.umLoggerResStatus.mSensorType = cSensType;
			vutRfDataBuffer.umLoggerResStatus.mSensorType = gvInFlash.mProcess.mModelCode;
			
			/* �o�[�W���� */
			vutRfDataBuffer.umLoggerResStatus.mVersion[ 0U ] = cRomVer[ 0U ];
			vutRfDataBuffer.umLoggerResStatus.mVersion[ 1U ] = cRomVer[ 1U ] * 10 + cRomVer[ 2U ];
			vutRfDataBuffer.umLoggerResStatus.mVersion[ 2U ] = cRomVer[ 3U ];
			
			/* �ُ�X�e�[�^�X�A�d�r�c�ʏ�� */
			ApiFlash_RfRTAbnStatusSet();
			
			/* �v�������̒l���Z�b�g */
			if( gvHsMeasAlmFlg == imOFF )
			{
				/* �ŐV�f�[�^���� */
				memcpy( &vutRfDataBuffer.umLoggerResStatus.mStatusTime[ 0U ], &gvRfRTMeasData.mRTMeasData_MeasTime[ 0U ], 4U );
				
				/* �ŐV�v���l�Ƙ_�� */
				memcpy( &vutRfDataBuffer.umLoggerResStatus.mStatusMeasure_AllCh_Event[ 0U ], &gvRfRTMeasData.mRTMeasData_MeasVal[ 0U ], 5U );
				
				/* �ُ�X�e�[�^�X�A�d�r�c�ʏ�� */
				vutRfDataBuffer.umLoggerResStatus.mStatusAbnormal = gvRfRTMeasData.mRTMeasData_AbnStatus & 0xC7U;
			}
			/* �x��Ď������̒l���Z�b�g */
			else
			{
				/* �ŐV�f�[�^���� */
				memcpy( &vutRfDataBuffer.umLoggerResStatus.mStatusTime[ 0U ], &gvRfRTMeasAlm2Data.mRTMeasData_MeasTime[ 0U ], 4U );
				
				/* �ŐV�v���l�Ƙ_�� */
				memcpy( &vutRfDataBuffer.umLoggerResStatus.mStatusMeasure_AllCh_Event[ 0U ], &gvRfRTMeasAlm2Data.mRTMeasData_MeasVal[ 0U ], 5U );
				
				/* �ُ�X�e�[�^�X�A�d�r�c�ʏ�� */
				vutRfDataBuffer.umLoggerResStatus.mStatusAbnormal = (gvRfRTMeasData.mRTMeasData_AbnStatus & 0xC7U) + 0x08U;
				
				/* �ŐV�x��t���O */
				ApiFlash_RfRTAlarmFlagSet( gvMeasPrm.mAlmFlg, gvMeasPrm.mDevFlg, gvRfRTMeasData.mRTMeasData_AlarmFlag );
				for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
				{
					gvHsAlmFlag[ wkLoop ] = gvMeasPrm.mDevFlg[ wkLoop ];
					gvHsAlmFlag[ wkLoop ] |= gvMeasPrm.mAlmFlg[ wkLoop ] << 4U;
				}
			}
			
			
			/* �ŐV�x��t���O */
			memcpy( &vutRfDataBuffer.umLoggerResStatus.mAlart_AllCh[ 0U ], &gvHsAlmFlag[ 0U ], 4U );
			
			/* ���p���ID */
			vutRfDataBuffer.umLoggerResStatus.mAppID[ 0U ] = (vrfMySerialID.mAppID[ 0U ] >> 4U) & 0x0F;
			vutRfDataBuffer.umLoggerResStatus.mAppID[ 1U ] = ((vrfMySerialID.mAppID[ 0U ] << 4U) & 0xF0) + ((vrfMySerialID.mAppID[ 1U ] >> 4U) & 0x0F);
			
			/* �ݒ�ύX���� */
			memcpy( &vutRfDataBuffer.umLoggerResStatus.mSetTime[ 0U ], &gvInFlash.mParam.mSetvalChgTime[ 0U ], 4U );
			
			/* �����ʐM�Ɏ��n��ID�ǉ� */
			SubRfDrv_SetStatusAdd( &vutRfDataBuffer.umLoggerResStatus.mMeasId.mNewDataId[ 0U ] );
			
			/* �\�� */
			vutRfDataBuffer.umLoggerResStatus.mReserve[ 0U ] = 0x00U;
			vutRfDataBuffer.umLoggerResStatus.mReserve[ 1U ] = 0x00U;
			
			SubRfDrv_Coding_HS( &vutRfDataBuffer.umData[ 0U ], RF_HSMODE_RES_STATUS_LENGTH, BCH_OFF );
			break;
			
		/* �v���l���� No22 */
		case ecRfDrvGw_Hs_ReqCmd_Measure:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_MEASURE_LENGTH;
			
			/* ���M�p�P�b�g�����Z�b�g */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 90U );
			
			/* Flash���瑗�M�f�[�^�̃��[�h���� */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_Meas1 ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResMeasure.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResMeasure.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
//				vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode = (vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode & 0xF0) + 0x02;	/* 0x02 ecRfDrvGw_Hs_ReqCmd_Measure */
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_MEASURE_LENGTH_HEADER - 8U, BCH_ON );
				SubRfDrv_Payload_Cording( 10U, 10U, 9U, RF_HSMODE_RES_MEASURE_LENGTH_HEADER );
				
				/* ����(1�p�P�b�g��)�f�[�^�Z�b�g */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* ���M�f�[�^��90���� */
					if( vutRfDataBuffer.umLoggerResMeasure.mDataNum < 90U )
					{
						/* Flash���[�h���� */
						vrfPacketSum = 2U;
					}
					
					/* ���^�f�[�^���M��ASleep�ƂȂ�IndexNo.���Z�o */
					if( gvInFlash.mParam.mFstConnBoot <= gvInFlash.mData.mMeas1_Index )
					{
						wkSleep_index = gvInFlash.mData.mMeas1_Index + 1U - gvInFlash.mParam.mFstConnBoot;
					}
					else
					{
						if( gvInFlash.mParam.mFstConnBoot <= gvInFlash.mData.mMeas1_Num )
						{
							wkSleep_index = imIndexMaxMeas1 + 1U - (gvInFlash.mParam.mFstConnBoot - gvInFlash.mData.mMeas1_Index);
						}
						else
						{
							wkSleep_index = 1U;
						}
					}
					
					/* �����ʐM�X���[�v�J�ڂ̏����Z�o */
					vSleep_Sts = 0U;
					vPrePacket_EndIndex = 0U;
					for( wkLoop = 0U ; wkLoop <= 589U ; wkLoop++ )
					{
						wkStaIndex = (uint32_t)vMem_RdIndexNo + (wkLoop * 90U);
						if( wkStaIndex > imIndexMaxMeas1 )
						{
							wkStaIndex -= imIndexMaxMeas1;
						}
						wkEndIndex = wkStaIndex + 89U;
						if( wkEndIndex > imIndexMaxMeas1 )
						{
							wkEndIndex -= imIndexMaxMeas1;
						}
						
						if( gvInFlash.mData.mMeas1_Num != imDataNumMeas1 && wkSleep_index < wkStaIndex )
						{
							vSleepPacket_StaIndex = wkStaIndex;
							break;
						}
						
						
						if( wkStaIndex <= wkEndIndex )
						{
							if( wkStaIndex <= wkSleep_index && wkSleep_index <= wkEndIndex )
							{
								vSleepPacket_StaIndex = wkStaIndex;
								break;
							}
						}
						else
						{
							if( wkSleep_index <= wkEndIndex || (wkStaIndex <= wkSleep_index && wkSleep_index <= imIndexMaxMeas1) )
							{
								vSleepPacket_StaIndex = wkStaIndex;
								break;
							}
						}
						vPrePacket_EndIndex = wkEndIndex;
					}
					
					/* �X���[�vIndexNo.�ƌ��݂�IndexNo.�̊Ԃ̗v���̂Ƃ� */
					if( wkSleep_index <= gvInFlash.mData.mMeas1_Index )
					{
						if( wkSleep_index <= vMem_RdIndexNo && vMem_RdIndexNo <= gvInFlash.mData.mMeas1_Index )
						{
							vSleepPacket_StaIndex = vMem_RdIndexNo;
							vPrePacket_EndIndex = 0U;
						}
					}
					else
					{
						if( wkSleep_index <= vMem_RdIndexNo || vMem_RdIndexNo <= gvInFlash.mData.mMeas1_Index )
						{
							vSleepPacket_StaIndex = vMem_RdIndexNo;
							vPrePacket_EndIndex = 0U;
						}
					}
				}
			}
			else
			{
				/* Flash���[�h�ł��Ȃ��Ƃ��̃G���[���� */
				M_NOP;
			}
			break;
			
		/* �x�񗚗����� No23 */
		case ecRfDrvGw_Hs_ReqCmd_AlartHis:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_ALHIS_LENGTH;
			
			/* ���M�p�P�b�g�����Z�b�g */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 100U );
			
			/* Flash���瑗�M�f�[�^�̃��[�h���� */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_MeasAlm ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResAlarmHist.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResAlarmHist.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
				vutRfDataBuffer.umLoggerResAlarmHist.mPacketCode = 0x03;
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_ALHIS_LENGTH_HEADER, BCH_OFF );
				SubRfDrv_Payload_Cording( 8U, 10U, 10U, RF_HSMODE_RES_ALHIS_LENGTH_HEADER );
				
				/* ����(1�p�P�b�g��)�f�[�^�Z�b�g */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* ���M�f�[�^��100���� */
					if( vutRfDataBuffer.umLoggerResAlarmHist.mDataNum < 100U )
					{
						/* Flash���[�h���� */
						vrfPacketSum = 2U;
					}
				}
			}
			else
			{
				/* Flash���[�h�ł��Ȃ��Ƃ��̃G���[���� */
				M_NOP;
			}
			break;
			
		/* �@��ُ헚������ No24 */
		case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_ABNORMAL_LENGTH;
			
			/* ���M�p�P�b�g�����Z�b�g */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 100U );
			
			/* Flash���瑗�M�f�[�^�̃��[�h���� */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_ErrInfo ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResAbnormalHist.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResAbnormalHist.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
				vutRfDataBuffer.umLoggerResAbnormalHist.mPacketCode = 0x04;
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_ABNORMAL_LENGTH_HEADER, BCH_OFF );
				SubRfDrv_Payload_Cording( 6U, 10U, 10U, RF_HSMODE_RES_ABNORMAL_LENGTH_HEADER );
				
				/* ����(1�p�P�b�g��)�f�[�^�Z�b�g */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* ���M�f�[�^��100���� */
					if( vutRfDataBuffer.umLoggerResAbnormalHist.mDataNum < 100U )
					{
						/* Flash���[�h���� */
						vrfPacketSum = 2U;
					}
				}
			}
			else
			{
				/* Flash���[�h�ł��Ȃ��Ƃ��̃G���[���� */
				M_NOP;
			}
			break;
			
		case ecRfDrvGw_Hs_ReqCmd_EventHis:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_EVENTHIS_LENGTH;
			
			/* ���M�p�P�b�g�����Z�b�g */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 60U );
			
			/* Flash���瑗�M�f�[�^�̃��[�h���� */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_Event ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResEventHist.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResEventHist.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
				vutRfDataBuffer.umLoggerResEventHist.mPacketCode = 0x05;
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_EVENTHIS_LENGTH_HEADER, BCH_OFF );
				SubRfDrv_Payload_Cording( 14U, 10U, 6U, RF_HSMODE_RES_EVENTHIS_LENGTH_HEADER );
				
				/* ����(1�p�P�b�g��)�f�[�^�Z�b�g */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* ���M�f�[�^��60���� */
					if( vutRfDataBuffer.umLoggerResEventHist.mDataNum < 60U )
					{
						/* Flash���[�h���� */
						vrfPacketSum = 2U;
					}
				}
			}
			else
			{
				M_NOP;
			}
			break;

		case ecRfDrvGw_Hs_ReqCmd_OpeHis:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_OPEHIS_LENGTH;
			
			/* ���M�p�P�b�g�����Z�b�g */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 90U );
			
			/* Flash���瑗�M�f�[�^�̃��[�h���� */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_ActLog ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResOpeHist.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResOpeHist.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
				vutRfDataBuffer.umLoggerResOpeHist.mPacketCode = 0x06;
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_OPEHIS_LENGTH_HEADER, BCH_OFF );
				SubRfDrv_Payload_Cording( 10U, 10U, 9U, RF_HSMODE_RES_OPEHIS_LENGTH_HEADER );
				
				/* ����(1�p�P�b�g��)�f�[�^�Z�b�g */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* ���M�f�[�^��90���� */
					if( vutRfDataBuffer.umLoggerResOpeHist.mDataNum < 90U )
					{
						/* Flash���[�h���� */
						vrfPacketSum = 2U;
					}
				}
			}
			else
			{
				M_NOP;
			}
			break;
			
		case ecRfDrvGw_Hs_ReqCmd_SysHis:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_SYSHIS_LENGTH;
			
			/* ���M�p�P�b�g�����Z�b�g */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 90U );
			
			/* Flash���瑗�M�f�[�^�̃��[�h���� */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_SysLog ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResSysHist.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResSysHist.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
				vutRfDataBuffer.umLoggerResSysHist.mPacketCode = 0x07;
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_OPEHIS_LENGTH_HEADER, BCH_OFF );
				SubRfDrv_Payload_Cording( 10U, 10U, 9U, RF_HSMODE_RES_OPEHIS_LENGTH_HEADER );
				
				/* ����(1�p�P�b�g��)�f�[�^�Z�b�g */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* ���M�f�[�^��90���� */
					if( vutRfDataBuffer.umLoggerResSysHist.mDataNum < 90U )
					{
						/* Flash���[�h���� */
						vrfPacketSum = 2U;
					}
				}
			}
			else
			{
				M_NOP;
			}
			break;
			
		/* ���K�[�ݒ�l������ */
		case ecRfDrvGw_Hs_ReqCmd_SetRead:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_SETREAD_LENGTH;
			vrfPacketLimit = 1U;				/* �ݒ�ǂݍ��݂�1�p�P�b�g */
			vrfPacketSum = vrfPacketLimit;
			
			memcpy( &vutRfDataBuffer.umLoggerResSetting.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
			memcpy( &vutRfDataBuffer.umLoggerResSetting.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
			vutRfDataBuffer.umLoggerResSetting.mPacketCode = 0x08;
			
			/* �ݒ�l�𖳐����M�f�[�^�ɓn�� */
			for( wkLoop = 0U ; wkLoop < imChNum ; wkLoop++ )
			{
				/* �����x��l1ch */
				wkCulc = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviUU[ wkLoop ] );
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUUAlarmValue[ 0U ] = wkCulc & 0x00FFU;
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUUAlarmValue[ 1U ] = wkCulc >> 8U;
				
				/* �����x��x��1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUUAlarmDelay = gvInFlash.mParam.mDelayUU[ wkLoop ];
				
				/* ����x��l1ch */
				wkCulc = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviU[ wkLoop ] );
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUAlarmValue[ 0U ] = wkCulc & 0x00FFU;
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUAlarmValue[ 1U ] = wkCulc >> 8U;
				
				/* ����x��x��1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUAlarmDelay = gvInFlash.mParam.mDelayU[ wkLoop ];
				
				/* �����x��l1ch */
				wkCulc = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviL[ wkLoop ] );
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLAlarmValue[ 0U ] = wkCulc & 0x00FFU;
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLAlarmValue[ 1U ] = wkCulc >> 8U;
				
				/* �����x��x��1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLAlarmDelay = gvInFlash.mParam.mDelayL[ wkLoop ];
				
				/* �������x��l1ch */
				wkCulc = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviLL[ wkLoop ] );
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLLAlarmValue[ 0U ] = wkCulc & 0x00FFU;
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLLAlarmValue[ 1U ] = wkCulc >> 8U;
				
				/* �������x��x��1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLLAlarmDelay = gvInFlash.mParam.mDelayLL[ wkLoop ];
				
				/* ��E���e���ԑI��臒l1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mToleranceLevel = gvInFlash.mParam.mDeviEnaLv[ wkLoop ];
				
				/* ��E���e����1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mToleranceTime[ 0U ] = gvInFlash.mParam.mDeviEnaTime[ wkLoop ] & 0x00FFU;
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mToleranceTime[ 1U ] = gvInFlash.mParam.mDeviEnaTime[ wkLoop ] >> 8U;
				
				/* �v���l�I�t�Z�b�g1ch */
				wkCulc = ApiInFlash_OftVal_ValtoCnt( gvInFlash.mParam.mOffset[ wkLoop ] );
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mMeasureOffset = (uint8_t)wkCulc;
				
				/* �v���l�ꎟ�X���␳1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mMeasureZeroSpan = gvInFlash.mParam.mSlope[ wkLoop ] - 70;
			}
			
			/* �d�r�킩��ڑ�����GW�܂ł���C�ɃZ�b�g */
			pwkFlashData = (uint8_t *)&gvInFlash.mParam.mBatType;
			pwkRfData = (uint8_t *)&vutRfDataBuffer.umLoggerResSetting.mBattCode;
			pwkEndAdr = (uint8_t *)&vutRfDataBuffer.umLoggerResSetting.mCommGwID[ 9U ][ 2U ];
			for(  ; pwkRfData <= pwkEndAdr ; pwkFlashData++, pwkRfData++ )
			{
				*pwkRfData = *pwkFlashData;
			}
			
			/* �X�P�[�����O�����ϊ����čăZ�b�g */
			wkCulc = ApiInFlash_ScaleVal_ValtoCnt( gvInFlash.mParam.mScalYInt );
			vutRfDataBuffer.umLoggerResSetting.mScalYInt[ 0U ] = wkCulc & 0x00FFU;
			vutRfDataBuffer.umLoggerResSetting.mScalYInt[ 1U ] = wkCulc >> 8U;
			
			wkCulc = ApiInFlash_ScaleVal_ValtoCnt( gvInFlash.mParam.mScalGrad );
			vutRfDataBuffer.umLoggerResSetting.mScalGrad[ 0U ] = wkCulc & 0x00FFU;
			vutRfDataBuffer.umLoggerResSetting.mScalGrad[ 1U ] = wkCulc >> 8U;
			
			/* �O���[�vID */
			vutRfDataBuffer.umLoggerResSetting.mGroupID = gvInFlash.mParam.mGroupID;
			
			/* �ݒ�ύX���� */
			memcpy( &vutRfDataBuffer.umLoggerResSetting.mSetTime[ 0U ], &gvInFlash.mParam.mSetvalChgTime[ 0U ], 4U );
			
			SubRfDrv_Coding_HS( &vutRfDataBuffer.umData[0U], RF_HSMODE_RES_SETREAD_LENGTH_HEADER, BCH_OFF );
			
			CRCD = 0x0000U;
			for( wkRadio = RF_HSMODE_RES_SETREAD_LENGTH_HEADER; wkRadio < RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_BCH - 2U; wkRadio++ )
			{
				/* CRC�v�Z */
				CRCIN = vutRfDataBuffer.umData[ wkRadio ];
			}
			vutRfDataBuffer.umLoggerResSetting.mCrc2[1U] = (uint8_t)(CRCD);
			vutRfDataBuffer.umLoggerResSetting.mCrc2[0U] = (uint8_t)(CRCD >> 8U);
			
			SubRfDrv_Coding_Bch( &vutRfDataBuffer.umData[RF_HSMODE_RES_SETREAD_LENGTH_HEADER]
				, RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_BCH - RF_HSMODE_RES_SETREAD_LENGTH_HEADER
				, RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_HEADER - RF_HSMODE_RES_SETREAD_LENGTH_BCH  );
			
			break;
			
		/* GW�ɐݒ�l�v�� */
		case ecRfDrvGw_Hs_ReqCmd_SetWrite:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_SETQUERY_LENGTH;
			vrfHsReqDataNum = 1U;
			vrfPacketLimit = vrfHsReqDataNum;
			vrfPacketSum = vrfPacketLimit;
			
			/* �e�X�g�Ƀ_�~�[�f�[�^���Z�b�g(�\��f�[�^) */
			wkRadio = 0U;
			for( wkCulc = 0U; wkCulc < RF_HSMODE_RES_SETQUERY_LENGTH; wkCulc++ )
			{
				vutRfDataBuffer.umData[wkCulc] = wkRadio;
				wkRadio++;
			}
			memcpy( &vutRfDataBuffer.umLoggerQuerySet.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
			memcpy( &vutRfDataBuffer.umLoggerQuerySet.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
			vutRfDataBuffer.umLoggerQuerySet.mPacketCode = 0x09;
			vutRfDataBuffer.umLoggerQuerySet.mGroupID = vrfMySerialID.mGroupID;
			SubRfDrv_Coding_HS( &vutRfDataBuffer.umData[0U], RF_HSMODE_RES_SETQUERY_LENGTH, BCH_OFF );
			break;
			
		case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
			/* �t�@�[���A�b�v�f�[�g�ʐM */
			vrfHsmodePacketLength = RF_HSMODE_RES_REQ_FIRM_LENGTH;
			if( vrfPacketLimit == 0U )
			{
				/* ����̂� */
				vrfHsReqDataNum = 1U;
				vrfPacketLimit = vrfHsReqDataNum;
				vrfPacketSum = vrfPacketLimit;
			}
			/* �e�X�g�Ƀ_�~�[�f�[�^���Z�b�g */
			wkRadio = 0U;
			for( wkCulc = 0U; wkCulc < RF_HSMODE_RES_REQ_FIRM_LENGTH; wkCulc++ )
			{
				vutRfDataBuffer.umData[wkCulc] = wkRadio;
				wkRadio++;
			}
			memcpy( &vutRfDataBuffer.umLoggerAckNack.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
			memcpy( &vutRfDataBuffer.umLoggerAckNack.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
			vutRfDataBuffer.umLoggerAckNack.mPacketCode = 0x11;
			vutRfDataBuffer.umLoggerAckNack.mResponce = 0x00;			/* �t�@�[���A�b�vOK���� */
			SubRfDrv_Coding_HS( &vutRfDataBuffer.umData[0U], RF_HSMODE_RES_REQ_FIRM_LENGTH, BCH_OFF );
			
			break;
			
			
		default:
			/* �R�}���h��@�s�� */
//			vrfInitState = ecRfDrv_Init_Non;
//			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
//			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
	}
}

/*
*******************************************************************************
*	�������[�h ���K�[�����f�[�^�̃Z�b�g(�X�e�[�^�X�����̃T�u�֐�)
*	
*	[���e]
*		���K�[�����f�[�^�̃Z�b�g(�X�e�[�^�X�����̃T�u�֐�)
*	[����]
*		�����o�b�t�@
*	[return]
*		����
*******************************************************************************
*/
static void SubRfDrv_SetStatusAdd( uint8_t *parBuffer )
{
	uint8_t		wkStatusLoop;
	uint8_t		wkLoop;
	uint16_t	wkNewIndex;
	uint16_t	wkOldIndex;
	uint32_t	wkOldTime;
	uint32_t	wkU32;
	
	
	for( wkStatusLoop = 0U ; wkStatusLoop < 6U ; wkStatusLoop++ )
	{
		switch( wkStatusLoop )
		{
			/* �v���l */
			default:
			case 0U:
				wkNewIndex = (gvRfRTMeasData.mRTMeasData_MeasID[ 1U ] << 8U) + gvRfRTMeasData.mRTMeasData_MeasID[ 0U ];
				wkOldIndex = gvInFlash.mData.mMeas1_PastIndex;
				wkOldTime = gvInFlash.mData.mMeas1_PastTime;
				break;
			/* �v���x�񗚗� */
			case 1U:
				wkNewIndex = gvInFlash.mData.mMeasAlm_Index;
				wkOldIndex = gvInFlash.mData.mMeasAlm_PastIndex;
				wkOldTime = gvInFlash.mData.mMeasAlm_PastTime;
				break;
			/* ���엚�� */
			case 2U:
				wkNewIndex = gvInFlash.mData.mActLog_Index;
				wkOldIndex = gvInFlash.mData.mActLog_PastIndex;
				wkOldTime = gvInFlash.mData.mActLog_PastTime;
				break;
			/* �C�x���g���� */
			case 3U:
				wkNewIndex = gvInFlash.mData.mEvt_Index;
				wkOldIndex = gvInFlash.mData.mEvt_PastIndex;
				wkOldTime = gvInFlash.mData.mEvt_PastTime;
				break;
			/* �@��ُ헚�� */
			case 4U:
				wkNewIndex = gvInFlash.mData.mErrInf_Index;
				wkOldIndex = gvInFlash.mData.mErrInf_PastIndex;
				wkOldTime = gvInFlash.mData.mErrInf_PastTime;
				break;
			/* �V�X�e������ */
			case 5U:
				wkNewIndex = gvInFlash.mData.mSysLog_Index;
				wkOldIndex = gvInFlash.mData.mSysLog_PastIndex;
				wkOldTime = gvInFlash.mData.mSysLog_PastTime;
				break;
		}
		
		/* �ŐVIndexNo. */
		(*parBuffer) = (uint8_t)(wkNewIndex & 0x00FFU);
		parBuffer ++;
		(*parBuffer) = (uint8_t)((wkNewIndex) >> 8U & 0x00FFU);
		parBuffer ++;
		
		/* �Ō�IndexNo. */
		(*parBuffer) = (uint8_t)(wkOldIndex & 0x00FFU);
		parBuffer ++;
		(*parBuffer) = (uint8_t)((wkOldIndex) >> 8U & 0x00FFU);
		parBuffer ++;
		
		/* �ŌÎ��� */
		for( wkLoop = 0U ; wkLoop < 4U; wkLoop++ )
		{
			wkU32 = wkOldTime >> (24U - wkLoop * 8U);
			(*parBuffer) = (uint8_t)(wkU32 & 0x000000FF);
			parBuffer ++;
		}
	}
}

/*
*******************************************************************************
*	�������[�h�@���K�[�����f�[�^�̃Z�b�g(�T�u�֐�)
*
*	[���e]
*		��������f�[�^���ő��M�p�P�b�g�����Z�o���Z�b�g����
* [����]
*		uint8_t arNum :1�p�P�b�g�ŉ�������f�[�^��
* [return]
*
*******************************************************************************
*/
static void SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( uint8_t arNum )
{
	if( vrfPacketLimit == 0U )
	{
		/* ����̂� */
		/* �v��IndexNo.���瑗�M�p�P�b�g�����Z�o */
		vrfPacketLimit = vrfHsReqDataNum / arNum;
		if(vrfHsReqDataNum % arNum > 0U)
		{
			vrfPacketLimit++;
		}
		
		/* �ŏ��̋����p�P�b�g�������f�[�^�ŉ������邽�߁A�p�P�b�g��1���₷ */
		vrfPacketLimit++;
		
		vrfPacketSum = vrfPacketLimit;
	}
}

/*
*******************************************************************************
*	�������[�h�@�����ʐM�X���[�v����
*
*	[���e]
*		��M��ɑ��M�������^�f�[�^�������ʐM�X���[�v�̐ݒ�l�܂ŒB�������𔻒�
* [����]
*
* [return]
*
*******************************************************************************
*/
static void SubRFDrv_CalFstConnBootCnt( void )
{
	uint16_t wkSnd_StaId;
	uint16_t wkSnd_EndId;
	
	if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Measure &&
		vutRfDataBuffer.umLoggerResMeasure.mDataNum > 0U )
	{
		/* ���O�ɃZ�b�g���ꂽ�p�P�b�g�̏�� */
		wkSnd_StaId = (uint16_t)vutRfDataBuffer.umLoggerResMeasure.mHeaderID[ 1U ] << 8U;
		wkSnd_StaId += vutRfDataBuffer.umLoggerResMeasure.mHeaderID[ 0U ];
		wkSnd_EndId = wkSnd_StaId + vutRfDataBuffer.umLoggerResMeasure.mDataNum - 1;
		
		if( wkSnd_EndId > imIndexMaxMeas1 )
		{
			wkSnd_EndId -= imIndexMaxMeas1;
		}
		
		/* �X���[�v�ƂȂ�N�_���܂܂��p�P�b�g�����M���ꂽ */
		if( wkSnd_StaId == vSleepPacket_StaIndex )
		{
			/* ����1�N���A */
			M_SETBIT( vSleep_Sts, M_BIT1 );
		}
		
		/* �N�_���܂܂��p�P�b�g�̈�O�̃p�P�b�g�����M���ꂽ */
		if( vPrePacket_EndIndex == 0U || wkSnd_EndId == vPrePacket_EndIndex )
		{
			/* ����2�N���A */
			M_SETBIT( vSleep_Sts, M_BIT0 );
		}
		
		if( gvFlashReadIndex[ 0U ] == wkSnd_StaId )
		{
			vKisu_Snd_EndId = wkSnd_EndId;
		}
		else
		{
			vGusu_Snd_EndId = wkSnd_EndId;
		}
		
	}
}


/*
*******************************************************************************
*	�������[�h�@���K�[�����f�[�^�̃p�P�b�g�|�C���^��ݒ�
*
*	[���e]
*		�������[�h�Ńr�[�R������M���A���̉�����Ԃ��f�[�^���Z�b�g���邽�߂̃p�P�b�g�|�C���^��ݒ�
*		�|�C���^�����Ƀ��K�[�̃t���b�V�����瑗�M�o�b�t�@�փZ�b�g���邱�Ƃ�z��
* [����]
*
* [return]
*
*******************************************************************************
*/
static void SubRFDrv_Hsmode_PcktPointerSet( void )
	{
	uint16_t wkU16;
	
		/* �Z�b�g�ς݂̎��̑��M�҂�����p�P�b�g */
		if( gvrfStsEventFlag.mHsmodeTxTurn == 0U )
		{
			gvrfHsmodePacketPt = vrfPacketPt[ 1U ];			/* ��Flash���烊�[�h����̂͋����p�P�b�g */
			
			/* �����p�P�b�g�̃��[�h�S�����F���M���� */
			if( vrfLogSendPacketCtl.mFlashReadFin[ 1U ] == ecLogSndCtl_ReadFin )
			{
				vrfLogSendPacketCtl.mFlashReadFin[ 1U ] = ecLogSndCtl_SndFin;
			}
		}
		/* �Z�b�g�ς݂̎��̑��M�҂��������p�P�b�g */
		else
		{
			gvrfHsmodePacketPt = vrfPacketPt[ 0U ];			/* ��Flash���烊�[�h����̂͊�p�P�b�g */
			
			/* ��p�P�b�g�̃��[�h�S�����F���M���� */
			if( vrfLogSendPacketCtl.mFlashReadFin[ 0U ] == ecLogSndCtl_ReadFin )
			{
				vrfLogSendPacketCtl.mFlashReadFin[ 0U ] = ecLogSndCtl_SndFin;
			}
		}
		
		/* �Ƃ��Ƀ��[�h�����Ȃ瑗�M���� */
		if( (vrfLogSendPacketCtl.mFlashReadFin[ 0U ] == ecLogSndCtl_SndFin) &&
			(vrfLogSendPacketCtl.mFlashReadFin[ 1U ] == ecLogSndCtl_SndFin) )
		{
			vrfPacketSum = 0U;
		}
		/* ��p�P�b�g�̂ݑ��M���� */
		else if( vrfLogSendPacketCtl.mFlashReadFin[ 0U ] == ecLogSndCtl_SndFin )
		{
			gvrfStsEventFlag.mHsmodeTxTurn = 0U;			/* ���Z�b�g�ς݂̑��M��ɋ����p�P�b�g�ɐ؂�ւ��悤�ɃZ�b�g */
			gvrfHsmodePacketPt = vrfPacketPt[ 1U ];			/* ��Flash���烊�[�h����̂͋����p�P�b�g */
		}
		/* �����p�P�b�g�̂ݑ��M���� */
		else if( vrfLogSendPacketCtl.mFlashReadFin[ 1U ] == ecLogSndCtl_SndFin )
		{
			gvrfStsEventFlag.mHsmodeTxTurn = 1U;			/* ���Z�b�g�ς݂̑��M��Ɋ�p�P�b�g�ɐ؂�ւ��悤�ɃZ�b�g */
			gvrfHsmodePacketPt = vrfPacketPt[ 0U ];			/* ��Flash���烊�[�h����̂͊�p�P�b�g */
		}
	
	
		if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Measure )
		{
			/* ����1��2��B�� �� �X���[�v�֑J�� */
			if( vSleep_Sts == 0x03U )
			{
				if( vKisu_Snd_EndId == 0U )
				{
					wkU16 = vGusu_Snd_EndId;
				}
				else if( vGusu_Snd_EndId == 0U )
				{
					wkU16 = vKisu_Snd_EndId;
				}
			else
			{
				if( vKisu_Snd_EndId >= gvInFlash.mData.mMeas1_Index && gvInFlash.mData.mMeas1_Index >= vGusu_Snd_EndId )
				{
					if( (imIndexMaxMeas1 - vKisu_Snd_EndId) + vGusu_Snd_EndId <= 90U )
					{
						wkU16 = vGusu_Snd_EndId;
					}
					else
					{
						wkU16 = vKisu_Snd_EndId;
					}
				}
				else if( gvInFlash.mData.mMeas1_Index >= vKisu_Snd_EndId && vGusu_Snd_EndId >= gvInFlash.mData.mMeas1_Index )
				{
					if( (imIndexMaxMeas1 - vGusu_Snd_EndId) + vKisu_Snd_EndId <= 90U )
					{
						wkU16 = vKisu_Snd_EndId;
					}
					else
					{
						wkU16 = vGusu_Snd_EndId;
					}
				}
				else
				{
					if( vGusu_Snd_EndId >= vKisu_Snd_EndId )
					{
						if( vGusu_Snd_EndId - vKisu_Snd_EndId <= 90U )
						{
							wkU16 = vGusu_Snd_EndId;
						}
						else
						{
							wkU16 = vKisu_Snd_EndId;
						}
					}
					else
					{
						if( vKisu_Snd_EndId - vGusu_Snd_EndId <= 90U )
						{
							wkU16 = vKisu_Snd_EndId;
						}
						else
						{
							wkU16 = vGusu_Snd_EndId;
						}
					}
				}
			}
			
			if( gvInFlash.mData.mMeas1_Index >= wkU16 )
			{
				gvFstConnBootCnt = gvInFlash.mData.mMeas1_Index - wkU16;
			}
			else
			{
				gvFstConnBootCnt = imIndexMaxMeas1 - wkU16 + gvInFlash.mData.mMeas1_Index;
			}
		}
	}
}


/*
*******************************************************************************
*	�������[�h�@���K�[�@�t�@�[��HEX,�ݒ�ύX���̎�M�ɑ΂���Ack�����f�[�^�Z�b�g
*
*	[���e]
*		�������[�h��GW�����M�����t�@�[��HEX,�ݒ�ύX���ɑ΂���ACK�f�[�^�̃Z�b�g
* [����]
*
* [return]
*
*******************************************************************************
*/
void SubRFDrv_Hsmode_Lg_To_Gw_AckDataSet( void )
{
	/* Logger ID */
	memcpy( &vrfHsmodeAckBuff.umCategories.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
	
	/* GW ID */
	memcpy( &vrfHsmodeAckBuff.umCategories.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
	
	/* �p�P�b�g��ʃR�[�h */
	switch( vrfHsReqCmdCode )
	{
		case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
			vrfHsmodeAckBuff.umCategories.mPacketCode = 0x14;
			break;
		case ecRfDrvGw_Hs_ReqCmd_SetWrite:
			vrfHsmodeAckBuff.umCategories.mPacketCode = 0x10;
			break;
		default:
			break;
	}
	
	/* �������e */
	if( vrfResInfo == ecRfDrv_Success )
	{
		vrfHsmodeAckBuff.umCategories.mResponce = 0x00;		/* ACK */
	}
	else
	{
		vrfHsmodeAckBuff.umCategories.mResponce = 0x01;		/* NACK */
	}
	
	/* �ݒ�ύX���� */
	memcpy( &vrfHsmodeAckBuff.umCategories.mSetTime[ 0U ], &gvInFlash.mParam.mSetvalChgTime[ 0U ], 4U );
	
	SubRfDrv_Coding_HS( &vrfHsmodeAckBuff.umData[0U], RF_HSMODE_ACK_LENGTH, BCH_OFF );

}

/*
************************************************************************************
*	GW��ID�t�B���^�����O�`�F�b�N
*	[���e]
*		��M����GWID�ƃ��K�[�̃t�B���^�����O�ݒ���r����
*		gvInFlash.mParam.mCnctEnaGwId[0U]��[0U]-[2U]��0x00 00 00 �̏ꍇ�͑SGW������
*	[����]
*		*arGwid : ��M����GW��ID����l�߂�3byte
*	[�Ԃ�l]
*		ecRfDrv_Success : �ʐM����
*		ecRfDrv_Fail : �ʐM����
************************************************************************************
*/
ET_RfDrv_ResInfo_t SubRFDrv_Filter_Gwid( uint8_t *arGwid )
{
	uint8_t wkFunc;
	ET_RfDrv_ResInfo_t wkReturn;

#if 1
	if( (gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 0U ] == 0x00) && (gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 1U ] == 0x00) && (gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 2U ] == 0x00) )
	{
#else
	if( (vrfGwConnectList[0U][0U] == 0x00) && (vrfGwConnectList[0U][1U] == 0x00) && (vrfGwConnectList[0U][2U] == 0x00) )
	{
#endif
		wkReturn = ecRfDrv_Success;
	}else
	{
		for( wkFunc = 0U; wkFunc < GW_FILTER_MAXNUM; wkFunc++ )
		{
#if 1
			if( (gvInFlash.mParam.mCnctEnaGwId[ wkFunc ][ 0U ] == *arGwid)
				&& (gvInFlash.mParam.mCnctEnaGwId[ wkFunc ][ 1U ] == *(arGwid + 1U) )
				&& (gvInFlash.mParam.mCnctEnaGwId[ wkFunc ][ 2U ] == *(arGwid + 2U) ) )
			{
#else
			if( (vrfGwConnectList[wkFunc][0U] == *arGwid)
				&& (vrfGwConnectList[wkFunc][1U] == *(arGwid + 1U) )
				&& (vrfGwConnectList[wkFunc][2U] == *(arGwid + 2U) ) )
			{
#endif
				break;
			}
		}
		if( wkFunc != GW_FILTER_MAXNUM )
		{
			wkReturn = ecRfDrv_Success;
		}else
		{
			wkReturn = ecRfDrv_Fail;
		}
	}
	return wkReturn;
}

#endif

#if(swLoggerBord == imDisalbe)
/*
*******************************************************************************
*	�������[�h�@���K�[����̉����f�[�^�ɑ΂���GW��Responce/Ack�f�[�^�Z�b�g
*
*	[���e]
*		�������[�h�Ńr�[�R���𑗐M���A���̉�����M�ɑ΂���Ack�f�[�^���Z�b�g����B
*		�v���l���̘A���Ńf�[�^����M����ۂ��Ăяo�����
*
*		�ݒ菑������/�t�@�[���A�b�v�ʐM�̃f�[�^�Z�b�g�̓��C�����[�`���ł̃Z�b�g��z��
*			gvrfStsEventFlag.mHsmodeGwToLgDataSet?==?1U�@���A
*			vRfDrvMainGwSts?=?ecRfDrvMainSts_HSmode_Gw_To_Lg_DataSet
*			�̏ꍇ�Ƀf�[�^���Z�b�g����B
*			�t�@�[���A�b�v�̏ꍇ��gvrfHsmodePacketPt�ɉ������p�P�b�g���Z�b�g����
*			(GW���͖�����)
*******************************************************************************
*/
void SubRFDrv_Hsmode_Logger_BcnAckDataSet( void )
{
	ET_RfDrv_GwReqCmd_t wkLoop;
	
	for( wkLoop = ecRfDrvGw_Hs_ReqCmd_Non ; wkLoop < ecRfDrvGw_Hs_ReqCmd_Max ; wkLoop++ )
	{
		if( cBcnAckDataSetTbl[ wkLoop ].mReqCmdCode == vrfHsReqCmdCode )
		{
			cBcnAckDataSetTbl[ wkLoop ].mFnc( wkLoop );
			break;
		}
	}
}

/*
 *******************************************************************************
 *	�����f�[�^�ɑ΂���ACK�f�[�^�Z�b�g
 *	(�v���l�A�x�񗚗��A�@��ُ헚���A�C�x���g�����A���엚���A�V�X�e������)
 *
 *	[���e]
 *		�����f�[�^�ɑ΂���ACK�f�[�^�Z�b�g
 *		(�v���l�A�x�񗚗��A�@��ُ헚���A�C�x���g�����A���엚���A�V�X�e������)
 *	[����]
 *		uint8_t arLoop:�����o�b�t�@�̃��[�v�J�E���^
 *******************************************************************************
 */
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_MeasHist( uint8_t arLoop )
{
	/* ��M�����Ȃ牞���o�b�t�@��CPU�ԒʐM�o�b�t�@�փZ�b�g */
	if( vrfResInfo == ecRfDrv_Success )
	{
		SubRFDrv_Hsmode_Logger_BcnAckDataSet_SubModDatSet();
	}
	
	vrfHsmodeAckBuff.umCategories.mGwID[0U] = vrfMySerialID.mUniqueID[0U];
	vrfHsmodeAckBuff.umCategories.mGwID[1U] = vrfMySerialID.mUniqueID[1U];
	vrfHsmodeAckBuff.umCategories.mGwID[2U] = vrfMySerialID.mUniqueID[2U];
	vrfHsmodeAckBuff.umCategories.mLoggerID[0U] = vrfHSLoggerID[0U];
	vrfHsmodeAckBuff.umCategories.mLoggerID[1U] = vrfHSLoggerID[1U];
	vrfHsmodeAckBuff.umCategories.mLoggerID[2U] = vrfHSLoggerID[2U];
	vrfHsmodeAckBuff.umCategories.mGroupID = vrfMySerialID.mGroupID;
	vrfHsmodeAckBuff.umCategories.mPacketCode = cBcnAckDataSetTbl[ arLoop ].mPacketCode;
	vrfHsmodeAckBuff.umCategories.mResponce = vrfResInfo;
	
	SubRfDrv_Coding_HS( &vrfHsmodeAckBuff.umData[ 0U ], RF_HSMODE_ACK_LENGTH, BCH_OFF );
}

/*
 *******************************************************************************
 *	��M���������o�b�t�@��CPU�ԒʐM�o�b�t�@�փZ�b�g
 *
 *	[���e]
 *		��M���������o�b�t�@��CPU�ԒʐM�o�b�t�@�փZ�b�g
 *	[����]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_SubModDatSet( void )
{
	if( gvutComBuff.umModInf.mComSts == ecComSts_RcvWait )
	{
		gvutComBuff.umModInf.mHiPriorityFlg = imON;			/* CPU��Modbus�ʐM�̍ŗD�摗�M�t���O(�����ʐM���W�f�[�^����)��ON */
		
		switch( vrfHsReqCmdCode )
		{
			/* �v���l���� */
			case ecRfDrvGw_Hs_ReqCmd_Measure:
				ApiModbus_SndFnc16Ref41201( &vutRfDataBuffer.umLoggerResMeasure );
				break;
				
			/* �x�񗚗����� */
			case ecRfDrvGw_Hs_ReqCmd_AlartHis:
				ApiModbus_SndFnc16Ref46001( &vutRfDataBuffer.umLoggerResAlarmHist );
				break;
				
			/* �ُ헚������ */
			case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
				ApiModbus_SndFnc16Ref46601( &vutRfDataBuffer.umLoggerResAbnormalHist );
				break;
				
			/* �C�x���g�������� */
			case ecRfDrvGw_Hs_ReqCmd_EventHis:
				ApiModbus_SndFnc16Ref47101( &vutRfDataBuffer.umLoggerResEventHist );
				break;
				
			/* ���엚������ */
			case ecRfDrvGw_Hs_ReqCmd_OpeHis:
				ApiModbus_SndFnc16Ref47701( &vutRfDataBuffer.umLoggerResOpeHist );
				break;
				
			/* �V�X�e���������� */
			case ecRfDrvGw_Hs_ReqCmd_SysHis:
				ApiModbus_SndFnc16Ref48401( &vutRfDataBuffer.umLoggerResSysHist );
				break;
		}
	}
	else
	{
		vrfResInfo = ecRfDrv_Fail;
	}
}


/*
 *******************************************************************************
 *	�����f�[�^�ɑ΂���ACK�f�[�^�Z�b�g(�ݒ�l�Ǎ���)
 *
 *	[���e]
 *		�����f�[�^�ɑ΂���ACK�f�[�^�Z�b�g(�ݒ�l�Ǎ���)
 *	[����]
 *		uint8_t arLoop:�����o�b�t�@�̃��[�v�J�E���^
 *******************************************************************************
 */
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_SetRead( uint8_t arLoop )
{
	/* �ݒ�l��1�p�P�b�g�̂���ACK�͂Ȃ� �f�[�^READ�̂� */
	/* ��M�����Ȃ牞���o�b�t�@��CPU�ԒʐM�o�b�t�@�փZ�b�g */
	if( vrfResInfo == ecRfDrv_Success )
	{
		/* ��M�����ݒ�l��CPU�ԒʐM�o�b�t�@�փZ�b�g */
		ApiModbus_SndFnc16Ref42101( &vutRfDataBuffer.umLoggerResSetting );
		ApiModbus_SndFnc16Ref42158( &vutRfDataBuffer.umLoggerResSetting );
		
		vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;
	}else
	{
		/* �����ɂ͗��Ȃ��͂� */
		vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
	}
}

/*
 *******************************************************************************
 *	�����f�[�^�ɑ΂���Response�f�[�^�Z�b�g(�ݒ�l������)
 *
 *	[���e]
 *		�����f�[�^�ɑ΂���Response�f�[�^�Z�b�g(�ݒ�l������)
 *	[����]
 *		uint8_t arLoop:�����o�b�t�@�̃��[�v�J�E���^
 *******************************************************************************
 */
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_SetWrite( uint8_t arLoop )
{
	uint16_t wkRadio;
	
	if( gvrfStsEventFlag.mHsmodeGwToLgDataSet == 1U )
	{
		/* Linux����擾�����ݒ�f�[�^�𖳐��o�b�t�@�ɃZ�b�g */
		ApiModbus_WtSetvalSet( &vutRfDataBuffer.umGwWriteSet );
		
		vutRfDataBuffer.umGwWriteSet.mGwID[0U] = vrfMySerialID.mUniqueID[0U];
		vutRfDataBuffer.umGwWriteSet.mGwID[1U] = vrfMySerialID.mUniqueID[1U];
		vutRfDataBuffer.umGwWriteSet.mGwID[2U] = vrfMySerialID.mUniqueID[2U];
		vutRfDataBuffer.umGwWriteSet.mLoggerID[0U] = vrfHSLoggerID[0U];
		vutRfDataBuffer.umGwWriteSet.mLoggerID[1U] = vrfHSLoggerID[1U];
		vutRfDataBuffer.umGwWriteSet.mLoggerID[2U] = vrfHSLoggerID[2U];
		vutRfDataBuffer.umGwWriteSet.mPacketCode = cBcnAckDataSetTbl[ arLoop ].mPacketCode;
		SubRfDrv_Coding_HS( &vutRfDataBuffer.umData[0U], RF_HSMODE_RES_SETWRITE_LENGTH_HEADER, BCH_OFF );
		
		CRCD = 0x0000U;
		for( wkRadio = RF_HSMODE_RES_SETWRITE_LENGTH_HEADER; wkRadio < RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_BCH - 2U; wkRadio++ )
		{
			/* CRC�v�Z */
			CRCIN = vutRfDataBuffer.umData[ wkRadio ];
		}
		vutRfDataBuffer.umGwWriteSet.mCrc2[1U] = (uint8_t)(CRCD);
		vutRfDataBuffer.umGwWriteSet.mCrc2[0U] = (uint8_t)(CRCD >> 8U);
		
		SubRfDrv_Coding_Bch( &vutRfDataBuffer.umData[RF_HSMODE_RES_SETWRITE_LENGTH_HEADER]
			, RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_BCH - RF_HSMODE_RES_SETWRITE_LENGTH_HEADER
			, RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_HEADER - RF_HSMODE_RES_SETWRITE_LENGTH_BCH	);
	}else
	{
		/* ���C�����[�`���ł̃f�[�^�Z�b�g�҂� */
	}
}

/*
 *******************************************************************************
 *	�����f�[�^�ɑ΂���Response�f�[�^�Z�b�g(�t�@�[��������)
 *
 *	[���e]
 *		�����f�[�^�ɑ΂���Response�f�[�^�Z�b�g(�t�@�[��������)
 *	[����]
 *		uint8_t arLoop:�����o�b�t�@�̃��[�v�J�E���^
 *******************************************************************************
 */
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_ReqFirm( uint8_t arLoop )
{
	uint8_t		wkLoop;
	uint8_t		wkRdBuff[ 16U ];
	uint32_t	wkAdr;
	uint16_t	wkRadio;
	static uint16_t		vCrcData;
	static uint16_t		vPrePacket;
	
	/* �t�@�[���A�b�v�̏ꍇ�̓��C���ɖ߂�����ɃZ�b�g���� */
	if( gvrfStsEventFlag.mHsmodeGwToLgDataSet == 1U )
	{
		/* �w�b�_�Z�b�g */
		vutRfDataBuffer.umFirmHex.mGwID[0U] = vrfMySerialID.mUniqueID[0U];
		vutRfDataBuffer.umFirmHex.mGwID[1U] = vrfMySerialID.mUniqueID[1U];
		vutRfDataBuffer.umFirmHex.mGwID[2U] = vrfMySerialID.mUniqueID[2U];
		vutRfDataBuffer.umFirmHex.mLoggerID[0U] = vrfHSLoggerID[0U];
		vutRfDataBuffer.umFirmHex.mLoggerID[1U] = vrfHSLoggerID[1U];
		vutRfDataBuffer.umFirmHex.mLoggerID[2U] = vrfHSLoggerID[2U];
		vutRfDataBuffer.umFirmHex.mPacketCode = cBcnAckDataSetTbl[ arLoop ].mPacketCode;
		if( vrfPacketSum == 0U )
		{
			vutRfDataBuffer.umFirmHex.mPacketNo[ 0U ] = 0x00;
			vutRfDataBuffer.umFirmHex.mPacketNo[ 1U ] = 0x00;
		}
		else
		{
			vutRfDataBuffer.umFirmHex.mPacketNo[ 0U ]= (uint8_t)(vrfPacketSum >> 8U);
			vutRfDataBuffer.umFirmHex.mPacketNo[1U] = (uint8_t)(vrfPacketSum);
		}
		vutRfDataBuffer.umFirmHex.mPacketSum[ 0U ]= (uint8_t)(vrfPacketLimit >> 8U);
		vutRfDataBuffer.umFirmHex.mPacketSum[ 1U ] = (uint8_t)(vrfPacketLimit);
		
		for( wkLoop = 0U ; wkLoop < 6U ; wkLoop++ )
		{
			vutRfDataBuffer.umFirmHex.mReserve[ wkLoop ] = 0U;
		}
		
		/* �ŏ��̃p�P�b�g:��p�P�b�g */
		if( vrfPacketSum == 0U )
		{
			for( wkRadio = 0U ; wkRadio < 890U ; wkRadio++ )
			{
				vutRfDataBuffer.umFirmHex.mHex[ wkRadio ] = 0xFFU;
			}
			vCrcData = 0x0000;
			vPrePacket = 1;
		}
		/* �Ō�̃p�P�b�g:CRC���� */
		else if( vrfPacketSum == vrfPacketLimit )
		{
			vutRfDataBuffer.umFirmHex.mReserve[ 0U ] = vCrcData >> 8U;				/* �t�@�[����CRC���� */
			vutRfDataBuffer.umFirmHex.mReserve[ 1U ] = vCrcData & 0x00FF;
			vutRfDataBuffer.umFirmHex.mReserve[ 2U ] = vrfHsLoginUserId >> 8U;		/* ���O�C�����[�UID */
			vutRfDataBuffer.umFirmHex.mReserve[ 3U ] = vrfHsLoginUserId & 0x00FF;
		}
		/* �ʏ�:�t�@�[���f�[�^ */
		else
		{
			/* �O�t��Flash���烊�[�h���AHEX�f�[�^���Z�b�g */
			wkAdr = vrfPacketSum - 1U;
			wkAdr *= 890U;
			for( wkRadio = 0U ; wkRadio < 880U ; wkRadio+=16, wkAdr+=16 )
			{
				ApiFlashDrv_ReadData( wkAdr, &wkRdBuff[ 0U ], 16U, ecFlashKind_Prim );
				memcpy( &vutRfDataBuffer.umFirmHex.mHex[ wkRadio ], &wkRdBuff[ 0U ], 16U );
			}
			ApiFlashDrv_ReadData( wkAdr, &wkRdBuff[ 0U ], 10U, ecFlashKind_Prim );
			memcpy( &vutRfDataBuffer.umFirmHex.mHex[ 880U ], &wkRdBuff[ 0U ], 10U );
			
			if( vrfPacketSum == vPrePacket )
			{
				CRCD = vCrcData;
				for( wkRadio = 0U ; wkRadio < 890U ; wkRadio++ )
				{
					if( vutRfDataBuffer.umFirmHex.mHex[ wkRadio ] == 0xFF )
					{
						M_NOP;
						break;
					}
					CRCIN = vutRfDataBuffer.umFirmHex.mHex[ wkRadio ];
				}
				M_NOP;
				vCrcData = CRCD;
				vPrePacket ++;
			}
		}
		
		/* CRC */
		SubRfDrv_Coding_HS( &vutRfDataBuffer.umData[0U], RF_HSMODE_FIRM_HEX_LENGTH_HEADER, BCH_OFF );
		CRCD = 0x0000U;
		for( wkRadio = 0U; wkRadio < RF_HSMODE_FIRM_HEX_LENGTH_HEX_CRC - 10U; wkRadio++ )
		{
			CRCIN = vutRfDataBuffer.umFirmHex.mHex[wkRadio];
		}
		vutRfDataBuffer.umFirmHex.mCrc2[1U] = (uint8_t)(CRCD);
		vutRfDataBuffer.umFirmHex.mCrc2[0U] = (uint8_t)(CRCD >> 8U);
		/* BCH */
		SubRfDrv_Coding_Bch( &vutRfDataBuffer.umFirmHex.mPacketNo[0U], RF_HSMODE_FIRM_HEX_LENGTH_HEX_CRC + 2U, RF_HSMODE_FIRM_HEX_LENGTH_HEX_CRC );
	}
}
#endif

#if swLoggerBord == imEnable
#else
/*
*******************************************************************************
*	�������[�h�@���K�[����̃r�[�R���ヌ�X�|���X�̃p�P�b�g�R�[�h���`�F�b�N����
*
*	[���e]
*		�p�P�b�g��ʃR�[�h�ƃQ�[�g�E�F�C�����o�������N�G�X�g�R�}���h�̈�v�m�F
*		�v���l�ʐM,����ʐM�Ȃ�Logger��GW�̃f�[�^�ʐM��
*	[����]
*		�Ȃ�
*	[return]
*		Success/Fail
*******************************************************************************
*/
uint8_t SubRFDrv_Hsmode_BcnPktCodeCheck( void )
{
	uint8_t wkReturn;
	
	wkReturn = FAIL;
	switch( vrfHsReqCmdCode )
	{
		case ecRfDrvGw_Hs_ReqCmd_Status:
			if( vutRfDataBuffer.umLoggerResStatus.mPacketCode == 0x01 )
			{
				wkReturn = SUCCESS;
			}
			break;
		case ecRfDrvGw_Hs_ReqCmd_Toroku:
			if( vutRfDataBuffer.umLoggerResStatus.mPacketCode == 0x0C )
			{
				wkReturn = SUCCESS;
			}
			if( vutRfDataBuffer.umLoggerResStatus.mPacketCode == 0x0D )
			{
				wkReturn = SUCCESS;
			}
			break;
		case ecRfDrvGw_Hs_ReqCmd_Measure:
			if( (vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode & 0x0F) == 0x02 )
			{
				wkReturn = SUCCESS;
			}
			break;
		case ecRfDrvGw_Hs_ReqCmd_AlartHis:
			if( (vutRfDataBuffer.umLoggerResAlarmHist.mPacketCode & 0x0F) == 0x03 )
			{
				wkReturn = SUCCESS;
			}
			break;
		case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
			if( (vutRfDataBuffer.umLoggerResAbnormalHist.mPacketCode & 0x0F) == 0x04 )
			{
				wkReturn = SUCCESS;
			}
			break;
		case ecRfDrvGw_Hs_ReqCmd_EventHis:
			if( (vutRfDataBuffer.umLoggerResEventHist.mPacketCode & 0x0F) == 0x05 )
			{
				wkReturn = SUCCESS;
			}
			break;
		case ecRfDrvGw_Hs_ReqCmd_OpeHis:
			if( (vutRfDataBuffer.umLoggerResOpeHist.mPacketCode & 0x0F) == 0x06 )
			{
				wkReturn = SUCCESS;
			}
			break;
		case ecRfDrvGw_Hs_ReqCmd_SysHis:
			if( (vutRfDataBuffer.umLoggerResSysHist.mPacketCode & 0x0F) == 0x07 )
			{
				wkReturn = SUCCESS;
			}
			break;
		case ecRfDrvGw_Hs_ReqCmd_SetRead:
			if( (vutRfDataBuffer.umLoggerResSetting.mPacketCode & 0x0F) == 0x08 )
			{
				wkReturn = SUCCESS;
			}
			break;
		case ecRfDrvGw_Hs_ReqCmd_SetWrite:
			if( (vutRfDataBuffer.umLoggerQuerySet.mPacketCode & 0x0F) == 0x09 )
			{
				wkReturn = SUCCESS;
			}
			break;
#if 0
		case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
			break;
		case ecRfDrvGw_Hs_FirmHex:
			if( vutRfDataBuffer.umFirmHex.mPacketCode == 0x13 )
			{
				wkReturn = SUCCESS;
			}
			break;
#endif
		default:
			break;
	}
	return wkReturn;
}
#endif

#if (swLoggerBord == imDisable)
/*
*******************************************************************************
* HSmode�w�b�_���@��������
*�@�X�e�[�^�X�ʐM�ȊO�̃w�b�_����
*
*******************************************************************************
*/
ET_RfDrv_ResInfo_t SubRfDrv_Header_Decryption( uint8_t arLength )
{
	ET_RfDrv_ResInfo_t wkReturn;

	wkReturn = ecRfDrv_Success;
	if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[0U], arLength, BCH_OFF ) != SUCCESS )
	{
		/* CRC�G���[ Nack���� */
//		vrfTestHeaderCrcError++;
		wkReturn = ecRfDrv_Fail;
	}
	if( memcmp( vutRfDataBuffer.umGwQueryBeacon.mGwID, vrfMySerialID.mUniqueID, 3U ) != 0 && (wkReturn == ecRfDrv_Success) )
	{
		/* ���@���ł͂Ȃ� ������ */
		wkReturn = ecRfDrv_Error;
	}
	if( (SubRFDrv_Hsmode_BcnPktCodeCheck() != SUCCESS) && (wkReturn == ecRfDrv_Success) )
	{
		/* �p�P�b�g��ʃR�[�h�̃G���[ ������ */
		wkReturn = ecRfDrv_Error;
	}
	
	return wkReturn;
}


/*
*******************************************************************************
* HSmode Payload���@��������
*  arPayloadLen : ���f�[�^��byte��
*  arBchFrameNum : ���f�[�^+BCH��1�t���[���Ƃ��āA���̌J��Ԃ���
*  arCrcFrameNum : arBchFrameNum ? arCrcFrameNum���Ƃ�CRC�t��
*  arHeaderNum : Header��byte���@bch,crc�܂�
*******************************************************************************
*/
ET_RfDrv_ResInfo_t SubRfDrv_Payload_Decryption( uint8_t arPayloadLen, uint8_t arBchFrameNum, uint8_t arCrcFrameNum, uint8_t arHeaderNum )
{
	uint8_t wkBch;
	uint8_t wkLoop1st;
	uint8_t wkLoop2nd;
	uint8_t wkFunc;
	uint8_t wkCrc[2U];
	ET_RfDrv_ResInfo_t wkReturn;
	
	wkBch = arPayloadLen / 2U;
	wkReturn = ecRfDrv_Success;
	
	for( wkLoop1st = 0U; wkLoop1st < arCrcFrameNum; wkLoop1st++ )		/* 10�v��data���Ƃ�CRC�t�� �ő�10�u���b�N(�v100data) */
	{
		/* CRC�`�F�b�N */
		CRCD = 0x0000U;
		for( wkLoop2nd = 0U; wkLoop2nd < arBchFrameNum; wkLoop2nd++ )
		{
			for( wkFunc = 0U; wkFunc < arPayloadLen; wkFunc++ )
			{
				CRCIN = vutRfDataBuffer.umData[(wkLoop1st * ((arPayloadLen + wkBch) * arBchFrameNum + 2U)) + arHeaderNum + wkFunc + (wkLoop2nd * (arPayloadLen + wkBch))];
			}
		}
		switch( vrfHsReqCmdCode )
		{
			case ecRfDrvGw_Hs_ReqCmd_Measure:
				wkCrc[1U] = vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[wkLoop1st].mCrcMeasure[1U];
				wkCrc[0U] = vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[wkLoop1st].mCrcMeasure[0U];
				break;
			
			case ecRfDrvGw_Hs_ReqCmd_AlartHis:
				wkCrc[1U] = vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[wkLoop1st].mCrc[1U];
				wkCrc[0U] = vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[wkLoop1st].mCrc[0U];
				break;
				
			case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
				wkCrc[1U] = vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[wkLoop1st].mCrc[1U];
				wkCrc[0U] = vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[wkLoop1st].mCrc[0U];
				break;
				
			case ecRfDrvGw_Hs_ReqCmd_EventHis:
				wkCrc[1U] = vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[wkLoop1st].mCrc[1U];
				wkCrc[0U] = vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[wkLoop1st].mCrc[0U];
				break;
				
			case ecRfDrvGw_Hs_ReqCmd_OpeHis:
				wkCrc[1U] = vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[wkLoop1st].mCrc[1U];
				wkCrc[0U] = vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[wkLoop1st].mCrc[0U];
				break;
				
			case ecRfDrvGw_Hs_ReqCmd_SysHis:
				wkCrc[1U] = vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[wkLoop1st].mCrc[1U];
				wkCrc[0U] = vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[wkLoop1st].mCrc[0U];
				break;
		}
		if((wkCrc[1U] == (uint8_t)(CRCD) ) && (wkCrc[0U] == (uint8_t)(CRCD >> 8U) ) )
		{
			/* �G���[�Ȃ� Ack���� */
//			vrfTest++;
		}
		else
		{
			/* �G���[���� �G���[�������� */
			for( wkLoop2nd = 0U; wkLoop2nd < arPayloadLen; wkLoop2nd++ )
			{
				switch( vrfHsReqCmdCode )
				{
					case ecRfDrvGw_Hs_ReqCmd_Measure:
						vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[wkLoop1st].mMeasure[wkLoop2nd], arPayloadLen, arPayloadLen);
						break;
						
					case ecRfDrvGw_Hs_ReqCmd_AlartHis:
						vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[wkLoop1st].mAlHist[wkLoop2nd], arPayloadLen, arPayloadLen);
						break;
						
					case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
						vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[wkLoop1st].mAbnormalHist[wkLoop2nd], arPayloadLen, arPayloadLen);
						break;
						
					case ecRfDrvGw_Hs_ReqCmd_EventHis:
						vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[wkLoop1st].mMeas_Evt[wkLoop2nd], arPayloadLen, arPayloadLen);
						break;
						
					case ecRfDrvGw_Hs_ReqCmd_OpeHis:
						vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[wkLoop1st].mOpeHist[wkLoop2nd], arPayloadLen, arPayloadLen);
						break;
						
					case ecRfDrvGw_Hs_ReqCmd_SysHis:
						vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[wkLoop1st].mSysHist[wkLoop2nd], arPayloadLen, arPayloadLen);
						break;
				}
				if( gvRfIntFlg.mRadioTimer )
				{
					return ecRfDrv_Fail;
				}
			}
			/* CRC�`�F�b�N */
			CRCD = 0x0000U;
			for( wkLoop2nd = 0U; wkLoop2nd < arBchFrameNum; wkLoop2nd++ )
			{
				for( wkFunc = 0U; wkFunc < arPayloadLen; wkFunc++ )
				{
					CRCIN = vutRfDataBuffer.umData[(wkLoop1st * ((arPayloadLen + wkBch) * arBchFrameNum + 2U)) + arHeaderNum + wkFunc + (wkLoop2nd * (arPayloadLen + wkBch))];
				}
			}
			switch( vrfHsReqCmdCode )
			{
				case ecRfDrvGw_Hs_ReqCmd_Measure:
					wkCrc[1U] = vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[wkLoop1st].mCrcMeasure[1U];
					wkCrc[0U] = vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[wkLoop1st].mCrcMeasure[0U];
					break;
					
				case ecRfDrvGw_Hs_ReqCmd_AlartHis:
					wkCrc[1U] = vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[wkLoop1st].mCrc[1U];
					wkCrc[0U] = vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[wkLoop1st].mCrc[0U];
					break;
					
				case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
					wkCrc[1U] = vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[wkLoop1st].mCrc[1U];
					wkCrc[0U] = vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[wkLoop1st].mCrc[0U];
					break;
					
				case ecRfDrvGw_Hs_ReqCmd_EventHis:
					wkCrc[1U] = vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[wkLoop1st].mCrc[1U];
					wkCrc[0U] = vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[wkLoop1st].mCrc[0U];
					break;
					
				case ecRfDrvGw_Hs_ReqCmd_OpeHis:
					wkCrc[1U] = vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[wkLoop1st].mCrc[1U];
					wkCrc[0U] = vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[wkLoop1st].mCrc[0U];
					break;
					
				case ecRfDrvGw_Hs_ReqCmd_SysHis:
					wkCrc[1U] = vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[wkLoop1st].mCrc[1U];
					wkCrc[0U] = vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[wkLoop1st].mCrc[0U];
					break;
			}
			if((wkCrc[1U] == (uint8_t)(CRCD) ) && (wkCrc[0U] == (uint8_t)(CRCD >> 8U) ) )
			{
				/* �G���[�������� Ack���� */
//				vrfTest++;
//				vrfCorrectSetCnt++;
			}else
			{
				/* �ʐM���s �p�P�b�g�j�� Nack���� */
				wkReturn = ecRfDrv_Fail;
//				vrfTestCrcErrorCnt++;
			}
		}
	}
	
	return wkReturn;
}
#endif

#if(swLoggerBord == imEnable)
/*
 *******************************************************************************
* HSmode�w�b�_���@����������
*�@�X�e�[�^�X�ʐM�ȊO�̃w�b�_����
*  arLength : CRC���܂�BCH������
*******************************************************************************
*/
void SubRfDrv_Header_Cording( uint8_t arLength, uint8_t arBchOption )
{
	uint16_t	wkDataCount;
	uint16_t	wkResult;

	/* CRC���܂߂��f�[�^���̃`�F�b�N */
	/* CRC�Z�o */
	CRCD = 0x0000U;
	for( wkDataCount = 0U; wkDataCount < arLength; wkDataCount++ )
	{
		if( wkDataCount == 3U )
		{
			wkDataCount += 2U;
		}
		CRCIN = vutRfDataBuffer.umData[wkDataCount];
	}
	wkResult = CRCD;
	
	/* �f�[�^��4,5byte�ڂ�CRC�ǉ� */
	vutRfDataBuffer.umData[3U] = (uint8_t)( (wkResult >> 8U) & 0x00FF );
	vutRfDataBuffer.umData[4U] = (uint8_t) wkResult;

	if( arBchOption == BCH_ON )
	{
		SubRfDrv_Coding_Bch( &vutRfDataBuffer.umData[0U], arLength, arLength );
	}
}

/*
*******************************************************************************
* HSmode Payload���@����������
*  arPayloadLen : ���f�[�^��byte��
*  arBchFrameNum : ���f�[�^+BCH��1�t���[���Ƃ��āA���̌J��Ԃ���
*  arCrcFrameNum : arBchFrameNum  arCrcFrameNum���Ƃ�CRC�t��
*  arHeaderNum : Header��byte���@bch,crc�܂�
*******************************************************************************
*/
void SubRfDrv_Payload_Cording( uint8_t arPayloadLen, uint8_t arBchFrameNum, uint8_t arCrcFrameNum, uint8_t arHeaderNum )
{
	uint8_t wkBch;
	uint8_t wkLoop1st;
	uint8_t wkLoop2nd;
	uint8_t wkFunc;
	uint8_t wkCrc[2U];
//	ET_RfDrv_ResInfo_t wkReturn;
	uint16_t wkU16;
	

	wkBch = arPayloadLen / 2U;
//	wkReturn = ecRfDrv_Success;
	
	for( wkLoop1st = 0U; wkLoop1st < arCrcFrameNum; wkLoop1st++ )		/* 10�v��data���Ƃ�CRC�t�� �ő�10�u���b�N(�v100data) */
	{
		/* CRC */
		CRCD = 0x0000U;
		for( wkLoop2nd = 0U; wkLoop2nd < arBchFrameNum; wkLoop2nd++ )
		{
			for( wkFunc = 0U; wkFunc < arPayloadLen; wkFunc++ )
			{
				wkU16 = (wkLoop1st * ((arPayloadLen + wkBch) * arBchFrameNum + 2U)) + arHeaderNum + wkFunc + (wkLoop2nd * (arPayloadLen + wkBch));
//				wkU16 = vutRfDataBuffer.umData[(wkLoop1st * ((arPayloadLen + wkBch) * arBchFrameNum + 2U)) + arHeaderNum + wkFunc + (wkLoop2nd * (arPayloadLen + wkBch))];
				CRCIN = vutRfDataBuffer.umData[ wkU16 ];
			}
		}
		wkCrc[1U] = (uint8_t)(CRCD);
		wkCrc[0U] = (uint8_t)(CRCD >> 8U);
//		wkU16 = ((wkLoop1st + 1U) * ((arPayloadLen + wkBch) * arBchFrameNum + 2U)) + arHeaderNum - 2U;
		vutRfDataBuffer.umData[ ((wkLoop1st + 1U) * ((arPayloadLen + wkBch) * arBchFrameNum + 2U)) + arHeaderNum - 2U ] = wkCrc[0U];
		vutRfDataBuffer.umData[ ((wkLoop1st + 1U) * ((arPayloadLen + wkBch) * arBchFrameNum + 2U)) + arHeaderNum + 1U -2U ] = wkCrc[1U];
#if 0
		switch( vrfHsReqCmdCode )
		{
			case ecRfDrvGw_Hs_ReqCmd_Measure:
				vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[wkLoop1st].mCrcMeasure[1U] = wkCrc[1U];
				vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[wkLoop1st].mCrcMeasure[0U] = wkCrc[0U];
				break;
			
			case ecRfDrvGw_Hs_ReqCmd_AlartHis:
				vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[wkLoop1st].mCrc[1U] = wkCrc[1U];
				vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[wkLoop1st].mCrc[0U] = wkCrc[0U];
				break;
				
			case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
				vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[wkLoop1st].mCrc[1U] = wkCrc[1U];
				vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[wkLoop1st].mCrc[0U] = wkCrc[0U];
				break;
				
			case ecRfDrvGw_Hs_ReqCmd_EventHis:
				vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[wkLoop1st].mCrc[1U] = wkCrc[1U];
				vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[wkLoop1st].mCrc[0U] = wkCrc[1U];
				break;
				
			case ecRfDrvGw_Hs_ReqCmd_OpeHis:
				vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[wkLoop1st].mCrc[1U] = wkCrc[1U];
				vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[wkLoop1st].mCrc[0U] = wkCrc[0U];
				break;
				
			case ecRfDrvGw_Hs_ReqCmd_SysHis:
				vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[wkLoop1st].mCrc[1U] = wkCrc[1U];
				vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[wkLoop1st].mCrc[0U] = wkCrc[0U];
				break;
		}
#endif
		/* BCH */
		for( wkLoop2nd = 0U; wkLoop2nd < arBchFrameNum; wkLoop2nd++ )
		{
			wkU16 = ( wkLoop1st * ((arPayloadLen + wkBch) * arBchFrameNum + 2U) ) + arHeaderNum + ( wkLoop2nd * (arPayloadLen + wkBch) );
			SubRfDrv_Coding_Bch( &vutRfDataBuffer.umData[ wkU16 ], arPayloadLen, arPayloadLen );
#if 0
			switch( vrfHsReqCmdCode )
			{
				case ecRfDrvGw_Hs_ReqCmd_Measure:
//					SubRfDrv_Coding_Bch( &vutRfDataBuffer.umData[arHeaderNum + wkLoop2nd * (arPayloadLen + wkBch)], arPayloadLen, arPayloadLen );
					vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[wkLoop1st].mMeasure[wkLoop2nd], arPayloadLen, arPayloadLen);
					break;
					
				case ecRfDrvGw_Hs_ReqCmd_AlartHis:
					vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[wkLoop1st].mAlHist[wkLoop2nd], arPayloadLen, arPayloadLen);
					break;
					
				case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
					vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[wkLoop1st].mAbnormalHist[wkLoop2nd], arPayloadLen, arPayloadLen);
					break;
					
				case ecRfDrvGw_Hs_ReqCmd_EventHis:
					vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[wkLoop1st].mMeas_Evt[wkLoop2nd], arPayloadLen, arPayloadLen);
					break;
					
				case ecRfDrvGw_Hs_ReqCmd_OpeHis:
					vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[wkLoop1st].mOpeHist[wkLoop2nd], arPayloadLen, arPayloadLen);
					break;
					
				case ecRfDrvGw_Hs_ReqCmd_SysHis:
					vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( (uint8_t *)&vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[wkLoop1st].mSysHist[wkLoop2nd], arPayloadLen, arPayloadLen);
					break;
			}
#endif
		}
	}
}
#endif


#if (swKouteiMode == imEnable)
/*
 *******************************************************************************
 *	PN9���M�֐�
 *	[���e]
 *******************************************************************************
 */
void SubRFDrv_PN9LoRa( uint8_t wkCH )
{
//	uint8_t wkRadio;
	
	rfStatus.Modem = MODEM_LORA;
	SubSX1272Sleep();
	SubRfDrv_PwChange( gvrfStsEventFlag.mRtmodePwmode );
	SubSX1272Stdby();
	SubSX1272FreqSet( wkCH, rfFreqOffset );
	SubSX1272Write( REG_LR_PAYLOADLENGTH, 64U );
	SubSX1272Write( REG_LR_MODEMCONFIG2, RFLR_MODEMCONFIG2_SF_8 | RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_ON | RFLR_MODEMCONFIG2_AGCAUTO_ON | RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB );
	/* CH�Z�b�g�A�b�v���K�v */
	SubSX1272Write( REG_LR_FIFOTXBASEADDR, 0x00 );				// Full buffer used for Tx
	SubSX1272Write( REG_LR_FIFOADDRPTR, 0x00 );
	SubSX1272WriteFifo( (uint8_t *)PN9, 64U );
//	SubSX1272WriteFifo( &PN9[0U], 64U );
	SubSX1272Tx( 64U );
}
#endif

/*
*******************************************************************************
*	SX1272 Sleep�J��
*
*	[���e]
*		SX1272��Speel���������d����Ԃɂ���B
*	FSK���[�h�ɑJ�ځB���W�X�^�͕ێ������B
*******************************************************************************
*/
void SubSX1272Sleep(void){

//	uint16_t wkTest = 0U;
//	uint8_t wkModeReady = 0U;
	
	/* Sleep Mode �J�� */
	if( rfStatus.Modem == MODEM_FSK )
	{
		SubSX1272Write( REG_OPMODE ,
			RF_OPMODE_LONGRANGEMODE_OFF |
			RF_OPMODE_MODULATIONTYPE_FSK |
			RF_OPMODE_MODULATIONSHAPING_11 |
			RF_OPMODE_SLEEP);
	}
	else
	{
		SubSX1272Write( REG_LR_OPMODE ,
			RF_OPMODE_LONGRANGEMODE_ON |
			RFLR_OPMODE_ACCESSSHAREDREG_DISABLE |
			RFLR_OPMODE_SLEEP );
	}
#if (swLoggerBord == imEnable)
	RADIO_SW = RF_SW_OFF;
#endif
}


/*
*******************************************************************************
*	SX1272 FSK���[�hStdby�J��
*
*	[���e]
*		SX1272��FSK���[�h�ŃX�^���o�C��ԂɑJ�ځB
*******************************************************************************
*/
void SubSX1272Stdby(void){
	
	if( rfStatus.Modem == MODEM_FSK )
	{
		/* FSK,Stdby mode */
		SubSX1272Write( REG_OPMODE ,
			RF_OPMODE_LONGRANGEMODE_OFF |
			RF_OPMODE_MODULATIONTYPE_FSK |
			RF_OPMODE_MODULATIONSHAPING_11 |
			RF_OPMODE_STANDBY);
	}
	else
	{
		/* LoRa,Stdby mode */
		SubSX1272Write( REG_LR_OPMODE ,
			RF_OPMODE_LONGRANGEMODE_ON |
			RFLR_OPMODE_ACCESSSHAREDREG_DISABLE |
			RFLR_OPMODE_STANDBY );
	}
	
	
#if (swLoggerBord == imEnable)
	RADIO_SW = RF_SW_OFF;
#endif
}


/*
*******************************************************************************
*	SX1272 Tx�J��
*
*	[���e]
*		SX1272��FSK/LoRa���[�h�ő��M��ԂɑJ�ځB
*	Tx���[�h,RF_SW��ON,�f�[�^���Z�b�g�B
* [����]
*	FSK�p�P�b�g���@wkDataLength�@1�`2047�@����M����RAM�e�ʂɂ��ˑ�
*******************************************************************************
*/
void SubSX1272Tx( uint16_t wkDataLength ){
	
//	uint8_t wkModeReady = 0U;
	
	if( rfStatus.Modem == MODEM_FSK )
	{
		/* �����͈͊O�̏ꍇ��Length�ݒ�X�L�b�v */
		if( (wkDataLength > 0U) && (wkDataLength < 2048U) )
		{
			/* Length�ݒ� */
			SubSX1272Write( REG_PAYLOADLENGTH , (uint8_t)(wkDataLength & 0x00FF) );
			SubSX1272Write(
				REG_PACKETCONFIG2 ,
				RF_PACKETCONFIG2_DATAMODE_PACKET
				| RF_PACKETCONFIG2_IOHOME_OFF
				| RF_PACKETCONFIG2_BEACON_OFF
				| (uint8_t)((wkDataLength >> 8U) & 0x0007)
			);/* ����3bit��Length */
		}
		
		/* FSK,Tx mode */
		SubSX1272Write( REG_OPMODE ,
			RF_OPMODE_LONGRANGEMODE_OFF |
			RF_OPMODE_MODULATIONTYPE_FSK |
			RF_OPMODE_MODULATIONSHAPING_11 |
			RF_OPMODE_TRANSMITTER);
	}
	else
	{
		if( (wkDataLength > 0U) && (wkDataLength < 257U) )
		{
			/* Length�ݒ� */
			SubSX1272Write( REG_LR_PAYLOADLENGTH, wkDataLength );
		}
		SubSX1272Write( REG_LR_OPMODE ,
			RF_OPMODE_LONGRANGEMODE_ON |
			RFLR_OPMODE_ACCESSSHAREDREG_DISABLE |
			RFLR_OPMODE_TRANSMITTER);
	}
	
	RADIO_SW = RF_SW_ON;
	
	RTCMK = 0U;				/* �L�����A�Z���X�I�����RTC���荞�݉��� */
}


/*
 *******************************************************************************
 *	SX1272 1�A�h���X�������݊֐�
 *
 *	[���e]
 *		1�f�[�^���w��A�h���X��Write
 *******************************************************************************
 */
void SubSX1272Write( uint8_t addr, uint8_t tx_data )
{
	SubSX1272WriteBuffer( addr, &tx_data, 1U );
}


/*
 *******************************************************************************
 *	SX1272 1�A�h���X�ǂݍ��݊֐�
 *
 *	[���e]
 *		1�f�[�^���w��A�h���Xaddr����Read / uint8��Return
 *******************************************************************************
 */
uint8_t SubSX1272Read( uint8_t addr )
{
	uint8_t data;
	SubSX1272ReadBuffer( addr, &data, 1U );
	return data;
}

/*
 *******************************************************************************
 *	SX1272 �V�[�P���V����Write�֐�
 *
 *	[���e]
 *		�w��A�h���X����T�C�Y���̘A����������
 *******************************************************************************
 */
void SubSX1272WriteFifo( uint8_t *tx_buffer, uint8_t size )
{
	/* FIFO�A�h���X0x00 */
	SubSX1272WriteBuffer( REG_FIFO, tx_buffer, size );
}


/*
 *******************************************************************************
 *	SX1272 Write�֐�
 *
 *	[���e]
 *		�w��A�h���X����T�C�Y���̔z��f�[�^��������
 *		��ʊ֐��FSX1272Write / SubSX1272WriteFifo
 *******************************************************************************
 */
void SubSX1272WriteBuffer( uint8_t addr, uint8_t *tx_buffer, uint8_t size )
{
	uint8_t rx_dummy;										/* Write���̎�M�f�[�^ �O��ݒ�lRead */
	uint8_t wkStatus;
	uint8_t wkLoopCount;
	
	addr |= 0x80U;										/* Write�t���Oset */
	
	/* NSS = 0; */
	RADIO_NSS = 0U;
	
	NOP();
	
	wkStatus = R_CSI10_Send_Receive( &addr , 1 , &rx_dummy );					/* �A�h���Xwrite */
	
	switch( wkStatus )
	{
		case MD_OK :									/* ���폈�� */
			for ( wkLoopCount = 1 ; wkLoopCount <= size ; wkLoopCount ++ )
			{
				R_CSI10_Send_Receive( tx_buffer , 1 , &rx_dummy);			/* 1�f�[�^�̑��� */
				
				/* ���M�����t���O�҂� */
#if 0
				while( (gvCsi10SndEndFlg != imON) | (SSR10 & 0x0040) )
#endif
				while( SSR10 & 0x0060 )
				{
					NOP();
				}
				tx_buffer ++;								/* rx_buffer�͔j��(������) */
			}
			break;
		default:										/* �G���[���[�`�� */
			break;
	}
	
#if (swLoggerBord == imEnable)
	/* DO = 0 */
//	RADIO_MISO = 0U;
#endif
	
	/* NSS = 1; */
	RADIO_NSS = 1U;
	NOP();
}


/*
 *******************************************************************************
 *	SX1272 Read�֐�
 *
 *	[���e]
 *		�w��A�h���X����T�C�Y��Read���A�z��f�[�^�n��
 *		��ʊ֐��FSX1272Read
 *******************************************************************************
 */
void SubSX1272ReadBuffer( uint8_t addr, uint8_t *rx_buffer, uint8_t size )
{
	uint8_t tx_dummy;										/* Read���̃_�~�[���M�f�[�^ */
	uint8_t wkStatus;
	uint8_t wkLoopCount;
	
	addr &= 0x7FU;										/* Read�t���Oset */
	
	/* NSS = 0; */
	RADIO_NSS = 0U;
	
	NOP();
	NOP();
	NOP();
	
	wkStatus = R_CSI10_Send_Receive( &addr , 1U , rx_buffer );					/* �A�h���Xwrite */
	
	switch( wkStatus )
	{
		case MD_OK :									/* ���폈�� */
			for ( wkLoopCount = 1 ; wkLoopCount <= size ; wkLoopCount ++ )
			{
				R_CSI10_Send_Receive( &tx_dummy , 1U , rx_buffer);			/* 1�f�[�^�̑��� */

#if 0
				while( (gvCsi10SndEndFlg != imON) | (SSR10 & 0x0040) )
#endif
				while( SSR10 & 0x0060 )
				{
					/* ���M��M�����t���O�҂� */
					NOP();
				}
				rx_buffer ++;								/* tx_buffer�͔j��(������) */
			}
			break;
		default:										/* �G���[���[�`�� */
			NOP();
			break;
	}
	
	RADIO_NSS = 1U;
	NOP();
}


/*
 *******************************************************************************
 *	SX1272 �ϒ����[�h�ݒ�
 *
 *	[���e]
 *		RadioRegsInit��Modem���e�ɏ]����FSK / Lora���[�h�̐ݒ���s��
 *		Sleep�ɑJ�ځA���̃��W�X�^�͕ێ�
 *******************************************************************************
 */
void SubSX1272SetModem( RadioModems_t modem )
{
	switch( modem )
	{
		default:
		case MODEM_FSK:
			SubSX1272Write( REG_OPMODE, ( SubSX1272Read( REG_OPMODE ) & RF_OPMODE_MASK ) | RF_OPMODE_SLEEP );	/* ���샂�[�h�ȊO��ێ����ăX���[�v���� */
			SubSX1272Write( REG_OPMODE, RFLR_OPMODE_LONGRANGEMODE_OFF | RF_OPMODE_MODULATIONTYPE_FSK | RF_OPMODE_MODULATIONSHAPING_11 | RF_OPMODE_SLEEP );
			break;
			
		case MODEM_LORA:
			SubSX1272Write( REG_OPMODE, RFLR_OPMODE_LONGRANGEMODE_ON | RFLR_OPMODE_ACCESSSHAREDREG_DISABLE | RFLR_OPMODE_SLEEP );
			break;
	}
}

#if 0
/*****************************************************************************
	[Ӽޭ�ٖ�]	SubSX1272RegReadAll
	  [���{��]	���W�X�^���[�h����
		[�T�v]	�V
		[�@�\]	�V
		[����]	�Ȃ�
		[�o��]	�Ȃ�
		[���l]	�Ȃ�
		[���]
		[����]
*****************************************************************************/
void  SubSX1272RegReadAll( void )
{
	uint8_t wkReg[113U];
	uint8_t wkTest;
/*
	SubSX1272SetModem( MODEM_FSK );
	for(wkTest = 1U; wkTest < 114U; wkTest++)
	{
		wkReg[wkTest] = SubSX1272Read( wkTest );
	}
*/
	SubSX1272SetModem( MODEM_LORA );
	for(wkTest = 1U; wkTest < 114U; wkTest++)
	{
		wkReg[wkTest] = SubSX1272Read( wkTest );
	}
}
#endif

/*
********************************************************************************
*  SX1272 ���g���ݒ�
*
*  [���e]
*	 ���g���̐ݒ���s���B
*  LoRa / FSK ����(���W�X�^����)
*  [����]
*	 ���g��ch�ԍ�(24-61) BW200kHz / 62-77ch BW100kHz
*	 ���g������Freq_Offset 61.035[Hz/Count]
********************************************************************************
*/
void SubSX1272FreqSet( uint8_t ChNo, sint16_t Freq_Offset)
{
	uint32_t wkReg32;
	uint8_t wkReg8;
	
	/* 920.6MHz -> 24ch  LoRa FSK���[�h���� */
	wkReg32 = (uint32_t)RFLR_FRFMSB_920_6_MHZ << 16U;
	wkReg32 = wkReg32 + (((uint32_t)RFLR_FRFMID_920_6_MHZ) << 8U);
	wkReg32 = wkReg32 + (uint32_t)RFLR_FRFLSB_920_6_MHZ;
	
	/* 62ch����100kHz�P�� 62ch��61ch+150kHz */
	if( ChNo > 61U )
	{
		/* +50kHz */
		wkReg32 += 819U;
		if( (ChNo - 61U) % 2U)
		{
			/* +100kHz */
			wkReg32 += 1638U;
		}
		ChNo = 61U + (ChNo - 61U) / 2U;
	}
	
	wkReg32 += (int32_t)(16384 * (int32_t)(ChNo -24) / 5);			// 2^19 / 32MHz * ChBw(0.2MHz) * ��ChNo
	/* offset */
	/* ����ch���痣���Ɛ����U���q�̕΍����Y���������邪�����ł���͈� */
	wkReg32 += (int32_t) ( Freq_Offset );
	
	wkReg8 = (uint8_t)( (wkReg32 & 0xFF0000) >> 16U);
	SubSX1272Write( REG_FRFMSB, wkReg8);
	wkReg8 = (uint8_t)( (wkReg32 & 0x00FF00) >> 8U);
	SubSX1272Write( REG_FRFMID, wkReg8);
	wkReg8 = (uint8_t)(wkReg32 & 0x0000FF);
	SubSX1272Write( REG_FRFLSB, wkReg8);
}


/*
*******************************************************************************
*	SX1272 FSK���[�h ��M��DIO�Z�b�g
*
*	[���e]
*		FSK���[�h�ł̎�M���W�X�^�Z�b�g�B
*	DIO�}�b�s���O�AFIFO�X���b�V�����h�l�ݒ�
* [����]
*	��M�p�P�b�g�� wkDataLength 1�`2047  ��������wkFIFOData�̔z�񐔂Ɉˑ�
*	��M�^�C���A�E�g���ԁ@wkTimeOut�@1�`65,000	�P��msec
*******************************************************************************
*/
void SubSX1272RxFSK_DIO( void ){
	/*
	*  DIO0=PayloadReady / RADIO_DIO_0,��H�}��DIO1
	*  DIO1=FifoLevel / RF_FIFOTHRESH_FIFOTHRESHOLD_RX = 0x3E (+1)
	*  DIO2=SyncAddr /	R60���t�� Connect
	*  DIO3=FifoEmpty / R63���O�� Non Connect
	*  DIO4=Preamble
	*  DIO5=ModeReady / Non Connect
	*/
	
	uint8_t wkRadio;
	
	SubSX1272Write( REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_00 |
								   RF_DIOMAPPING1_DIO1_00 |
								   RF_DIOMAPPING1_DIO2_11 |
								   RF_DIOMAPPING1_DIO3_00);
	SubSX1272Write( REG_DIOMAPPING2, RF_DIOMAPPING2_DIO4_11 |
								  RF_DIOMAPPING2_DIO5_11 |
								  //RF_DIOMAPPING2_MAP_RSSI );
								  RF_DIOMAPPING2_MAP_PREAMBLEDETECT );
	SubSX1272Write( REG_FIFOTHRESH,RF_FIFOTHRESH_FIFOTHRESHOLD_RX );
	
	/* DIOMapping �o�[�X�g�������� */
	for( wkRadio = 3U; wkRadio < 7U; wkRadio++ )
	{
		SubSX1272Write( RadioRegsBroadcastInit[wkRadio].Addr, RadioRegsBroadcastInit[wkRadio].Value );
	}

}


/*
*******************************************************************************
*	SX1272 FSK���[�h �L�����A�Z���XDIO�Z�b�g
*
*	[���e]
*		FSK���[�h�ł̃L�����A�Z���X���̃��W�X�^�Z�b�g�B
*		DIO4_11 / RF_DIOMAPPING2_MAP_RSSI
* [����]
*
*******************************************************************************
*/
void SubSX1272CarrierSenseFSK_DIO( void )
{
	/*
	*  DIO0=PayloadReady / RADIO_DIO_0,��H�}��DIO1
	*  DIO1=FifoLevel / RF_FIFOTHRESH_FIFOTHRESHOLD_RX = 0x3E (+1)
	*  DIO2=SyncAddr /	R60���t�� Connect
	*  DIO3=FifoEmpty / R63���O�� Non Connect
	*  DIO4=Preamble
	*  DIO5=ModeReady / Non Connect
	*/
	SubSX1272Write( REG_DIOMAPPING2, RF_DIOMAPPING2_DIO4_11 |
								  RF_DIOMAPPING2_DIO5_11 |
								  RF_DIOMAPPING2_MAP_RSSI );
								  //RF_DIOMAPPING2_MAP_PREAMBLEDETECT );
}

/*
*******************************************************************************
*	SX1272 FSK���[�h ��MON
*
*	[���e]
*		FSK���[�h�Ŏ�M��ԑJ�ځBRF_SW��ON
*	SubSX1272RxFSK_DIO,SubSX1272FSK_Length�ł̐ݒ�ˑ�
* [����]
*
*******************************************************************************
*/
void SubSX1272RxFSK_On( void )
{
	/* RF_OPMODE_SYNTHESIZER_RX�̊����҂�������ꍇ��TIMEOUT�ݒ�Ɉˑ�����̂Œ��� */
	
	RADIO_SW = RF_SW_ON;
	
	SubSX1272Write( REG_OPMODE ,
		RF_OPMODE_LONGRANGEMODE_OFF |
		RF_OPMODE_MODULATIONTYPE_FSK |
		RF_OPMODE_MODULATIONSHAPING_11 |
		RF_OPMODE_RECEIVER);
		
}


/*
*******************************************************************************
*	SX1272 FSK���[�h ��MLength,Sync�Z�b�g
*
*	[���e]
*		FSK���[�h�ł̎�M���W�X�^�Z�b�g�B
*	Length�ݒ�ASyncWord�ݒ�
* [����]
*	��M�p�P�b�g�� arDataLength 1�`2047
*******************************************************************************
*/
void SubSX1272FSK_Length( uint16_t arDataLength )
{
	/* Length�ݒ� */
	SubSX1272Write( REG_PAYLOADLENGTH , (uint8_t)(arDataLength & 0x00FF) );
	SubSX1272Write(
		REG_PACKETCONFIG2 ,
		RF_PACKETCONFIG2_DATAMODE_PACKET
		| RF_PACKETCONFIG2_IOHOME_OFF
		| RF_PACKETCONFIG2_BEACON_OFF
		| (uint8_t)((arDataLength >> 8U) & 0x0007)
	);/* ����3bit��Length */
}


/*
*******************************************************************************
*	SX1272 FSK���[�hRx�J��
*
*	[���e]
*		SX1272��FSK���[�h�Ŏ�M��ԂɑJ�ځB
*		���O�Ɏ�M�^�C���A�E�g�p��TAU���N�������邱�ƁB
*		����Ɏ��O��DIO�ݒ肵�ARX���N�������邱�ƁB
*		RSSI�l�擾����(vrfRssi)
* [����]
*	��M�p�P�b�g�� arDataLength 1�`2047  �y�폜�z��������wkFIFOData�̔z�񐔂Ɉˑ�
*	��M�f�[�^�̓|�C���^��
*	�y�폜�z��M�^�C���A�E�g���ԁ@wkTimeOut�@1�`65,000	�P��msec
*******************************************************************************
*/
void SubSX1272RxFSK( uint16_t arDataLength )
{
//	uint8_t wkModeReady;
	uint16_t wkRadio = 0U;
//	uint8_t wkRadio2nd;
//	uint8_t vrfRssi;
	uint8_t wkExtTimeFlag = 0U;
	uint16_t wkDebug = 0U;
	
	/*
	*  DIO0=PayloadReady / RADIO_DIO_0,��H�}��DIO1
	*  DIO1=FifoLevel / RF_FIFOTHRESH_FIFOTHRESHOLD_RX = 0x3E (+1)
	*  DIO2=SyncAddr /	R60���t�� Connect
	*  DIO3=FifoEmpty / R63���O�� Non Connect
	*  DIO4=Preamble
	*  DIO5=ModeReady / Non Connect
	*/
	//SubSX1272RxFSK_DIO();
	SubSX1272FSK_Length( arDataLength );
	gvRfIntFlg.mRadioDio0 = 0U;
	gvRfIntFlg.mRadioDio1 = 0U;
	gvRfIntFlg.mRadioDio2 = 0U;
	gvRfIntFlg.mRadioDio4 = 0U;
	R_INTC6_Start();
	R_INTC4_Start();
	R_INTC3_Start();			/* Sync DIO2 */
	R_INTC1_Start();			/* Preamble DIO4 */
	
	/*-------------- ��M����FIFO��ꏈ�� ------------------*/
	/* PayloadReady���̃I�[�o�[�t���[�Ď� */
//	SubIntervalTimer_Sleep( 50U * 375U, 1U );
	vrfCurrentRssi = SubSX1272Read( REG_RSSIVALUE );	/* ��ǂ� */
	
	while( (!gvRfIntFlg.mRadioDio0) && (wkRadio < arDataLength) )
	{
//		M_HALT;
		/* FIFO Thrsh����œǂݍ���(���ۂ�+1) 0x3E(+1) */
		if( gvRfIntFlg.mRadioDio1 )
		{
//			vrfDebug[wkDebug] =  SubSX1272Read( REG_IRQFLAGS2 ) | 0x0100;
//			vrfDebugCntL[wkDebug] = gvRfTimingCounterL;
			wkDebug++;
#if swLoggerBord == imEnable
			if( vRfDrvMainLoggerSts == ecRfDrvMainSts_HSmode_BeaconResAckRxStart )
			{
				/* HSmode�ł�Ack�̓f�[�^�o�b�t�@�̏㏑����� */
				SubSX1272ReadBuffer( REG_FIFO, &vrfHsmodeAckBuff.umData[wkRadio], RF_FIFOTHRESH_FIFOTHRESHOLD_RX - 1U );
			}
			else
			{
				SubSX1272ReadBuffer( REG_FIFO, &vutRfDataBuffer.umData[wkRadio], RF_FIFOTHRESH_FIFOTHRESHOLD_RX - 1U );
			}
#else
			if( vRfDrvMainGwSts == ecRfDrvMainSts_HSmode_Lg_To_Gw_DataAckRx )
			{
				/* HSmode�ł�Ack�̓f�[�^�o�b�t�@�̏㏑����� */
				SubSX1272ReadBuffer( REG_FIFO, &vrfHsmodeAckBuff.umData[wkRadio], RF_FIFOTHRESH_FIFOTHRESHOLD_RX - 1U );
			}else
			{
				SubSX1272ReadBuffer( REG_FIFO, &vutRfDataBuffer.umData[wkRadio], RF_FIFOTHRESH_FIFOTHRESHOLD_RX - 1U );
			}
#endif
			wkRadio += (RF_FIFOTHRESH_FIFOTHRESHOLD_RX - 1U);
			gvRfIntFlg.mRadioDio1 = 0U;
		}
		if( gvRfIntFlg.mRadioTimer )
		{
			/* ���p�P�b�g��M���͎��ԉ��� */
			if( wkRadio > 1000U )
			{
				if( !wkExtTimeFlag )
				{
					SubIntervalTimer_Sleep( 8U * 375U, 1U );
//					gvRfIntFlg.mRadioTimer = 0U;
					wkExtTimeFlag = 1U;
//					vrfDebug[wkDebug] =  SubSX1272Read( REG_IRQFLAGS2 ) | 0x0200;
//					vrfDebugCntL[wkDebug] = gvRfTimingCounterL;
					wkDebug++;
				}else
				{
					/* �ă^�C���A�E�g */
//					vrfDebug[wkDebug] =  SubSX1272Read( REG_IRQFLAGS2 ) | 0x0300;
//					vrfDebugCntL[wkDebug] = gvRfTimingCounterL;
					wkDebug++;
					NOP();
					break;
				}
			}else
			{
				break;
			}
		}
		/* FIFO�~�ς̑҂����� */
	}
	/* Debug */
//	vrfDebug[wkDebug] =  SubSX1272Read( REG_IRQFLAGS2 ) | 0x0400;
//	vrfDebugCntL[wkDebug] = gvRfTimingCounterL;
	
	vrfCurrentRssi = SubRfDrv_GetRssi();
	
/*
	if( gvRfIntFlg.mRadioTimer )
	{
		SubIntervalTimer_Stop();
	}
*/
	/* RX�I�� / FIFO�c���� */
	SubSX1272Stdby();
	
	/* DIO2��FifoEmpty�ɕύX */
	SubSX1272Write( REG_DIOMAPPING1, (SubSX1272Read( REG_DIOMAPPING1 ) & RF_DIOMAPPING1_DIO1_MASK) | RF_DIOMAPPING1_DIO1_01 );
	gvRfIntFlg.mRadioDio1 = 0U;
	/* FIFO�c�̓ǂݏo�� */
	while( !gvRfIntFlg.mRadioDio1 && !gvRfIntFlg.mRadioTimer )
	{
		if( wkRadio > arDataLength )
		{
			NOP();
			break;
		}
#if swLoggerBord == imEnable
		if( vRfDrvMainLoggerSts == ecRfDrvMainSts_HSmode_BeaconResAckRxStart )
		{
			vrfHsmodeAckBuff.umData[wkRadio] = SubSX1272Read( REG_FIFO );
		}
		else
		{
			vutRfDataBuffer.umData[wkRadio] = SubSX1272Read( REG_FIFO );
		}
#else
		if( vRfDrvMainGwSts == ecRfDrvMainSts_HSmode_Lg_To_Gw_DataAckRx )
		{
			vrfHsmodeAckBuff.umData[wkRadio] = SubSX1272Read( REG_FIFO );
		}
		else
		{
			vutRfDataBuffer.umData[wkRadio] = SubSX1272Read( REG_FIFO );
		}
#endif
		
		wkRadio++;
	}
	
	/* Test */
	if( gvRfIntFlg.mRadioDio2 | gvRfIntFlg.mRadioDio4)
	{
		NOP();
	}
	
	/* RADIO_DIO_1�������1byte�ǂݏo�� */
/* �z���30�o�C�g�܂� */
	
	/* DIO�����Ƃɖ߂� */
	SubSX1272RxFSK_DIO();
	R_INTC6_Stop();
	R_INTC4_Stop();
	R_INTC3_Stop();
	R_INTC1_Stop();
}


/*
*******************************************************************************
*	SX1272 FSK���[�h���M����
*
*	[���e]
*		SX1272��FSK���[�h�ő��M�BvutRfDataBuffer����FIFO�ɓ]�����Ȃ��瑗�M�B
*	���O��vutRfDataBuffer�փf�[�^�Z�b�g���邱��
*	���g���APreamble,Syncword���Z�b�g�ς݂ł��邱��
*	���M��������TUA��STOP���Ȃ��B
* [����]
*	�p�P�b�g�� arDataLength 1�`2047
*******************************************************************************
*/
uint8_t SubSX1272TxFSK( uint16_t arDataLength )
{
	uint16_t wkHsmodeDataPointer;
	uint8_t wkReturn;
	
	wkHsmodeDataPointer = 0U;
	rfStatus.Modem = MODEM_FSK;
	SubSX1272TxFSK_DIO_240kbps();
	SubSX1272FSK_Length( arDataLength );
//	SubRfDrv_Hsmode_DataSet( &wkHsmodeDataSetPointer );
	
	if( arDataLength <= 64U )
	{
		SubSX1272Write( REG_FIFOTHRESH,RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY );
#if swLoggerBord == imEnable
		if( vRfDrvMainLoggerSts == ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxStart )
		{
			SubSX1272WriteFifo( &vrfHsmodeAckBuff.umData[ 0U ], arDataLength);
		}else
		{
			SubSX1272WriteFifo( &vutRfDataBuffer.umData[ 0U ], arDataLength);
		}
#else
		if( vRfDrvMainGwSts == ecRfDrvMainSts_HSmode_BeaconResAckTxStart )
		{
			SubSX1272WriteFifo( &vrfHsmodeAckBuff.umData[ 0U ], arDataLength);
		}
		else
		{
			SubSX1272WriteFifo( &vutRfDataBuffer.umData[ 0U ], arDataLength);
		}
#endif
		wkHsmodeDataPointer = arDataLength;
	}else
	{
		/* �v���l����ack�f�[�^��64byte�ȉ��ƂȂ邽��vrfHsmodeAckBuff�͏��� */
		SubSX1272WriteFifo( &vutRfDataBuffer.umData[ 0U ], 64U );
		wkHsmodeDataPointer += 64U;
	}
	wkReturn = SubSX1272Read( REG_IRQFLAGS2 );
	SubSX1272Tx( arDataLength );
	SubIntervalTimer_Sleep( 50U * 375U - 37U, 1U );		/* 49.9msec */
	gvRfIntFlg.mRadioDio0 = 0U;			/* PacketSent */
	R_INTC6_Start();
	
	while( wkHsmodeDataPointer < arDataLength )
	{
		/*FIFO�[�U*/
		gvRfIntFlg.mRadioDio1 = 0U;			/* FifoLevel */
		R_INTC4_Start();
		
		while( !gvRfIntFlg.mRadioDio1 && !gvRfIntFlg.mRadioTimer && !gvRfIntFlg.mRadioDio0 )
		{
			M_HALT;
		}
		R_INTC4_Stop();
		if( gvRfIntFlg.mRadioTimer )
		{
			/* ���M���ԃ^�C���A�E�g */
			SubSX1272Sleep();
			wkHsmodeDataPointer = arDataLength;
			wkReturn = FAIL;
		}else if( gvRfIntFlg.mRadioDio0 )
		{
			/* ���M����(����Ȃ��͂�) */
			SubSX1272Sleep();
//			SubIntervalTimer_Stop();
			wkHsmodeDataPointer = arDataLength;
			wkReturn = FAIL;
		}else
		{
			/* FIFO TH�t���O */
			R_INTC4_Stop();
			if( (arDataLength - wkHsmodeDataPointer) > (63U - RF_FIFOTHRESH_FIFOTHRESHOLD_TX))
			{
				SubSX1272WriteFifo( &vutRfDataBuffer.umData[wkHsmodeDataPointer], 63U - RF_FIFOTHRESH_FIFOTHRESHOLD_TX);
				wkHsmodeDataPointer += (63U - RF_FIFOTHRESH_FIFOTHRESHOLD_TX);
			}else
			{
				SubSX1272WriteFifo( &vutRfDataBuffer.umData[wkHsmodeDataPointer], arDataLength - wkHsmodeDataPointer );
				wkHsmodeDataPointer = arDataLength;
			}
		}
	}
	SubSX1272Write( REG_FIFOTHRESH,RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY );
	/* ���M�����҂� */
	while( !gvRfIntFlg.mRadioTimer && !gvRfIntFlg.mRadioDio0 )
	{
		M_HALT;
	}
	if( gvRfIntFlg.mRadioDio0 )
	{
		/* ���M���� */
		SubSX1272Sleep();
#if (swLoggerBord == imEnable)
		if( vRfDrvMainLoggerSts != ecRfDrvMainSts_HSmode_BeaconResTxStart )
		{
			SubIntervalTimer_Stop();
		}
#else
		SubIntervalTimer_Stop();
#endif
		R_INTC6_Stop();
		R_INTC4_Stop();
		wkReturn = SUCCESS;
	}
	else
	{
		/* ���M�^�C���A�E�g */
		wkReturn = SubSX1272Read( REG_IRQFLAGS2 );
		SubSX1272Sleep();
		R_INTC6_Stop();
		R_INTC4_Stop();
		wkReturn = FAIL;
	}
	return wkReturn;
}


/*
*******************************************************************************
*	SX1272 FSK���[�h DIO�A���g���΍��A�r�b�g���[�g 250kbps�ݒ�
*
*	[���e]
*		SX1272 Broadcast����DIO��ݒ肷��BFSK���[�h
*		 DIO0=PacketSent / RADIO_DIO_0 => ��H�}��DIO1 INT6
*		 DIO1=FifoLevel INT4
*		 DIO2=FifoFull / R60���t�� Connect
*		 DIO3=FifoEmpty / R63���O�� Non Connect
*		 DIO4=LowBat
*		 DIO5=ModeReady / Non Connect
*	���g���΍�500kHz�@�r�b�g���[�g250kbps
*******************************************************************************
*/
void SubSX1272TxFSK_DIO_240kbps(void)
{
	uint8_t wkRadio;
	
	/* DIOMapping �o�[�X�g�������� */
	for( wkRadio = 0U; wkRadio < sizeof( RadioRegsBroadcastInit ) / sizeof( RadioRegisters_t ); wkRadio++ )
	{
		SubSX1272Write( RadioRegsBroadcastInit[wkRadio].Addr, RadioRegsBroadcastInit[wkRadio].Value );
	}
}


/*
********************************************************************************
* SX1272 FSK���[�h SyncWord�ݒ�
*
* [���e]
* Syncword 8byte�̐ݒ�
*
********************************************************************************
*/
void SubSX1272FSK_Sync( uint8_t *wkSyncWord )
{
	uint8_t wkRadio;
	
	SubSX1272Write( REG_SYNCCONFIG, (RF_SYNCCONFIG_AUTORESTARTRXMODE_WAITPLL_ON | RF_SYNCCONFIG_PREAMBLEPOLARITY_AA | RF_SYNCCONFIG_SYNC_ON | RF_SYNCCONFIG_FIFOFILLCONDITION_AUTO | (RF_FSK_SYNCWORD_LENGTH_FSK - 1U)));
	/* REG_SYNCVALUE1 - REG_SYNCVALUE8�֏������� */
	for( wkRadio = 0U; wkRadio < RF_FSK_SYNCWORD_LENGTH_FSK; wkRadio++ )
	{
		SubSX1272Write( REG_SYNCVALUE1 + wkRadio, wkSyncWord[ wkRadio ] );
	}
}


/*
********************************************************************************
* SX1272 FSK���[�h Preamble�ݒ�
*
* [���e]
* Preamble�� 32byte�̐ݒ�
*�@��SX1272��0XFFFF�܂Őݒ��4symbol�I�[�o�[�w�b�h
*
********************************************************************************
*/
void SubSX1272FSK_Preamble( uint16_t wkPreambleLength )
{
//	uint8_t wkRadio;
	
	/* REG_SYNCVALUE1 - REG_SYNCVALUE8�֏������� */
	SubSX1272Write( REG_PREAMBLEMSB, (uint8_t) (wkPreambleLength >> 8U) );
	SubSX1272Write( REG_PREAMBLELSB, (uint8_t) wkPreambleLength );
}


/*
*******************************************************************************
*	SX1272 LoRa���[�h ���MDIO�Z�b�g
*
*	[���e]
*		LoRa���[�h�ł̑��M���̃��W�X�^�Z�b�g�B
*
* [����]
*
*******************************************************************************
*/
void SubSX1272TxLoRa_DIO( void )
{
	/*
	*  DIO0=TxDone / RADIO_DIO_0,��H�}��DIO1
	*  DIO1=�s��
	*  DIO2=FhssChageChanel /  R60���t�� Connect
	*  DIO3=CadDone / R63���O�� Non Connect
	*/
	
	SubSX1272Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00 );

}


/*
*******************************************************************************
*	SX1272 LoRa���[�h ��MDIO�Z�b�g
*
*	[���e]
*		LoRa���[�h�ł̎�M���̃��W�X�^�Z�b�g�B
*
* [����]
*
*******************************************************************************
*/
void SubSX1272RxLoRa_DIO( void )
{
	/*
	*  DIO0=RxDone / RADIO_DIO_0,��H�}��DIO1
	*  DIO1=RxTimeout / RF_FIFOTHRESH_FIFOTHRESHOLD_RX = 0x3E (+1)
	*  DIO2=FhssChageChanel /  R60���t�� Connect
	*  DIO3=CadDone / R63���O�� Non Connect
	*  DIO4=CadDetect
	*  DIO5=ModeReady / Non Connect
	*/
	SubSX1272Write( REG_LR_DIOMAPPING1,
			RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00 );
	SubSX1272Write( REG_LR_DIOMAPPING2,
			RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00 | RFLR_DIOMAPPING2_MAP_RSSI );
}


/*
*******************************************************************************
*	SX1272 LoRa���[�h CAD DIO�Z�b�g
*
*	[���e]
*		LoRa���[�h�ł�CAD���̃��W�X�^�Z�b�g�B
*
* [����]
*
*******************************************************************************
*/
void SubSX1272CadLoRa_DIO( void )
{
	/*
	*  DIO0=CadDone / RADIO_DIO_0,��H�}��DIO1
	*  DIO1=RxTimeout / RF_FIFOTHRESH_FIFOTHRESHOLD_RX = 0x3E (+1)
	*  DIO2=FhssChageChanel /  R60���t�� Connect
	*  DIO3=CadDone / R63���O�� Non Connect
	*  DIO4=CadDetect
	*  DIO5=ModeReady / Non Connect
	*/
	SubSX1272Write( REG_LR_DIOMAPPING1,
			RFLR_DIOMAPPING1_DIO0_10 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00 );
	SubSX1272Write( REG_LR_DIOMAPPING2,
			RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00 | RFLR_DIOMAPPING2_MAP_RSSI );
}

#if (swRfTxTest == imDisable) && (swRfRxTest == imDisable)
/*
*******************************************************************************
*	SX1272 LoRa���[�h CAD�N��
*
*	[���e]
*		LoRa���[�h�ł�CAD�N��
*	���g���ݒ�͍s��Ȃ�
*	mRadioDio0:CAD Done
*	mRadioDio4:CAD Detect
*	mRadioTimer:CAD�N���^�C���A�E�g
* [����]
*
*******************************************************************************
*/
void SubSX1272CadLoRa( void )
{
//	uint8_t wkTest;
	
	SubSX1272Stdby();
	SubSX1272CadLoRa_DIO();
	
	RADIO_SW = RF_SW_ON;
	
	SubSX1272Write( REG_LR_OPMODE ,
		RF_OPMODE_LONGRANGEMODE_ON | RFLR_OPMODE_ACCESSSHAREDREG_DISABLE |
		RFLR_OPMODE_CAD );
	
	gvRfIntFlg.mRadioDio0 = 0U;
	gvRfIntFlg.mRadioDio4 = 0U;
	gvRfIntFlg.mRadioTimer = 0U;
//	wkTest = 0U;
	R_INTC6_Start();
	R_INTC1_Start();
#if swLoggerBord == imEnable
	SubIntervalTimer_Sleep( 1875U, 1U );		/* 5.0msec */
#else
	SubIntervalTimer_Sleep( RF_CAD_RSSI_TIME, 1U );
	while( !gvRfIntFlg.mRadioTimer )
	{
		M_HALT;
	}
	SubIntervalTimer_Stop();
	vrfCurrentRssi = 139U - SubSX1272Read( REG_LR_RSSIVALUE );
	SubIntervalTimer_Sleep( 1875U - RF_CAD_RSSI_TIME, 1U );
#endif
	while( !(gvRfIntFlg.mRadioDio4 | gvRfIntFlg.mRadioDio0 | gvRfIntFlg.mRadioTimer) )
	{
#if swLoggerBord == imEnable
		M_HALT;
//		wkTest++;			/* Test */
#else
//		wkTest++;			/* Test */
#endif
	}
	SubIntervalTimer_Stop();
	R_INTC6_Stop();
	R_INTC1_Stop();
}


/*
*******************************************************************************
*	SX1272 LoRa���[�h Tx���W�X�^�ݒ�
*
*	[���e]
*		LoRa���[�h�ł̑��M���W�X�^�ݒ�(���g���ݒ�Ȃ�)
*
* [����]
*�@�@�y�C���[�h��/�v���A���u����/���M���镄����(1/4or3/4)
*******************************************************************************
*/
void SubRFDrv_LoRa_Tx_Setting(uint8_t arPayLoadLength, uint8_t arPreambleLength, uint8_t arTxMode)
{
	SubSX1272Sleep();
	rfStatus.Modem = MODEM_LORA;
	SubSX1272Sleep();
	SubSX1272TxLoRa_DIO();
	SubSX1272Write( REG_LR_FIFOTXBASEADDR, 0x00 );				// Full buffer used for Tx
	SubSX1272Write( REG_LR_PAYLOADLENGTH, arPayLoadLength );
	SubSX1272Write( REG_LR_PREAMBLELSB, arPreambleLength);
	SubSX1272Write( REG_LR_FIFOADDRPTR, 0x00 );
	SubSX1272Write( REG_LR_MODEMCONFIG1, RFLR_MODEMCONFIG1_BW_125_KHZ
		 | arTxMode | RFLR_MODEMCONFIG1_IMPLICITHEADER_ON
		 | RFLR_MODEMCONFIG1_RXPAYLOADCRC_OFF | RFLR_MODEMCONFIG1_LOWDATARATEOPTIMIZE_OFF);
}

/*
*******************************************************************************
*	SX1272 LoRa���[�h Rx���s
*
*	[���e]
*		LoRa���[�h�ł̎�M���W�X�^�ݒ�(���g���ݒ�Ȃ�)���A��M���J�n����
*		mRadioDio0:��M����
* [����]
*�@�@�y�C���[�h��/�v���A���u����/���M���镄����(1/4or3/4)
*******************************************************************************
*/
void SubRFDrv_LoRa_Rx_Start(uint8_t arPayLoadLength, uint8_t arPreambleLength, uint8_t arRxMode)
{
	uint8_t wkRfFuncWork;
	uint8_t wkLoopCounter;
	
	SubSX1272Sleep();
	rfStatus.Modem = MODEM_LORA;
	SubSX1272Sleep();
	SubSX1272Stdby();
	SubSX1272Write( REG_LR_PAYLOADLENGTH, arPayLoadLength );
	SubSX1272Write( REG_LR_PREAMBLELSB, arPreambleLength);
	SubSX1272Write( REG_LR_FIFORXBASEADDR, 0x00 );
	SubSX1272Write( REG_LR_FIFOADDRPTR, 0x00 );
	SubSX1272Write( REG_LR_MODEMCONFIG1, RFLR_MODEMCONFIG1_BW_125_KHZ
		 | arRxMode | RFLR_MODEMCONFIG1_IMPLICITHEADER_ON
		 | RFLR_MODEMCONFIG1_RXPAYLOADCRC_OFF | RFLR_MODEMCONFIG1_LOWDATARATEOPTIMIZE_OFF);
	
	SubSX1272RxLoRa_DIO();
	SubSX1272RxLoRa_Continu();
	
	/* Rx Done Wait */
	gvRfIntFlg.mRadioDio0 = 0U;
	R_INTC6_Start();
	
	/* RX on-going Flag */
	wkLoopCounter = 20U;			/* ����4�`5Loop */
	wkRfFuncWork = 0U;
	while( (wkRfFuncWork != 0x04U) && (wkLoopCounter > 0U) )
	{
		wkLoopCounter--;
		wkRfFuncWork = SubSX1272Read( REG_LR_MODEMSTAT );
	}
}

/*
*******************************************************************************
*  RSSI�l���擾���Ԃ�
*  [�T�v]
*  RSSI�l��dBm�P��(��Βl)�ŕԂ��BFSK/Lora��rfStatus.Modem�Ŕ��f
*******************************************************************************
*/
uint8_t SubRfDrv_GetRssi( void )
{
	uint8_t wkReturn;

	if( rfStatus.Modem == MODEM_LORA )
	{
		wkReturn = SubSX1272Read( REG_LR_PKTRSSIVALUE );
		if( wkReturn < 140U )
		{
			wkReturn = 139U - wkReturn;
		}else
		{
			wkReturn = 139U;
		}
	}else
	{
		wkReturn = SubSX1272Read( REG_RSSIVALUE ) / 2U;
	}
	return wkReturn;
}

/*
*******************************************************************************
*	SX1272 LoRa���[�h ��M�f�[�^�ǂݍ���
*
*	[���e]
*		LoRa���[�h�ł̎�M�f�[�^��ǂݍ���
*		vutRfDataBuffer[0U]����f�[�^�z�u
*	Sleep�Ńf�[�^�������邽��Sleep�����ɔ�����
* [����]
*	��M�f�[�^��
*	RSSI�l
*******************************************************************************
*/
void SubRFDrv_LoRa_Rx_Read( uint8_t *arRxLength, uint8_t *arRssi )
{
	uint8_t wkRfFuncWork;
	
	wkRfFuncWork = SubSX1272Read( REG_LR_FIFORXCURRENTADDR );
	SubSX1272Write( REG_LR_FIFOADDRPTR, wkRfFuncWork );
	wkRfFuncWork = SubSX1272Read( REG_LR_RXNBBYTES );
	*arRxLength = wkRfFuncWork;
	SubSX1272ReadBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], wkRfFuncWork);
	/* RSSI */
	*arRssi = SubSX1272Read( REG_LR_PKTRSSIVALUE );
	if( *arRssi < 140U )
	{
		*arRssi = 139U - *arRssi;
	}else
	{
		*arRssi = 139U;
	}
}


/*
*******************************************************************************
*	SX1272 LoRa���[�h ��M �A�����[�h
*
*	[���e]
*		LoRa���[�h�ł̘A����M���̃��W�X�^�Z�b�g�B
*
* [����]
*
*******************************************************************************
*/
void SubSX1272RxLoRa_Continu( void )
{
	SubSX1272Write( REG_LR_OPMODE ,	RFLR_OPMODE_LONGRANGEMODE_ON
		| RFLR_OPMODE_ACCESSSHAREDREG_DISABLE | RFLR_OPMODE_RECEIVER);
	
	RADIO_SW = RF_SW_ON;
}
#endif

#if 0
/*
 *******************************************************************************
 *	�u���[�h�L���X�g���[�v�J�E���g�擾����
 *
 *	[���e]
 *		�u���[�h�L���X�g���[�v�J�E���g���f�N�������g���Ď擾����B
 *******************************************************************************
*/
uint8_t ApiRFDrv_BroadLoop( uint8_t arKind )
{
	static uint8_t	vLoop;
	
	switch( arKind )
	{
		case 0U:
			break;
		case 1U:
			vLoop --;
			break;
		case 2U:
			vLoop = 200U;
			break;
	}
	
	return vLoop;
}
#endif

/*
 *******************************************************************************
 *	RF(�����ʐM) �������X�e�[�^�X�擾����
 *
 *	[���e]
 *		RF(�����ʐM)�̏������X�e�[�^�X���擾����B
 *******************************************************************************
*/
ET_RFDrvInitSts_t ApiRFDrv_GetInitSts( void )
{
	return vRFPrm.mInitSts;
}


/*
 *******************************************************************************
 *	�����X���[�v���[�v�J�E���g�擾����
 *
 *	[���e]
 *		�����X���[�v���[�v�J�E���g���f�N�������g���Ď擾����B
 *******************************************************************************
*/
uint16_t ApiRFDrv_GetSleepLoop( void )
{
	if( vSleepLoop > 0 )
	{
		vSleepLoop--;
	}
	else
	{
		vSleepLoop = 0U;
	}
	return vSleepLoop;
}



#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	�����p�v���g�R���ɑ���l�A�x��l����������
 *
 *	[���e]
 *		�����p�v���g�R���ɑ���l�A�x��l����������
 *	[����]
 *		uint8_t	*arFromAlmParmArray: �x��l
 *		uint8_t	*arToAlmParmArray: �������̃v���g�R��
 *******************************************************************************
 */
void ApiRfDrv_SetMeaAlmValArray( sint16_t *arFromAlmParmArray, uint8_t *arToAlmParmArray )
{
	/* 1ch */
	arToAlmParmArray[ 0U ] = (uint8_t)(arFromAlmParmArray[ 0U ] & 0x00FF);
	arToAlmParmArray[ 1U ] = (uint8_t)((arFromAlmParmArray[ 0U ] & 0x1F00) >> 8U);
	/* 2ch */
	arToAlmParmArray[ 1U ] |= (uint8_t)((arFromAlmParmArray[ 1U ] & 0x0007) << 5U);
	arToAlmParmArray[ 2U ] = (uint8_t)((arFromAlmParmArray[ 1U ] & 0x07F8) >> 3U);
	arToAlmParmArray[ 3U ] = (uint8_t)((arFromAlmParmArray[ 1U ] & 0x1800) >> 11U);
	/* 3ch */
	arToAlmParmArray[ 3U ] |= (uint8_t)((arFromAlmParmArray[ 2U ] & 0x003F) << 2U);
	arToAlmParmArray[ 4U ] = (uint8_t)((arFromAlmParmArray[ 2U ] & 0x1FC0) >> 6U);
	/* 4ch */
	arToAlmParmArray[ 4U ] |= (uint8_t)((arFromAlmParmArray[ 3U ] & 0x0001) << 7U);
	arToAlmParmArray[ 5U ] = (uint8_t)((arFromAlmParmArray[ 3U ] & 0x01FE) >> 1U);
	arToAlmParmArray[ 6U ] = (uint8_t)((arFromAlmParmArray[ 3U ] & 0x1E00) >> 9U);
}


/*
 *******************************************************************************
 *	�����p�v���g�R���Ɍx��x���l����������
 *
 *	[���e]
 *		�����p�v���g�R���Ɍx��x���l����������
 *	[����]
 *		uint8_t	*arFromAlmParmArray: �x��x���l
 *		uint8_t	*arToAlmParmArray: �������̃v���g�R��
 *******************************************************************************
 */
static void SubRfDrv_SetAlmDelayArray( uint8_t *arFromAlmParmArray, uint8_t *arToAlmParmArray )
{
	arToAlmParmArray[ 0U ] = arFromAlmParmArray[ 0U ] & 0x3F;
	arToAlmParmArray[ 0U ] |= (arFromAlmParmArray[ 1U ] & 0x03) << 6U;
	arToAlmParmArray[ 1U ] = (arFromAlmParmArray[ 1U ] & 0x3C) >> 2U;
	arToAlmParmArray[ 1U ] |= (arFromAlmParmArray[ 2U ] & 0x0F) << 4U;
	arToAlmParmArray[ 2U ] = (arFromAlmParmArray[ 2U ] & 0x30) >> 4U;
	arToAlmParmArray[ 2U ] |= (arFromAlmParmArray[ 3U ] & 0x3F) << 2U;
}


/*
 *******************************************************************************
 *	�����p�v���g�R���Ɉ�E���e���Ԃ���������
 *
 *	[���e]
 *		�����p�v���g�R���Ɍx��x���l����������
 *	[����]
 *		uint8_t	*arToAlmParmArray: ��E���e����
 *******************************************************************************
 */
static void SubRfDrv_SetDevi( uint8_t *arToAlmParmArray )
{
	
	/* ��E���e���ԗL���I�� */
	arToAlmParmArray[ 0U ] = (gvInFlash.mParam.mDeviEnaLv[ 0U ] & 0x10) >> 4U;
	arToAlmParmArray[ 0U ] |= (gvInFlash.mParam.mDeviEnaLv[ 1U ] & 0x10) >> 3U;
	arToAlmParmArray[ 0U ] |= (gvInFlash.mParam.mDeviEnaLv[ 2U ] & 0x10) >> 2U;
	arToAlmParmArray[ 0U ] |= (gvInFlash.mParam.mDeviEnaLv[ 3U ] & 0x10) >> 1U;
	
	/* ���e���� */
	arToAlmParmArray[ 0U ] |= (uint8_t)((gvInFlash.mParam.mDeviEnaTime[ 0U ] & 0x000F) << 4U);
	arToAlmParmArray[ 1U ] = (uint8_t)((gvInFlash.mParam.mDeviEnaTime[ 0U ] & 0x0FF0) >> 4U);
	
	/* ��E���e���ԃ��x���I�� */
	arToAlmParmArray[ 2U ] = gvInFlash.mParam.mDeviEnaLv[ 0U ] & 0x03;
	arToAlmParmArray[ 2U ] |= (gvInFlash.mParam.mDeviEnaLv[ 1U ] & 0x03) << 2U;
	arToAlmParmArray[ 2U ] |= (gvInFlash.mParam.mDeviEnaLv[ 2U ] & 0x03) << 4U;
	arToAlmParmArray[ 2U ] |= (gvInFlash.mParam.mDeviEnaLv[ 3U ] & 0x03) << 6U;
	
	/* �N���A�_�@(��) */
	arToAlmParmArray[ 3U ] = (gvInFlash.mParam.mDeviClrTimeMin & 0x07) << 4U;
	/* �N���A�_�@(��) */
	arToAlmParmArray[ 3U ] |= (gvInFlash.mParam.mDeviClrTimeHour & 0x01) << 7U;
	arToAlmParmArray[ 3U ] |= (gvInFlash.mParam.mDeviClrTimeHour & 0x1E) >> 1U;

}
#endif

#if (swKouteiMode == imEnable)
/*
 *******************************************************************************
 * �������M�`���l���ݒ�
 *
 *	[���e]
 *		�������M�`���l����ݒ肷��B
 *	[����]
 *		uint16_t		arVal: �`���l��24�`77
 *		ET_SelRfCh_t	arSel: �������
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_SetCh( uint16_t arVal, ET_SelRfCh_t arSel )
{
	switch( arSel )
	{
		case ecSelRfCh_CurrentCh_LoRa:
			rfCurrentCh_LoRa = arVal;
			break;
		case ecSelRfCh_CurrentCh_FSK:
			rfCurrentCh_FSK = arVal;
			break;
		case ecSelRfCh_LoraChGrupe:
//			vrfLoraChGrupeCnt = arVal;
			gvInFlash.mParam.mrfLoraChGrupeCnt = arVal;
			vrfLoraChGrupeMes = arVal;
			break;
#if (swLoggerBord == imDisable)
		case ecSelRfCh_LoraChGrupeMes:
			vrfRtChSelect = gvInFlash.mParam.mrfRtChSelect;
			vrfLoraChGrupeMes = gvInFlash.mParam.mrfLoraChGrupeMes;
			break;
#endif
		default:
			break;
	}
}
#endif

#if (swKouteiMode == imEnable)
/*
 *******************************************************************************
 * �������M�`���l���ǂݏo��
 *
 *	[���e]
 *		�������M�`���l����ǂݏo���B
 *	[����]
 *		ET_SelRfCh_t: �������
 *	[�ߒl]
 *		uint16_t	wkVal: �`���l��24�`77
 *******************************************************************************
 */
uint16_t ApiRfDrv_GetCh( ET_SelRfCh_t arSel )
{
	uint16_t	wkVal;
	
	switch( arSel )
	{
		case ecSelRfCh_CurrentCh_LoRa:
			wkVal = rfCurrentCh_LoRa;
			break;
		case ecSelRfCh_CurrentCh_FSK:
			wkVal = rfCurrentCh_FSK;
			break;
		case ecSelRfCh_LoraChGrupe:
//			wkVal = vrfLoraChGrupeCnt;
			wkVal = gvInFlash.mParam.mrfLoraChGrupeCnt;
			break;
		default:
			break;
	}
	return wkVal;
}
#endif

#if (swLoggerBord == imDisable)
/*
 *******************************************************************************
 * �������[�h:�f�[�^�v���R�}���h�ݒ�
 *
 *	[���e]
 *		�������[�h�f�[�^�v���R�}���h��ݒ肷��
 *	[����]
 *		ET_RfDrv_GwReqCmd_t arCmd: �v���R�}���h
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_SetHSReqCmd( ET_RfDrv_GwReqCmd_t arCmd )
{
	vrfHsReqCmdCode = arCmd;
	gvrfStsEventFlag.mHsmodeInit = 0U;
	gvrfStsEventFlag.mReset = RFIC_INI;
}

/*
 *******************************************************************************
 * �������[�h:���O�C�����[�UID�ݒ�
 *
 *	[���e]
 *		�������[�h���O�C�����[�UID��ݒ肷��
 *	[����]
 *		ET_RfDrv_GwReqCmd_t arCmd: �v���R�}���h
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_SetHSLoginUserId( uint16_t arUserId )
{
	vrfHsLoginUserId = arUserId;
}

/*
 *******************************************************************************
 * �������[�h:�ďo���K�[�ݒ�
 *
 *	[���e]
 *		�������[�h�ďo���K�[ID��ݒ肷��B
 *	[����]
 *		uint16_t	arId: ID_0x00~0xFF
 *		uint8_t		arRefno: ���t�@�����X�ԍ�0~2
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_SetHSLoggerID( uint16_t arId, uint8_t arRefno )
{
	if( (arRefno <= 2U) && (arId <= 0xFF) )
	{
		vrfHSLoggerID[ arRefno ] = (uint8_t)arId;
	}
}


/*
 *******************************************************************************
 * �������[�h:�ďo���K�[�ǂݏo��
 *
 *	[���e]
 *		�������[�h�ďo���K�[ID��ǂݏo��
 *	[����]
 *		uint8_t		arRefno: ���t�@�����X�ԍ�0~2
 *	[�ߒl]
 *		uint16_t	vrfHSLoggerID[arRefno]: ID_0x00~0xFF
 *******************************************************************************
 */
uint16_t ApiRfDrv_GetHSLoggerID( uint8_t arRefno )
{
	return vrfHSLoggerID[ arRefno ];
}


/*
 *******************************************************************************
 * �������[�h�F�v���f�[�^�擪���n��ID
 *
 *	[���e]
 *		�������[�h�̗v���f�[�^�擪���n��ID(IndexNo.)��ݒ肷��B
 *	[����]
 *		uint16_t	arIndex: �擪���n��ID
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_SetHSIndexNo( uint16_t arIndex )
{
	vrfHSReqHeadMeaID = arIndex;
}


/*
 *******************************************************************************
 * �������[�h�F�v���f�[�^�擪����
 *
 *	[���e]
 *		�������[�h�̗v���f�[�^�擪������ݒ肷��B
 *		uint16_t	arHeadMeaTime: �擪����
 *		uint8_t		arRefno: ���t�@�����X�ԍ�0~2
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_SetHSHeadMeaTime( uint16_t arHeadMeaTime, uint8_t arRefno )
{
	if( arRefno <= 1U )
	{
		vrfHSReqHeadMeaTime[ arRefno ] = arHeadMeaTime;
	}
}


#endif


/*
 *******************************************************************************
 *	����RSSI�ǂݏo��
 *
 *	[���e]
 *		����RSSI��ǂݏo���B
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		uint8_t	vrfRssi: RSSI
 *******************************************************************************
 */
uint8_t ApiRfDrv_GetRssi( void )
{
	return vrfRssi;
}


#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	���A���^�C���ڑ����Z�b�g
 *
 *	[���e]
 *		���A���^�C���ڑ����Z�b�g
 *	[����]
 *		uint8_t	arSel: 0�F���ڑ��A1�F�ڑ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void SubRfDrv_SetRtConInfo( uint8_t arSel )
{
	if( arSel == imON )
	{
		if( vrfConnectSuccess == 0U )
		{
			gvModuleSts.mLcd = ecLcdModuleSts_Run;		/* LCD�\���X�V */
			ApiFlash_WriteSysLog( ecSysLogItm_RealTmCom, 0U );		/* �V�X�e�������F�������ʐM�ڑ� */
		}
		vrfConnectSuccess = 1U;
	}
	else
	{
		if( vrfConnectSuccess == 1U )
		{
			gvModuleSts.mLcd = ecLcdModuleSts_Run;		/* LCD�\���X�V */
			ApiTimeComp_TimeCompFlgClr();				/* ���v�␳�t���O���� */
			ApiFlash_WriteSysLog( ecSysLogItm_RealTmDisCom, 0U );	/* �V�X�e�������F�������ʐM�ؒf */
		}
//		vrfConnectSuccess = 0U;
		vrfRTmodeCommFailNum = vrfRTmodeCommCutTime;	/* �����I�� */
		
	}
}
#endif


#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	���A���^�C���ڑ����ǂݏo��
 *
 *	[���e]
 *		���A���^�C���ڑ����ǂݏo��
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		uint8_t	vrfConnectSuccess: 0�F���ڑ��A1�F�ڑ�
 *******************************************************************************
 */
uint8_t ApiRfDrv_GetRtConInfo( void )
{
//	return vrfConnectSuccess;
	return ( vrfConnectSuccess + gvrfStsEventFlag.mOnlineLcdOnOff );			/* 2022.9.16 �ʐM�J�n����LCD�_�� */
}
#endif

#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	�����ʐM�ڑ��\��
 *
 *	[���e]
 *		�����ʐM�ڑ��\��
 *	[����]
 *		uint8_t	arSel: 0�F���ڑ��A1�F�ڑ��A3�F�ǂݏo��
 *	[�ߒl]
 *		uint8_t	vrfHsStsDisp: 0�F���ڑ��A1�F�ڑ�
 *******************************************************************************
 */
uint8_t ApiRfDrv_HsStsDisp( uint8_t arSel )
{
	static uint8_t vrfHsStsDisp = imOFF;
	
	switch( arSel )
	{
		case 0U:
			if( vrfHsStsDisp == imON )
			{
				gvModuleSts.mLcd = ecLcdModuleSts_Run;		/* LCD�\���X�V */
			}
			vrfHsStsDisp = imOFF;
			break;
		case 1U:
			if( vrfHsStsDisp == imOFF )
			{
				gvModuleSts.mLcd = ecLcdModuleSts_Run;		/* LCD�\���X�V */
			}
			vrfHsStsDisp = imON;
			break;
		default:
		case 3U:
			break;
	}
	
	return vrfHsStsDisp;
}
#endif


#if (swKouteiMode == imEnable)
/*
 *******************************************************************************
 * �H���p������M���s
 *
 *	[���e]
 *		�H���p�̖�����M�����s����
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubRfDrv_TestRx( void )
{
	uint8_t		wkRadio;
	uint8_t		wkCarrerFlg;
	
#if (swLoggerBord == imEnable)
	rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
#else
	
#endif
	
	if( gvMode == ecMode_RfRxLoRa )
	{
		SubSX1272FreqSet( rfCurrentCh_LoRa, rfFreqOffset );			/* ���g���ݒ�:LoRa */
		rfStatus.Modem = MODEM_LORA;
	}
	else
	{
		SubSX1272FreqSet( rfCurrentCh_FSK, rfFreqOffset );			/* ���g���ݒ�:FSK */
		rfStatus.Modem = MODEM_FSK;
	}
	SubSX1272Sleep();
	SubSX1272CarrierSenseFSK_DIO();
	
	
	/* Carrire Sence 125usec */
	if( SubRFDrv_CarrierSens_FSK( CARRIRE_TIME ) == NO_CARRIER )
	{
		wkRadio = SubSX1272Read( REG_RSSIVALUE );
		wkCarrerFlg = imCareerFlgOff;
	}else
	{
		wkRadio = SubSX1272Read( REG_RSSIVALUE );
		wkCarrerFlg = imCarrerFlgOn;
	}
	SubSX1272Sleep();
	
	ApiRfDrv_GetSetRssiCareerFlg( imCarrerFlgWrite, wkCarrerFlg );	/* �L�����A�Z���X����l�ۑ� */
	ApiRfDrv_GetSetRssiCareerFlg( imRssiWrite, wkRadio );			/* RSSI�l�ۑ� */
	
	SubIntervalTimer_Sleep( 0xFFFF, 2U );
	gvModuleSts.mRf = ecRfModuleSts_Sleep;
}
#endif

#if (swKouteiMode == imEnable)
/*
****************************************************************************************************
*  ��M�d������
****************************************************************************************************
*/
void SubRfDrv_TestCurrentRx( void )
{
	rfStatus.Modem = MODEM_LORA;
	SubSX1272Sleep();
	SubRFDrv_LoRa_Rx_Start( 255U, 255U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
	SubIntervalTimer_Sleep( 100U * 375U, 20U );
	while( !gvRfIntFlg.mRadioTimer )
	{
		M_NOP;
	}
	SubSX1272Sleep();
}
#endif

#if (swKouteiMode == imEnable)
/*
 *******************************************************************************
 * ������M�e�X�g��RSSI�A�L�����A�Z���X����̓ǂݏ���
 *
 *	[���e]
 *		������M�e�X�g��RSSI�A�L�����A�Z���X����̓ǂݏ������s��
 *	[����]
 *		uint8_t	arSel	: 	0�cRSSI�̏����݁A1�cRSSI�̓Ǎ���
 *							2�c�L�����A�Z���X����l�����݁A3�c�L�����A�Z���X����l�Ǎ���
 *		uint8_t	arVal	: RSSI�l or �L�����A�Z���X����l
 *	[�ߒl]
 *		uint8_t			: RSSI�l or �L�����A�Z���X����l
 *******************************************************************************
 */
uint8_t ApiRfDrv_GetSetRssiCareerFlg( uint8_t arSel, uint8_t arVal )
{
	uint8_t				wkRet;
	static uint8_t		vRadio = 0U;
	static uint8_t		vCareerFlg = 0U;
	
	switch( arSel )
	{
		case imRssiWrite:
			vRadio = arVal;
			break;
		case imRssiRead:
			wkRet = vRadio;
			break;
		case imCarrerFlgWrite:
			vCareerFlg = arVal;
			break;
		case imCarrerFlgRead:
			wkRet = vCareerFlg;
			break;
	}
	return wkRet;
}
#endif

#if (swLoggerBord == imEnable)

/*
 *******************************************************************************
 *	�������ʐM�]���f�[�^�N�_�ɂ�鑗�M�|�C���^�̕ύX
 *
 *	[���e]
 *		�������ʐM�]���f�[�^�N�_��GW�ڑ����Ɏ�M���A���M�|�C���^�̕ύX����
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubRfDrv_ChgRealTimeSndPt( void )
{
	uint16_t	wkU16;
	uint16_t	wkRealTimeSndPt;
#if 0
	uint8_t		ConGwId[ 3U ];
	static uint8_t vPreGwId[ 3U ] = { 0x00, 0x00, 0x00 };
#endif
	
#if 0
	ConGwId[ 0U ] = ((vutRfDataBuffer.umRT_Gw_ConnAuth.mAppID_GwUniqueID[ 1U ] & 0x0F) << 4U) + ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[ 0U ] & 0xF0) >> 4U);
	ConGwId[ 1U ] = ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[ 0U ] & 0x0F) << 4U) + ((vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[ 1U ] & 0xF0) >> 4U);
	ConGwId[ 2U ] = (vutRfDataBuffer.umRT_Gw_ConnAuth.mGwUniqueID[ 1U ] & 0x0F) << 4U;
#endif
	
	
	/* �C�x���g�L�����瑗�M */
	if( gvInFlash.mParam.mRealTimeSndPt == 510U )
	{
		/* �C�x���g�L���|�C���^�����݂���Ȃ� */
		if( gvInFlash.mData.mMeas1_EvtPtrIndex != 0U )
		{
			gvInFlash.mData.mMeas1_OldPtrIndex = gvInFlash.mData.mMeas1_EvtPtrIndex;		/* �ߋ����M�|�C���^�ɃC�x���g�L���|�C���^���� */
//			gvInFlash.mData.mMeas1_EvtPtrIndex = 0U;										/* �C�x���g�L���|�C���^���N���A */
		}
		else
		{
			/* �ߋ����M�|�C���^���瑗�M */
		}
	}
	/* ���݂�10�`5000�f�[�^�O���瑗�M */
	else if( gvInFlash.mParam.mRealTimeSndPt >= 1U && gvInFlash.mParam.mRealTimeSndPt <= 500U )
	{
		wkRealTimeSndPt = gvInFlash.mParam.mRealTimeSndPt * 10U;							/* �ݒ�l1�`500�f�[�^��10�`5000�f�[�^�Ƃ��Ĉ��� */
	
#if 1
		if( gvInFlash.mData.mMeas1_Index == 0U ||
			gvInFlash.mData.mMeas1_OldPtrIndex == 0U ||
			gvInFlash.mData.mMeas1_Index == gvInFlash.mData.mMeas1_OldPtrIndex )
		{
			/* �ߋ����M�|�C���^���瑗�M */
		}
		else
		{
			if( gvInFlash.mData.mMeas1_Index > gvInFlash.mData.mMeas1_OldPtrIndex )
			{
				wkU16 = (gvInFlash.mData.mMeas1_Index - 1U) - gvInFlash.mData.mMeas1_OldPtrIndex;
				
				/* ���M�����ݒ萔���傫�� */
				if( wkU16 > wkRealTimeSndPt )
				{
					/* �ߋ����M�|�C���^�́A����Index����ݒ�����}�C�i�X�̈ʒu�ֈړ� */
					gvInFlash.mData.mMeas1_OldPtrIndex = (gvInFlash.mData.mMeas1_Index - 1U) - wkRealTimeSndPt;
				}
				else
				{
					/* �ߋ����M�|�C���^���瑗�M */
				}
			}
			else
			{
				wkU16 = (imIndexMaxMeas1 - 1U) + gvInFlash.mData.mMeas1_Index - gvInFlash.mData.mMeas1_OldPtrIndex;
				
				/* ���M�����ݒ萔���傫�� */
				if( wkU16 > wkRealTimeSndPt )
				{
					/* �ߋ����M�|�C���^�́A����Index����ݒ�����}�C�i�X�̈ʒu�ֈړ� */
					if( gvInFlash.mData.mMeas1_Index - 1U > wkRealTimeSndPt )
					{
						gvInFlash.mData.mMeas1_OldPtrIndex = (gvInFlash.mData.mMeas1_Index - 1U) - wkRealTimeSndPt;
					}
					else
					{
						gvInFlash.mData.mMeas1_OldPtrIndex = imIndexMaxMeas1 - (wkRealTimeSndPt - gvInFlash.mData.mMeas1_Index - 1U);
					}
				}
				else
				{
					/* �ߋ����M�|�C���^���瑗�M */
				}
			}
		}
#else
		/* ���^�f�[�^�������M�v���f�[�^���ɑ���Ȃ� */
		if( gvInFlash.mData.mMeas1_Num < wkRealTimeSndPt )
		{
			gvInFlash.mData.mMeas1_OldPtrIndex = 1;											/* �ߋ����M�|�C���^��1�ɂ��� */
		}
		else
		{
			/* �O��ڑ�����GW�ƈقȂ� */
			if( vPreGwId[ 0U ] != ConGwId[ 0U ] || vPreGwId[ 1U ] != ConGwId[ 1U ] || vPreGwId[ 2U ] != ConGwId[ 2U ] )
			{
				/* �ݒ����IndexNo.�̃����O�o�b�t�@���܂����Ȃ� */
				if( gvInFlash.mData.mMeas1_Index >= wkRealTimeSndPt )
				{
					/* �ߋ����M�|�C���^�́A����Index����ݒ�����}�C�i�X�̈ʒu�ֈړ� */
					gvInFlash.mData.mMeas1_OldPtrIndex = (gvInFlash.mData.mMeas1_Index - 1U) - wkRealTimeSndPt;
				}
				else
				{
					/* �ߋ����M�|�C���^�́A����Index����ݒ�����}�C�i�X�̈ʒu�ֈړ�(IndexNo.�̃����O�o�b�t�@�܂���) */
					gvInFlash.mData.mMeas1_OldPtrIndex = imIndexMaxMeas1 - (wkRealTimeSndPt - gvInFlash.mData.mMeas1_Index - 1U);
				}
			}
			/* �O��ڑ�����GW�Ɠ��� */
			else
			{
				if( gvInFlash.mData.mMeas1_Index > gvInFlash.mData.mMeas1_OldPtrIndex )
				{
					wkU16 = (gvInFlash.mData.mMeas1_Index - 1U) - gvInFlash.mData.mMeas1_OldPtrIndex;
					
					/* ���M�����ݒ萔���傫�� */
					if( wkU16 > wkRealTimeSndPt )
					{
						/* �ߋ����M�|�C���^�́A����Index����ݒ�����}�C�i�X�̈ʒu�ֈړ� */
						gvInFlash.mData.mMeas1_OldPtrIndex = (gvInFlash.mData.mMeas1_Index - 1U) - wkRealTimeSndPt;
					}
					else
					{
						/* �ߋ����M�|�C���^���瑗�M */
					}
				}
				else
				{
					wkU16 = (imIndexMaxMeas1 - 1U) + gvInFlash.mData.mMeas1_Index - gvInFlash.mData.mMeas1_OldPtrIndex;
					
					/* ���M�����ݒ萔���傫�� */
					if( wkU16 > wkRealTimeSndPt )
					{
						/* �ߋ����M�|�C���^�́A����Index����ݒ�����}�C�i�X�̈ʒu�ֈړ� */
						if( gvInFlash.mData.mMeas1_Index - 1U > wkRealTimeSndPt )
						{
							gvInFlash.mData.mMeas1_OldPtrIndex = (gvInFlash.mData.mMeas1_Index - 1U) - wkRealTimeSndPt;
						}
						else
						{
							gvInFlash.mData.mMeas1_OldPtrIndex = imIndexMaxMeas1 - (wkRealTimeSndPt - gvInFlash.mData.mMeas1_Index - 1U);
						}
					}
					else
					{
						/* �ߋ����M�|�C���^���瑗�M */
					}
				}
			}
		}
#endif
	}
	/* �ȑO���M�����ӏ����瑗�M */
	else
	{
		/* �ߋ����M�|�C���^���瑗�M */
	}
	
#if 0
	/* �ڑ���GW�������� */
	vPreGwId[ 0U ] = ConGwId[ 0U ];
	vPreGwId[ 1U ] = ConGwId[ 1U ];
	vPreGwId[ 2U ] = ConGwId[ 2U ];
#endif
}

/*
 *******************************************************************************
 *	�������ʐM�f�[�^��Flash���烊�[�h���v�Z
 *
 *	[���e]
 *		�������ʐM�ɂē]������f�[�^��Flash���烊�[�h��������v�Z����
 *		�v���f�[�^:1�`9�f�[�^
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubRfDrv_RtmodeDataFlashReadKosu( void )
{
	uint16_t	wkKosu;
	
	if( gvInFlash.mData.mMeas1_Index == 0U || gvInFlash.mData.mMeas1_Index == gvInFlash.mData.mMeas1_OldPtrIndex )
	{
		wkKosu = 0U;
	}
	else
	{
		if( gvInFlash.mData.mMeas1_Index > gvInFlash.mData.mMeas1_OldPtrIndex )
		{
			wkKosu = gvInFlash.mData.mMeas1_Index - gvInFlash.mData.mMeas1_OldPtrIndex;
		}
		else
		{
			wkKosu = imIndexMaxMeas1 - gvInFlash.mData.mMeas1_OldPtrIndex;
			if( wkKosu <= 9U )
			{
				if( gvInFlash.mData.mMeas1_Index <= (9U - wkKosu) )
				{
					wkKosu += gvInFlash.mData.mMeas1_Index;
				}
				else
				{
					wkKosu = 9U;
				}
			}
		}
		
		if( wkKosu > 9U )
		{
			wkKosu = 9U;
		}
	}
	
	vRtMode_MeasSndKosu = wkKosu;
}
#endif


#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	������M�v���f�[�^��Flash���烊�[�h
 *
 *	[���e]
 *		���A���^�C�����[�h�̒�����M�v���f�[�^��Flash���烊�[�h����
 *	[����]
 *		uint8_t arSel: MEASURE_DATASET:�v���l�AMEASALM_DATASET:�v���x�񗚗�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubRfDrv_RtmodeMeasDataFlashRead( uint8_t arSel )
{
	uint16_t	wkOldPtrIndex;
	uint16_t		wkSndKosu;
	
	ET_Error_t		wkError;
	ET_RegionKind_t	wkRegionKind;
	
	/* �v���l�̃��[�h */
	if( arSel == MEASURE_DATASET )
	{
		wkOldPtrIndex = gvInFlash.mData.mMeas1_OldPtrIndex + 1U;
		if( wkOldPtrIndex > imIndexMaxMeas1 )
		{
			wkOldPtrIndex = 1U;
		}
		wkSndKosu = vRtMode_MeasSndKosu;
		wkRegionKind = ecRegionKind_Meas1;
	}
	/* �v���x�񗚗��̃��[�h */
	else
	{
		wkOldPtrIndex = gvInFlash.mData.mMeasAlm_OldPtrIndex;
		wkSndKosu = vRtMode_MeasAlmSndKosu;
		wkRegionKind = ecRegionKind_MeasAlm;
	}
	
	/* Flash�Ƀ��[�h�L���[���Z�b�g */
	wkError = ApiFlash_ReadQueSet( 0U, 0U, wkOldPtrIndex, wkSndKosu, wkRegionKind, 1U );
	if( wkError == ecERR_OK )
	{
		ApiSetFlash_ReqRdFlg( 1U, wkRegionKind );					/* �O�t���t���b�V�����[�h�v���t���O�Z�b�g */
		gvModuleSts.mExFlashRd = ecExFlashRdModuleSts_Run;			/* Flash���[�h�������f�����֑J�� */
	}
	else
	{
		/* �L���[�������ς�:debug*/
		M_NOP;
	}

}
#endif


#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	������M�v���f�[�^�Z�b�g�F���A���^�C�����[�h
 *
 *	[���e]
 *		���A���^�C�����[�h�̒�����M�v���f�[�^���Z�b�g����
 *	[����]
 *		uint8_t arSel: MEASURE_DATASET:�v���f�[�^�AMEASALM_DATASET:�v���x�񗚗�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubRfDrv_RTmodeMeasDataSet( uint8_t arSel )
{
	uint8_t					wkLoop;
	uint8_t					wkLoopA;
//	uint8_t					wkBitShift;
//	uint8_t					wkNum;
	uint8_t					wkInterval;
	uint8_t					wkMeasAlm2;
	
	sint16_t				wkS16;
	uint16_t				wkClockDiff;
	uint16_t				wkClockDiffPre;
	uint16_t				wkOldPtrIndex;
	
	uint32_t				wkU32;
	uint32_t				wkClock;
	uint32_t				wkClockPre;
	ST_FlashVal1Evt_t		wkRfRtMeasData[ 9U ];
	ST_FlashMeasAlm_t		wkRfRtMeasAlmData[ 6U ];
	
	
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		vutRfDataBuffer.umRT_MeasureTx.mLoggerID[ wkLoop ] = gvInFlash.mProcess.mUniqueID[ wkLoop ];		/* ���K�[�V���A��ID */
		vutRfDataBuffer.umRT_MeasureTx.mGwID[ wkLoop ] = vrfCommTargetID[ wkLoop ];							/* GW�V���A��ID�Ƒ��M�ߋ��f�[�^�� */
	}
	vutRfDataBuffer.umRT_MeasureTx.mCompanyID = gvInFlash.mProcess.mOpeCompanyID;
	vutRfDataBuffer.umRT_MeasureTx.mAppID = vrfMySerialID.mAppID[0U];
	vutRfDataBuffer.umRT_MeasureTx.mGwID[2U] &= 0xF0;
	vutRfDataBuffer.umRT_MeasureTx.mGwID[2U] += ( (vrfMySerialID.mAppID[1U] >> 4U) & 0x0F);
	
	wkMeasAlm2 = imOFF;
	/* �v���x�񗚗𑗐M�Ȃ� */
	if( arSel == MEASALM_DATASET )
	{
		/* �x��Ď�����2 */
		if( ApiAlarm_ReadMeasAlmFlg() )
		{
			wkMeasAlm2 = imON;						/* �x��Ď�����2�ɂ��v���x�񗚗𑗐M */
		}
	}
	
	/* �ُ�X�e�[�^�X�A�d�r�c�� */
	ApiFlash_RfRTAbnStatusSet();
	
	/* �v���x�񗚗𑗐M(����2) */
	if( wkMeasAlm2 == imON )
	{
		/* ����1�F3bit:0�A����2�F3bit:1 */
		gvRfRTMeasAlm2Data.mRTMeasData_AbnStatus = gvRfRTMeasData.mRTMeasData_AbnStatus;
		vutRfDataBuffer.umRT_MeasureTx.mAbnormalSratus = gvRfRTMeasAlm2Data.mRTMeasData_AbnStatus & 0xC7U;
		vutRfDataBuffer.umRT_MeasureTx.mAbnormalSratus |= 0x08U;
		
		/* �ŐV�f�[�^���� */
		memcpy( &vutRfDataBuffer.umRT_MeasureTx.mNewMeasureTime[ 0U ], &gvRfRTMeasAlm2Data.mRTMeasData_MeasTime[ 0U ], 4U );
		
		/* �ŐV�x��t���O */
		vutRfDataBuffer.umRT_MeasureTx.mNewAlarmFlag[ 0U ] = gvRfRTMeasAlm2Data.mRTMeasData_AlarmFlag[ 0U ];
		vutRfDataBuffer.umRT_MeasureTx.mNewAlarmFlag[ 1U ] = gvRfRTMeasAlm2Data.mRTMeasData_AlarmFlag[ 1U ];
		
		for( wkLoop = 0U ; wkLoop < 5U ; wkLoop++ )
		{
			/* �ŐV�v���l�Ƙ_�� */
			vutRfDataBuffer.umRT_MeasureTx.mNewMeasureValue[ wkLoop ] = gvRfRTMeasAlm2Data.mRTMeasData_MeasVal[ wkLoop ];
		}
	}
	/* �v���l���M�A���͌v���x�񗚗𑗐M(����1) */
	else
	{
		/* �ُ�X�e�[�^�X��� */
		vutRfDataBuffer.umRT_MeasureTx.mAbnormalSratus = gvRfRTMeasData.mRTMeasData_AbnStatus;
		
		/* �ŐV�f�[�^���� */
		memcpy( &vutRfDataBuffer.umRT_MeasureTx.mNewMeasureTime[ 0U ], &gvRfRTMeasData.mRTMeasData_MeasTime[ 0U ], 4U );
		
		/* �ŐV�x��t���O */
		vutRfDataBuffer.umRT_MeasureTx.mNewAlarmFlag[ 0U ] = gvRfRTMeasData.mRTMeasData_AlarmFlag[ 0U ];
		vutRfDataBuffer.umRT_MeasureTx.mNewAlarmFlag[ 1U ] = gvRfRTMeasData.mRTMeasData_AlarmFlag[ 1U ];
		
		for( wkLoop = 0U ; wkLoop < 5U ; wkLoop++ )
		{
			/* �ŐV�v���l�Ƙ_�� */
			vutRfDataBuffer.umRT_MeasureTx.mNewMeasureValue[ wkLoop ] = gvRfRTMeasData.mRTMeasData_MeasVal[ wkLoop ];
		}
		
		/* �v���x�񗚗𑗐M���́A3bit:0�Ƃ��� */
		if( arSel == MEASALM_DATASET )
		{
			vutRfDataBuffer.umRT_MeasureTx.mAbnormalSratus &= 0xC7U;
		}
	}
	
	/* �ŐV�v���l�f�[�^���n��ID */
	vutRfDataBuffer.umRT_MeasureTx.mNewMeasureID[ 0U ] = gvRfRTMeasData.mRTMeasData_MeasID[ 0U ];
	vutRfDataBuffer.umRT_MeasureTx.mNewMeasureID[ 1U ] = gvRfRTMeasData.mRTMeasData_MeasID[ 1U ];
	
	/******************** �v���l���M�ƌx�񗚗𑗐M�ł������番�� *************************/
	if( arSel == MEASURE_DATASET )
	{
	/* ���M�ߋ��f�[�^�� */
	vutRfDataBuffer.umRT_MeasureTx.mDataCntKind = vRtMode_MeasSndKosu;
	
	if( vRtMode_MeasSndKosu != 0U )
	{
		ApiFlash_RfRTMeasDataGet( &wkRfRtMeasData[ 0U ] );
		
		/* �擪�f�[�^���� */
		for( wkLoop = 0U ; wkLoop < 4U; wkLoop++ )
		{
			wkU32 = wkRfRtMeasData[ 0U ].mTimeInfo >> (24U - wkLoop * 8U);
			vutRfDataBuffer.umRT_MeasureTx.mHeaderTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
		}
		
		/* �擪�v���f�[�^�̎��n��ID(IndexNo.) */
		wkOldPtrIndex = gvInFlash.mData.mMeas1_OldPtrIndex + 1U;
		if( wkOldPtrIndex > imIndexMaxMeas1 )
		{
			wkOldPtrIndex = 1U;
		}
		vutRfDataBuffer.umRT_MeasureTx.mHeaderID[ 0U ] = (uint8_t)(wkOldPtrIndex & 0x00FF);
		vutRfDataBuffer.umRT_MeasureTx.mHeaderID[ 1U ] = (uint8_t)((wkOldPtrIndex >> 8U) & 0x00FF);
		
		/* ���^�Ԋu1(����4bit) */
		vutRfDataBuffer.umRT_MeasureTx.mMeasureIntFlag[ 0U ] = gvInFlash.mParam.mLogCyc1;
		
		/* �v���ߋ��x��t���O(���4bit) */
		for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop ++ )
		{
			if( gvInFlash.mData.mAlmPast[ wkLoop ] == ecAlmPastSts_Set )
			{
				vutRfDataBuffer.umRT_MeasureTx.mMeasureIntFlag[ 0U ] += M_BIT4 << wkLoop;
			}
		}
			
		/* �v���l�Ƙ_���ƌx��t���O�F1~9�f�[�^ */
		for( wkLoop = 0U; wkLoop < vRtMode_MeasSndKosu; wkLoop++ )
		{
			/* �f�[�^�Ԃ̎������Ń��[�h��������čX�V���� */
			wkClock = wkRfRtMeasData[ wkLoop ].mTimeInfo;
			
			if( wkLoop != 0U )
			{
				wkClockDiff = wkClock - wkClockPre;
				wkInterval = ApiIntMeasExist( wkClockDiff );
				if( wkInterval == (uint8_t)ecRecKind1_None )
				{
					vRtMode_MeasSndKosu = wkLoop;
					/* ���M�ߋ��f�[�^���ύX */
					vutRfDataBuffer.umRT_MeasureTx.mDataCntKind = vRtMode_MeasSndKosu;
					break;
				}
				else
				{
					if( (wkLoop >= 2U) && (wkClockDiff != wkClockDiffPre) )
					{
						vRtMode_MeasSndKosu = wkLoop;
						/* ���M�ߋ��f�[�^���ύX */
						vutRfDataBuffer.umRT_MeasureTx.mDataCntKind = vRtMode_MeasSndKosu;
						break;
					}
					else
					{
						/* ���^�Ԋu1�X�V */
						vutRfDataBuffer.umRT_MeasureTx.mMeasureIntFlag[ 0U ] &= 0xF0U;
						vutRfDataBuffer.umRT_MeasureTx.mMeasureIntFlag[ 0U ] |= wkInterval;
					}
				}
				wkClockDiffPre = wkClockDiff;
			}
			wkClockPre = wkClock;
			
			
			/* �v���l1CH~3CH */
			wkS16 = wkRfRtMeasData[ wkLoop ].mMeasVal[ 0U ] + gvInFlash.mProcess.mModelOffset;
			vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 0U ] = (uint8_t)wkS16;
			vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 1U ] = (uint8_t)((wkS16 & 0x1F00) >> 8U);
			
			wkS16 = wkRfRtMeasData[ wkLoop ].mMeasVal[ 1U ] + gvInFlash.mProcess.mModelOffset;
			vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 1U ] |= (uint8_t)((wkS16 & 0x0007) << 5U);
			vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 2U ] = (uint8_t)((wkS16 & 0x07F8) >> 3U);
			vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 3U ] = (uint8_t)((wkS16 & 0x1800) >> 11U);
			
			wkS16 = wkRfRtMeasData[ wkLoop ].mMeasVal[ 2U ] + gvInFlash.mProcess.mModelOffset;
			vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 3U ] |= (uint8_t)((wkS16 & 0x003F) << 2U);
			vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 4U ] = (uint8_t)((wkS16 & 0x1FC0) >> 6U);
			
			/* �_�� */
			vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 4U ] |= (uint8_t)(wkRfRtMeasData[ wkLoop ].mLogic << 7U);
			
			/* �x��t���O */
#if 1
			ApiFlash_RfRTAlarmFlagSet( wkRfRtMeasData[ wkLoop ].mAlmFlg, wkRfRtMeasData[ wkLoop ].mDevFlg, &vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U ] );
#else
			vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U ] = 0U;
				vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 6U ] = 0U;
				for( wkLoopA = 0U ; wkLoopA < 3U ; wkLoopA ++ )
				{
					switch( wkLoopA )
			{
						case 0U:
							wkNum = 0U;
							wkBitShift = 0U;
							break;
						case 1U:
							wkNum = 0U;
							wkBitShift = 4U;
							break;
						case 2U:
							wkNum = 1U;
							wkBitShift = 0U;
							break;
					}
					
					if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgHH )
					{
						if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgH )
						{
							/* ����x������x�� */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmHAlmHH << wkBitShift);
			}
						else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgH )
			{
							/* �����E�����x�� */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevHAlmHH << wkBitShift);
			}
					}
					else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgHH )
					{
						if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgH )
			{
							/* ����x�������E */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmHDevHH << wkBitShift);
			}
						else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgH )
			{
							/* �����E������E */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevHDevHH << wkBitShift);
			}
					}
					else if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgH )
					{
						/* ����x�� */
						vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmH << wkBitShift);
					}
					else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgH )
					{
						/* �����E */
						vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevH << wkBitShift);
					}
					else if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgLL )
					{
						if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgL )
						{
							/* �����x�񉺉����x�� */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmLAlmLL << wkBitShift);
						}
						else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgL )
						{
							/* ������E�������x�� */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevLAlmLL << wkBitShift);
						}
					}
					else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgLL )
					{
						if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgL )
			{
							/* �����x�񉺉�����E */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmLDevLL << wkBitShift);
			}
						else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgL )
			{
							/* ������E��������E */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevLDevLL << wkBitShift);
			}
					}
					else if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgL )
					{
						/* �����x�� */
						vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmL << wkBitShift);
					}
					else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgL )
			{
						/* ������E */
						vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevL << wkBitShift);
			}
				}
#endif
			}
			/* �@��ُ�ߋ��x��t���O */
			if( gvInFlash.mData.mAlmPast[ 4U ] == ecAlmPastSts_Set )
			{
				vutRfDataBuffer.umRT_MeasureTx.mDataCntKind |= 0x40U;
			}
		}
	}
	else
	{
		/* ���M�ߋ��f�[�^�� */
		vutRfDataBuffer.umRT_AuditTx.mDataCntKind = vRtMode_MeasAlmSndKosu | 0x80;
		
		/* �ߋ��x��t���O(���4bit) */
		vutRfDataBuffer.umRT_AuditTx.mPastAlarmFL = 0U;
		for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop ++ )
		{
			if( gvInFlash.mData.mAlmPast[ wkLoop ] == ecAlmPastSts_Set )
			{
				vutRfDataBuffer.umRT_AuditTx.mPastAlarmFL += M_BIT4 << wkLoop;
			}
		}
		/* �@��ُ�ߋ��x��t���O */
		if( gvInFlash.mData.mAlmPast[ 4U ] == ecAlmPastSts_Set )
		{
			vutRfDataBuffer.umRT_AuditTx.mDataCntKind |= 0x40U;
		}
		
		if( vRtMode_MeasAlmSndKosu != 0U )
		{
			/* �擪�v���x�񗚗��̎��n��ID(IndexNo.) */
			wkOldPtrIndex = gvInFlash.mData.mMeasAlm_OldPtrIndex;
			if( wkOldPtrIndex > imIndexMaxMeasAlm )
			{
				wkOldPtrIndex = 1U;
			}
			vutRfDataBuffer.umRT_AuditTx.mHeaderID[ 0U ] = (uint8_t)(wkOldPtrIndex & 0x00FF);
			vutRfDataBuffer.umRT_AuditTx.mHeaderID[ 1U ] = (uint8_t)((wkOldPtrIndex >> 8U) & 0x00FF);
			
			/* Flash���烊�[�h�����v���x�񗚗���ϐ��Ɋi�[ */
			ApiFlash_RfRTMeasAlmDataGet( &wkRfRtMeasAlmData[ 0U ] );
			
			/* �x�񗚗��F1~6�f�[�^ */
			for( wkLoop = 0U; wkLoop < vRtMode_MeasAlmSndKosu; wkLoop++ )
			{
				/* ���� */
				for( wkLoopA = 0U ; wkLoopA < 4U; wkLoopA++ )
				{
					wkU32 = wkRfRtMeasAlmData[ wkLoop ].mTimeInfo >> (24U - wkLoopA * 8U);
					vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ wkLoopA ] = (uint8_t)(wkU32 & 0x000000FF);
				}
				
				/* �x��ch(mMeasureData[4]:0~1bit) */
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 4U ] = wkRfRtMeasAlmData[ wkLoop ].mAlmChannel;
				
				/* �_��(mMeasureData[4]:2bit) */
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 4U ] |= (uint8_t)(wkRfRtMeasAlmData[ wkLoop ].mEvent << 2U);
				
				/* ���������v���l��1CH��(mMeasureData[8]-[9]) */
				wkS16 = wkRfRtMeasAlmData[ wkLoop ].mMeasVal + gvInFlash.mProcess.mModelOffset;
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 8U ] = (uint8_t)wkS16;
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 9U ] = (uint8_t)((wkS16 & 0x1F00) >> 8U);
				
				/* �A���[���t���O:0bit */
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 10U ] = wkRfRtMeasAlmData[ wkLoop ].mAlmFlg;
				
				/* ���x��:1~2bit */
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 10U ] |= (wkRfRtMeasAlmData[ wkLoop ].mLevel << 1U);
				
				/* �x�񍀖�:3~7bit */
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 10U ] |= (wkRfRtMeasAlmData[ wkLoop ].mItem << 3U);
			}
		}
	}
}
#endif


#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 * �v���l1�̉ߋ����M�|�C���^���X�V
 *
 *	[���e]
 *		�v���l1�̉ߋ����M�|�C���^���X�V
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubRfDrv_RtmodeMeas1OldPtrUpdate( void )
{
	gvInFlash.mData.mMeas1_OldPtrIndex += vRtMode_MeasSndKosu;
	
	if( gvInFlash.mData.mMeas1_OldPtrIndex > imIndexMaxMeas1 )
	{
		gvInFlash.mData.mMeas1_OldPtrIndex -= imIndexMaxMeas1;
	}
	
	/* �C�x���g�L�����瑗�M */
	if( gvInFlash.mParam.mRealTimeSndPt == 510U )
	{
		/* �C�x���g�L���|�C���^�ɉߋ����M�|�C���^���� */
		gvInFlash.mData.mMeas1_EvtPtrIndex = gvInFlash.mData.mMeas1_OldPtrIndex;
	}
}
#endif

#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 * �������ʐM(�v���x�񗚗�)��Flash���[�hIndexNo.���Z�b�g
 *
 *	[���e]
 *		�������ʐM(�v���x�񗚗�)�Ƀf�[�^�Z�b�g���邽�߁A
 *		Flash���烊�[�h����擪IndexNo.���Z�o����
 *		�Z�o�́A�v���x�񗚗��ŐVIndexNo.���烊�[�h����������IndexNo.�Ƃ���
 *	[����]
 *		uint16_t arKosu:���[�h�����(1�`6��)
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubRfDrv_RtmodeMeasAlmOldPtrUpdate( uint16_t arKosu )
{

	if( gvInFlash.mData.mMeasAlm_Index < arKosu )
	{
		if( gvInFlash.mData.mMeasAlm_Num == imDataNumMeasAlm )
		{
			gvInFlash.mData.mMeasAlm_OldPtrIndex = (imIndexMaxMeasAlm + 1U) - (arKosu - gvInFlash.mData.mMeasAlm_Index);
		}
		else
		{
			gvInFlash.mData.mMeasAlm_OldPtrIndex = 1U;
		}
	}
	else
	{
		gvInFlash.mData.mMeasAlm_OldPtrIndex = (gvInFlash.mData.mMeasAlm_Index + 1U) - arKosu;
	}
}
#endif


#if (swLoggerBord == imEnable)
/*
***********************************************************************************
* �������[�h �r�[�R����M�f�[�^�̏���
*
* [���e]
* ��M�����r�[�R������K�v�ȃf�[�^�����o���A�v���R�}���h�ɉ����đJ�ڐ���Z�b�g
* �������M�̏������s��
***********************************************************************************
*/
void SubRfDrv_Hsmode_Logger_BeaconRxPrcss( void )
{
	uint32_t wkU32;
	
	vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxWait;
	gvModuleSts.mRf = ecRfModuleSts_Sleep;
	SubSX1272TxFSK_DIO_240kbps();
//	SubRFDrv_SyncSet();
	SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_RESPONSE );
	
	memcpy( &vrfCallID[ 0U ], &vutRfDataBuffer.umGwQueryBeacon.mGwID[ 0U ], 3U );
	
	/* ��M�R�}���h�R�[�h��ϊ�(�������l�͎�M�l�Ɠ���) */
	switch( vutRfDataBuffer.umGwQueryBeacon.mCommand )
	{
		case 0x00:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Non;
			break;
		case 0x01:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;
			break;
		case 0x02:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Measure;
			break;
		case 0x03:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_AlartHis;
			break;
		case 0x04:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_AbnormalHis;
			break;
		case 0x05:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_EventHis;
			break;
		case 0x06:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_OpeHis;
			break;
		case 0x07:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_SysHis;
			break;
		case 0x08:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_SetRead;
			break;
		case 0x09:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_SetWrite;
			break;
		case 0x12:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_ReqFirm;
			break;
		case 0x0B:
			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Toroku;
			break;
		default:
			vutRfDataBuffer.umGwQueryBeacon.mSleepMode = HSMODE_SLEEP_MODE_CNTDWN;
			break;
	}
	
	/* Sleep�̃R�[�h���J�E���g�l�ɕϊ� */
	if( vutRfDataBuffer.umGwQueryBeacon.mSleepMode == HSMODE_SLEEP_MODE_NON )
	{
			vrfHsmodeSleepMode = 0U;
	}
	else if( vutRfDataBuffer.umGwQueryBeacon.mSleepMode == HSMODE_SLEEP_MODE_TRIG )
	{
			vrfHsmodeSleepMode = 0xFF;
	}
	else
	{
		vrfHsmodeSleepMode = 10U;
	}
	
	/* GW������M */
	wkU32 = vutRfDataBuffer.umGwQueryBeacon.mTime[ 0U ];
	wkU32 |= (uint32_t)(vutRfDataBuffer.umGwQueryBeacon.mTime[ 1U ]) << 8U;
	wkU32 |= (uint32_t)(vutRfDataBuffer.umGwQueryBeacon.mTime[ 2U ]) << 16U;
	wkU32 |= (uint32_t)(vutRfDataBuffer.umGwQueryBeacon.mTime[ 3U ] & 0x7FU) << 24U;
#if 1
	ApiTimeComp_first( wkU32, 0U );
#else
/* debug�p */
	gvClock = ApiRtcDrv_localtime( wkU32/2 );
	/* ����RTC�Ɏ������C�g */
	ApiRtcDrv_SetInRtc( gvClock );
	
	/* ����RTC�̎������O�t��RTC�Ɏ������C�g */
	ApiRtcDrv_InRtcToExRtc();
#endif
	
	
	if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Status )
	{
		/* �u���[�h�L���X�g�̂��߃��K�[ID�ƍ��Ȃ� */
		/* COLLECT�\��ON */
		ApiRfDrv_HsStsDisp( 1U );
	}
	else if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Toroku )
	{
		/* �u���[�h�L���X�g�̂��߃��K�[ID�ƍ��Ȃ� */
		/* COLLECT�\��ON */
		ApiRfDrv_HsStsDisp( 1U );
	}
	else if( (vutRfDataBuffer.umGwQueryBeacon.mLoggerID[0U] == vrfMySerialID.mUniqueID[0U])
				&&	(vutRfDataBuffer.umGwQueryBeacon.mLoggerID[1U] == vrfMySerialID.mUniqueID[1U])
				&&	(vutRfDataBuffer.umGwQueryBeacon.mLoggerID[2U] == vrfMySerialID.mUniqueID[2U]))
	{
		/* ID�w��Ăяo�� */
		rfTimeSlotNum = 0U;					/* �f�[�^���M�̂��߂̑��M���Ԋm�� */
		rfTimeSlotGrupe = 2U;				/* 200msec��ɉ��� */
		/* COLLECT�\��ON */
		ApiRfDrv_HsStsDisp( 1U );
		
		if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_ReqFirm )
		{
			gvFirmupMode = imON;			/* �t�@�[���A�b�v���s�� */
			ApiFlash_FlashPowerCtl( imON, imON );
		}
	}
	else if( vutRfDataBuffer.umGwQueryBeacon.mGroupID == vrfMySerialID.mGroupID )
	{
		/* �O���[�v�Ăяo�� �����t�F�[�Y�ł̎��������� */
		rfTimeSlotNum = 0U;					/* �f�[�^���M�̂��߂̑��M���Ԋm�� */
//		/* COLLECT�\��ON */
//		ApiRfDrv_HsStsDisp( 1U );
		SubRFDrv_Hsmode_End();
	}
	else
	{
		/* �Y�����Ȃ� */
		SubRFDrv_Hsmode_End();
	}
}


/*
***********************************************************************************
* �������[�h �r�[�R����M�f�[�^��͂���������
*
* [���e]
*		��M�����r�[�R������K�v�ȃf�[�^�����o���A���
*		�����̂��߂̃f�[�^�Z�b�g�̏���(Flash�փf�[�^���[�h�̃L���[���Z�b�g)���s��
***********************************************************************************
*/
static void SubRfDrv_Hsmode_Logger_BeaconRxAnalysis( void )
{
#if (swKouteiMode == imDisable)
	ST_BeaconRxAnalysis_t wkAnalysis;
	
	
	/* �v���J�n���� */
	wkAnalysis.mReqStartTime = (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mRequestHeaderMeasureTime[ 0U ] << 24U;
	wkAnalysis.mReqStartTime |= (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mRequestHeaderMeasureTime[ 1U ] << 16U;
	wkAnalysis.mReqStartTime |= (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mRequestHeaderMeasureTime[ 2U ] << 8U;
	wkAnalysis.mReqStartTime |= (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mRequestHeaderMeasureTime[ 3U ];
	
	/* �v��IndexNo. */
	wkAnalysis.mReqIndexNo = ApiInFlash_1ByteToBig2Byte( &vutRfDataBuffer.umGwQueryBeacon.mRequestHeaderMeasureID[ 0U ] );
	
	switch( vutRfDataBuffer.umGwQueryBeacon.mCommand )
	{
		case ecRfDrvGw_Hs_ReqCmd_Status:
		case ecRfDrvGw_Hs_ReqCmd_Toroku:
		case ecRfDrvGw_Hs_ReqCmd_SetRead:
		case ecRfDrvGw_Hs_ReqCmd_SetWrite:
		case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
		case ecRfDrvGw_Hs_FirmHex:
		default:
			break;
			
		/* �v���l�f�[�^���W�v�� */
		case 0x02:
			ApiRfDrv_MakeReadQue( ecRegionKind_Meas1, wkAnalysis.mReqStartTime, wkAnalysis.mReqIndexNo );
			break;
			
		/* �x�񗚗����W�v�� */
		case 0x03:
			ApiRfDrv_MakeReadQue( ecRegionKind_MeasAlm, wkAnalysis.mReqStartTime, wkAnalysis.mReqIndexNo );
			break;
			
		/* �@��ُ헚�����W�v�� */
		case 0x04:
			ApiRfDrv_MakeReadQue( ecRegionKind_ErrInfo, wkAnalysis.mReqStartTime, wkAnalysis.mReqIndexNo );
			break;
			
		/* �C�x���g���W�v�� */
		case 0x05:
			ApiRfDrv_MakeReadQue( ecRegionKind_Event, wkAnalysis.mReqStartTime, wkAnalysis.mReqIndexNo );
			break;
			
		/* ���엚�����W�v�� */
		case 0x06:
			ApiRfDrv_MakeReadQue( ecRegionKind_ActLog, wkAnalysis.mReqStartTime, wkAnalysis.mReqIndexNo );
			break;
			
		/* �V�X�e���������W�v�� */
		case 0x07:
			ApiRfDrv_MakeReadQue( ecRegionKind_SysLog, wkAnalysis.mReqStartTime, wkAnalysis.mReqIndexNo );
			break;
	}
#endif
}

void ApiRfDrv_MakeReadQue( ET_RegionKind_t arRegionKind, uint32_t arReqStartTime, uint16_t arReqIndexNo )
{
#if (swKouteiMode == imDisable)
	ST_BeaconRxAnalysis_t	wkAnalysis;
	
	wkAnalysis.mReqStartTime = arReqStartTime;
	wkAnalysis.mReqIndexNo = arReqIndexNo;
	
	/* �f�[�^���W���M�p�P�b�g����p�ϐ������� */
	vrfLogSendPacketCtl.mFlashReadCnt[ 0U ] = 0U;
	vrfLogSendPacketCtl.mFlashReadCnt[ 1U ] = 0U;
	vrfLogSendPacketCtl.mFlashReadFin[ 0U ] = ecLogSndCtl_Init;
	vrfLogSendPacketCtl.mFlashReadFin[ 1U ] = ecLogSndCtl_Init;
	vrfLogSendPacketCtl.mFlashReadTime[ 0U ] = 0U;
	vrfLogSendPacketCtl.mFlashReadTime[ 1U ] = 0U;
	vrfLogSendPacketCtl.mInterval = 0U;
			
	switch( arRegionKind )
	{
		/* �@��ُ헚�����W�v�� */
		case ecRegionKind_ErrInfo:
			/* Flash���烊�[�h���邽�߂̃p�����[�^�Z�b�g */
			wkAnalysis.mData_Num = gvInFlash.mData.mErrInf_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mErrInf_Index;
			wkAnalysis.mMax_Num = imDataNumErrInf;
			wkAnalysis.mMax_Index = imIndexMaxErrInf;
			
			wkAnalysis.mKosu = 100U;
			wkAnalysis.mRegionKind = ecRegionKind_ErrInfo;
			break;
			
		/* �x�񗚗����W�v�� */
		case ecRegionKind_MeasAlm:
			/* Flash���烊�[�h���邽�߂̃p�����[�^�Z�b�g */
			wkAnalysis.mData_Num = gvInFlash.mData.mMeasAlm_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mMeasAlm_Index;
			wkAnalysis.mMax_Num = imDataNumMeasAlm;
			wkAnalysis.mMax_Index = imIndexMaxMeasAlm;
			
			wkAnalysis.mKosu = 100U;
			wkAnalysis.mRegionKind = ecRegionKind_MeasAlm;
			break;
			
		/* �C�x���g���W�v�� */
		case ecRegionKind_Event:
			/* Flash���烊�[�h���邽�߂̃p�����[�^�Z�b�g */
			wkAnalysis.mData_Num = gvInFlash.mData.mEvt_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mEvt_Index;
			wkAnalysis.mMax_Num = imDataNumEvt;
			wkAnalysis.mMax_Index = imIndexMaxEvt;
			
			wkAnalysis.mKosu = 60U;
			wkAnalysis.mRegionKind = ecRegionKind_Event;
			break;
			
		/* ���엚�����W�v�� */
		case ecRegionKind_ActLog:
			/* Flash���烊�[�h���邽�߂̃p�����[�^�Z�b�g */
			wkAnalysis.mData_Num = gvInFlash.mData.mActLog_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mActLog_Index;
			wkAnalysis.mMax_Num = imDataNumActLog;
			wkAnalysis.mMax_Index = imIndexMaxActLog;
			
			wkAnalysis.mKosu = 90U;
			wkAnalysis.mRegionKind = ecRegionKind_ActLog;
			break;
			
		/* �v���l�f�[�^���W�v�� */
		case ecRegionKind_Meas1:
			/* Flash���烊�[�h���邽�߂̃p�����[�^�Z�b�g */
			wkAnalysis.mData_Num = gvInFlash.mData.mMeas1_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mMeas1_Index;
			wkAnalysis.mMax_Num = imDataNumMeas1;
			wkAnalysis.mMax_Index = imIndexMaxMeas1;
			
			wkAnalysis.mKosu = 90U;
			wkAnalysis.mRegionKind = ecRegionKind_Meas1;
			break;
			
		/* �V�X�e���������W�v�� */
		case ecRegionKind_SysLog:
			/* Flash���烊�[�h���邽�߂̃p�����[�^�Z�b�g */
			wkAnalysis.mData_Num = gvInFlash.mData.mSysLog_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mSysLog_Index;
			wkAnalysis.mMax_Num = imDataNumSysLog;
			wkAnalysis.mMax_Index = imIndexMaxSysLog;
			
			wkAnalysis.mKosu = 90U;
			wkAnalysis.mRegionKind = ecRegionKind_SysLog;
			break;
	}
	
	/* Flash�Ƀ��[�h�L���[���Z�b�g */
	SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Sub( wkAnalysis );
	
#endif
}


/*
***********************************************************************************
* �������[�h �r�[�R����M�f�[�^��͂���������(�T�u�֐�)
*
* [���e]
*		�������[�h�̉����̂��߂ɁAFlash�Ƀ��[�h����L���[���Z�b�g����
*		1�p�P�b�g�ڂȂ̂ŁA���[�h����擪��IndexNo.���Z�o����Flash���[�h�L���[�̃Z�b�g
***********************************************************************************
*/
static void SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Sub( ST_BeaconRxAnalysis_t arAnalysis )
{
#if (swKouteiMode == imDisable)
	ET_Error_t wkError;
	uint16_t wkU16;
	
	
	/*** �擪IndexNo.�̎Z�o ***/
	
	/* ���^�f�[�^���F0�̂Ƃ� */
	if( arAnalysis.mData_Num == 0U )
	{
		vrfHsReqDataNum = 1U;										/* ����������^�f�[�^���F1 */
		arAnalysis.mReqIndexNo = 1U;								/* Flash���烊�[�h����擪IndexNo.�F1 */
	}
	/* ���^�f�[�^���F1�`(���^�f�[�^MAX��-1)�̂Ƃ� */
	else if( arAnalysis.mData_Num < arAnalysis.mMax_Num )
	{
		/* �v��IndexNo.�F1�`�ŐVIndexNo.�Ȃ� */
		if( arAnalysis.mReqIndexNo >= 1U && arAnalysis.mReqIndexNo <= arAnalysis.mData_Index )
		{
			wkU16 = arAnalysis.mData_Index + 1U - arAnalysis.mReqIndexNo;
			vrfHsReqDataNum = wkU16;								/* ����������^�f�[�^���F(�ŐVIndexNo.+1)-�v��IndexNo. */
		}
		/* �v��IndexNo.�F0�A���͍ŐVIndexNo.���߂Ȃ� */
		else
		{
			vrfHsReqDataNum = arAnalysis.mData_Num;					/* ����������^�f�[�^���F���^�f�[�^��=�ŐVIndexNo. */
			arAnalysis.mReqIndexNo = 1U;							/* Flash���烊�[�h����擪IndexNo.�F1 */
		}
	}
	/* ���^�f�[�^MAX���̂Ƃ� */
	else
	{
		/* �v��IndexNo.�F1�`�ŐVIndexNo. */
		if( arAnalysis.mReqIndexNo >= 1U && arAnalysis.mReqIndexNo <= arAnalysis.mData_Index )
		{
			/* ���M�f�[�^�����Z */
			wkU16 = arAnalysis.mData_Index + 1U - arAnalysis.mReqIndexNo;
		}
		/* �v��IndexNo.�F�ŐVIndexNo.���傫�� */
		else if( arAnalysis.mReqIndexNo > arAnalysis.mData_Index )
		{
			/* ���M�f�[�^�����Z */
			wkU16 = arAnalysis.mMax_Index + 1U - arAnalysis.mReqIndexNo;
			wkU16 += arAnalysis.mData_Index;
		}
		
		/* �v��IndexNo.�F0�A���͑��M�f�[�^�������^�f�[�^MAX�����߂̂Ƃ� */
		if( arAnalysis.mReqIndexNo == 0U || wkU16 > arAnalysis.mMax_Num )
		{
			vrfHsReqDataNum = arAnalysis.mMax_Num;					/* ����������^�f�[�^���F���^�f�[�^��(���^�f�[�^MAX��) */
			
			/* ���݂�IndexNo.�����^�f�[�^MAX���ȏ� */
			if( arAnalysis.mData_Index >= arAnalysis.mMax_Num )
			{
				wkU16 = arAnalysis.mData_Index + 1U - arAnalysis.mMax_Num;
				arAnalysis.mReqIndexNo = wkU16;						/* Flash���烊�[�h����擪IndexNo.�F(����IndexNo. + 1) - ���^�f�[�^MAX�� */
			}
			/* ���݂�IndexNo.:1�`(���^�f�[�^MAX��-1) */
			else
			{
				wkU16 = arAnalysis.mMax_Index + 1U - arAnalysis.mMax_Num;
				wkU16 += arAnalysis.mData_Index;
				arAnalysis.mReqIndexNo = wkU16;						/* Flash���烊�[�h����擪IndexNo.�F((IndexNo.�ő�l + 1) - ���^�f�[�^MAX��) + ����IndexNo. */
			}
		}
		else
		{
			vrfHsReqDataNum = wkU16;								/* ����������^�f�[�^���F�ŐVIndexNo.+1-�v��IndexNo. */
		}
	}
	
	vMem_ReqIndexNo_St = arAnalysis.mReqIndexNo;					/* Flash���[�h�̐擪IndexNo���Z�b�g */
	
	/* Flash�Ƀ��[�h�L���[���Z�b�g */
	vMem_RdIndexNo = arAnalysis.mReqIndexNo;
	wkError = ApiFlash_ReadQueSet( arAnalysis.mReqStartTime, 0U, arAnalysis.mReqIndexNo, arAnalysis.mKosu, arAnalysis.mRegionKind, 0U );
	if( wkError == ecERR_OK )
	{
		ApiSetFlash_ReqRdFlg( 1U, arAnalysis.mRegionKind );			/* �O�t���t���b�V�����[�h�v���t���O�Z�b�g */
		gvModuleSts.mExFlashRd = ecExFlashRdModuleSts_Run;			/* Flash���[�h�������f�����֑J�� */
	}
	else
	{
		/* �L���[�������ς�:debug*/
		M_NOP;
	}
#endif
}


/*
***********************************************************************************
*	�������[�h �A�������̏���
*
* [���e]
*	�����ĉ������邽�߂̃f�[�^�Z�b�g�̏���(Flash�փf�[�^���[�h�̃L���[���Z�b�g)���s��
***********************************************************************************
*/
static void SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Continue( void )
{
	ET_RfDrv_GwReqCmd_t wkLoop;
	
	for( wkLoop = ecRfDrvGw_Hs_ReqCmd_Non ; wkLoop < ecRfDrvGw_Hs_ReqCmd_Max ; wkLoop++ )
	{
		if( cBeaconRxAnalysis_ContinueTbl[ wkLoop ].mReqCmdCode == vrfHsReqCmdCode )
		{
			if( cBeaconRxAnalysis_ContinueTbl[ wkLoop ].mRegionKind != ecRegionKind_Non )
			{
				SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Continue_Sub( cBeaconRxAnalysis_ContinueTbl[ wkLoop ].mKosu, cBeaconRxAnalysis_ContinueTbl[ wkLoop ].mRegionKind );
			}
			break;
		}
	}
}


/*
***********************************************************************************
* �������[�h �A�������̏���(�T�u�֐�)
*
* [���e]
*	�������[�h�̉����̂��߂ɁAFlash�Ƀ��[�h����L���[���Z�b�g����
***********************************************************************************
*/
static void SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Continue_Sub( uint16_t arKosu, ET_RegionKind_t arRegion )
{
	ET_Error_t wkError;
	
	/* Flash�Ƀ��[�h�L���[���Z�b�g */
	wkError = ApiFlash_ReadQueSet( 0U, 0U, vMem_RdIndexNo, arKosu, arRegion, 0U );
	if( wkError == ecERR_OK )
	{
		ApiSetFlash_ReqRdFlg( 1U, arRegion );					/* �O�t���t���b�V�����[�h�v���t���O�Z�b�g */
		gvModuleSts.mExFlashRd = ecExFlashRdModuleSts_Run;		/* Flash���[�h�������f�����֑J�� */
	}
	else
	{
		/* �L���[�������ς�:debug*/
		M_NOP;
	}
}


/*
***********************************************************************************
* �������[�h �A��������ACK�ɂ�鎟�̑��M�|�C���^�Z�b�g
*
* [���e]
* �����ĉ������邽�߂�ACK�ɂ�鎟�̑��M�|�C���^���Z�b�g����
***********************************************************************************
*/
static void SubRfDrv_Hsmode_Logger_SndPtSet( void )
{
//	uint16_t	wkU16;
	uint32_t	wkDataKosu;
	uint16_t	wkIndexMax;
	uint32_t	wkU32;
	
	switch( vrfHsReqCmdCode )
	{
		case ecRfDrvGw_Hs_ReqCmd_Status:
		case ecRfDrvGw_Hs_ReqCmd_Toroku:
		case ecRfDrvGw_Hs_ReqCmd_SetRead:
		case ecRfDrvGw_Hs_ReqCmd_SetWrite:
		case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
		case ecRfDrvGw_Hs_FirmHex:
		default:
			wkDataKosu = 0U;
			break;
			
		case ecRfDrvGw_Hs_ReqCmd_Measure:
			wkDataKosu = 90U;
			wkIndexMax = imIndexMaxMeas1;
			break;
		case ecRfDrvGw_Hs_ReqCmd_AlartHis:
			wkDataKosu = 100U;
			wkIndexMax = imIndexMaxMeasAlm;
			break;
		case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
			wkDataKosu = 100U;
			wkIndexMax = imIndexMaxErrInf;
			break;
		case ecRfDrvGw_Hs_ReqCmd_EventHis:
			wkDataKosu = 60U;
			wkIndexMax = imIndexMaxEvt;
			break;
		case ecRfDrvGw_Hs_ReqCmd_OpeHis:
			wkDataKosu = 90U;
			wkIndexMax = imIndexMaxActLog;
			break;
		case ecRfDrvGw_Hs_ReqCmd_SysHis:
			wkDataKosu = 90U;
			wkIndexMax = imIndexMaxSysLog;
			break;
	}
	
	if( wkDataKosu != 0U )
	{
		if( gvrfHsmodePacketPt <= 2 )
		{
			vMem_RdIndexNo = vMem_ReqIndexNo_St;
		}
		else
		{
			wkU32 = (uint32_t)(gvrfHsmodePacketPt - 2U) * wkDataKosu;
			wkU32 += vMem_ReqIndexNo_St;
			
			if( wkU32 > wkIndexMax )
			{
				vMem_RdIndexNo = wkU32 - wkIndexMax;
			}
			else
			{
				vMem_RdIndexNo = wkU32;
			}
		}
	}
}
#endif


#if (swLoggerBord == imEnable)
/*
***********************************************************************************
* �v��IndexNo.���Z�b�g����
*
* [���e]
* �v��IndexNo.���Z�b�g����
* 
***********************************************************************************
*/
void ApiRfDrv_SetReqIndexNo( uint16_t wkQueIndex )
{
	vMem_ReqIndexNo_St = wkQueIndex;
}
#endif


#if (swLoggerBord == imEnable)
/*
***********************************************************************************
* �d�r�E���̊��荞�ݖ�����~
*
* [���e]
* �d�r�𔲂����Ƃ��ɋ���sleep��������
* 
***********************************************************************************
*/
void ApiRfDrv_ForcedSleep( void )
{
	/* ����IC������ */
#if 0
	SubRfDrv_RfReset();
#else
	if( gvrfStsEventFlag.mReset == RFIC_INI )
	{
		SubRfDrv_RfReset();
		gvrfStsEventFlag.mReset = RFIC_RUN;
	}
#endif

#if 0
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
#endif
	
	vrfInitState = ecRfDrv_Init_Non;
	if( vRfDrvMainLoggerSts == ecRfDrvMainSts_RTmode_LoggerIdle )
	{
		SubIntervalTimer_Stop();
		gvModuleSts.mRf = ecRfModuleSts_Sleep;
	}else
	{
		gvModuleSts.mRf = ecRfModuleSts_Run;
	}
	vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
}
#endif

/*
***********************************************************************************
* ����IC���Z�b�g����
*
* [���e]
* ����IC�����Z�b�g/����������BPOR�����͂��Ȃ��BTUA���g�p�B
* 
***********************************************************************************
*/
void SubRfDrv_RfReset( void )
{
//	SubSX1272POR();														/* POR���� */
	/* 10msec��Wait�K�v �� 10�`11msec��Wait */
//	SubIntervalTimer_Sleep( 3750U, 1U );
//	while(!gvRfIntFlg.mRadioTimer){	NOP(); }
//	SubIntervalTimer_Stop();
	ApiSX1272Reset( imON );
	/* 100usec�ȏ�K�v �� Wait 1�`2 ms */
	SubIntervalTimer_Sleep( 375U, 1U );		/* 0.1msec */
	while(!gvRfIntFlg.mRadioTimer){	NOP(); }
	SubIntervalTimer_Stop();
	ApiSX1272Reset( imOFF );
	/* 5msec�K�v �� Wait 6�`7 ms */
	SubIntervalTimer_Sleep( 375U * 6U, 1U );		/* 5msec */
	while(!gvRfIntFlg.mRadioTimer){	NOP(); }
	SubIntervalTimer_Stop();
	SubSX1272SetModem( MODEM_FSK );
	/* ������ */
	SubSX1272Sleep();
	SubSX1272InitReg();
	rfStatus.Modem = MODEM_FSK;
	SubSX1272Sleep();

//	2021.7.12�ǉ�
#if (swLoggerBord == imEnable)
	/* AppID�`�F�b�N */
	if( (vrfMySerialID.mAppID[0U] != (((gvInFlash.mParam.mAppID[ 0U ] << 4U) & 0xF0) + ((gvInFlash.mParam.mAppID[ 1U ] >> 4U) & 0x0F)))
	|| (vrfMySerialID.mAppID[1U] != ((gvInFlash.mParam.mAppID[ 1U ] << 4U) & 0xF0)) )
	{
		if( (gvInFlash.mParam.mAppID[0U] != APP_ID_LOGGER_INIT_UP)
				|| (gvInFlash.mParam.mAppID[1U] != APP_ID_LOGGER_INIT_LO) )
		{
			if( gvInFlash.mParam.mOnCertLmt == CENTRAL )
			{
				vrfMySerialID.mAppID[0U] = ((gvInFlash.mParam.mAppID[ 0U ] << 4U) & 0xF0)
									+ ((gvInFlash.mParam.mAppID[ 1U ] >> 4U) & 0x0F);
				vrfMySerialID.mAppID[1U] = (gvInFlash.mParam.mAppID[ 1U ] << 4U) & 0xF0;
			}else
			{
				vrfMySerialID.mAppID[0U] = APP_ID_LOCAL_UP;
				vrfMySerialID.mAppID[1U] = APP_ID_LOCAL_LO;
			}
		}else
		{
			vrfMySerialID.mAppID[0U] = APP_ID_LOGGER_INIT_UP_TOP_ALIGN;
			vrfMySerialID.mAppID[1U] = APP_ID_LOGGER_INIT_LO_TOP_ALIGN;
		}
		if( vRfTestFlg != 0U )
		{
			ApiFlash_WriteSysLog( ecSysLogItm_IdDiff, 1U );				/* �V�X�e�������FID���������(�A�v��ID) */
//			ApiAbn_AbnStsSet( (imAbnSts_Dummy3 << vRfTestFlg), ecAbnInfKind_AbnSts );
		}
		vRfTestFlg = 0U;
	}
	if( memcmp( vrfMySerialID.mUniqueID, gvInFlash.mProcess.mUniqueID, 3U ) != 0 )
	{
		memcpy( &vrfMySerialID.mUniqueID[ 0U ], &gvInFlash.mProcess.mUniqueID[ 0U ], 3U );
		
		if( vRfTestFlg != 0U )
		{
			ApiFlash_WriteSysLog( ecSysLogItm_IdDiff, 2U );				/* �V�X�e�������FID���������(�A�v��ID) */
//			ApiAbn_AbnStsSet( (imAbnSts_Dummy3 << vRfTestFlg), ecAbnInfKind_AbnSts );
		}
		vRfTestFlg = 0U;
	}
	if( vrfMySerialID.mOpeCompanyID != gvInFlash.mProcess.mOpeCompanyID )
	{
		vrfMySerialID.mOpeCompanyID = gvInFlash.mProcess.mOpeCompanyID;
		
		if( vRfTestFlg != 0U )
		{
			ApiFlash_WriteSysLog( ecSysLogItm_IdDiff, 3U );				/* �V�X�e�������FID���������(�A�v��ID) */
//			ApiAbn_AbnStsSet( (imAbnSts_Dummy3 << vRfTestFlg), ecAbnInfKind_AbnSts );
		}
		vRfTestFlg = 0U;
	}
	
	rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
	vrfMySerialID.mGroupID = gvInFlash.mParam.mGroupID;

#else
	if( (vrfMySerialID.mAppID[0U] != (((gvInFlash.mId.mAppID[ 0U ] << 4U) & 0xF0) + ((gvInFlash.mId.mAppID[ 1U ] >> 4U) & 0x0F)))
		|| (vrfMySerialID.mAppID[1U] != ((gvInFlash.mId.mAppID[ 1U ] << 4U) & 0xF0)) )
	{
		vrfMySerialID.mAppID[0U] = ((gvInFlash.mId.mAppID[ 0U ] << 4U) & 0xF0) + ((gvInFlash.mId.mAppID[ 1U ] >> 4U) & 0x0F);
		vrfMySerialID.mAppID[1U] = (gvInFlash.mId.mAppID[ 1U ] << 4U) & 0xF0;
	}
	if( memcmp( vrfMySerialID.mUniqueID, gvInFlash.mId.mUniqueID, 3U ) != 0 )
	{
		memcpy( &vrfMySerialID.mUniqueID[ 0U ], &gvInFlash.mId.mUniqueID[ 0U ], 3U );
	}
	if( vrfMySerialID.mOpeCompanyID != gvInFlash.mId.mOpeCompanyID )
	{
		vrfMySerialID.mOpeCompanyID = gvInFlash.mId.mOpeCompanyID;
	}
#endif

}


#if (swLoggerBord == imEnable)
/*
***********************************************************************************
* ����IC���牷�x�l���擾
*
* [���e]
* ����IC�̑����@�\�Ń��K�[�������x���擾����
* [�Ԃ�l]
* ����\1���̉��x�l
***********************************************************************************
*/
sint8_t ApiRfDrv_Temp( void )
{
#if 0
	sint8_t wkTemp[5U];
	uint8_t wkReg;
	uint8_t wkLoop;
	uint8_t wkCount;

	SubSX1272Sleep();
	if(rfStatus.Modem == MODEM_LORA)
	{
		rfStatus.Modem = MODEM_FSK;
		SubSX1272Sleep();
		SubSX1272Stdby();
		SubSX1272RxFSK_On();
/*
		SubSX1272Write( REG_IMAGECAL, RF_IMAGECAL_AUTOIMAGECAL_OFF |
			RF_IMAGECAL_TEMPTHRESHOLD_10 | RF_IMAGECAL_TEMPMONITOR_ON );
		SubSX1272Write( REG_OPMODE ,
			RF_OPMODE_LONGRANGEMODE_OFF |
			RF_OPMODE_MODULATIONTYPE_FSK |
			RF_OPMODE_MODULATIONSHAPING_11 |
			RF_OPMODE_SYNTHESIZER_RX );*/
		rfStatus.Modem = MODEM_LORA;
	}else
	{
		SubSX1272Sleep();
		SubSX1272Stdby();
		SubSX1272RxFSK_On();
/*		SubSX1272Write( REG_IMAGECAL, RF_IMAGECAL_AUTOIMAGECAL_OFF |
			RF_IMAGECAL_TEMPTHRESHOLD_10 | RF_IMAGECAL_TEMPMONITOR_ON );
		SubSX1272Write( REG_OPMODE ,
			RF_OPMODE_LONGRANGEMODE_OFF |
			RF_OPMODE_MODULATIONTYPE_FSK |
			RF_OPMODE_MODULATIONSHAPING_11 |
			RF_OPMODE_SYNTHESIZER_RX );*/
	}

	/* �̂�read */
	for( wkLoop = 0U; wkLoop < 4U; wkLoop++ )
	{
		SubIntervalTimer_Sleep( 30U ,1U );
		while(!gvRfIntFlg.mRadioTimer)
		{
			M_HALT;
		}
		wkReg = SubSX1272Read( REG_TEMP );
	}
	for( wkLoop = 0U; wkLoop < 5U; wkLoop++ )
	{
		SubIntervalTimer_Sleep( 30U ,1U );
		while(!gvRfIntFlg.mRadioTimer)
		{
			M_HALT;
		}
		wkReg = SubSX1272Read( REG_TEMP );
		if( (wkReg & 0x80) == 0x80 )
		{
			wkTemp[wkLoop] = 255U - wkReg + vrfTempCal;
		}else
		{
			wkTemp[wkLoop] = wkReg;
			wkTemp[wkLoop] *= -1;
			wkTemp[wkLoop] += vrfTempCal;
		}
	}

	/* rfStatus.Modem = MODEM_LORA */
	SubSX1272Sleep();

	/* �����l���� */
	wkReg = 0U;
	for( wkLoop = 0U; wkLoop < 5U; wkLoop++ )
	{
		for( wkCount = 0U; wkCount < 5U; wkCount++ )
		{
			if( wkTemp[wkLoop] >= wkTemp[wkCount] )
			{
				wkReg++;
			}
		}
		if( wkReg == 3U )
		{
			/* �����l */
			break;
		}
		if( wkReg >= 4U )
		{
			wkTemp[wkLoop] = 100;
		}
		if( wkReg <= 2U )
		{
			wkTemp[wkLoop] = -100;
		}
		wkReg = 0U;
	}
#if 0
	wkTemp = (sint8_t)wkReg;
	wkTemp *= -1;
	wkTemp = wkTemp + vrfTempCal;
#endif
#if 0
	wkTemp = (sint8_t)(wkReg & 0x7F);
	if( (wkReg & 0x80) == 0x80 )
	{
		wkTemp *= -1;
	}	
	wkTemp = wkTemp + vrfTempCal;
#endif
#if swLoggerBord == imEnable
	ApiLcd_Upp7SegNumDsp( vrfTempCal, imOFF, imOFF );
	ApiLcd_Low7SegNumDsp( wkTemp[wkLoop], imOFF );
	ApiLcd_Main();
#endif

	return wkTemp[wkLoop];
#else
	uint16_t wkAdcResult;
	uint8_t wkLoop;
	sint8_t wkReturn;

	/* �d�rAD�A���x����̃R���p���[�^����ON��ԂƔ������O��̒l���̗p */
	if( gvBatAd.mPhase == ecAdcPhase_Init && CMPEN == 0U )
	{
		ADCEN = 1U;  /* supply AD clock */
//		ADM1 = _00_AD_TRIGGER_SOFTWARE | _20_AD_CONVMODE_ONESELECT;
//		ADM2 = _00_AD_POSITIVE_VDD | _00_AD_NEGATIVE_VSS | _00_AD_AREA_MODE_1 | _00_AD_RESOLUTION_10BIT;	
		ADS = _80_AD_INPUT_TEMPERSENSOR;
//		ADCE = 1U;	/* enable AD comparator */
		gvrfStsEventFlag.mTempAdc = 0U;
		R_ADC_Set_OperationOn();
		/* 1usec wait */
		for( wkAdcResult = 0U; wkAdcResult < 10U; wkAdcResult++ )
		{
			NOP();
		}
		for( wkLoop = 0U; wkLoop < 2U; wkLoop++ )
		{
			SubIntervalTimer_Sleep( 375U, 1U );
			R_ADC_Start();
//			HALT();
			while( !gvrfStsEventFlag.mTempAdc && !gvRfIntFlg.mRadioTimer )
			{
				NOP();
			}
			R_ADC_Stop();
			
			SubIntervalTimer_Stop();
			gvrfStsEventFlag.mTempAdc = 0U;
		}
		
		R_ADC_Get_Result( &wkAdcResult );
		ADS = _00_AD_INPUT_CHANNEL_0;
		
		/* AD�l�̊��Z */
#if (swLoggerBord == imEnable)
		wkAdcResult = (wkAdcResult * 3000UL)/(1023UL);
#else
		wkAdcResult = (wkAdcResult * 3300UL)/(1023UL);
#endif
		if( wkAdcResult >= 1050UL )
		{
			wkAdcResult -= 1050UL;			/* 1050mV(@25��) */
			wkAdcResult <<= 4U;				
			wkAdcResult /= 58;				/* 3.6mV << 4bit */
//			wkAdcResult >>= 4U;
			wkReturn = (sint8_t)(25 - (sint8_t)(wkAdcResult));
		}else
		{
			wkAdcResult = 1050UL - wkAdcResult;
			wkAdcResult <<= 4U;
			wkAdcResult /= 58;
//			wkAdcResult >>= 4U;
			wkReturn = (sint8_t)(25 + (sint8_t)(wkAdcResult));
		}

		wkReturn += gvInFlash.mProcess.mRfTmpHosei;			/* ���x�Z������␳ */
		
		/* 2021.10.27 ���xA/D�˕ϑ΍� �R�����g�A�E�g */	
//		gvInTemp = wkReturn;								/* �������x�i�[ */
		
#if (swRfTestLcd == imEnable)
		ApiLcd_Upp7SegNumDsp( vrfTempCal, imOFF, imOFF );
		ApiLcd_Low7SegNumDsp( wkReturn, imOFF );
		ApiLcd_Main();
#endif
	}
	else
	{
#if 0
		/* �f�o�b�O�p���� */
		ApiFlash_WriteActLog( ecActLogItm_Boot, gvBatAd.mPhase, 200 );
		ApiFlash_WriteActLog( ecActLogItm_Boot, gvInTemp, 210 );
#endif
		wkReturn = gvInTemp;								/* �O��̓������x���̗p */
	}
	
	/* 2021.10.27 ���xA/D�˕ϑ΍� */
	if( wkReturn < -40 )
	{
		wkReturn = -40;
	}
	if( wkReturn > 70 )
	{
		wkReturn = 70;
	}

	/* 5���ȏ�̕ϓ��͇�5���ɌŒ� */
	if( (gvInTemp - wkReturn) > 5 )
	{
		gvInTemp -= 5; 
	}
	else if( (wkReturn - gvInTemp) > 5 )
	{
		gvInTemp += 5;
	}
	else
	{
		gvInTemp = wkReturn;
	}

	return wkReturn;
#endif
}
#endif


#if (swLoggerBord == imEnable)
/*
***********************************************************************************
* �d�r�������Ƃ��̖�������
*
* [���e]
*		�d�r�������Ƃ��̖�������
* [����]
*		�폜 uint8_t arSel:0�c���������ɂ�閳����~�A1�c�d�r�c��0�{(AD)�ɂ�閳����~
* [�Ԃ�l]
* 		�Ȃ�
***********************************************************************************
*/
void ApiRfDrv_BattOffInt( void )
{
	ApiSX1272Reset( imON );
	/* 100usec�ȏ�K�v �� Wait 1�`2 ms */
	SubIntervalTimer_Sleep( 375U, 1U );									/* 0.1msec */
	while(!gvRfIntFlg.mRadioTimer){	NOP(); }
	SubIntervalTimer_Stop();
	ApiSX1272Reset( imOFF );
	/* 5msec�K�v �� Wait 6�`7 ms */
	SubIntervalTimer_Sleep( 375U * 6U, 1U );							/* 5msec */
	while(!gvRfIntFlg.mRadioTimer){	NOP(); }
	SubIntervalTimer_Stop();
	
	/* �t���OON��OFF�̐؂�ւ��Ɏ��s */
	if( gvrfStsEventFlag.mPowerInt == BATT_POWER_ON )
	{
		SubRfDrv_SetRtConInfo( imOFF );									/* �ڑ����� */
		vrfRTmodeCommFailNum = vrfRTmodeCommCutTime;					/* 2021.9.22 �ؒf�������@�̕ύX */
		/* 2021.8.27 */
		vrfRtmodeStopCnt = 0U;
		
#if 0	/* ������~(�d�r�c�ʋ�A�d�r�c��0�{):���엚�� �� �d�r�c�ʒቺ(�c��0�{�A������~):�@��ُ헚���ɓ��� */
		if( arSel == 0U )
		{
			ApiFlash_WriteActLog( ecActLogItm_RfStopBatEmpty, 0U, 0U );	/* ���엚���F������~�A�������� */
		}
		else
		{
			ApiFlash_WriteActLog( ecActLogItm_RfStopBatLow, 0U, 0U );	/* ���엚���F������~�A�������� */
		}
#endif
	}
	
	/* COLLECT�\��OFF */
	ApiRfDrv_HsStsDisp( 0U );
}
#endif

/*
***********************************************************************************
* �����ʐM�^�C�~���O�̉��x�␳�l���Z�o
*
* [���e]
* �����ʐM�^�C�~���O(gvRfTimingCounter)�����x�ɂ��␳����B
* �����d�����̂��߂�Return�l�ł͂Ȃ��O���[�o���ϐ�gvRfTimingTempCorrect�ŕێ�
* gvRfTimingCounterL(0x7FF - gvRfTimingTempCorrect)��gvRfTimingCounterH���J�E���g
* [�Ԃ�l]
* �Ȃ�
***********************************************************************************
*/
void ApiRfDrv_TimingCorrect( void )
{
	sint8_t wkTemp;
	
#if (swLoggerBord == imEnable)
	wkTemp = ApiRfDrv_Temp();
#else
	wkTemp = (sint8_t)(ApiMeas_GetTempVal() / 10L);
#endif
	
	/* 2021.10.27 ���xA/D�˕ϑ΍� */
#if 0
	if( wkTemp < -40 )
	{
		wkTemp = -40;
	}
	if( wkTemp > 70 )
	{
		wkTemp = 70;
	}
#endif

	if( wkTemp < 5 || wkTemp > 45 )
	{
		gvRfTimingTempCorrect = ( ((sint16_t)wkTemp - 25 ) * ((sint16_t)wkTemp - 25 ) * 3U) / 1000U;			/*	-0.03 * ��T ^2 ppm , 1sec��-1count(-10ppm) */
//		gvRfTimingTempCorrect++;					/* �v���X�����ɕ␳��ǉ� */
#if (swLoggerBord == imEnable)
		if( gvRfTimingTempCorrect > 200U )
		{
			gvRfTimingTempCorrect = 200U;
		}
#else
		if( gvRfTimingTempCorrect > 0x0E )
		{
			gvRfTimingTempCorrect = 0x0E;
		}
#endif
	}
	else
	{
		gvRfTimingTempCorrect = 0U;
	}
	gvRfTimingTempCorrect += 2U;					/* �v���X�����ɕ␳��ǉ� */

	/* Debug */
//	gvRfTimingTempCorrect = 0x0E;
#if (swRfTestLcd == imEnable)
	ApiLcd_Upp7SegNumDsp( wkTemp, imOFF, imOFF);
	ApiLcd_Low7SegNumDsp( gvRfTimingTempCorrect, imOFF );
	ApiLcd_Main();
#endif
}

#if (swLoggerBord == imEnable)
/*
********************************************************************************
*  �������[�h�I������
*
*  [���e]
*  �����ʐM���[�h����Idle�֑J�ڂ���ۂ̏���
********************************************************************************
*/
void SubRFDrv_Hsmode_End( void )
{
	/* COLLECT�\��OFF */
	ApiRfDrv_HsStsDisp( 0U );
	
	vrfInitState = ecRfDrv_Init_Non;
	//vrfHsmodeSleepMode = 0x00;
	vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
	gvModuleSts.mRf = ecRfModuleSts_Sleep;
}


/*
 *******************************************************************************
 * ���[�h�����v���f�[�^�𖳐����M�o�b�t�@�Ɋi�[
 *
 *	[���e]
 *		Flash���烊�[�h�����v���f�[�^�𖳐����M�o�b�t�@�Ɋi�[
 *	[����]
 *		ST_FlashVal1Evt_t	arRdData: ���[�h�����v���l
 *		uint8_t				arLoop: ���[�h������(0~89)
 *		uint8_t				arKosu: ���[�h�����
 *		uint16_t			arIndex: ���[�h�����擪IndexNo.
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_MeasData_StorageRfBuff( ST_FlashVal1Evt_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex )
{
	uint8_t					wkTurn;
	uint8_t					wkLoop;
	uint8_t					wkInterval;
	uint8_t					wkBufCrcCnt;
	uint8_t					wkBufCnt;
	uint16_t				wkClockDiff;
	sint16_t				wkS16;
	uint32_t				wkU32;	
	static uint32_t			vClockPre;				/* �O��i�[�����v���f�[�^�̎��� */
	static uint16_t			vClockDiffPre;			/* �O��ƍ���̌v�������̊Ԋu */
	static uint8_t			vDummyFlg;				/* �v���l���_�~�[�ɂ��邩�ǂ����̃t���O */
	
	if( gvrfStsEventFlag.mHsmodeTxTurn == 0U )
	{
		wkTurn = 0U;								/* ��p�P�b�g���[�h */
	}
	else
	{
		wkTurn = 1U;								/* �����p�P�b�g���[�h */
	}
	
	/* �����[�h���� */
	if( arLoop == 0U )
	{
		memset( vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResMeasure) );
		
		vDummyFlg = imOFF;
		
		/* �擪�f�[�^����(���Bit���疳���o�b�t�@�ɖ��߂�) */
		for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
		{
			wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
			vutRfDataBuffer.umLoggerResMeasure.mHeaderTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
		}
		
		/* �v���f�[�^���n��ID */
		vutRfDataBuffer.umLoggerResMeasure.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResMeasure.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		gvFlashReadIndex[ wkTurn ] = arIndex;
		
		/* �p�P�b�g��ʃR�[�h */
		vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode = 0x02U;
		
		if( arRdData->mTimeInfo == 0x3FFFFFFF )
		{
			/* ���^�Ԋu�͌��݂̐ݒ�l�ɂ��āA���񂩂�v���l���_�~�[�ɂ��� */
			vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode |= gvInFlash.mParam.mLogCyc1 << 4U;
			
			vDummyFlg = imON;									/* �v���l�_�~�[ */
			vutRfDataBuffer.umLoggerResMeasure.mDataNum = 0U;	/* �L���f�[�^��:0 */
		}
		
		
		/* ����(1�p�P�b�g��) */
		if( vrfLogSendPacketCtl.mFlashReadCnt[ wkTurn ] == 0U )
		{
			vrfLogSendPacketCtl.mFlashReadCnt[ wkTurn ] ++;
		}
		else
		{
			if( vrfLogSendPacketCtl.mFlashReadFin[ wkTurn ] == ecLogSndCtl_Init )
			{
				/* �O�񃊁[�h�����擪�����ƍ���̎������Ⴄ�Ȃ�A�قȂ�A�h���X���烊�[�h */
				if( vrfLogSendPacketCtl.mFlashReadTime[ wkTurn ] != arRdData->mTimeInfo )
				{
					/* �O��̎����ƍ���̎����̍���2�p�P�b�g�����ǂ���(�A���A����2�p�P�b�g������) */
					wkU32 = vrfLogSendPacketCtl.mFlashReadTime[ wkTurn ];
					if( vrfLogSendPacketCtl.mFlashReadCnt[ 0U ] == 1 && wkTurn == 0U )
					{
						wkU32 += vrfLogSendPacketCtl.mInterval * 90U;
					}
					else
					{
						wkU32 += vrfLogSendPacketCtl.mInterval * 180U;
					}
					
					if( wkU32 != arRdData->mTimeInfo )
					{
						/* ���������Ⴄ�̂�Flash����̃��[�h�S���� */
						vDummyFlg = imON;									/* �v���l�_�~�[ */
						vutRfDataBuffer.umLoggerResMeasure.mDataNum = 0U;	/* �L���f�[�^��:0 */
					}
					else
					{
						/* Flash���[�h�J�E���^���C���N�������g */
						vrfLogSendPacketCtl.mFlashReadCnt[ wkTurn ] ++;
					}
				}
			}
			else
			{
				vDummyFlg = imON;									/* �v���l�_�~�[ */
				vutRfDataBuffer.umLoggerResMeasure.mDataNum = 0U;	/* �L���f�[�^��:0 */
			}
			
			
			if( vDummyFlg == imON )
			{
				vrfLogSendPacketCtl.mFlashReadFin[ wkTurn ] = ecLogSndCtl_SndFin;		/* �/�����p�P�b�g�̑��M���� */
			}
		}
		
		/* ���񃊁[�h�����擪�������i�[ */
		vrfLogSendPacketCtl.mFlashReadTime[ wkTurn ] = arRdData->mTimeInfo;
		
	}
	else
	{
		wkClockDiff = arRdData->mTimeInfo - vClockPre;
		
		if( vDummyFlg == imOFF )
		{
			/* �L���f�[�^�� */
			vutRfDataBuffer.umLoggerResMeasure.mDataNum = arLoop + 1U;
			
			if( arLoop == 1U )
			{
				/* ���M�p�P�b�g����p�Ɏ��^�C���^�[�o�����i�[ */
				if( vrfLogSendPacketCtl.mInterval == 0U )
				{
					vrfLogSendPacketCtl.mInterval = wkClockDiff;
				}
				
				wkInterval = ApiIntMeasExist( wkClockDiff );
				
				/* �f�[�^�Ԃ̎��������^�����̐ݒ�ȊO�̒l */
				if( wkInterval == (uint8_t)ecRecKind1_None )
				{
					/* ���^�Ԋu�͌��݂̐ݒ�l�ɂ��āA���񂩂�v���l���_�~�[�ɂ��� */
					vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode |= gvInFlash.mParam.mLogCyc1 << 4U;
					
					vDummyFlg = imON;										/* �v���l�_�~�[ */
					vutRfDataBuffer.umLoggerResMeasure.mDataNum = arLoop;	/* �L���f�[�^��:���[�h�񐔕� */
				}
				else
				{
					vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode |= wkInterval << 4U;
				}
			}
			else
			{
				/* �O��ƍ���̌v�������̊Ԋu���Ⴄ���𔻒f */
				if( wkClockDiff != vClockDiffPre )
				{
					/* ���񂩂�v���l���_�~�[�ɂ��� */
					vDummyFlg = imON;										/* �v���l�_�~�[ */
					vutRfDataBuffer.umLoggerResMeasure.mDataNum = arLoop;	/* �L���f�[�^��:���[�h�񐔕� */
				}
			}
			
			if( vDummyFlg == imON )
			{
				/* �/�����p�P�b�g�����M�����łȂ���΃��[�h�����֑J�� */
				if( vrfLogSendPacketCtl.mFlashReadFin[ wkTurn ] != ecLogSndCtl_SndFin )
				{
					vrfLogSendPacketCtl.mFlashReadFin[ wkTurn ] = ecLogSndCtl_ReadFin;
				}
				
				/* ��p�P�b�g���[�h */
				if( wkTurn == 0U )
				{
					if( (vrfLogSendPacketCtl.mFlashReadCnt[ 0U ] - 1U) <= vrfLogSendPacketCtl.mFlashReadCnt[ 1U ] )
					{
						if( vrfLogSendPacketCtl.mFlashReadFin[ 1U ] != ecLogSndCtl_SndFin )
						{
							vrfLogSendPacketCtl.mFlashReadFin[ 1U ] = ecLogSndCtl_ReadFin;
						}
					}
				}
				/* �����p�P�b�g���[�h */
				else
				{
					if( vrfLogSendPacketCtl.mFlashReadCnt[ 1U ] <= vrfLogSendPacketCtl.mFlashReadCnt[ 0U ] )
					{
						if( vrfLogSendPacketCtl.mFlashReadFin[ 0U ] != ecLogSndCtl_SndFin )
						{
							vrfLogSendPacketCtl.mFlashReadFin[ 0U ] = ecLogSndCtl_ReadFin;
						}
					}
				}
			}
		}
		vClockDiffPre = wkClockDiff;		/* �f�[�^�Ԃ̎������ꎞ�i�[ */
	}
	vClockPre = arRdData->mTimeInfo;		/* ���[�h�����������ꎞ�i�[ */
	
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* �v���l1~3CH */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		/* �����������l or �_�~�[�t���OON */
		if( arRdData->mTimeInfo == 0x3FFFFFFF || vDummyFlg == imON )
		{
			wkS16 = imSndTmpInit;						/* �_�~�[�f�[�^ */
			if( gvInFlash.mProcess.mModelCode == ecSensType_V ||
				gvInFlash.mProcess.mModelCode == ecSensType_Pulse )
			{
				wkS16 += 2000;
			}
		}
		else
		{
			wkS16 = arRdData->mMeasVal[ wkLoop ];		/* ���[�h�����v���l */
		}
		
		wkS16 += gvInFlash.mProcess.mModelOffset;
		vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[ wkBufCrcCnt ].mMeasure[ wkBufCnt ].mMeasure_AllCh_Event[ wkLoop * 2U ] = (uint8_t)wkS16;
		vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[ wkBufCrcCnt ].mMeasure[ wkBufCnt ].mMeasure_AllCh_Event[ (wkLoop * 2U) + 1U ] = (uint8_t)((wkS16 & 0x1F00) >> 8U);
	}
	
	/* �_�� */
	vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[ wkBufCrcCnt ].mMeasure[ wkBufCnt ].mMeasure_AllCh_Event[ 5U ] |= (arRdData->mLogic << 7U);
	
	/* �x��t���O1~3CH(���4bit:�x��A����4bit:��E) */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[ wkBufCrcCnt ].mMeasure[ wkBufCnt ].mAlart_AllCh[ wkLoop ] = (arRdData->mAlmFlg[ wkLoop ] << 4U) + arRdData->mDevFlg[ wkLoop ];
	}
	
	/* �x��t���O4CH */
	vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[ wkBufCrcCnt ].mMeasure[ wkBufCnt ].mAlart_AllCh[ 3U ] = arRdData->m4chDevAlmFlg;
	
}


/*
 *******************************************************************************
 * ���[�h�����x�񗚗��𖳐����M�o�b�t�@�Ɋi�[
 *
 *	[���e]
 *		Flash���烊�[�h�����x�񗚗��𖳐����M�o�b�t�@�Ɋi�[
 *	[����]
 *		ST_FlashMeasAlm_t	arRdData: ���[�h�����v���l
 *		uint8_t				arLoop: ���[�h������(100�f�[�^:0~99)
 *		uint8_t				arKosu: ���[�h�����
 *		uint16_t			arIndex: ���[�h�����擪IndexNo.
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_MeasAlm_StorageRfBuff( ST_FlashMeasAlm_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex )
{
#if (swKouteiMode == imDisable)
	uint8_t					wkLoop;
	uint8_t					wkBufCrcCnt;
	uint8_t					wkBufCnt;
	sint16_t				wkS16;
	uint32_t				wkU32;
	static uint8_t			vDummyFlg;				/* �������_�~�[�ɂ��邩�ǂ����̃t���O */
	
	/* ���[�h���� */
	if( arLoop == 0U )
	{
		memset( vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResAlarmHist) );
		
		vDummyFlg = imOFF;
		
		/* �x�񗚗����n��ID */
		vutRfDataBuffer.umLoggerResAlarmHist.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResAlarmHist.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		/* �p�P�b�g��ʃR�[�h */
		vutRfDataBuffer.umLoggerResAlarmHist.mPacketCode = 0x03U;
	}
	
	if( vDummyFlg == imOFF )
	{
		/* ������񏉊��l�Ȃ�A�ȍ~�̑��M�o�b�t�@�̓_�~�[ */
		vutRfDataBuffer.umLoggerResAlarmHist.mDataNum = arLoop + 1U;	/* �L���f�[�^���X�V */
		
		if( arRdData->mTimeInfo == 0x3FFFFFFFU )
		{
			vutRfDataBuffer.umLoggerResAlarmHist.mDataNum = arLoop;		/* �L���f�[�^��:���[�h������ */
			vDummyFlg = imON;											/* �ȍ~�_�~�[�f�[�^ */
		}
	}
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* �������(���Bit���疳���o�b�t�@�ɖ��߂�) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
		vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* �`���l��:0~1bit */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mCh_Measure_Event[ 0U ] = arRdData->mAlmChannel;
	
	/* �v���l:2~14bit */
	wkS16 = ((arRdData->mMeasVal + gvInFlash.mProcess.mModelOffset) & 0x003F) << 2U;
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mCh_Measure_Event[ 0U ] |= (uint8_t)wkS16;
	
	wkS16 = ((arRdData->mMeasVal + gvInFlash.mProcess.mModelOffset) & 0x1FC0) >> 6U;
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mCh_Measure_Event[ 1U ] = (uint8_t)wkS16;
	
	/* �_��:15bit */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mCh_Measure_Event[ 1U ] |= (arRdData->mEvent << 7U);
	
	/* �A���[���t���O:0bit */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mAlarm = arRdData->mAlmFlg;
	
	/* ���x��:1~2bit */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mAlarm |= (arRdData->mLevel << 1U);
	
	/* �x�񍀖�:3~7bit */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mAlarm |= (arRdData->mItem << 3U);
	
	/* �\�� */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mReserve = 0U;
#endif
}


/*
 *******************************************************************************
 * ���[�h�����ُ헚���𖳐����M�o�b�t�@�Ɋi�[
 *
 *	[���e]
 *		Flash���烊�[�h�����ُ헚���𖳐����M�o�b�t�@�Ɋi�[
 *	[����]
 *		ST_FlashErrInfo_t	arRdData: ���[�h�����v���l
 *		uint8_t				arLoop: ���[�h������(100�f�[�^:0~99)
 *		uint8_t				arKosu: ���[�h�����
 *		uint16_t			arIndex: ���[�h�����擪IndexNo.
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_MeasErr_StorageRfBuff( ST_FlashErrInfo_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex )
{
#if (swKouteiMode == imDisable)
	uint8_t					wkLoop;
	uint8_t					wkBufCrcCnt;
	uint8_t					wkBufCnt;
	uint32_t				wkU32;
	static uint8_t			vDummyFlg;				/* �������_�~�[�ɂ��邩�ǂ����̃t���O */
	
	/* ���[�h���� */
	if( arLoop == 0U )
	{
		memset( vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResAbnormalHist) );
		
		vDummyFlg = imOFF;
		
		/* �ُ헚�����n��ID */
		vutRfDataBuffer.umLoggerResAbnormalHist.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResAbnormalHist.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		/* �p�P�b�g��ʃR�[�h */
		vutRfDataBuffer.umLoggerResAbnormalHist.mPacketCode = 0x04U;
	}
	
	if( vDummyFlg == imOFF )
	{
		/* �L���f�[�^�� */
		vutRfDataBuffer.umLoggerResAbnormalHist.mDataNum = arLoop + 1U;		/* �L���f�[�^���X�V */
		
		/* ������񏉊��l�Ȃ�A�ȍ~�̑��M�o�b�t�@�̓_�~�[ */
		if( arRdData->mTimeInfo == 0x3FFFFFFFU )
		{
			vutRfDataBuffer.umLoggerResAbnormalHist.mDataNum = arLoop;		/* �L���f�[�^��:���[�h������ */
			vDummyFlg = imON;												/* �ȍ~�_�~�[�f�[�^ */
		}
	}
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* �������(���Bit���疳���o�b�t�@�ɖ��߂�) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
		vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[ wkBufCrcCnt ].mAbnormalHist[ wkBufCnt ].mTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* �@��ُ��ʃR�[�h:0~6bit */
	vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[ wkBufCrcCnt ].mAbnormalHist[ wkBufCnt ].mAbnormalCode_Flag = (uint8_t)arRdData->mItem & 0x7F;
	
	/* �A���[���t���O:7bit */
	vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[ wkBufCrcCnt ].mAbnormalHist[ wkBufCnt ].mAbnormalCode_Flag |= ((arRdData->mAlmFlg & 0x01) << 7U);
	
	/* �\�� */
	vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[ wkBufCrcCnt ].mAbnormalHist[ wkBufCnt ].mReserve = 0U;
#endif
}



/*
 *******************************************************************************
 * ���[�h�����C�x���g�����𖳐����M�o�b�t�@�Ɋi�[
 *
 *	[���e]
 *		Flash���烊�[�h�����C�x���g�����𖳐����M�o�b�t�@�Ɋi�[
 *	[����]
 *		ST_FlashVal1Evt_t	arRdData: ���[�h�����v���l
 *		uint8_t				arLoop: ���[�h������(60�f�[�^:0~59)
 *		uint8_t				arKosu: ���[�h�����
 *		uint16_t			arIndex: ���[�h�����擪IndexNo.
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_MeasEvt_StorageRfBuff( ST_FlashVal1Evt_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex )
{
#if (swKouteiMode == imDisable)
	uint8_t					wkLoop;
	uint8_t					wkBufCrcCnt;
	uint8_t					wkBufCnt;
	sint16_t				wkS16;
	uint32_t				wkU32;
	static uint8_t			vDummyFlg;				/* �������_�~�[�ɂ��邩�ǂ����̃t���O */
	
	/* ���[�h���� */
	if( arLoop == 0U )
	{
		memset( vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResEventHist) );
		
		vDummyFlg = imOFF;
		
		/* �C�x���g�������n��ID */
		vutRfDataBuffer.umLoggerResEventHist.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResEventHist.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		/* �p�P�b�g��ʃR�[�h */
		vutRfDataBuffer.umLoggerResEventHist.mPacketCode = 0x05U;
	}
	
	
	if( vDummyFlg == imOFF )
	{
		vutRfDataBuffer.umLoggerResEventHist.mDataNum = arLoop + 1U;		/* �L���f�[�^���X�V */
		
		/* ������񏉊��l�Ȃ�A�ȍ~�̑��M�o�b�t�@�̓_�~�[ */
		if( arRdData->mTimeInfo == 0x3FFFFFFFU )
		{
			vutRfDataBuffer.umLoggerResEventHist.mDataNum = arLoop;		/* �L���f�[�^��:���[�h������ */
			vDummyFlg = imON;											/* �ȍ~�_�~�[�f�[�^ */
		}
	}
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* �������(���Bit���疳���o�b�t�@�ɖ��߂�) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
		vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* �v���l1~3CH */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		wkS16 = arRdData->mMeasVal[ wkLoop ] + gvInFlash.mProcess.mModelOffset;
		vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mMeasure_AllCh_Event[ wkLoop * 2U ] = (uint8_t)wkS16;
		vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mMeasure_AllCh_Event[ wkLoop * 2U + 1U ] = (uint8_t)((wkS16 & 0x1F00) >> 8U);
	}
	
	/* �_�� */
	vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mMeasure_AllCh_Event[ 5U ] |= (arRdData->mLogic << 7U);
	
	/* �x��t���O1~3CH(���4bit:�x��A����4bit:��E) */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mAlart_AllCh[ wkLoop ] = (arRdData->mAlmFlg[ wkLoop ] << 4U) + arRdData->mDevFlg[ wkLoop ];
	}
	
	/* �x��t���O4CH */
	vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mAlart_AllCh[ 3U ] = arRdData->m4chDevAlmFlg;
#endif
}


/*
 *******************************************************************************
 * ���[�h�������엚���𖳐����M�o�b�t�@�Ɋi�[
 *
 *	[���e]
 *		Flash���烊�[�h�������엚���𖳐����M�o�b�t�@�Ɋi�[
 *	[����]
 *		ST_FlashSysLog_t	arRdData: ���[�h�����v���l
 *		uint8_t				arLoop: ���[�h������(90�f�[�^:0~89)
 *		uint8_t				arKosu: ���[�h�����
 *		uint16_t			arIndex: ���[�h�����擪IndexNo.
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_ActLog_StorageRfBuff( ST_FlashActLog_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex )
{
#if (swKouteiMode == imDisable)
	
	uint8_t					wkLoop;
	uint8_t					wkBufCrcCnt;
	uint8_t					wkBufCnt;
	uint8_t					*pwkBuf;
	uint16_t				wkClrLoop;
	uint16_t				wkU16;
	uint32_t				wkU32;	
	static uint8_t			vDummyFlg;				/* �������_�~�[�ɂ��邩�ǂ����̃t���O */
	
	/* ���[�h���� */
	if( arLoop == 0U )
	{
		memset( &vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResOpeHist) );
		
		vDummyFlg = imOFF;
		
		pwkBuf = &vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[ 0U ].mOpeHist[ 0U ].mTime[ 0U ];
		for( wkClrLoop = 0U ; wkClrLoop < 90U ; wkClrLoop++, pwkBuf++ )
		{
			*pwkBuf = 0U;
		}
		
		/* ���엚�����n��ID */
		vutRfDataBuffer.umLoggerResOpeHist.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResOpeHist.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		/* �p�P�b�g��ʃR�[�h */
		vutRfDataBuffer.umLoggerResOpeHist.mPacketCode = 0x06U;
	}
	
	if( vDummyFlg == imOFF )
	{
		vutRfDataBuffer.umLoggerResOpeHist.mDataNum = arLoop + 1U;		/* �L���f�[�^���X�V */
		
		/* ������񏉊��l�Ȃ�A�ȍ~�̑��M�o�b�t�@�̓_�~�[ */
		if( arRdData->mTimeInfo == 0x3FFFFFFFU )
		{
			vutRfDataBuffer.umLoggerResOpeHist.mDataNum = arLoop;		/* �L���f�[�^��:���[�h������ */
			vDummyFlg = imON;											/* �ȍ~�_�~�[�f�[�^ */
		}
	}
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* �������(���Bit���疳���o�b�t�@�ɖ��߂�) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
		vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[ wkBufCrcCnt ].mOpeHist[ wkBufCnt ].mTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* ��������(����Bit���疳���o�b�t�@�ɖ��߂�) */
	for( wkLoop = 0U ; wkLoop < 2U ; wkLoop++ )
	{
		wkU16 = arRdData->mItem >> (wkLoop * 8U);
		vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[ wkBufCrcCnt ].mOpeHist[ wkBufCnt ].mHistCode[ wkLoop ] = (uint8_t)wkU16;
	}
	
	/* �ύX��ID */
	vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[ wkBufCrcCnt ].mOpeHist[ wkBufCnt ].mUserID = arRdData->mUserId;
	
	/* ������e(����Bit���疳���o�b�t�@�ɖ��߂�) */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		wkU32 = arRdData->mData >> (wkLoop * 8U);
		vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[ wkBufCrcCnt ].mOpeHist[ wkBufCnt ].mHistPrt[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
#endif
}



/*
 *******************************************************************************
 * ���[�h�����V�X�e�������𖳐����M�o�b�t�@�Ɋi�[
 *
 *	[���e]
 *		Flash���烊�[�h�����V�X�e�������𖳐����M�o�b�t�@�Ɋi�[
 *	[����]
 *		ST_FlashSysLog_t	arRdData: ���[�h�����v���l
 *		uint8_t				arLoop: ���[�h������(90�f�[�^:0~89)
 *		uint8_t				arKosu: ���[�h�����
 *		uint16_t			arIndex: ���[�h�����擪IndexNo.
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_SysLog_StorageRfBuff( ST_FlashSysLog_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex )
{
#if (swKouteiMode == imDisable)
	uint8_t					wkLoop;
	uint8_t					wkBufCrcCnt;
	uint8_t					wkBufCnt;
	uint32_t				wkU32;
	
	static uint8_t			vDummyFlg;				/* �������_�~�[�ɂ��邩�ǂ����̃t���O */
	
	/* ���[�h���� */
	if( arLoop == 0U )
	{
		memset( vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResSysHist) );
		
		vDummyFlg = imOFF;
		
		/* �V�X�e���������n��ID */
		vutRfDataBuffer.umLoggerResSysHist.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResSysHist.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		/* �p�P�b�g��ʃR�[�h */
		vutRfDataBuffer.umLoggerResSysHist.mPacketCode = 0x07U;
	}
	
	if( vDummyFlg == imOFF )
	{
		vutRfDataBuffer.umLoggerResSysHist.mDataNum = arLoop + 1U;		/* �L���f�[�^���X�V */
		
		/* ������񏉊��l�Ȃ�A�ȍ~�̑��M�o�b�t�@�̓_�~�[ */
		if( arRdData->mTimeInfo == 0x3FFFFFFFU )
		{
			vutRfDataBuffer.umLoggerResSysHist.mDataNum = arLoop;		/* �L���f�[�^��:���[�h������ */
			vDummyFlg = imON;											/* �ȍ~�_�~�[�f�[�^ */
		}
	}
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* �������(���Bit���疳���o�b�t�@�ɖ��߂�) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
		vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[ wkBufCrcCnt ].mSysHist[ wkBufCnt ].mTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* �������� */
	vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[ wkBufCrcCnt ].mSysHist[ wkBufCnt ].mSysCode = arRdData->mItem;
	
	/* �f�[�^:24bit(����Bit���疳���o�b�t�@�ɖ��߂�) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mData >> (wkLoop * 8U);
		vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[ wkBufCrcCnt ].mSysHist[ wkBufCnt ].mSysPrt[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* �\�� */
	vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[ wkBufCrcCnt ].mSysHist[ wkBufCnt ].mReserve = 0U;
#endif
}
#endif


#if (swLoggerBord == imDisable)
#if 0
/*
 *******************************************************************************
 *	�ڑ����K�[��ؒf��Ԃɂ���
 *
 *	[���e]
 *		�ڑ����K�[��ؒf��Ԃɂ���
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_ConLoggerClr( void )
{
	uint8_t wkLoop;
	uint8_t wkSize;
	uint8_t wkDelList[ 20U ][ 3U ];
	
	for( wkLoop = 0U, wkSize = 0U ; wkLoop < vrfLoggerSumNumber ; wkLoop++ )
	{
		if( (vrfLoggerList[ wkLoop ][ 0U ] != 0x00) ||
			(vrfLoggerList[ wkLoop ][ 1U ] != 0x00) ||
			(vrfLoggerList[ wkLoop ][ 2U ] != 0x00) )
		{
			wkDelList[ wkSize ][ 0U ] = vrfLoggerList[ wkLoop ][ 0U ];
			wkDelList[ wkSize ][ 1U ] = vrfLoggerList[ wkLoop ][ 1U ];
			wkDelList[ wkSize ][ 2U ] = vrfLoggerList[ wkLoop ][ 2U ];
			wkSize ++;
			
			/* �ؒf���ꂽ���K�[���X�g��Modbus���M���X�g�ɒǉ� */
			if( wkSize >= 20U )
			{
				ApiModbus_SndFnc16Ref40301( 0U, wkSize, wkDelList );
				wkSize = 0U;
			}
		}
		vrfLoggerList[ wkLoop ][ 0U ] = 0U;
		vrfLoggerList[ wkLoop ][ 1U ] = 0U;
		vrfLoggerList[ wkLoop ][ 2U ] = 0U;
		vrfLoggerComCnt[ wkLoop ] = 0U;
	}
	
	vrfLoggerSumNumber = 0U;
	
	/* �ؒf���ꂽ���K�[���X�g��Modbus���M���X�g�ɒǉ� */
	if( wkSize != 0U )
	{
		ApiModbus_SndFnc16Ref40301( 0U, wkSize, wkDelList );
	}
}
#endif

#if 0
/*
 *******************************************************************************
 *	�ڑ����K�[��ؒf��Ԃɂ���(�ʐؒf)
 *
 *	[���e]
 *		�ڑ����K�[��ؒf��Ԃɂ���(�ʐؒf)
 *	[����]
 *		uint16_t arId:���K�[ID
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_ConLoggerClrId( uint8_t *arId )
{
	uint8_t wkLoop;
	
	wkLoop = vrfLoggerSumNumber;
	if( wkLoop > 0 && wkLoop <= 60U )
	{
		do
		{
			wkLoop--;
			
			if( (vrfLoggerList[ wkLoop ][ 0U ] == arId[ 0U ]) &&
				(vrfLoggerList[ wkLoop ][ 1U ] == arId[ 1U ]) &&
				(vrfLoggerList[ wkLoop ][ 2U ] == arId[ 2U ]) )
			{
				vrfLoggerList[wkLoop][0U] = 0x00;
				vrfLoggerList[wkLoop][1U] = 0x00;
				vrfLoggerList[wkLoop][2U] = 0x00;
				vrfLoggerComCnt[wkLoop] = 0x00;
				if( (vrfLoggerSumNumber - 1U) == wkLoop )
				{
					/* �ڑ��䐔�������̃��K�[�̏ꍇ�͑���-1 */
					vrfLoggerSumNumber--;
				}
			}
		}while(wkLoop > 0U);
	}
}
#endif

/*
 *******************************************************************************
 *	�ڑ����K�[�����[�h�A���͍폜����
 *
 *	[���e]
 *		�ڑ����K�[�����[�h���AModbus�ʐM���X�g�ɃZ�b�g����
 *		���́A�ڑ����K�[���ꊇ�ؒf���AModbus�ʐM���X�g�ɃZ�b�g����
 *	[����]
 *		uint8_t arSel:	0�F���X�g�ꊇ�폜�A1�F�ڑ����X�g���[�h
 *	[�ߒl]��
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_ConLoggerListSet( uint8_t arSel )
{
	uint8_t wkLoop;
	uint8_t wkSize;
	uint8_t wkList[ 20U ][ 3U ];
	
	for( wkLoop = 0U, wkSize = 0U ; wkLoop < vrfLoggerSumNumber ; wkLoop++ )
	{
		if( (vrfLoggerList[ wkLoop ][ 0U ] != 0x00) ||
			(vrfLoggerList[ wkLoop ][ 1U ] != 0x00) ||
			(vrfLoggerList[ wkLoop ][ 2U ] != 0x00) )
		{
			memcpy( &wkList[ wkSize ][ 0U ], &vrfLoggerList[ wkLoop ][ 0U ], 3U );
			wkSize ++;
			
			/* ���K�[���X�g��Modbus���M���X�g�ɒǉ� */
			if( wkSize >= 20U )
			{
				ApiModbus_SndFnc16Ref40301( arSel, wkSize, wkList );
				wkSize = 0U;
			}
		}
		if( arSel == 0U )
		{
			vrfLoggerList[ wkLoop ][ 0U ] = 0U;
			vrfLoggerList[ wkLoop ][ 1U ] = 0U;
			vrfLoggerList[ wkLoop ][ 2U ] = 0U;
			vrfLoggerComCnt[ wkLoop ] = 0U;
		}
	}

	if( arSel == 0U )
	{
		vrfLoggerSumNumber = 0U;
	}
	
	/* ���K�[���X�g��Modbus���M���X�g�ɒǉ� */
	if( wkSize != 0U )
	{
		ApiModbus_SndFnc16Ref40301( arSel, wkSize, wkList );
	}
}

/*
 *******************************************************************************
 *	�A�v��ID�ݒ�ύX
 *
 *	[���e]
 *		�A�v��ID�ݒ�ύX
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_AppID_Chg( void )
{
	vrfMySerialID.mAppID[0U] = ((gvInFlash.mId.mAppID[ 0U ] << 4U) & 0xF0) + ((gvInFlash.mId.mAppID[ 1U ] >> 4U) & 0x0F);
	vrfMySerialID.mAppID[1U] = (gvInFlash.mId.mAppID[ 1U ] << 4U) & 0xF0;
}
#endif

# if 0
#if (swLoggerBord == imEnable)
/*
********************************************************************************
*  �������[�h�X���[�v����
*
*  [���e]
*  �������[�h�X���[�v����
********************************************************************************
*/
static void SubRFDrv_Hsmode_SleepFin( void )
{
	/* �����ʐM�X���[�v���� */
	if( gvInFlash.mParam.mFstConnBoot <= gvFstConnBootCnt )
	{
		vrfHsmodeSleepMode = 0x00;
	}
}
#endif
#endif

#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	�O���[�vID�ݒ�ύX
 *
 *	[���e]
 *		�O���[�vID�ݒ�ύX
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_GrID_Chg( void )
{
	vrfMySerialID.mGroupID = gvInFlash.mParam.mGroupID;
}

/*
 *******************************************************************************
 *	�����ʐM�Ԋu�𗘗p���ă��A���^�C���ʐM�Ҏ��OFF�ɂ���
 *
 *	[���e]
 *		�����ʐM�Ԋu�𗘗p���ă��A���^�C���ʐM�Ҏ��OFF�ɂ���
 *	[����]
 *		uint8_t arSel: 0:�����ʐM�Ԋu���[�h�A1:DISP�L�[�����ɂ���ԕύX
 *						2:�����ʐM�Ԋu������
 *	[�ߒl]
 *		uint16_t vrfRTmodeCommInt
 *******************************************************************************
 */
uint16_t ApiRfDrv_RtStbyOnOff( uint8_t arSel )
{
	switch( arSel )
	{
		/* �����ʐM�Ԋu���[�h */
		default:
		case 0U:
			break;
		/* DISP�L�[�����ɂ���ԕύX */
		case 1U:
			/* ���݂̏�Ԋm�F */
			if( vrfRTmodeCommInt == 0xFFFF )
			{
				/* HS-Rt�Ҏ��Ԃ֕ύX */
				vrfRTmodeCommInt = vrfPreRTmodeCommInt;
				M_CLRBIT( gvInFlash.mParam.mParmFlg, imRtStbyOffFlg );
				
				/* ���엚��(�@�����[�hOFF�A�@�푀��) */
				ApiFlash_WriteActLog( ecActLogItm_RfAirplaneOff, 0U, 4U );
				
			}
			else
			{
				/* ���݂�Rt�Ҏ�������L�� */
				vrfPreRTmodeCommInt = vrfRTmodeCommInt;
				
				/* HS�̂ݑҎ��Ԃ֕ύX */
				vrfRTmodeCommInt = 0xFFFF;
				M_SETBIT( gvInFlash.mParam.mParmFlg, imRtStbyOffFlg );
				
				/* Rt��ONLINE�\����OFF */
				SubRfDrv_SetRtConInfo( imOFF );
				vrfRTmodeCommFailNum = vrfRTmodeCommCutTime;					/* 2021.9.22 �ؒf�������@�̕ύX */
				vrfRtmodeStopCnt = 0U;
				
				/* ���엚��(�@�����[�hON�A�@�푀��) */
				ApiFlash_WriteActLog( ecActLogItm_RfAirplaneOn, 0U, 4U );
				
				/* �����ʐM�X���[�v���� */
				vrfHsmodeSleepMode = 0x00U;
			}
			gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;	/* ����Flash�����ݍX�V */
			break;
		/* �����ʐM�Ԋu������ */
		case 2U:
			if( M_TSTBIT( gvInFlash.mParam.mParmFlg, imRtStbyOffFlg ) )
			{
				vrfRTmodeCommInt = 0xFFFF;
			}
			else
			{
				vrfRTmodeCommInt = 300U;					/* �����l1min�����̒ʐM�Ԋu */
			}
			vrfPreRTmodeCommInt = 300U;
			break;
	}
	
	return vrfRTmodeCommInt;
}
#endif


#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	�t�@�[���A�b�v�f�[�^�i�[
 *
 *	[���e]
 *		�t�@�[���A�b�v�f�[�^�i�[
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static ET_RfDrv_ResInfo_t SubRfDrv_FirmStore( ST_RF_Gw_Hex_Tx_t *arRfBuff )
{
#if (swKouteiMode == imDisable)
	uint8_t				wkRdBuff[ 16U ];
	uint8_t				wkSize;
	uint16_t			wkLoop;
	uint16_t			wkPacket;
	uint16_t			wkPacketSum;
	int16_t				wkCmpRet;
	uint32_t			wkAddr;
	ET_Error_t			wkError;
	ET_RfDrv_ResInfo_t	wkRet;
	uint8_t				wkCrc[ 2U ];
	static uint16_t		vCrcData;
	
	wkRet = ecRfDrv_Success;
	wkError = ecERR_OK;
	
	wkPacket = arRfBuff->mPacketNo[ 0U ] << 8U;
	wkPacket += arRfBuff->mPacketNo[ 1U ];
	
	wkPacketSum = arRfBuff->mPacketSum[ 0U ] << 8U;
	wkPacketSum += arRfBuff->mPacketSum[ 1U ];
	
	vFirmPacketSum = wkPacketSum;
	
	if( vFirmPacketNum == wkPacket )
	{
		if( vFirmPacketNum == 0U )
		{
			ApiFlash_FlashPowerCtl( imON, imON );
			ApiFlashDrv_FirmMemErase( ecFlashKind_Prim );
			vFirmPacketNum ++;
			vCrcData = 0x0000U;
			CRCD = vCrcData;
		}
		else
		{
			if( wkPacketSum != wkPacket )
			{
				wkAddr = vFirmPacketNum - 1U;
				wkAddr *= (RF_HSMODE_FIRM_HEX_LENGTH_HEX_CRC - 10U);
				wkAddr += imFirmUp_FlashWrTopAddr;
				
				wkSize = 16U;
				for( wkLoop = 0U ; wkLoop < 56U ; wkLoop++, wkAddr+=16U )
				{
					if( wkLoop == 55U )
					{
						wkSize = 10U;
					}
					wkError = ApiFlashDrv_WriteMemory( wkAddr, &arRfBuff->mHex[ wkLoop * 16U ], wkSize, ecFlashKind_Prim );
					if( wkError == ecERR_OK )
					{
						wkError = ApiFlashDrv_ReadData( wkAddr, &wkRdBuff[ 0U ], wkSize, ecFlashKind_Prim );
						if( wkError == ecERR_OK )
						{
							wkCmpRet = memcmp( &arRfBuff->mHex[ wkLoop * 16U ], &wkRdBuff[0], wkSize );
							if( wkCmpRet == 0U )
							{
								wkError = ecERR_OK;
							}
							else
							{
								wkError = ecERR_NG;
							}
						}
						else
						{
							wkError = ecERR_NG;
						}
					}
					else
					{
						wkError = ecERR_NG;
					}
				}
				vFirmPacketNum ++;
		
				if( wkError == ecERR_NG )
				{
					vFirmPacketNum = 0U;
				}
				
				
				CRCD = vCrcData;
				for( wkLoop = 0U ; wkLoop < 890U ; wkLoop++ )
				{
					if( arRfBuff->mHex[ wkLoop ] == 0xFF )
					{
						M_NOP;
						break;
					}
					CRCIN = arRfBuff->mHex[ wkLoop ];
				}
				M_NOP;
				vCrcData = CRCD;
				
			}
			else
			{
				wkCrc[ 0U ] = vCrcData >> 8U;
				wkCrc[ 1U ] = vCrcData & 0x00FF;
				/* CRC��r */
				if( arRfBuff->mReserve[ 0U ] == wkCrc[ 0U ] && arRfBuff->mReserve[ 1U ] == wkCrc[ 1U ] )
				{
					wkError = ecERR_OK;
					
					/* ���O�C�����[�UID */
					gvInFlash.mProcess.mUpUserId = (arRfBuff->mReserve[ 2U ] << 8U) + arRfBuff->mReserve[ 3U ];
					
					/* �t�@�[���X�V���� */
					gvInFlash.mProcess.mUpDate[ 0U ] = gvClock.mYear;	/* �N */
					gvInFlash.mProcess.mUpDate[ 1U ] = gvClock.mMonth;	/* �� */
					gvInFlash.mProcess.mUpDate[ 2U ] = gvClock.mDay;	/* �� */
					
					/* �t�@�[���A�b�v���s�̂��߃X�e�[�^�X�J�� */
					gvModuleSts.mFirmup = ecFirmupModuleSts_Pre;
				}
				else
				{
					wkError = ecERR_NG;
				}
			}
		}
	}
	else
	{
		if( vFirmPacketNum < wkPacket )
		{
			wkError = ecERR_NG;
		}
	}
	
	if( wkError != ecERR_OK )
	{
		wkRet = ecRfDrv_Error;
	}
	return wkRet;
#endif
}
#endif

#if (swLoggerBord == imDisable)
/*
 *******************************************************************************
 *	RF�ʐM�C���W�P�[�^LED��ON/OFF����
 *
 *	[���e]
 *		RF�ʐM�C���W�P�[�^LED��ON/OFF����
 *	[����]
 *		uint8_t arLedCntrl:	imON(1)�FLED�_���AimOFF(0)�FLED����
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiRfDrv_Led( uint8_t arLedCtrl )
{
	if( arLedCtrl == imON )
	{
		M_SETBIT(P3,M_BIT0);
	}
	else
	{
		M_CLRBIT(P3,M_BIT0);
	}
}
#endif

#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	�����ʐM���[�h�@GW�ďo�M���̌��m����@�J��Ԃ�����
 *
 *	[���e]
 *		�����ʐM�ł̌ďo�M�����m�̂��߁A����(RF_HSMODE_WAKEUP_TIME)�A
 *		���Ԋu(RF_HSMODE_DECT_INT)�ŋN������B
 * 		���̊֐��ɓ��鎞�_�łP��N���ς݁B
 * 		���񐔎��{��̓A�C�h���ɖ߂�B
 * 		����(RF_HSMODE_WAKEUP_SET)��Sync�؂�ւ��B
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void SubRfDrv_Wakeup_Cycle( void )
{
	uint8_t wkRadio;

	vrfHsWkupDectNum++;
	if( vrfHsWkupDectNum == RF_HSMODE_WAKEUP_SET )
	{
		if( gvrfStsEventFlag.mHsmodeSyncDetect )
		{
			vrfInitState = ecRfDrv_Init_Non;
			gvrfStsEventFlag.mHsmodeSyncDetect = 0U;
			SubRFDrv_SyncSet();
		}
		else
		{
			vrfHsWkupDectNum = RF_HSMODE_WAKEUP_TIME;
		}
	}
	if( vrfHsWkupDectNum < RF_HSMODE_WAKEUP_TIME )
	{
#if (swSensorCom == imDisable)
		if( gvMeasPhase == ecMeasPhase_ChrgRef || gvMeasPhase == ecMeasPhase_ChrgTh )
		{
			return;
		}
#endif
		
//		SubIntervalTimer_Sleep( RF_HSMODE_DECT_INT * 375UL, 1U );
		/* ���g���C���� 125msec Sleep + 19msec Halt */
		wkRadio = gvRfTimingCounterL;			/* gvRfTimingCounterL��0-7�ŌJ��Ԃ� */
		if( wkRadio < 7U )
		{
			while( gvRfTimingCounterL < wkRadio + 1U )
			{
				/* �R���p���[�^���s����STOP����̋N���ɂ��w���Y���h�~�Ƃ��āFHALT */
				if( CMPMK0 == 0U )
				{
					M_HALT;
				}
				else
				{
					M_STOP;
				}
			}
		}
		else
		{
			while( !(gvRfTimingCounterL == (wkRadio - 7U)) )
			{
				/* �R���p���[�^���s����STOP����̋N���ɂ��w���Y���h�~�Ƃ��āFHALT */
				if( CMPMK0 == 0U )
				{
					M_HALT;
				}
				else
				{
					M_STOP;
				}
			}
		}
		SubIntervalTimer_Sleep( vrfHsWkupDectNum * 2UL * 375UL, 1U );
		while( !gvRfIntFlg.mRadioTimer )
		{
			M_HALT;
		}
//		SubIntervalTimer_Stop();
//		R_TAU0_Channel6_Stop();

		gvrfStsEventFlag.mHsmodeDetectLoop = 1U;
//		vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconPre1stDetect;
		vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconPre1stDetect_Inter;
		gvModuleSts.mRf = ecRfModuleSts_Run;
	}
	else
	{
		vrfHsWkupDectNum = 0U;
		gvrfStsEventFlag.mHsmodeDetectLoop = 0U;
		gvModuleSts.mRf = ecRfModuleSts_Sleep;
		vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
		if( vrfHsmodeSleepMode != 0xFF )
		{
			if( vrfHsmodeSleepMode > 0U )
			{
				vrfHsmodeSleepMode--;
			}
		}
		else
		{
//			SubRFDrv_Hsmode_SleepFin();
			/* �����ʐM�X���[�v���� */
			if( gvInFlash.mParam.mFstConnBoot <= gvFstConnBootCnt )
			{
				vrfHsmodeSleepMode = 0x00;
			}
		}
	}
}
#endif

#if (swLoggerBord == imEnable)
#else
/*
 *******************************************************************************
 *	�����ʐM���[�h�@�Ăяo���r�[�R����̃��X�|���X��M���Wait
 *
 *	[���e]
 *		�����ʐM�ł̃r�[�R�����o��̃��K�[����̃��X�|���X����M���A
 *		��M���Wait�𒲐����A�^�C�~���O�����킹��B
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void SubRfDrv_Hsmode_ResRx_AfterWait( void )
{
	uint16_t wkCulc;
	if( gvrfStsEventFlag.mHsmodeRxEnter == 0U )
	{
		/* Ack���M��50msec Wait */
		wkCulc = 0UL;
		while( !gvRfIntFlg.mRadioTimer && wkCulc < 65500UL )
		{
			wkCulc++;
		}
		if( !gvRfIntFlg.mRadioTimer )
		{
			NOP();
		}
//		SubIntervalTimer_Sleep( 90UL * 375UL, 1U );
		SubIntervalTimer_Sleep( 40UL * 375UL, 1U );
		gvrfStsEventFlag.mHsmodeRxEnter = 1U;
		gvModuleSts.mRf = ecRfModuleSts_Sleep;
	}
}
#endif

#if (swRssiLogDebug == imEnable)
/*
 *******************************************************************************
 *	�f�o�b�O�p�@RSSI(�G���[�R�[�hvrfErorrCause)�Ɛڑ���GW�̃��O(2ch,3ch)����l��ύX
 *	[���e]
 *		
 *		
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void SubRfDrv_RssiLogDebug( void )
{
	/* RSSI��CH2�Ɋi�[ */
	if( vrfDebugRTmodeCnt == 0U )
	{
#if (swSensorCom == imDisable)
		gvMeasPrm.mMeasVal[ 1U ] = 0U;
		gvDebugRssi = 0U;
#else
		gvMeasPrm.mMeasVal[ 2U ] = 0U;
#endif
	}
	else
	{
		if( vrfRssi > 0U )
		{
#if (swSensorCom == imDisable)
			gvMeasPrm.mMeasVal[ 1U ] = vrfRssi;
			gvDebugRssi = vrfRssi;
#else
			gvMeasPrm.mMeasVal[ 2U ] = vrfRssi;
#endif
		}
		else
		{
#if (swSensorCom == imDisable)
			gvMeasPrm.mMeasVal[ 1U ] = vrfErorrCause;
			gvDebugRssi = vrfErorrCause;
#else
			gvMeasPrm.mMeasVal[ 2U ] = vrfErorrCause;
#endif
		}
	}
#if (swSensorCom == imDisable)
	/* �ڑ�GW��CH3�Ɋi�[ */
	gvMeasPrm.mMeasVal[ 2U ] = gvInFlash.mData.mMeas1_OldPtrIndex & 0x1FFF;
	gvMeasPrm.mMeasVal[ 2U ] = ((vrfCommTargetID[ 2U ] >> 4U) & 0x0FU) + ((vrfCommTargetID[ 1U ] << 4U) & 0xF0U);
#endif
}
#endif

#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	�����ʐM���[�h�@�p�P�b�g��M��̍đ��M�J�ڏ���
 *	[���e]
 *		ROM�팸�̂��߁A���ʉӏ����܂Ƃ߂�
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubRfDrv_Hsmode_RptTx( void )
{
	vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;
	SubIntervalTimer_Sleep( 2U * 375U, 1U );
	gvModuleSts.mRf = ecRfModuleSts_Sleep;
}

/*
 *******************************************************************************
 *	�������ʐM���[�h�@�ڑ��ʐM�̏I������
 *	[���e]
 *		ROM�팸�̂��߁A���ʉӏ����܂Ƃ߂�@2022.9.16
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void SubRfDrv_Rtmode_CnctEnd( void )
{
	SubSX1272Sleep();
	gvrfStsEventFlag.mOnlineLcdOnOff = 0;				/* LCD OFF 2022.9.16 */
	gvModuleSts.mLcd = ecLcdModuleSts_Run;
	vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconInit;
	gvModuleSts.mRf = ecRfModuleSts_Sleep;
}


#endif

#pragma section