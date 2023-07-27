/*
 *******************************************************************************
 *	File name	:	RF_Struct.h
 *
 *	[内容]
 *		無線通信部 構造体・共用体
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2020.4.13
 *******************************************************************************
 */

#ifndef	INCLUDED_RF_STRUCT_H
#define	INCLUDED_RF_STRUCT_H


#include "RF_Enum.h"
#include "switch.h"

/* 構造体メンバのアライメントを1にする */
#pragma pack

/*
 *==============================================================================
 *	構造体定義
 *==============================================================================
 */
typedef struct ST_RFChGrupe
{
	uint8_t		mCh[2U];					/* 無線通信チャネルグループ */
	uint8_t		mLoraSync;
}ST_RFChGrupe_t;

typedef struct ST_RFPrm
{
	ET_RFDrvInitSts_t	mInitSts;												/* 初期化ステータス */
	uint8_t				mInitWaitCnt;											/* イニシャライズ待ちカウンタ(10ms/カウント) */
} ST_RFPrm_t;


/*!
 * FSK bandwidth definition
 */
typedef struct
{
	uint32_t bandwidth;
	uint8_t  RegValue;
} FskBandwidth_t;


/*!
 * Radio registers definition
 */
typedef struct RadioRegisters
{
	RadioModems_t Modem;
	uint8_t		Addr;
	uint8_t		Value;
} RadioRegisters_t;


/*
  *RF状態の定義
*/
typedef struct
{
	RadioModems_t Modem;					  /* FSK or LoRa */
	RFState_t State;						  /* RFのState RX,TXの設定状態含む */
} ST_RFStatus_t;


/*
  無線CALL信号の応答カウントダウン定義(送信用)
*/
typedef struct
{
	uint8_t upper;
	uint8_t lower;
} ST_RF_Call_Count_t;

typedef struct ST_Rf_Rtmode_Tx_Wait
{
	uint8_t mTime;
	uint8_t mNum;
}ST_Rf_Rtmode_Tx_Wait_t;

/*
  無線CALL信号の受信データ定義
*/
typedef struct
{
	uint8_t GwUniquID[2U];
	uint8_t Command;
	uint8_t Ch;
	uint8_t Sec;
	uint8_t Min;
	uint8_t Hour;
	uint8_t Day;
	uint8_t Week;
	uint8_t Month;
	uint8_t Year;
	uint8_t Bch1st[4U];
	uint8_t CallID[3U];
	uint8_t OnlineAuth;
	uint8_t Parameter[3U];
	uint8_t Countdown[2U];
	uint8_t Crc[2U];
	uint8_t Bch2nd[4U];
} ST_RF_Call_Rx_Data_t;


/* 計測データ構造体 */
typedef struct ST_RF_Measure_Data
{
	uint8_t mEvent_Measure[2U];
	uint8_t mAlart;
} ST_RF_Measure_Data_t;

typedef struct
{
	uint8_t mGroupID;
	uint8_t mOpeCompanyID;		/* 事業会社ID 8bit */
	uint8_t mAppID[2U];			/* 利用会社ID(アプリID)12bit , [2]の下位4bitはNULL	*/
	uint8_t mUniqueID[3U];		/* [3U]の下位4bitはNULL 20bit */
} ST_RF_SerialID_t;


/* BCH前のインターリーブ用定義 */
typedef struct
{
	uint16_t Mask;
	int8_t Shift;
} ST_RF_PreBCH_t;


/***************************************
*  配列[0U]がMSBとなるように配置
****************************************/
/* HS mode 無線パケットNo20 ブロードキャスト */
typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mPacketCode;
	uint8_t mCommand;
	uint8_t mWaitCount[2U];
	
	uint8_t mTime[4U];
	uint8_t mOnline;
	uint8_t mLoggerID[3U];
	uint8_t mGroupID;
	uint8_t mRequestHeaderMeasureTime[4U];
	
	uint8_t mRequestHeaderMeasureID[2U];
	uint8_t mReserve[2U];
	uint8_t mAppID[2U];
	uint8_t mSleepMode;
	uint8_t mCompanyID;
} ST_RF_Gw_Query_Beacon_t;

/* HS Mode 無線パケットNo21 ステータス応答 */
typedef struct
{
	uint8_t mNewDataId[2U];
	uint8_t mOldDataId[2U];
	uint8_t mOldTime[4U];
} ST_RF_Logger_TimeId_t;

typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
	uint8_t mGroupID;
	uint8_t mLoggerName[20U];
	uint8_t mSensorType;
	uint8_t mVersion[3U];
	uint8_t mStatusTime[4U];
	uint8_t mStatusMeasure_AllCh_Event[5U];
	uint8_t mStatusAbnormal;
	uint8_t mAlart_AllCh[4U];
	uint8_t mPacketCode;
	uint8_t mAppID[2U];
	uint8_t mSetTime[4U];
	ST_RF_Logger_TimeId_t mMeasId;
	ST_RF_Logger_TimeId_t mAlId;
	ST_RF_Logger_TimeId_t mOpId;
	ST_RF_Logger_TimeId_t mEvId;
	ST_RF_Logger_TimeId_t mAbId;
	ST_RF_Logger_TimeId_t mSysId;
	uint8_t mReserve[2U];
} ST_RF_Logger_Responce_Status_t;


/* 無線パケットNo22 ロガー計測値応答 */
typedef struct
{
	uint8_t mMeasure_AllCh_Event[6U];
	uint8_t mAlart_AllCh[4U];
	uint8_t mBch[5U];
} ST_RF_Logger_Measure_t;

typedef struct
{
	ST_RF_Logger_Measure_t mMeasure[10U];
	uint8_t mCrcMeasure[2U];
} ST_RF_Logger_MeasureCrc_t;		/* 150byteごとにCRC */

typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
//	uint8_t mPacketCode;
	uint8_t mDataNum;
	uint8_t mHeaderTime[4U];
	uint8_t mHeaderID[2U];
	uint8_t mRecInt_PckCode;
	uint8_t mBch[8U];					/* 追加 */
	ST_RF_Logger_MeasureCrc_t mMeasureCrc[9U];
} ST_RF_Logger_Responce_Measure_t;

/* HS Mode 無線パケットNo23 警報履歴データ応答 */
typedef struct
{
	uint8_t mTime[4U];
	uint8_t mCh_Measure_Event[2U];
	uint8_t mAlarm;
	uint8_t mReserve;
	uint8_t mBch[4U];
} ST_RF_Logger_AlartHist_t;

typedef struct 
{
	ST_RF_Logger_AlartHist_t mAlHist[10U];
	uint8_t mCrc[2U];
} ST_RF_Logger_AlHistCrc_t;

typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
	uint8_t mPacketCode;
	uint8_t mDataNum;
	uint8_t mHeaderID[2U];
	ST_RF_Logger_AlHistCrc_t mAlartHistCrc[10U];
} ST_RF_Logger_Responce_AlartHist_t;


/* HS Mode 無線パケットNo24 機器異常履歴データ応答 */
typedef struct
{
	uint8_t mTime[4U];
	uint8_t mAbnormalCode_Flag;
	uint8_t mReserve;
	uint8_t mBch[3U];
} ST_RF_Logger_AbnormalHist_t;

typedef struct 
{
	ST_RF_Logger_AbnormalHist_t mAbnormalHist[10U];
	uint8_t mCrc[2U];
} ST_RF_Logger_AbnormalHistCrc_t;

typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
	uint8_t mPacketCode;
	uint8_t mDataNum;
	uint8_t mHeaderID[2U];
	ST_RF_Logger_AbnormalHistCrc_t mAbnormalHistCrc[10U];
} ST_RF_Logger_Responce_AbnormalHist_t;


/* HS Mode 無線パケットNo25 イベントデータ応答 */
typedef struct
{
	uint8_t mTime[4U];
	uint8_t mMeasure_AllCh_Event[6U];
	uint8_t mAlart_AllCh[4U];
		uint8_t mBch[7U];
} ST_RF_Logger_EventHist_t;

typedef struct
{
	ST_RF_Logger_EventHist_t mMeas_Evt[10U];
	uint8_t mCrc[2U];
} ST_RF_Logger_EventHistCrc_t;

typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
	uint8_t mPacketCode;
	uint8_t mDataNum;
	uint8_t mHeaderID[2U];
	ST_RF_Logger_EventHistCrc_t mEvtHistCrc[6U];
} ST_RF_Logger_Responce_EventHist_t;


/* HS Mode 無線パケットNo26 動作履歴データ応答 */
typedef struct
{
	uint8_t mTime[4U];
	uint8_t mHistCode[2U];
	uint8_t mUserID;
	uint8_t mHistPrt[3U];
	uint8_t mBch[5U];
} ST_RF_Logger_OpeHist_t;

typedef struct
{
	ST_RF_Logger_OpeHist_t mOpeHist[10U];
	uint8_t mCrc[2U];
} ST_RF_Logger_OpeHistCrc_t;

typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
	uint8_t mPacketCode;
	uint8_t mDataNum;
	uint8_t mHeaderID[2U];
	ST_RF_Logger_OpeHistCrc_t mOpeHistCrc[9U];
} ST_RF_Logger_Responce_OpeHist_t;


/* HS mode 無線パケットNo27 システム履歴データ	*/
typedef struct
{
	uint8_t mTime[4U];
	uint8_t mSysCode;
	uint8_t mSysPrt[4U];
	uint8_t mReserve;
	uint8_t mBch[5U];
} ST_RF_Logger_SysHist_t;

typedef struct
{
	ST_RF_Logger_SysHist_t mSysHist[10U];
	uint8_t mCrc[2U];
} ST_RF_Logger_SysHistCrc_t;

typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
	uint8_t mPacketCode;
	uint8_t mDataNum;
	uint8_t mHeaderID[2U];
	ST_RF_Logger_SysHistCrc_t mSysHistCrc[9U];
} ST_RF_Logger_Responce_SysHist_t;


/* HS mode 無線パケットNo28 設定読み込み */
typedef struct
{
	uint8_t mUUAlarmValue[2U];
	uint8_t mUUAlarmDelay;
	uint8_t mUAlarmValue[2U];
	uint8_t mUAlarmDelay;
	uint8_t mLAlarmValue[2U];
	uint8_t mLAlarmDelay;
	uint8_t mLLAlarmValue[2U];
	uint8_t mLLAlarmDelay;
	uint8_t mToleranceLevel;
	uint8_t mToleranceTime[2U];
	uint8_t mMeasureOffset;
	uint8_t mMeasureZeroSpan;
} ST_RF_Logger_Ch_Setting_t;

typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
	uint8_t mPacketCode;
	ST_RF_Logger_Ch_Setting_t mChSetting[4U];
	uint8_t	mBattCode;
	uint8_t	mKeylock;
	uint8_t	mRtModeCh;
	uint8_t	mRecordIntMain;
	uint8_t	mRecordIntAlarm;
	uint8_t	mLoggerName[20U];
	uint8_t	mHsModeCh;
	uint8_t	mHSmodeWakeUpLogValue[2U];
	uint8_t	mAlarmCulcCode;
	uint8_t	mAlarmCulcPrmFlag;
	uint8_t	mEventCtrl;
	uint8_t	mRtModeDisConVal;
	uint8_t	mRtModeTxPoint[2U];
	uint8_t	mToleranceClarMeth;
	uint8_t	mToleranceClarTime[2U];
	uint8_t	mCalDate[2U];
	uint8_t	mScalDecimal;
	uint8_t	mScalYInt[2U];
	uint8_t	mScalGrad[2U];
	uint8_t	mMemorySelect;
	uint8_t mCommGwID[10U][3U];
	uint8_t mGroupID;
//	uint8_t mReserve[3U];
//	uint8_t mUserID;
	uint8_t mSetTime[4U];
	uint8_t mCrc2[2U];
	uint8_t mBch[75U];
} ST_RF_Logger_SettingRes_t;

/* HS mode 無線パケットNo29 設定値要求 */
typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
	uint8_t mGroupID;
	uint8_t mPacketCode;
	uint8_t mUserID;
	uint8_t mReserve[9U];
}ST_RF_Logger_QuerySet_t;


typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
	uint8_t mPacketCode;
	ST_RF_Logger_Ch_Setting_t mChSetting[4U];
	uint8_t	mBattCode;
	uint8_t	mKeylock;
	uint8_t	mRtModeCh;
	uint8_t	mRecordIntMain;
	uint8_t	mRecordIntAlarm;
	uint8_t	mLoggerName[20U];
	uint8_t	mHsModeCh;
	uint8_t	mHSmodeWakeUpLogValue[2U];
	uint8_t	mAlarmCulcCode;
	uint8_t	mAlarmCulcPrmFlag;
	uint8_t	mEventCtrl;
	uint8_t	mRtModeDisConVal;
	uint8_t	mRtModeTxPoint[2U];
	uint8_t	mToleranceClarMeth;
	uint8_t	mToleranceClarTime[2U];
	uint8_t	mCalDate[2U];
	uint8_t	mScalDecimal;
	uint8_t	mScalYInt[2U];
	uint8_t	mScalGrad[2U];
	uint8_t	mMemorySelect;
	uint8_t mCommGwID[10U][3U];
	uint8_t mGroupID;
	uint8_t mReserve[3U];
	uint8_t mUserID;
//	uint8_t mSetTime[4U];
	uint8_t mCrc2[2U];
	uint8_t mBch[75U];
} ST_RF_Logger_SettingWt_t;

/* HS mode Logger　通信ACK/NACK */
typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
	uint8_t mGroupID;
	uint8_t mPacketCode;
	uint8_t mResponce;
	uint8_t mSetTime[ 4U ];
	uint8_t mReserve[ 5U ];
}ST_RF_Logger_AckNack_t;

/* HS mode Firm HEX */
typedef struct
{
	uint8_t mGwID[3U];
	uint8_t mCrc[2U];
	uint8_t mLoggerID[3U];
	uint8_t mPacketCode;
	uint8_t mPacketNo[2U];
	uint8_t mPacketSum[2U];
	uint8_t mReserve[6U];
	uint8_t mHex[890U];
	uint8_t mCrc2[2U];
	uint8_t mBch[450U];
}ST_RF_Gw_Hex_Tx_t;


/*****************************************************/

/* RTmode 修正 No1 ビーコン送信 */
typedef struct ST_RF_RT_Beacon
{
	uint8_t mCompanyID;
	uint8_t mAppID_Online[2U];
	uint8_t mReserve[2U];
	uint8_t mCrc[2U];
} ST_RF_RT_Beacon_t;


/* RTmode 修正 No2 ロガー接続許可 */
typedef struct ST_RF_RT_ConnectAuth
{
	uint8_t mCompanyID;
	uint8_t mAppID_GwUniqueID[2U];
	uint8_t mGwUniqueID[2U];
	uint8_t mComInt;
	uint8_t mSyncTimerL[2U];
	uint8_t mSyncTimerH[2U];
	uint8_t mTimeSlot;
	uint8_t mChGrupe[2U];
	uint8_t mOnline_Time[4U];
	uint8_t mComOffTime;
	uint8_t mRecStartTime[4U];
	uint8_t mReserve[3U];
	uint8_t mCrc[2U];
	uint8_t mDummy[256U - 27U];
	uint8_t mTxCompanyID;
	uint8_t mTxAppID_GwUniqueID[2U];
	uint8_t mTxGwUniqueID[2U];
	uint8_t mTxComInt;
	uint8_t mTxSyncTimerL[2U];
	uint8_t mTxSyncTimerH[2U];
	uint8_t mTxTimeSlot;
	uint8_t mTxChGrupe[2U];
	uint8_t mTxOnline_Time[4U];
	uint8_t mTxComOffTime;
	uint8_t mTxRecStartTime[4U];
	uint8_t mTxReserve[3U];
	uint8_t mTxCrc[2U];
} ST_RF_RT_ConnectAuth_t;


/* RTmode 修正 No3 ロガー情報 */
typedef struct ST_RF_RT_LoggerInfo
{
	uint8_t mLoggerID[3U];
	uint8_t mLoggerName[20U];
	uint8_t mGroupID;
	uint8_t mGwID[3U];
	uint8_t mLoggerSensorType;
	uint8_t mLastUpperLimitAlarm[7U];
	uint8_t mLastUpperLimitDelay[3U];
	uint8_t mUpperLimitAlarm[7U];
	uint8_t mUpperLimitDelay[3U];
	uint8_t mLowerLimitAlarm[7U];
	uint8_t mLowerLimitDelay[3U];
	uint8_t mLastLowerLimitAlarm[7U];
	uint8_t mLastLowerLimitDelay[3U];
	uint8_t mAlarmDeviationTime[4U];
	uint8_t mRecInterval;							/* 収録間隔1 上位4bit,収録間隔2 下位4bit */
	uint8_t mCalcAlarmKind;
	uint8_t mCalcAlarmParameter;
	uint8_t mMeasOldId[2U];
	uint8_t mMeasOldTime[4U];
	uint8_t mAlHistOldId[2U];
	uint8_t mAlHistOldTime[4U];
	uint8_t mVer[2U];
	uint8_t mCrc[2U];
	uint8_t mDummy[256U - 91U];
	
	uint8_t mBuffLoggerID[3U];
	uint8_t mBuffLoggerName[20U];
	uint8_t mBuffGrupeID;
	uint8_t mBuffGwID[3U];
	uint8_t mBuffLoggerSensorType;
	uint8_t mBuffLastUpperLimitAlarm[7U];
	uint8_t mBuffLastUpperLimitDelay[3U];
	uint8_t mBuffUpperLimitAlarm[7U];
	uint8_t mBuffUpperLimitDelay[3U];
	uint8_t mBuffLowerLimitAlarm[7U];
	uint8_t mBuffLowerLimitDelay[3U];
	uint8_t mBuffLastLowerLimitAlarm[7U];
	uint8_t mBuffLastLowerLimitDelay[3U];
	uint8_t mBuffAlarmDeviationTime[4U];
	uint8_t mBuffRecInterval;							/* 収録間隔1 上位4bit,収録間隔2 下位4bit */
	uint8_t mBuffCalcAlarmKind;
	uint8_t mBuffCalcAlarmParameter;
	uint8_t mBuffMeasOldId[2U];
	uint8_t mBuffMeasOldTime[4U];
	uint8_t mBuffAlHistOldId[2U];
	uint8_t mBuffAlHistOldTime[4U];
	uint8_t mBuffVer[2U];
} ST_RF_RT_LoggerInfo_t;


/* RTmode 修正 No4 接続手順ACK */
typedef struct ST_RF_RT_ConnectAck
{
	uint8_t mLoggerID[3U];
	uint8_t mGwID_Sequence[3U];
	uint8_t mTimeSlot;
	uint8_t mReserve[2U];
	uint8_t mCrc[2U];
} ST_RF_RT_ConnectAck_t;


/* RTmode No5 計測データのリアルタイム送信 */
typedef struct ST_RF_RT_Measure_Tx
{
	uint8_t mCompanyID;
	uint8_t mLoggerID[3U];
	uint8_t mAppID;
	uint8_t mGwID[3U];
	uint8_t mDataCntKind;
	uint8_t mAbnormalSratus;
	uint8_t mNewMeasureTime[4U];
	uint8_t mNewMeasureID[2U];
	uint8_t mNewMeasureValue[5U];
	uint8_t mNewAlarmFlag[2U];
	/* ここまでNo.10と同じ */
	uint8_t mHeaderTime[4U];
	uint8_t mHeaderID[2U];
	uint8_t mMeasureIntFlag[1U];
	uint8_t mCrc[2U];
	uint8_t mMeasureData[9U][7U];
	uint8_t mDummy[256U - 95U];
	uint8_t mBuffCompanyID;
	uint8_t mBuffLoggerID[3U];
	uint8_t mBuffAppID;
	uint8_t mBuffGwID[3U];
	uint8_t mBuffDataCntKind;
	uint8_t mBuffAbnormalSratus;
	uint8_t mBuffNewMeasureTime[4U];
	uint8_t mBuffNewMeasureID[2U];
	uint8_t mBuffNewMeasureValue[5U];
	uint8_t mBuffNewAlarmFlag[2U];
	uint8_t mBuffHeaderTime[4U];
	uint8_t mBuffHeaderID[2U];
	uint8_t mBuffMeasureIntFlag[1U];
	uint8_t mBuffCrc[2U];
	uint8_t mBuffMeasureData[9U][7U];
} ST_RF_RT_Measure_Tx_t;

/* RTmode No10 履歴データのリアルタイム送信 */
typedef struct ST_RF_RT_Audit_Tx
{
	uint8_t mCompanyID;
	uint8_t mLoggerID[3U];
	uint8_t mAppID;
	uint8_t mGwID[3U];
	uint8_t mDataCntKind;
	uint8_t mAbnormalSratus;
	uint8_t mNewMeasureTime[4U];
	uint8_t mNewMeasureID[2U];
	uint8_t mNewMeasureValue[5U];
	uint8_t mNewAlarmFlag[2U];
	/* ここまでNo5と同じ */
	uint8_t mPastAlarmFL;
	uint8_t mHeaderID[2U];
	uint8_t mReserve;
	uint8_t mCrc[2U];
	uint8_t mMeasureData[6U][11U];
	uint8_t mDummy[256U - 95U];
	uint8_t mBuffCompanyID;
	uint8_t mBuffLoggerID[3U];
	uint8_t mBuffAppID;
	uint8_t mBuffGwID[3U];
	uint8_t mBuffDataCntKind;
	uint8_t mBuffAbnormalSratus;
	uint8_t mBuffNewMeasureTime[4U];
	uint8_t mBuffNewMeasureID[2U];
	uint8_t mBuffNewMeasureValue[5U];
	uint8_t mBuffNewAlarmFlag[2U];
	uint8_t mBuffPastAlarmFL;
	uint8_t mBuffHeaderID[2U];
	uint8_t mBuffReserve;
	uint8_t mBuffCrc[2U];
	uint8_t mBuffMeasureData[6U][11U];
} ST_RF_RT_Audit_Tx_t;


/* RTmode No6 計測データのリアルタイム送信へのACK */
/*
typedef struct ST_RF_RT_Measure_Ack
{
	uint8_t mLoggerID[3U];
	uint8_t mSequenceSettingDisConnect;
	uint8_t mSyncTimer[3U];
	uint8_t mTime[2U];
	uint8_t mCrc[2U];
} ST_RF_RT_Measure_Ack_t;
*/
typedef struct ST_RF_RT_Measure_Ack
{
	uint8_t mCompanyID;
	uint8_t mLoggerID[3U];
	uint8_t mSettingDisConnectTime[2U];
	uint8_t mSyncTimer[3U];
	uint8_t mCrc[2U];
} ST_RF_RT_Measure_Ack_t;


/* RTmode No1 設定変更予約ビーコン */
typedef struct ST_RF_RT_ChangeSet_Beacon
{
	uint8_t mLoggerID1[2U];
	uint8_t mOnline_Flag;
	uint8_t mLoggerID2;
	uint8_t mReserve;
	uint8_t mCrc[2U];
	uint8_t mDummy[256U - 7U];
	uint8_t mBuffLoggerID1[2U];
	uint8_t mBuffOnline_Flag;
	uint8_t mBuffLoggerID2;
	uint8_t mBuffReserve;
} ST_RF_RT_ChangeSet_Beacon_t;


/* RTmode No9 設定変更ID通知 (削除) */
/*
typedef struct ST_RF_RT_ChangeSet_ID
{
	uint8_t mGwID[3U];
	uint8_t mLoggerID[3U];
	uint8_t mReserve[3U];
	uint8_t mCrc[2U];
	uint8_t mBch[4U];
} ST_RF_RT_ChangeSet_ID_t;
*/

/* RTmode No7 ロガー設定変更１ */
typedef struct ST_RF_RT_ChangeSet_Prm1
{
	uint8_t mLoggerID[3U];
	uint8_t mGwID[3U];
	uint8_t mPermitGwID[10U][3U];
	uint8_t mGroupID;
	uint8_t mUserID;
	uint8_t mOnline_Batt_Key;
	uint8_t mReserve[2U];
	uint8_t mCrc[2U];
	uint8_t mDummy[64U - 43U];
	uint8_t mBuffLoggerID[3U];
	uint8_t mBuffGwID[3U];
	uint8_t mBuffPermitGwID[10U][3U];
	uint8_t mBuffGrupeID;
	uint8_t mBuffUserID;
	uint8_t mBuffOnline_Batt_Key;
	uint8_t mBuffReserve[2U];
} ST_RF_RT_ChangeSet_Prm1_t;

#if 0
/* RTmode No7 ロガー設定変更2 */
typedef struct ST_RF_RT_ChangeSet_Prm2
{
	uint8_t mUUAlarmValue[7U];
	uint8_t mUUAlarmDelay[3U];
	uint8_t mUAlarmValue[7U];
	uint8_t mUAlarmDelay[3U];
	uint8_t mLAlarmValue[7U];
	uint8_t mLAlarmDelay[3U];
	uint8_t mLLAlarmValue[7U];
	uint8_t mLLAlarmDelay[3U];
	uint8_t mToleranceTime[4U];
	uint8_t mRecordInt;
	uint8_t mMeasureOffset[3U];
	uint8_t mMeasureZeroSpan[3U];
	uint8_t mLoggerName[20U];
	uint8_t mHSmodeCh;
	uint8_t mHSmodeWakeUpLogValue[2U];
	uint8_t mAlarmCulcSet[2U];
	uint8_t mAlCulcFlag_EventCont_MemorySet;
	uint8_t mReserve[4U];
	uint8_t mCrc[2U];
	uint8_t mDummy[512U - 83U];
	uint8_t mBuffUUAlarmValue[7U];
	uint8_t mBuffUUAlarmDelay[3U];
	uint8_t mBuffUAlarmValue[7U];
	uint8_t mBuffUAlarmDelay[3U];
	uint8_t mBuffLLAlarmValue[7U];
	uint8_t mBuffLLAlarmDelay[3U];
	uint8_t mBuffLAlarmValue[7U];
	uint8_t mBuffLAlarmDelay[3U];
	uint8_t mBuffToleranceTime[4U];
	uint8_t mBuffRecordInt;
	uint8_t mBuffMeasureOffset[3U];
	uint8_t mBuffMeasureZeroSpan[3U];
	uint8_t mBuffLoggerName[20U];
	uint8_t mBuffHSmodeCh;
	uint8_t mBuffHSmodeWakeUpLogValue[2U];
	uint8_t mBuffAlarmCulcSet[2U];
	uint8_t mBuffAlCulcFlag_EventCont_MemorySet;
	uint8_t mBuffReserve[4U];
} ST_RF_RT_ChangeSet_Prm2_t;
#endif

/* RTmode No7 ロガー設定変更2 */
typedef struct ST_RF_RT_ChangeSet_Prm2
{
	uint8_t mRtModeCh;
	uint8_t mUUAlarmValue[7U];
	uint8_t mUUAlarmDelay[3U];
	uint8_t mUAlarmValue[7U];
	uint8_t mUAlarmDelay[3U];
	uint8_t mLAlarmValue[7U];
	uint8_t mLAlarmDelay[3U];
	uint8_t mLLAlarmValue[7U];
	uint8_t mLLAlarmDelay[3U];
	uint8_t mCrc[2U];
	uint8_t mDummy[128U - 43U];
	uint8_t mBuffRtModeCh;
	uint8_t mBuffUUAlarmValue[7U];
	uint8_t mBuffUUAlarmDelay[3U];
	uint8_t mBuffUAlarmValue[7U];
	uint8_t mBuffUAlarmDelay[3U];
	uint8_t mBuffLAlarmValue[7U];
	uint8_t mBuffLAlarmDelay[3U];
	uint8_t mBuffLLAlarmValue[7U];
	uint8_t mBuffLLAlarmDelay[3U];
}ST_RF_RT_ChangeSet_Prm2_t;

/* RTmode No7 ロガー設定変更3 */
typedef struct ST_RF_RT_ChangeSet_Prm3
{
	uint8_t mToleranceTime[4U];
	uint8_t mRecordInt;
	uint8_t mMeasureOffset[3U];
	uint8_t mMeasureZeroSpan[3U];
	uint8_t mLoggerName[20U];
	uint8_t mHSmodeCh;
	uint8_t mHSmodeWakeUpLogValue[2U];
	uint8_t mAlarmCulcSet[2U];
	uint8_t mAlCulcFlag_EventCont_MemorySet;
	uint8_t mCalOffset[2U];
	uint8_t mCalZeroSpan[2U];
	uint8_t mCrc[2U];
	uint8_t mDummy[192U - 43U];
	uint8_t mBuffToleranceTime[4U];
	uint8_t mBuffRecordInt;
	uint8_t mBuffMeasureOffset[3U];
	uint8_t mBuffMeasureZeroSpan[3U];
	uint8_t mBuffLoggerName[20U];
	uint8_t mBuffHSmodeCh;
	uint8_t mBuffHSmodeWakeUpLogValue[2U];
	uint8_t mBuffAlarmCulcSet[2U];
	uint8_t mBuffAlCulcFlag_EventCont_MemorySet;
	uint8_t mBuffCalOffset[2U];
	uint8_t mBuffCalZeroSpan[2U];
} ST_RF_RT_ChangeSet_Prm3_t;

/* RTmode No9 設定変更Ack */
typedef struct ST_RF_RT_ChangeSet_Ack
{
	uint8_t mLoggerID[3U];
	uint8_t mGwID[3U];
	uint8_t mSequenceNo;
	uint8_t mReserve[2U];
	uint8_t mCrc[2U];
} ST_RF_RT_ChangeSet_Ack_t;


/*
*-------------------------------------------------------------------------------
*　共用体
*-------------------------------------------------------------------------------
*/
typedef union UT_RF_Data
{
	uint8_t umData[ 1512U ];
//	ST_RF_BroadCast_Response_t umBroadCast_Response;
	ST_RF_Gw_Query_Beacon_t umGwQueryBeacon;
	ST_RF_Logger_Responce_Status_t umLoggerResStatus;
	ST_RF_Logger_Responce_Measure_t umLoggerResMeasure;
	ST_RF_Logger_Responce_AlartHist_t umLoggerResAlarmHist;
	ST_RF_Logger_Responce_AbnormalHist_t umLoggerResAbnormalHist;
	ST_RF_Logger_Responce_EventHist_t umLoggerResEventHist;
	ST_RF_Logger_Responce_OpeHist_t umLoggerResOpeHist;
	ST_RF_Logger_Responce_SysHist_t umLoggerResSysHist;
	ST_RF_Logger_SettingRes_t umLoggerResSetting;
	ST_RF_Logger_QuerySet_t umLoggerQuerySet;
	ST_RF_Logger_SettingWt_t umGwWriteSet;
	ST_RF_Logger_AckNack_t umLoggerAckNack;
	ST_RF_Gw_Hex_Tx_t umFirmHex;
//	ST_RF_Logger_Call_t umLoggerCall;
//	ST_RF_Measure_Query_t umMeasure_Query;
//	ST_RF_Measure_Main_Responce_t umMeasureMain_Responce;
//	ST_RF_RT_Call_First_t umRT_LoggerCall;
//	ST_RF_RT_Call_Responce_t umRT_LoggerCall_Responce;
//	ST_RF_RT_Call_Ack_t umRT_LoggerCall_Ack;
	ST_RF_RT_Beacon_t umRT_Logger_Beacon;
	ST_RF_RT_ConnectAuth_t umRT_Gw_ConnAuth;
	ST_RF_RT_LoggerInfo_t umRT_LoggerInfo;
	ST_RF_RT_ConnectAck_t umRT_Gw_ConnAck;
	ST_RF_RT_Measure_Tx_t umRT_MeasureTx;
	ST_RF_RT_Audit_Tx_t umRT_AuditTx;
	ST_RF_RT_Measure_Ack_t umRT_MeasureAck;
	ST_RF_RT_ChangeSet_Beacon_t umRT_ChangeSetBeacon;
//	ST_RF_RT_ChangeSet_ID_t umRT_ChangeSetID;
	ST_RF_RT_ChangeSet_Prm1_t umRT_ChangeSetPrm1;
	ST_RF_RT_ChangeSet_Prm2_t umRT_ChangeSetPrm2;
	ST_RF_RT_ChangeSet_Prm3_t umRT_ChangeSetPrm3;
	ST_RF_RT_ChangeSet_Ack_t umRT_ChangeSetAck;
} UT_RF_Data_t;

typedef union UT_RF_Hsmode_AckData
{
	uint8_t umData[20U];
	ST_RF_Logger_AckNack_t umCategories;
}UT_RF_Hsmode_AckData_t;




/* 矢田目追加 */
#if (swLoggerBord == imEnable)

/* 高速通信データ収集 パケット送信制御用 */
typedef struct ST_Rf_LogSendCtl
{
	uint16_t			mFlashReadCnt[ 2U ];		/* 奇数・偶数Flashリードカウンタ */
	ET_Rf_LogSndCtl_t	mFlashReadFin[ 2U ];		/* 奇数・偶数Flashリード/送信完了監視 */
	uint32_t			mFlashReadTime[ 2U ];		/* 奇数・偶数リード先頭時刻 */
	uint16_t			mInterval;					/* 送信中の収録インターバル */
} ST_Rf_LogSendCtl_t;

/*	高速通信データ収集 受信データ解析用 */
typedef struct ST_BeaconRxAnalysis_t
{
	uint16_t mData_Num;					/* 現在の収録データ数 */
	uint16_t mData_Index;				/* 現在の収録IndexNo. */
	uint16_t mMax_Num;					/* 収録データ最大数 */
	uint16_t mMax_Index;				/* 収録データ最大IndexNo. */
	uint8_t mKosu;						/* Flashリード個数 */
	ET_RegionKind_t mRegionKind;		/* Flashリード領域 */
	
	uint32_t mReqStartTime;				/* 要求開始時刻 */
	uint16_t mReqIndexNo;				/* 要求IndexNo. */
} ST_BeaconRxAnalysis_t;

typedef	struct ST_BeaconRxAnalysis_ContinueTbl
{
	ET_RfDrv_GwReqCmd_t		mReqCmdCode;
	uint8_t					mKosu;
	ET_RegionKind_t			mRegionKind;
} ST_BeaconRxAnalysis_ContinueTbl_t;

#else
typedef	struct ST_BcnAckDataSetTbl
{
	ET_RfDrv_GwReqCmd_t		mReqCmdCode;
	uint8_t					mPacketCode;
	void					( *mFnc )( uint8_t arLoop );
} ST_BcnAckDataSetTbl_t;
#endif

#pragma unpack


#endif
