/*
 *******************************************************************************
 *	File name	:	RF_Enum.h
 *
 *	[内容]
 *		無線通信部 列挙型
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2020.4.13
 *******************************************************************************
 */

#ifndef	INCLUDED_RF_ENUM_H
#define	INCLUDED_RF_ENUM_H

#include "switch.h"


/*
 *==============================================================================
 *	列挙型定義
 *==============================================================================
 */
typedef enum ET_RfDrvMainSts
{
	ecRfDrvMainSts_FSK_RxInit = 0,
	ecRfDrvMainSts_FSK_RxTry,
	ecRfDrvMainSts_FSK_RxReTry,
	ecRfDrvMainSts_FSK_PreRxTxWait,
	ecRfDrvMainSts_FSK_RxTxWait,
	
//	ecRfDrvMainSts_RTmode_Init,
	ecRfDrvMainSts_RTmode_CadStart,
	ecRfDrvMainSts_RTmode_CadWait,
	ecRfDrvMainSts_RTmode_RxStart,
	ecRfDrvMainSts_RTmode_RxWait,
	ecRfDrvMainSts_RTmode_RxDataRead,
	ecRfDrvMainSts_RTmode_ResponceWait,
	ecRfDrvMainSts_RTmode_ResponceTxDataSet,
	ecRfDrvMainSts_RTmode_ResponceCarrierSens,
	ecRfDrvMainSts_RTmode_ResponceTxStart,
	ecRfDrvMainSts_RTmode_ResponceTxWait,
	ecRfDrvMainSts_RTmode_ResponceTxEnd,
	
	ecRfDrvMainSts_RTmode_Init,
	ecRfDrvMainSts_RTmode_BeaconInit,
	ecRfDrvMainSts_RTmode_MeasureInit,
	ecRfDrvMainSts_RTmode_BeaconDataSet,
	ecRfDrvMainSts_RTmode_LoggerIdle,
	ecRfDrvMainSts_RTmode_BeaconCarrierSens,
	ecRfDrvMainSts_RTmode_BeaconTxStart,
	ecRfDrvMainSts_RTmode_BeaconTxWait,
	ecRfDrvMainSts_RTmode_ConnectAuthCad,
	ecRfDrvMainSts_RTmode_ConnectAuthRxStart,
	ecRfDrvMainSts_RTmode_LoggerInfoDataSet,
	ecRfDrvMainSts_RTmode_ConnectAuthRxWait,
	ecRfDrvMainSts_RTmode_ConnectAuthRxDataRead,
	ecRfDrvMainSts_RTmode_LoogerInfoCarrierSense,
	ecRfDrvMainSts_RTmode_LoogerInfoTxStart,
	ecRfDrvMainSts_RTmode_LoogerInfoTxWait,
	ecRfDrvMainSts_RTmode_ConnectAckCad,
	ecRfDrvMainSts_RTmode_ConnectAckRxStart,
	ecRfDrvMainSts_RTmode_ConnectAckRxWait,
	ecRfDrvMainSts_RTmode_ConnectAckRxRead,
	
	ecRfDrvMainSts_RTmode_MeasureBattAd,
	ecRfDrvMainSts_RTmode_MeasureDataSet,
	ecRfDrvMainSts_RTmode_MeasAlmDataSet,
	ecRfDrvMainSts_RTmode_MeasureCarrierSens,
	ecRfDrvMainSts_RTmode_MeasureTxStart,
	ecRfDrvMainSts_RTmode_MeasureTxWait,
	ecRfDrvMainSts_RTmode_MeasureAckCad,
	ecRfDrvMainSts_RTmode_MeasureAckRxStart,
	ecRfDrvMainSts_RTmode_MeasureAckRxWait,
	ecRfDrvMainSts_RTmode_MeasureAckRxRead,
	
	ecRfDrvMainSts_RTmode_ChangeSetBeaconInit,
	ecRfDrvMainSts_RTmode_ChangeSetBeaconCarrireSense,
	ecRfDrvMainSts_RTmode_ChangeSetBeaconTxStart,
	ecRfDrvMainSts_RTmode_ChangeSetBeaconTxWait,
	ecRfDrvMainSts_RTmode_ChangeSetRx1Cad,
	ecRfDrvMainSts_RTmode_ChangeSetRx1Start,
	ecRfDrvMainSts_RTmode_ChangeSetRx1Wait,
	ecRfDrvMainSts_RTmode_ChangeSetRx1Read,
	ecRfDrvMainSts_RTmode_ChangeSetRx2Start,
	ecRfDrvMainSts_RTmode_ChangeSetRx2Wait,
	ecRfDrvMainSts_RTmode_ChangeSetRx2Read,
	ecRfDrvMainSts_RTmode_ChangeSetRx3Start,
	ecRfDrvMainSts_RTmode_ChangeSetRx3Wait,
	ecRfDrvMainSts_RTmode_ChangeSetRx3Read,
	ecRfDrvMainSts_RTmode_ChangeSetAckCarrierSens,
	ecRfDrvMainSts_RTmode_ChangeSetAckTxStart,
	ecRfDrvMainSts_RTmode_ChangeSetAckTxWait,

	ecRfDrvMainSts_HSmode_BeaconCsInit,
	ecRfDrvMainSts_HSmode_BeaconPre1stDetect_Inter,
	ecRfDrvMainSts_HSmode_BeaconPre1stDetect,
	ecRfDrvMainSts_HSmode_BeaconPre2ndDetect,
	ecRfDrvMainSts_HSmode_BeaconSync1stDetect,
	ecRfDrvMainSts_HSmode_BeaconSync2ndDetect,
	ecRfDrvMainSts_HSmode_BeaconRxStart,
	ecRfDrvMainSts_HSmode_BeaconResTxWait,
	ecRfDrvMainSts_HSmode_BeaconResTxStart,
	ecRfDrvMainSts_HSmode_BeaconResDataSetWait,
	ecRfDrvMainSts_HSmode_BeaconResAckRxPreDetect,
	ecRfDrvMainSts_HSmode_BeaconResAckRxStart,
	
	ecRfDrvMainSts_HSmode_SettingRx,
	
	ecRfDrvMainSts_HSmode_FirmHexRx,
	
	ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxCs,
	ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxStart,

	
	ecRfDrvMainStsMax
} ET_RfDrvMainLoggerSts_t;


typedef enum ET_RfDrvMainGWSts
{
	ecRfDrvMainSts_TxInit_FSK = 0,
	ecRfDrvMainSts_PreTx_FSK,
	ecRfDrvMainSts_Tx_FSK,
	ecRfDrvMainSts_TxContinue_FSK,
	
	ecRfDrvMainSts_RTmode_GWInit,
	ecRfDrvMainSts_RTmode_GWIdle,
	
	ecRfDrvMainSts_RTmode_CallInit,
	ecRfDrvMainSts_RTmode_CallDataPreSet,
	ecRfDrvMainSts_RTmode_CallDataSet,
	ecRfDrvMainSts_RTmode_CallCarrireSens,
	ecRfDrvMainSts_RTmode_CallTx,
	ecRfDrvMainSts_RTmode_CallTxWait,
	ecRfDrvMainSts_RTmode_CallTxEnd,
	
	ecRfDrvMainSts_RTmode_ConnectAuthCadSetting,
	ecRfDrvMainSts_RTmode_MeasureCadSetting,
	ecRfDrvMainSts_RTmode_ConnectAuthSetData,
	ecRfDrvMainSts_RTmode_BeaconCadScan,
	ecRfDrvMainSts_RTmode_BeaconRxStart,
	ecRfDrvMainSts_HSmode_BeaconRxContinue,
	ecRfDrvMainSts_RTmode_BeaconRxWait,
	ecRfDrvMainSts_RTmode_BeaconRxDataRead,
	ecRfDrvMainSts_RTmode_ConnectAuthCarrierSens,
	ecRfDrvMainSts_RTmode_ConnectAuthTxStart,
	ecRfDrvMainSts_RTmode_ConnectAuthTxWait,
	
	ecRfDrvMainSts_RTmode_LoggerInfoRxStart,
	ecRfDrvMainSts_RTmode_LoggerInfoRxWait,
	ecRfDrvMainSts_RTmode_LoggerInfoRxRead,
	
	ecRfDrvMainSts_RTmode_ConnectAckDataSet,
	ecRfDrvMainSts_RTmode_ConnectAckCarrireSens,
	ecRfDrvMainSts_RTmode_ConnectAckTxStart,
	ecRfDrvMainSts_RTmode_ConnectAckTxWait,
	
	ecRfDrvMainSts_RTmode_MeasureCadScan,
	ecRfDrvMainSts_RTmode_MeasureRxStart,
	ecRfDrvMainSts_RTmode_MeasureRxWait,
	ecRfDrvMainSts_RTmode_MeasureRxRead,
	ecRfDrvMainSts_RTmode_MeasureAckDataSet,
	ecRfDrvMainSts_RTmode_MeasureAckCarrireSence,
	ecRfDrvMainSts_RTmode_MeasureAckTxStart,
	ecRfDrvMainSts_RTmode_MeasureAckTxWait,
	
	ecRfDrvMainSts_RTmode_LoggerSettingData1Set,
	ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence1,
	ecRfDrvMainSts_RTmode_LoggerSettingTxStart1,
	ecRfDrvMainSts_RTmode_LoggerSettingTxWait1,
	ecRfDrvMainSts_RTmode_LoggerSettingData2Set,
	ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence2,
	ecRfDrvMainSts_RTmode_LoggerSettingTxStart2,
	ecRfDrvMainSts_RTmode_LoggerSettingTxWait2,
	ecRfDrvMainSts_RTmode_LoggerSettingData3Set,
	ecRfDrvMainSts_RTmode_LoggerSettingCarrierSence3,
	ecRfDrvMainSts_RTmode_LoggerSettingTxStart3,
	ecRfDrvMainSts_RTmode_LoggerSettingTxWait3,
	ecRfDrvMainSts_RTmode_LoggerSettingAckRxStart,
	ecRfDrvMainSts_RTmode_LoggerSettingAckRxWait,
	ecRfDrvMainSts_RTmode_LoggerSettingAckRxRead,

	ecRfDrvMainSts_HSmode_BeaconInit,
	ecRfDrvMainSts_HSmode_BeaconDataSet,
	ecRfDrvMainSts_HSmode_BeaconTxCs,
	ecRfDrvMainSts_HSmode_BeaconTxStart,
	ecRfDrvMainSts_HSmode_BeaconResRxStart,
	ecRfDrvMainSts_HSmode_BeaconResAckTxCs,
	ecRfDrvMainSts_HSmode_BeaconResAckTxStart,

//	ecRfDrvMainSts_HSmode_FirmHexTxDataSet,
	ecRfDrvMainSts_HSmode_Gw_To_Lg_DataSet,
//	ecRfDrvMainSts_HSmode_FirmHexTxCS,
	ecRfDrvMainSts_HSmode_Gw_To_Lg_CS,
//	ecRfDrvMainSts_HSmode_FirmHexTx,
	ecRfDrvMainSts_HSmode_Gw_To_Lg_Tx,
//	ecRfDrvMainSts_HSmode_FirmAckRx,
	ecRfDrvMainSts_HSmode_Lg_To_Gw_DataAckRx,
	
	ecRfDrvMainGwStsMax
} ET_RfDrvMainGwSts_t;


/*!
 * Radio driver supported modems
 */
typedef enum RadioModems
{
	MODEM_FSK = 0U,
	MODEM_LORA,
} RadioModems_t;


/*!
 * RF動作 internal state machine states definition
 */
typedef enum RFState
{
	RF_SLEEP = 0U,
	RF_IDLE,
	RF_SLEEP_SET_CALL,
	RF_RX_RUNNING_WAIT,
	RF_RX_RUNNING,
	RF_TX_RUNNING,
	RF_CAD,
	RF_CALL_PRE_DETECT_WAIT,
	RF_CALL_PRE_DETECT,
	RF_CALL_PRE_WAIT,
	RF_CALL_SYNC_DETECT,
	RF_CALL_SYNC_MISS,
	RF_CALL_SYNC_WAIT,
	RF_CALL_OUT,
	RF_SLEEP_SET_CALL_RX,
	RF_CALL_RX_PRE_DETECT,
	RF_CALL_RX_SYNC_DETECT,
	RF_CALL_RX_OUT,
	RF_TX_FIFO_FREE,
	RF_TX_FIFO_FULL,
	RF_TX_CREATE_COMMAND,
	RF_TX_DATA_REMAIN_WAIT,
	RF_TX_DATA_REMAIN,
	RF_SEND_OUT_WAIT1,
	RF_SEND_OUT_WAIT2,
	RF_SEND_OUT,
	RF_WAKEUP_STATE_PRE1ST_NOTSET,
	RF_WAKEUP_STATE_PRE1ST_SET,
	RF_WAKEUP_STATE_PRE2ND,
	RF_WAKEUP_STATE_SYNC1ST,
	RF_WAKEUP_STATE_SYNC2ND,
	RF_WAKEUP_STATE_RECIEVE,
	RF_WAKEUP_STATE_END,
} RFState_t;


typedef enum RFWakeupReturn
{
	RF_WAKEUP_RETURN_SUCCESS = 0U,
	RF_WAKEUP_RETURN_CRCMISS,
	RF_WAKEUP_RETURN_FRAMEMISS,
	RF_WAKEUP_RETURN_SYNCMISS,
	RF_WAKEUP_RETURN_PREMISS,
	RF_WAKEUP_RETURN_ERROR,
	RF_WAKEUP_RETURN_NEXT,
	RF_WAKEUP_RETURN_WAIT,
} RFWakeupReturn_t;


typedef enum CommandKind
{
	COMMAND_RESERVE = 0U,
	COMMAND_BROADCAST,
	COMMAND_LOGGER_CALL,
	COMMAND_QUERY_MEASURE_MAIN,
	COMMAND_QUERY_MEASURE_AL,
	COMMAND_QUERY_AUDITTRAIL,
	COMMAND_QUERY_EVENT,
	COMMAND_QUERY_ALARM,
	COMMAND_QUERY_SYSLOG,
} CommandKind_t;


typedef enum ET_RfDrv_CommIntMode
{
	ecRfDrv_LongIntMode = 0U,
	ecRfDrv_MidIntMode,
	ecRfDrv_ShortIntMode,
} ET_RfDrv_CommIntMode_t;


typedef enum ET_RfDrv_InitState
{
	ecRfDrv_Init_Non = 0U,
	ecRfDrv_Init_Hsmode,
	ecRfDrv_Init_Rtmode_Beacon,
	ecRfDrv_Init_Rtmode_Measure,
	ecRfDrv_Init_Rtmode_ChangeSet,
} ET_RfDrv_InitState_t;


typedef enum ET_RfDrv_GwReqCmd
{
	ecRfDrvGw_Hs_ReqCmd_Non = 0U,
	ecRfDrvGw_Hs_ReqCmd_Status,				/* ステータス要求 */
	ecRfDrvGw_Hs_ReqCmd_Measure,			/* 計測値要求 */
	ecRfDrvGw_Hs_ReqCmd_AlartHis,			/* 警報履歴要求 */
	ecRfDrvGw_Hs_ReqCmd_AbnormalHis,		/* 機器異常履歴要求 */
	ecRfDrvGw_Hs_ReqCmd_EventHis,			/* イベント要求 */
	ecRfDrvGw_Hs_ReqCmd_OpeHis,				/* 動作履歴要求 */
	ecRfDrvGw_Hs_ReqCmd_SysHis,				/* システム履歴要求 */
	ecRfDrvGw_Hs_ReqCmd_SetRead,			/* 設定値読込み要求 */
	ecRfDrvGw_Hs_ReqCmd_SetWrite,			/* 設定値書込み要求 */
	ecDummy1,
	ecRfDrvGw_Hs_ReqCmd_Toroku,
	ecDummy2,
	ecDummy3,
	ecDummy4,
	ecDummy5,
	ecDummy6,
	ecDummy7,
	ecRfDrvGw_Hs_ReqCmd_ReqFirm,
	ecRfDrvGw_Hs_FirmHex,
	
	
	
	ecRfDrvGw_Hs_ReqCmd_Max,
} ET_RfDrv_GwReqCmd_t;

typedef enum ET_RfDrv_ResInfo
{
	ecRfDrv_Success,
	ecRfDrv_Fail,
	ecRfDrv_Error,
	ecRfDrv_End,
	ecRfDrv_Continue,
} ET_RfDrv_ResInfo_t;


#if (swLoggerBord == imEnable)
typedef enum ET_Rf_LogSndCtl
{
	ecLogSndCtl_Init = 0,
	ecLogSndCtl_ReadFin,
	ecLogSndCtl_SndFin,
	
} ET_Rf_LogSndCtl_t;
#endif

#endif


