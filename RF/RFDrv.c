/*
 *******************************************************************************
 *	File name	:	RFDrv.c
 *
 *	[内容]
 *		無線通信処理
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2020.9.10
 *******************************************************************************
 */
#include <string.h>
#include <stdlib.h>

#include "cpu_sfr.h"															/* SFR定義ヘッダー */
#include "typedef.h"															/* データ型定義 */
#include "UserMacro.h"															/* ユーザー作成マクロ定義 */
#include "immediate.h"															/* immediate定数定義 */
#include "switch.h"																/* コンパイルスイッチ定義 */
#include "enum.h"																/* 列挙型の定義 */
#include "struct.h"																/* 構造体定義 */
#include "func.h"																/* 関数の外部参照(extern)定義 */
#include "glLabel.h"															/* グローバル変数・定数(const)ラベル定義 */

#include "RF_Struct.h"
#include "RF_Enum.h"
#include "RF_Const.h"
#include "RF_Immediate.h"
#include "sx1272regs-Fsk.h"
#include "sx1272regs-LoRa.h"


/*
 *==============================================================================
 *	プロトタイプ宣言
 *==============================================================================
 */
void SubIntervalTimer_Sleep( uint16_t wkTimerCount, uint16_t wkLoopCount );
void SubIntervalTimer_Stop( void );

/* 初期化関連 */
void SubSX1272POR( void );
void SubSX1272Reset( uint8_t arRstExe );
void SubSX1272InitReg( void );

/* 無線アプリレイヤ */
void ApiRfDrv_TimingCorrect( void );
sint8_t ApiRfDrv_Temp( void );
#if (swLoggerBord == imEnable)
void SubRfDrv_RtConctTiAdd( void );
#else
void SubRfDrv_RtConctCut( void );
#endif

/* 無線通信レイヤ */
/* モード共通 */
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
void SubRfDrv_Wakeup_Cycle( void );			/* 高速モード呼出信号検知繰り返し処理 */
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

/* SX1272制御 */
/* モード共通 */
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
/* 工程用無線受信実行 */
static void SubRfDrv_TestRx( void );
static void SubRfDrv_TestCurrentRx( void );
#endif

#if (swLoggerBord == imEnable)
/* 無線用プロトコルに警報遅延値を書き込む */
static void SubRfDrv_SetAlmDelayArray( uint8_t *arFromAlmParmArray, uint8_t *arToAlmParmArray );
/* 無線用プロトコルに逸脱許容時間を書き込む */
static void SubRfDrv_SetDevi( uint8_t *arToAlmParmArray );
/* リアルタイム接続情報セット */
void SubRfDrv_SetRtConInfo( uint8_t arSel );
/* 定期送信計測データのFlashからリード */
static void SubRfDrv_RtmodeMeasDataFlashRead( uint8_t arSel );
static void SubRfDrv_ChgRealTimeSndPt( void );
/* 定期送信計測データのFlashからリード個数計算 */
static void SubRfDrv_RtmodeDataFlashReadKosu( void );
/* 定期送信計測データセット：リアルタイムモード */
static void SubRfDrv_RTmodeMeasDataSet( uint8_t arSel );
//static void SubRfDrv_RTmodeAuditDataSet( void );

/* 計測値1の過去送信ポインタを更新 */
static void SubRfDrv_RtmodeMeas1OldPtrUpdate( void );
static void SubRfDrv_RtmodeMeasAlmOldPtrUpdate( uint16_t arKosu );

#endif

#if (swRssiLogDebug == imEnable)

void SubRfDrv_RssiLogDebug( void );
static void SubRFDrv_CalFstConnBootCnt( void );
#endif



/*
 *==============================================================================
 *	定数定義
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

/* リアルタイム通信間隔リスト */
const uint16_t cComIntList[ 9U ] = { 0xFFFF, 10U, 20U, 60U, 300U, 600U, 900U, 1200U, 1800U };

/*
 *==============================================================================
 *	変数定義
 *==============================================================================
 */
/* 工程パラメータ */
static sint16_t			rfFreqOffset;
#if (swKouteiMode == imEnable)
static uint16_t			rfCurrentCh_FSK;
static uint16_t			rfCurrentCh_LoRa;					/* 工程モード用 */
#endif
//static int8_t			vrfTempCal;							/* 内蔵温度センサ補正値 */

/* ユーザー設定パラメータ */
//static uint8_t			vrfSysModeLocalCentral;
static ST_RF_SerialID_t	vrfMySerialID;
static uint8_t			vrfLoraChGrupeMes;					/* Loraチャネルグループ設定 計測通信用 */
//static uint8_t			vrfLoraChGrupeCnt;					/* Loraチャネルグループ設定 接続用 */

static uint16_t			vrfHsReqDataNum;					/* 高速通信でのビーコン呼び出しの要求データ件数 */



#if (swLoggerBord == imEnable)
	static uint16_t	vrfRTmodeCommInt;						/* 無線通信間隔 */
	static uint16_t	vrfPreRTmodeCommInt;					/* 前回までの無線通信間隔 */
	static uint8_t	vrfRTmodeCommCutTime;					/* 無線通信失敗によるRTmode切断の失敗回数閾値 */
	static uint8_t	vrfHsmodeSleepMode;						/* 0xFFの場合は変数へ直接0など書き込むまでHSmodeは起動しない */
	static uint8_t	vrfHsWkupDectNum;						/* 高速通信モード 1setでの起動回数 3回ロガー検出用,3回計測設定用 */
#if 0
	static uint8_t vrfGwConnectList[ GW_FILTER_MAXNUM ][ 3U ];	/* GWに接続許可のあるリスト 上詰めで配置 [0U][0U]が0x00 00 00はALL許可 */
#endif
#else
	static uint8_t vrfLoggerSetReserveList[ 5U ][ 3U ];			/* 設定変更予約リスト5台分 */
//	static uint8_t vrfHsmodeDuty;								/* HSmodeのBeaconをON/OFFする周期のduty 0:連続 , 1:25sec/1min , 2:25sec/3min */
//	static uint8_t vrfLoggerDisconnectCnt;						/* ロガー切断判断の設定カウント数 Idleの先頭で+1 */
#if 0
	static uint8_t vrfTorokuMode;								/* GWで新規ロガーの登録受付け */
#endif
#endif

/* ステータス情報 */
static uint8_t			vrfRssi;
static uint8_t	vrfCurrentRssi;
//static uint8_t	vrfCurrentRssi_Big;
#if (swLoggerBord == imEnable)
	static uint8_t	vrfConnectSuccess;
	static uint8_t	vrfRTmodeTimeSlot;
	//static uint16_t vrfRTmodeCommFailNum;
	static uint8_t	vrfRtmodeStopCnt;						/* 2021.8.2 */
#else
	static uint8_t 	vrfRtChSelect;							/* RTmodeでの無線通信チャネルの選択モード「CH_AUTO」「CH_MANU」 */
	static uint8_t	vrfHSLoggerID[ 3U ];					/* 高速通信モード: 要求ロガーID */
	static uint16_t	vrfHSReqHeadMeaID;						/* 高速通信モード: 先頭計測時系列ID */
	static uint16_t	vrfHSReqHeadMeaTime[ 2U ];				/* 高速通信モード: 先頭計測時刻 */
	static uint8_t vrfLoggerSumNumber;
	static uint8_t vrfLoggerList[ 60U ][ 3U ];				/* RTmodeで接続しているロガー */
	static uint8_t vrfLoggerComCnt[ 60U ];					/* 計測値通信のカウント 通信成功でクリア */
	static uint8_t vrfLoggerRssi[ 60U ];
	static uint16_t vrfScanRssiOver;
	static uint16_t vrfScanRssiUnder;
#endif

/* デバッグ変数 */
/* HSmode Debug変数 */
#if swRfMonitor == imEnable
static uint16_t vrfDebug[56U];
//static uint16_t vrfDebug[20U];
#endif

//static uint16_t vrfDebugCntL[50U];
//static uint16_t vrfTest;						/* グローバル宣言 */
static uint16_t vrfErrorCorrectCnt;
/*
static uint16_t vrfCorrectSetCnt;
static uint16_t vrfTestPacketCnt;
static uint16_t vrfTestCrcErrorCnt;
static uint16_t vrfTestCorrectTimeout;
static uint16_t vrfTestHeaderCrcError;
static uint8_t vrfTestHeaderTimeOut;
*/
/* RTmode Debug変数 */
#if (swLoggerBord == imEnable)
#if (swRssiLogDebug == imEnable)
	static uint8_t vrfErorrCause;						/* 通信エラーの原因 */
#endif
	static uint16_t vrfDebugRTmodeCnt;				/* RTmodeデバッグ用 計測値送信回数 */
	static uint16_t vrfDebugRTmodeSuccess;			/* RTmodeデバッグ用 計測値通信成功回数 */
	static uint16_t vrfDebugRTmodeRxTimeout;		/* RTmodeデバッグ用 計測値通信Ack受信タイムアウト回数 */
	static uint16_t vrfDebugRTmodeCadMiss;			/* RTmodeデバッグ用 計測値通信AckのCADミス */
	static uint16_t vrfDebugRTmodeCrc;
#else
#endif

/* 処理用変数 */
static ST_RFPrm_t		vRFPrm;							/* 無線通信処理パラメータ */
static ST_RFStatus_t	rfStatus;						/* 無線部状態パラメータ */

static uint16_t			vSleepLoop;
//static uint16_t			vrfDummy;
static uint16_t			vrfPacketSum;					/* 送信する現在のパケット */
static uint16_t			vrfPacketLimit;					/* 送信するパケット総数 */
static UT_RF_Data_t		vutRfDataBuffer;
static UT_RF_Hsmode_AckData_t vrfHsmodeAckBuff;			/* Ack受信時にvutRfDataBufferが利用できないため */

static ET_RfDrv_CommIntMode_t		vRfDrv_CommIntMode;

static uint8_t		vLoopLoRa;
static uint8_t		vErrorLoopCounter;
static uint8_t 		vCallChLoopLora;
static ET_RfDrv_ResInfo_t		vrfResInfo;			/* HSmodeでロガーからの受信データに対するAck(0U) or Nack(1U)及びGW受信処理の結果 */
//static uint8_t		vRtmode_DataCh[3U];
//static uint8_t		vHsmode_Ch[3U];
static uint8_t		vrfSetCommTime;
static uint16_t		vrfHsmodePacketLength;			/* セットするデータ合計長 */
static ET_RfDrv_GwReqCmd_t vrfHsReqCmdCode;
static uint8_t		vrfMesureCommIntTime;			/* 10sec,20sec,60sec GWのIdle用 */
static uint16_t		vrfRtmodeCommTimeTotal;			/* 送信時間制限用 送信時間カウント変数 */

#if (swLoggerBord == imDisable)
static uint8_t		vrfRtMeasChMask;				/* 計測通信でAppID違いの通信を受信したときにマスクするch */
#endif
//static int8_t		vrfTempCorrect;					/* 環境温度によるタイミング補正 */

/* 2021.7.12 追加 */
static uint8_t		vrfInitCnt;						/* 一定時間無線通信しない場合に初期化 */
static uint8_t 		vrfRfErr;						/* 無線IC関連エラー数 */

#if (swLoggerBord == imEnable)
	static uint16_t			vrfPacketPt[2U];		/* 送信成功したパケットのポインタ(2種) 奇数回数の送信分[0U]と偶数回数の送信分[1U] */
	static uint8_t	vrfCallID[3U];					/* 下位4bitは空白 */
	static uint8_t	vrfCommTargetID[3U];
	static ET_RfDrv_InitState_t vrfInitState;
	static ET_RfDrvMainLoggerSts_t	vRfDrvMainLoggerSts;
	static uint8_t	rfTimeSlotGrupe;
	static uint8_t	rfTimeSlotNum;
	static uint16_t	vRtMode_MeasSndKosu;
	static uint16_t	vRtMode_MeasAlmSndKosu;
	static uint16_t gvrfHsmodePacketPt;				/* Hsmode 送信パケットのポインタ */
	static uint16_t	vMem_ReqIndexNo_St;				/* Flashからリードする先頭のIndexNo */
	static uint16_t vMem_RdIndexNo;					/* FlashからリードするIndexNo */
	static uint16_t	vFirmPacketNum;
	static uint16_t	vFirmPacketSum;
#else
	static ET_RfDrvMainGwSts_t		vRfDrvMainGwSts;
	static uint16_t vrfHsmodeRandom;
	static uint16_t vrfHsLoginUserId;
	static uint16_t	vrfHsLgDct;					/* ロガー検知数 */
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
 *	無線通信処理 パラメータ初期化処理
 *
 *	[内容]
 *		無線通信のパラメータ初期化処理を行う。
 *******************************************************************************
 */
void ApiRFDrv_ParamInitial( void )
{
	vRFPrm.mInitSts = ecRFDrvInitSts_Init;				/* 初期化ステータス */
	vRFPrm.mInitWaitCnt = 100U;							/* イニシャライズ待ちカウンタ(10ms/カウント) */
	vSleepLoop = 0U;
	
	gvRfTimingCounterL = 0U;
	gvRfTimingCounterH = 0U;
	vCallChLoopLora = 0U;
#if (swLoggerBord == imDisable)
	vrfRtMeasChMask = RT_MODE_CH_LOOP_NUM + 1U;		/* 最大値より+1でマスクなし */
#endif
	vrfLoraChGrupeMes = 0U;				/* Loraのチャネルグループ計測通信 0-6 */
//	vrfLoraChGrupeCnt = 0U;				/* Loraのチャネルグループ接続通信 0-1 */
	vRfDrv_CommIntMode = ecRfDrv_LongIntMode;
	vrfMesureCommIntTime = RT_INT_LONG_MODE_TX_INT;
	vrfSetCommTime = RT_INT_LONG_MODE_SET_TIME;
	vrfRtmodeCommTimeTotal = 0U;
//	vrfTempCal = 0U;								/* 内蔵温度センサ補正値 */
	gvRfTimingCorrectCnt = 0U;			/* タイミング補正の3secカウンタ初期化 */
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
//	vrfLoggerDisconnectCnt = 3U;			/* 初期値3回連続エラーで切断 */
	gvrfStsEventFlag.mHsmodeInit = 0U;
	M_CLRBIT(P3,M_BIT0);					/* RF COM LED */
	vrfScanRssiOver = 0U;
	vrfScanRssiUnder = 0U;
//	vrfHsmodeDuty = 0U;					/* FSK送信 連続 */
	vrfHsmodeRandom = 0U;
	vErrorLoopCounter = 0U;
	/* 2021.7.12 追加 */
	vrfInitCnt = 0U;
	/* 2021.7.20 */
	vrfHsLgDct = 0U;
#else
	/* Logger RTmode */
	vrfRTmodeCommInt = 300U;			/* 初期値1min周期の通信間隔 => Idleの初期化でも定義している */
	vrfPreRTmodeCommInt = vrfRTmodeCommInt;
	vrfHsWkupDectNum = 0U;
	
	vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_Init;
	R_IT_Create_Custom( 0xFFF );		/* 125msec */
	gvrfStsEventFlag.mTimerSync = 0U;
	vrfConnectSuccess = 0U;
	vrfRTmodeCommFailNum = 0U;
	vrfRTmodeCommCutTime = 4U;			/* RTmode通信切断判定 初期値3回 */
	vrfHsmodeSleepMode = 0U;
	gvrfStsEventFlag.mSetReserve = 0U;
	gvrfStsEventFlag.mHsmodeeResCmdStatus = 0U;
	vrfRtmodeStopCnt = 0U;							/* 2021.8.2 */
	gvrfStsEventFlag.mOnlineLcdOnOff = 0;			/* LCD OFF 2022.9.16 */
#endif
	
	vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;				/* ロガーステータス(ブロードキャスト) */
	
	/* Debug変数初期化 */
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
 *	無線通信処理 初期化処理
 *
 *	[内容]
 *		無線通信の初期化処理を行う。(10ms周期で本関数を実行)
 *******************************************************************************
*/
void ApiRFDrv_Initial( void )
{
	gvModuleSts.mRf = ecRfModuleSts_Sleep;
	switch( vRFPrm.mInitSts )
	{
		case ecRFDrvInitSts_Init:
			/* 本処理は起動後1度のみ実行 */
			RADIO_SW = RF_SW_ON;
			SubSX1272POR();														/* POR処理 */
			/* 10msecのWait必要 → 10〜11msecのWait */
			SubIntervalTimer_Sleep( 375U * 12U, 1U );
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			vRFPrm.mInitSts = ecRFDrvInitSts_InitPorWait;
			break;
			
		case ecRFDrvInitSts_InitPorWait:
			/*
			 ***************************************
			 *	無線割込み許可してから1秒後の処理
			 *	(無線通信の初期化/設定等)を以下に記載
			 *	 ※本処理は1度のみ実行
			 ***************************************
			 */
			ApiSX1272Reset( imON );
			/* 100usec以上必要 → Wait 1〜2 ms */
			SubIntervalTimer_Sleep( 75U, 1U );		/* 0.1msec */
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			vRFPrm.mInitSts = ecRfDrvInitSts_InitRstWait;
			break;
			
		case ecRfDrvInitSts_InitRstWait:
			ApiSX1272Reset( imOFF );
			/* 5msec必要 → Wait 6〜7 ms */
			SubIntervalTimer_Sleep( 375U * 6U, 1U );		/* 5msec */
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			vRFPrm.mInitSts = ecRfDrvInitSts_InitRstEndWait;
			break;
			
		case ecRfDrvInitSts_InitRstEndWait:
			SubSX1272SetModem( MODEM_FSK );
			/* 初期化 */
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
 *	無線通信処理メイン
 *
 *	[内容]
 *		無線通信処理メイン(1秒周期で本関数を実行)
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
	static uint8_t vOldPtrFlg;				/* 長距離通信選択フラグ：0:計測通信、1:計測警報履歴 */
#else
	uint8_t		wkReg;
	uint8_t		wkErrCode;
#endif
	
	
#if 0
	static uint8_t vTestDebugCnt;	/* debug */
#endif

	
#if (swKouteiMode == imEnable)
	/***** 工程モード *****/
//	gvMode = ecMode_RfTxFSK;		/* 強制的にFSKテスト */
//	rfCurrentCh_FSK = 75U;			/* 強制的にFSKテスト */
//	rfCurrentCh_LoRa = 0U;			/* 強制的にFSKテストの場合は0 */
//	rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
	
	/* LoRa送信 */
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
					
					/* RF(無線通信)の初期化完了 */
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
				
				/* RF(無線通信)の初期化完了 */
				if( ecRFDrvInitSts_InitEnd == ApiRFDrv_GetInitSts() )
				{
					gvModuleSts.mRf = ecRfModuleSts_Run;
					break;
				}
			}
		}
#endif
	}
	/* FSK送信 */
	else if( gvMode == ecMode_RfTxFSK )
	{
		rfStatus.Modem = MODEM_FSK;
		SubRFDrv_PN9FSK( rfCurrentCh_FSK );
		R_WDT_Restart();
		return;
	}
	/* LoRa、FSK受信 */
	else if( gvMode == ecMode_RfRxLoRa || gvMode == ecMode_RfRxFSK )
	{
		SubRfDrv_TestRx();
		return;
	}
	/* LoRa接続 */
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
	
	/*** 技適・周波数調整テスト用 ***/
#if (swRfTxTest == imEnable)
	while( 1U )
	{
		/* 周波数調整用変数 61.035[Hz/Count] */
		rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
		rfCurrentCh_LoRa = 39U;
		SubRFDrv_PN9LoRa( rfCurrentCh_LoRa );
		while( 1U )
		{
			R_WDT_Restart();
		}
	}
#endif
	
	
	/*** 受信感度テスト用 ***/
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
	
	
	/*********** 通常処理 ***********/
#if (swLoggerBord == imEnable) && (swRfTxTest == imDisable) && (swRfRxTest == imDisable)
	
	/* 周波数調整用変数 61.035[Hz/Count] */
//	gvInFlash.mParam.mOnCertLmt = CENTRAL;
	rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
	
	gvModuleSts.mRf = ecRfModuleSts_Sleep;
	gvRfIntFlg.mRadioStsMsk = 0U;		/* 125msec ITimerによるecRfModuleSts_Runを有効 */
	
	/* 電池抜時の処理 */
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
		/* *** ロガー FSKモード ********************/
		case ecRfDrvMainSts_HSmode_BeaconCsInit:
//			rfFreqOffset = RF_FREQ_OFFSET;
			vrfHsWkupDectNum = 0U;
			
			/* 定期的な初期化処理 */
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
			/* 連続WakeUpの防止 */
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
				/* 2回に1回のみステータス呼び出しを検知する */
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
			//breakなし
		case ecRfDrvMainSts_HSmode_BeaconPre1stDetect_Inter:
			if(SubRFDrv_Wakeup_PreDetect() != HS_MODE_PREAMBLE_SUCCESS )
			{
				/* 1.056msec Wait後、再サーチ処理 ただし何故か初回起動時間+0.3msec */
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
					/* ロガー検出を停止する（計測・設定のみ） */
					SubRfDrv_Wakeup_Cycle();
				}
				else
				{
					/* リトライ処理 250msec Sleep */
					wkRadio = gvRfTimingCounterL;			/* gvRfTimingCounterLは0-7で繰り返す */
					if( wkRadio < 6U )
					{
						while( gvRfTimingCounterL < wkRadio + 2U )
						{
							/* コンパレータ実行中はSTOPからの起動による指示ズレ防止として：HALT */
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
							/* コンパレータ実行中はSTOPからの起動による指示ズレ防止として：HALT */
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
				/* 0.96msec Wait後、再サーチ処理 初回は起動時間+0.3msec ?? */
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
					/* リトライ処理 250msec Sleep */
					wkRadio = gvRfTimingCounterL;			/* gvRfTimingCounterLは0-7で繰り返す */
					if( wkRadio < 6U )
					{
						while( gvRfTimingCounterL < wkRadio + 2U )
						{
							/* コンパレータ実行中はSTOPからの起動による指示ズレ防止として：HALT */
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
							/* コンパレータ実行中はSTOPからの起動による指示ズレ防止として：HALT */
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
			/* Syncword検出 受信開始 */
			SubSX1272RxFSK_DIO();						/* SubRFDrv_Wakeup_SyncDetect()でSubSX1272RxFSK_On()実行 */
			SubIntervalTimer_Sleep( 10 * 375U , 1U );
			SubSX1272RxFSK( RF_HSMODE_BEACON_LENGTH );
			SubSX1272Sleep();
			/* CRCチェック */
			if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[ 0U ], RF_HSMODE_BEACON_LENGTH, BCH_OFF ) == SUCCESS )
			{
				/* CRC OK */
				wkRadio = FAIL;
				if( (vutRfDataBuffer.umGwQueryBeacon.mPacketCode == 0x00)
					&& (vutRfDataBuffer.umGwQueryBeacon.mCompanyID == vrfMySerialID.mOpeCompanyID) )
				{
					/* 事業会社ID、パケットコード一致 */
					if( ((vutRfDataBuffer.umGwQueryBeacon.mAppID[0U] == vrfMySerialID.mAppID[0U])
						&& (vutRfDataBuffer.umGwQueryBeacon.mAppID[1U] == vrfMySerialID.mAppID[1U])) )
					{
#if 0
						/* 利用会社ID一致 */
						if( (vrfMySerialID.mAppID[0U] == APP_ID_LOCAL_UP) && (vrfMySerialID.mAppID[1U] == APP_ID_LOCAL_LO) )
						{
							/* ローカルモード */
							if( SubRFDrv_Filter_Gwid( &vutRfDataBuffer.umGwQueryBeacon.mGwID[0U] ) == ecRfDrv_Success )
							{
								wkRadio = SUCCESS;
							}
						}else
						{
							/* 集中監視モード(サーバーモード)ではフィルタリングなし */
							wkRadio = SUCCESS;
						}
#else
						/* 利用会社ID一致 */
						if( SubRFDrv_Filter_Gwid( &vutRfDataBuffer.umGwQueryBeacon.mGwID[0U] ) == ecRfDrv_Success )
						{
							wkRadio = SUCCESS;
							
							wkU32 = (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mGwID[ 0U ] << 12U;
							wkU32 |= (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mGwID[ 1U ] << 4U;
							wkU32 |= (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mGwID[ 2U ] >> 4U;
							
							/* システム履歴：高速通信(対向GWID) */
//							ApiFlash_WriteSysLog( ecSysLogItm_FastCom, wkU32 );
							
						}
#endif
					}
					if( (((vrfMySerialID.mAppID[0U] >> 4U) & 0x0F) == APP_ID_LOGGER_INIT_UP )
						&& ( (((vrfMySerialID.mAppID[0U] << 4U) & 0xF0) + ((vrfMySerialID.mAppID[1U] >> 4U) & 0x0F)) == APP_ID_LOGGER_INIT_LO ))
					{
						/* 利用会社IDが初期値 */
						wkRadio = SUCCESS;
					}
				}
				if( wkRadio == SUCCESS )
				{
					/* システムID整合 初期AppID時は必ず応答する(ただしSyncの関係でGWが登録モード時しか受信しない) */
					wkCulc = SubRfDrv_CallResponseTimeslot( &rfTimeSlotGrupe, &rfTimeSlotNum );
					/* Test */
//					rfTimeSlotGrupe = 3U;
//					rfTimeSlotNum = 0U;
					SubIntervalTimer_Sleep( wkCulc , 1U );
					
					/* 受信ビーコンのデータ処理 遷移先判定,応答タイミング設定 */
					SubRfDrv_Hsmode_Logger_BeaconRxPrcss();
				}else
				{
					/* CompanyID,PacketID,AppID違い・GWIDフィルタリング拒否 */
					SubRFDrv_Hsmode_End();
				}
			}
			else
			{
				/* CRCエラー */
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
					/* rfTimeSlotGrupe == 1Uまでにフラッシュから送信バッファへデータをセットする */
					
					gvrfStsEventFlag.mHsmodeTxTurn = 0U;				/* 送信回数が奇数でセット(最初は1回目) */
					SubRfDrv_Hsmode_Logger_BeaconRxAnalysis();				/* 受信データ解析し、応答準備 */
				}
				else if( rfTimeSlotGrupe == 1U )
				{
					/* 処理時間(6msec) */
//					SubIntervalTimer_Sleep(_CALL_RESPONCE_SEND_TIMESLOT_GRUPE - 2250U, 1U );
					/* -------レスポンスデータのセット(100msec - 2250/375msec)94msec----- */
//					SubIntervalTimer_Sleep(_CALL_RESPONCE_SEND_TIMESLOT_GRUPE - 700U, 1U );	/* -5.5msec */
					SubIntervalTimer_Sleep(_CALL_RESPONCE_SEND_TIMESLOT_GRUPE, 1U );	/* -5.5msec */
					vErrorLoopCounter = 0U;				/* リトライ回数カウント変数 */
					/* パケット数セットとポインタ初期化 */
//					vrfHsReqDataNum = (uint16_t)vutRfDataBuffer.umGwQueryBeacon.mRequestEndMeasureCount[0U] << 8U;
//					vrfHsReqDataNum += vutRfDataBuffer.umGwQueryBeacon.mRequestEndMeasureCount[1U];
					
					vrfPacketSum = 1U;					/* ロガーの場合はvrfPacketSumが0まで減っていき、GWの場合はvrfPacketLimitまで増えていく */
					vrfPacketLimit = 0U;
					vrfPacketPt[0U] = 1U;
					vrfPacketPt[1U] = 2U;
					
					gvrfHsmodePacketPt = vrfPacketPt[1U];		/* メインルーチンにリターンするポインタ */
					
//					gvrfStsEventFlag.mHsmodeTxTurn = 0U;				/* 送信回数が奇数でセット(最初は1回目) */
					
					SubRFDrv_Hsmode_Logger_ResponceDataSet( 0U );		/* Flashからリードしたデータを無線バッファに格納 */
					
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
			/* 繰り返し送信を設定 */
			if( vrfPacketSum == 0U )
			{
				/* パケット送信完了 */
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
				/* 2021.11.4 外乱耐性の強化 */
//				if( vLoopLoRa < 30U )
				if( vLoopLoRa < 50U )
				{
					SubIntervalTimer_Sleep( (3U - 2U) * 375U, 1U );		/* キャリアセンス所要時間 2.2msec */
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
				
				/*	2021.7.12 追加 */
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
						/* 送信開始からGWのレスポンスまでHALT */
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
				/*** 繰り返し送信を設定 ***/
				gvrfStsEventFlag.mHsmodeTxTurn = ~gvrfStsEventFlag.mHsmodeTxTurn;			/* データセットのため奇数偶数入れ替え */
//				SubIntervalTimer_Sleep( 95U * 375U, 1U );									/* 50msecの間に次パケットのデータセット */
				SubIntervalTimer_Sleep( (45U - 10U) * 375U, 1U );							/* 50msecの間に次パケットのデータセット */
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
				
				/* 乾電池AD測定 */
				ApiAdc_AdExe();

				/* Ack受信 */
				switch ( vrfHsReqCmdCode )
				{
					case ecRfDrvGw_Hs_ReqCmd_Status:
						/* 2021.8.2 長距離通信を飛ばす */
						/* 2021.8.27 */
						if( vrfConnectSuccess == 1U )
						{
							vrfRtmodeStopCnt = (180U / vrfMesureCommIntTime);					/* 3min間(+1回) 長距離通信を停止する(タイミングにより直ぐ-演算) */
						}
						else
						{
							vrfRtmodeStopCnt = 7U;												/* 7 * 35sec = 245sec休止 */
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
						vLoopLoRa = 25U;													/* 25 * 2msec 受信待ち */
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
					/* 続けてFlashからデータ収集をリードするためのキューをセット */
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
						/* GWからNACK応答が予測されるため次パケットはセットしない */
//						SubIntervalTimer_Sleep( 95U * 375U, 1U );							/* GWの受信処理を見越して待ちを長くしてもOK ただし送信休止50msec以下はNG */
						SubIntervalTimer_Sleep( (45U - 10U) * 375U, 1U );							/* GWの受信処理を見越して待ちを長くしてもOK ただし送信休止50msec以下はNG */
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
			/* 2021.11.4 外乱耐性の強化 */
//			vLoopLoRa = 19U;													/* 25 * 2msec 受信待ち */
			vLoopLoRa = 39U;
			break;
			
		case ecRfDrvMainSts_HSmode_BeaconResDataSetWait:
			switch( vrfHsReqCmdCode )
			{
				case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_FirmHexRx;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					vFirmPacketNum = 0U;										/* ファームアップデータパケット番号初期化 */
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
						/* 最低待ち時間50msecオーバー */
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
				/* 最低待ち時間50msecオーバー */
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
			SubSX1272RxFSK_DIO();										/* SubRFDrv_Wakeup_SyncDetect()でSubSX1272RxFSK_On()実行 */
#if swRfPortDebug == imEnable
			P7_bit.no4 = 0U;
#endif
			if( SubRFDrv_Wakeup_PreDetect() != HS_MODE_PREAMBLE_SUCCESS )
			{
				if( vLoopLoRa > 0U)
				{
					/* Ack受信リトライ */
					SubIntervalTimer_Sleep( 1U * 375U - _PREAMBLE_WAIT_ITMCP_VALUE, 1U );							/* AckのPreamble長は4msec */
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
					/* Ack受信タイムアウト処理 */
					if( vErrorLoopCounter < 10U )
					{
						gvrfStsEventFlag.mHsmodeTxTurn = ~gvrfStsEventFlag.mHsmodeTxTurn;			/* データセットのため奇数偶数入れ替え */
						/*	パケット再送 */
						vErrorLoopCounter ++;
						/* ポインタ変更なし */
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;

						gvModuleSts.mRf = ecRfModuleSts_Run;
					}else
					{
						/* リトライ回数オーバー */
						SubRFDrv_Hsmode_End();
					}
				}
			}
			else
			{
#if swRfPortDebug == imEnable
			P7_bit.no4 = 1U;
#endif
				/* プリアンブル検知 コマンドにより分岐 */
				switch ( vrfHsReqCmdCode )
				{
					case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_FirmHexRx;
						vFirmPacketNum = 0U;									/* ファームアップデータパケット番号初期化 */
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
			/* Logger ⇒ GWのデータ伝送方向におけるGW ⇒ LoggerのAck Preamble検出 受信開始 */
			SubSX1272RxFSK_DIO();										/* SubRFDrv_Wakeup_SyncDetect()でSubSX1272RxFSK_On()実行 */
//			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_ACK );
			SubSX1272RxFSK_On();
			SubIntervalTimer_Sleep( 10U * 375U, 1U );					/* GWのACK送信時間4.9msec */
			SubSX1272RxFSK( RF_HSMODE_ACK_LENGTH );
//			SubSX1272Stdby();
#if swRfPortDebug == imEnable
			P7_bit.no4 = 1U;
#endif
			wkRadio = SUCCESS;
			if( !gvRfIntFlg.mRadioTimer )
			{
				/* CRCチェック */
				if( SubRfDrv_Crc_Decoding_HS( &vrfHsmodeAckBuff.umData[ 0U ], RF_HSMODE_ACK_LENGTH, BCH_OFF ) == SUCCESS )
				{
					SubSX1272Sleep();
					/* Ack or Nackの判定 */
					/* 通信相手のGWでない or 自機宛でない:送信終了 */
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
								/* 計測値通信 */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x0E )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_AlartHis:
								/* 警報履歴通信 */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x0F )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
								/* 機器異常履歴通信 */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x15 )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_EventHis:
								/* イベント履歴通信 */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x16 )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_OpeHis:
								/* 動作履歴通信 */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x17 )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_SysHis:
								/* システム履歴通信 */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x18 )
								{
									wkRadio = SUCCESS;
								}
								break;
							case ecRfDrvGw_Hs_ReqCmd_SetRead:
								/* 設定読み込み */
								if( vrfHsmodeAckBuff.umCategories.mPacketCode == 0x08 )
								{
									wkRadio = SUCCESS;
								}
								break;
							default:
								/* パケット違い　終了 */
								break;
						}
					}
					if( wkRadio == SUCCESS )
					{
						vLoopLoRa = 0U;			/* 送信成功につきキャリアセンスループ0クリア */
						
						/* 受信正常 */
						if( vrfHsmodeAckBuff.umCategories.mResponce == 0x01 )
						{
							/* NACK受信 パケット再送 (注意：永遠NACK応答だと永久ループする) */
							/* ポインタ変更なし */
							SubRfDrv_Hsmode_RptTx();
//							vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;
//							SubIntervalTimer_Sleep( 2U * 375U, 1U );
//							gvModuleSts.mRf = ecRfModuleSts_Sleep;
						}
						else
						{
							/* ACK受信 */
							if( vrfPacketSum > 0U )
							{
								vrfPacketSum--;					/* 次のパケット */
							}
							vErrorLoopCounter = 0U;				/* リトライ回数カウント変数リセット */
							
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
									/* 奇数回のデータセットされている場合は偶数回の送信が成功 */
									vrfPacketPt[1U] += 2U;
								}
								else
								{
									/* 偶数回のデータセットされている場合は奇数回の送信が成功 */
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
					/* CRCエラー */
					if( vErrorLoopCounter < 10U )
					{
						/*	パケット再送 */
						vErrorLoopCounter ++;
						/* ポインタ変更なし */
						SubRfDrv_Hsmode_RptTx();
//						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;
//						SubIntervalTimer_Sleep( 2U * 375U, 1U );
//						gvModuleSts.mRf = ecRfModuleSts_Sleep;
					}else
					{
						/* リトライ回数オーバー 送信終了 */
						SubRFDrv_Hsmode_End();
					}
				}
			}else
			{
				/* Ack受信タイムアウト */
				if( vErrorLoopCounter < 10U )
				{
					/*	パケット再送 */
					vErrorLoopCounter ++;
					/* ポインタ変更なし */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_BeaconResTxStart;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}else
				{
					/* リトライ回数オーバー 送信終了 */
					SubRFDrv_Hsmode_End();
				}
			}
			
			/* Flashからリードしたデータを無線バッファに格納 */
			if( vrfPacketSum > 0 )
			{
				SubRFDrv_Hsmode_Logger_ResponceDataSet( 1U );					/* Pointer gvrfHsmodePacketPtをセット */
			}
			
			if( vRfDrvMainLoggerSts == ecRfDrvMainSts_HSmode_BeaconResTxStart )
			{
				/* ロガー応答データのパケットポインタを設定 */
				SubRFDrv_Hsmode_PcktPointerSet();
				
				/* 次の送信ポインタセット */
				SubRfDrv_Hsmode_Logger_SndPtSet();
			}
			break;
			
		/* 設定変更受信 */
		case ecRfDrvMainSts_HSmode_SettingRx:
#if 0		/* 書き込みエラーCRC検証 */
			SubSX1272Sleep();
			SubSX1272RxFSK_DIO();										/* SubRFDrv_Wakeup_SyncDetect()でSubSX1272RxFSK_On()実行 */
			SubSX1272FSK_Preamble( RF_FSK_PREAMBLE_LENGTH_ACK );
			SubSX1272RxFSK_On();
#if swRfPortDebug == imEnable
			P7_bit.no4 = 1U;
#endif
			SubIntervalTimer_Sleep( 60U * 375U, 1U );					/* 受信データ長 12msec Ack送信Wait 50msec */
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
				/* CRCチェック */
				if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[ 0U ], RF_HSMODE_RES_SETWRITE_LENGTH_HEADER, BCH_OFF ) == SUCCESS )
				{
					/* Ack or Nackの判定 */
					if( memcmp( vutRfDataBuffer.umGwWriteSet.mGwID, vrfCallID, 3U ) != 0U )
					{
						/* 通信相手のGWでない 送信終了 */
						wkRadio = ERROR;
						
						/* 操作履歴(不正設定要求、自動操作) */
						wkU32 = (vutRfDataBuffer.umGwWriteSet.mGwID[0U] << 12U) + (vutRfDataBuffer.umGwWriteSet.mGwID[1U] << 4U) + (vutRfDataBuffer.umGwWriteSet.mGwID[2U] >> 4U);
						ApiFlash_WriteActLog( ecActLogItm_DisSetReq, wkU32, 0U );
					}
					else if( memcmp( vutRfDataBuffer.umGwWriteSet.mLoggerID, vrfMySerialID.mUniqueID, 3U ) != 0U )
					{
						/* 自機宛でない 送信終了 */
						wkRadio = ERROR;
					}
					else if( vutRfDataBuffer.umGwWriteSet.mPacketCode != 0x0A )
					{
						/* 受信パケットが違う */
						wkRadio = ERROR;
					}
					
					if( wkRadio == SUCCESS )
					{
						/* CRCチェック */
						CRCD = 0x0000U;
						for( wkRadio = RF_HSMODE_RES_SETWRITE_LENGTH_HEADER; wkRadio < (RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_BCH - 2U); wkRadio++ )
						{
							CRCIN = vutRfDataBuffer.umData[wkRadio];
						}
						if((vutRfDataBuffer.umGwWriteSet.mCrc2[1U] == (uint8_t)(CRCD) )
								&& (vutRfDataBuffer.umGwWriteSet.mCrc2[0U] == (uint8_t)(CRCD >> 8U) ) )
						{
							/* エラーなし Ack応答 */
//							vrfTest++;
						}
						else
						{
							/* エラーあり エラー訂正処理 20データごとに処理(端数はループ外で処理) */
							for( wkRadio = 0U; wkRadio < 7U; wkRadio++ )
							{
								vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( &vutRfDataBuffer.umData[ RF_HSMODE_RES_SETWRITE_LENGTH_HEADER + ( wkRadio * 20U ) ] 
																			,RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_HEADER - RF_HSMODE_RES_SETWRITE_LENGTH_BCH -  ( wkRadio * 10U)
																			,20U );
							}
							vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( &vutRfDataBuffer.umData[ RF_HSMODE_RES_SETWRITE_LENGTH_HEADER + ( wkRadio * 20U ) ]
																		, RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_HEADER - RF_HSMODE_RES_SETWRITE_LENGTH_BCH -  ( wkRadio * 10U)
																		, RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_HEADER - RF_HSMODE_RES_SETWRITE_LENGTH_BCH - ( wkRadio * 20U ) );
							/* CRCチェック */
							CRCD = 0x0000U;
							for( wkRadio = RF_HSMODE_RES_SETWRITE_LENGTH_HEADER; wkRadio < (RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_BCH - 2U); wkRadio++ )
							{
								CRCIN = vutRfDataBuffer.umData[wkRadio];
							}
							if((vutRfDataBuffer.umGwWriteSet.mCrc2[1U] == (uint8_t)(CRCD) )
									&& (vutRfDataBuffer.umGwWriteSet.mCrc2[0U] == (uint8_t)(CRCD >> 8U) ) )
							{
								/* エラー訂正成功 Ack応答 */
//								vrfTest++;
//								vrfCorrectSetCnt++;
								wkRadio = SUCCESS;
							}
							else
							{
								/* 通信失敗 パケット破棄 無応答 */
								vrfResInfo = ecRfDrv_Fail;
//								vrfTestCrcErrorCnt++;
								wkRadio = ERROR;
							}
						}
					}
					else
					{
						/* 受信パケットエラー */
						wkRadio = ERROR;
					}
				}
				else
				{
					/* ヘッダーCRCエラー */
					wkRadio = ERROR;
				}
			}
			else
			{
				/* 受信タイムアウトエラー */
				wkRadio = ERROR;
			}
			
#if swRfPortDebug == imEnable
			P7_bit.no4 = 1U;
#endif
			if( wkRadio != ERROR )
			{
				ApiInFlash_SetValWrite( &vutRfDataBuffer.umGwWriteSet );				/* RAMに設定値書込み */
				
				vLoopLoRa = 30U;
				SubRFDrv_Hsmode_Lg_To_Gw_AckDataSet();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxCs;
//				gvModuleSts.mRf = ecRfModuleSts_Run;
#if 0	/* ロガー設定書き込みNG検討 */
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
					SubIntervalTimer_Sleep( 15U * 375U, 1U );					/* リトライ間隔93msec wait 10msec + RX 85msec  */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_SettingRx;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					SubRFDrv_Hsmode_End();
				}
			}
			break;
			
		/* ファームHEX受信 */
		case ecRfDrvMainSts_HSmode_FirmHexRx:
			/* Firm Update Preamble検出 受信開始 */
			SubSX1272RxFSK_DIO();
			SubSX1272RxFSK_On();
			SubIntervalTimer_Sleep( 100U * 375U, 2U );			/* GW側のデータ処理時間を考慮 */
			SubSX1272RxFSK( RF_HSMODE_FIRM_HEX_LENGTH );		/* SubRFDrv_Wakeup_SyncDetect()でSubSX1272RxFSK_On()実行 */
			SubSX1272Sleep();
			
			vrfInitState = ecRfDrv_Init_Non;
			vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxCs;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			vrfResInfo = ecRfDrv_Fail;
			
			if( !gvRfIntFlg.mRadioTimer )
			{
				//SubIntervalTimer_Stop();
				//SubIntervalTimer_Sleep( 50U * 375U, 1U );					/* GWの送信時間制限を考慮し50msec Wait */
				/* CRCチェック */
				if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[ 0U ], RF_HSMODE_FIRM_HEX_LENGTH_HEADER, BCH_OFF ) == SUCCESS )
				{
					/* Ack or Nackの判定 */
					if( memcmp( vutRfDataBuffer.umFirmHex.mGwID, vrfCallID, 3U ) == 0U )
					{
						if( memcmp( vutRfDataBuffer.umFirmHex.mLoggerID, vrfMySerialID.mUniqueID, 3U ) == 0 )
						{
							if( vutRfDataBuffer.umFirmHex.mPacketCode == 0x13 )
							{
								
								gvrfStsEventFlag.mHsmodeRxEnter = 1U;				/* 受信開始フラグ */
								vErrorLoopCounter = 0U;								/* リトライカウンタ リセット */
								
								for( wkRadio = 0U; wkRadio < (RF_HSMODE_FIRM_HEX_LENGTH_HEX_CRC / 20U); wkRadio++ )		/* 45ブロック(計900data CRC含む) */
								{
									/* エラー訂正処理 */
									vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( &vutRfDataBuffer.umFirmHex.mPacketNo[wkRadio * 20U] , RF_HSMODE_FIRM_HEX_LENGTH_HEX_CRC - (wkRadio * 10U) + 2U, 20U);
								}
								/* CRCチェック */
								CRCD = 0x0000U;
								for( wkCulc = 0U; wkCulc < (RF_HSMODE_FIRM_HEX_LENGTH_HEX_CRC - 10U); wkCulc++ )
								{
									CRCIN = vutRfDataBuffer.umFirmHex.mHex[wkCulc];
								}
								
								if((vutRfDataBuffer.umFirmHex.mCrc2[1U] == (uint8_t)(CRCD) )
									&& (vutRfDataBuffer.umFirmHex.mCrc2[0U] == (uint8_t)(CRCD >> 8U) ) )
								{
									/* エラー訂正成功 Ack応答 */
									vrfResInfo = ecRfDrv_Success;
//									vrfTest++;
//									vrfCorrectSetCnt++;
								}
								else
								{
									M_NOP;
									/* 通信失敗 パケット破棄 Nack応答 */
//									vrfTestCrcErrorCnt++;
								}
							}
							else
							{
								M_NOP;
								/* パケット種別コードのエラー 無応答 */
							}
						}
						else
						{
							M_NOP;
							/* 自機宛ではない 無応答 */
						}
					}
					else
					{
						M_NOP;
						/* 通信先GWではない */
					}
				}
				else
				{
					M_NOP;
					/* CRCエラー Nack応答 */
//					vrfTestHeaderCrcError++;
				}
			}
			else
			{
				/* 受信タイムアウト Nack応答 */
				//SubIntervalTimer_Sleep( 50U * 375U, 1U );
//				vrfTestHeaderTimeOut++;
			}
			
			SubSX1272Sleep();
			if( vrfResInfo == ecRfDrv_Success )
			{
				vLoopLoRa = 40U;
				//SubRFDrv_Hsmode_Logger_BcnAckDataSet();
				/* ファームアップデータ格納 */
				vrfResInfo = SubRfDrv_FirmStore( &vutRfDataBuffer.umFirmHex );
				
				/* ACK/NACKデータセット */
				SubRFDrv_Hsmode_Lg_To_Gw_AckDataSet();
			}
			
			if( vrfResInfo != ecRfDrv_Success )
			{
				/* ERRORの場合は無応答処理 タイムアウト以外のエラーは永久ループ防止が必要かも(ERROR続きは考えられないが) */
				vErrorLoopCounter++;
				if( vErrorLoopCounter < 20U )
				{
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_FirmHexRx;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
					gvFirmupMode = imOFF;					/* ファームアップ通信失敗→未実行 */
					
					/* ERRORのリトライ回数がオーバー */
					vErrorLoopCounter = 0U;
					SubRFDrv_Hsmode_End();
				}
			}
			else if( gvrfStsEventFlag.mHsmodeRxEnter == 0U )
			{
				gvFirmupMode = imOFF;						/* ファームアップ通信失敗→未実行 */
				/* パケット受信開始していない状態 */
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
						/* ファームのパケット総数を事前にGWからもらわなければならない */
						if( vFirmPacketNum >= vFirmPacketSum )
						{
							SubRFDrv_Hsmode_End();
							
							if( gvModuleSts.mFirmup == ecFirmupModuleSts_Pre )
							{
								gvModuleSts.mFirmup = ecFirmupModuleSts_Run;
							}
						}
					}
					/* 送信成功時も失敗時も受信を継続する */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_FirmHexRx;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					break;
				case ecRfDrvGw_Hs_ReqCmd_SetWrite:
					SubSX1272TxFSK( RF_HSMODE_ACK_LENGTH );
					SubSX1272Sleep();
					/* 3回Ack送信 */
					vLoopLoRa++;
					if( vLoopLoRa < 3U )
					{
						SubIntervalTimer_Sleep( 1U * 375U, 1U );
						vRfDrvMainLoggerSts = ecRfDrvMainSts_HSmode_Lg_To_Gw_AckTxStart;
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
					}
					else
					{
						/* RAMに設定値書込み(送信成功時も失敗時も設定値格納) */
						gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
						SubRFDrv_Hsmode_End();
					}
					break;
				default:
					SubRFDrv_Hsmode_End();
					break;
				}
			break;
			
		/*************** ロガー LoRaモード ********************/
		case ecRfDrvMainSts_RTmode_Init:
			/* 2022.9.16 電源投入後にすぐ接続通信 */
			gvRfTimingCounterH = 30;
			gvrfStsEventFlag.mReset = RFIC_INI;
#if 0
			rfStatus.Modem = MODEM_FSK;
			SubSX1272Sleep();
			SubSX1272Write(REG_RXBW, RF_RXBW_MANT_16 | RF_RXBW_EXP_2);
			rfStatus.Modem = MODEM_LORA;
			SubSX1272Sleep();
			/* 工場出荷時 設定値 */
			rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
			vrfMySerialID.mUniqueID[0U] = gvInFlash.mProcess.mUniqueID[ 0U ];
			vrfMySerialID.mUniqueID[1U] = gvInFlash.mProcess.mUniqueID[ 1U ];
			vrfMySerialID.mUniqueID[2U] = gvInFlash.mProcess.mUniqueID[ 2U ];
			vrfMySerialID.mOpeCompanyID = gvInFlash.mProcess.mOpeCompanyID;
			
			/* ユーザー 設定値 */
//			gvInFlash.mParam.mOnCertLmt = CENTRAL;
//			gvInFlash.mParam.mOnCertLmt = LOCAL;
//			gvInFlash.mParam.mrfLoraChGrupeCnt = 0;
			vrfMySerialID.mGroupID = gvInFlash.mParam.mGroupID;
			/* AppIDの展開 初期値,ローカルモード時は固定値 */
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
			
			/* ユーザー設定値 */
//			vrfLoraChGrupeMes = 1U;			/* GWより取得 */
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
			/* breakなし */
			
		case ecRfDrvMainSts_RTmode_BeaconInit:
			/* 定期的な初期化処理 */
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
				ApiLcd_Low7SegNumDsp( gvRfTimingCounterH, imOFF );				/* キャリアセンスで引っかかった場合は+100U */
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
			/* 接続GWの下三桁ID(LCD表示用) */
			gvLcdComGwId = (vrfCommTargetID[ 1U ] << 4U) + (vrfCommTargetID[ 2U ] >> 4U);
			
			vrfInitState = ecRfDrv_Init_Rtmode_Measure;
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		/****** Logger Idle ******/
		case ecRfDrvMainSts_RTmode_LoggerIdle:
			wkRadio = gvRfTimingCounterH % 35UL;
			wkRadio2nd = gvRfTimingCounterH % vrfMesureCommIntTime;				/* 10,20,60sec周期で"0" */
//			vrfRTmodeCommInt = 0xFFFF;
			wkRtmodeInterval = gvRfTimingCounterH % vrfRTmodeCommInt;			/* 測定周期で"0" */
//			wkHsmodeInterval = gvRfTimingCounterH % 6U;
			wkHsmodeInterval = gvRfTimingCounterH % 6UL;
			/* 警報発生時に立つフラグがあれば、それをチェックし、wkRadio2ndが0かつフラグセットされていればwkRtmodeIntervalを0にする */
			
			gvFirmupMode = imOFF;												/* ファームアップ未実行 */
			
			if( wkRadio2nd == 0U )
			{
				/* RT mode計測値通信インターバルでの0sec */
				if( gvRfTimingCounterL == 0U )
				{
#if 0
					/* 温度によるタイミング補正 */
					if( vRfDrv_CommIntMode == ecRfDrv_LongIntMode )
					{
						ApiRfDrv_TimingCorrect();					/* 通信周期の長い場合のみ修正 */
					}else
					{
						gvRfTimingTempCorrect = 0U;
					}
#endif
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
					gvrfStsEventFlag.mSetReserve = 0U;			/* 1setのTxで諦める */
					vrfRtmodeCommTimeTotal = 0U;				/* 送信時間制限用の設定変更回数をクリア */
					gvrfStsEventFlag.mTempMeasure = 0U;
					/* 2021.8.2 */
					if( vrfRtmodeStopCnt > 0U )
					{
						vrfRtmodeStopCnt--;								/* 高速通信での検知でセット */
					}
					
					/* 測定警報発生/解除あり：割込み長距離計測警報通信実施 */
					if( gvMeasAlmNum )
					{
						wkRtmodeInterval = 0U;
						/* 2021.8.2 */
						vrfRtmodeStopCnt = 0U;
					}
					
//					vrfInitState = ecRfDrv_Init_Non;
					if( (vrfRTmodeCommFailNum >= vrfRTmodeCommCutTime) || (vrfRTmodeCommInt == 0xFFFF) )
					{
						/* GW接続解除処理 */
						/*デバッグ
						if( vrfDebugRTmodeCnt > 0U )
						{
							vrfDebugRTmodeCnt = 0U;
						}*/
						
						SubRfDrv_SetRtConInfo( imOFF );			/* 接続解除 */
						vrfConnectSuccess = 0U;
						/* 2021.8.27 */
						vrfRtmodeStopCnt = 0U;
						vrfRssi = 0U;
						vrfRTmodeCommFailNum = 0U;				/* 失敗回数リセット */	/* 2021.8.24 削除 ⇒ 2021.9.22復活 */
						SubRfDrv_RtConctTiAdd();				/* 次回接続ビーコンのタイミング調整 */
						vrfCommTargetID[0U] = 0x00;
						vrfCommTargetID[1U] = 0x00;
						vrfCommTargetID[2U] = 0x00;
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconInit;
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
						break;			/* BeaconInitに遷移するため */
					}
					
					if( (vrfConnectSuccess == 0U) && (gvRfTimingCounterH > 315U) )
					{
						/* 内蔵温度取得 */
						ApiRfDrv_Temp();
						SubRfDrv_RtConctTiAdd();				/* 接続通信のタイミングをずらす */
						/* 2021.7.12 追加 */
						vrfInitCnt ++;
						if( vrfInitCnt > RF_INI_CNT )
						{
							vrfInitCnt = 0U;
							gvrfStsEventFlag.mReset = RFIC_INI;
							/* 無線ICエラー 履歴or表示 */
							vRfTestFlg = 1U;

						}
					}
					/* 2021.7.12 追加 */
					if( vrfRfErr > RF_ERR_CNT )
					{
						/* 無線ICエラー 履歴or表示 */
						vRfTestFlg = 2U;
					}

					if( vRfDrv_CommIntMode != ecRfDrv_LongIntMode )
					{
						/* 10,20secインターバル時は1minごとに温度更新 */
						ApiRfDrv_Temp();
					}
#if 0
					else if( vrfConnectSuccess && (vrfInitState != ecRfDrv_Init_Rtmode_Measure) )
					{
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
						gvModuleSts.mRf = ecRfModuleSts_Run;
						break;			/* MeasureInitに遷移するため */
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
			/* 電池抜時の処理 */	
			if( gvrfStsEventFlag.mPowerInt == BATT_POWER_OFF )
			{
				ApiRfDrv_ForcedSleep();
				vrfErorrCause = 26U;
#if (swRssiLogDebug == imEnable)
				SubRfDrv_RssiLogDebug();
#endif
				break;
			}
			
			if( (wkRadio == 0U) && (vrfConnectSuccess == 0U) && (vrfRtmodeStopCnt == 0U) )			/* 2021.8.2 高速通信時に飛ばす */
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
				/* 35sec周期 0sec 未接続状態での接続通信 */
				if( gvRfTimingCounterL < 2U )				/* 初期化時のループ考慮 */
				{
					/* 35sec周期 RTmode BeaconTx */
					if( vrfInitState == ecRfDrv_Init_Rtmode_Beacon )
					{
						/* 初期設定済み */
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconCarrierSens;
					}
					else
					{
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconInit;
					}
					SubIntervalTimer_Sleep( rand() % 20U * 375U, 1U );		/* 接続通信のタイミングをずらす */
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					/* 複数送信の防止(SleepせずにRun状態で遷移した場合) */
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				break;
			}
//			if( (wkRtmodeInterval < (vrfMesureCommIntTime - vrfSetCommTime)) && (vrfConnectSuccess != 0U) && (vrfRtmodeStopCnt == 0U) )			/* 2021.8.2 高速通信時に飛ばす */
			if( (wkRtmodeInterval < (vrfMesureCommIntTime - vrfSetCommTime)) && (vrfConnectSuccess != 0U) )			/* 2021.8.2 高速通信時に飛ばす ⇒　2021.8.24 判定部を後ろに移動 */
			{
				/* 1-39sec 低消費電流化が必要 */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
				if( vrfRTmodeTimeSlot > 0U )
				{
					/* GW接続済み */
					wkCulc = (vrfRTmodeTimeSlot - 1U) * 660U;			/* msecに変換 max39,600U */
					if( wkRadio2nd == wkCulc / 1000U )			/* 秒単位の判定 */
					{
						if(vrfInitState == ecRfDrv_Init_Rtmode_Measure)
						{
							/* GWフィルタリングのチェック */
							if( SubRFDrv_Filter_Gwid( &vrfCommTargetID[0U] ) == ecRfDrv_Fail )
							{
								vrfRTmodeCommFailNum = vrfRTmodeCommCutTime;
								break;
							}
							/* 2021.8.24 高速通信範囲内のため通信中断 */
							if( vrfRtmodeStopCnt > 0U )
							{
								vrfRTmodeCommFailNum++;
								break;
							}

							/* 通信周期のチェック */
							
							/* RTmodeでの送信開始(wait付加) */
							if( RT_MODE_TIME_SLOT_IT_COUNT[vrfRTmodeTimeSlot - 1U] > 0U )
							{
								/* 0x0は除く */
#if 0
								SubIntervalTimer_Sleep( RT_MODE_TIME_SLOT_IT_COUNT[vrfRTmodeTimeSlot - 1U], 12U );			/* 87msecまで */
#else
								SubIntervalTimer_Sleep( RT_MODE_TIME_SLOT_IT_COUNT[vrfRTmodeTimeSlot - 1U] + 2812UL, 12U );			/* タイミング調整用+90msec( 2812 * 12 ) */
#endif
								gvModuleSts.mRf = ecRfModuleSts_Sleep;
							}
							else
							{
#if 0
								gvModuleSts.mRf =ecRfModuleSts_Run;
#else
								SubIntervalTimer_Sleep( 60U * 375U, 1U );			/* タイミングずれを考慮し送信を遅らせる */
								gvModuleSts.mRf = ecRfModuleSts_Sleep;
#endif
							}
							vrfDebugRTmodeCnt++;				/* Debug用 送信回数カウント */
							if( vrfDebugRTmodeCnt >= 0xFFFFU )
							{
								vrfDebugRTmodeCnt = 1U;
							}
							/* 計測値送信(警報履歴送信) */
							vLoopLoRa = 0U;
							
							/* 計測警報発生/解除回数あり：回数分Flashからリード */
							if( gvMeasAlmNum > 0U )
							{
								/* 計測警報履歴の送信 */
								vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasAlmDataSet;
								
								if( gvMeasAlmNum > 6U )
								{
									vRtMode_MeasAlmSndKosu = 6U;
								}
								else
								{
									vRtMode_MeasAlmSndKosu = gvMeasAlmNum;
								}
								
								SubRfDrv_RtmodeMeasAlmOldPtrUpdate( vRtMode_MeasAlmSndKosu );	/* FlashリードIndexNo.をセット */
								SubRfDrv_RtmodeMeasDataFlashRead( MEASALM_DATASET );			/* Flashから個数分リード(警報履歴) */
							}
							else
							{
								/* 計測値の送信 */
								vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureDataSet;
								
								SubRfDrv_RtmodeDataFlashReadKosu();							/* 計測値過去送信個数 */
								if( vRtMode_MeasSndKosu != 0U )
								{
									SubRfDrv_RtmodeMeasDataFlashRead( MEASURE_DATASET );	/* Flashから個数分リード(測定値) */
								}
							}
						}
						else
						{
							/* 初期化が必要 */
							//vrfDebug[0U]++;
							vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
							gvModuleSts.mRf = ecRfModuleSts_Run;
						}
					}
					else if( wkHsmodeInterval == 0U )
					{
						/* Measure送信タイミング以外は高速モード受信待機 */
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
					/* GW接続状態だがTimeSlot == 0U ?? */
//					vrfConnectSuccess = 0U;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
					//vrfDebug[1U]++;
					SubRfDrv_SetRtConInfo( imOFF );		/* 接続しなおし */
				}
				break;
			}
			
			/* 40-59sec 設定変更時に動作 設定変更予約がない場合は高速モード */
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			
			if( gvrfStsEventFlag.mSetReserve && (vrfRtmodeCommTimeTotal < 8U) )				/* リトライは8回まで */
			{
				vrfRtmodeCommTimeTotal++;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetBeaconInit;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			/* 温度によるタイミング補正 */
#if 1
			else if( ( wkRadio2nd == vrfMesureCommIntTime - 1U ) 
					&& ( !gvrfStsEventFlag.mTempMeasure )
				&& ( vrfMesureCommIntTime == RT_INT_LONG_MODE_TX_INT )
				&& ( C0ENB == 0U ) )													/* 測定用コンパレータON時は次のタイミング */
			{
				ApiRfDrv_TimingCorrect();					/* 通信周期の長い場合のみ修正 */
				gvrfStsEventFlag.mTempMeasure = 1U;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconInit;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
#endif
			else if( wkHsmodeInterval == 0U )
			{
				/* 高速モード */
				/* Measure送信タイミング以外は高速モード受信待機 */
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
			/* 消費電流を考慮し最小設定 関数を使わない */
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
				/* 送信中にタイムアウト(399.8msec) */
				/* 規制以上の送信時間 */
				R_INTC6_Stop();
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */	
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 2U;
#endif
			}
			else
			{
				/* 送信完了 */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAuthCad;
				SubIntervalTimer_Sleep( 4U + 10U * 375U, 1U );		/* 4msec + 10msec(GWのランダムwait考慮) */
				
				/* 乾電池AD測定 */
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
				/* CAD検知 */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAuthRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else if( gvRfIntFlg.mRadioDio0 )
			{
				vCallChLoopLora++;		/* 次回チャネル変更 */
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 3U;
#endif
			}
			else
			{
				/* 起動タイムアウト */
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 4U;
#endif
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAuthRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_2, 22U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait 設定 */
			SubIntervalTimer_Sleep( 80U * 375U, 2U );		/* 160msec */
			/* 受信待ち中にデータセット */
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAuthRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAuthRxWait:
			/* 受信完了待ち */
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* 受信完了 */
				R_INTC6_Stop();
				SubSX1272Stdby();
				SubIntervalTimer_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAuthRxDataRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 受信タイムアウト */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
//				wkRadio = SubSX1272Read( REG_LR_IRQFLAGS );
				vCallChLoopLora++;			/* 次回チャネル変更 */
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
				/* 事業会社ID確認 */
				if( vutRfDataBuffer.umRT_Gw_ConnAuth.mCompanyID == gvInFlash.mProcess.mOpeCompanyID )
				{
					/* 初期App IDの確認処理 */
					if( (gvInFlash.mParam.mAppID[ 0U ] == APP_ID_LOGGER_INIT_UP) && (gvInFlash.mParam.mAppID[1U] == APP_ID_LOGGER_INIT_LO) )
					{
						wkRadio = SUCCESS;
					}
					/* App ID一致確認 */
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
						/* 通信許可GWの検索 */
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
								/* 通信許可GWリストにあり */
								wkRadio = SUCCESS;
							}
						}
#else
						wkRadio = FAIL;
						vrfRssi = vrfCurrentRssi;
						/* GWの許可通信リスト確認 */
#if 1
						if( (gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 0U ] == 0x00) && (gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 1U ] == 0x00) && (gvInFlash.mParam.mCnctEnaGwId[ 0U ][ 2U ] == 0x00) )
						{
#else
						if( (vrfGwConnectList[0U][0U] == 0x00) && (vrfGwConnectList[0U][1U] == 0x00) && (vrfGwConnectList[0U][2U] == 0x00) )
						{
#endif
							wkRadio = SUCCESS;		/* GWフィルタ設定無し */
						}
						else
						{
							/* 通信許可GWの検索 */
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
									/* 通信許可GWリストにあり */
									wkRadio = SUCCESS;
								}
							}
						}
#endif
					}
				}
				else
				{
					/* 事業会社IDが異なる */
				}
				
				if( wkRadio == SUCCESS )
				{
					/* システムIDが一致 かつ 通信許可GW */
					/*------------GWタイミング同期------------ */
					R_IT_Stop();
					gvRfTimingCounterL = (vutRfDataBuffer.umRT_Gw_ConnAuth.mSyncTimerL[0U] << 8U) & 0xFF00;
					gvRfTimingCounterL += vutRfDataBuffer.umRT_Gw_ConnAuth.mSyncTimerL[1U];
					gvRfTimingCounterH = (vutRfDataBuffer.umRT_Gw_ConnAuth.mSyncTimerH[0U] << 8U) & 0xFF00;
					gvRfTimingCounterH += vutRfDataBuffer.umRT_Gw_ConnAuth.mSyncTimerH[1U];
					/* GWから約181msecの遅延(TX時間144msec/GW⇒Logger処理時間7msec/Beacon受信中にDataSet30msec) */
					/* gvRfTimingCounterLは~0x07FFまで GWのIntTimerは0x000F */
//					gvRfTimingCounterL += 360U;
					gvRfTimingCounterL += 268U;			/* 144msec - 13msec(preamble)のみに変更 */
					vrfITSyncCount = gvRfTimingCounterL;
					gvRfTimingCounterL = ((gvRfTimingCounterL & 0x0F00) >> 8U) & 0x000F;		/* gvRfTimingCounterLは0-7もしくは8まで */
					vrfITSyncCount = (vrfITSyncCount << 4U) & 0x0FF0;			/* vrfITSyncCountは0-0x0FF0 */
					vrfITSyncCount = 0x0FF0 - (vrfITSyncCount & 0x0FF0) + 8U;		/* 最後の桁は0x0と0xFの間 */
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
//					vrfRTmodeTimeSlot = vutRfDataBuffer.umRT_Gw_ConnAuth.mTimeSlot;			/* 接続の最終Ack受信時に移動 */
					
					/* 通信間隔の取得 */
					vrfPreRTmodeCommInt = vrfRTmodeCommInt;									/* 現在のRt待受周期を記憶 */
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
					
					/* リアルタイム通信周期が更新された場合 */
					if( vrfRTmodeCommInt != vrfPreRTmodeCommInt )
					{
						/* 無線通信周期あり → 無し */
						if( vrfRTmodeCommInt == 0xFFFF )
						{
							/* HSのみ待受状態へ変更 */
							M_SETBIT( gvInFlash.mParam.mParmFlg, imRtStbyOffFlg );
							gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;					/* 内蔵Flash書込み更新 */
							gvModuleSts.mLcd = ecLcdModuleSts_Run;							/* LCD表示更新 */
						}
						else
						{
							/* 無線通信周期無し → あり */
							if( vrfPreRTmodeCommInt == 0xFFFF )
							{
								/* HS-Rt待受状態へ変更 */
								M_CLRBIT( gvInFlash.mParam.mParmFlg, imRtStbyOffFlg );
								gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;				/* 内蔵Flash書込み更新 */
								gvModuleSts.mLcd = ecLcdModuleSts_Run;						/* LCD表示更新 */
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
					
					/* GW時刻 */
					wkU32 = vutRfDataBuffer.umRT_Gw_ConnAuth.mOnline_Time[ 0U ];
					wkU32 |= (uint32_t)(vutRfDataBuffer.umRT_Gw_ConnAuth.mOnline_Time[ 1U ]) << 8U;
					wkU32 |= (uint32_t)(vutRfDataBuffer.umRT_Gw_ConnAuth.mOnline_Time[ 2U ]) << 16U;
					wkU32 |= (uint32_t)(vutRfDataBuffer.umRT_Gw_ConnAuth.mOnline_Time[ 3U ] & 0x7FU) << 24U;
#if 1
					ApiTimeComp_first( wkU32, 0U );
#else
/* debug用 */
					gvClock = ApiRtcDrv_localtime( wkU32/2 );
					/* 内蔵RTCに時刻ライト */
					ApiRtcDrv_SetInRtc( gvClock );
					
					/* 内蔵RTCの時刻を外付けRTCに時刻ライト */
					ApiRtcDrv_InRtcToExRtc();
#endif
					/* 計測通信用チャネル取得 */
					vrfLoraChGrupeMes = vutRfDataBuffer.umRT_Gw_ConnAuth.mChGrupe[0U];
					
					/* リアルタイム通信切断判定時間 */
					vrfRTmodeCommCutTime = vutRfDataBuffer.umRT_Gw_ConnAuth.mComOffTime;
					
					/* 長距離通信転送始点データ */
					gvInFlash.mParam.mRealTimeSndPt = (uint16_t)(vutRfDataBuffer.umRT_Gw_ConnAuth.mRecStartTime[ 0U ] << 8U);
					gvInFlash.mParam.mRealTimeSndPt += vutRfDataBuffer.umRT_Gw_ConnAuth.mRecStartTime[ 1U ];
					SubRfDrv_ChgRealTimeSndPt();											/* 送信ポインタ変更 */
					
					/* 警報発生/解除回数を0クリア */
					gvMeasAlmNum = 0U;
					
//					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerInfoDataSet;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
					/* システムIDが不一致 or GWフィルタ */
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
			/* 処理時間8.6msec */
			/* 受信データの退避 */
			for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_2 - 2U; wkRadio++ )
			{
				vutRfDataBuffer.umData[wkRadio + 256U] = vutRfDataBuffer.umData[wkRadio];
			}
			/* ダミーデータセット */
			for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_3 - 2U; wkRadio++ )
			{
				vutRfDataBuffer.umData[wkRadio] = wkRadio;
			}
			
			/* 3:ロガー情報 */
			vutRfDataBuffer.umRT_LoggerInfo.mLoggerID[ 0U ] = gvInFlash.mProcess.mUniqueID[ 0U ];					/* 1:ユニークD */
			vutRfDataBuffer.umRT_LoggerInfo.mLoggerID[ 1U ] = gvInFlash.mProcess.mUniqueID[ 1U ];
			vutRfDataBuffer.umRT_LoggerInfo.mLoggerID[ 2U ] = gvInFlash.mProcess.mUniqueID[ 2U ];
			for( wkLoop = 0U ; wkLoop < imLoggerNameNum ; wkLoop++ )
			{
				vutRfDataBuffer.umRT_LoggerInfo.mLoggerName[ wkLoop ] = gvInFlash.mParam.mLoggerName[ wkLoop ];	/* 2:ロガー名称 */
			}
			vutRfDataBuffer.umRT_LoggerInfo.mGroupID = gvInFlash.mParam.mGroupID;									/* 3:グループID */
			vutRfDataBuffer.umRT_LoggerInfo.mGwID[ 0U ] = (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxAppID_GwUniqueID[ 1U ] << 4U) & 0xF0;	/* 4:GWシリアルID */
			vutRfDataBuffer.umRT_LoggerInfo.mGwID[ 0U ] += (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[ 0U ] >> 4U) & 0x0F;
			vutRfDataBuffer.umRT_LoggerInfo.mGwID[ 1U ] = (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[ 0U ] << 4U) & 0xF0;
			vutRfDataBuffer.umRT_LoggerInfo.mGwID[ 1U ] += (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[ 1U ] >> 4U) & 0x0F;
			vutRfDataBuffer.umRT_LoggerInfo.mGwID[ 2U ] = (vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[ 1U ] << 4U) & 0xF0;
//			vutRfDataBuffer.umRT_LoggerInfo.mLoggerSensorType = (uint8_t)cSensType;								/* 5:センサ種 */
			vutRfDataBuffer.umRT_LoggerInfo.mLoggerSensorType = (uint8_t)gvInFlash.mProcess.mModelCode;			/* 5:センサ種 */
			
			for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
			{
				wkU16Array[ wkLoop ] = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviUU[ wkLoop ] );
				wkU8Array[ wkLoop ] = gvInFlash.mParam.mDelayUU[ wkLoop ];
			}
			ApiRfDrv_SetMeaAlmValArray( &wkU16Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLastUpperLimitAlarm[ 0U ] );	/* 6:上上限値 */
			SubRfDrv_SetAlmDelayArray( &wkU8Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLastUpperLimitDelay[ 0U ] );		/* 7:上上限遅延回数 */
			
			for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
			{
				wkU16Array[ wkLoop ] = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviU[ wkLoop ] );
				wkU8Array[ wkLoop ] = gvInFlash.mParam.mDelayU[ wkLoop ];
			}
			ApiRfDrv_SetMeaAlmValArray( &wkU16Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mUpperLimitAlarm[ 0U ] );		/* 8:上限値 */
			SubRfDrv_SetAlmDelayArray( &wkU8Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mUpperLimitDelay[ 0U ] );			/* 9:上限遅延回数 */
			
			for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
			{
				wkU16Array[ wkLoop ] = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviL[ wkLoop ] );
				wkU8Array[ wkLoop ] = gvInFlash.mParam.mDelayL[ wkLoop ];
			}
			ApiRfDrv_SetMeaAlmValArray( &wkU16Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLowerLimitAlarm[ 0U ] );		/* 10:下限値 */
			SubRfDrv_SetAlmDelayArray( &wkU8Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLowerLimitDelay[ 0U ] );			/* 11:下限遅延回数 */
			
			for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
			{
				wkU16Array[ wkLoop ] = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviLL[ wkLoop ] );
				wkU8Array[ wkLoop ] = gvInFlash.mParam.mDelayLL[ wkLoop ];
			}
			ApiRfDrv_SetMeaAlmValArray( &wkU16Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLastLowerLimitAlarm[ 0U ] );	/* 12:下限値 */
			SubRfDrv_SetAlmDelayArray( &wkU8Array[ 0U ], &vutRfDataBuffer.umRT_LoggerInfo.mLastLowerLimitDelay[ 0U ] );		/* 13:下限遅延回数 */
			
			SubRfDrv_SetDevi( &vutRfDataBuffer.umRT_LoggerInfo.mAlarmDeviationTime[ 0U ] );							/* 14:逸脱許容時間 */
			
			vutRfDataBuffer.umRT_LoggerInfo.mRecInterval = gvInFlash.mParam.mLogCyc1 & 0x0F;						/* 15:収録周期1 */
			vutRfDataBuffer.umRT_LoggerInfo.mRecInterval |= (gvInFlash.mParam.mLogCyc2 & 0x0F) << 4U;				/* 16:収録周期2 */
			
			vutRfDataBuffer.umRT_LoggerInfo.mCalcAlarmKind = gvInFlash.mParam.mAlertType;							/* 17:警報監視演算種 */
			vutRfDataBuffer.umRT_LoggerInfo.mCalcAlarmParameter = gvInFlash.mParam.mAlertParmFlag;					/* 18:警報監視演算パラメータ、フラグ */
			
			vutRfDataBuffer.umRT_LoggerInfo.mMeasOldId[ 0U ] = gvInFlash.mData.mMeas1_PastIndex >> 8U;					/* 19:最古IndexNo.(計測値) */
			vutRfDataBuffer.umRT_LoggerInfo.mMeasOldId[ 1U ] = gvInFlash.mData.mMeas1_PastIndex & 0x00FFU;
			
			for( wkLoop = 0U ; wkLoop < 4U; wkLoop++ )
			{
				wkU32 = gvInFlash.mData.mMeas1_PastTime >> (24U - wkLoop * 8U);
				vutRfDataBuffer.umRT_LoggerInfo.mMeasOldTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);					/* 20:最古時刻(計測値) */
			}
			
			vutRfDataBuffer.umRT_LoggerInfo.mAlHistOldId[ 0U ] = gvInFlash.mData.mMeasAlm_PastIndex >> 8U;				/* 21:最古IndexNo.(計測警報) */
			vutRfDataBuffer.umRT_LoggerInfo.mAlHistOldId[ 1U ] = gvInFlash.mData.mMeasAlm_PastIndex & 0x00FFU;
			
			for( wkLoop = 0U ; wkLoop < 4U; wkLoop++ )
			{
				wkU32 = gvInFlash.mData.mMeasAlm_PastTime >> (24U - wkLoop * 8U);
				vutRfDataBuffer.umRT_LoggerInfo.mAlHistOldTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);				/* 22:最古時刻(計測警報) */
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
				/* 送信中にタイムアウト(399.8msec) */
				/* 規制以上の送信時間 */
				R_INTC6_Stop();
				SubIntervalTimer_Stop();
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 9U;
#endif
			}
			else
			{
				/* 送信完了 */
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAckCad;
				SubIntervalTimer_Sleep( 6U * 375U, 1U );		/* 10msec */
				
				/* 乾電池AD測定 */
				ApiAdc_AdExe();
				
				vLoopLoRa = 0U;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckCad:
			SubSX1272CadLoRa();
			if( gvRfIntFlg.mRadioDio4 )
			{
				/* CAD検知 */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAckRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else if( gvRfIntFlg.mRadioDio0 )
			{
				/* 非検知 */
				if( vLoopLoRa < 2U)
				{
					/* Retry */
					SubSX1272Sleep();		/* フラグクリアのため */
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
				/* 起動タイムアウト */
				SubIntervalTimer_Stop();
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 11U;
#endif
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_4, 6U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait 設定 */
			SubIntervalTimer_Sleep( 100U * 375U, 1U );			// 100msec
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAckRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckRxWait:
			/* 受信完了待ち */
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* 受信完了 */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubSX1272Stdby();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ConnectAckRxRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 受信タイムアウト */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 12U;
#endif
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAckRxRead:
			/* ロガー接続許可通信(Logger CoonectAuth)は256U以降に格納済み */
			SubRFDrv_LoRa_Rx_Read( &wkRadio, &vrfCurrentRssi);
			SubSX1272Sleep();
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS )
			{
				/* CRC OK */
				/* LoggerIDとシーケンスNoのチェック */
				if( memcmp( vutRfDataBuffer.umRT_Gw_ConnAck.mLoggerID, gvInFlash.mProcess.mUniqueID, 3U ) == 0U )
				{
					/* 自機宛の通信 シーケンスNoチェック*/
					if( (vutRfDataBuffer.umRT_Gw_ConnAck.mGwID_Sequence[2U] & 0x0F) == 0x01 )
					{
						/* 2021.7.12 追加 */
						vrfInitCnt = 0U;

						vrfRssi = vrfCurrentRssi;
						/* ロガー接続のACK通信である */
						/* RTmodeでの計測値通信開始 */
						SubRfDrv_SetRtConInfo( imON );
						/* Auth通信で受信済みだが、再登録の場合はこちらが優先(Auth通信を削除してもOK) */
						vrfRTmodeTimeSlot = vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot;
						if( (vRfDrv_CommIntMode == ecRfDrv_ShortIntMode) && (vrfRTmodeTimeSlot > RT_SHORT_MODE_CONNECT_NUM ) )
						{
							/* Short Modeだが接続台数オーバー */
							SubRfDrv_SetRtConInfo( imOFF );
							vrfRTmodeTimeSlot = 0U;
#if (swRssiLogDebug == imEnable)
							vrfErorrCause = 16U;
#endif
						}
						else if( (vRfDrv_CommIntMode == ecRfDrv_MidIntMode) && (vrfRTmodeTimeSlot > RT_MID_MODE_CONNECT_NUM ) )
						{
							/* Mid Modeだが接続台数オーバー */
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
							/* 電波強度確認ジグ */
							//SubModbus_MakeSndData( wkFunCode, wkRefNo, wkNum, wkSetData );
//							vrfDebug[0U] = vrfRssi;
//							vrfDebug[1U] = vrfCommTargetID[0U];
//							vrfDebug[2U] = vrfCommTargetID[1U];
//							vrfDebug[3U] = vrfCommTargetID[2U];
							SubModbus_MakeSndData( imMod_Fnc16, 40011U, 4U, vrfDebug );							/*112byteまで送信可(引数は56まで)*/
							
							gvutComBuff.umModInf.mRsrvFlg = imON;			/* 送信予約フラグON */
							gvModuleSts.mMod = ecModModuleSts_Run;			/* Modbusメインルーチン開始 */
#else
							/* 初期AppIDの場合はロガー接続許可受信時のGW AppIDを登録 */
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
							/* ローカルモードの場合でGWフィルタ設定がない場合はGWを登録 */
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
						/* 自機宛の違う15byte通信 */
						SubRfDrv_Rtmode_CnctEnd();		/* 2022.9.16 */
#if (swRssiLogDebug == imEnable)
						vrfErorrCause = 15U;
#endif
					}
				}
				else
				{
					/* 他機宛の通信 */
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
			
		/* Logger Measure(警報履歴) 送信--------------------------------------- */
		case ecRfDrvMainSts_RTmode_MeasureDataSet:
		case ecRfDrvMainSts_RTmode_MeasAlmDataSet:
			vrfRssi = 0U;
			if(vLoopLoRa == 0U)
			{
# if 0
				/* 遷移前の箇所でsleepに変更 */
				SubIntervalTimer_Sleep( 90U * 375U, 1U );			/* タイミングずれを考慮し送信を遅らせる */
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
			
			/* 過去データ送信数0、又は送信数1以上でFlashから送信データのリード完了のとき */
			if( (vRfDrvMainLoggerSts == ecRfDrvMainSts_RTmode_MeasAlmDataSet && imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_MeasAlm )) ||
				(vRfDrvMainLoggerSts == ecRfDrvMainSts_RTmode_MeasureDataSet && (imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_Meas1 ) || vRtMode_MeasSndKosu == 0U)) )
			{
				if( vRfDrvMainLoggerSts == ecRfDrvMainSts_RTmode_MeasureDataSet )
				{
					SubRfDrv_RTmodeMeasDataSet( MEASURE_DATASET );				/* 定期送信計測データセット */
					/* 計測値の送信 */
				SubRfDrv_Crc_Coding( &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_5 - (9U - vRtMode_MeasSndKosu) * 7U );
					vOldPtrFlg = MEASURE_DATASET;
				}
				else
				{
					SubRfDrv_RTmodeMeasDataSet( MEASALM_DATASET );				/* 警報履歴データセット */
					/* 警報履歴の送信 */
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
				/* Flashリードできないときのエラー処理 */
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
			/* ONLINE消灯 */
			ApiLcd_SegDsp(ecLcdSeg_S2, imOFF);
			ApiLcd_Main();
			
			if( (vutRfDataBuffer.umRT_MeasureTx.mDataCntKind & 0x80) == 0x80 )
			{
				/* 警報履歴送信 */
				SubRFDrv_LoRa_Tx_Setting(RT_MODE_PAYLOAD_LENGTH_10 - (6U - vRtMode_MeasAlmSndKosu) * 11U, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
				/* 2022.9.16 効果なしと判断し元に戻す 2022.9.20 */
//				SubRFDrv_LoRa_Tx_Setting(RT_MODE_PAYLOAD_LENGTH_10 - (6U - vRtMode_MeasAlmSndKosu) * 11U, 16U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
				SubSX1272WriteBuffer( REG_LR_FIFO, &vutRfDataBuffer.umData[0U], RT_MODE_PAYLOAD_LENGTH_10 - (6U - vRtMode_MeasAlmSndKosu) * 11U);
				SubSX1272Tx( RT_MODE_PAYLOAD_LENGTH_10 - (6U - vRtMode_MeasAlmSndKosu) * 11U );
			}
			else
			{
				/* 計測値送信 */
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
			/* 2022.9.16 元に戻す 2022.9.20 */
//			wkRadio += 8;
//			SubIntervalTimer_Sleep( wkRadio * 375, 4U );
#endif
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureBattAd:
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureTxWait;
			
			/* 乾電池AD測定 */
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
			/* ONLINE点灯 */
			ApiLcd_SegDsp(ecLcdSeg_S2, imON);
			ApiLcd_Main();
#if swRfTestLcd == imEnable
			P7_bit.no5 = 0U;
#endif
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* 送信中にタイムアウト(399.8msec) */
				/* 規制以上の送信時間 */
				SubSX1272Sleep();
				R_INTC6_Stop();
				SubIntervalTimer_Stop();
				vrfRTmodeCommFailNum++;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;			/*	強制終了 */
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
#if (swRssiLogDebug == imEnable)
				vrfErorrCause = 19U;
#endif
			}
			else
			{
				/* 送信完了 */
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
					/* 警報履歴 */
					SubIntervalTimer_Sleep(cRF_RTMODE_TX_AUDIT_WAIT[vRtMode_MeasAlmSndKosu].mTime * 375U ,cRF_RTMODE_TX_AUDIT_WAIT[vRtMode_MeasAlmSndKosu].mNum);
				}
				else
				{
					/* 計測値 */
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
				/* CAD検知 */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureAckRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
#if swRfTestLcd == imEnable
				P7_bit.no5 = 0U;
#endif
			}
			else if( gvRfIntFlg.mRadioDio0 )
			{
				/* 非検知 */
				if( vLoopLoRa < 3U)
				{
					/* Retry */
					SubSX1272Sleep();		/* フラグクリアのため */
//					SubSX1272Write( REG_LR_IRQFLAGS, 0xFF );
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureAckCad;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					vLoopLoRa++;
				}
				else
				{
					/* 3 Retry Time out */
					SubSX1272Sleep();
					vCallChLoopLora++;			/* 次回チャネル変更 */
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
				/* 起動タイムアウト */
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
			
			/* RxWait 設定 */
			SubIntervalTimer_Sleep( 85U * 375U, 1U );			// 85msec
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureAckRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureAckRxWait:
			/* 受信完了待ち */
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* 受信完了 */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubSX1272Stdby();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureAckRxRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 受信タイムアウト */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				vCallChLoopLora++;		/* 次回チャネル変更 */
				vrfRTmodeCommFailNum++;
				gvrfStsEventFlag.mRtmodePwmode = PW_H;
				vrfDebugRTmodeRxTimeout++;			/* デバッグ用 */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureAckRxRead:
			/* ロガー接続許可通信(Logger CoonectAuth)は256U以降に格納済み */
			SubRFDrv_LoRa_Rx_Read( &wkRadio, &vrfCurrentRssi);
			SubSX1272Sleep();
			if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, wkRadio) == SUCCESS )
			{
				/* CRC OK */
				/* LoggerIDとシーケンスNoのチェック */
				if( memcmp( vutRfDataBuffer.umRT_MeasureAck.mLoggerID, gvInFlash.mProcess.mUniqueID, 3U ) == 0U )
				{
					/* 自機宛の通信 シーケンスNoチェック*/
//					if( (vutRfDataBuffer.umRT_MeasureAck.mSequenceSettingDisConnect & 0xE0) == 0x40 )
//					{
						vrfRssi = vrfCurrentRssi;
						/* 計測データ通信のACKである */
						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
						vrfRTmodeCommFailNum = 0U;			/* 失敗回数クリア */
						/* タイミングカウンタ反映 */
						R_IT_Stop();
#if 0
						gvRfTimingCounterH =  vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[0U] & 0xF8;
						gvRfTimingCounterH = (gvRfTimingCounterH << 5U) & 0x1F00;
						gvRfTimingCounterH += vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[2U];
#endif
						/* 調査 */
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
						gvRfTimingCounterL += 170U;			/* 78msec + 5msec(実測調整) */
						vrfITSyncCount = gvRfTimingCounterL;
						gvRfTimingCounterL = ((gvRfTimingCounterL & 0x0F00) >> 8U) & 0x000F;		/* gvRfTimingCounterLは0-7もしくは8まで */
						vrfITSyncCount = (vrfITSyncCount << 4U) & 0x0FF0;			/* vrfITSyncCountは0-0x0FF0 */
						vrfITSyncCount = 0x0FF0 - (vrfITSyncCount & 0x0FF0) + 8U;		/* 最後の桁は0x0と0xFの間 */
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
						
						/* 切断フラグ判定 */
						if( vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[0U] & 0x40 )
						{
							SubRfDrv_SetRtConInfo( imOFF );
							vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_BeaconInit;
						}
						/* 設定変更要求判定 */
						if( vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[0U] & 0x80 )
						{
							gvrfStsEventFlag.mSetReserve = 1U;
						}
						
						/* GW時刻(14bit) */
						wkU32 = (vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[ 0U ] & 0x3F) << 8U;
						wkU32 |= vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[ 1U ];
						ApiTimeComp_first( wkU32, 1U );
						
						/* 送信電力変更 */
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
							;/* 他のケースは現状維持 */
						}
						
						if( vOldPtrFlg == MEASURE_DATASET )
						{
							SubRfDrv_RtmodeMeas1OldPtrUpdate();			/* 測定値の過去送信ポインタ更新 */
						}
						else
						{
							/* 計測警報発生/解除回数をデクリメント */
							if( gvMeasAlmNum >= vRtMode_MeasAlmSndKosu )
							{
								gvMeasAlmNum -= vRtMode_MeasAlmSndKosu;
							}
							else
							{
								gvMeasAlmNum = 0U;
							}
						}
						
						gvInFlash.mData.mAlmPast[ 0U ] = ecAlmPastSts_Non;				/* 過去警報フラグクリア */
						gvInFlash.mData.mAlmPast[ 1U ] = ecAlmPastSts_Non;
						gvInFlash.mData.mAlmPast[ 2U ] = ecAlmPastSts_Non;
						gvInFlash.mData.mAlmPast[ 3U ] = ecAlmPastSts_Non;
						gvInFlash.mData.mAlmPast[ 4U ] = ecAlmPastSts_Non;
						
						vrfDebugRTmodeSuccess++;			/* Debug用 計測値送信成功回数 */
						
						gvModuleSts.mRf = ecRfModuleSts_Sleep;
						gvModuleSts.mLcd = ecLcdModuleSts_Run;
//					}
//					else
//					{
//						/* 自機宛の違う15byte通信 */
//						vCallChLoopLora++;		/* 次回チャネル変更 */
//						vrfRTmodeCommFailNum++;
//						vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_MeasureInit;
//						gvModuleSts.mRf = ecRfModuleSts_Sleep;
//						vrfErorrCause = 25U;
//					}
				}
				else
				{
					/* 他機宛の通信 */
					vCallChLoopLora++;		/* 次回チャネル変更 */
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
			/* ランダムwait */
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
				/* 送信中にタイムアウト(100msec) */
				/* 規制以上の送信時間 */
//				SubSX1272Sleep();
//				R_INTC6_Stop();
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;			/*	強制終了 */
//				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			else
			{
				/* 送信完了 */
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
				/* CAD検知 */
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx1Start;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else if( gvRfIntFlg.mRadioDio0 )
			{
				SubSX1272Sleep();
				/* 非検知 */
#if swRTmodeSetting == imEnable
				if( vLoopLoRa < 10U)
#else
				if( vLoopLoRa < 3U)
#endif
				{
					/* Retry */
//					SubSX1272Sleep();		/* フラグクリアのため */
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
				/* 起動タイムアウト */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx1Start:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_7_1, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait 設定 */
			SubIntervalTimer_Sleep( 100U * 375U, 2U );		/* 200msec */
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx1Wait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx1Wait:
			/* 受信完了待ち */
			R_INTC6_Stop();
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* 受信完了 */
//				R_INTC6_Stop();
				SubSX1272Stdby();
				SubIntervalTimer_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx1Read;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 受信タイムアウト */
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
				/* CRC OK and ID一致*/
//				vrfRssi = vrfCurrentRssi;
//				for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_7_1; wkRadio++ )
//				{
//					vutRfDataBuffer.umData[wkRadio + 256U] = vutRfDataBuffer.umData[wkRadio];
//				}
				
				ApiInFlash_RtSetValWrite1( &vutRfDataBuffer.umRT_ChangeSetPrm1 );				/* RAMに設定値書込み */
				
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx2Start;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* CRCエラー or ID不一致 */
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
					/* ID不一致 */
					vCallChLoopLora++;
					vrfInitState = ecRfDrv_Init_Non;
					vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
			}
			else
			{
				/* CRCエラー */
				vCallChLoopLora++;
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
#endif
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx2Start:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_7_2, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait 設定 */
			SubIntervalTimer_Sleep( 100U * 375U, 2U );		/* 200msec */
			
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx2Wait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx2Wait:
			/* 受信完了待ち */
			R_INTC6_Stop();
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* 受信完了 */
//				R_INTC6_Stop();
				SubSX1272Stdby();
				SubIntervalTimer_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx2Read;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 受信タイムアウト */
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
				ApiInFlash_RtSetValWrite2( &vutRfDataBuffer.umRT_ChangeSetPrm2 );				/* RAMに設定値書込み */
				
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx3Start;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* CRCエラー */
				vCallChLoopLora++;
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetRx3Start:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_7_3, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait 設定 */
			SubIntervalTimer_Sleep( 100U * 375U, 2U );		/* 200msec */
			
			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx3Wait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
				case ecRfDrvMainSts_RTmode_ChangeSetRx3Wait:
			/* 受信完了待ち */
			R_INTC6_Stop();
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* 受信完了 */
//				R_INTC6_Stop();
				SubSX1272Stdby();
				SubIntervalTimer_Stop();
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_ChangeSetRx3Read;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 受信タイムアウト */
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
				ApiInFlash_RtSetValWrite3( &vutRfDataBuffer.umRT_ChangeSetPrm3 );				/* RAMに設定値書込み */
				
#if 0 /* THモデル校正時 */
				/* 校正設定値書込み */
				/* GWのIDが00001のみ校正書換え実行 */
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
				/* CRCエラー */
				vCallChLoopLora++;
				vrfInitState = ecRfDrv_Init_Non;
				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ChangeSetAckCarrierSens:
			/* データセット */
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
				/* 送信中にタイムアウト(399.8msec) */
				/* 規制以上の送信時間 */
//				SubSX1272Sleep();
//				R_INTC6_Stop();
//				vrfInitState = ecRfDrv_Init_Non;
//				vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;			/*	強制終了 */
//				gvModuleSts.mRf = ecRfModuleSts_Sleep;
			}
			else
			{
				/* 送信完了 */
				gvrfStsEventFlag.mSetReserve = 0U;				/* 設定変更フラグクリア */
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
		/*************** GW FSKモード ****************/
		case ecRfDrvMainSts_HSmode_BeaconInit:
			/* ユーザー設定 */
//			vrfHSLoggerID[ 0U ] = 0x00;			/* 呼び出しロガーID */
//			vrfHSLoggerID[ 1U ] = 0x00;
//			vrfHSLoggerID[ 2U ] = 0x01;
			
			vrfMySerialID.mGroupID = gvInFlash.mId.mGroupID;
			vrfMySerialID.mAppID[0U] = ((gvInFlash.mId.mAppID[ 0U ] << 4U) & 0xF0) + ((gvInFlash.mId.mAppID[ 1U ] >> 4U) & 0x0F);
			vrfMySerialID.mAppID[1U] = (gvInFlash.mId.mAppID[ 1U ] << 4U) & 0xF0;
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Measure;				/* 計測値要求 */ 
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_AlartHis;				/* 警報履歴要求 */ 
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_ReqFirm;				/* ファームアップ要求 */
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;				/* ロガーステータス(ブロードキャスト) */
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_SetRead;				/* ロガー設定値読み込み */
//			vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_SetWrite;				/* ロガー設定値読み込み */
//			gvInFlash.mParam.mrfTorokuMode = LOGGER_TOROKU_ON;

//			vrfHsReqDataNum = 46000U;									/* 要求データ数 */
			if((gvInFlash.mParam.mrfTorokuMode == LOGGER_TOROKU_ON) && (vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Status) )
			{
				vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Toroku;
			}
			
			rfFreqOffset = RF_FREQ_OFFSET;
			
			/* 定期的な初期化処理 */
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
			/* 処理時間1.4msec */
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
				/* GWユニークID */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mUniqueID[0U];
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mUniqueID[1U];
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mUniqueID[2U];
				vrfHsmodePacketLength++;
				
				/* CRC予約 */
				vrfHsmodePacketLength += 2U;
				
				/* パケット種別コード */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0x00;
				vrfHsmodePacketLength++;
				
				/* 要求データ種別コード */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfHsReqCmdCode;	/* ステータス情報取得 */
				vrfHsmodePacketLength++;
				
				/* Waitカウントダウン値 */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = 20U - wkRadio;
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0xFF;		/* 予約 */
				vrfHsmodePacketLength++;
				
				/* 時刻データ */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)(gvElapseSec & 0x000000FF);			/* GW時刻 */
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((gvElapseSec & 0x0000FF00) >> 8U);
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((gvElapseSec & 0x00FF0000) >> 16U);
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((gvElapseSec & 0x7F000000) >> 24U);
				vrfHsmodePacketLength++;
				
				/* Online情報 */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = gvInFlash.mParam.mOnCertLmt;
				vrfHsmodePacketLength++;
				
				/* 呼び出しロガーID */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfHSLoggerID[ 0U ];
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfHSLoggerID[ 1U ];
				vrfHsmodePacketLength++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfHSLoggerID[ 2U ];
				vrfHsmodePacketLength++;
				
				/* グループID */
//				vutRfDataBuffer.umData[vrfHsmodePacketLength] = vrfMySerialID.mGroupID;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = 0xFF;				/* グループへのブロードキャストは未実装 */
				vrfHsmodePacketLength ++;
				
				/* 要求データ先頭時刻 */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((vrfHSReqHeadMeaTime[ 0U ] >> 8U) & 0x00FF);
				vrfHsmodePacketLength ++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)(vrfHSReqHeadMeaTime[ 0U ] & 0x00FF );
				vrfHsmodePacketLength ++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((vrfHSReqHeadMeaTime[ 1U ] >> 8U) & 0x00FF);
				vrfHsmodePacketLength ++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)(vrfHSReqHeadMeaTime[ 1U ] & 0x00FF );
				vrfHsmodePacketLength ++;
				
				/* 要求データ先頭時系列ID */
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)((vrfHSReqHeadMeaID >> 8U) & 0x00FF) ;
				vrfHsmodePacketLength ++;
				vutRfDataBuffer.umData[vrfHsmodePacketLength] = (uint8_t)(vrfHSReqHeadMeaID & 0x00FF );
				vrfHsmodePacketLength ++;
				
				/* 予約 */
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
				
				/* ロガーSleep種別 */
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
				
				/* 事業会社ID */
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
				vErrorLoopCounter = 0U;							/* 受信タイムアウト時のリトライ回数 */
//				gvrfStsEventFlag.mHsmodeRxRetry = 0U;			/* 受信リトライ用のフラグクリア */
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
				/* 送信タイムアウト */
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
			/* GW HSmode ビーコン応答の受信開始 */
			if( gvrfStsEventFlag.mHsmodeRxEnter )
			{
//				SubIntervalTimer_Sleep( 72U * 375U, 2U );
				/* 2021.11.4 待ち時間の代わりに受信待ちにする */
//				SubIntervalTimer_Sleep( 95UL * 375UL, 1U );	
				SubIntervalTimer_Sleep( 110UL * 375UL, 1U );	
			}
			else
			{
				SubIntervalTimer_Sleep( 47U * 375U, 1U );
			}
			/* breakなし */
		case ecRfDrvMainSts_HSmode_BeaconRxContinue:
		
			switch( vrfHsReqCmdCode )
			{
				case ecRfDrvGw_Hs_ReqCmd_Status:
				case ecRfDrvGw_Hs_ReqCmd_Toroku:
					wkCulc = RF_HSMODE_RES_STATUS_LENGTH;
					break;
				case ecRfDrvGw_Hs_ReqCmd_Measure:
					wkCulc = RF_HSMODE_RES_MEASURE_LENGTH;
					wkReg = RF_HSMODE_RES_MEASURE_LENGTH_HEADER - 8U;		/* BCH除く */
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
			SubSX1272RxFSK( wkCulc );					/* 受信TimeOutから送信開始まで2msec */
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
					/* Loggerステータス応答 */
					case ecRfDrvGw_Hs_ReqCmd_Status:
					case ecRfDrvGw_Hs_ReqCmd_Toroku:
						vrfResInfo = ecRfDrv_Continue;
						if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[0U], wkCulc, BCH_OFF ) == SUCCESS )
						{
							if( (vutRfDataBuffer.umGwQueryBeacon.mGwID[0U] == vrfMySerialID.mUniqueID[0U])
								&&(vutRfDataBuffer.umGwQueryBeacon.mGwID[1U] == vrfMySerialID.mUniqueID[1U])
								&&(vutRfDataBuffer.umGwQueryBeacon.mGwID[2U] == vrfMySerialID.mUniqueID[2U]))
							{
								/* 自機宛のレスポンス受信 */
								if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[0U], wkCulc, BCH_OFF ) == SUCCESS )
								{
//									gvrfStsEventFlag.mHsmodeRxEnter = 1U;							/* 受信開始フラグ */
									
									if( (vutRfDataBuffer.umLoggerResStatus.mPacketCode == 0x01) ||
										(vutRfDataBuffer.umLoggerResStatus.mPacketCode == 0x0C) ||
										(vutRfDataBuffer.umLoggerResStatus.mPacketCode == 0x0D) )
									{
										/* ロガー登録、又はステータス更新 */
										ApiModbus_SndFnc16Ref41101( &vutRfDataBuffer.umLoggerResStatus );
									}
									
									switch( vutRfDataBuffer.umLoggerResStatus.mPacketCode )
									{
										/* ステータス応答受信処理 */
										case 0x01:
											/* 2021.7.20 */
											vrfHsLgDct++;
											/* 2021.9.16 */
											vrfHsLgDct++;
										/* ロガー登録 */
										case 0x0C:
										/* 登録モードだが登録済みの同一AppIDロガー */
										case 0x0D:
										/* パケット種別コードのエラー */
										default:
											vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconRxContinue;
											gvModuleSts.mRf = ecRfModuleSts_Run;
											break;
									}
								}
							}
							else
							{
								/* 他機へのレスポンス */
								vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconRxContinue;
								gvModuleSts.mRf = ecRfModuleSts_Run;
							}
						}
						else
						{
							/* CRCエラー */
							vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconRxContinue;
							gvModuleSts.mRf = ecRfModuleSts_Run;
						}
						break;
						
					/* Logger計測値応答 */
					case ecRfDrvGw_Hs_ReqCmd_Measure:
					/* Logger警報履歴データ応答 */
					case ecRfDrvGw_Hs_ReqCmd_AlartHis:
					/* Logger機器異常履歴データ応答 */
					case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
					/* Loggerイベントデータ応答 */
					case ecRfDrvGw_Hs_ReqCmd_EventHis:
					/* Logger動作履歴データ応答 */
					case ecRfDrvGw_Hs_ReqCmd_OpeHis:
					/* Loggerシステム履歴データ応答 */
					case ecRfDrvGw_Hs_ReqCmd_SysHis:
#if 0
						SubIntervalTimer_Stop();
						SubIntervalTimer_Sleep( 50U * 375U, 1U );				/* Ack応答待ち時間(CRCエラー等で処理が早かった場合) */
//						vrfTestPacketCnt++;										/* 受信パケット総数をカウントする */
#else
#endif
						if( SubRfDrv_Header_Decryption( wkReg ) == ecRfDrv_Success )
						{
//							gvrfStsEventFlag.mHsmodeRxEnter = 1U;			/* 受信開始フラグ */
							vErrorLoopCounter = 0U;								/* リトライカウンタ リセット */
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
						/* 1パケット受信完了 ACK/NACK応答送信へ */
						
						/* 初回受信時はWait追加 */
						SubRfDrv_Hsmode_ResRx_AfterWait();
						
						break;
						
					/* Logger設定値読み込み */
					case ecRfDrvGw_Hs_ReqCmd_SetRead:
//						SubIntervalTimer_Stop();
//						SubIntervalTimer_Sleep( 50U * 375U, 1U );				/* Ack応答待ち時間(CRCエラー等で処理が早かった場合) */
//						vrfTestPacketCnt++;										/* 受信パケット総数をカウントする */
						vrfResInfo = ecRfDrv_Success;
						/* ヘッダー部のCRCチェック(BCH除く) */
						if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[0U], RF_HSMODE_RES_SETREAD_LENGTH_HEADER, BCH_OFF ) != SUCCESS )
						{
							/* CRCエラー Nack応答 */
//							vrfTestHeaderCrcError++;
							vrfResInfo = ecRfDrv_End;
						}
						if( memcmp( vutRfDataBuffer.umGwQueryBeacon.mGwID, vrfMySerialID.mUniqueID, 3U ) != 0 && (vrfResInfo == ecRfDrv_Success) )
						{
							/* 自機宛ではない 無応答 */
							vrfResInfo = ecRfDrv_End;
						}
						if( (SubRFDrv_Hsmode_BcnPktCodeCheck() != SUCCESS) && (vrfResInfo == ecRfDrv_Success) )
						{
							/* パケット種別コードのエラー 無応答 */
							vrfResInfo = ecRfDrv_End;
						}
						if( vrfResInfo == ecRfDrv_Success )
						{
							gvrfStsEventFlag.mHsmodeRxEnter = 1U;			/* 受信開始フラグ */
							vErrorLoopCounter = 0U;								/* リトライカウンタ リセット */
							/* CRCチェック */
							CRCD = 0x0000U;
							for( wkRadio = RF_HSMODE_RES_SETREAD_LENGTH_HEADER; wkRadio < (RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_BCH - 2U); wkRadio++ )
							{
								CRCIN = vutRfDataBuffer.umData[wkRadio];
							}
							if((vutRfDataBuffer.umLoggerResSetting.mCrc2[1U] == (uint8_t)(CRCD) )
									&& (vutRfDataBuffer.umLoggerResSetting.mCrc2[0U] == (uint8_t)(CRCD >> 8U) ) )
							{
								/* エラーなし Ack応答 */
//								vrfTest++;
							}
							else
							{
								/* エラーあり エラー訂正処理 20データごとに処理(端数はループ外で処理) */
								for( wkRadio = 0U; wkRadio < 7U; wkRadio++ )
								{
									vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( &vutRfDataBuffer.umData[ RF_HSMODE_RES_SETREAD_LENGTH_HEADER + ( wkRadio * 20U ) ] 
																				,RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_HEADER - RF_HSMODE_RES_SETREAD_LENGTH_BCH -  ( wkRadio * 10U)
																				,20U );
								}
								vrfErrorCorrectCnt +=  SubRfDrv_Decoding_Bch( &vutRfDataBuffer.umData[ RF_HSMODE_RES_SETREAD_LENGTH_HEADER + ( wkRadio * 20U ) ]
																			, RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_HEADER - RF_HSMODE_RES_SETREAD_LENGTH_BCH -  ( wkRadio * 10U)
																			, RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_HEADER - RF_HSMODE_RES_SETREAD_LENGTH_BCH - ( wkRadio * 20U ) );
								/* CRCチェック */
								CRCD = 0x0000U;
								for( wkRadio = RF_HSMODE_RES_SETREAD_LENGTH_HEADER; wkRadio < (RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_BCH - 2U); wkRadio++ )
								{
									CRCIN = vutRfDataBuffer.umData[wkRadio];
								}
								if((vutRfDataBuffer.umLoggerResSetting.mCrc2[1U] == (uint8_t)(CRCD) )
										&& (vutRfDataBuffer.umLoggerResSetting.mCrc2[0U] == (uint8_t)(CRCD >> 8U) ) )
								{
									/* エラー訂正成功 Ack応答 */
//									vrfTest++;
//									vrfCorrectSetCnt++;
								}else
								{
									/* 通信失敗 パケット破棄 無応答 */
									vrfResInfo = ecRfDrv_End;
//									vrfTestCrcErrorCnt++;
								}
							}
						}
						SubRfDrv_Hsmode_ResRx_AfterWait();
						
						break;
						
					/* 設定変更情報の送信 */
					case ecRfDrvGw_Hs_ReqCmd_SetWrite:
						SubIntervalTimer_Stop();
						SubIntervalTimer_Sleep( 40U * 375U, 1U );				/* Ack応答待ち時間(CRCエラー等で処理が早かった場合) */
//						vrfTestPacketCnt++;										/* 受信パケット総数をカウントする */
						vrfResInfo = ecRfDrv_Success;
						/* ヘッダー部のCRCチェック(BCH除く) */
						if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[0U], RF_HSMODE_RES_SETQUERY_LENGTH, BCH_OFF ) != SUCCESS )
						{
							/* CRCエラー Nack応答 */
//							vrfTestHeaderCrcError++;
							vrfResInfo = ecRfDrv_End;
						}
						if( memcmp( vutRfDataBuffer.umGwQueryBeacon.mGwID, vrfMySerialID.mUniqueID, 3U ) != 0 && (vrfResInfo == ecRfDrv_Success) )
						{
							/* 自機宛ではない 無応答 */
							vrfResInfo = ecRfDrv_End;
						}
						if( (SubRFDrv_Hsmode_BcnPktCodeCheck() != SUCCESS) && (vrfResInfo == ecRfDrv_Success) )
						{
							/* パケット種別コードのエラー 無応答 */
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
						SubIntervalTimer_Sleep( 40U * 375U, 1U );				/* Ack応答待ち時間(CRCエラー等で処理が早かった場合) */
						/* CRCチェック */
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
									/* ファームアップ準備がOKかチェック */
									if( vutRfDataBuffer.umLoggerAckNack.mResponce == 0U )
									{
										vrfResInfo = ecRfDrv_Success;
										/* ファームアップOK */
										
										vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_DataSet;
//										gvModuleSts.mRf = ecRfModuleSts_Run;
										gvModuleSts.mRf = ecRfModuleSts_Sleep;
										vrfPacketSum = 0U;
										vErrorLoopCounter = 0U;
										vrfPacketLimit = gvInFlash.mParam.mFirmPacketNum + 2U;		/* 最初の空パケットと最後のCRC分の2パケットを足す */
									}
									else
									{
										/* ファームアップNG */
									}
								}
								else
								{
									/* 自機宛でない 送信終了 */
								}
							}
							else
							{
								/* 通信相手のGWでない 送信終了 */
							}
						}
						else
						{
							/* CRCエラー */
						}
						break;
						
					default:
						break;
				}
			}
			else
			{
				/* 受信タイムアウト Nack応答 */
//				vrfTestHeaderTimeOut++;
				vrfResInfo = ecRfDrv_Error;
			}
			
			/*
			***Responce 結果処理
			*  END:終了
			*  ERROR:受信リトライ
			*  FAIL:NACK応答
			*  SUCCESS:ACK応答 or Responce送信
			*  CONTINUE:Beacon受信継続
			*/
			SubSX1272Sleep();
			if( vrfResInfo == ecRfDrv_End )
			{
				/* ファームアップ要求に対するAckがNG */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
				gvModuleSts.mRf = ecRfModuleSts_Sleep;				/* 技適制限考慮 */
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
				/* 2021.9.16  成功回数分 呼び出しwait */
				vrfHsLgDct ++;
				SubRFDrv_Hsmode_Logger_BcnAckDataSet();
				gvrfStsEventFlag.mHsmodeGwToLgDataSet = 1U;		/* メインルーチンでのデータセット通知フラグ */
			}
			else if( gvrfStsEventFlag.mHsmodeRxEnter == 0U )
			{
				/* パケット受信開始していない状態 */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* ERRORの場合は無応答処理 タイムアウト以外のエラーは永久ループ防止が必要かも(ERROR続きは考えられないが) */
				SubIntervalTimer_Stop();
				vErrorLoopCounter++;
				if( vErrorLoopCounter < 10U )
				{
					vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_BeaconResRxStart;
					/* 2021.11.4 待ち時間の代わりに受信待ちにする */
					gvModuleSts.mRf = ecRfModuleSts_Run;
//					SubIntervalTimer_Sleep( 15U * 375U, 1U );					/* ロガー側のAck待ちタイムアウト時間に依存 Pre検知のリトライ回数20⇒93msec 25⇒104msec */
//					gvModuleSts.mRf = ecRfModuleSts_Sleep;
				}
				else
				{
					/* ERRORのリトライ回数がオーバー */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
					gvModuleSts.mRf = ecRfModuleSts_Run;
					
					/* 高速通信連続受信完了 */
					gvutComBuff.umModInf.mHiPriorityFlg = imOFF;		/* CPU間Modbus通信の最優先送信フラグ(高速通信収集データ応答)をOFF */
					gvutComBuff.umModInf.mComSts = ecComSts_SndEnd;		/* Modbus送信完了へ遷移 */
					gvModuleSts.mMod = ecModModuleSts_Run;
					
					vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;
				}
			}
			
			if( gvRfIntFlg.mRadioTimer )
			{
				/* 処理が50msec(受信開始から100msec)以上かかった場合は即Ack送信 */
				gvModuleSts.mRf = ecRfModuleSts_Run;
				/* デバッグ */
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
				/* 送信失敗時も受信を継続する */
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
			
		/* 設定変更データ送信 */
		/* ファームHEXデータ送信 */
		case ecRfDrvMainSts_HSmode_Gw_To_Lg_DataSet:
			SubRFDrv_Hsmode_Logger_BcnAckDataSet();				/* 5.4msec */
			vLoopLoRa = 20U;			/* CSリトライ回数セット */
			if( gvrfStsEventFlag.mHsmodeRxEnter == 0U )
			{
				/* 初回 */
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
							/* 前回Ack受信 もしくは最初のパケット */
							vrfPacketSum++;							/* 通信成功　次のパケットをセット */
							SubSX1272TxFSK( RF_HSMODE_FIRM_HEX_LENGTH );
							break;
						case 1U:
							/* リトライ初回 */
							vrfPacketSum--;							/* リトライ時は先にセット済みのパケットを戻す */
							SubIntervalTimer_Sleep( 50U * 375U, 1U );
							if( !gvRfIntFlg.mRadioTimer ){ M_HALT; }
							break;
						default:
							/* リトライ中 */
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
					vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Lg_To_Gw_DataAckRx;		/* すぐACK受信 */
					break;
					
				default:
					SubSX1272TxFSK( RF_HSMODE_RES_SETWRITE_LENGTH );
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
					break;
			}
			gvModuleSts.mRf = ecRfModuleSts_Run;
			break;
			
		case ecRfDrvMainSts_HSmode_Lg_To_Gw_DataAckRx:
			/* Ack Preamble検出 受信開始 */
			SubSX1272RxFSK_On();
			SubSX1272RxFSK_DIO();
//			P7_bit.no5 = 1U;
//			SubIntervalTimer_Sleep( 13U * 375U, 1U );					/* GWのACK送信時間4.9msec 10msecはTimeOut,11msecは受信できる */
			switch( vrfHsReqCmdCode )
			{
				case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
					SubIntervalTimer_Sleep( 65U * 375U, 3U );					/* GWのACK送信時間4.9msec 10msecはTimeOut,11msecは受信できる */
					break;
				case ecRfDrvGw_Hs_ReqCmd_SetWrite:
//					SubIntervalTimer_Sleep( 50U * 375U, 1U );					/* GWのACK送信時間4.9msec 10msecはTimeOut,11msecは受信できる */
					/* 2022.8.10 */
					SubIntervalTimer_Sleep( 80U * 375U, 1U );					/* GWのACK送信時間4.9msec 10msecはTimeOut,11msecは受信できる */
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
				/* CRCチェック */
				if( SubRfDrv_Crc_Decoding_HS( &vrfHsmodeAckBuff.umData[ 0U ], RF_HSMODE_ACK_LENGTH, BCH_OFF ) == SUCCESS )
				{
					wkRadio = SUCCESS;
					if( memcmp( vrfHsmodeAckBuff.umCategories.mGwID, vrfMySerialID.mUniqueID, 3U ) != 0 )
					{
						/* 通信相手のGWでない 送信終了 */
						wkRadio = FAIL;
					}
					
					if( memcmp( vrfHsmodeAckBuff.umCategories.mLoggerID, vrfHSLoggerID, 3U ) != 0 )
					{
						/* 自機宛でない 送信終了 */
						wkRadio = FAIL;
					}
					
					switch (vrfHsReqCmdCode)
					{
						case ecRfDrvGw_Hs_ReqCmd_ReqFirm:
							if( vrfHsmodeAckBuff.umCategories.mPacketCode != 0x14 )
							{
								/* コマンドコード不一致 */
								wkRadio = FAIL;
							}
							break;
						case ecRfDrvGw_Hs_ReqCmd_SetWrite:
							if( vrfHsmodeAckBuff.umCategories.mPacketCode != 0x10 )
							{
								/* コマンドコード不一致 */
								wkRadio = FAIL;
							}
							break;
						default:
							/* パケット違い終了 */
							wkRadio = FAIL;
							break;
					}
					
					if( wkRadio == SUCCESS )
					{
						/* 受信正常 */
						if( vrfHsmodeAckBuff.umCategories.mResponce == 0x01 )
						{
							/* NACK受信 パケット再送 (注意：永遠NACK応答だと永久ループする) */
							vErrorLoopCounter++;
							
							/* ファームCRC演算結果がNACK応答 */
							if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_ReqFirm && vrfPacketSum > vrfPacketLimit )
							{
								/* LinuxへCRC結果(NG)応答 */
								ApiModbus_SndRef40047( 0U );
								vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;
							}
							else
							{
								/* ポインタ変更なし */
								vRfDrvMainGwSts = ecRfDrvMainSts_HSmode_Gw_To_Lg_CS;
							}
						}
						else
						{
							/* ACK受信 */
							vErrorLoopCounter = 0U;				/* リトライ回数カウント変数リセット */
							
							switch( vrfHsReqCmdCode )
							{
								case ecRfDrvGw_Hs_ReqCmd_SetWrite:
									/* 設定書込完了 */
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
										/* LinuxへCRC結果(OK)応答 */
										ApiModbus_SndRef40047( 1U );
										break;
									}
									break;
							}
//							vrfPacketSum++;						/* 次のパケット */
						}
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					else
					{
						/* ID,Code不一致 */
						SubIntervalTimer_Sleep( 100U * 375U, 1U );
					}
				}
				else
				{
					/* ヘッダCRCエラー */
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
				/* 受信タイムアウト */
				if( vErrorLoopCounter > 20U )
				{
					M_NOP;
					/* ファーム受信失敗 */
				}
				else
				{
					//SubIntervalTimer_Sleep( 72U * 375U, 1U );			/* Ack待ち20msec + 符号化処理5.4msec + キャリアセンス0.6*5ch msec */
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
			
			/* 工場出荷時固定値 */
			rfFreqOffset = gvInFlash.mProcess.mRfFreqOffset;
			vrfMySerialID.mOpeCompanyID = gvInFlash.mId.mOpeCompanyID;
			vrfMySerialID.mUniqueID[0U] = gvInFlash.mId.mUniqueID[ 0U ];
			vrfMySerialID.mUniqueID[1U] = gvInFlash.mId.mUniqueID[ 1U ];
			vrfMySerialID.mUniqueID[2U] = gvInFlash.mId.mUniqueID[ 2U ];
			
			/* ******ユーザー設定****** */
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
			vrfLoraChGrupeMes = 12U;			/* Loraのチャネルグループ計測値通信用 0-6,7-25は技適用 33-51ch */
#else
			if(vrfRtChSelect == CH_MANU)
			{
//				vrfLoraChGrupeMes = 0U;			/* Loraのチャネルグループ計測値通信用 0-6,7-25は技適用 33-51ch 12U(38ch)は温度調整ジグ */
				vrfLoraChGrupeMes = gvInFlash.mParam.mrfLoraChGrupeMes;
			}
#endif
//			vrfLoraChGrupeCnt = 24U;			/* 接続通信用 */
//			gvInFlash.mParam.mrfRTmodeCommInt = 10U;				/* 計測値通信間隔 */
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
			/*break;なし*/
			
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
//			SubSX1272Sleep();												/* 測温時必要 */
			
			if( wkRadio > 0U )
			{
				gvrfStsEventFlag.mMuskMultiEvent = 0U;				/* wkRadio == 0Uの連続イベントの防止 */
			}
			if( wkRadio == 0U && !gvrfStsEventFlag.mMuskMultiEvent )
			{
				/* 1min周期で1回のみ RTmode受信開始 */
				gvrfStsEventFlag.mMuskMultiEvent = 1U;			/* 連続イベントの防止 */
				gvrfStsEventFlag.mMuskCadSetting = 0U;			/* Cad初回のみ設定 */
				gvrfStsEventFlag.mTempMeasure = 0U;
				vrfRtmodeCommTimeTotal = 0U;
				gvrfStsEventFlag.mReset = RFIC_INI;
				
				if( wkRtmodeInterval == 0U )
				{
					/* 異なるAppID受信による通信チャネルマスク解除を判定 */
					for( wkCulc = 0U; wkCulc < vrfLoggerSumNumber; wkCulc++ )
					{
						if( vrfLoggerComCnt[wkCulc] > 0U )
						{
							break;
						}
					}
					if( wkCulc == vrfLoggerSumNumber )
					{
						/* 全ロガーの通信成功 or ロガーなし */
						vrfRtMeasChMask = RT_MODE_CH_LOOP_NUM + 1U;		/* 最大値より+1でマスクなし */
					}
					/* 長距離⇒高速 変更時のリスト削除を考慮 */
#if 1
//					if( gvModeSelect == RT_MODE )
#else
					if( gvInFlash.mParam.mModeSelect == RT_MODE )
#endif
//					{
						SubRfDrv_RtConctCut();
//					}
					
					/* 通信失敗カウント処理 */
					if( vrfLoggerSumNumber > 0U )
					{
						for( wkCulc = 0U; wkCulc < vrfLoggerSumNumber; wkCulc++ )
						{
							if( vrfLoggerComCnt[wkCulc] < 0xFF )
							{
								vrfLoggerComCnt[wkCulc]++;			/* 通信失敗カウント+1 */
								
								if( vrfLoggerComCnt[ wkCulc ] == 2U )
								{
									/* 通信失敗送信 */
									ApiModbus_SndRef40025( &vrfLoggerList[ wkCulc ][ 0U ] );
								}
							}
						}
					}
					else if( vrfRtChSelect == CH_AUTO )
					{
						vrfLoraChGrupeMes = rand() % 7U;			/* 自動チャネル選択 */
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
						/* RTmode 接続台数に応じた計測値送信時間 */
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadScan;
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					else if( !gvrfStsEventFlag.mMuskCadSetting )
					{
//						P7_bit.no5 = 0U;
						/* 計測値通信完了後に１回実行 */
						SubRfDrv_RtConctCut();				/* 計測値通信不通カウントアップ処理、切断処理 */
						/* 計測値受信完了後、１回初期化 */
						gvrfStsEventFlag.mMuskCadSetting = 1U;
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					/* 温度によるタイミング補正 */
					else if( ( wkRadio == vrfMesureCommIntTime - 1U ) 
							&& ( !gvrfStsEventFlag.mTempMeasure ) )
					{
						ApiRfDrv_TimingCorrect();					/* 通信周期の長い場合のみ修正 */
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
								/* 常時FSK送信 */
								wkRadio = 1U;
								break;
							case 1U:
								/* 1minごとに25sec送信 */
								if( ( gvRfTimingCounterH > vrfHsmodeRandom + 10UL ) && ( gvRfTimingCounterH < ( vrfHsmodeRandom + 36UL ) ) )		/* 10-60sec から 35-85sec */
								{
									wkRadio = 1U;
								}
									else if( gvRfTimingCounterH > ( vrfHsmodeRandom + 36UL ) )
									{
										gvrfStsEventFlag.mHsmodeRandom = 0U;					/* 計測通信などにより周期が過ぎたケース */
									}
									else if( gvRfTimingCounterH <= (vrfHsmodeRandom + 10UL) )
									{
										if( ((vrfHsmodeRandom + 10UL) - gvRfTimingCounterH) > 60UL )
										{
											gvrfStsEventFlag.mHsmodeRandom = 0U;				/* 計測通信などにより周期が過ぎ、7200secをロールオーバーしたケース */
										}
									}
								break;
							case 2U:
								/* 3minごとに25sec送信 */
								if( ( gvRfTimingCounterH > (vrfHsmodeRandom + 130UL) ) && ( gvRfTimingCounterH < ( vrfHsmodeRandom + 156UL ) ) )	/* 130-180sec から 155-205sec */
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

						/* 2021.7.20 Linux側へ過剰にロガー検知されないように制限 */
						if( vrfHsLgDct > 20U )
						{
							wkRadio = 0U;
						}
						if( gvRfTimingCounterL == 0U )
						{
							/* 2021.9.16 収集後の非検知時間調整のため */
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
					/* 送信終了 */
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
					if( gvrfStsEventFlag.mHsmodeRandom == 0U )
					{
						/* 送信終了1回目 */
						gvrfStsEventFlag.mHsmodeRandom = 1U;
						vrfHsmodeRandom = gvRfTimingCounterH + (rand() % 50U);			/* gvRfTimingCounterHは7200でゼロにリセット */
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
					/* 送信中 */
					gvrfStsEventFlag.mHsmodeRandom = 0U;

					if( !gvrfStsEventFlag.mHsmodeInit )
					{
						/* 初期化していない */
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
					/* ここで電波使用率取得 vrfScanRssiOver/20 [%] */
					ApiModbus_SndRef40019( vrfScanRssiOver / 20 );
					/* 1000回スキャンしたらリセット */
					vrfScanRssiUnder = 0U;
					vrfScanRssiOver = 0U;
				}
				if( !gvrfStsEventFlag.mHsmodeInit )
				{
					/* 初期化していない */
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
				/* CAD検知 */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_BeaconRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else if( gvRfIntFlg.mRadioDio0 )
			{
				/* 非検知 */
				SubSX1272Write( REG_LR_IRQFLAGS, 0xFF );
				vCallChLoopLora++;
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 起動タイムアウト */
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
			/* 受信完了待ち */
			if( !gvRfIntFlg.mRadioDio0 )
			{
				/* 受信タイムアウト */
				SubSX1272Stdby();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 受信完了 */
				R_INTC6_Stop();
				SubSX1272Stdby();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_BeaconRxDataRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_ConnectAuthSetData:
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxCompanyID = vrfMySerialID.mOpeCompanyID;								/* 事業会社ID */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxAppID_GwUniqueID[0U] = vrfMySerialID.mAppID[0U];						/* システムID(アプリID) */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxAppID_GwUniqueID[1U] = vrfMySerialID.mAppID[1U] & 0xF0;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxAppID_GwUniqueID[1U] += (vrfMySerialID.mUniqueID[0U] >> 4U) & 0x0F;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[0U] = (vrfMySerialID.mUniqueID[0U] << 4U) & 0xF0;			/* GWユニークID */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[0U] += (vrfMySerialID.mUniqueID[1U] >> 4U) & 0x0F;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[1U] = (vrfMySerialID.mUniqueID[1U] << 4U) & 0xF0;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxGwUniqueID[1U] += (vrfMySerialID.mUniqueID[2U] >> 4U) & 0x0F;
			
			/* RTmode 通信間隔 */
#if 1
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComInt = 0x04;		/* 初期値 */
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
					/* なし */
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
			/* 演算中のインクリメント対策が必要 */
			TMKAMK = 1U;							/* disable INTIT interrupt */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxSyncTimerL[0U] = (uint8_t)((gvRfTimingCounterL & 0xFF00) >> 8U);	/* 同期タイミングカウンタ0.5msec */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxSyncTimerL[1U] = (uint8_t)(gvRfTimingCounterL);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxSyncTimerH[0U] = (uint8_t)((gvRfTimingCounterH & 0xFF00) >> 8U);	/* 同期タイミングカウンタ1sec */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxSyncTimerH[1U] = (uint8_t)(gvRfTimingCounterH);
			TMKAMK = 0U;							/* enable INTIT interrupt */
#if 0
			/* LoggerListに空きがあるかを検索し、計測通信の順番を決定 */
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
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxTimeSlot = 0U;														/* ここでは通信タイムスロット設定無し(0Uは未接続扱い)削除可 */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxChGrupe[0U] = vrfLoraChGrupeMes;
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxChGrupe[1U] = 0x00;													/* chグループ仕様変更によりダミー化 */
			
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxOnline_Time[ 0U ] = (uint8_t)(gvElapseSec & 0x000000FF);			/* GW時刻 */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxOnline_Time[ 1U ] = (uint8_t)((gvElapseSec & 0x0000FF00) >> 8U);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxOnline_Time[ 2U ] = (uint8_t)((gvElapseSec & 0x00FF0000) >> 16U);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxOnline_Time[ 3U ] = (uint8_t)((gvElapseSec & 0x7F000000) >> 24U);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxOnline_Time[ 3U ] |= gvInFlash.mParam.mOnCertLmt << 7U;				/* オンライン認証限定 */
			
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxComOffTime = gvInFlash.mParam.mDelayTimeDiscon;						/* RT通信切断判定時間 */
			
#if 0
			wkU32 = gvElapseSec / 2U;			/* 分解能1sec(0.5sec切捨て) */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 0U ] = (uint8_t)((wkU32 - 60U) & 0x000000FF);			/* とりあえず1min前 */
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 1U ] = (uint8_t)((wkU32 & 0x0000FF00) >> 8U);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 2U ] = (uint8_t)((wkU32 & 0x00FF0000) >> 16U);
			vutRfDataBuffer.umRT_Gw_ConnAuth.mTxRecStartTime[ 3U ] = (uint8_t)(wkU32 >> 24U);						/* RTモードでの計測データ開始時刻 */
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
			
			/* RSSI値ごとに wait */
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
				/* 接続ビーコンか設定変更ビーコンかの判定 */
				if( vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[1U] & 0x01 )
				{
					/****** 設定変更 ********/
					/* 送信時間制限チェック */
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
						/* 制限時間オーバー */
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
						gvModuleSts.mRf = ecRfModuleSts_Run;
					}
					else
					{
//						vrfRssi = vrfCurrentRssi;
						/* 接続ロガーリストにあるロガーかどうか判定(設定変更ロガーリストの判定があれば不要) */
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
							/* Loggerなし */
							vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
							gvModuleSts.mRf = ecRfModuleSts_Run;
						}else
						{
							/* Loggerあり データ退避 */
							for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_1; wkRadio++ )
							{
								vutRfDataBuffer.umData[wkRadio + 256U] = vutRfDataBuffer.umData[wkRadio];
							}
							/* 設定変更ロガーリストにあるロガーかどうか判定(未実装) */
							/* 設定変更ありの場合 */
							vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingData1Set;
							gvModuleSts.mRf = ecRfModuleSts_Run;
							/* 設定変更なしの場合は無応答 */
						}
						/* ------------------------------------------------------------------------- */
					}
				}else
				{
					/******** 接続ビーコン *********/
					/* システムIDの一致確認が必要 */
					if( vrfMySerialID.mOpeCompanyID == vutRfDataBuffer.umRT_Logger_Beacon.mCompanyID )
					{
						/* 以降事業会社IDが一致 */
						/* 登録モード確認 */
						wkRadio2nd = FAIL;
						if( gvInFlash.mParam.mrfTorokuMode == LOGGER_TOROKU_ON )
						{
							if( ((vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[0U] >> 4U) & 0x0F) == APP_ID_LOGGER_INIT_UP
								&& ( ((vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[0U] << 4U) & 0xF0) + ((vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[1U] >> 4U) & 0x0F) )
								== APP_ID_LOGGER_INIT_LO )
							{
								/* ロガー登録OK (AppID初期値) */
								wkRadio2nd = SUCCESS;
							}
						}
						if( (vrfMySerialID.mAppID[0U] == vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[0U]) &&
							(vrfMySerialID.mAppID[1U] == (vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[1U] & 0xF0)) &&
							(gvInFlash.mParam.mOnCertLmt == ((vutRfDataBuffer.umRT_Logger_Beacon.mAppID_Online[1U] & 0x02) >> 1U)) )
						{
							/* ロガー登録OK (AppID一致 and ローカル/集中管理モード一致) */
							wkRadio2nd = SUCCESS;
						}
						/* ロガー受け入れ判定 */
						if( wkRadio2nd == SUCCESS )
						{
//							vrfRssi = vrfCurrentRssi;
							/* Online認証が一致(もしくはAppID初期値0x0FFF) */
							/* 送信時間制限チェック */
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
								/* 送信時間制限 */
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
							/* ロガー登録拒否 */
							vCallChLoopLora++;
							vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
							gvModuleSts.mRf = ecRfModuleSts_Run;
						}
					}
					else
					{
						/* 事業者の違うシステム */
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
			vrfRtmodeCommTimeTotal += RT_MODE_TX_TIME_2;					/* 156msec送信 */
			
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
				/* 送信中にタイムアウト(399.8msec) */
				/* 規制以上の送信時間 */
				ApiRfDrv_Led( imOFF );
				SubSX1272Sleep();
				R_INTC6_Stop();
				vCallChLoopLora++;
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/*	強制終了 */
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 送信完了 */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerInfoRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerInfoRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_3, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait 設定 */
			SubIntervalTimer_Sleep( 80U * 375U, 5U );		/*400msec*/
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerInfoRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerInfoRxWait:
			/* 受信完了待ち */
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* 受信完了 */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubSX1272Stdby();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerInfoRxRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 受信タイムアウト */
				ApiRfDrv_Led( imOFF );
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/*	強制終了 */
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
					/* 自機宛の通信 */
					/* 受信データの退避 */
					for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_3; wkRadio++ )
					{
						vutRfDataBuffer.umData[wkRadio + 256U] = vutRfDataBuffer.umData[wkRadio];
					}
					
					vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAckDataSet;
					gvModuleSts.mRf = ecRfModuleSts_Run;
				}
				else
				{
					/* 他機宛の通信 */
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
			/* 接続済みLogggerのチェック */
			wkRadio2nd = 0xFF;
			for( wkRadio = 0U; wkRadio < vrfLoggerSumNumber; wkRadio++ )
			{
				if( (vrfLoggerList[wkRadio][0U] == vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[0U]) &&
					(vrfLoggerList[wkRadio][1U] == vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[1U]) &&
					(vrfLoggerList[wkRadio][2U] == vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[2U]) )
				{
					/* リストにロガーあり */
					vrfLoggerComCnt[wkRadio] = 0U;
					vrfLoggerRssi[wkRadio] = vrfRssi;
					break;
				}
				if( (vrfLoggerList[wkRadio][0U] == 0U) && (vrfLoggerList[wkRadio][1U] == 0U) && (vrfLoggerList[wkRadio][2U] == 0U) )
				{
					/* リストに空きあり */
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
				/* Logger新規登録 */
				if( wkRadio2nd == 0xFF )
				{
					/* 登録台数がmaxでないかどうか */
					if( ((vRfDrv_CommIntMode == ecRfDrv_LongIntMode) && (vrfLoggerSumNumber < RT_LONG_MODE_CONNECT_NUM)) ||
						 ((vRfDrv_CommIntMode == ecRfDrv_ShortIntMode) && (vrfLoggerSumNumber < RT_SHORT_MODE_CONNECT_NUM)) ||
						 ((vRfDrv_CommIntMode == ecRfDrv_MidIntMode) && (vrfLoggerSumNumber < RT_MID_MODE_CONNECT_NUM)) )
					{
						vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot = vrfLoggerSumNumber + 1U;		/* リスト空きなし 新規追加 */
					}else
					{
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/* Logger MAX 登録無応答 */
					}
					
				}else
				{
					vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot = wkRadio2nd + 1U;				/* 空きリストの通信順 */
				}
			}
			else
			{
				vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot = wkRadio + 1U;						/* Logger登録済み */
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
			
			/* キャリアセンス時RTC割込み禁止 */
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
				/* 送信中にタイムアウト(399.8msec) */
				/* 規制以上の送信時間 */
				SubSX1272Sleep();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 送信完了 */
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				
				ApiModbus_SndFnc16Ref40201( (ST_RF_RT_LoggerInfo_t *)&vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[ 0U ], vrfRssi );
				
				if( vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot == vrfLoggerSumNumber + 1U )
				{
					/* Logger登録の追加 */
					vrfLoggerList[ vrfLoggerSumNumber][0U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[0U];
					vrfLoggerList[ vrfLoggerSumNumber][1U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[1U];
					vrfLoggerList[ vrfLoggerSumNumber][2U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[2U];
					vrfLoggerComCnt[vrfLoggerSumNumber] = 0x00;
					vrfLoggerRssi[vrfLoggerSumNumber] = vrfRssi;		/* 不要? */
					vrfLoggerSumNumber++;
				}else
				{
					vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot--;
					vrfLoggerList[ vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot][0U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[0U];
					vrfLoggerList[ vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot][1U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[1U];
					vrfLoggerList[ vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot][2U] = vutRfDataBuffer.umRT_LoggerInfo.mBuffLoggerID[2U];
					vrfLoggerComCnt[vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot] = 0x00;
					vrfLoggerRssi[vutRfDataBuffer.umRT_Gw_ConnAck.mTimeSlot] = vrfRssi;			/* 不要? */
				}
				
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		/* GW RTmode 計測値受信----------------------------------------------------------- */
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
						/* CAD検知 */
						M_SETBIT(P3,M_BIT0);
//						SubSX1272RxLoRa_DIO();
//						SubSX1272RxLoRa_Continu();
						vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureRxStart;
						gvModuleSts.mRf = ecRfModuleSts_Run;
						break;
					}
					else if( gvRfIntFlg.mRadioDio0 )
					{
						/* 非検知 */
						SubSX1272Write( REG_LR_IRQFLAGS, 0xFF );
					}
					else
					{
						/* 起動タイムアウト */
						SubSX1272Stdby();
						SubSX1272Write( REG_LR_IRQFLAGS, 0xFF );
						break;
					}
				}
				else
				{
					/* 異なるAppIDを前回受信 */
				}
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_5, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait 設定 */
			SubIntervalTimer_Sleep( 95U * 375U, 4U );		/* 380msec */
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureRxWait:
			/* 受信完了待ち */
			if( SubSX1272Read( REG_LR_RXNBBYTES ) > 0U )
			{
				/* 受信完了 */
				R_INTC6_Stop();
				SubIntervalTimer_Stop();
				SubSX1272Stdby();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureRxRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 受信タイムアウト */
				M_CLRBIT(P3,M_BIT0);
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_MeasureCadSetting;			/*	強制終了 */
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_MeasureRxRead:
			SubRFDrv_LoRa_Rx_Read( &wkRadio, (uint8_t *)&wkCulc);
			SubSX1272Sleep();
			wkRadio = FAIL;
			if( (vutRfDataBuffer.umRT_AuditTx.mDataCntKind & 0x80) == 0x80 )
			{
				/* 長距離通信:計測警報履歴の送信過去データ数：0〜6個 */
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
				/* 長距離通信:計測値の送信過去データ数：0〜9個 */
				if( (vutRfDataBuffer.umRT_MeasureTx.mDataCntKind & 0x0F) <= 9U )
				{
					if( SubRfDrv_Crc_Decoding(vutRfDataBuffer.umData, RT_MODE_PAYLOAD_LENGTH_5_CRC + ((vutRfDataBuffer.umRT_MeasureTx.mDataCntKind & 0x0F) * 7U)) == SUCCESS )
					{
						wkRadio = SUCCESS;
					}
				}
			}
			
			/* ファームアップ(データ転送、又はファームアップ中):受信失敗とし、ロガーの送信Ptを更新させない */
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
				SubModbus_MakeSndData( 0x10, 40011U, wkCulc + 3U, &vrfDebug );				/*112byteまで送信可(引数は56まで)*/
				gvutComBuff.umModInf.mRsrvFlg = imON;			/* 送信予約フラグON */
				gvModuleSts.mMod = ecModModuleSts_Run;			/* Modbusメインルーチン開始 */

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
					/* ロガーがリスト内にあるかをcheck */
					for( wkRadio = 0U; wkRadio < vrfLoggerSumNumber; wkRadio++ )
					{
						if( (vrfLoggerList[wkRadio][0U] == vutRfDataBuffer.umRT_MeasureTx.mLoggerID[0U]) &&
							(vrfLoggerList[wkRadio][1U] == vutRfDataBuffer.umRT_MeasureTx.mLoggerID[1U]) &&
							(vrfLoggerList[wkRadio][2U] == vutRfDataBuffer.umRT_MeasureTx.mLoggerID[2U]) )
						{
							vrfRssi = vrfCurrentRssi;						/* SubSX1272CadLoRa()で取得したRSSI */
							/* リストにロガーあり */
							/* 受信データの退避 */
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
							/* 送信電力変更 */
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
								/* 他のケース */
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
					/* 他機宛の通信 */
					vrfRtMeasChMask = vCallChLoopLora;			/* Chをマスク */
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
			
			/* ---ここでLoggerIDごとの処理を判断する---------------------------------  */
			/* 通信成功のカウント処理 */
			for( wkRadio = 0U; wkRadio < vrfLoggerSumNumber; wkRadio++ )
			{
				if( (vrfLoggerList[wkRadio][0U] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[0U]) &&
					(vrfLoggerList[wkRadio][1U] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[1U]) &&
					(vrfLoggerList[wkRadio][2U] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[2U]) )
				{
					vrfLoggerComCnt[wkRadio] = 0U;			/* 登録済みLogger通信成功 クリア処理 */
					vrfLoggerRssi[wkRadio] = vrfRssi;
					break;
				}
			}
			if( wkRadio > 0U && wkRadio == vrfLoggerSumNumber )
			{
				wkRadio--;
			}
			
			/* ロガー校正設定値書込み確認 */
			vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[ 0U ] = 0x00;
#if (swKouteiCalibration == imEnable)
			if( (gvCalVal.mLoggerId[ 0U ] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[ 0U ]) &&
				(gvCalVal.mLoggerId[ 1U ] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[ 1U ]) &&
				(gvCalVal.mLoggerId[ 2U ] == vutRfDataBuffer.umRT_MeasureAck.mLoggerID[ 2U ]) )
			{
				/* ロガー校正設定あり */
				vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[ 0U ] |= 0x80;
			}
#endif
			/* Test切断フラグ */
//			vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[0U] += 0x40;
			/* Test 設定変更フラグON */
//			vutRfDataBuffer.umRT_MeasureAck.mSettingDisConnectTime[0U] += 0x80;
			
			/* LoggerID検索し、成功時に通信失敗カウントをクリアする */
			
/******************************* ACK送信後に移動できればベスト *******************************************/
			if( (vutRfDataBuffer.umRT_AuditTx.mBuffDataCntKind & 0x80) == 0x80 )
			{
				/* 警報履歴を受信 */
				wkErrCode = ApiModbus_SndFnc16Ref40401( (ST_RF_RT_Audit_Tx_t *)&vutRfDataBuffer.umRT_AuditTx.mBuffCompanyID, vrfLoggerRssi[ wkRadio ] );
			}
			else
			{
				/* 計測値を受信 */
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
			/* タイミングカウンタH 8-13bit */
			vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[0U] = (uint8_t)(0x00F8 & (gvRfTimingCounterH >> 5U));
			vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[0U] += (uint8_t)(0x0007 & (gvRfTimingCounterL >> 8U));
			vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[1U] = (uint8_t)(gvRfTimingCounterL);
			vutRfDataBuffer.umRT_MeasureAck.mSyncTimer[2U] = (uint8_t)(gvRfTimingCounterH);
			
			/* 時刻データ(下位14bit, 1cnt/0.5sec) */
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
				/* 送信中にタイムアウト(399.8msec) */
				/* 規制以上の送信時間 */
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
				/* 送信完了 */
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
			
		/* GW RTmode 設定変更 */
		case ecRfDrvMainSts_RTmode_LoggerSettingData1Set:
			/* ダミーデータ書き込み */
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
				gvCalVal.mAck = 2U;				/* 校正設定完了待ち */
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
			/*送信中にデータセット*/
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingData2Set;
//			gvModuleSts.mRf = ecRfModuleSts_Run;
//			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingData2Set:
			/* ダミーデータ書き込み */
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
				/* 送信中にタイムアウト(200msec) */
				/* 規制以上の送信時間 */
				ApiRfDrv_Led( imOFF );
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 送信完了 */
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				SubIntervalTimer_Sleep( 2U * 375U, 1U );			/* 休止時間2msec */
				
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
			/* ダミーデータ書き込み */
//			for( wkRadio = 0U; wkRadio < RT_MODE_PAYLOAD_LENGTH_7_3 - 2U; wkRadio++ )
//			{
//				vutRfDataBuffer.umData[wkRadio] = wkRadio;
//			}
			
#if (swKouteiCalibration == imEnable)
			/* ロガー校正設定値の有無確認 */
			/* 設定完了待ちあり */
			if( gvCalVal.mAck == 2U )
			{
				/* 校正値を送信パケットに記載 */
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
				/* 送信中にタイムアウト(200msec) */
				/* 規制以上の送信時間 */
				ApiRfDrv_Led( imOFF );
				SubSX1272Sleep();
				R_INTC6_Stop();
				vCallChLoopLora++;
//				vrfTxLimitTime = 5U;				/* 送信時間制限 4sec-5sec */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/*	強制終了 */
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 送信完了 */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubIntervalTimer_Sleep( 2U * 375U, 1U );			/* 休止時間2msec */
//				vrfTxLimitTime = 5U;				/* 送信時間制限 4sec-5sec */
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
				/* 送信中にタイムアウト(200msec) */
				/* 規制以上の送信時間 */
				ApiRfDrv_Led( imOFF );
				SubSX1272Sleep();
				R_INTC6_Stop();
				vCallChLoopLora++;
//				vrfTxLimitTime = 5U;				/* 送信時間制限 4sec-5sec */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/*	強制終了 */
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 送信完了 */
				SubSX1272Sleep();
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
//				vrfTxLimitTime = 5U;				/* 送信時間制限 4sec-5sec */
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingAckRxStart;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingAckRxStart:
			SubRFDrv_LoRa_Rx_Start( RT_MODE_PAYLOAD_LENGTH_8, 12U, RFLR_MODEMCONFIG1_CODINGRATE_4_7);
			
			/* RxWait 設定 */
			SubIntervalTimer_Sleep( 100U * 375U, 5U );		/*100msec*/
			vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingAckRxWait;
			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
			
		case ecRfDrvMainSts_RTmode_LoggerSettingAckRxWait:
			/* 受信完了待ち */
			if( gvRfIntFlg.mRadioDio0 )
			{
				/* 受信完了 */
				SubIntervalTimer_Stop();
				R_INTC6_Stop();
				SubSX1272Stdby();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_LoggerSettingAckRxRead;
				gvModuleSts.mRf = ecRfModuleSts_Run;
			}
			else
			{
				/* 受信タイムアウト */
				ApiRfDrv_Led( imOFF );
				SubIntervalTimer_Stop();
				SubSX1272Sleep();
				R_INTC6_Stop();
				vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_ConnectAuthCadSetting;			/*	強制終了 */
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
					/* 自機宛の通信 */
					if( vutRfDataBuffer.umRT_ChangeSetAck.mSequenceNo == 0x03 )
					{
						/* 設定変更ACKである */
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
							/* 設定変更したロガーのチェックOK */
#if (swKouteiCalibration == imEnable)
							/* 設定変更OK 予約リストからロガーを削除 */
							if( (gvCalVal.mLoggerId[ 0U ] == vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[ 0U ]) &&
								(gvCalVal.mLoggerId[ 1U ] == vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[ 1U ]) &&
								(gvCalVal.mLoggerId[ 2U ] == vutRfDataBuffer.umRT_ChangeSetAck.mLoggerID[ 2U ]) )
							{
								/* 設定完了待ちあり */
								if( gvCalVal.mAck == 2U )
								{
									gvCalVal.mAck = 1U;				/* 校正設定完了 */
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
							/* 設定変更したロガーが違う?? */
						}
					}
					else
					{
						/* シーケンスErr */
					}
				}
				else
				{
					/* 他機宛の通信 */
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


/* ここから関数 */
/*
********************************************************************************
* インターバルタイマによるsleep処理
* [内容]
*	  引数をインターバルタイマの割り込みカウンタにセットしsleep、カウント後起動
*	  他要因での割り込みでの起動もあり(割り込みマスクなし)
*	  20回ループ毎にウォッチドッグタイマをリセット
*	カウントソース375kHz
* [引数]
*	  wkTimerCount	インターバルタイマ割り込みカウンタ
*	  wkLoopCount  繰り返し回数
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
		gvRfIntFlg.mRadioStsMsk = 1U;		/* ITによるecRfModuleSts_Runをマスク */
	}
#endif
}


void SubIntervalTimer_Stop( void )
{
//	RTCMK = 0U;   /* enable INTRTC interrupt */
	R_TAU0_Channel6_Stop();
	gvRfIntFlg.mRadioTimer = 0U;
	gvRfIntFlg.mRadioStsMsk = 0U;		/* ITによるecRfModuleSts_Runマスク解除 */
}


/*
********************************************************************************
* SX1272 POR処理
*
*[内容]
*	  SX1272のPOR処理を行う
********************************************************************************
*/
void SubSX1272POR( void )
{
	RADIO_NSS = imHigh;
	/* Reset pin 入力モード */
	RADIO_RESET_IO = imIoIn;
	/* RFIO4,RFIO2,RFIO6,RFIO1 入力モード */
	RADIO_DIO_2_IO = imIoIn;
	RADIO_DIO_1_IO = imIoIn;
	RADIO_DIO_4_IO = imIoIn;
	RADIO_DIO_0_IO = imIoIn;
}

/*
 *******************************************************************************
 *	SX1272リセット処理
 *
 *	[内容]
 *		SX1272のリセットを行う。
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
		
		/* NSS pin 1 → H */
		
//		RADIO_NSS = imHigh;
	}
	else
	{
		/* Configure RESET as input 推奨HiZ */
		/* RESET Pin Input */
		RADIO_RESET_IO = imIoIn;
//		RADIO_RESET = imLow;
	}
}


/*
 *******************************************************************************
 *	SX1272 FSKモード初期化
 *
 *	[内容]
 *		SX1272のFSKモードでレジスタ初期化を行う
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
*  ロガー処理　RTmode接続断時の再接続ビーコンのタイミング調整関数
*  【概要】
*	GWのロガー接続待ちタイミングに切断直後のビーコンが来るように調整
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
		// 補正なしでOK
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
*  GW処理　RTmode接続状態の判断処理とロガーリスト更新
*  【概要】
*	計測値通信不通のカウントアップ処理と連続失敗回数から切断判断，ロガーリストの更新
*	20個を超える場合は、20個分割で格納
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
//			vrfLoggerComCnt[wkCulc]++;							/* 関数を計測通信後に移動し、カウント増加はIdele先頭に残す */
			/* ロガー切断判定 */
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
					
					/* 切断されたロガーリストをModbus送信リストに追加 */
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
						gvCalVal.mAck = 3U;				/* 校正失敗 */
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
					/* 接続台数が末尾のロガーの場合は総数-1 */
					vrfLoggerSumNumber--;
					vrfLoggerComCnt[wkCulc] = 0x00;
				}
			}
		}while(wkCulc > 0U);
		
		/* 切断されたロガーリストをModbus送信リストに追加 */
		if( wkSize != 0U )
		{
			ApiModbus_SndFnc16Ref40301( 0U, wkSize, wkDelList );
		}
	}
}
#endif

/*
*******************************************************************************
*	FSKモードでのキャリアセンス
*
*	[内容]
*		FSKモード/チャネル設定済み/DIO04_11,MAP_RSSI/SLEEP or STANDBYが前提
*　　設定時間キャリアセンスを行う。
*		割り込みはマスクすること(マスク状態のまま抜ける)
*		Sleep状態で抜ける
* [引数]
*	　インターバルタイマのカウント値(キャリアセンス時間)
* [Return]
*	NO_CARRIER：キャリアなし
*	SENS_CARRIER : キャリアあり
*******************************************************************************
*/
uint8_t SubRFDrv_CarrierSens_FSK( uint16_t arSensTime )
{
	uint8_t wkRet;
	uint8_t wkLoop;
	
	gvRfIntFlg.mRadioDio4 = 0U;
	// RSSI set time 16usec / RX wake up 63usec / PLL 60usec / Standby 250usec
	// WakeUp合計 381usec + 125usec => 16U (519usec)
	// 実測140usec(RSSI set time 16usec / RX wake up 63usec / PLL 60usec)
	R_INTC1_Start();
	SubSX1272RxFSK_On();
	vrfCurrentRssi = SubRfDrv_GetRssi();			/* 捨てRSSI値 */
	wkLoop = 30U;				/* だいたい6〜7ループする */
	while( !(SubSX1272Read(REG_IRQFLAGS1) & 0x80) & wkLoop > 0U)
	{
		wkLoop--;
	}
#if 0
	if( !gvRfIntFlg.mRadioDio4 )
	{
		R_IT_Create_Custom( arSensTime );		/* RSSI開始からSTOP解除まで実測150usec */
//	R_IT_Start_Custom();				/* R_IT_Create_Custom関数内でstartしている */
		M_STOP;
	}
	
	SubSX1272Sleep();
	R_INTC1_Stop();
	R_IT_Stop();
#else
	if( !gvRfIntFlg.mRadioDio4 )
	{
		SubIntervalTimer_Sleep( arSensTime, 1U );		/* RSSI開始からSTOP解除まで実測150usec */
		/* M_STOPの代わり */
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
	
	/* 2021.7.12 vrfRfErrを追加 */
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
		/* 動作エラー */
		wkRet = SENS_CARRIER;
		vrfRfErr++;
		gvrfStsEventFlag.mReset = RFIC_INI;
		gvrfStsEventFlag.mHsmodeInit = 0U;
	}
	
	return wkRet;
}

/*
*******************************************************************************
*	FSKモード 5ch幅でのキャリアセンス
*
*	[内容]
*		FSKモード/DIO04_11,MAP_RSSI/SLEEP or STANDBYが前提
*		チャンネル100kHz幅を5回スキャンする(計500kHz)
*　　　　一定時間キャリアセンスを行う。(計2.2msec)
*		割り込みはマスクすること(マスク状態のまま抜ける)
*		Sleep状態で抜ける
* [引数]
*	　チャンネル情報
* [Return]
*	NO_CARRIER：キャリアなし
*	SENS_CARRIER : キャリアあり
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
*  キャリアセンス後のリトライ判定とチャネル変更
*  [概要]
*  キャリアセンスの結果からリトライ回数プラス、wait時間設定、
*  次回キャリアセンスのチャネル変更判断をする(END時は必ずチャネル変更)
*  vLoopLoRaは事前に0にセットする
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
*	パケットにCRC,BCHを付加
*		[内容]
*		データの4,5byte目にCRC追加
*		[引数]
*		wkLength：CRC、BCHを除くデータ長。11の倍数となる
*		wkLength：BCHなしの場合、全データ長
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

	/* CRCを含めたデータ長のチェック */
	if( !(arLength % 11U) || (arBchOption == BCH_OFF) )
	{
		/* CRC算出 */
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
		
		/* データの4,5byte目にCRC追加 */
		*(arDataIn + 3U) = (uint8_t)( (wkResult >> 8U) & 0x00FF );
		*(arDataIn + 4U) = (uint8_t) wkResult;
#if 0
		if( arBchOption == BCH_ON )
		{
			/*BCH符号化*/
			wkDataBlockNum = arLength / 11U;
			for( wkDataBlockCount = 0U; wkDataBlockCount < wkDataBlockNum; wkDataBlockCount++ )
			{
				SubRfDrv_PreBCH( arDataIn + (wkDataBlockCount * 11U), &wkBchWork );
				SubRfDrv_MakeBCH( &wkBchWork );
				SubRfDrv_Interleave( &wkBchWork, arDataIn + (arLength + (wkDataBlockCount * 4U)) );		/* BCH符号の位置を指定 */
			}
		}
#endif
		wkReturn = SUCCESS;
	}
	else
	{
		/* データ長が11の倍数でない */
		wkReturn = ERROR;
	}
	
	return( wkReturn );
}

#if 0
/*
********************************************************************************
*	No22の計測値パケットにCRC,BCHを付加
*		[内容]
*		HSmode計測値のパケットにCRC,BCHを付加。9byteデータ,2byte CRC,4byte BCH
*		[引数]
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
	
	for( wkDataCount = 0U; wkDataCount < 5U; wkDataCount++ )	/* 1ループで2byte処理する */
	{
		/* BCHはmBchの上位ビットから埋める */
		*( arDataIn + 10U + wkDataCount ) = RF_HSMODE_BCH[ *(arDataIn + wkDataCount * 2U) ] << 4U;
		*( arDataIn + 10U + wkDataCount ) &= 0xF0;
		*( arDataIn + 10U + wkDataCount ) += RF_HSMODE_BCH[ *(arDataIn + wkDataCount * 2U + 1U) ];
	}
	
#if 0
	/* CRC算出 */
	CRCD = 0x0000U;
	for( wkDataCount = 0U; wkDataCount < 9U; wkDataCount++ )
	{
		wkCrc = *(arDataIn + wkDataCount);
		CRCIN = wkCrc;
	}
	wkResult = CRCD;
	
	/* データの11,12byte目にCRC追加 */
	*(arDataIn + 10U) = (uint8_t)( (wkResult >> 8U) & 0x00FF );
	*(arDataIn + 11U) = (uint8_t) wkResult;
	/* 項10-18まで12byteBCH付加 */
	SubRfDrv_MakeBCH( arDataIn, arDataIn + 12U, 12U );
	/*BCH符号化*/
//	SubRfDrv_PreBCH( arDataIn , &wkBchWork );
//	SubRfDrv_MakeBCH( &wkBchWork );
//	SubRfDrv_Interleave( &wkBchWork, arDataIn );		/* BCH符号の位置を指定 */
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
*	データにBCHを付加
*		[内容]
*		2byte単位のデータに1byteのBCH符号を付加する
*		[引数]
*		arData：符号化するデータの先頭ポインタ
*		arBchOffset：BCH符号を格納する先頭データ位置 (10ならarDataの位置 +10~+14までBCH)
*		arDataNumber：符号化するデータ数　※偶数であること
********************************************************************************
*/
void SubRfDrv_Coding_Bch(uint8_t *arDataIn, uint16_t arBchOffset, uint16_t arDataNumber)
{
	uint16_t		wkDataCount;
//	uint16_t	wkResult;
//	uint8_t		wkCrc;
	
	for( wkDataCount = 0U; wkDataCount < (arDataNumber / 2U); wkDataCount++ )	/* 1ループで2byte処理する */
	{
		/* BCHはmBchの上位ビットから埋める */
		*( arDataIn + arBchOffset + wkDataCount ) = RF_HSMODE_BCH[ *(arDataIn + wkDataCount * 2U) ] << 4U;
		*( arDataIn + arBchOffset + wkDataCount ) &= 0xF0;
		*( arDataIn + arBchOffset + wkDataCount ) += RF_HSMODE_BCH[ *(arDataIn + wkDataCount * 2U + 1U) ];
	}
}

#if 0
/*
********************************************************************************
*	受信したNo22の計測値パケットをBCH復号
*		[内容]
*		HSmode計測値のパケットを復号。10byteデータ,5byte BCH
*		[Return]
*		エラー訂正数
********************************************************************************
*/
uint8_t SubRfDrv_Decoding_Measure(uint8_t *arCulcData)
{
	uint16_t wkDataOut;
	uint16_t wkDataIn;
	uint8_t wkShiftReg = 0x00; 		/* シフトレジスタ値			*/
	uint8_t wkFFout = 0x00;			/* フリップフロップ出力		*/
	uint8_t wkLoop;
	uint8_t wkDataCnt;
//	uint8_t wkCulc;
	uint8_t wkBch[10U];
	uint8_t wkReturn = 0U;
//	uint16_t	wkResult;
	
	/* BCH符号を4bitごとに展開 */
	for( wkDataCnt = 0U; wkDataCnt < 5U; wkDataCnt++ )
	{
		/* 場所はvutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[].mMeasure[].mBch */
		wkBch[wkDataCnt * 2U] = (*(arCulcData + 10U + wkDataCnt) >> 4U) & 0x0F;
		wkBch[wkDataCnt * 2U + 1U] = *(arCulcData + 10U + wkDataCnt) & 0x0F;
	}
	/* BCH復号 10データ */
	for( wkDataCnt = 0U; wkDataCnt < 10U; wkDataCnt++ )
	{
		wkDataIn = ((uint16_t)(*arCulcData) << 4U) & 0x0FF0;
		wkDataIn += wkBch[wkDataCnt];
		wkDataOut = wkDataIn;
		wkFFout = 0U;
		wkShiftReg = 0U;
		
		for( wkLoop = 0U; wkLoop < 15U; wkLoop++ )
		{
			wkFFout = (wkFFout & 0xEF) | ( ((wkShiftReg & 0x08) << 1U) ^ (uint8_t)( (wkDataIn & 0x4000) >> 10U) ); 	/* X4 = R4 xor D11	※上位1bitは無視	*/
			wkFFout = (wkFFout & 0xF7) | ( (wkShiftReg & 0x04) << 1U); 									/* X3 = R3								*/
			wkFFout = (wkFFout & 0xFB) | ( (wkShiftReg & 0x02) << 1U); 									/* X2 = R2								*/
			wkFFout = (wkFFout & 0xFD) | ( ( ((wkShiftReg & 0x01) << 1U) ^ ( (wkFFout & 0x10) >> 3U) ));		/* X1 = X4 xor R1						*/
			wkFFout = (wkFFout & 0xFE) | ( (wkFFout & 0x10) >> 4U);										/* X0 = X4								*/
			wkShiftReg = wkFFout & 0x0F;
			wkDataIn = wkDataIn << 1U;
		}
		
		if(!(wkShiftReg == 0U) )
		{
			/* 上位3bitD14-D12は0固定、BCH符号部D0-D3は無視 */
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
*	BCH復号処理
*		[内容]
*		BCH符号付きデータを復号。
*		[引数]
*		arCulcData：データの先頭アドレス
*		arOffset：BCH符号の先頭アドレス arCulcData + arOfset
*		arDataNumber：復号するデータ数　※偶数であること　※最大22データまで
*		[Return]
*		エラー訂正数
********************************************************************************
*/
uint8_t SubRfDrv_Decoding_Bch(uint8_t *arCulcData, uint16_t arOffset, uint8_t arDataNumber)
{
	uint16_t wkDataOut;
	uint16_t wkDataIn;
	uint8_t wkShiftReg = 0x00; 		/* シフトレジスタ値			*/
	uint8_t wkFFout = 0x00;			/* フリップフロップ出力		*/
	uint8_t wkLoop;
	uint8_t wkDataCnt;
//	uint8_t wkCulc;
	uint8_t wkBch[22U];
	uint8_t wkReturn = 0U;
//	uint16_t	wkResult;
	
	/* BCH符号を4bitごとに展開 */
	for( wkDataCnt = 0U; wkDataCnt < (arDataNumber / 2U); wkDataCnt++ )
	{
		/* 場所はvutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[].mMeasure[].mBch */
		wkBch[wkDataCnt * 2U] = (*(arCulcData + arOffset + wkDataCnt) >> 4U) & 0x0F;
		wkBch[wkDataCnt * 2U + 1U] = *(arCulcData + arOffset + wkDataCnt) & 0x0F;
	}
	/* BCH復号 10データ */
	for( wkDataCnt = 0U; wkDataCnt < arDataNumber; wkDataCnt++ )
	{
		wkDataIn = ((uint16_t)(*arCulcData) << 4U) & 0x0FF0;
		wkDataIn += wkBch[wkDataCnt];
		wkDataOut = wkDataIn;
		wkFFout = 0U;
		wkShiftReg = 0U;
		
		for( wkLoop = 0U; wkLoop < 15U; wkLoop++ )
		{
			wkFFout = (wkFFout & 0xEF) | ( ((wkShiftReg & 0x08) << 1U) ^ (uint8_t)( (wkDataIn & 0x4000) >> 10U) ); 	/* X4 = R4 xor D11	※上位1bitは無視	*/
			wkFFout = (wkFFout & 0xF7) | ( (wkShiftReg & 0x04) << 1U); 									/* X3 = R3								*/
			wkFFout = (wkFFout & 0xFB) | ( (wkShiftReg & 0x02) << 1U); 									/* X2 = R2								*/
			wkFFout = (wkFFout & 0xFD) | ( ( ((wkShiftReg & 0x01) << 1U) ^ ( (wkFFout & 0x10) >> 3U) ));		/* X1 = X4 xor R1						*/
			wkFFout = (wkFFout & 0xFE) | ( (wkFFout & 0x10) >> 4U);										/* X0 = X4								*/
			wkShiftReg = wkFFout & 0x0F;
			wkDataIn = wkDataIn << 1U;
		}
		
		if(!(wkShiftReg == 0U) )
		{
			/* 上位3bitD14-D12は0固定、BCH符号部D0-D3は無視 */
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
*	パケットにCRCを付加
*		[内容]
*		RTモードのパケットにCRCを付加
*		[引数]
*		wkLength：CRCを除くデータ長。(ecRfDrvMainSts_RTmode_MeasureDataSetの場合除く)
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
	
	
	/* CRC算出 */
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
			
			/* RTmode計測値送信時はデータ長可変 */
			for( wkDataCount = 0U; wkDataCount < wkLength; wkDataCount++ )
			{
				/* CRCは31,32byte目 (30U,31U) */
				if( wkDataCount == wkCrcLen - 2U )
				{
					if( wkLength == (wkCrcLen) )
					{
						/* 計測値長ゼロの場合 */
						break;
					}
					wkDataCount += 2U;
				}
				wkCrc = *(wkDataIn + wkDataCount);
				CRCIN = wkCrc;
			}
			wkResult = CRCD;
			
			/* 計測値手前にCRC追加 */
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
			
			/* データの後ろ(BCH手前)にCRC追加 */
			*(wkDataIn + wkLength) = (uint8_t)( (wkResult >> 8U) & 0x00FF );
			*(wkDataIn + wkLength + 1U) = (uint8_t) wkResult;
			break;
	}
#endif
#if (swLoggerBord == imDisable)
	/* CRC算出 */
	CRCD = 0x0000U;
	for( wkDataCount = 0U; wkDataCount < wkLength; wkDataCount++ )
	{
		wkCrc = *(wkDataIn + wkDataCount);
		CRCIN = wkCrc;
	}
	wkResult = CRCD;
	
	/* データの後ろ(BCH手前)にCRC追加 */
	*(wkDataIn + wkLength) = (uint8_t)( (wkResult >> 8U) & 0x00FF );
	*(wkDataIn + wkLength + 1U) = (uint8_t) wkResult;
#endif
}


/*
********************************************************************************
*	パケットのCRCチェック
*		[内容]
*		RTモードのパケットのCRCチェック
*		[引数]
*		wkLength：CRCを含むデータ長。
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
			/* RTmodeでの計測値受信は可変長パケット */
			if( (vutRfDataBuffer.umRT_AuditTx.mDataCntKind & 0x80) == 0x80 )
			{
				wkReturn = RT_MODE_PAYLOAD_LENGTH_10_CRC;
			}
			else
			{
				wkReturn = RT_MODE_PAYLOAD_LENGTH_5_CRC;
			}
			/* CRC算出 */
			CRCD = 0x0000U;
			/* CRC算出 */
			for( wkDataCount = 0U; wkDataCount < wkLength; wkDataCount++ )
			{
				/* CRCの箇所(31,32byte目)か判断 30U */
				if( wkDataCount == (wkReturn - 2U) )
				{
					/* CRCまでは32byte */
					if( wkLength == (wkReturn) )
					{
						/* 計測値長ゼロの場合 */
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
				/* BCH復号の場合は、ここから復号化 */
				wkReturn = ERROR;
			}
			break;
			
		default:
			/* CRCとBCHを除いたデータ長に変換 */
			wkLength -= 2U ;
			/* CRC算出 */
			CRCD = 0x0000U;
			/* CRC算出 */
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
				/* BCH復号の場合は、ここから復号化 */
				wkReturn = ERROR;
			}
			break;
	}
	
	return( wkReturn );
	
#endif
#if (swLoggerBord == imEnable)
	/* CRCとBCHを除いたデータ長に変換 */
	wkLength -= 2U ;
	/* CRC算出 */
	CRCD = 0x0000U;
	/* CRC算出 */
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
		/* BCH復号の場合は、ここから復号化 */
		wkReturn = ERROR;
	}
	return( wkReturn );
#endif
}


/*
********************************************************************************
*	パケットのCRCチェック
*		[内容]
*		HSモードのパケットのCRCチェック。CRC位置は4,5byte
*		[引数]
*		wkLength：CRCを含むデータ長。BCH符号なしの場合は全データ長。
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
	
	/* CRCとBCHを除いたデータ長に変換 */
	if( arBch == BCH_ON )
	{
		wkCrc =  wkLength / 15U;
		wkLength -= (wkCrc * 4U);
	}
	
	/* CRC算出 */
	CRCD = 0x0000U;
	/* CRC算出 */
	for( wkDataCount = 0U; wkDataCount < wkLength; wkDataCount++ )
	{
		if(wkDataCount == 3U )
		{
			/* CRC挿入位置は飛ばす */
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
		/* BCH復号の場合は、ここから復号化 */
		wkReturn = ERROR;
	}
	return( wkReturn );
}


/*****************************************************************************
	[ﾓｼﾞｭｰﾙ名]
	  [日本名]	BCH符号化前処理
		[概要]	行列変換　uint8_t [11] → uint16_t [8]
		[機能]	〃
		[入力]	uint8_t wkDataIn[0-10]
		[出力]	uint16_t wkDataOut[0-7]
		[備考]	入出力の配列長は固定
		[上位]
		[下位]
*****************************************************************************/
void SubRfDrv_PreBCH( uint8_t *wkDataIn, uint16_t *wkDataOut)
{
	uint16_t wkBCH;
//	uint8_t wkTest;
//	uint8_t wkMakeDataOutLoop;
//	uint8_t wkBitShiftLoop;
	
	int8_t wkX_Axis;
	int8_t wkY_Axis;
	
	/* 初期化処理 */
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
	/* 初期化処理 */
	for( wkBCH = 0U; wkBCH < 8U; wkBCH++ )
	{
		*(wkDataOut + wkBCH) = 0x00;
	}

	for(wkMakeDataOutLoop = 0U; wkMakeDataOutLoop < 8U; wkMakeDataOutLoop++)
	{
		/* 上位1bitは空白、下位4bitはBCH符号で埋まる */
		for( wkBitShiftLoop = 0U; wkBitShiftLoop < 11U; wkBitShiftLoop++ )
		{
			/* マスクし1bit抽出 */
			wkBCH = ((uint16_t)*(wkDataIn + wkBitShiftLoop)) & RF_PRE_BCH[wkMakeDataOutLoop].Mask;
			if( (int8_t) (RF_PRE_BCH[wkMakeDataOutLoop].Shift - wkBitShiftLoop) > 0 )
			{
				/* 左シフト */
				wkTest = RF_PRE_BCH[wkMakeDataOutLoop].Shift - wkBitShiftLoop;
				*wkDataOut |= wkBCH << (RF_PRE_BCH[wkMakeDataOutLoop].Shift - wkBitShiftLoop);
			}
			else if( RF_PRE_BCH[wkMakeDataOutLoop].Shift - wkBitShiftLoop == 0U )
			{
				/* シフトなし */
				*wkDataOut |= wkBCH;
			}
			else
			{
				/* 右シフト */
				*wkDataOut |= wkBCH >> ( wkBitShiftLoop - RF_PRE_BCH[wkMakeDataOutLoop].Shift );
			}
		}
		/* 次のデータ */
		wkDataOut++;
	}
#endif
}


/*****************************************************************************
	[ﾓｼﾞｭｰﾙ名]
	  [日本名]	BCH符号　付加
		[概要]	BCH符号を下位4bitに追加する。
		[機能]	〃
		[入力]	arDatatNumは偶数でないとBCHが1byta単位で埋まらない
		[出力]
		[備考]	入力は8bit単位固定、3bit(0固定)捨てて合計11bit処理。
					wkDataOutの末尾4bitに符号付加
					奇数番目は出力BCH符号(arBchOut)1byteの上位4bit、偶数番目は下位4bit
		[上位]
		[下位]
*****************************************************************************/
void SubRfDrv_MakeBCH(uint8_t *arCulcData, uint8_t *arBchOut, uint8_t arDatatNum)
{
	uint16_t wkDataOut;
	uint16_t wkDataIn;
	uint8_t wkShiftReg = 0x00; 		/* シフトレジスタ値			*/
	uint8_t wkFFout = 0x00;			/* フリップフロップ出力		*/
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
			wkFFout = (wkFFout & 0xEF) | ( (wkCulc & 0x10) ^  ((uint8_t)((wkDataIn & 0x4000) >> 10U) )); 	/* X4 = R4 xor D11	※上位1bitは無視	*/
			wkFFout = (wkFFout & 0xF7) | ( wkCulc & 0x08); 									/* X3 = R3 */
			wkFFout = (wkFFout & 0xFB) | ( wkCulc & 0x04); 									/* X2 = R2 */
			wkFFout = (wkFFout & 0xFD) | ( ( (wkCulc & 0x02) ^ ( (wkFFout & 0x10) >> 3U) ));		/* X1 = X4 xor R1	*/
			/* ↓下位1bit */
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
			wkFFout = (wkFFout & 0xEF) | ( ((wkShiftReg & 0x08) << 1U) ^ (uint8_t)( (wkDataIn & 0x4000) >> 10U) ); 	/* X4 = R4 xor D11	※上位1bitは無視	*/
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
		/* 前処理で4bitシフト済み */
		wkDataOut = wkDataIn;
		for( wkLoop = 0U; wkLoop < 11U; wkLoop++ )
		{
			wkCulc = wkShiftReg << 1U;
			wkFFout = (wkFFout & 0xEF) | ( (wkCulc & 0x10) ^  ((uint8_t)((wkDataIn & 0x4000) >> 10U) )); 	/* X4 = R4 xor D11	※上位1bitは無視	*/
			wkFFout = (wkFFout & 0xF7) | ( wkCulc & 0x08); 									/* X3 = R3 */
			wkFFout = (wkFFout & 0xFB) | ( wkCulc & 0x04); 									/* X2 = R2 */
			wkFFout = (wkFFout & 0xFD) | ( ( (wkCulc & 0x02) ^ ( (wkFFout & 0x10) >> 3U) ));		/* X1 = X4 xor R1	*/
			/* ↓下位1bit */
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
	[ﾓｼﾞｭｰﾙ名]
	  [日本名]	インターリーブ処理
		[概要]	BCH符号を含めインターリーブ処理。
		[機能]	〃
		[入力]	uint16_t[0-7]
		[出力]	uint8_t[0-14] -> BCH符号のみの処理にすることにより[0-3]
		[備考]
		[上位]
		[下位]
*****************************************************************************/
void SubRfDrv_Interleave( uint16_t *wkDataIn, uint8_t *wkDataOut )
{
	uint16_t wkBCH;
//	uint8_t wkTest;
//	uint8_t wkMakeDataOutLoop;
//	uint8_t wkBitShiftLoop;
//	uint16_t wkMask = 0x0008; 	  /* BCH符号のみ処理 */
//	int8_t wkBitShiftCnt = -4;	  /* BCH符号のみの処理 */
	
	int8_t wkX_Axis;
	int8_t wkY_Axis;
	
	/* 初期化処理 */
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
	/* BCH符号のみの処理 */
	for( wkBCH = 0U; wkBCH < 4U; wkBCH++ )
	{
		*(wkDataOut + wkBCH) = 0x00;
	}
	
	/* wkDataInの上位1bitはダミー「0」のため省く */
	/* BCH符号(下位4butのみ処理) */
	for(wkMakeDataOutLoop = 0U; wkMakeDataOutLoop < 4U; wkMakeDataOutLoop++)
	{
		/*	*/
		for( wkBitShiftLoop = 0U; wkBitShiftLoop < 8U; wkBitShiftLoop++ )
		{
			/* マスクし1bit抽出 */
			wkBCH = ((uint16_t)*(wkDataIn + wkBitShiftLoop)) & wkMask;
			if( (int8_t) (wkBitShiftCnt + wkBitShiftLoop) > 0 )
			{
				/* 右シフト */
				wkTest = wkBitShiftCnt + wkBitShiftLoop;
				*wkDataOut |= wkBCH >> (wkBitShiftCnt + wkBitShiftLoop);
			}
			else if( wkBitShiftCnt + wkBitShiftLoop == 0U )
			{
				/* シフトなし */
				*wkDataOut |= wkBCH;
			}
			else
			{
				/* 左シフト */
				wkTest = (0xFF ^ ( wkBitShiftLoop + wkBitShiftCnt ) ) + 1U;
				*wkDataOut |= wkBCH << ( (0xFF ^ ( wkBitShiftLoop + wkBitShiftCnt ) ) + 1U);
			}
		}
		/* マクスを1bitシフト、シフト量を1つ減らし、次のデータ */
		wkMask >>= 1U;
		wkBitShiftCnt--;
		wkDataOut++;
	}
#endif
}
#endif

/*
*******************************************************************************
*  無線送信出力の変更
*  [概要]
*  無線の送信出力を13dBm or -1dBmにする。
*  FSK/Loraの両方共通設定となる ⇒ FSK mode時は-1dBm固定
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
 *	PN9送信関数
 *	[内容]
 *		2.53msec送信(計算上2.496msec) / 1.04msec休止 / バースト周期 3.57msec
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
/* 技適申請の時は有効にする(キャリアセンス有り) */
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
					
					/* RF(無線通信)の初期化完了 */
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
		
		/* RF(無線通信)の初期化完了 */
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
*	Syncへ利用会社ID,グループIDをセット
*
*	[内容]
*		オンライン認証有無に応じてSyncへ利用会社ID,グループIDをセット
*　　6byte SX1272へレジスタ書き込み
* [引数]
*	　wkMyIDを使用
*	オンライン認証情報
*	GW : vRfDrvMainGwSts情報 コンパイラスイッチで切り替え
*	Logger : vRfDrvMainLoggerSts情報 コンパイラスイッチで切り替え
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
				/* AppIDが初期値の場合 */
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
		/* 関数実行はecRfDrvMainSts_HSmode_BeaconRxStartのcase文 */
			if( (vrfMySerialID.mAppID[0U] == ( ((wkDefAppID[0U] << 4U) & 0xF0) + ((wkDefAppID[1U] & 0xF0) >> 4U) ))
				&& (vrfMySerialID.mAppID[1U] == ((wkDefAppID[1U] << 4U) & 0xF0)) )
			{
				/* AppIDが初期値の場合 */
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
				/* [3U]の下位4bitはNULL */
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
			/* ApiRFDrv_LoggerBroadCastFSK関数 */
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
			/* SubRFDrv_CallResponseRecieve関数 */
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
					/* [0U]の上位4bitはNULL */
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
			/* [0U]の上位4bitはNULL */
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
* CALL信号のプリアンブル検出関数
*
* [内容]
*  検知したらRX継続/未検出時はSLEEP
* [Return]
*  SUCCESS / FAIL
********************************************************************************
*/
uint8_t SubRFDrv_Wakeup_PreDetect(void)
{
	uint8_t wkReturn = FAIL;
	
	SubSX1272RxFSK_On();
	/* SLEEPし、ポート割り込みorタイマ割り込みでWAKE UP */
	SubIntervalTimer_Sleep( _PREAMBLE_WAIT_ITMCP_VALUE, 1U );
	gvRfIntFlg.mRadioDio4 = 0U;
	R_INTC1_Start();
	
	while( !gvRfIntFlg.mRadioTimer && !gvRfIntFlg.mRadioDio4 )
	{
		M_HALT;
	}
	
	/* Pre検出チェック */
	if( gvRfIntFlg.mRadioTimer ){	SubIntervalTimer_Stop(); }
	R_INTC1_Stop();
	if( gvRfIntFlg.mRadioDio4 )
	{
		/* Preamble検知 RSSI値取得 */
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
* CALL信号のSync検出関数
*
* [内容]
*  検知したらRX継続/未検出時はSLEEP
*  RSSIは必ず取得する
* [Return]
*  SUCCESS / FAIL
********************************************************************************
*/
uint8_t SubRFDrv_Wakeup_SyncDetect( void )
{
//	uint8_t wkFunc;
	uint8_t wkReturn = HS_MODE_SYNC_FAIL;
	/* ------------SyncWord待ち------------ */
	/* SLEEPし、ポート割り込みorタイマ割り込み3.44msecでWAKE UP */
	gvRfIntFlg.mRadioDio2 = 0U;
	gvRfIntFlg.mRadioTimer = 0U;
	R_INTC3_Start();
	SubIntervalTimer_Sleep( _SYNC_WAIT_ITMCP_VALUE, 1U );			/* 1.376msec */
	
	while( !(gvRfIntFlg.mRadioDio2 | gvRfIntFlg.mRadioTimer) )
	{
		M_HALT;
	}
	
	/* Sync検出チェック */
	R_INTC3_Stop();
	SubIntervalTimer_Stop();
	if( gvRfIntFlg.mRadioDio2 )
	{
		/* sync検知 */
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
*	CALL信号に対する応答タイミング処理
*
*	[内容]
*		ApiRFDrv_Wakeup後の応答信号タイミングを算出する。
*		ロガー側の処理
*		rfRxData.Countdown値入力が必要(受信時に取得)
*		rfMyID.UniqueID[2U]が必要
* [引数]
*		wkTimeSlotGrupe　：　1U〜9U / 0msec,100msec,150msec ....
*		wkTimeSlotNum		:	0U〜3U / 0msec,9msec,18msec,27msec....
* [return]
*		受信カウント値から算出した応答タイミング
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
*	高速モード　ロガー応答データのセット
*
*	[内容]
*		高速モードでビーコンを受信し、その応答を返すデータをセット,符号化する。
*		フラッシュ等からのデータセットは、本関数の前にmainルーチンでセットする。
*		計測値等の連続でデータを送信する際も呼び出される
* [引数]
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
	uint8_t *pwkFlashData;		/* Flash保持設定値 */
	uint8_t *pwkRfData;			/* 無線送受信設定値 */
	uint8_t *pwkEndAdr;			/* 設定値エンドアドレス */
	uint16_t wkLoop;
	uint16_t wkEndIndex;
	uint16_t wkSleep_index;
	uint32_t wkStaIndex;
	
	switch( vrfHsReqCmdCode )
	{
		case ecRfDrvGw_Hs_ReqCmd_Status:
		case ecRfDrvGw_Hs_ReqCmd_Toroku:
			/* ステータス情報を応答 No21 , 登録モードでのステータス情報応答 */
			/* ステータス要求 or 登録モードにより処理変更 */
			if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Status )
			{
				vutRfDataBuffer.umLoggerResStatus.mPacketCode = 0x01;
			}
			else if( ( ((vrfMySerialID.mAppID[0U] >> 4U) & 0x0F) == APP_ID_LOGGER_INIT_UP )
						&& ( (((vrfMySerialID.mAppID[0U] << 4U) & 0xF0) + ((vrfMySerialID.mAppID[1U] >> 4U) & 0x0F)) == APP_ID_LOGGER_INIT_LO ) )
			{
				/* GWのAppIDを登録 */
				vutRfDataBuffer.umLoggerResStatus.mPacketCode = 0x0C;
				memcpy( &vrfMySerialID.mAppID[ 0U ], &vutRfDataBuffer.umGwQueryBeacon.mAppID[ 0U ], 2U );
				gvInFlash.mParam.mAppID[ 0U ] = (vutRfDataBuffer.umGwQueryBeacon.mAppID[0U] >> 4U) & 0x0F;
				gvInFlash.mParam.mAppID[ 1U ] = ((vutRfDataBuffer.umGwQueryBeacon.mAppID[0U] << 4U) & 0xF0) + ((vutRfDataBuffer.umGwQueryBeacon.mAppID[1U] >> 4U) & 0x0F);
				vrfHsmodeSleepMode = 0x00;			/* 登録直後はスリープせずにステータスを返す */
				/* ローカルモードの場合、接続先GWを登録 */
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
				/* GWが登録モードだが既に登録済み ただし登録モード時のAppIDが異なるため、ここには来ない */
				vutRfDataBuffer.umLoggerResStatus.mPacketCode = 0x0D;
				SubRFDrv_Hsmode_End();
				
				ApiFlash_WriteSysLog( ecSysLogItm_IdDiff, 4U );				/* システム履歴：ID書き換わりの履歴に相乗り */
				
				break;
			}
			
			vrfHsmodePacketLength = RF_HSMODE_RES_STATUS_LENGTH;
			
			/* ロガーIDと宛先GW IDをセット */
			memcpy( &vutRfDataBuffer.umLoggerResStatus.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
			
			/* 宛先GWID */
			memcpy( &vutRfDataBuffer.umLoggerResStatus.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
			
			/* グループID */
			vutRfDataBuffer.umLoggerResStatus.mGroupID = vrfMySerialID.mGroupID;
			
			/* ロガー名称 */
			memcpy( &vutRfDataBuffer.umLoggerResStatus.mLoggerName[ 0U ], &gvInFlash.mParam.mLoggerName[ 0U ], imLoggerNameNum );
			
			/* センサタイプ */
//			vutRfDataBuffer.umLoggerResStatus.mSensorType = cSensType;
			vutRfDataBuffer.umLoggerResStatus.mSensorType = gvInFlash.mProcess.mModelCode;
			
			/* バージョン */
			vutRfDataBuffer.umLoggerResStatus.mVersion[ 0U ] = cRomVer[ 0U ];
			vutRfDataBuffer.umLoggerResStatus.mVersion[ 1U ] = cRomVer[ 1U ] * 10 + cRomVer[ 2U ];
			vutRfDataBuffer.umLoggerResStatus.mVersion[ 2U ] = cRomVer[ 3U ];
			
			/* 異常ステータス、電池残量情報 */
			ApiFlash_RfRTAbnStatusSet();
			
			/* 計測周期の値をセット */
			if( gvHsMeasAlmFlg == imOFF )
			{
				/* 最新データ時刻 */
				memcpy( &vutRfDataBuffer.umLoggerResStatus.mStatusTime[ 0U ], &gvRfRTMeasData.mRTMeasData_MeasTime[ 0U ], 4U );
				
				/* 最新計測値と論理 */
				memcpy( &vutRfDataBuffer.umLoggerResStatus.mStatusMeasure_AllCh_Event[ 0U ], &gvRfRTMeasData.mRTMeasData_MeasVal[ 0U ], 5U );
				
				/* 異常ステータス、電池残量情報 */
				vutRfDataBuffer.umLoggerResStatus.mStatusAbnormal = gvRfRTMeasData.mRTMeasData_AbnStatus & 0xC7U;
			}
			/* 警報監視周期の値をセット */
			else
			{
				/* 最新データ時刻 */
				memcpy( &vutRfDataBuffer.umLoggerResStatus.mStatusTime[ 0U ], &gvRfRTMeasAlm2Data.mRTMeasData_MeasTime[ 0U ], 4U );
				
				/* 最新計測値と論理 */
				memcpy( &vutRfDataBuffer.umLoggerResStatus.mStatusMeasure_AllCh_Event[ 0U ], &gvRfRTMeasAlm2Data.mRTMeasData_MeasVal[ 0U ], 5U );
				
				/* 異常ステータス、電池残量情報 */
				vutRfDataBuffer.umLoggerResStatus.mStatusAbnormal = (gvRfRTMeasData.mRTMeasData_AbnStatus & 0xC7U) + 0x08U;
				
				/* 最新警報フラグ */
				ApiFlash_RfRTAlarmFlagSet( gvMeasPrm.mAlmFlg, gvMeasPrm.mDevFlg, gvRfRTMeasData.mRTMeasData_AlarmFlag );
				for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
				{
					gvHsAlmFlag[ wkLoop ] = gvMeasPrm.mDevFlg[ wkLoop ];
					gvHsAlmFlag[ wkLoop ] |= gvMeasPrm.mAlmFlg[ wkLoop ] << 4U;
				}
			}
			
			
			/* 最新警報フラグ */
			memcpy( &vutRfDataBuffer.umLoggerResStatus.mAlart_AllCh[ 0U ], &gvHsAlmFlag[ 0U ], 4U );
			
			/* 利用会社ID */
			vutRfDataBuffer.umLoggerResStatus.mAppID[ 0U ] = (vrfMySerialID.mAppID[ 0U ] >> 4U) & 0x0F;
			vutRfDataBuffer.umLoggerResStatus.mAppID[ 1U ] = ((vrfMySerialID.mAppID[ 0U ] << 4U) & 0xF0) + ((vrfMySerialID.mAppID[ 1U ] >> 4U) & 0x0F);
			
			/* 設定変更時刻 */
			memcpy( &vutRfDataBuffer.umLoggerResStatus.mSetTime[ 0U ], &gvInFlash.mParam.mSetvalChgTime[ 0U ], 4U );
			
			/* 無線通信に時系列ID追加 */
			SubRfDrv_SetStatusAdd( &vutRfDataBuffer.umLoggerResStatus.mMeasId.mNewDataId[ 0U ] );
			
			/* 予約 */
			vutRfDataBuffer.umLoggerResStatus.mReserve[ 0U ] = 0x00U;
			vutRfDataBuffer.umLoggerResStatus.mReserve[ 1U ] = 0x00U;
			
			SubRfDrv_Coding_HS( &vutRfDataBuffer.umData[ 0U ], RF_HSMODE_RES_STATUS_LENGTH, BCH_OFF );
			break;
			
		/* 計測値応答 No22 */
		case ecRfDrvGw_Hs_ReqCmd_Measure:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_MEASURE_LENGTH;
			
			/* 送信パケット数をセット */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 90U );
			
			/* Flashから送信データのリード完了 */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_Meas1 ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResMeasure.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResMeasure.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
//				vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode = (vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode & 0xF0) + 0x02;	/* 0x02 ecRfDrvGw_Hs_ReqCmd_Measure */
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_MEASURE_LENGTH_HEADER - 8U, BCH_ON );
				SubRfDrv_Payload_Cording( 10U, 10U, 9U, RF_HSMODE_RES_MEASURE_LENGTH_HEADER );
				
				/* 初回(1パケット目)データセット */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* 送信データ数90未満 */
					if( vutRfDataBuffer.umLoggerResMeasure.mDataNum < 90U )
					{
						/* Flashリード完了 */
						vrfPacketSum = 2U;
					}
					
					/* 収録データ送信後、SleepとなるIndexNo.を算出 */
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
					
					/* 高速通信スリープ遷移の条件算出 */
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
					
					/* スリープIndexNo.と現在のIndexNo.の間の要求のとき */
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
				/* Flashリードできないときのエラー処理 */
				M_NOP;
			}
			break;
			
		/* 警報履歴応答 No23 */
		case ecRfDrvGw_Hs_ReqCmd_AlartHis:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_ALHIS_LENGTH;
			
			/* 送信パケット数をセット */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 100U );
			
			/* Flashから送信データのリード完了 */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_MeasAlm ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResAlarmHist.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResAlarmHist.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
				vutRfDataBuffer.umLoggerResAlarmHist.mPacketCode = 0x03;
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_ALHIS_LENGTH_HEADER, BCH_OFF );
				SubRfDrv_Payload_Cording( 8U, 10U, 10U, RF_HSMODE_RES_ALHIS_LENGTH_HEADER );
				
				/* 初回(1パケット目)データセット */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* 送信データ数100未満 */
					if( vutRfDataBuffer.umLoggerResAlarmHist.mDataNum < 100U )
					{
						/* Flashリード完了 */
						vrfPacketSum = 2U;
					}
				}
			}
			else
			{
				/* Flashリードできないときのエラー処理 */
				M_NOP;
			}
			break;
			
		/* 機器異常履歴応答 No24 */
		case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_ABNORMAL_LENGTH;
			
			/* 送信パケット数をセット */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 100U );
			
			/* Flashから送信データのリード完了 */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_ErrInfo ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResAbnormalHist.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResAbnormalHist.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
				vutRfDataBuffer.umLoggerResAbnormalHist.mPacketCode = 0x04;
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_ABNORMAL_LENGTH_HEADER, BCH_OFF );
				SubRfDrv_Payload_Cording( 6U, 10U, 10U, RF_HSMODE_RES_ABNORMAL_LENGTH_HEADER );
				
				/* 初回(1パケット目)データセット */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* 送信データ数100未満 */
					if( vutRfDataBuffer.umLoggerResAbnormalHist.mDataNum < 100U )
					{
						/* Flashリード完了 */
						vrfPacketSum = 2U;
					}
				}
			}
			else
			{
				/* Flashリードできないときのエラー処理 */
				M_NOP;
			}
			break;
			
		case ecRfDrvGw_Hs_ReqCmd_EventHis:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_EVENTHIS_LENGTH;
			
			/* 送信パケット数をセット */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 60U );
			
			/* Flashから送信データのリード完了 */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_Event ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResEventHist.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResEventHist.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
				vutRfDataBuffer.umLoggerResEventHist.mPacketCode = 0x05;
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_EVENTHIS_LENGTH_HEADER, BCH_OFF );
				SubRfDrv_Payload_Cording( 14U, 10U, 6U, RF_HSMODE_RES_EVENTHIS_LENGTH_HEADER );
				
				/* 初回(1パケット目)データセット */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* 送信データ数60未満 */
					if( vutRfDataBuffer.umLoggerResEventHist.mDataNum < 60U )
					{
						/* Flashリード完了 */
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
			
			/* 送信パケット数をセット */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 90U );
			
			/* Flashから送信データのリード完了 */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_ActLog ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResOpeHist.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResOpeHist.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
				vutRfDataBuffer.umLoggerResOpeHist.mPacketCode = 0x06;
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_OPEHIS_LENGTH_HEADER, BCH_OFF );
				SubRfDrv_Payload_Cording( 10U, 10U, 9U, RF_HSMODE_RES_OPEHIS_LENGTH_HEADER );
				
				/* 初回(1パケット目)データセット */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* 送信データ数90未満 */
					if( vutRfDataBuffer.umLoggerResOpeHist.mDataNum < 90U )
					{
						/* Flashリード完了 */
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
			
			/* 送信パケット数をセット */
			SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( 90U );
			
			/* Flashから送信データのリード完了 */
			if( imARI == ApiSetFlash_ReqRdFlg( 0U, ecRegionKind_SysLog ) )
			{
				memcpy( &vutRfDataBuffer.umLoggerResSysHist.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
				memcpy( &vutRfDataBuffer.umLoggerResSysHist.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
				vutRfDataBuffer.umLoggerResSysHist.mPacketCode = 0x07;
				
				SubRfDrv_Header_Cording( RF_HSMODE_RES_OPEHIS_LENGTH_HEADER, BCH_OFF );
				SubRfDrv_Payload_Cording( 10U, 10U, 9U, RF_HSMODE_RES_OPEHIS_LENGTH_HEADER );
				
				/* 初回(1パケット目)データセット */
				if( arPacket == 0U )
				{
					vMem_RdIndexNo = vMem_ReqIndexNo_St;
					/* 送信データ数90未満 */
					if( vutRfDataBuffer.umLoggerResSysHist.mDataNum < 90U )
					{
						/* Flashリード完了 */
						vrfPacketSum = 2U;
					}
				}
			}
			else
			{
				M_NOP;
			}
			break;
			
		/* ロガー設定値を応答 */
		case ecRfDrvGw_Hs_ReqCmd_SetRead:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_SETREAD_LENGTH;
			vrfPacketLimit = 1U;				/* 設定読み込みは1パケット */
			vrfPacketSum = vrfPacketLimit;
			
			memcpy( &vutRfDataBuffer.umLoggerResSetting.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
			memcpy( &vutRfDataBuffer.umLoggerResSetting.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
			vutRfDataBuffer.umLoggerResSetting.mPacketCode = 0x08;
			
			/* 設定値を無線送信データに渡す */
			for( wkLoop = 0U ; wkLoop < imChNum ; wkLoop++ )
			{
				/* 上上限警報値1ch */
				wkCulc = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviUU[ wkLoop ] );
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUUAlarmValue[ 0U ] = wkCulc & 0x00FFU;
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUUAlarmValue[ 1U ] = wkCulc >> 8U;
				
				/* 上上限警報遅延1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUUAlarmDelay = gvInFlash.mParam.mDelayUU[ wkLoop ];
				
				/* 上限警報値1ch */
				wkCulc = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviU[ wkLoop ] );
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUAlarmValue[ 0U ] = wkCulc & 0x00FFU;
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUAlarmValue[ 1U ] = wkCulc >> 8U;
				
				/* 上限警報遅延1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mUAlarmDelay = gvInFlash.mParam.mDelayU[ wkLoop ];
				
				/* 下限警報値1ch */
				wkCulc = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviL[ wkLoop ] );
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLAlarmValue[ 0U ] = wkCulc & 0x00FFU;
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLAlarmValue[ 1U ] = wkCulc >> 8U;
				
				/* 下限警報遅延1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLAlarmDelay = gvInFlash.mParam.mDelayL[ wkLoop ];
				
				/* 下下限警報値1ch */
				wkCulc = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviLL[ wkLoop ] );
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLLAlarmValue[ 0U ] = wkCulc & 0x00FFU;
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLLAlarmValue[ 1U ] = wkCulc >> 8U;
				
				/* 下下限警報遅延1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mLLAlarmDelay = gvInFlash.mParam.mDelayLL[ wkLoop ];
				
				/* 逸脱許容時間選択閾値1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mToleranceLevel = gvInFlash.mParam.mDeviEnaLv[ wkLoop ];
				
				/* 逸脱許容時間1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mToleranceTime[ 0U ] = gvInFlash.mParam.mDeviEnaTime[ wkLoop ] & 0x00FFU;
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mToleranceTime[ 1U ] = gvInFlash.mParam.mDeviEnaTime[ wkLoop ] >> 8U;
				
				/* 計測値オフセット1ch */
				wkCulc = ApiInFlash_OftVal_ValtoCnt( gvInFlash.mParam.mOffset[ wkLoop ] );
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mMeasureOffset = (uint8_t)wkCulc;
				
				/* 計測値一次傾き補正1ch */
				vutRfDataBuffer.umLoggerResSetting.mChSetting[ wkLoop ].mMeasureZeroSpan = gvInFlash.mParam.mSlope[ wkLoop ] - 70;
			}
			
			/* 電池種から接続許可GWまでを一気にセット */
			pwkFlashData = (uint8_t *)&gvInFlash.mParam.mBatType;
			pwkRfData = (uint8_t *)&vutRfDataBuffer.umLoggerResSetting.mBattCode;
			pwkEndAdr = (uint8_t *)&vutRfDataBuffer.umLoggerResSetting.mCommGwID[ 9U ][ 2U ];
			for(  ; pwkRfData <= pwkEndAdr ; pwkFlashData++, pwkRfData++ )
			{
				*pwkRfData = *pwkFlashData;
			}
			
			/* スケーリングだけ変換して再セット */
			wkCulc = ApiInFlash_ScaleVal_ValtoCnt( gvInFlash.mParam.mScalYInt );
			vutRfDataBuffer.umLoggerResSetting.mScalYInt[ 0U ] = wkCulc & 0x00FFU;
			vutRfDataBuffer.umLoggerResSetting.mScalYInt[ 1U ] = wkCulc >> 8U;
			
			wkCulc = ApiInFlash_ScaleVal_ValtoCnt( gvInFlash.mParam.mScalGrad );
			vutRfDataBuffer.umLoggerResSetting.mScalGrad[ 0U ] = wkCulc & 0x00FFU;
			vutRfDataBuffer.umLoggerResSetting.mScalGrad[ 1U ] = wkCulc >> 8U;
			
			/* グループID */
			vutRfDataBuffer.umLoggerResSetting.mGroupID = gvInFlash.mParam.mGroupID;
			
			/* 設定変更時刻 */
			memcpy( &vutRfDataBuffer.umLoggerResSetting.mSetTime[ 0U ], &gvInFlash.mParam.mSetvalChgTime[ 0U ], 4U );
			
			SubRfDrv_Coding_HS( &vutRfDataBuffer.umData[0U], RF_HSMODE_RES_SETREAD_LENGTH_HEADER, BCH_OFF );
			
			CRCD = 0x0000U;
			for( wkRadio = RF_HSMODE_RES_SETREAD_LENGTH_HEADER; wkRadio < RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_BCH - 2U; wkRadio++ )
			{
				/* CRC計算 */
				CRCIN = vutRfDataBuffer.umData[ wkRadio ];
			}
			vutRfDataBuffer.umLoggerResSetting.mCrc2[1U] = (uint8_t)(CRCD);
			vutRfDataBuffer.umLoggerResSetting.mCrc2[0U] = (uint8_t)(CRCD >> 8U);
			
			SubRfDrv_Coding_Bch( &vutRfDataBuffer.umData[RF_HSMODE_RES_SETREAD_LENGTH_HEADER]
				, RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_BCH - RF_HSMODE_RES_SETREAD_LENGTH_HEADER
				, RF_HSMODE_RES_SETREAD_LENGTH - RF_HSMODE_RES_SETREAD_LENGTH_HEADER - RF_HSMODE_RES_SETREAD_LENGTH_BCH  );
			
			break;
			
		/* GWに設定値要求 */
		case ecRfDrvGw_Hs_ReqCmd_SetWrite:
			
			vrfHsmodePacketLength = RF_HSMODE_RES_SETQUERY_LENGTH;
			vrfHsReqDataNum = 1U;
			vrfPacketLimit = vrfHsReqDataNum;
			vrfPacketSum = vrfPacketLimit;
			
			/* テストにダミーデータをセット(予約データ) */
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
			/* ファームアップデート通信 */
			vrfHsmodePacketLength = RF_HSMODE_RES_REQ_FIRM_LENGTH;
			if( vrfPacketLimit == 0U )
			{
				/* 初回のみ */
				vrfHsReqDataNum = 1U;
				vrfPacketLimit = vrfHsReqDataNum;
				vrfPacketSum = vrfPacketLimit;
			}
			/* テストにダミーデータをセット */
			wkRadio = 0U;
			for( wkCulc = 0U; wkCulc < RF_HSMODE_RES_REQ_FIRM_LENGTH; wkCulc++ )
			{
				vutRfDataBuffer.umData[wkCulc] = wkRadio;
				wkRadio++;
			}
			memcpy( &vutRfDataBuffer.umLoggerAckNack.mLoggerID[ 0U ], &vrfMySerialID.mUniqueID[ 0U ], 3U );
			memcpy( &vutRfDataBuffer.umLoggerAckNack.mGwID[ 0U ], &vrfCallID[ 0U ], 3U );
			vutRfDataBuffer.umLoggerAckNack.mPacketCode = 0x11;
			vutRfDataBuffer.umLoggerAckNack.mResponce = 0x00;			/* ファームアップOK応答 */
			SubRfDrv_Coding_HS( &vutRfDataBuffer.umData[0U], RF_HSMODE_RES_REQ_FIRM_LENGTH, BCH_OFF );
			
			break;
			
			
		default:
			/* コマンド種　不明 */
//			vrfInitState = ecRfDrv_Init_Non;
//			vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
//			gvModuleSts.mRf = ecRfModuleSts_Sleep;
			break;
	}
}

/*
*******************************************************************************
*	高速モード ロガー応答データのセット(ステータス応答のサブ関数)
*	
*	[内容]
*		ロガー応答データのセット(ステータス応答のサブ関数)
*	[引数]
*		無線バッファ
*	[return]
*		無し
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
			/* 計測値 */
			default:
			case 0U:
				wkNewIndex = (gvRfRTMeasData.mRTMeasData_MeasID[ 1U ] << 8U) + gvRfRTMeasData.mRTMeasData_MeasID[ 0U ];
				wkOldIndex = gvInFlash.mData.mMeas1_PastIndex;
				wkOldTime = gvInFlash.mData.mMeas1_PastTime;
				break;
			/* 計測警報履歴 */
			case 1U:
				wkNewIndex = gvInFlash.mData.mMeasAlm_Index;
				wkOldIndex = gvInFlash.mData.mMeasAlm_PastIndex;
				wkOldTime = gvInFlash.mData.mMeasAlm_PastTime;
				break;
			/* 動作履歴 */
			case 2U:
				wkNewIndex = gvInFlash.mData.mActLog_Index;
				wkOldIndex = gvInFlash.mData.mActLog_PastIndex;
				wkOldTime = gvInFlash.mData.mActLog_PastTime;
				break;
			/* イベント履歴 */
			case 3U:
				wkNewIndex = gvInFlash.mData.mEvt_Index;
				wkOldIndex = gvInFlash.mData.mEvt_PastIndex;
				wkOldTime = gvInFlash.mData.mEvt_PastTime;
				break;
			/* 機器異常履歴 */
			case 4U:
				wkNewIndex = gvInFlash.mData.mErrInf_Index;
				wkOldIndex = gvInFlash.mData.mErrInf_PastIndex;
				wkOldTime = gvInFlash.mData.mErrInf_PastTime;
				break;
			/* システム履歴 */
			case 5U:
				wkNewIndex = gvInFlash.mData.mSysLog_Index;
				wkOldIndex = gvInFlash.mData.mSysLog_PastIndex;
				wkOldTime = gvInFlash.mData.mSysLog_PastTime;
				break;
		}
		
		/* 最新IndexNo. */
		(*parBuffer) = (uint8_t)(wkNewIndex & 0x00FFU);
		parBuffer ++;
		(*parBuffer) = (uint8_t)((wkNewIndex) >> 8U & 0x00FFU);
		parBuffer ++;
		
		/* 最古IndexNo. */
		(*parBuffer) = (uint8_t)(wkOldIndex & 0x00FFU);
		parBuffer ++;
		(*parBuffer) = (uint8_t)((wkOldIndex) >> 8U & 0x00FFU);
		parBuffer ++;
		
		/* 最古時刻 */
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
*	高速モード　ロガー応答データのセット(サブ関数)
*
*	[内容]
*		応答するデータ数で送信パケット数を算出しセットする
* [引数]
*		uint8_t arNum :1パケットで応答するデータ数
* [return]
*
*******************************************************************************
*/
static void SubRFDrv_Hsmode_Logger_ResponceDataSet_PacketSet( uint8_t arNum )
{
	if( vrfPacketLimit == 0U )
	{
		/* 初回のみ */
		/* 要求IndexNo.から送信パケット数を算出 */
		vrfPacketLimit = vrfHsReqDataNum / arNum;
		if(vrfHsReqDataNum % arNum > 0U)
		{
			vrfPacketLimit++;
		}
		
		/* 最初の偶数パケットも同じデータで応答するため、パケット数1つ増やす */
		vrfPacketLimit++;
		
		vrfPacketSum = vrfPacketLimit;
	}
}

/*
*******************************************************************************
*	高速モード　高速通信スリープ動作
*
*	[内容]
*		受信器に送信した収録データが高速通信スリープの設定値まで達したかを判定
* [引数]
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
		/* 直前にセットされたパケットの情報 */
		wkSnd_StaId = (uint16_t)vutRfDataBuffer.umLoggerResMeasure.mHeaderID[ 1U ] << 8U;
		wkSnd_StaId += vutRfDataBuffer.umLoggerResMeasure.mHeaderID[ 0U ];
		wkSnd_EndId = wkSnd_StaId + vutRfDataBuffer.umLoggerResMeasure.mDataNum - 1;
		
		if( wkSnd_EndId > imIndexMaxMeas1 )
		{
			wkSnd_EndId -= imIndexMaxMeas1;
		}
		
		/* スリープとなる起点が含まれるパケットが送信された */
		if( wkSnd_StaId == vSleepPacket_StaIndex )
		{
			/* 条件1クリア */
			M_SETBIT( vSleep_Sts, M_BIT1 );
		}
		
		/* 起点が含まれるパケットの一つ前のパケットが送信された */
		if( vPrePacket_EndIndex == 0U || wkSnd_EndId == vPrePacket_EndIndex )
		{
			/* 条件2クリア */
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
*	高速モード　ロガー応答データのパケットポインタを設定
*
*	[内容]
*		高速モードでビーコンを受信し、その応答を返すデータをセットするためのパケットポインタを設定
*		ポインタを元にロガーのフラッシュから送信バッファへセットすることを想定
* [引数]
*
* [return]
*
*******************************************************************************
*/
static void SubRFDrv_Hsmode_PcktPointerSet( void )
	{
	uint16_t wkU16;
	
		/* セット済みの次の送信待ちが奇数パケット */
		if( gvrfStsEventFlag.mHsmodeTxTurn == 0U )
		{
			gvrfHsmodePacketPt = vrfPacketPt[ 1U ];			/* 次Flashからリードするのは偶数パケット */
			
			/* 偶数パケットのリード全完了：送信完了 */
			if( vrfLogSendPacketCtl.mFlashReadFin[ 1U ] == ecLogSndCtl_ReadFin )
			{
				vrfLogSendPacketCtl.mFlashReadFin[ 1U ] = ecLogSndCtl_SndFin;
			}
		}
		/* セット済みの次の送信待ちが偶数パケット */
		else
		{
			gvrfHsmodePacketPt = vrfPacketPt[ 0U ];			/* 次Flashからリードするのは奇数パケット */
			
			/* 奇数パケットのリード全完了：送信完了 */
			if( vrfLogSendPacketCtl.mFlashReadFin[ 0U ] == ecLogSndCtl_ReadFin )
			{
				vrfLogSendPacketCtl.mFlashReadFin[ 0U ] = ecLogSndCtl_SndFin;
			}
		}
		
		/* ともにリード完了なら送信完了 */
		if( (vrfLogSendPacketCtl.mFlashReadFin[ 0U ] == ecLogSndCtl_SndFin) &&
			(vrfLogSendPacketCtl.mFlashReadFin[ 1U ] == ecLogSndCtl_SndFin) )
		{
			vrfPacketSum = 0U;
		}
		/* 奇数パケットのみ送信完了 */
		else if( vrfLogSendPacketCtl.mFlashReadFin[ 0U ] == ecLogSndCtl_SndFin )
		{
			gvrfStsEventFlag.mHsmodeTxTurn = 0U;			/* 今セット済みの送信後に偶数パケットに切り替わるようにセット */
			gvrfHsmodePacketPt = vrfPacketPt[ 1U ];			/* 次Flashからリードするのは偶数パケット */
		}
		/* 偶数パケットのみ送信完了 */
		else if( vrfLogSendPacketCtl.mFlashReadFin[ 1U ] == ecLogSndCtl_SndFin )
		{
			gvrfStsEventFlag.mHsmodeTxTurn = 1U;			/* 今セット済みの送信後に奇数パケットに切り替わるようにセット */
			gvrfHsmodePacketPt = vrfPacketPt[ 0U ];			/* 次Flashからリードするのは奇数パケット */
		}
	
	
		if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Measure )
		{
			/* 条件1と2を達成 → スリープへ遷移 */
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
*	高速モード　ロガー　ファームHEX,設定変更情報の受信に対するAck応答データセット
*
*	[内容]
*		高速モードでGWから受信したファームHEX,設定変更情報に対するACKデータのセット
* [引数]
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
	
	/* パケット種別コード */
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
	
	/* 応答内容 */
	if( vrfResInfo == ecRfDrv_Success )
	{
		vrfHsmodeAckBuff.umCategories.mResponce = 0x00;		/* ACK */
	}
	else
	{
		vrfHsmodeAckBuff.umCategories.mResponce = 0x01;		/* NACK */
	}
	
	/* 設定変更時刻 */
	memcpy( &vrfHsmodeAckBuff.umCategories.mSetTime[ 0U ], &gvInFlash.mParam.mSetvalChgTime[ 0U ], 4U );
	
	SubRfDrv_Coding_HS( &vrfHsmodeAckBuff.umData[0U], RF_HSMODE_ACK_LENGTH, BCH_OFF );

}

/*
************************************************************************************
*	GWのIDフィルタリングチェック
*	[内容]
*		受信したGWIDとロガーのフィルタリング設定を比較する
*		gvInFlash.mParam.mCnctEnaGwId[0U]の[0U]-[2U]が0x00 00 00 の場合は全GWを許可
*	[引数]
*		*arGwid : 受信したGWのIDを上詰めで3byte
*	[返り値]
*		ecRfDrv_Success : 通信許可
*		ecRfDrv_Fail : 通信拒否
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
*	高速モード　ロガーからの応答データに対するGWのResponce/Ackデータセット
*
*	[内容]
*		高速モードでビーコンを送信し、その応答受信に対するAckデータをセットする。
*		計測値等の連続でデータを受信する際も呼び出される
*
*		設定書き込み/ファームアップ通信のデータセットはメインルーチンでのセットを想定
*			gvrfStsEventFlag.mHsmodeGwToLgDataSet?==?1U　かつ、
*			vRfDrvMainGwSts?=?ecRfDrvMainSts_HSmode_Gw_To_Lg_DataSet
*			の場合にデータをセットする。
*			ファームアップの場合はgvrfHsmodePacketPtに応じたパケットをセットする
*			(GW側は未実装)
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
 *	応答データに対するACKデータセット
 *	(計測値、警報履歴、機器異常履歴、イベント履歴、動作履歴、システム履歴)
 *
 *	[内容]
 *		応答データに対するACKデータセット
 *		(計測値、警報履歴、機器異常履歴、イベント履歴、動作履歴、システム履歴)
 *	[引数]
 *		uint8_t arLoop:応答バッファのループカウンタ
 *******************************************************************************
 */
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_MeasHist( uint8_t arLoop )
{
	/* 受信成功なら応答バッファをCPU間通信バッファへセット */
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
 *	受信した応答バッファをCPU間通信バッファへセット
 *
 *	[内容]
 *		受信した応答バッファをCPU間通信バッファへセット
 *	[引数]
 *		なし
 *******************************************************************************
 */
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_SubModDatSet( void )
{
	if( gvutComBuff.umModInf.mComSts == ecComSts_RcvWait )
	{
		gvutComBuff.umModInf.mHiPriorityFlg = imON;			/* CPU間Modbus通信の最優先送信フラグ(高速通信収集データ応答)をON */
		
		switch( vrfHsReqCmdCode )
		{
			/* 計測値応答 */
			case ecRfDrvGw_Hs_ReqCmd_Measure:
				ApiModbus_SndFnc16Ref41201( &vutRfDataBuffer.umLoggerResMeasure );
				break;
				
			/* 警報履歴応答 */
			case ecRfDrvGw_Hs_ReqCmd_AlartHis:
				ApiModbus_SndFnc16Ref46001( &vutRfDataBuffer.umLoggerResAlarmHist );
				break;
				
			/* 異常履歴応答 */
			case ecRfDrvGw_Hs_ReqCmd_AbnormalHis:
				ApiModbus_SndFnc16Ref46601( &vutRfDataBuffer.umLoggerResAbnormalHist );
				break;
				
			/* イベント履歴応答 */
			case ecRfDrvGw_Hs_ReqCmd_EventHis:
				ApiModbus_SndFnc16Ref47101( &vutRfDataBuffer.umLoggerResEventHist );
				break;
				
			/* 動作履歴応答 */
			case ecRfDrvGw_Hs_ReqCmd_OpeHis:
				ApiModbus_SndFnc16Ref47701( &vutRfDataBuffer.umLoggerResOpeHist );
				break;
				
			/* システム履歴応答 */
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
 *	応答データに対するACKデータセット(設定値読込み)
 *
 *	[内容]
 *		応答データに対するACKデータセット(設定値読込み)
 *	[引数]
 *		uint8_t arLoop:応答バッファのループカウンタ
 *******************************************************************************
 */
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_SetRead( uint8_t arLoop )
{
	/* 設定値は1パケットのためACKはなし データREADのみ */
	/* 受信成功なら応答バッファをCPU間通信バッファへセット */
	if( vrfResInfo == ecRfDrv_Success )
	{
		/* 受信した設定値をCPU間通信バッファへセット */
		ApiModbus_SndFnc16Ref42101( &vutRfDataBuffer.umLoggerResSetting );
		ApiModbus_SndFnc16Ref42158( &vutRfDataBuffer.umLoggerResSetting );
		
		vrfHsReqCmdCode = ecRfDrvGw_Hs_ReqCmd_Status;
	}else
	{
		/* ここには来ないはず */
		vRfDrvMainGwSts = ecRfDrvMainSts_RTmode_GWIdle;
	}
}

/*
 *******************************************************************************
 *	応答データに対するResponseデータセット(設定値書込み)
 *
 *	[内容]
 *		応答データに対するResponseデータセット(設定値書込み)
 *	[引数]
 *		uint8_t arLoop:応答バッファのループカウンタ
 *******************************************************************************
 */
static void SubRFDrv_Hsmode_Logger_BcnAckDataSet_SetWrite( uint8_t arLoop )
{
	uint16_t wkRadio;
	
	if( gvrfStsEventFlag.mHsmodeGwToLgDataSet == 1U )
	{
		/* Linuxから取得した設定データを無線バッファにセット */
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
			/* CRC計算 */
			CRCIN = vutRfDataBuffer.umData[ wkRadio ];
		}
		vutRfDataBuffer.umGwWriteSet.mCrc2[1U] = (uint8_t)(CRCD);
		vutRfDataBuffer.umGwWriteSet.mCrc2[0U] = (uint8_t)(CRCD >> 8U);
		
		SubRfDrv_Coding_Bch( &vutRfDataBuffer.umData[RF_HSMODE_RES_SETWRITE_LENGTH_HEADER]
			, RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_BCH - RF_HSMODE_RES_SETWRITE_LENGTH_HEADER
			, RF_HSMODE_RES_SETWRITE_LENGTH - RF_HSMODE_RES_SETWRITE_LENGTH_HEADER - RF_HSMODE_RES_SETWRITE_LENGTH_BCH	);
	}else
	{
		/* メインルーチンでのデータセット待ち */
	}
}

/*
 *******************************************************************************
 *	応答データに対するResponseデータセット(ファーム書込み)
 *
 *	[内容]
 *		応答データに対するResponseデータセット(ファーム書込み)
 *	[引数]
 *		uint8_t arLoop:応答バッファのループカウンタ
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
	
	/* ファームアップの場合はメインに戻った後にセットする */
	if( gvrfStsEventFlag.mHsmodeGwToLgDataSet == 1U )
	{
		/* ヘッダセット */
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
		
		/* 最初のパケット:空パケット */
		if( vrfPacketSum == 0U )
		{
			for( wkRadio = 0U ; wkRadio < 890U ; wkRadio++ )
			{
				vutRfDataBuffer.umFirmHex.mHex[ wkRadio ] = 0xFFU;
			}
			vCrcData = 0x0000;
			vPrePacket = 1;
		}
		/* 最後のパケット:CRC結果 */
		else if( vrfPacketSum == vrfPacketLimit )
		{
			vutRfDataBuffer.umFirmHex.mReserve[ 0U ] = vCrcData >> 8U;				/* ファームのCRC結果 */
			vutRfDataBuffer.umFirmHex.mReserve[ 1U ] = vCrcData & 0x00FF;
			vutRfDataBuffer.umFirmHex.mReserve[ 2U ] = vrfHsLoginUserId >> 8U;		/* ログインユーザID */
			vutRfDataBuffer.umFirmHex.mReserve[ 3U ] = vrfHsLoginUserId & 0x00FF;
		}
		/* 通常:ファームデータ */
		else
		{
			/* 外付けFlashからリードし、HEXデータをセット */
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
*	高速モード　ロガーからのビーコン後レスポンスのパケットコードをチェックする
*
*	[内容]
*		パケット種別コードとゲートウェイが送出したリクエストコマンドの一致確認
*		計測値通信,履歴通信などLogger⇒GWのデータ通信時
*	[引数]
*		なし
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
* HSmodeヘッダ部　復号処理
*　ステータス通信以外のヘッダ処理
*
*******************************************************************************
*/
ET_RfDrv_ResInfo_t SubRfDrv_Header_Decryption( uint8_t arLength )
{
	ET_RfDrv_ResInfo_t wkReturn;

	wkReturn = ecRfDrv_Success;
	if( SubRfDrv_Crc_Decoding_HS( &vutRfDataBuffer.umData[0U], arLength, BCH_OFF ) != SUCCESS )
	{
		/* CRCエラー Nack応答 */
//		vrfTestHeaderCrcError++;
		wkReturn = ecRfDrv_Fail;
	}
	if( memcmp( vutRfDataBuffer.umGwQueryBeacon.mGwID, vrfMySerialID.mUniqueID, 3U ) != 0 && (wkReturn == ecRfDrv_Success) )
	{
		/* 自機宛ではない 無応答 */
		wkReturn = ecRfDrv_Error;
	}
	if( (SubRFDrv_Hsmode_BcnPktCodeCheck() != SUCCESS) && (wkReturn == ecRfDrv_Success) )
	{
		/* パケット種別コードのエラー 無応答 */
		wkReturn = ecRfDrv_Error;
	}
	
	return wkReturn;
}


/*
*******************************************************************************
* HSmode Payload部　復号処理
*  arPayloadLen : 実データのbyte数
*  arBchFrameNum : 実データ+BCHを1フレームとして、その繰り返し数
*  arCrcFrameNum : arBchFrameNum ? arCrcFrameNumごとにCRC付加
*  arHeaderNum : Headerのbyte数　bch,crc含む
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
	
	for( wkLoop1st = 0U; wkLoop1st < arCrcFrameNum; wkLoop1st++ )		/* 10計測dataごとにCRC付加 最大10ブロック(計100data) */
	{
		/* CRCチェック */
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
			/* エラーなし Ack応答 */
//			vrfTest++;
		}
		else
		{
			/* エラーあり エラー訂正処理 */
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
			/* CRCチェック */
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
				/* エラー訂正成功 Ack応答 */
//				vrfTest++;
//				vrfCorrectSetCnt++;
			}else
			{
				/* 通信失敗 パケット破棄 Nack応答 */
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
* HSmodeヘッダ部　符号化処理
*　ステータス通信以外のヘッダ処理
*  arLength : CRCを含みBCHを除く
*******************************************************************************
*/
void SubRfDrv_Header_Cording( uint8_t arLength, uint8_t arBchOption )
{
	uint16_t	wkDataCount;
	uint16_t	wkResult;

	/* CRCを含めたデータ長のチェック */
	/* CRC算出 */
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
	
	/* データの4,5byte目にCRC追加 */
	vutRfDataBuffer.umData[3U] = (uint8_t)( (wkResult >> 8U) & 0x00FF );
	vutRfDataBuffer.umData[4U] = (uint8_t) wkResult;

	if( arBchOption == BCH_ON )
	{
		SubRfDrv_Coding_Bch( &vutRfDataBuffer.umData[0U], arLength, arLength );
	}
}

/*
*******************************************************************************
* HSmode Payload部　符号化処理
*  arPayloadLen : 実データのbyte数
*  arBchFrameNum : 実データ+BCHを1フレームとして、その繰り返し数
*  arCrcFrameNum : arBchFrameNum  arCrcFrameNumごとにCRC付加
*  arHeaderNum : Headerのbyte数　bch,crc含む
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
	
	for( wkLoop1st = 0U; wkLoop1st < arCrcFrameNum; wkLoop1st++ )		/* 10計測dataごとにCRC付加 最大10ブロック(計100data) */
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
 *	PN9送信関数
 *	[内容]
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
	/* CHセットアップが必要 */
	SubSX1272Write( REG_LR_FIFOTXBASEADDR, 0x00 );				// Full buffer used for Tx
	SubSX1272Write( REG_LR_FIFOADDRPTR, 0x00 );
	SubSX1272WriteFifo( (uint8_t *)PN9, 64U );
//	SubSX1272WriteFifo( &PN9[0U], 64U );
	SubSX1272Tx( 64U );
}
#endif

/*
*******************************************************************************
*	SX1272 Sleep遷移
*
*	[内容]
*		SX1272をSpeelさせ低消費電流状態にする。
*	FSKモードに遷移。レジスタは保持される。
*******************************************************************************
*/
void SubSX1272Sleep(void){

//	uint16_t wkTest = 0U;
//	uint8_t wkModeReady = 0U;
	
	/* Sleep Mode 遷移 */
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
*	SX1272 FSKモードStdby遷移
*
*	[内容]
*		SX1272をFSKモードでスタンバイ状態に遷移。
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
*	SX1272 Tx遷移
*
*	[内容]
*		SX1272をFSK/LoRaモードで送信状態に遷移。
*	Txモード,RF_SWをON,データ長セット。
* [引数]
*	FSKパケット長　wkDataLength　1〜2047　※受信側のRAM容量にも依存
*******************************************************************************
*/
void SubSX1272Tx( uint16_t wkDataLength ){
	
//	uint8_t wkModeReady = 0U;
	
	if( rfStatus.Modem == MODEM_FSK )
	{
		/* 引数範囲外の場合はLength設定スキップ */
		if( (wkDataLength > 0U) && (wkDataLength < 2048U) )
		{
			/* Length設定 */
			SubSX1272Write( REG_PAYLOADLENGTH , (uint8_t)(wkDataLength & 0x00FF) );
			SubSX1272Write(
				REG_PACKETCONFIG2 ,
				RF_PACKETCONFIG2_DATAMODE_PACKET
				| RF_PACKETCONFIG2_IOHOME_OFF
				| RF_PACKETCONFIG2_BEACON_OFF
				| (uint8_t)((wkDataLength >> 8U) & 0x0007)
			);/* 下位3bitがLength */
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
			/* Length設定 */
			SubSX1272Write( REG_LR_PAYLOADLENGTH, wkDataLength );
		}
		SubSX1272Write( REG_LR_OPMODE ,
			RF_OPMODE_LONGRANGEMODE_ON |
			RFLR_OPMODE_ACCESSSHAREDREG_DISABLE |
			RFLR_OPMODE_TRANSMITTER);
	}
	
	RADIO_SW = RF_SW_ON;
	
	RTCMK = 0U;				/* キャリアセンス終了後のRTC割り込み解除 */
}


/*
 *******************************************************************************
 *	SX1272 1アドレス書き込み関数
 *
 *	[内容]
 *		1データを指定アドレスにWrite
 *******************************************************************************
 */
void SubSX1272Write( uint8_t addr, uint8_t tx_data )
{
	SubSX1272WriteBuffer( addr, &tx_data, 1U );
}


/*
 *******************************************************************************
 *	SX1272 1アドレス読み込み関数
 *
 *	[内容]
 *		1データを指定アドレスaddrからRead / uint8でReturn
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
 *	SX1272 シーケンシャルWrite関数
 *
 *	[内容]
 *		指定アドレスからサイズ分の連続書き込み
 *******************************************************************************
 */
void SubSX1272WriteFifo( uint8_t *tx_buffer, uint8_t size )
{
	/* FIFOアドレス0x00 */
	SubSX1272WriteBuffer( REG_FIFO, tx_buffer, size );
}


/*
 *******************************************************************************
 *	SX1272 Write関数
 *
 *	[内容]
 *		指定アドレスからサイズ分の配列データ書き込み
 *		上位関数：SX1272Write / SubSX1272WriteFifo
 *******************************************************************************
 */
void SubSX1272WriteBuffer( uint8_t addr, uint8_t *tx_buffer, uint8_t size )
{
	uint8_t rx_dummy;										/* Write時の受信データ 前回設定値Read */
	uint8_t wkStatus;
	uint8_t wkLoopCount;
	
	addr |= 0x80U;										/* Writeフラグset */
	
	/* NSS = 0; */
	RADIO_NSS = 0U;
	
	NOP();
	
	wkStatus = R_CSI10_Send_Receive( &addr , 1 , &rx_dummy );					/* アドレスwrite */
	
	switch( wkStatus )
	{
		case MD_OK :									/* 正常処理 */
			for ( wkLoopCount = 1 ; wkLoopCount <= size ; wkLoopCount ++ )
			{
				R_CSI10_Send_Receive( tx_buffer , 1 , &rx_dummy);			/* 1データの送受 */
				
				/* 送信完了フラグ待ち */
#if 0
				while( (gvCsi10SndEndFlg != imON) | (SSR10 & 0x0040) )
#endif
				while( SSR10 & 0x0060 )
				{
					NOP();
				}
				tx_buffer ++;								/* rx_bufferは破棄(未処理) */
			}
			break;
		default:										/* エラールーチン */
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
 *	SX1272 Read関数
 *
 *	[内容]
 *		指定アドレスからサイズ分Readし、配列データ渡し
 *		上位関数：SX1272Read
 *******************************************************************************
 */
void SubSX1272ReadBuffer( uint8_t addr, uint8_t *rx_buffer, uint8_t size )
{
	uint8_t tx_dummy;										/* Read時のダミー送信データ */
	uint8_t wkStatus;
	uint8_t wkLoopCount;
	
	addr &= 0x7FU;										/* Readフラグset */
	
	/* NSS = 0; */
	RADIO_NSS = 0U;
	
	NOP();
	NOP();
	NOP();
	
	wkStatus = R_CSI10_Send_Receive( &addr , 1U , rx_buffer );					/* アドレスwrite */
	
	switch( wkStatus )
	{
		case MD_OK :									/* 正常処理 */
			for ( wkLoopCount = 1 ; wkLoopCount <= size ; wkLoopCount ++ )
			{
				R_CSI10_Send_Receive( &tx_dummy , 1U , rx_buffer);			/* 1データの送受 */

#if 0
				while( (gvCsi10SndEndFlg != imON) | (SSR10 & 0x0040) )
#endif
				while( SSR10 & 0x0060 )
				{
					/* 送信受信完了フラグ待ち */
					NOP();
				}
				rx_buffer ++;								/* tx_bufferは破棄(未処理) */
			}
			break;
		default:										/* エラールーチン */
			NOP();
			break;
	}
	
	RADIO_NSS = 1U;
	NOP();
}


/*
 *******************************************************************************
 *	SX1272 変調モード設定
 *
 *	[内容]
 *		RadioRegsInitのModem内容に従ってFSK / Loraモードの設定を行う
 *		Sleepに遷移、他のレジスタは保持
 *******************************************************************************
 */
void SubSX1272SetModem( RadioModems_t modem )
{
	switch( modem )
	{
		default:
		case MODEM_FSK:
			SubSX1272Write( REG_OPMODE, ( SubSX1272Read( REG_OPMODE ) & RF_OPMODE_MASK ) | RF_OPMODE_SLEEP );	/* 動作モード以外を保持してスリープする */
			SubSX1272Write( REG_OPMODE, RFLR_OPMODE_LONGRANGEMODE_OFF | RF_OPMODE_MODULATIONTYPE_FSK | RF_OPMODE_MODULATIONSHAPING_11 | RF_OPMODE_SLEEP );
			break;
			
		case MODEM_LORA:
			SubSX1272Write( REG_OPMODE, RFLR_OPMODE_LONGRANGEMODE_ON | RFLR_OPMODE_ACCESSSHAREDREG_DISABLE | RFLR_OPMODE_SLEEP );
			break;
	}
}

#if 0
/*****************************************************************************
	[ﾓｼﾞｭｰﾙ名]	SubSX1272RegReadAll
	  [日本名]	レジスタリード処理
		[概要]	〃
		[機能]	〃
		[入力]	なし
		[出力]	なし
		[備考]	なし
		[上位]
		[下位]
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
*  SX1272 周波数設定
*
*  [内容]
*	 周波数の設定を行う。
*  LoRa / FSK 共通(レジスタ共通)
*  [引数]
*	 周波数ch番号(24-61) BW200kHz / 62-77ch BW100kHz
*	 周波数調整Freq_Offset 61.035[Hz/Count]
********************************************************************************
*/
void SubSX1272FreqSet( uint8_t ChNo, sint16_t Freq_Offset)
{
	uint32_t wkReg32;
	uint8_t wkReg8;
	
	/* 920.6MHz -> 24ch  LoRa FSKモード共通 */
	wkReg32 = (uint32_t)RFLR_FRFMSB_920_6_MHZ << 16U;
	wkReg32 = wkReg32 + (((uint32_t)RFLR_FRFMID_920_6_MHZ) << 8U);
	wkReg32 = wkReg32 + (uint32_t)RFLR_FRFLSB_920_6_MHZ;
	
	/* 62chから100kHz単位 62chは61ch+150kHz */
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
	
	wkReg32 += (int32_t)(16384 * (int32_t)(ChNo -24) / 5);			// 2^19 / 32MHz * ChBw(0.2MHz) * ΔChNo
	/* offset */
	/* 調整chから離れると水晶振動子の偏差分ズレが生じるが無視できる範囲 */
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
*	SX1272 FSKモード 受信時DIOセット
*
*	[内容]
*		FSKモードでの受信レジスタセット。
*	DIOマッピング、FIFOスレッショルド値設定
* [引数]
*	受信パケット長 wkDataLength 1〜2047  ※ただしwkFIFODataの配列数に依存
*	受信タイムアウト時間　wkTimeOut　1〜65,000	単位msec
*******************************************************************************
*/
void SubSX1272RxFSK_DIO( void ){
	/*
	*  DIO0=PayloadReady / RADIO_DIO_0,回路図はDIO1
	*  DIO1=FifoLevel / RF_FIFOTHRESH_FIFOTHRESHOLD_RX = 0x3E (+1)
	*  DIO2=SyncAddr /	R60取り付け Connect
	*  DIO3=FifoEmpty / R63取り外し Non Connect
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
	
	/* DIOMapping バースト書き込み */
	for( wkRadio = 3U; wkRadio < 7U; wkRadio++ )
	{
		SubSX1272Write( RadioRegsBroadcastInit[wkRadio].Addr, RadioRegsBroadcastInit[wkRadio].Value );
	}

}


/*
*******************************************************************************
*	SX1272 FSKモード キャリアセンスDIOセット
*
*	[内容]
*		FSKモードでのキャリアセンス時のレジスタセット。
*		DIO4_11 / RF_DIOMAPPING2_MAP_RSSI
* [引数]
*
*******************************************************************************
*/
void SubSX1272CarrierSenseFSK_DIO( void )
{
	/*
	*  DIO0=PayloadReady / RADIO_DIO_0,回路図はDIO1
	*  DIO1=FifoLevel / RF_FIFOTHRESH_FIFOTHRESHOLD_RX = 0x3E (+1)
	*  DIO2=SyncAddr /	R60取り付け Connect
	*  DIO3=FifoEmpty / R63取り外し Non Connect
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
*	SX1272 FSKモード 受信ON
*
*	[内容]
*		FSKモードで受信状態遷移。RF_SWをON
*	SubSX1272RxFSK_DIO,SubSX1272FSK_Lengthでの設定依存
* [引数]
*
*******************************************************************************
*/
void SubSX1272RxFSK_On( void )
{
	/* RF_OPMODE_SYNTHESIZER_RXの完了待ちをする場合はTIMEOUT設定に依存するので注意 */
	
	RADIO_SW = RF_SW_ON;
	
	SubSX1272Write( REG_OPMODE ,
		RF_OPMODE_LONGRANGEMODE_OFF |
		RF_OPMODE_MODULATIONTYPE_FSK |
		RF_OPMODE_MODULATIONSHAPING_11 |
		RF_OPMODE_RECEIVER);
		
}


/*
*******************************************************************************
*	SX1272 FSKモード 受信Length,Syncセット
*
*	[内容]
*		FSKモードでの受信レジスタセット。
*	Length設定、SyncWord設定
* [引数]
*	受信パケット長 arDataLength 1〜2047
*******************************************************************************
*/
void SubSX1272FSK_Length( uint16_t arDataLength )
{
	/* Length設定 */
	SubSX1272Write( REG_PAYLOADLENGTH , (uint8_t)(arDataLength & 0x00FF) );
	SubSX1272Write(
		REG_PACKETCONFIG2 ,
		RF_PACKETCONFIG2_DATAMODE_PACKET
		| RF_PACKETCONFIG2_IOHOME_OFF
		| RF_PACKETCONFIG2_BEACON_OFF
		| (uint8_t)((arDataLength >> 8U) & 0x0007)
	);/* 下位3bitがLength */
}


/*
*******************************************************************************
*	SX1272 FSKモードRx遷移
*
*	[内容]
*		SX1272をFSKモードで受信状態に遷移。
*		事前に受信タイムアウト用のTAUを起動させること。
*		さらに事前にDIO設定し、RXを起動させること。
*		RSSI値取得する(vrfRssi)
* [引数]
*	受信パケット長 arDataLength 1〜2047  【削除】※ただしwkFIFODataの配列数に依存
*	受信データはポインタ受け
*	【削除】受信タイムアウト時間　wkTimeOut　1〜65,000	単位msec
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
	*  DIO0=PayloadReady / RADIO_DIO_0,回路図はDIO1
	*  DIO1=FifoLevel / RF_FIFOTHRESH_FIFOTHRESHOLD_RX = 0x3E (+1)
	*  DIO2=SyncAddr /	R60取り付け Connect
	*  DIO3=FifoEmpty / R63取り外し Non Connect
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
	
	/*-------------- 受信中のFIFO溢れ処理 ------------------*/
	/* PayloadReady中のオーバーフロー監視 */
//	SubIntervalTimer_Sleep( 50U * 375U, 1U );
	vrfCurrentRssi = SubSX1272Read( REG_RSSIVALUE );	/* 空読み */
	
	while( (!gvRfIntFlg.mRadioDio0) && (wkRadio < arDataLength) )
	{
//		M_HALT;
		/* FIFO Thrsh判定で読み込み(実際は+1) 0x3E(+1) */
		if( gvRfIntFlg.mRadioDio1 )
		{
//			vrfDebug[wkDebug] =  SubSX1272Read( REG_IRQFLAGS2 ) | 0x0100;
//			vrfDebugCntL[wkDebug] = gvRfTimingCounterL;
			wkDebug++;
#if swLoggerBord == imEnable
			if( vRfDrvMainLoggerSts == ecRfDrvMainSts_HSmode_BeaconResAckRxStart )
			{
				/* HSmodeでのAckはデータバッファの上書き回避 */
				SubSX1272ReadBuffer( REG_FIFO, &vrfHsmodeAckBuff.umData[wkRadio], RF_FIFOTHRESH_FIFOTHRESHOLD_RX - 1U );
			}
			else
			{
				SubSX1272ReadBuffer( REG_FIFO, &vutRfDataBuffer.umData[wkRadio], RF_FIFOTHRESH_FIFOTHRESHOLD_RX - 1U );
			}
#else
			if( vRfDrvMainGwSts == ecRfDrvMainSts_HSmode_Lg_To_Gw_DataAckRx )
			{
				/* HSmodeでのAckはデータバッファの上書き回避 */
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
			/* 長パケット受信中は時間延長 */
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
					/* 再タイムアウト */
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
		/* FIFO蓄積の待ち時間 */
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
	/* RX終了 / FIFO残あり */
	SubSX1272Stdby();
	
	/* DIO2をFifoEmptyに変更 */
	SubSX1272Write( REG_DIOMAPPING1, (SubSX1272Read( REG_DIOMAPPING1 ) & RF_DIOMAPPING1_DIO1_MASK) | RF_DIOMAPPING1_DIO1_01 );
	gvRfIntFlg.mRadioDio1 = 0U;
	/* FIFO残の読み出し */
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
	
	/* RADIO_DIO_1成立後の1byte読み出し */
/* 配列は30バイトまで */
	
	/* DIOをもとに戻す */
	SubSX1272RxFSK_DIO();
	R_INTC6_Stop();
	R_INTC4_Stop();
	R_INTC3_Stop();
	R_INTC1_Stop();
}


/*
*******************************************************************************
*	SX1272 FSKモード送信動作
*
*	[内容]
*		SX1272をFSKモードで送信。vutRfDataBufferからFIFOに転送しながら送信。
*	事前にvutRfDataBufferへデータセットすること
*	周波数、Preamble,Syncwordもセット済みであること
*	送信成功時はTUAはSTOPしない。
* [引数]
*	パケット長 arDataLength 1〜2047
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
		/* 計測値等のackデータは64byte以下となるためvrfHsmodeAckBuffは除く */
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
		/*FIFO充填*/
		gvRfIntFlg.mRadioDio1 = 0U;			/* FifoLevel */
		R_INTC4_Start();
		
		while( !gvRfIntFlg.mRadioDio1 && !gvRfIntFlg.mRadioTimer && !gvRfIntFlg.mRadioDio0 )
		{
			M_HALT;
		}
		R_INTC4_Stop();
		if( gvRfIntFlg.mRadioTimer )
		{
			/* 送信時間タイムアウト */
			SubSX1272Sleep();
			wkHsmodeDataPointer = arDataLength;
			wkReturn = FAIL;
		}else if( gvRfIntFlg.mRadioDio0 )
		{
			/* 送信完了(入らないはず) */
			SubSX1272Sleep();
//			SubIntervalTimer_Stop();
			wkHsmodeDataPointer = arDataLength;
			wkReturn = FAIL;
		}else
		{
			/* FIFO THフラグ */
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
	/* 送信完了待ち */
	while( !gvRfIntFlg.mRadioTimer && !gvRfIntFlg.mRadioDio0 )
	{
		M_HALT;
	}
	if( gvRfIntFlg.mRadioDio0 )
	{
		/* 送信完了 */
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
		/* 送信タイムアウト */
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
*	SX1272 FSKモード DIO、周波数偏差、ビットレート 250kbps設定
*
*	[内容]
*		SX1272 Broadcast時のDIOを設定する。FSKモード
*		 DIO0=PacketSent / RADIO_DIO_0 => 回路図はDIO1 INT6
*		 DIO1=FifoLevel INT4
*		 DIO2=FifoFull / R60取り付け Connect
*		 DIO3=FifoEmpty / R63取り外し Non Connect
*		 DIO4=LowBat
*		 DIO5=ModeReady / Non Connect
*	周波数偏差500kHz　ビットレート250kbps
*******************************************************************************
*/
void SubSX1272TxFSK_DIO_240kbps(void)
{
	uint8_t wkRadio;
	
	/* DIOMapping バースト書き込み */
	for( wkRadio = 0U; wkRadio < sizeof( RadioRegsBroadcastInit ) / sizeof( RadioRegisters_t ); wkRadio++ )
	{
		SubSX1272Write( RadioRegsBroadcastInit[wkRadio].Addr, RadioRegsBroadcastInit[wkRadio].Value );
	}
}


/*
********************************************************************************
* SX1272 FSKモード SyncWord設定
*
* [内容]
* Syncword 8byteの設定
*
********************************************************************************
*/
void SubSX1272FSK_Sync( uint8_t *wkSyncWord )
{
	uint8_t wkRadio;
	
	SubSX1272Write( REG_SYNCCONFIG, (RF_SYNCCONFIG_AUTORESTARTRXMODE_WAITPLL_ON | RF_SYNCCONFIG_PREAMBLEPOLARITY_AA | RF_SYNCCONFIG_SYNC_ON | RF_SYNCCONFIG_FIFOFILLCONDITION_AUTO | (RF_FSK_SYNCWORD_LENGTH_FSK - 1U)));
	/* REG_SYNCVALUE1 - REG_SYNCVALUE8へ書き込み */
	for( wkRadio = 0U; wkRadio < RF_FSK_SYNCWORD_LENGTH_FSK; wkRadio++ )
	{
		SubSX1272Write( REG_SYNCVALUE1 + wkRadio, wkSyncWord[ wkRadio ] );
	}
}


/*
********************************************************************************
* SX1272 FSKモード Preamble設定
*
* [内容]
* Preamble長 32byteの設定
*　※SX1272は0XFFFFまで設定可で4symbolオーバーヘッド
*
********************************************************************************
*/
void SubSX1272FSK_Preamble( uint16_t wkPreambleLength )
{
//	uint8_t wkRadio;
	
	/* REG_SYNCVALUE1 - REG_SYNCVALUE8へ書き込み */
	SubSX1272Write( REG_PREAMBLEMSB, (uint8_t) (wkPreambleLength >> 8U) );
	SubSX1272Write( REG_PREAMBLELSB, (uint8_t) wkPreambleLength );
}


/*
*******************************************************************************
*	SX1272 LoRaモード 送信DIOセット
*
*	[内容]
*		LoRaモードでの送信時のレジスタセット。
*
* [引数]
*
*******************************************************************************
*/
void SubSX1272TxLoRa_DIO( void )
{
	/*
	*  DIO0=TxDone / RADIO_DIO_0,回路図はDIO1
	*  DIO1=不定
	*  DIO2=FhssChageChanel /  R60取り付け Connect
	*  DIO3=CadDone / R63取り外し Non Connect
	*/
	
	SubSX1272Write( REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00 );

}


/*
*******************************************************************************
*	SX1272 LoRaモード 受信DIOセット
*
*	[内容]
*		LoRaモードでの受信時のレジスタセット。
*
* [引数]
*
*******************************************************************************
*/
void SubSX1272RxLoRa_DIO( void )
{
	/*
	*  DIO0=RxDone / RADIO_DIO_0,回路図はDIO1
	*  DIO1=RxTimeout / RF_FIFOTHRESH_FIFOTHRESHOLD_RX = 0x3E (+1)
	*  DIO2=FhssChageChanel /  R60取り付け Connect
	*  DIO3=CadDone / R63取り外し Non Connect
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
*	SX1272 LoRaモード CAD DIOセット
*
*	[内容]
*		LoRaモードでのCAD時のレジスタセット。
*
* [引数]
*
*******************************************************************************
*/
void SubSX1272CadLoRa_DIO( void )
{
	/*
	*  DIO0=CadDone / RADIO_DIO_0,回路図はDIO1
	*  DIO1=RxTimeout / RF_FIFOTHRESH_FIFOTHRESHOLD_RX = 0x3E (+1)
	*  DIO2=FhssChageChanel /  R60取り付け Connect
	*  DIO3=CadDone / R63取り外し Non Connect
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
*	SX1272 LoRaモード CAD起動
*
*	[内容]
*		LoRaモードでのCAD起動
*	周波数設定は行わない
*	mRadioDio0:CAD Done
*	mRadioDio4:CAD Detect
*	mRadioTimer:CAD起動タイムアウト
* [引数]
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
*	SX1272 LoRaモード Txレジスタ設定
*
*	[内容]
*		LoRaモードでの送信レジスタ設定(周波数設定なし)
*
* [引数]
*　　ペイロード長/プリアンブル長/送信する符号化(1/4or3/4)
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
*	SX1272 LoRaモード Rx実行
*
*	[内容]
*		LoRaモードでの受信レジスタ設定(周波数設定なし)し、受信を開始する
*		mRadioDio0:受信完了
* [引数]
*　　ペイロード長/プリアンブル長/送信する符号化(1/4or3/4)
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
	wkLoopCounter = 20U;			/* 実測4〜5Loop */
	wkRfFuncWork = 0U;
	while( (wkRfFuncWork != 0x04U) && (wkLoopCounter > 0U) )
	{
		wkLoopCounter--;
		wkRfFuncWork = SubSX1272Read( REG_LR_MODEMSTAT );
	}
}

/*
*******************************************************************************
*  RSSI値を取得し返す
*  [概要]
*  RSSI値をdBm単位(絶対値)で返す。FSK/LoraをrfStatus.Modemで判断
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
*	SX1272 LoRaモード 受信データ読み込み
*
*	[内容]
*		LoRaモードでの受信データを読み込む
*		vutRfDataBuffer[0U]からデータ配置
*	Sleepでデータが消えるためSleepせずに抜ける
* [引数]
*	受信データ数
*	RSSI値
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
*	SX1272 LoRaモード 受信 連続モード
*
*	[内容]
*		LoRaモードでの連続受信時のレジスタセット。
*
* [引数]
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
 *	ブロードキャストループカウント取得処理
 *
 *	[内容]
 *		ブロードキャストループカウントをデクリメントして取得する。
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
 *	RF(無線通信) 初期化ステータス取得処理
 *
 *	[内容]
 *		RF(無線通信)の初期化ステータスを取得する。
 *******************************************************************************
*/
ET_RFDrvInitSts_t ApiRFDrv_GetInitSts( void )
{
	return vRFPrm.mInitSts;
}


/*
 *******************************************************************************
 *	無線スリープループカウント取得処理
 *
 *	[内容]
 *		無線スリープループカウントをデクリメントして取得する。
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
 *	無線用プロトコルに測定値、警報値を書き込む
 *
 *	[内容]
 *		無線用プロトコルに測定値、警報値を書き込む
 *	[引数]
 *		uint8_t	*arFromAlmParmArray: 警報値
 *		uint8_t	*arToAlmParmArray: 無線側のプロトコル
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
 *	無線用プロトコルに警報遅延値を書き込む
 *
 *	[内容]
 *		無線用プロトコルに警報遅延値を書き込む
 *	[引数]
 *		uint8_t	*arFromAlmParmArray: 警報遅延値
 *		uint8_t	*arToAlmParmArray: 無線側のプロトコル
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
 *	無線用プロトコルに逸脱許容時間を書き込む
 *
 *	[内容]
 *		無線用プロトコルに警報遅延値を書き込む
 *	[引数]
 *		uint8_t	*arToAlmParmArray: 逸脱許容時間
 *******************************************************************************
 */
static void SubRfDrv_SetDevi( uint8_t *arToAlmParmArray )
{
	
	/* 逸脱許容時間有効選択 */
	arToAlmParmArray[ 0U ] = (gvInFlash.mParam.mDeviEnaLv[ 0U ] & 0x10) >> 4U;
	arToAlmParmArray[ 0U ] |= (gvInFlash.mParam.mDeviEnaLv[ 1U ] & 0x10) >> 3U;
	arToAlmParmArray[ 0U ] |= (gvInFlash.mParam.mDeviEnaLv[ 2U ] & 0x10) >> 2U;
	arToAlmParmArray[ 0U ] |= (gvInFlash.mParam.mDeviEnaLv[ 3U ] & 0x10) >> 1U;
	
	/* 許容時間 */
	arToAlmParmArray[ 0U ] |= (uint8_t)((gvInFlash.mParam.mDeviEnaTime[ 0U ] & 0x000F) << 4U);
	arToAlmParmArray[ 1U ] = (uint8_t)((gvInFlash.mParam.mDeviEnaTime[ 0U ] & 0x0FF0) >> 4U);
	
	/* 逸脱許容時間レベル選択 */
	arToAlmParmArray[ 2U ] = gvInFlash.mParam.mDeviEnaLv[ 0U ] & 0x03;
	arToAlmParmArray[ 2U ] |= (gvInFlash.mParam.mDeviEnaLv[ 1U ] & 0x03) << 2U;
	arToAlmParmArray[ 2U ] |= (gvInFlash.mParam.mDeviEnaLv[ 2U ] & 0x03) << 4U;
	arToAlmParmArray[ 2U ] |= (gvInFlash.mParam.mDeviEnaLv[ 3U ] & 0x03) << 6U;
	
	/* クリア契機(分) */
	arToAlmParmArray[ 3U ] = (gvInFlash.mParam.mDeviClrTimeMin & 0x07) << 4U;
	/* クリア契機(時) */
	arToAlmParmArray[ 3U ] |= (gvInFlash.mParam.mDeviClrTimeHour & 0x01) << 7U;
	arToAlmParmArray[ 3U ] |= (gvInFlash.mParam.mDeviClrTimeHour & 0x1E) >> 1U;

}
#endif

#if (swKouteiMode == imEnable)
/*
 *******************************************************************************
 * 無線送信チャネル設定
 *
 *	[内容]
 *		無線送信チャネルを設定する。
 *	[引数]
 *		uint16_t		arVal: チャネル24〜77
 *		ET_SelRfCh_t	arSel: 無線種類
 *	[戻値]
 *		なし
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
 * 無線送信チャネル読み出し
 *
 *	[内容]
 *		無線送信チャネルを読み出す。
 *	[引数]
 *		ET_SelRfCh_t: 無線種類
 *	[戻値]
 *		uint16_t	wkVal: チャネル24〜77
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
 * 高速モード:データ要求コマンド設定
 *
 *	[内容]
 *		高速モードデータ要求コマンドを設定する
 *	[引数]
 *		ET_RfDrv_GwReqCmd_t arCmd: 要求コマンド
 *	[戻値]
 *		なし
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
 * 高速モード:ログインユーザID設定
 *
 *	[内容]
 *		高速モードログインユーザIDを設定する
 *	[引数]
 *		ET_RfDrv_GwReqCmd_t arCmd: 要求コマンド
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiRfDrv_SetHSLoginUserId( uint16_t arUserId )
{
	vrfHsLoginUserId = arUserId;
}

/*
 *******************************************************************************
 * 高速モード:呼出ロガー設定
 *
 *	[内容]
 *		高速モード呼出ロガーIDを設定する。
 *	[引数]
 *		uint16_t	arId: ID_0x00~0xFF
 *		uint8_t		arRefno: リファレンス番号0~2
 *	[戻値]
 *		なし
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
 * 高速モード:呼出ロガー読み出し
 *
 *	[内容]
 *		高速モード呼出ロガーIDを読み出す
 *	[引数]
 *		uint8_t		arRefno: リファレンス番号0~2
 *	[戻値]
 *		uint16_t	vrfHSLoggerID[arRefno]: ID_0x00~0xFF
 *******************************************************************************
 */
uint16_t ApiRfDrv_GetHSLoggerID( uint8_t arRefno )
{
	return vrfHSLoggerID[ arRefno ];
}


/*
 *******************************************************************************
 * 高速モード：要求データ先頭時系列ID
 *
 *	[内容]
 *		高速モードの要求データ先頭時系列ID(IndexNo.)を設定する。
 *	[引数]
 *		uint16_t	arIndex: 先頭時系列ID
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiRfDrv_SetHSIndexNo( uint16_t arIndex )
{
	vrfHSReqHeadMeaID = arIndex;
}


/*
 *******************************************************************************
 * 高速モード：要求データ先頭時刻
 *
 *	[内容]
 *		高速モードの要求データ先頭時刻を設定する。
 *		uint16_t	arHeadMeaTime: 先頭時刻
 *		uint8_t		arRefno: リファレンス番号0~2
 *	[戻値]
 *		なし
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
 *	無線RSSI読み出し
 *
 *	[内容]
 *		無線RSSIを読み出す。
 *	[引数]
 *		なし
 *	[戻値]
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
 *	リアルタイム接続情報セット
 *
 *	[内容]
 *		リアルタイム接続情報セット
 *	[引数]
 *		uint8_t	arSel: 0：未接続、1：接続
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void SubRfDrv_SetRtConInfo( uint8_t arSel )
{
	if( arSel == imON )
	{
		if( vrfConnectSuccess == 0U )
		{
			gvModuleSts.mLcd = ecLcdModuleSts_Run;		/* LCD表示更新 */
			ApiFlash_WriteSysLog( ecSysLogItm_RealTmCom, 0U );		/* システム履歴：長距離通信接続 */
		}
		vrfConnectSuccess = 1U;
	}
	else
	{
		if( vrfConnectSuccess == 1U )
		{
			gvModuleSts.mLcd = ecLcdModuleSts_Run;		/* LCD表示更新 */
			ApiTimeComp_TimeCompFlgClr();				/* 時計補正フラグ無効 */
			ApiFlash_WriteSysLog( ecSysLogItm_RealTmDisCom, 0U );	/* システム履歴：長距離通信切断 */
		}
//		vrfConnectSuccess = 0U;
		vrfRTmodeCommFailNum = vrfRTmodeCommCutTime;	/* 強制終了 */
		
	}
}
#endif


#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	リアルタイム接続情報読み出し
 *
 *	[内容]
 *		リアルタイム接続情報読み出し
 *	[引数]
 *		なし
 *	[戻値]
 *		uint8_t	vrfConnectSuccess: 0：未接続、1：接続
 *******************************************************************************
 */
uint8_t ApiRfDrv_GetRtConInfo( void )
{
//	return vrfConnectSuccess;
	return ( vrfConnectSuccess + gvrfStsEventFlag.mOnlineLcdOnOff );			/* 2022.9.16 通信開始時にLCD点灯 */
}
#endif

#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	高速通信接続表示
 *
 *	[内容]
 *		高速通信接続表示
 *	[引数]
 *		uint8_t	arSel: 0：未接続、1：接続、3：読み出し
 *	[戻値]
 *		uint8_t	vrfHsStsDisp: 0：未接続、1：接続
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
				gvModuleSts.mLcd = ecLcdModuleSts_Run;		/* LCD表示更新 */
			}
			vrfHsStsDisp = imOFF;
			break;
		case 1U:
			if( vrfHsStsDisp == imOFF )
			{
				gvModuleSts.mLcd = ecLcdModuleSts_Run;		/* LCD表示更新 */
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
 * 工程用無線受信実行
 *
 *	[内容]
 *		工程用の無線受信を実行する
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
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
		SubSX1272FreqSet( rfCurrentCh_LoRa, rfFreqOffset );			/* 周波数設定:LoRa */
		rfStatus.Modem = MODEM_LORA;
	}
	else
	{
		SubSX1272FreqSet( rfCurrentCh_FSK, rfFreqOffset );			/* 周波数設定:FSK */
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
	
	ApiRfDrv_GetSetRssiCareerFlg( imCarrerFlgWrite, wkCarrerFlg );	/* キャリアセンス判定値保存 */
	ApiRfDrv_GetSetRssiCareerFlg( imRssiWrite, wkRadio );			/* RSSI値保存 */
	
	SubIntervalTimer_Sleep( 0xFFFF, 2U );
	gvModuleSts.mRf = ecRfModuleSts_Sleep;
}
#endif

#if (swKouteiMode == imEnable)
/*
****************************************************************************************************
*  受信電流測定
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
 * 無線受信テストのRSSI、キャリアセンス判定の読み書き
 *
 *	[内容]
 *		無線受信テストのRSSI、キャリアセンス判定の読み書きを行う
 *	[引数]
 *		uint8_t	arSel	: 	0…RSSIの書込み、1…RSSIの読込み
 *							2…キャリアセンス判定値書込み、3…キャリアセンス判定値読込み
 *		uint8_t	arVal	: RSSI値 or キャリアセンス判定値
 *	[戻値]
 *		uint8_t			: RSSI値 or キャリアセンス判定値
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
 *	長距離通信転送データ起点による送信ポインタの変更
 *
 *	[内容]
 *		長距離通信転送データ起点をGW接続時に受信し、送信ポインタの変更する
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
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
	
	
	/* イベント有効から送信 */
	if( gvInFlash.mParam.mRealTimeSndPt == 510U )
	{
		/* イベント有効ポインタが存在するなら */
		if( gvInFlash.mData.mMeas1_EvtPtrIndex != 0U )
		{
			gvInFlash.mData.mMeas1_OldPtrIndex = gvInFlash.mData.mMeas1_EvtPtrIndex;		/* 過去送信ポインタにイベント有効ポインタを代入 */
//			gvInFlash.mData.mMeas1_EvtPtrIndex = 0U;										/* イベント有効ポインタをクリア */
		}
		else
		{
			/* 過去送信ポインタから送信 */
		}
	}
	/* 現在の10〜5000データ前から送信 */
	else if( gvInFlash.mParam.mRealTimeSndPt >= 1U && gvInFlash.mParam.mRealTimeSndPt <= 500U )
	{
		wkRealTimeSndPt = gvInFlash.mParam.mRealTimeSndPt * 10U;							/* 設定値1〜500データを10〜5000データとして扱う */
	
#if 1
		if( gvInFlash.mData.mMeas1_Index == 0U ||
			gvInFlash.mData.mMeas1_OldPtrIndex == 0U ||
			gvInFlash.mData.mMeas1_Index == gvInFlash.mData.mMeas1_OldPtrIndex )
		{
			/* 過去送信ポインタから送信 */
		}
		else
		{
			if( gvInFlash.mData.mMeas1_Index > gvInFlash.mData.mMeas1_OldPtrIndex )
			{
				wkU16 = (gvInFlash.mData.mMeas1_Index - 1U) - gvInFlash.mData.mMeas1_OldPtrIndex;
				
				/* 送信数が設定数より大きい */
				if( wkU16 > wkRealTimeSndPt )
				{
					/* 過去送信ポインタは、現在Indexから設定個数分マイナスの位置へ移動 */
					gvInFlash.mData.mMeas1_OldPtrIndex = (gvInFlash.mData.mMeas1_Index - 1U) - wkRealTimeSndPt;
				}
				else
				{
					/* 過去送信ポインタから送信 */
				}
			}
			else
			{
				wkU16 = (imIndexMaxMeas1 - 1U) + gvInFlash.mData.mMeas1_Index - gvInFlash.mData.mMeas1_OldPtrIndex;
				
				/* 送信数が設定数より大きい */
				if( wkU16 > wkRealTimeSndPt )
				{
					/* 過去送信ポインタは、現在Indexから設定個数分マイナスの位置へ移動 */
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
					/* 過去送信ポインタから送信 */
				}
			}
		}
#else
		/* 収録データ数が送信要求データ数に足りない */
		if( gvInFlash.mData.mMeas1_Num < wkRealTimeSndPt )
		{
			gvInFlash.mData.mMeas1_OldPtrIndex = 1;											/* 過去送信ポインタを1にする */
		}
		else
		{
			/* 前回接続したGWと異なる */
			if( vPreGwId[ 0U ] != ConGwId[ 0U ] || vPreGwId[ 1U ] != ConGwId[ 1U ] || vPreGwId[ 2U ] != ConGwId[ 2U ] )
			{
				/* 設定個数がIndexNo.のリングバッファをまたがない */
				if( gvInFlash.mData.mMeas1_Index >= wkRealTimeSndPt )
				{
					/* 過去送信ポインタは、現在Indexから設定個数分マイナスの位置へ移動 */
					gvInFlash.mData.mMeas1_OldPtrIndex = (gvInFlash.mData.mMeas1_Index - 1U) - wkRealTimeSndPt;
				}
				else
				{
					/* 過去送信ポインタは、現在Indexから設定個数分マイナスの位置へ移動(IndexNo.のリングバッファまたぐ) */
					gvInFlash.mData.mMeas1_OldPtrIndex = imIndexMaxMeas1 - (wkRealTimeSndPt - gvInFlash.mData.mMeas1_Index - 1U);
				}
			}
			/* 前回接続したGWと同一 */
			else
			{
				if( gvInFlash.mData.mMeas1_Index > gvInFlash.mData.mMeas1_OldPtrIndex )
				{
					wkU16 = (gvInFlash.mData.mMeas1_Index - 1U) - gvInFlash.mData.mMeas1_OldPtrIndex;
					
					/* 送信数が設定数より大きい */
					if( wkU16 > wkRealTimeSndPt )
					{
						/* 過去送信ポインタは、現在Indexから設定個数分マイナスの位置へ移動 */
						gvInFlash.mData.mMeas1_OldPtrIndex = (gvInFlash.mData.mMeas1_Index - 1U) - wkRealTimeSndPt;
					}
					else
					{
						/* 過去送信ポインタから送信 */
					}
				}
				else
				{
					wkU16 = (imIndexMaxMeas1 - 1U) + gvInFlash.mData.mMeas1_Index - gvInFlash.mData.mMeas1_OldPtrIndex;
					
					/* 送信数が設定数より大きい */
					if( wkU16 > wkRealTimeSndPt )
					{
						/* 過去送信ポインタは、現在Indexから設定個数分マイナスの位置へ移動 */
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
						/* 過去送信ポインタから送信 */
					}
				}
			}
		}
#endif
	}
	/* 以前送信した箇所から送信 */
	else
	{
		/* 過去送信ポインタから送信 */
	}
	
#if 0
	/* 接続先GWをメモリ */
	vPreGwId[ 0U ] = ConGwId[ 0U ];
	vPreGwId[ 1U ] = ConGwId[ 1U ];
	vPreGwId[ 2U ] = ConGwId[ 2U ];
#endif
}

/*
 *******************************************************************************
 *	長距離通信データのFlashからリード個数計算
 *
 *	[内容]
 *		長距離通信にて転送するデータをFlashからリードする個数を計算する
 *		計測データ:1〜9データ
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
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
 *	定期送信計測データのFlashからリード
 *
 *	[内容]
 *		リアルタイムモードの定期送信計測データをFlashからリードする
 *	[引数]
 *		uint8_t arSel: MEASURE_DATASET:計測値、MEASALM_DATASET:計測警報履歴
 *	[戻値]
 *		なし
 *******************************************************************************
 */
static void SubRfDrv_RtmodeMeasDataFlashRead( uint8_t arSel )
{
	uint16_t	wkOldPtrIndex;
	uint16_t		wkSndKosu;
	
	ET_Error_t		wkError;
	ET_RegionKind_t	wkRegionKind;
	
	/* 計測値のリード */
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
	/* 計測警報履歴のリード */
	else
	{
		wkOldPtrIndex = gvInFlash.mData.mMeasAlm_OldPtrIndex;
		wkSndKosu = vRtMode_MeasAlmSndKosu;
		wkRegionKind = ecRegionKind_MeasAlm;
	}
	
	/* Flashにリードキューをセット */
	wkError = ApiFlash_ReadQueSet( 0U, 0U, wkOldPtrIndex, wkSndKosu, wkRegionKind, 1U );
	if( wkError == ecERR_OK )
	{
		ApiSetFlash_ReqRdFlg( 1U, wkRegionKind );					/* 外付けフラッシュリード要求フラグセット */
		gvModuleSts.mExFlashRd = ecExFlashRdModuleSts_Run;			/* Flashリード完了判断処理へ遷移 */
	}
	else
	{
		/* キューがいっぱい:debug*/
		M_NOP;
	}

}
#endif


#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	定期送信計測データセット：リアルタイムモード
 *
 *	[内容]
 *		リアルタイムモードの定期送信計測データをセットする
 *	[引数]
 *		uint8_t arSel: MEASURE_DATASET:計測データ、MEASALM_DATASET:計測警報履歴
 *	[戻値]
 *		なし
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
		vutRfDataBuffer.umRT_MeasureTx.mLoggerID[ wkLoop ] = gvInFlash.mProcess.mUniqueID[ wkLoop ];		/* ロガーシリアルID */
		vutRfDataBuffer.umRT_MeasureTx.mGwID[ wkLoop ] = vrfCommTargetID[ wkLoop ];							/* GWシリアルIDと送信過去データ数 */
	}
	vutRfDataBuffer.umRT_MeasureTx.mCompanyID = gvInFlash.mProcess.mOpeCompanyID;
	vutRfDataBuffer.umRT_MeasureTx.mAppID = vrfMySerialID.mAppID[0U];
	vutRfDataBuffer.umRT_MeasureTx.mGwID[2U] &= 0xF0;
	vutRfDataBuffer.umRT_MeasureTx.mGwID[2U] += ( (vrfMySerialID.mAppID[1U] >> 4U) & 0x0F);
	
	wkMeasAlm2 = imOFF;
	/* 計測警報履歴送信なら */
	if( arSel == MEASALM_DATASET )
	{
		/* 警報監視周期2 */
		if( ApiAlarm_ReadMeasAlmFlg() )
		{
			wkMeasAlm2 = imON;						/* 警報監視周期2による計測警報履歴送信 */
		}
	}
	
	/* 異常ステータス、電池残量 */
	ApiFlash_RfRTAbnStatusSet();
	
	/* 計測警報履歴送信(周期2) */
	if( wkMeasAlm2 == imON )
	{
		/* 周期1：3bit:0、周期2：3bit:1 */
		gvRfRTMeasAlm2Data.mRTMeasData_AbnStatus = gvRfRTMeasData.mRTMeasData_AbnStatus;
		vutRfDataBuffer.umRT_MeasureTx.mAbnormalSratus = gvRfRTMeasAlm2Data.mRTMeasData_AbnStatus & 0xC7U;
		vutRfDataBuffer.umRT_MeasureTx.mAbnormalSratus |= 0x08U;
		
		/* 最新データ時刻 */
		memcpy( &vutRfDataBuffer.umRT_MeasureTx.mNewMeasureTime[ 0U ], &gvRfRTMeasAlm2Data.mRTMeasData_MeasTime[ 0U ], 4U );
		
		/* 最新警報フラグ */
		vutRfDataBuffer.umRT_MeasureTx.mNewAlarmFlag[ 0U ] = gvRfRTMeasAlm2Data.mRTMeasData_AlarmFlag[ 0U ];
		vutRfDataBuffer.umRT_MeasureTx.mNewAlarmFlag[ 1U ] = gvRfRTMeasAlm2Data.mRTMeasData_AlarmFlag[ 1U ];
		
		for( wkLoop = 0U ; wkLoop < 5U ; wkLoop++ )
		{
			/* 最新計測値と論理 */
			vutRfDataBuffer.umRT_MeasureTx.mNewMeasureValue[ wkLoop ] = gvRfRTMeasAlm2Data.mRTMeasData_MeasVal[ wkLoop ];
		}
	}
	/* 計測値送信、又は計測警報履歴送信(周期1) */
	else
	{
		/* 異常ステータス情報 */
		vutRfDataBuffer.umRT_MeasureTx.mAbnormalSratus = gvRfRTMeasData.mRTMeasData_AbnStatus;
		
		/* 最新データ時刻 */
		memcpy( &vutRfDataBuffer.umRT_MeasureTx.mNewMeasureTime[ 0U ], &gvRfRTMeasData.mRTMeasData_MeasTime[ 0U ], 4U );
		
		/* 最新警報フラグ */
		vutRfDataBuffer.umRT_MeasureTx.mNewAlarmFlag[ 0U ] = gvRfRTMeasData.mRTMeasData_AlarmFlag[ 0U ];
		vutRfDataBuffer.umRT_MeasureTx.mNewAlarmFlag[ 1U ] = gvRfRTMeasData.mRTMeasData_AlarmFlag[ 1U ];
		
		for( wkLoop = 0U ; wkLoop < 5U ; wkLoop++ )
		{
			/* 最新計測値と論理 */
			vutRfDataBuffer.umRT_MeasureTx.mNewMeasureValue[ wkLoop ] = gvRfRTMeasData.mRTMeasData_MeasVal[ wkLoop ];
		}
		
		/* 計測警報履歴送信時は、3bit:0とする */
		if( arSel == MEASALM_DATASET )
		{
			vutRfDataBuffer.umRT_MeasureTx.mAbnormalSratus &= 0xC7U;
		}
	}
	
	/* 最新計測値データ時系列ID */
	vutRfDataBuffer.umRT_MeasureTx.mNewMeasureID[ 0U ] = gvRfRTMeasData.mRTMeasData_MeasID[ 0U ];
	vutRfDataBuffer.umRT_MeasureTx.mNewMeasureID[ 1U ] = gvRfRTMeasData.mRTMeasData_MeasID[ 1U ];
	
	/******************** 計測値送信と警報履歴送信でここから分岐 *************************/
	if( arSel == MEASURE_DATASET )
	{
	/* 送信過去データ数 */
	vutRfDataBuffer.umRT_MeasureTx.mDataCntKind = vRtMode_MeasSndKosu;
	
	if( vRtMode_MeasSndKosu != 0U )
	{
		ApiFlash_RfRTMeasDataGet( &wkRfRtMeasData[ 0U ] );
		
		/* 先頭データ時刻 */
		for( wkLoop = 0U ; wkLoop < 4U; wkLoop++ )
		{
			wkU32 = wkRfRtMeasData[ 0U ].mTimeInfo >> (24U - wkLoop * 8U);
			vutRfDataBuffer.umRT_MeasureTx.mHeaderTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
		}
		
		/* 先頭計測データの時系列ID(IndexNo.) */
		wkOldPtrIndex = gvInFlash.mData.mMeas1_OldPtrIndex + 1U;
		if( wkOldPtrIndex > imIndexMaxMeas1 )
		{
			wkOldPtrIndex = 1U;
		}
		vutRfDataBuffer.umRT_MeasureTx.mHeaderID[ 0U ] = (uint8_t)(wkOldPtrIndex & 0x00FF);
		vutRfDataBuffer.umRT_MeasureTx.mHeaderID[ 1U ] = (uint8_t)((wkOldPtrIndex >> 8U) & 0x00FF);
		
		/* 収録間隔1(下位4bit) */
		vutRfDataBuffer.umRT_MeasureTx.mMeasureIntFlag[ 0U ] = gvInFlash.mParam.mLogCyc1;
		
		/* 計測過去警報フラグ(上位4bit) */
		for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop ++ )
		{
			if( gvInFlash.mData.mAlmPast[ wkLoop ] == ecAlmPastSts_Set )
			{
				vutRfDataBuffer.umRT_MeasureTx.mMeasureIntFlag[ 0U ] += M_BIT4 << wkLoop;
			}
		}
			
		/* 計測値と論理と警報フラグ：1~9データ */
		for( wkLoop = 0U; wkLoop < vRtMode_MeasSndKosu; wkLoop++ )
		{
			/* データ間の時刻差でリードする個数を再更新する */
			wkClock = wkRfRtMeasData[ wkLoop ].mTimeInfo;
			
			if( wkLoop != 0U )
			{
				wkClockDiff = wkClock - wkClockPre;
				wkInterval = ApiIntMeasExist( wkClockDiff );
				if( wkInterval == (uint8_t)ecRecKind1_None )
				{
					vRtMode_MeasSndKosu = wkLoop;
					/* 送信過去データ数変更 */
					vutRfDataBuffer.umRT_MeasureTx.mDataCntKind = vRtMode_MeasSndKosu;
					break;
				}
				else
				{
					if( (wkLoop >= 2U) && (wkClockDiff != wkClockDiffPre) )
					{
						vRtMode_MeasSndKosu = wkLoop;
						/* 送信過去データ数変更 */
						vutRfDataBuffer.umRT_MeasureTx.mDataCntKind = vRtMode_MeasSndKosu;
						break;
					}
					else
					{
						/* 収録間隔1更新 */
						vutRfDataBuffer.umRT_MeasureTx.mMeasureIntFlag[ 0U ] &= 0xF0U;
						vutRfDataBuffer.umRT_MeasureTx.mMeasureIntFlag[ 0U ] |= wkInterval;
					}
				}
				wkClockDiffPre = wkClockDiff;
			}
			wkClockPre = wkClock;
			
			
			/* 計測値1CH~3CH */
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
			
			/* 論理 */
			vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 4U ] |= (uint8_t)(wkRfRtMeasData[ wkLoop ].mLogic << 7U);
			
			/* 警報フラグ */
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
							/* 上限警報上上限警報 */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmHAlmHH << wkBitShift);
			}
						else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgH )
			{
							/* 上限逸脱上上限警報 */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevHAlmHH << wkBitShift);
			}
					}
					else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgHH )
					{
						if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgH )
			{
							/* 上限警報上上限逸脱 */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmHDevHH << wkBitShift);
			}
						else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgH )
			{
							/* 上限逸脱上上限逸脱 */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevHDevHH << wkBitShift);
			}
					}
					else if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgH )
					{
						/* 上限警報 */
						vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmH << wkBitShift);
					}
					else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgH )
					{
						/* 上限逸脱 */
						vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevH << wkBitShift);
					}
					else if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgLL )
					{
						if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgL )
						{
							/* 下限警報下下限警報 */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmLAlmLL << wkBitShift);
						}
						else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgL )
						{
							/* 下限逸脱下下限警報 */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevLAlmLL << wkBitShift);
						}
					}
					else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgLL )
					{
						if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgL )
			{
							/* 下限警報下下限逸脱 */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmLDevLL << wkBitShift);
			}
						else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgL )
			{
							/* 下限逸脱下下限逸脱 */
							vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevLDevLL << wkBitShift);
			}
					}
					else if( wkRfRtMeasData[ wkLoop ].mAlmFlg[ wkLoopA ] & imAlmFlgL )
					{
						/* 下限警報 */
						vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_AlmL << wkBitShift);
					}
					else if( wkRfRtMeasData[ wkLoop ].mDevFlg[ wkLoopA ] & imAlmFlgL )
			{
						/* 下限逸脱 */
						vutRfDataBuffer.umRT_MeasureTx.mMeasureData[ wkLoop ][ 5U + wkNum ] |= (ecAlmFlg_DevL << wkBitShift);
			}
				}
#endif
			}
			/* 機器異常過去警報フラグ */
			if( gvInFlash.mData.mAlmPast[ 4U ] == ecAlmPastSts_Set )
			{
				vutRfDataBuffer.umRT_MeasureTx.mDataCntKind |= 0x40U;
			}
		}
	}
	else
	{
		/* 送信過去データ数 */
		vutRfDataBuffer.umRT_AuditTx.mDataCntKind = vRtMode_MeasAlmSndKosu | 0x80;
		
		/* 過去警報フラグ(上位4bit) */
		vutRfDataBuffer.umRT_AuditTx.mPastAlarmFL = 0U;
		for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop ++ )
		{
			if( gvInFlash.mData.mAlmPast[ wkLoop ] == ecAlmPastSts_Set )
			{
				vutRfDataBuffer.umRT_AuditTx.mPastAlarmFL += M_BIT4 << wkLoop;
			}
		}
		/* 機器異常過去警報フラグ */
		if( gvInFlash.mData.mAlmPast[ 4U ] == ecAlmPastSts_Set )
		{
			vutRfDataBuffer.umRT_AuditTx.mDataCntKind |= 0x40U;
		}
		
		if( vRtMode_MeasAlmSndKosu != 0U )
		{
			/* 先頭計測警報履歴の時系列ID(IndexNo.) */
			wkOldPtrIndex = gvInFlash.mData.mMeasAlm_OldPtrIndex;
			if( wkOldPtrIndex > imIndexMaxMeasAlm )
			{
				wkOldPtrIndex = 1U;
			}
			vutRfDataBuffer.umRT_AuditTx.mHeaderID[ 0U ] = (uint8_t)(wkOldPtrIndex & 0x00FF);
			vutRfDataBuffer.umRT_AuditTx.mHeaderID[ 1U ] = (uint8_t)((wkOldPtrIndex >> 8U) & 0x00FF);
			
			/* Flashからリードした計測警報履歴を変数に格納 */
			ApiFlash_RfRTMeasAlmDataGet( &wkRfRtMeasAlmData[ 0U ] );
			
			/* 警報履歴：1~6データ */
			for( wkLoop = 0U; wkLoop < vRtMode_MeasAlmSndKosu; wkLoop++ )
			{
				/* 時刻 */
				for( wkLoopA = 0U ; wkLoopA < 4U; wkLoopA++ )
				{
					wkU32 = wkRfRtMeasAlmData[ wkLoop ].mTimeInfo >> (24U - wkLoopA * 8U);
					vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ wkLoopA ] = (uint8_t)(wkU32 & 0x000000FF);
				}
				
				/* 警報ch(mMeasureData[4]:0~1bit) */
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 4U ] = wkRfRtMeasAlmData[ wkLoop ].mAlmChannel;
				
				/* 論理(mMeasureData[4]:2bit) */
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 4U ] |= (uint8_t)(wkRfRtMeasAlmData[ wkLoop ].mEvent << 2U);
				
				/* 発生した計測値の1CH分(mMeasureData[8]-[9]) */
				wkS16 = wkRfRtMeasAlmData[ wkLoop ].mMeasVal + gvInFlash.mProcess.mModelOffset;
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 8U ] = (uint8_t)wkS16;
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 9U ] = (uint8_t)((wkS16 & 0x1F00) >> 8U);
				
				/* アラームフラグ:0bit */
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 10U ] = wkRfRtMeasAlmData[ wkLoop ].mAlmFlg;
				
				/* レベル:1~2bit */
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 10U ] |= (wkRfRtMeasAlmData[ wkLoop ].mLevel << 1U);
				
				/* 警報項目:3~7bit */
				vutRfDataBuffer.umRT_AuditTx.mMeasureData[ wkLoop ][ 10U ] |= (wkRfRtMeasAlmData[ wkLoop ].mItem << 3U);
			}
		}
	}
}
#endif


#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 * 計測値1の過去送信ポインタを更新
 *
 *	[内容]
 *		計測値1の過去送信ポインタを更新
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
static void SubRfDrv_RtmodeMeas1OldPtrUpdate( void )
{
	gvInFlash.mData.mMeas1_OldPtrIndex += vRtMode_MeasSndKosu;
	
	if( gvInFlash.mData.mMeas1_OldPtrIndex > imIndexMaxMeas1 )
	{
		gvInFlash.mData.mMeas1_OldPtrIndex -= imIndexMaxMeas1;
	}
	
	/* イベント有効から送信 */
	if( gvInFlash.mParam.mRealTimeSndPt == 510U )
	{
		/* イベント有効ポインタに過去送信ポインタを代入 */
		gvInFlash.mData.mMeas1_EvtPtrIndex = gvInFlash.mData.mMeas1_OldPtrIndex;
	}
}
#endif

#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 * 長距離通信(計測警報履歴)のFlashリードIndexNo.をセット
 *
 *	[内容]
 *		長距離通信(計測警報履歴)にデータセットするため、
 *		Flashからリードする先頭IndexNo.を算出する
 *		算出は、計測警報履歴最新IndexNo.からリード個数分引いたIndexNo.とする
 *	[引数]
 *		uint16_t arKosu:リードする個数(1〜6個)
 *	[戻値]
 *		なし
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
* 高速モード ビーコン受信データの処理
*
* [内容]
* 受信したビーコンから必要なデータを取り出し、要求コマンドに応じて遷移先をセット
* 無線送信の準備も行う
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
	
	/* 受信コマンドコードを変換(ただし値は受信値と同じ) */
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
	
	/* Sleepのコードをカウント値に変換 */
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
	
	/* GW時刻受信 */
	wkU32 = vutRfDataBuffer.umGwQueryBeacon.mTime[ 0U ];
	wkU32 |= (uint32_t)(vutRfDataBuffer.umGwQueryBeacon.mTime[ 1U ]) << 8U;
	wkU32 |= (uint32_t)(vutRfDataBuffer.umGwQueryBeacon.mTime[ 2U ]) << 16U;
	wkU32 |= (uint32_t)(vutRfDataBuffer.umGwQueryBeacon.mTime[ 3U ] & 0x7FU) << 24U;
#if 1
	ApiTimeComp_first( wkU32, 0U );
#else
/* debug用 */
	gvClock = ApiRtcDrv_localtime( wkU32/2 );
	/* 内蔵RTCに時刻ライト */
	ApiRtcDrv_SetInRtc( gvClock );
	
	/* 内蔵RTCの時刻を外付けRTCに時刻ライト */
	ApiRtcDrv_InRtcToExRtc();
#endif
	
	
	if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Status )
	{
		/* ブロードキャストのためロガーID照合なし */
		/* COLLECT表示ON */
		ApiRfDrv_HsStsDisp( 1U );
	}
	else if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_Toroku )
	{
		/* ブロードキャストのためロガーID照合なし */
		/* COLLECT表示ON */
		ApiRfDrv_HsStsDisp( 1U );
	}
	else if( (vutRfDataBuffer.umGwQueryBeacon.mLoggerID[0U] == vrfMySerialID.mUniqueID[0U])
				&&	(vutRfDataBuffer.umGwQueryBeacon.mLoggerID[1U] == vrfMySerialID.mUniqueID[1U])
				&&	(vutRfDataBuffer.umGwQueryBeacon.mLoggerID[2U] == vrfMySerialID.mUniqueID[2U]))
	{
		/* ID指定呼び出し */
		rfTimeSlotNum = 0U;					/* データ送信のための送信時間確保 */
		rfTimeSlotGrupe = 2U;				/* 200msec後に応答 */
		/* COLLECT表示ON */
		ApiRfDrv_HsStsDisp( 1U );
		
		if( vrfHsReqCmdCode == ecRfDrvGw_Hs_ReqCmd_ReqFirm )
		{
			gvFirmupMode = imON;			/* ファームアップ実行中 */
			ApiFlash_FlashPowerCtl( imON, imON );
		}
	}
	else if( vutRfDataBuffer.umGwQueryBeacon.mGroupID == vrfMySerialID.mGroupID )
	{
		/* グループ呼び出し 初期フェーズでの実装見送り */
		rfTimeSlotNum = 0U;					/* データ送信のための送信時間確保 */
//		/* COLLECT表示ON */
//		ApiRfDrv_HsStsDisp( 1U );
		SubRFDrv_Hsmode_End();
	}
	else
	{
		/* 該当しない */
		SubRFDrv_Hsmode_End();
	}
}


/*
***********************************************************************************
* 高速モード ビーコン受信データ解析し応答準備
*
* [内容]
*		受信したビーコンから必要なデータを取り出し、解析
*		応答のためのデータセットの準備(Flashへデータリードのキューをセット)を行う
***********************************************************************************
*/
static void SubRfDrv_Hsmode_Logger_BeaconRxAnalysis( void )
{
#if (swKouteiMode == imDisable)
	ST_BeaconRxAnalysis_t wkAnalysis;
	
	
	/* 要求開始時刻 */
	wkAnalysis.mReqStartTime = (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mRequestHeaderMeasureTime[ 0U ] << 24U;
	wkAnalysis.mReqStartTime |= (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mRequestHeaderMeasureTime[ 1U ] << 16U;
	wkAnalysis.mReqStartTime |= (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mRequestHeaderMeasureTime[ 2U ] << 8U;
	wkAnalysis.mReqStartTime |= (uint32_t)vutRfDataBuffer.umGwQueryBeacon.mRequestHeaderMeasureTime[ 3U ];
	
	/* 要求IndexNo. */
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
			
		/* 計測値データ収集要求 */
		case 0x02:
			ApiRfDrv_MakeReadQue( ecRegionKind_Meas1, wkAnalysis.mReqStartTime, wkAnalysis.mReqIndexNo );
			break;
			
		/* 警報履歴収集要求 */
		case 0x03:
			ApiRfDrv_MakeReadQue( ecRegionKind_MeasAlm, wkAnalysis.mReqStartTime, wkAnalysis.mReqIndexNo );
			break;
			
		/* 機器異常履歴収集要求 */
		case 0x04:
			ApiRfDrv_MakeReadQue( ecRegionKind_ErrInfo, wkAnalysis.mReqStartTime, wkAnalysis.mReqIndexNo );
			break;
			
		/* イベント収集要求 */
		case 0x05:
			ApiRfDrv_MakeReadQue( ecRegionKind_Event, wkAnalysis.mReqStartTime, wkAnalysis.mReqIndexNo );
			break;
			
		/* 動作履歴収集要求 */
		case 0x06:
			ApiRfDrv_MakeReadQue( ecRegionKind_ActLog, wkAnalysis.mReqStartTime, wkAnalysis.mReqIndexNo );
			break;
			
		/* システム履歴収集要求 */
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
	
	/* データ収集送信パケット制御用変数初期化 */
	vrfLogSendPacketCtl.mFlashReadCnt[ 0U ] = 0U;
	vrfLogSendPacketCtl.mFlashReadCnt[ 1U ] = 0U;
	vrfLogSendPacketCtl.mFlashReadFin[ 0U ] = ecLogSndCtl_Init;
	vrfLogSendPacketCtl.mFlashReadFin[ 1U ] = ecLogSndCtl_Init;
	vrfLogSendPacketCtl.mFlashReadTime[ 0U ] = 0U;
	vrfLogSendPacketCtl.mFlashReadTime[ 1U ] = 0U;
	vrfLogSendPacketCtl.mInterval = 0U;
			
	switch( arRegionKind )
	{
		/* 機器異常履歴収集要求 */
		case ecRegionKind_ErrInfo:
			/* Flashからリードするためのパラメータセット */
			wkAnalysis.mData_Num = gvInFlash.mData.mErrInf_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mErrInf_Index;
			wkAnalysis.mMax_Num = imDataNumErrInf;
			wkAnalysis.mMax_Index = imIndexMaxErrInf;
			
			wkAnalysis.mKosu = 100U;
			wkAnalysis.mRegionKind = ecRegionKind_ErrInfo;
			break;
			
		/* 警報履歴収集要求 */
		case ecRegionKind_MeasAlm:
			/* Flashからリードするためのパラメータセット */
			wkAnalysis.mData_Num = gvInFlash.mData.mMeasAlm_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mMeasAlm_Index;
			wkAnalysis.mMax_Num = imDataNumMeasAlm;
			wkAnalysis.mMax_Index = imIndexMaxMeasAlm;
			
			wkAnalysis.mKosu = 100U;
			wkAnalysis.mRegionKind = ecRegionKind_MeasAlm;
			break;
			
		/* イベント収集要求 */
		case ecRegionKind_Event:
			/* Flashからリードするためのパラメータセット */
			wkAnalysis.mData_Num = gvInFlash.mData.mEvt_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mEvt_Index;
			wkAnalysis.mMax_Num = imDataNumEvt;
			wkAnalysis.mMax_Index = imIndexMaxEvt;
			
			wkAnalysis.mKosu = 60U;
			wkAnalysis.mRegionKind = ecRegionKind_Event;
			break;
			
		/* 動作履歴収集要求 */
		case ecRegionKind_ActLog:
			/* Flashからリードするためのパラメータセット */
			wkAnalysis.mData_Num = gvInFlash.mData.mActLog_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mActLog_Index;
			wkAnalysis.mMax_Num = imDataNumActLog;
			wkAnalysis.mMax_Index = imIndexMaxActLog;
			
			wkAnalysis.mKosu = 90U;
			wkAnalysis.mRegionKind = ecRegionKind_ActLog;
			break;
			
		/* 計測値データ収集要求 */
		case ecRegionKind_Meas1:
			/* Flashからリードするためのパラメータセット */
			wkAnalysis.mData_Num = gvInFlash.mData.mMeas1_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mMeas1_Index;
			wkAnalysis.mMax_Num = imDataNumMeas1;
			wkAnalysis.mMax_Index = imIndexMaxMeas1;
			
			wkAnalysis.mKosu = 90U;
			wkAnalysis.mRegionKind = ecRegionKind_Meas1;
			break;
			
		/* システム履歴収集要求 */
		case ecRegionKind_SysLog:
			/* Flashからリードするためのパラメータセット */
			wkAnalysis.mData_Num = gvInFlash.mData.mSysLog_Num;
			wkAnalysis.mData_Index = gvInFlash.mData.mSysLog_Index;
			wkAnalysis.mMax_Num = imDataNumSysLog;
			wkAnalysis.mMax_Index = imIndexMaxSysLog;
			
			wkAnalysis.mKosu = 90U;
			wkAnalysis.mRegionKind = ecRegionKind_SysLog;
			break;
	}
	
	/* Flashにリードキューをセット */
	SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Sub( wkAnalysis );
	
#endif
}


/*
***********************************************************************************
* 高速モード ビーコン受信データ解析し応答準備(サブ関数)
*
* [内容]
*		高速モードの応答のために、Flashにリードするキューをセットする
*		1パケット目なので、リードする先頭のIndexNo.を算出してFlashリードキューのセット
***********************************************************************************
*/
static void SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Sub( ST_BeaconRxAnalysis_t arAnalysis )
{
#if (swKouteiMode == imDisable)
	ET_Error_t wkError;
	uint16_t wkU16;
	
	
	/*** 先頭IndexNo.の算出 ***/
	
	/* 収録データ数：0のとき */
	if( arAnalysis.mData_Num == 0U )
	{
		vrfHsReqDataNum = 1U;										/* 応答する収録データ数：1 */
		arAnalysis.mReqIndexNo = 1U;								/* Flashからリードする先頭IndexNo.：1 */
	}
	/* 収録データ数：1〜(収録データMAX数-1)のとき */
	else if( arAnalysis.mData_Num < arAnalysis.mMax_Num )
	{
		/* 要求IndexNo.：1〜最新IndexNo.なら */
		if( arAnalysis.mReqIndexNo >= 1U && arAnalysis.mReqIndexNo <= arAnalysis.mData_Index )
		{
			wkU16 = arAnalysis.mData_Index + 1U - arAnalysis.mReqIndexNo;
			vrfHsReqDataNum = wkU16;								/* 応答する収録データ数：(最新IndexNo.+1)-要求IndexNo. */
		}
		/* 要求IndexNo.：0、又は最新IndexNo.超過なら */
		else
		{
			vrfHsReqDataNum = arAnalysis.mData_Num;					/* 応答する収録データ数：収録データ数=最新IndexNo. */
			arAnalysis.mReqIndexNo = 1U;							/* Flashからリードする先頭IndexNo.：1 */
		}
	}
	/* 収録データMAX数のとき */
	else
	{
		/* 要求IndexNo.：1〜最新IndexNo. */
		if( arAnalysis.mReqIndexNo >= 1U && arAnalysis.mReqIndexNo <= arAnalysis.mData_Index )
		{
			/* 送信データ数演算 */
			wkU16 = arAnalysis.mData_Index + 1U - arAnalysis.mReqIndexNo;
		}
		/* 要求IndexNo.：最新IndexNo.より大きい */
		else if( arAnalysis.mReqIndexNo > arAnalysis.mData_Index )
		{
			/* 送信データ数演算 */
			wkU16 = arAnalysis.mMax_Index + 1U - arAnalysis.mReqIndexNo;
			wkU16 += arAnalysis.mData_Index;
		}
		
		/* 要求IndexNo.：0、又は送信データ数が収録データMAX数超過のとき */
		if( arAnalysis.mReqIndexNo == 0U || wkU16 > arAnalysis.mMax_Num )
		{
			vrfHsReqDataNum = arAnalysis.mMax_Num;					/* 応答する収録データ数：収録データ数(収録データMAX数) */
			
			/* 現在のIndexNo.が収録データMAX数以上 */
			if( arAnalysis.mData_Index >= arAnalysis.mMax_Num )
			{
				wkU16 = arAnalysis.mData_Index + 1U - arAnalysis.mMax_Num;
				arAnalysis.mReqIndexNo = wkU16;						/* Flashからリードする先頭IndexNo.：(現在IndexNo. + 1) - 収録データMAX数 */
			}
			/* 現在のIndexNo.:1〜(収録データMAX数-1) */
			else
			{
				wkU16 = arAnalysis.mMax_Index + 1U - arAnalysis.mMax_Num;
				wkU16 += arAnalysis.mData_Index;
				arAnalysis.mReqIndexNo = wkU16;						/* Flashからリードする先頭IndexNo.：((IndexNo.最大値 + 1) - 収録データMAX数) + 現在IndexNo. */
			}
		}
		else
		{
			vrfHsReqDataNum = wkU16;								/* 応答する収録データ数：最新IndexNo.+1-要求IndexNo. */
		}
	}
	
	vMem_ReqIndexNo_St = arAnalysis.mReqIndexNo;					/* Flashリードの先頭IndexNoをセット */
	
	/* Flashにリードキューをセット */
	vMem_RdIndexNo = arAnalysis.mReqIndexNo;
	wkError = ApiFlash_ReadQueSet( arAnalysis.mReqStartTime, 0U, arAnalysis.mReqIndexNo, arAnalysis.mKosu, arAnalysis.mRegionKind, 0U );
	if( wkError == ecERR_OK )
	{
		ApiSetFlash_ReqRdFlg( 1U, arAnalysis.mRegionKind );			/* 外付けフラッシュリード要求フラグセット */
		gvModuleSts.mExFlashRd = ecExFlashRdModuleSts_Run;			/* Flashリード完了判断処理へ遷移 */
	}
	else
	{
		/* キューがいっぱい:debug*/
		M_NOP;
	}
#endif
}


/*
***********************************************************************************
*	高速モード 連続応答の準備
*
* [内容]
*	続けて応答するためのデータセットの準備(Flashへデータリードのキューをセット)を行う
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
* 高速モード 連続応答の準備(サブ関数)
*
* [内容]
*	高速モードの応答のために、Flashにリードするキューをセットする
***********************************************************************************
*/
static void SubRfDrv_Hsmode_Logger_BeaconRxAnalysis_Continue_Sub( uint16_t arKosu, ET_RegionKind_t arRegion )
{
	ET_Error_t wkError;
	
	/* Flashにリードキューをセット */
	wkError = ApiFlash_ReadQueSet( 0U, 0U, vMem_RdIndexNo, arKosu, arRegion, 0U );
	if( wkError == ecERR_OK )
	{
		ApiSetFlash_ReqRdFlg( 1U, arRegion );					/* 外付けフラッシュリード要求フラグセット */
		gvModuleSts.mExFlashRd = ecExFlashRdModuleSts_Run;		/* Flashリード完了判断処理へ遷移 */
	}
	else
	{
		/* キューがいっぱい:debug*/
		M_NOP;
	}
}


/*
***********************************************************************************
* 高速モード 連続応答のACKによる次の送信ポインタセット
*
* [内容]
* 続けて応答するためにACKによる次の送信ポインタをセットする
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
* 要求IndexNo.をセットする
*
* [内容]
* 要求IndexNo.をセットする
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
* 電池脱時の割り込み無線停止
*
* [内容]
* 電池を抜いたときに強制sleep処理する
* 
***********************************************************************************
*/
void ApiRfDrv_ForcedSleep( void )
{
	/* 無線IC初期化 */
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
		
		/* RF(無線通信)の初期化完了 */
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
* 無線ICリセット処理
*
* [内容]
* 無線ICをリセット/初期化する。POR処理はしない。TUAを使用。
* 
***********************************************************************************
*/
void SubRfDrv_RfReset( void )
{
//	SubSX1272POR();														/* POR処理 */
	/* 10msecのWait必要 → 10〜11msecのWait */
//	SubIntervalTimer_Sleep( 3750U, 1U );
//	while(!gvRfIntFlg.mRadioTimer){	NOP(); }
//	SubIntervalTimer_Stop();
	ApiSX1272Reset( imON );
	/* 100usec以上必要 → Wait 1〜2 ms */
	SubIntervalTimer_Sleep( 375U, 1U );		/* 0.1msec */
	while(!gvRfIntFlg.mRadioTimer){	NOP(); }
	SubIntervalTimer_Stop();
	ApiSX1272Reset( imOFF );
	/* 5msec必要 → Wait 6〜7 ms */
	SubIntervalTimer_Sleep( 375U * 6U, 1U );		/* 5msec */
	while(!gvRfIntFlg.mRadioTimer){	NOP(); }
	SubIntervalTimer_Stop();
	SubSX1272SetModem( MODEM_FSK );
	/* 初期化 */
	SubSX1272Sleep();
	SubSX1272InitReg();
	rfStatus.Modem = MODEM_FSK;
	SubSX1272Sleep();

//	2021.7.12追加
#if (swLoggerBord == imEnable)
	/* AppIDチェック */
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
			ApiFlash_WriteSysLog( ecSysLogItm_IdDiff, 1U );				/* システム履歴：ID書き換わり(アプリID) */
//			ApiAbn_AbnStsSet( (imAbnSts_Dummy3 << vRfTestFlg), ecAbnInfKind_AbnSts );
		}
		vRfTestFlg = 0U;
	}
	if( memcmp( vrfMySerialID.mUniqueID, gvInFlash.mProcess.mUniqueID, 3U ) != 0 )
	{
		memcpy( &vrfMySerialID.mUniqueID[ 0U ], &gvInFlash.mProcess.mUniqueID[ 0U ], 3U );
		
		if( vRfTestFlg != 0U )
		{
			ApiFlash_WriteSysLog( ecSysLogItm_IdDiff, 2U );				/* システム履歴：ID書き換わり(アプリID) */
//			ApiAbn_AbnStsSet( (imAbnSts_Dummy3 << vRfTestFlg), ecAbnInfKind_AbnSts );
		}
		vRfTestFlg = 0U;
	}
	if( vrfMySerialID.mOpeCompanyID != gvInFlash.mProcess.mOpeCompanyID )
	{
		vrfMySerialID.mOpeCompanyID = gvInFlash.mProcess.mOpeCompanyID;
		
		if( vRfTestFlg != 0U )
		{
			ApiFlash_WriteSysLog( ecSysLogItm_IdDiff, 3U );				/* システム履歴：ID書き換わり(アプリID) */
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
* 無線ICから温度値を取得
*
* [内容]
* 無線ICの測温機能でロガー内部温度を取得する
* [返り値]
* 分解能1℃の温度値
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

	/* 捨てread */
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

	/* 中央値検索 */
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
			/* 中央値 */
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

	/* 電池AD、温度測定のコンパレータ動作ON状態と被ったら前回の値を採用 */
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
		
		/* AD値の換算 */
#if (swLoggerBord == imEnable)
		wkAdcResult = (wkAdcResult * 3000UL)/(1023UL);
#else
		wkAdcResult = (wkAdcResult * 3300UL)/(1023UL);
#endif
		if( wkAdcResult >= 1050UL )
		{
			wkAdcResult -= 1050UL;			/* 1050mV(@25℃) */
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

		wkReturn += gvInFlash.mProcess.mRfTmpHosei;			/* 温度校正分を補正 */
		
		/* 2021.10.27 温度A/D突変対策 コメントアウト */	
//		gvInTemp = wkReturn;								/* 内部温度格納 */
		
#if (swRfTestLcd == imEnable)
		ApiLcd_Upp7SegNumDsp( vrfTempCal, imOFF, imOFF );
		ApiLcd_Low7SegNumDsp( wkReturn, imOFF );
		ApiLcd_Main();
#endif
	}
	else
	{
#if 0
		/* デバッグ用履歴 */
		ApiFlash_WriteActLog( ecActLogItm_Boot, gvBatAd.mPhase, 200 );
		ApiFlash_WriteActLog( ecActLogItm_Boot, gvInTemp, 210 );
#endif
		wkReturn = gvInTemp;								/* 前回の内部温度を採用 */
	}
	
	/* 2021.10.27 温度A/D突変対策 */
	if( wkReturn < -40 )
	{
		wkReturn = -40;
	}
	if( wkReturn > 70 )
	{
		wkReturn = 70;
	}

	/* 5℃以上の変動は��5℃に固定 */
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
* 電池抜いたときの無線処理
*
* [内容]
*		電池抜いたときの無線処理
* [引数]
*		削除 uint8_t arSel:0…割込発生による無線停止、1…電池残量0本(AD)による無線停止
* [返り値]
* 		なし
***********************************************************************************
*/
void ApiRfDrv_BattOffInt( void )
{
	ApiSX1272Reset( imON );
	/* 100usec以上必要 → Wait 1〜2 ms */
	SubIntervalTimer_Sleep( 375U, 1U );									/* 0.1msec */
	while(!gvRfIntFlg.mRadioTimer){	NOP(); }
	SubIntervalTimer_Stop();
	ApiSX1272Reset( imOFF );
	/* 5msec必要 → Wait 6〜7 ms */
	SubIntervalTimer_Sleep( 375U * 6U, 1U );							/* 5msec */
	while(!gvRfIntFlg.mRadioTimer){	NOP(); }
	SubIntervalTimer_Stop();
	
	/* フラグON→OFFの切り替わりに実行 */
	if( gvrfStsEventFlag.mPowerInt == BATT_POWER_ON )
	{
		SubRfDrv_SetRtConInfo( imOFF );									/* 接続解除 */
		vrfRTmodeCommFailNum = vrfRTmodeCommCutTime;					/* 2021.9.22 切断処理方法の変更 */
		/* 2021.8.27 */
		vrfRtmodeStopCnt = 0U;
		
#if 0	/* 無線停止(電池残量空、電池残量0本):動作履歴 → 電池残量低下(残量0本、無線停止):機器異常履歴に統一 */
		if( arSel == 0U )
		{
			ApiFlash_WriteActLog( ecActLogItm_RfStopBatEmpty, 0U, 0U );	/* 操作履歴：無線停止、自動操作 */
		}
		else
		{
			ApiFlash_WriteActLog( ecActLogItm_RfStopBatLow, 0U, 0U );	/* 操作履歴：無線停止、自動操作 */
		}
#endif
	}
	
	/* COLLECT表示OFF */
	ApiRfDrv_HsStsDisp( 0U );
}
#endif

/*
***********************************************************************************
* 無線通信タイミングの温度補正値を算出
*
* [内容]
* 無線通信タイミング(gvRfTimingCounter)を温度により補正する。
* 低消費電流化のためにReturn値ではなくグローバル変数gvRfTimingTempCorrectで保持
* gvRfTimingCounterL(0x7FF - gvRfTimingTempCorrect)でgvRfTimingCounterHをカウント
* [返り値]
* なし
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
	
	/* 2021.10.27 温度A/D突変対策 */
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
		gvRfTimingTempCorrect = ( ((sint16_t)wkTemp - 25 ) * ((sint16_t)wkTemp - 25 ) * 3U) / 1000U;			/*	-0.03 * δT ^2 ppm , 1secで-1count(-10ppm) */
//		gvRfTimingTempCorrect++;					/* プラス方向に補正を追加 */
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
	gvRfTimingTempCorrect += 2U;					/* プラス方向に補正を追加 */

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
*  高速モード終了処理
*
*  [内容]
*  高速通信モードからIdleへ遷移する際の処理
********************************************************************************
*/
void SubRFDrv_Hsmode_End( void )
{
	/* COLLECT表示OFF */
	ApiRfDrv_HsStsDisp( 0U );
	
	vrfInitState = ecRfDrv_Init_Non;
	//vrfHsmodeSleepMode = 0x00;
	vRfDrvMainLoggerSts = ecRfDrvMainSts_RTmode_LoggerIdle;
	gvModuleSts.mRf = ecRfModuleSts_Sleep;
}


/*
 *******************************************************************************
 * リードした計測データを無線送信バッファに格納
 *
 *	[内容]
 *		Flashからリードした計測データを無線送信バッファに格納
 *	[引数]
 *		ST_FlashVal1Evt_t	arRdData: リードした計測値
 *		uint8_t				arLoop: リードした個数(0~89)
 *		uint8_t				arKosu: リードする個数
 *		uint16_t			arIndex: リードした先頭IndexNo.
 *	[戻値]
 *		なし
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
	static uint32_t			vClockPre;				/* 前回格納した計測データの時刻 */
	static uint16_t			vClockDiffPre;			/* 前回と今回の計測時刻の間隔 */
	static uint8_t			vDummyFlg;				/* 計測値をダミーにするかどうかのフラグ */
	
	if( gvrfStsEventFlag.mHsmodeTxTurn == 0U )
	{
		wkTurn = 0U;								/* 奇数パケットリード */
	}
	else
	{
		wkTurn = 1U;								/* 偶数パケットリード */
	}
	
	/* 個数リード初回 */
	if( arLoop == 0U )
	{
		memset( vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResMeasure) );
		
		vDummyFlg = imOFF;
		
		/* 先頭データ時刻(上位Bitから無線バッファに埋める) */
		for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
		{
			wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
			vutRfDataBuffer.umLoggerResMeasure.mHeaderTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
		}
		
		/* 計測データ時系列ID */
		vutRfDataBuffer.umLoggerResMeasure.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResMeasure.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		gvFlashReadIndex[ wkTurn ] = arIndex;
		
		/* パケット種別コード */
		vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode = 0x02U;
		
		if( arRdData->mTimeInfo == 0x3FFFFFFF )
		{
			/* 収録間隔は現在の設定値にして、今回から計測値をダミーにする */
			vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode |= gvInFlash.mParam.mLogCyc1 << 4U;
			
			vDummyFlg = imON;									/* 計測値ダミー */
			vutRfDataBuffer.umLoggerResMeasure.mDataNum = 0U;	/* 有効データ数:0 */
		}
		
		
		/* 初回(1パケット目) */
		if( vrfLogSendPacketCtl.mFlashReadCnt[ wkTurn ] == 0U )
		{
			vrfLogSendPacketCtl.mFlashReadCnt[ wkTurn ] ++;
		}
		else
		{
			if( vrfLogSendPacketCtl.mFlashReadFin[ wkTurn ] == ecLogSndCtl_Init )
			{
				/* 前回リードした先頭時刻と今回の時刻が違うなら、異なるアドレスからリード */
				if( vrfLogSendPacketCtl.mFlashReadTime[ wkTurn ] != arRdData->mTimeInfo )
				{
					/* 前回の時刻と今回の時刻の差が2パケット分かどうか(但し、偶数2パケットを除く) */
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
						/* 時刻差が違うのでFlashからのリード全完了 */
						vDummyFlg = imON;									/* 計測値ダミー */
						vutRfDataBuffer.umLoggerResMeasure.mDataNum = 0U;	/* 有効データ数:0 */
					}
					else
					{
						/* Flashリードカウンタをインクリメント */
						vrfLogSendPacketCtl.mFlashReadCnt[ wkTurn ] ++;
					}
				}
			}
			else
			{
				vDummyFlg = imON;									/* 計測値ダミー */
				vutRfDataBuffer.umLoggerResMeasure.mDataNum = 0U;	/* 有効データ数:0 */
			}
			
			
			if( vDummyFlg == imON )
			{
				vrfLogSendPacketCtl.mFlashReadFin[ wkTurn ] = ecLogSndCtl_SndFin;		/* 奇数/偶数パケットの送信完了 */
			}
		}
		
		/* 今回リードした先頭時刻を格納 */
		vrfLogSendPacketCtl.mFlashReadTime[ wkTurn ] = arRdData->mTimeInfo;
		
	}
	else
	{
		wkClockDiff = arRdData->mTimeInfo - vClockPre;
		
		if( vDummyFlg == imOFF )
		{
			/* 有効データ数 */
			vutRfDataBuffer.umLoggerResMeasure.mDataNum = arLoop + 1U;
			
			if( arLoop == 1U )
			{
				/* 送信パケット制御用に収録インターバルを格納 */
				if( vrfLogSendPacketCtl.mInterval == 0U )
				{
					vrfLogSendPacketCtl.mInterval = wkClockDiff;
				}
				
				wkInterval = ApiIntMeasExist( wkClockDiff );
				
				/* データ間の時刻が収録周期の設定以外の値 */
				if( wkInterval == (uint8_t)ecRecKind1_None )
				{
					/* 収録間隔は現在の設定値にして、今回から計測値をダミーにする */
					vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode |= gvInFlash.mParam.mLogCyc1 << 4U;
					
					vDummyFlg = imON;										/* 計測値ダミー */
					vutRfDataBuffer.umLoggerResMeasure.mDataNum = arLoop;	/* 有効データ数:リード回数分 */
				}
				else
				{
					vutRfDataBuffer.umLoggerResMeasure.mRecInt_PckCode |= wkInterval << 4U;
				}
			}
			else
			{
				/* 前回と今回の計測時刻の間隔が違うかを判断 */
				if( wkClockDiff != vClockDiffPre )
				{
					/* 今回から計測値をダミーにする */
					vDummyFlg = imON;										/* 計測値ダミー */
					vutRfDataBuffer.umLoggerResMeasure.mDataNum = arLoop;	/* 有効データ数:リード回数分 */
				}
			}
			
			if( vDummyFlg == imON )
			{
				/* 奇数/偶数パケットが送信完了でなければリード完了へ遷移 */
				if( vrfLogSendPacketCtl.mFlashReadFin[ wkTurn ] != ecLogSndCtl_SndFin )
				{
					vrfLogSendPacketCtl.mFlashReadFin[ wkTurn ] = ecLogSndCtl_ReadFin;
				}
				
				/* 奇数パケットリード */
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
				/* 偶数パケットリード */
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
		vClockDiffPre = wkClockDiff;		/* データ間の時刻を一時格納 */
	}
	vClockPre = arRdData->mTimeInfo;		/* リードした時刻を一時格納 */
	
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* 計測値1~3CH */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		/* 時刻が初期値 or ダミーフラグON */
		if( arRdData->mTimeInfo == 0x3FFFFFFF || vDummyFlg == imON )
		{
			wkS16 = imSndTmpInit;						/* ダミーデータ */
			if( gvInFlash.mProcess.mModelCode == ecSensType_V ||
				gvInFlash.mProcess.mModelCode == ecSensType_Pulse )
			{
				wkS16 += 2000;
			}
		}
		else
		{
			wkS16 = arRdData->mMeasVal[ wkLoop ];		/* リードした計測値 */
		}
		
		wkS16 += gvInFlash.mProcess.mModelOffset;
		vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[ wkBufCrcCnt ].mMeasure[ wkBufCnt ].mMeasure_AllCh_Event[ wkLoop * 2U ] = (uint8_t)wkS16;
		vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[ wkBufCrcCnt ].mMeasure[ wkBufCnt ].mMeasure_AllCh_Event[ (wkLoop * 2U) + 1U ] = (uint8_t)((wkS16 & 0x1F00) >> 8U);
	}
	
	/* 論理 */
	vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[ wkBufCrcCnt ].mMeasure[ wkBufCnt ].mMeasure_AllCh_Event[ 5U ] |= (arRdData->mLogic << 7U);
	
	/* 警報フラグ1~3CH(上位4bit:警報、下位4bit:逸脱) */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[ wkBufCrcCnt ].mMeasure[ wkBufCnt ].mAlart_AllCh[ wkLoop ] = (arRdData->mAlmFlg[ wkLoop ] << 4U) + arRdData->mDevFlg[ wkLoop ];
	}
	
	/* 警報フラグ4CH */
	vutRfDataBuffer.umLoggerResMeasure.mMeasureCrc[ wkBufCrcCnt ].mMeasure[ wkBufCnt ].mAlart_AllCh[ 3U ] = arRdData->m4chDevAlmFlg;
	
}


/*
 *******************************************************************************
 * リードした警報履歴を無線送信バッファに格納
 *
 *	[内容]
 *		Flashからリードした警報履歴を無線送信バッファに格納
 *	[引数]
 *		ST_FlashMeasAlm_t	arRdData: リードした計測値
 *		uint8_t				arLoop: リードした個数(100データ:0~99)
 *		uint8_t				arKosu: リードする個数
 *		uint16_t			arIndex: リードした先頭IndexNo.
 *	[戻値]
 *		なし
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
	static uint8_t			vDummyFlg;				/* 履歴をダミーにするかどうかのフラグ */
	
	/* リード初回 */
	if( arLoop == 0U )
	{
		memset( vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResAlarmHist) );
		
		vDummyFlg = imOFF;
		
		/* 警報履歴時系列ID */
		vutRfDataBuffer.umLoggerResAlarmHist.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResAlarmHist.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		/* パケット種別コード */
		vutRfDataBuffer.umLoggerResAlarmHist.mPacketCode = 0x03U;
	}
	
	if( vDummyFlg == imOFF )
	{
		/* 時刻情報初期値なら、以降の送信バッファはダミー */
		vutRfDataBuffer.umLoggerResAlarmHist.mDataNum = arLoop + 1U;	/* 有効データ数更新 */
		
		if( arRdData->mTimeInfo == 0x3FFFFFFFU )
		{
			vutRfDataBuffer.umLoggerResAlarmHist.mDataNum = arLoop;		/* 有効データ数:リード完了分 */
			vDummyFlg = imON;											/* 以降ダミーデータ */
		}
	}
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* 時刻情報(上位Bitから無線バッファに埋める) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
		vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* チャネル:0~1bit */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mCh_Measure_Event[ 0U ] = arRdData->mAlmChannel;
	
	/* 計測値:2~14bit */
	wkS16 = ((arRdData->mMeasVal + gvInFlash.mProcess.mModelOffset) & 0x003F) << 2U;
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mCh_Measure_Event[ 0U ] |= (uint8_t)wkS16;
	
	wkS16 = ((arRdData->mMeasVal + gvInFlash.mProcess.mModelOffset) & 0x1FC0) >> 6U;
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mCh_Measure_Event[ 1U ] = (uint8_t)wkS16;
	
	/* 論理:15bit */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mCh_Measure_Event[ 1U ] |= (arRdData->mEvent << 7U);
	
	/* アラームフラグ:0bit */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mAlarm = arRdData->mAlmFlg;
	
	/* レベル:1~2bit */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mAlarm |= (arRdData->mLevel << 1U);
	
	/* 警報項目:3~7bit */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mAlarm |= (arRdData->mItem << 3U);
	
	/* 予約 */
	vutRfDataBuffer.umLoggerResAlarmHist.mAlartHistCrc[ wkBufCrcCnt ].mAlHist[ wkBufCnt ].mReserve = 0U;
#endif
}


/*
 *******************************************************************************
 * リードした異常履歴を無線送信バッファに格納
 *
 *	[内容]
 *		Flashからリードした異常履歴を無線送信バッファに格納
 *	[引数]
 *		ST_FlashErrInfo_t	arRdData: リードした計測値
 *		uint8_t				arLoop: リードした個数(100データ:0~99)
 *		uint8_t				arKosu: リードする個数
 *		uint16_t			arIndex: リードした先頭IndexNo.
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiRfDrv_MeasErr_StorageRfBuff( ST_FlashErrInfo_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex )
{
#if (swKouteiMode == imDisable)
	uint8_t					wkLoop;
	uint8_t					wkBufCrcCnt;
	uint8_t					wkBufCnt;
	uint32_t				wkU32;
	static uint8_t			vDummyFlg;				/* 履歴をダミーにするかどうかのフラグ */
	
	/* リード初回 */
	if( arLoop == 0U )
	{
		memset( vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResAbnormalHist) );
		
		vDummyFlg = imOFF;
		
		/* 異常履歴時系列ID */
		vutRfDataBuffer.umLoggerResAbnormalHist.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResAbnormalHist.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		/* パケット種別コード */
		vutRfDataBuffer.umLoggerResAbnormalHist.mPacketCode = 0x04U;
	}
	
	if( vDummyFlg == imOFF )
	{
		/* 有効データ数 */
		vutRfDataBuffer.umLoggerResAbnormalHist.mDataNum = arLoop + 1U;		/* 有効データ数更新 */
		
		/* 時刻情報初期値なら、以降の送信バッファはダミー */
		if( arRdData->mTimeInfo == 0x3FFFFFFFU )
		{
			vutRfDataBuffer.umLoggerResAbnormalHist.mDataNum = arLoop;		/* 有効データ数:リード完了分 */
			vDummyFlg = imON;												/* 以降ダミーデータ */
		}
	}
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* 時刻情報(上位Bitから無線バッファに埋める) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
		vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[ wkBufCrcCnt ].mAbnormalHist[ wkBufCnt ].mTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* 機器異常種別コード:0~6bit */
	vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[ wkBufCrcCnt ].mAbnormalHist[ wkBufCnt ].mAbnormalCode_Flag = (uint8_t)arRdData->mItem & 0x7F;
	
	/* アラームフラグ:7bit */
	vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[ wkBufCrcCnt ].mAbnormalHist[ wkBufCnt ].mAbnormalCode_Flag |= ((arRdData->mAlmFlg & 0x01) << 7U);
	
	/* 予約 */
	vutRfDataBuffer.umLoggerResAbnormalHist.mAbnormalHistCrc[ wkBufCrcCnt ].mAbnormalHist[ wkBufCnt ].mReserve = 0U;
#endif
}



/*
 *******************************************************************************
 * リードしたイベント履歴を無線送信バッファに格納
 *
 *	[内容]
 *		Flashからリードしたイベント履歴を無線送信バッファに格納
 *	[引数]
 *		ST_FlashVal1Evt_t	arRdData: リードした計測値
 *		uint8_t				arLoop: リードした個数(60データ:0~59)
 *		uint8_t				arKosu: リードする個数
 *		uint16_t			arIndex: リードした先頭IndexNo.
 *	[戻値]
 *		なし
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
	static uint8_t			vDummyFlg;				/* 履歴をダミーにするかどうかのフラグ */
	
	/* リード初回 */
	if( arLoop == 0U )
	{
		memset( vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResEventHist) );
		
		vDummyFlg = imOFF;
		
		/* イベント履歴時系列ID */
		vutRfDataBuffer.umLoggerResEventHist.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResEventHist.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		/* パケット種別コード */
		vutRfDataBuffer.umLoggerResEventHist.mPacketCode = 0x05U;
	}
	
	
	if( vDummyFlg == imOFF )
	{
		vutRfDataBuffer.umLoggerResEventHist.mDataNum = arLoop + 1U;		/* 有効データ数更新 */
		
		/* 時刻情報初期値なら、以降の送信バッファはダミー */
		if( arRdData->mTimeInfo == 0x3FFFFFFFU )
		{
			vutRfDataBuffer.umLoggerResEventHist.mDataNum = arLoop;		/* 有効データ数:リード完了分 */
			vDummyFlg = imON;											/* 以降ダミーデータ */
		}
	}
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* 時刻情報(上位Bitから無線バッファに埋める) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
		vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* 計測値1~3CH */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		wkS16 = arRdData->mMeasVal[ wkLoop ] + gvInFlash.mProcess.mModelOffset;
		vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mMeasure_AllCh_Event[ wkLoop * 2U ] = (uint8_t)wkS16;
		vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mMeasure_AllCh_Event[ wkLoop * 2U + 1U ] = (uint8_t)((wkS16 & 0x1F00) >> 8U);
	}
	
	/* 論理 */
	vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mMeasure_AllCh_Event[ 5U ] |= (arRdData->mLogic << 7U);
	
	/* 警報フラグ1~3CH(上位4bit:警報、下位4bit:逸脱) */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mAlart_AllCh[ wkLoop ] = (arRdData->mAlmFlg[ wkLoop ] << 4U) + arRdData->mDevFlg[ wkLoop ];
	}
	
	/* 警報フラグ4CH */
	vutRfDataBuffer.umLoggerResEventHist.mEvtHistCrc[ wkBufCrcCnt ].mMeas_Evt[ wkBufCnt ].mAlart_AllCh[ 3U ] = arRdData->m4chDevAlmFlg;
#endif
}


/*
 *******************************************************************************
 * リードした動作履歴を無線送信バッファに格納
 *
 *	[内容]
 *		Flashからリードした動作履歴を無線送信バッファに格納
 *	[引数]
 *		ST_FlashSysLog_t	arRdData: リードした計測値
 *		uint8_t				arLoop: リードした個数(90データ:0~89)
 *		uint8_t				arKosu: リードする個数
 *		uint16_t			arIndex: リードした先頭IndexNo.
 *	[戻値]
 *		なし
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
	static uint8_t			vDummyFlg;				/* 履歴をダミーにするかどうかのフラグ */
	
	/* リード初回 */
	if( arLoop == 0U )
	{
		memset( &vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResOpeHist) );
		
		vDummyFlg = imOFF;
		
		pwkBuf = &vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[ 0U ].mOpeHist[ 0U ].mTime[ 0U ];
		for( wkClrLoop = 0U ; wkClrLoop < 90U ; wkClrLoop++, pwkBuf++ )
		{
			*pwkBuf = 0U;
		}
		
		/* 動作履歴時系列ID */
		vutRfDataBuffer.umLoggerResOpeHist.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResOpeHist.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		/* パケット種別コード */
		vutRfDataBuffer.umLoggerResOpeHist.mPacketCode = 0x06U;
	}
	
	if( vDummyFlg == imOFF )
	{
		vutRfDataBuffer.umLoggerResOpeHist.mDataNum = arLoop + 1U;		/* 有効データ数更新 */
		
		/* 時刻情報初期値なら、以降の送信バッファはダミー */
		if( arRdData->mTimeInfo == 0x3FFFFFFFU )
		{
			vutRfDataBuffer.umLoggerResOpeHist.mDataNum = arLoop;		/* 有効データ数:リード完了分 */
			vDummyFlg = imON;											/* 以降ダミーデータ */
		}
	}
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* 時刻情報(上位Bitから無線バッファに埋める) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
		vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[ wkBufCrcCnt ].mOpeHist[ wkBufCnt ].mTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* 履歴項目(下位Bitから無線バッファに埋める) */
	for( wkLoop = 0U ; wkLoop < 2U ; wkLoop++ )
	{
		wkU16 = arRdData->mItem >> (wkLoop * 8U);
		vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[ wkBufCrcCnt ].mOpeHist[ wkBufCnt ].mHistCode[ wkLoop ] = (uint8_t)wkU16;
	}
	
	/* 変更者ID */
	vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[ wkBufCrcCnt ].mOpeHist[ wkBufCnt ].mUserID = arRdData->mUserId;
	
	/* 履歴内容(下位Bitから無線バッファに埋める) */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		wkU32 = arRdData->mData >> (wkLoop * 8U);
		vutRfDataBuffer.umLoggerResOpeHist.mOpeHistCrc[ wkBufCrcCnt ].mOpeHist[ wkBufCnt ].mHistPrt[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
#endif
}



/*
 *******************************************************************************
 * リードしたシステム履歴を無線送信バッファに格納
 *
 *	[内容]
 *		Flashからリードしたシステム履歴を無線送信バッファに格納
 *	[引数]
 *		ST_FlashSysLog_t	arRdData: リードした計測値
 *		uint8_t				arLoop: リードした個数(90データ:0~89)
 *		uint8_t				arKosu: リードする個数
 *		uint16_t			arIndex: リードした先頭IndexNo.
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiRfDrv_SysLog_StorageRfBuff( ST_FlashSysLog_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex )
{
#if (swKouteiMode == imDisable)
	uint8_t					wkLoop;
	uint8_t					wkBufCrcCnt;
	uint8_t					wkBufCnt;
	uint32_t				wkU32;
	
	static uint8_t			vDummyFlg;				/* 履歴をダミーにするかどうかのフラグ */
	
	/* リード初回 */
	if( arLoop == 0U )
	{
		memset( vutRfDataBuffer.umData, 0, sizeof(vutRfDataBuffer.umLoggerResSysHist) );
		
		vDummyFlg = imOFF;
		
		/* システム履歴時系列ID */
		vutRfDataBuffer.umLoggerResSysHist.mHeaderID[ 0U ] = (uint8_t)(arIndex & 0x00FF);
		vutRfDataBuffer.umLoggerResSysHist.mHeaderID[ 1U ] = (uint8_t)((arIndex >> 8U) & 0x00FF);
		
		/* パケット種別コード */
		vutRfDataBuffer.umLoggerResSysHist.mPacketCode = 0x07U;
	}
	
	if( vDummyFlg == imOFF )
	{
		vutRfDataBuffer.umLoggerResSysHist.mDataNum = arLoop + 1U;		/* 有効データ数更新 */
		
		/* 時刻情報初期値なら、以降の送信バッファはダミー */
		if( arRdData->mTimeInfo == 0x3FFFFFFFU )
		{
			vutRfDataBuffer.umLoggerResSysHist.mDataNum = arLoop;		/* 有効データ数:リード完了分 */
			vDummyFlg = imON;											/* 以降ダミーデータ */
		}
	}
	
	wkBufCrcCnt = arLoop / 10U;
	wkBufCnt = arLoop % 10U;
	
	/* 時刻情報(上位Bitから無線バッファに埋める) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mTimeInfo >> (24U - wkLoop * 8U);
		vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[ wkBufCrcCnt ].mSysHist[ wkBufCnt ].mTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* 履歴項目 */
	vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[ wkBufCrcCnt ].mSysHist[ wkBufCnt ].mSysCode = arRdData->mItem;
	
	/* データ:24bit(下位Bitから無線バッファに埋める) */
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		wkU32 = arRdData->mData >> (wkLoop * 8U);
		vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[ wkBufCrcCnt ].mSysHist[ wkBufCnt ].mSysPrt[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* 予約 */
	vutRfDataBuffer.umLoggerResSysHist.mSysHistCrc[ wkBufCrcCnt ].mSysHist[ wkBufCnt ].mReserve = 0U;
#endif
}
#endif


#if (swLoggerBord == imDisable)
#if 0
/*
 *******************************************************************************
 *	接続ロガーを切断状態にする
 *
 *	[内容]
 *		接続ロガーを切断状態にする
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
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
			
			/* 切断されたロガーリストをModbus送信リストに追加 */
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
	
	/* 切断されたロガーリストをModbus送信リストに追加 */
	if( wkSize != 0U )
	{
		ApiModbus_SndFnc16Ref40301( 0U, wkSize, wkDelList );
	}
}
#endif

#if 0
/*
 *******************************************************************************
 *	接続ロガーを切断状態にする(個別切断)
 *
 *	[内容]
 *		接続ロガーを切断状態にする(個別切断)
 *	[引数]
 *		uint16_t arId:ロガーID
 *	[戻値]
 *		なし
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
					/* 接続台数が末尾のロガーの場合は総数-1 */
					vrfLoggerSumNumber--;
				}
			}
		}while(wkLoop > 0U);
	}
}
#endif

/*
 *******************************************************************************
 *	接続ロガーをリード、又は削除する
 *
 *	[内容]
 *		接続ロガーをリードし、Modbus通信リストにセットする
 *		又は、接続ロガーを一括切断し、Modbus通信リストにセットする
 *	[引数]
 *		uint8_t arSel:	0：リスト一括削除、1：接続リストリード
 *	[戻値]じ
 *		なし
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
			
			/* ロガーリストをModbus送信リストに追加 */
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
	
	/* ロガーリストをModbus送信リストに追加 */
	if( wkSize != 0U )
	{
		ApiModbus_SndFnc16Ref40301( arSel, wkSize, wkList );
	}
}

/*
 *******************************************************************************
 *	アプリID設定変更
 *
 *	[内容]
 *		アプリID設定変更
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
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
*  高速モードスリープ解除
*
*  [内容]
*  高速モードスリープ解除
********************************************************************************
*/
static void SubRFDrv_Hsmode_SleepFin( void )
{
	/* 高速通信スリープ解除 */
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
 *	グループID設定変更
 *
 *	[内容]
 *		グループID設定変更
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void ApiRfDrv_GrID_Chg( void )
{
	vrfMySerialID.mGroupID = gvInFlash.mParam.mGroupID;
}

/*
 *******************************************************************************
 *	無線通信間隔を利用してリアルタイム通信待受をOFFにする
 *
 *	[内容]
 *		無線通信間隔を利用してリアルタイム通信待受をOFFにする
 *	[引数]
 *		uint8_t arSel: 0:無線通信間隔リード、1:DISPキー押下による状態変更
 *						2:無線通信間隔初期化
 *	[戻値]
 *		uint16_t vrfRTmodeCommInt
 *******************************************************************************
 */
uint16_t ApiRfDrv_RtStbyOnOff( uint8_t arSel )
{
	switch( arSel )
	{
		/* 無線通信間隔リード */
		default:
		case 0U:
			break;
		/* DISPキー押下による状態変更 */
		case 1U:
			/* 現在の状態確認 */
			if( vrfRTmodeCommInt == 0xFFFF )
			{
				/* HS-Rt待受状態へ変更 */
				vrfRTmodeCommInt = vrfPreRTmodeCommInt;
				M_CLRBIT( gvInFlash.mParam.mParmFlg, imRtStbyOffFlg );
				
				/* 操作履歴(機内モードOFF、機器操作) */
				ApiFlash_WriteActLog( ecActLogItm_RfAirplaneOff, 0U, 4U );
				
			}
			else
			{
				/* 現在のRt待受周期を記憶 */
				vrfPreRTmodeCommInt = vrfRTmodeCommInt;
				
				/* HSのみ待受状態へ変更 */
				vrfRTmodeCommInt = 0xFFFF;
				M_SETBIT( gvInFlash.mParam.mParmFlg, imRtStbyOffFlg );
				
				/* RtのONLINE表示をOFF */
				SubRfDrv_SetRtConInfo( imOFF );
				vrfRTmodeCommFailNum = vrfRTmodeCommCutTime;					/* 2021.9.22 切断処理方法の変更 */
				vrfRtmodeStopCnt = 0U;
				
				/* 操作履歴(機内モードON、機器操作) */
				ApiFlash_WriteActLog( ecActLogItm_RfAirplaneOn, 0U, 4U );
				
				/* 高速通信スリープ解除 */
				vrfHsmodeSleepMode = 0x00U;
			}
			gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;	/* 内蔵Flash書込み更新 */
			break;
		/* 無線通信間隔初期化 */
		case 2U:
			if( M_TSTBIT( gvInFlash.mParam.mParmFlg, imRtStbyOffFlg ) )
			{
				vrfRTmodeCommInt = 0xFFFF;
			}
			else
			{
				vrfRTmodeCommInt = 300U;					/* 初期値1min周期の通信間隔 */
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
 *	ファームアップデータ格納
 *
 *	[内容]
 *		ファームアップデータ格納
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
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
				/* CRC比較 */
				if( arRfBuff->mReserve[ 0U ] == wkCrc[ 0U ] && arRfBuff->mReserve[ 1U ] == wkCrc[ 1U ] )
				{
					wkError = ecERR_OK;
					
					/* ログインユーザID */
					gvInFlash.mProcess.mUpUserId = (arRfBuff->mReserve[ 2U ] << 8U) + arRfBuff->mReserve[ 3U ];
					
					/* ファーム更新日時 */
					gvInFlash.mProcess.mUpDate[ 0U ] = gvClock.mYear;	/* 年 */
					gvInFlash.mProcess.mUpDate[ 1U ] = gvClock.mMonth;	/* 月 */
					gvInFlash.mProcess.mUpDate[ 2U ] = gvClock.mDay;	/* 日 */
					
					/* ファームアップ実行のためステータス遷移 */
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
 *	RF通信インジケータLEDをON/OFFする
 *
 *	[内容]
 *		RF通信インジケータLEDをON/OFFする
 *	[引数]
 *		uint8_t arLedCntrl:	imON(1)：LED点灯、imOFF(0)：LED消灯
 *	[戻値]
 *		なし
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
 *	高速通信モード　GW呼出信号の検知動作　繰り返し処理
 *
 *	[内容]
 *		高速通信での呼出信号検知のため、一定回数(RF_HSMODE_WAKEUP_TIME)、
 *		一定間隔(RF_HSMODE_DECT_INT)で起動する。
 * 		この関数に入る時点で１回起動済み。
 * 		一定回数実施後はアイドルに戻る。
 * 		一定回数(RF_HSMODE_WAKEUP_SET)でSync切り替え。
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
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
		/* リトライ処理 125msec Sleep + 19msec Halt */
		wkRadio = gvRfTimingCounterL;			/* gvRfTimingCounterLは0-7で繰り返す */
		if( wkRadio < 7U )
		{
			while( gvRfTimingCounterL < wkRadio + 1U )
			{
				/* コンパレータ実行中はSTOPからの起動による指示ズレ防止として：HALT */
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
				/* コンパレータ実行中はSTOPからの起動による指示ズレ防止として：HALT */
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
			/* 高速通信スリープ解除 */
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
 *	高速通信モード　呼び出しビーコン後のレスポンス受信後のWait
 *
 *	[内容]
 *		高速通信でのビーコン送出後のロガーからのレスポンスを受信し、
 *		受信後のWaitを調整し、タイミングをあわせる。
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void SubRfDrv_Hsmode_ResRx_AfterWait( void )
{
	uint16_t wkCulc;
	if( gvrfStsEventFlag.mHsmodeRxEnter == 0U )
	{
		/* Ack送信の50msec Wait */
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
 *	デバッグ用　RSSI(エラーコードvrfErorrCause)と接続先GWのログ(2ch,3ch)する値を変更
 *	[内容]
 *		
 *		
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
 *******************************************************************************
 */
void SubRfDrv_RssiLogDebug( void )
{
	/* RSSIをCH2に格納 */
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
	/* 接続GWをCH3に格納 */
	gvMeasPrm.mMeasVal[ 2U ] = gvInFlash.mData.mMeas1_OldPtrIndex & 0x1FFF;
	gvMeasPrm.mMeasVal[ 2U ] = ((vrfCommTargetID[ 2U ] >> 4U) & 0x0FU) + ((vrfCommTargetID[ 1U ] << 4U) & 0xF0U);
#endif
}
#endif

#if (swLoggerBord == imEnable)
/*
 *******************************************************************************
 *	高速通信モード　パケット受信後の再送信遷移処理
 *	[内容]
 *		ROM削減のため、共通箇所をまとめる
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
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
 *	長距離通信モード　接続通信の終了処理
 *	[内容]
 *		ROM削減のため、共通箇所をまとめる　2022.9.16
 *	[引数]
 *		なし
 *	[戻値]
 *		なし
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
