/*
 *******************************************************************************
 *	File name	:	RF_Struct.h
 *
 *	[���e]
 *		�����ʐM�� �\���́E���p��
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2020.4.13
 *******************************************************************************
 */

#ifndef	INCLUDED_RF_STRUCT_H
#define	INCLUDED_RF_STRUCT_H


#include "RF_Enum.h"
#include "switch.h"

/* �\���̃����o�̃A���C�����g��1�ɂ��� */
#pragma pack

/*
 *==============================================================================
 *	�\���̒�`
 *==============================================================================
 */
typedef struct ST_RFChGrupe
{
	uint8_t		mCh[2U];					/* �����ʐM�`���l���O���[�v */
	uint8_t		mLoraSync;
}ST_RFChGrupe_t;

typedef struct ST_RFPrm
{
	ET_RFDrvInitSts_t	mInitSts;												/* �������X�e�[�^�X */
	uint8_t				mInitWaitCnt;											/* �C�j�V�����C�Y�҂��J�E���^(10ms/�J�E���g) */
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
  *RF��Ԃ̒�`
*/
typedef struct
{
	RadioModems_t Modem;					  /* FSK or LoRa */
	RFState_t State;						  /* RF��State RX,TX�̐ݒ��Ԋ܂� */
} ST_RFStatus_t;


/*
  ����CALL�M���̉����J�E���g�_�E����`(���M�p)
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
  ����CALL�M���̎�M�f�[�^��`
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


/* �v���f�[�^�\���� */
typedef struct ST_RF_Measure_Data
{
	uint8_t mEvent_Measure[2U];
	uint8_t mAlart;
} ST_RF_Measure_Data_t;

typedef struct
{
	uint8_t mGroupID;
	uint8_t mOpeCompanyID;		/* ���Ɖ��ID 8bit */
	uint8_t mAppID[2U];			/* ���p���ID(�A�v��ID)12bit , [2]�̉���4bit��NULL	*/
	uint8_t mUniqueID[3U];		/* [3U]�̉���4bit��NULL 20bit */
} ST_RF_SerialID_t;


/* BCH�O�̃C���^�[���[�u�p��` */
typedef struct
{
	uint16_t Mask;
	int8_t Shift;
} ST_RF_PreBCH_t;


/***************************************
*  �z��[0U]��MSB�ƂȂ�悤�ɔz�u
****************************************/
/* HS mode �����p�P�b�gNo20 �u���[�h�L���X�g */
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

/* HS Mode �����p�P�b�gNo21 �X�e�[�^�X���� */
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


/* �����p�P�b�gNo22 ���K�[�v���l���� */
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
} ST_RF_Logger_MeasureCrc_t;		/* 150byte���Ƃ�CRC */

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
	uint8_t mBch[8U];					/* �ǉ� */
	ST_RF_Logger_MeasureCrc_t mMeasureCrc[9U];
} ST_RF_Logger_Responce_Measure_t;

/* HS Mode �����p�P�b�gNo23 �x�񗚗��f�[�^���� */
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


/* HS Mode �����p�P�b�gNo24 �@��ُ헚���f�[�^���� */
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


/* HS Mode �����p�P�b�gNo25 �C�x���g�f�[�^���� */
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


/* HS Mode �����p�P�b�gNo26 ���엚���f�[�^���� */
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


/* HS mode �����p�P�b�gNo27 �V�X�e�������f�[�^	*/
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


/* HS mode �����p�P�b�gNo28 �ݒ�ǂݍ��� */
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

/* HS mode �����p�P�b�gNo29 �ݒ�l�v�� */
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

/* HS mode Logger�@�ʐMACK/NACK */
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

/* RTmode �C�� No1 �r�[�R�����M */
typedef struct ST_RF_RT_Beacon
{
	uint8_t mCompanyID;
	uint8_t mAppID_Online[2U];
	uint8_t mReserve[2U];
	uint8_t mCrc[2U];
} ST_RF_RT_Beacon_t;


/* RTmode �C�� No2 ���K�[�ڑ����� */
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


/* RTmode �C�� No3 ���K�[��� */
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
	uint8_t mRecInterval;							/* ���^�Ԋu1 ���4bit,���^�Ԋu2 ����4bit */
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
	uint8_t mBuffRecInterval;							/* ���^�Ԋu1 ���4bit,���^�Ԋu2 ����4bit */
	uint8_t mBuffCalcAlarmKind;
	uint8_t mBuffCalcAlarmParameter;
	uint8_t mBuffMeasOldId[2U];
	uint8_t mBuffMeasOldTime[4U];
	uint8_t mBuffAlHistOldId[2U];
	uint8_t mBuffAlHistOldTime[4U];
	uint8_t mBuffVer[2U];
} ST_RF_RT_LoggerInfo_t;


/* RTmode �C�� No4 �ڑ��菇ACK */
typedef struct ST_RF_RT_ConnectAck
{
	uint8_t mLoggerID[3U];
	uint8_t mGwID_Sequence[3U];
	uint8_t mTimeSlot;
	uint8_t mReserve[2U];
	uint8_t mCrc[2U];
} ST_RF_RT_ConnectAck_t;


/* RTmode No5 �v���f�[�^�̃��A���^�C�����M */
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
	/* �����܂�No.10�Ɠ��� */
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

/* RTmode No10 �����f�[�^�̃��A���^�C�����M */
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
	/* �����܂�No5�Ɠ��� */
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


/* RTmode No6 �v���f�[�^�̃��A���^�C�����M�ւ�ACK */
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


/* RTmode No1 �ݒ�ύX�\��r�[�R�� */
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


/* RTmode No9 �ݒ�ύXID�ʒm (�폜) */
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

/* RTmode No7 ���K�[�ݒ�ύX�P */
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
/* RTmode No7 ���K�[�ݒ�ύX2 */
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

/* RTmode No7 ���K�[�ݒ�ύX2 */
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

/* RTmode No7 ���K�[�ݒ�ύX3 */
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

/* RTmode No9 �ݒ�ύXAck */
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
*�@���p��
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




/* ��c�ڒǉ� */
#if (swLoggerBord == imEnable)

/* �����ʐM�f�[�^���W �p�P�b�g���M����p */
typedef struct ST_Rf_LogSendCtl
{
	uint16_t			mFlashReadCnt[ 2U ];		/* ��E����Flash���[�h�J�E���^ */
	ET_Rf_LogSndCtl_t	mFlashReadFin[ 2U ];		/* ��E����Flash���[�h/���M�����Ď� */
	uint32_t			mFlashReadTime[ 2U ];		/* ��E�������[�h�擪���� */
	uint16_t			mInterval;					/* ���M���̎��^�C���^�[�o�� */
} ST_Rf_LogSendCtl_t;

/*	�����ʐM�f�[�^���W ��M�f�[�^��͗p */
typedef struct ST_BeaconRxAnalysis_t
{
	uint16_t mData_Num;					/* ���݂̎��^�f�[�^�� */
	uint16_t mData_Index;				/* ���݂̎��^IndexNo. */
	uint16_t mMax_Num;					/* ���^�f�[�^�ő吔 */
	uint16_t mMax_Index;				/* ���^�f�[�^�ő�IndexNo. */
	uint8_t mKosu;						/* Flash���[�h�� */
	ET_RegionKind_t mRegionKind;		/* Flash���[�h�̈� */
	
	uint32_t mReqStartTime;				/* �v���J�n���� */
	uint16_t mReqIndexNo;				/* �v��IndexNo. */
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
