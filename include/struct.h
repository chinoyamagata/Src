/*
 *******************************************************************************
 *	File name	:	struct.h
 *
 *	[���e]
 *		�\���̒�`
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2017.11.08		Softex N.I		�V�K�쐬
 *******************************************************************************
 */
#ifndef	INCLUDED_STRUCT_H
#define	INCLUDED_STRUCT_H


#include "immediate.h"															/* immediate�萔��` */
#include "enum.h"																/* �񋓌^�̒�` */



typedef struct ST_ModuleSts
{
	ET_RtcIntSts_t			mRtcInt;
	ET_ExeModuleSts_t		mExe;
	ET_MeaModuleSts_t		mMea;
	ET_KeyIntModuleSts_t	mKeyInt;
	ET_EventKeyModuleSts_t	mEventKey;
	ET_BatModuleSts_t		mBat;
	ET_ComModuleSts_t		mCom;
	ET_RfModuleSts_t		mRf;
	ET_RtcModuleSts_t		mRtc;
	ET_ExFlashModuleSts_t	mExFlash;
	ET_LcdModuleSts_t		mLcd;
	uint8_t					mErrChk;
	ET_ExFlashRdModuleSts_t	mExFlashRd;
	ET_InFlashStsWrExe_t	mInFlash;
	ET_HistorySts_t			mHistory;
	ET_FirmupSts_t			mFirmup;
} ST_ModuleSts_t;


/*
 *==============================================================================
 *	MODBUS�ʐM
 *==============================================================================
 */
/* MODBUS�ʐM��� */
typedef struct ST_ModInf
{
	uint8_t			mSndBuf[imMod_TxBufSize];									/* ���M�o�b�t�@ */
	uint8_t			mRcvBuf[imMod_RxBufSize];									/* ��M�o�b�t�@ */
	uint16_t		mSndLen;													/* ���M�f�[�^�� */
	uint16_t		mRcvLen;													/* ��M�f�[�^�� */
	uint16_t		mRcvPos;													/* ��M�f�[�^�ʒu */
	ET_ComSts_t		mComSts;													/* �ʐM��� */
	uint8_t			mRcvTmCnt;													/* ��M�^�C�}�J�E���^ */
#if (swSensorCom == imEnable)
	uint8_t			mUnitPwrOnTmCnt;											/* ���j�b�g�N�����ԃ^�C�}�J�E���^ */
	uint8_t			mRetryCnt;													/* ���j�b�g�ʐM���g���C�J�E���^ */
#endif
} ST_ModInf_t;

/*
 *==============================================================================
 *	RTC����
 *==============================================================================
 */
/* RTC�f�[�^ */
typedef struct ST_RTC
{
	uint8_t		mYear;															/* �N */
	uint8_t		mMonth;															/* �� */
	uint8_t		mDay;															/* �� */
	uint8_t		mWeek;															/* �j�� */
	uint8_t		mHour;															/* �� */
	uint8_t		mMin;															/* �� */
	uint8_t		mSec;															/* �b */
} ST_RTC_t;

/*
 *==============================================================================
 *	���菈��
 *==============================================================================
 */
/* ���菈���p�^�C�}�J�E���g */
typedef struct ST_MeasTmCnt
{
	uint16_t	mHigh;															/* ���2byte */
	uint16_t	mLow;															/* ����2byte */
} ST_MeasTmCnt_t;

/* ���菈���p�p�����[�^ */
typedef struct ST_MeasPrm
{
	uint32_t		mTime;									/* ���莞�� */
	uint32_t		mTimeEvt;								/* ���莞��(�C�x���g��) */
	sint16_t		mMeasVal[ imChannelNum ];				/* ����l */
	uint8_t			mMeasError[ imChannelNum ];				/* ����G���[ */
	uint8_t			mLogic;									/* �_�� */
	uint8_t			mDevFlg[ imChannelNum ];				/* ��E�t���O */
	uint8_t			mAlmFlg[ imChannelNum ];				/* �x��t���O */
} ST_MeasPrm_t;


/*
 *==============================================================================
 *	�d�r
 *==============================================================================
 */
/* �d�r��� */
typedef struct ST_BatAd
{
	uint16_t		mDryBattAdCnt;			/* �d�r�pA/D�J�E���g�l */
	uint16_t		mRetryCnt;				/* �d�r�pAD���g���C�� */
	ET_BattSts_t	mBattSts;				/* �d�r�X�e�[�^�X */
	ET_AdcPhase_t	mPhase;					/* AD����̃X�e�[�^�X */
} ST_BatAd_t;


/*
 *==============================================================================
 *	�O�t���t���b�V��
 *==============================================================================
 */
 
/* �ۑ��̈��:�ُ��� */
typedef struct ST_FlashErrInfo
{
	uint32_t		mTimeInfo;								/* ������� */
	ET_ErrInfItm_t	mItem;									/* �x�񍀖� */
	uint8_t			mAlmFlg;								/* �A���[���t���O */
} ST_FlashErrInfo_t;


/* �ۑ��̈��:�v���x�� */
typedef struct ST_FlashMeasAlm
{
	uint32_t		mTimeInfo;								/* ������� */
	uint8_t			mAlmFlg;								/* �A���[���t���O */
	uint8_t			mAlmChannel;							/* �`���l�� */
	uint8_t			mLevel;									/* ���x�� */
	uint16_t		mMeasVal;								/* �v���l */
	ET_MeasAlmItm_t	mItem;									/* �x�񍀖� */
	uint8_t			mEvent;									/* �_�� */
} ST_FlashMeasAlm_t;


/* �ۑ��̈��:�v���l1�A�v���l2�A�C�x���g */
typedef struct ST_FlashMeasVal1Event
{
	uint32_t		mTimeInfo;								/* ������� */
	uint16_t		mMeasVal[ imFlashQue_ChannelNum ];		/* 1~3ch�v���l */
	uint8_t			mLogic;									/* 1ch�_�� */
	uint8_t			mDevFlg[ imFlashQue_ChannelNum ];		/* 1~3ch��E�t���O */
	uint8_t			mAlmFlg[ imFlashQue_ChannelNum ];		/* 1~3ch�x��t���O */
	uint8_t			m4chDevAlmFlg;							/* 4ch��E�x��t���O */
} ST_FlashVal1Evt_t;


/* �ۑ��̈��:���엚�� */
typedef struct ST_FlashActLog
{
	uint32_t		mTimeInfo;								/* ������� */
	ET_ActLogItm_t	mItem;									/* �������� */
	uint32_t		mData;									/* �ő�24bit�f�[�^ */
	uint8_t			mUserId;								/* �ύX��ID */
} ST_FlashActLog_t;


/* �ۑ��̈��:�V�X�e�����O */
typedef struct ST_FlashSysLog
{
	uint32_t		mTimeInfo;								/* ������� */
	ET_SysLogItm_t	mItem;									/* �V�X�e�����O���� */
	uint32_t		mData;									/* �ő�25bit�f�[�^ */
} ST_FlashSysLog_t;


/* �ۑ��̈��:�v���l1�A�v���l2�A�V�X�e�����O�̃A�h���X�e�[�u�� */
typedef struct ST_FlashAdrTbl
{
	uint32_t	mEndAdr;
	uint16_t	mNum;
	uint16_t	mIndex;
} ST_FlashAdrTbl_t;



/*
 *==============================================================================
 *	�����t���b�V���ۑ��̈��
 *==============================================================================
 */
/* �ۑ��̈�� */
typedef struct ST_FlashRegionData
{
	uint32_t		mWrNum;									/* �������݌��� */
	uint32_t		mWrAddrEnd;								/* �������ݏI�[�A�h���X */
	uint16_t		mIndexNo;
} ST_FlashRegionData_t;



/* �i�[�f�[�^ */
#define imChNum		4U

/* �\���̃����o�̃A���C�����g��1�ɂ��� */
#pragma pack
typedef struct ST_InFlashTblProcess
{
	uint8_t		mFirstAcsChk;					/* ����Flash�t�@�[�X�g�A�N�Z�X */
	uint8_t		mRtcFirstWrChk;					/* �O��RTC�t�@�[�X�g���C�g */
	uint8_t		mOpeCompanyID;					/* ���Ɖ��ID */
	uint8_t		mUniqueID[ 3U ];				/* ���j�[�NID */
	
	uint32_t	mSerialNo;						/* �����ԍ� */
	sint16_t	mRfFreqOffset;					/* �������g�������l */
	uint8_t		mUpDate[ 3U ];					/* �t�@�[���E�F�A�X�V���� */
	uint8_t		mCalWrDate[ 3U ];				/* �Z���l�����ݓ� */
	uint8_t		mOffset[ imChNum ];				/* �Z���l�F�I�t�Z�b�g */
	uint8_t		mSlope[ imChNum ];				/* �Z���l�F�X�� */
	sint8_t		mRfTmpHosei;					/* �������̉��x�␳ */
	uint8_t		mModelCode;						/* �@��ʃR�[�h */
	uint8_t		mUpVer[ 3U ];					/* �t�@�[��Ver. */
	uint16_t	mUpUserId;						/* �t�@�[���X�V���[�UID */
	uint16_t	mModelOffset;					/* �Z���T�했�̃I�t�Z�b�g�� */
	uint8_t		mDummy[ 3U ];
} ST_InFlashTblProcess_t;

typedef struct ST_InFlashTblParam
{
	uint8_t		mParmFlg;						/* �p�����[�^�t���O�Ǘ� */
												/* 0bit:Hi�ōH��o�׏��������s�A1bit:Hi�Ȃ�HS�̂ݑҎ��ԁALo�Ȃ�HS-Rt�Ҏ��� */
	uint8_t		mTimDispKind;					/* ���߁A���q�\�� */
	
	uint8_t		mGroupID;						/* �O���[�vID */
	uint8_t		mAppID[ 2U ];					/* ���p���ID(�A�v��ID) */
	uint8_t		mOnCertLmt;						/* �I�����C���F�،��� */
	
	/* �ݒ�l:start */
	sint16_t	mDeviUU[ imChNum ];				/* �x��l-���� */
	sint16_t	mDeviU[ imChNum ];				/* �x��l-��� */
	sint16_t	mDeviL[ imChNum ];				/* �x��l-���� */
	sint16_t	mDeviLL[ imChNum ];				/* �x��l-������ */
	uint8_t		mDelayUU[ imChNum ];			/* �x����-���� */
	uint8_t		mDelayU[ imChNum ];				/* �x����-��� */
	uint8_t		mDelayL[ imChNum ];				/* �x����-���� */
	uint8_t		mDelayLL[ imChNum ];			/* �x����-������ */
	uint8_t		mDeviEnaLv[ imChNum ];			/* ��E���e���ԑI��臒l */
	uint16_t	mDeviEnaTime[ imChNum ];		/* ��E���e���� */
	sint8_t		mOffset[ imChNum ];				/* �v���l�I�t�Z�b�g */
	uint8_t		mSlope[ imChNum ];				/* �v���l�ꎟ�X���␳ */
	uint8_t		mBatType;						/* �d�r�� */
	uint8_t		mKeyLock;						/* �L�[���b�N */
	uint8_t		mrfLoraChGrupeCnt;				/* ���A���^�C���ʐM�Ҏ�Ch */
	uint8_t		mLogCyc1;						/* ���^����1 */
	uint8_t		mLogCyc2;						/* ���^����2 */
	uint8_t		mLoggerName[ imLoggerNameNum ];	/* ���K�[���� */
	uint8_t		mrfHsCh;						/* �����ʐM�Ҏ�Ch */
	uint16_t	mFstConnBoot;					/* �����ʐM�N���̃��O�f�[�^臒l */
	uint8_t		mAlertType;						/* �x��Ď����Z�ݒ�(���Z��) */
	uint8_t		mAlertParmFlag;					/* �x��Ď����Z�ݒ�(�p�����[�^) */
	uint8_t		mEventKeyFlg;					/* �C�x���g�{�^������ */
	uint8_t		mDummy1;						/* �\�� */
	uint16_t	mRealTimeSndPt;					/* ���A���^�C���ʐM�]���n�_�f�[�^ */
	uint8_t		mDeviClrPoint;					/* ��E���e���Ԃ̃N���A�_�@ */
	uint8_t		mDeviClrTimeHour;				/* ��E���e���Ԃ̃N���A����(��) */
	uint8_t		mDeviClrTimeMin;				/* ��E���e���Ԃ̃N���A����(��) */
	uint16_t	mCalDate;						/* �Z���� */
	uint8_t		mScalDecimal;					/* �X�P�[�����O�ϊ�-�����_�ʒu */
	uint16_t	mScalYInt;						/* �X�P�[�����O�ϊ�-�ؕ� */
	uint16_t	mScalGrad;						/* �X�P�[�����O�ϊ�-�X�� */
	uint8_t		mFlashSelect;					/* �t���b�V���������؂�ւ� */
	uint8_t		mCnctEnaGwId[ 10U ][ 3U ];		/* �ڑ����Q�[�g�E�F�CID */
	/* �ݒ�l:end */
	uint8_t		mSetvalChgTime[ 4U ];			/* �ݒ�ύX���� */
	uint8_t		mDummy[ 10U ];

} ST_InFlashTblParam_t;


typedef struct ST_InFlashTblData
{
	uint32_t	mErrInf_AddrEnd;				/* �ُ���F�I�[�A�h���X */
	uint16_t	mErrInf_Num;					/* �ُ���F�f�[�^�� */
	uint16_t	mErrInf_Index;					/* �ُ���FIndexNo. */
	
	uint32_t	mMeasAlm_AddrEnd;				/* �v���x��F�I�[�A�h���X */
	uint16_t	mMeasAlm_Num;					/* �v���x��F�f�[�^�� */
	uint16_t	mMeasAlm_Index;					/* �v���x��FIndexNo. */
	
	uint32_t	mMeas1_AddrEnd;					/* ����l1�F�I�[�A�h���X */
	uint16_t	mMeas1_Num;						/* ����l1�F�f�[�^�� */
	uint16_t	mMeas1_Index;					/* ����l1�FIndexNo. */
	uint16_t	mMeas1_OldPtrIndex;				/* ����l1�F�ߋ��v���l���M�|�C���^IndexNo. */
	uint8_t		mMeas1_IndexNextLap;			/* ����l1�FIndexNo.��1���������ǂ����̃t���O */
	uint16_t	mMeas1_EvtPtrIndex;						/* ����l1�F�ߋ��v���l���M�|�C���^�p��Event�L������IndexNo. */
	
	uint32_t	mEvt_AddrEnd;					/* �C�x���g�F�I�[�A�h���X */
	uint16_t	mEvt_Num;						/* �C�x���g�F�f�[�^�� */
	uint16_t	mEvt_Index;						/* �C�x���g�FIndexNo. */
	
	uint32_t	mActLog_AddrEnd;				/* ���엚���F�I�[�A�h���X */
	uint16_t	mActLog_Num;					/* ���엚���F�f�[�^�� */
	uint16_t	mActLog_Index;					/* ���엚���FIndexNo. */
	
	uint32_t	mSysLog_AddrEnd;				/* �V�X�e�����O�F�I�[�A�h���X */
	uint16_t	mSysLog_Num;					/* �V�X�e�����O�F�f�[�^�� */
	uint16_t	mSysLog_Index;					/* �V�X�e�����O�FIndexNo. */
	
	uint32_t	mMeasVal1Adr_AddrEnd;					/* �v���l1�̃A�h���X�e�[�u���F�I�[�A�h���X */
	uint16_t	mMeasVal1Adr_Num;						/* �v���l1�̃A�h���X�e�[�u���F�f�[�^�� */
	uint32_t	mMeasVal2Adr_AddrEnd;					/* �v���l2�̃A�h���X�e�[�u���F�I�[�A�h���X */
	uint16_t	mMeasVal2Adr_Num;						/* �v���l2�̃A�h���X�e�[�u���F�f�[�^�� */
	uint32_t	mSysLogAdr_AddrEnd;						/* �V�X�e�����O�̃A�h���X�e�[�u���F�I�[�A�h���X */
	uint16_t	mSysLogAdr_Num;							/* �V�X�e�����O�̃A�h���X�e�[�u���F�f�[�^�� */
	
	uint16_t	mMeasAlm_OldPtrIndex;					/* �v���x��F�ߋ��v���x�񑗐M�|�C���^IndexNo. */
	
	uint16_t	mErrInf_PastIndex;						/* �ُ���F�Ō�IndexNo. */
	uint32_t	mErrInf_PastTime;						/* �ُ���F�ŌÎ��� */
	uint16_t	mMeasAlm_PastIndex;						/* �v���x��F�Ō�IndexNo. */
	uint32_t	mMeasAlm_PastTime;						/* �v���x��F�ŌÎ��� */
	uint16_t	mMeas1_PastIndex;						/* ����l1�F�Ō�IndexNo. */
	uint32_t	mMeas1_PastTime;						/* ����l1�F�ŌÎ��� */
	uint16_t	mEvt_PastIndex;							/* �C�x���g�F�Ō�IndexNo. */
	uint32_t	mEvt_PastTime;							/* �C�x���g�F�ŌÎ��� */
	uint16_t	mActLog_PastIndex;						/* ���엚���F�Ō�IndexNo. */
	uint32_t	mActLog_PastTime;						/* ���엚���F�ŌÎ��� */
	uint16_t	mSysLog_PastIndex;						/* �V�X�e�������F�Ō�IndexNo. */
	uint32_t	mSysLog_PastTime;						/* �V�X�e�������F�ŌÎ��� */
	
	uint16_t	mAbnInf_Sts;							/* �@��ُ��� */
	
	uint32_t	mNewTimeInfo;							/* �ŐV���� */
	uint16_t	mNewMeasVal[ imFlashQue_ChannelNum ];	/* �ŐV�v���l1~3ch */
	uint8_t		mNewLogic;								/* �ŐV�_�� */
	
	uint8_t		mAlmPast[ 5U ];							/* �ߋ��x��t���O(1~4ch, �@��ُ�) */
	
	uint32_t	mNewestHistTime;						/* �S���𒆂̍ŐV���� */
	uint32_t	mNewestLogTime;							/* �v���l�̍ŐV���� */
	
} ST_InFlashTblData_t;

typedef struct ST_InFlashTbl
{
	ST_InFlashTblProcess_t	mProcess;
	ST_InFlashTblParam_t	mParam;
	ST_InFlashTblData_t		mData;
} ST_InFlashTbl_t;
#pragma unpack

/*
 *==============================================================================
 *	����
 *==============================================================================
 */
/* �|�[�g���荞�݃t���O */
typedef struct
{
	uint8_t mRadioDio0 : 1U;
	uint8_t mRadioDio1 : 1U;
	uint8_t mRadioDio2 : 1U;
	uint8_t mRadioDio3 : 1U;
	uint8_t mRadioDio4 : 1U;
	uint8_t mRadioDio5 : 1U;
	uint8_t mRadioTimer : 1U;
	uint8_t mRadioStsMsk : 1U;
} ST_RF_INT_t;


/* �v�����A���^�C�����M�f�[�^(Flash�ɏ������܂�Ă���ŐV�f�[�^) */
typedef struct ST_RTMeasData
{
	uint8_t		mRTMeasData_AbnStatus;
	uint8_t		mRTMeasData_MeasTime[ 4U ];
	uint8_t		mRTMeasData_MeasID[ 2U ];
	uint8_t		mRTMeasData_MeasVal[ 5U ];
	uint8_t		mRTMeasData_AlarmFlag[ 2U ];
} ST_RTMeasData_t;

typedef struct ST_RTMeasAlm2Data
{
	uint8_t		mRTMeasData_AbnStatus;
	uint8_t		mRTMeasData_MeasTime[ 4U ];
	uint8_t		mRTMeasData_MeasVal[ 5U ];
	uint8_t		mRTMeasData_AlarmFlag[ 2U ];
} ST_RTMeasAlm2Data_t;


typedef struct ST_RF_StsFlag
{
	uint8_t mMuskMultiEvent : 1U;			/* Idle���̑��d�C�x���g�h�~ */
	uint8_t mMuskCadSetting : 1U;			/* CAD���d�Z�b�e���O�h�~�t���O */
	uint8_t mTimerSync : 1U;				/* GW�Ƃ̃^�C�~���O�������t���O */
	uint8_t mSetReserve : 1U;				/* GW����̐ݒ�ύX�\���M�t���O */
//	uint8_t mTxTimeLimit : 1U;				/* GW�̗݌v���M���Ԑ����t���O */
	uint8_t mModeSelect	:	1U;				/* GW�̃��[�h��� 1:RTmode 0:HSmode */
	uint8_t mHsmodeDetectLoop	:	1U;		/* Logger HS Mode�̃r�[�R�����m���g���C�p�t���O */
	uint8_t mHsmodeInit	:	1U;				/* GW HSmode�ł̏������L����� */
//	uint8_t mHsmodeRxRetry : 1U;			/* GW HSmode�ł̌v���l���̎�MCRC�G���[�ł̃��g���C��M�t���O */
//	uint8_t mHsmodeRxEnd : 1U;				/* GW HSmode�ł̌v���l���̎�M�I���t���O */
	uint8_t mHsmodeRxEnter : 1U;			/* GW HSmode�ł̃f�[�^��M�J�n�t���O */
	uint8_t mHsmodeTxTurn : 1U;				/* GW HSmode�ł̃p�P�b�g���M�񐔂�����������̃t���O */
	uint8_t mHsmodeFirmSet	: 1U;
//	uint8_t mRtmodeBeaconInit	:	1U;		/* RTmode�ł�Beacon���M��������� */
//	uint8_t mRtmodeMeasureInit	:	1U;		/* RTmode�ł̌v���l���M��������� */
	uint8_t mRtmodePwmode		:	1U;		/* RTmode�ł̑��M�o�͑I���uPW_H�v�uPW_L�v */
	uint8_t mPowerInt			:	1U;		/* �d�r�����̊��荞�݃t���O */
	uint8_t mTimingCorrect		:	1U;		/* �����ʐM�^�C�~���O�̉��x�␳�t���O 1sec���ƂɎ��s */
	uint8_t mTempMeasure		:	1U;
	uint8_t mTempAdc			:	1U;		/* �}�C�R���������xAD�����t���O */
	uint8_t mReset				:	1U;		/* �L�����A�Z���X����G���[���̖���IC�������t���O */
	uint8_t mHsmodeRandom		:	1U;
	uint8_t mHsmodeSyncDetect	:	1U;		/* �ďo���r�[�R����Sync�����o���v���E�ݒ肩 1:���o */
	uint8_t mHsmodeeResCmdStatus	:	1U;		/* �����ʐM���[�h �Ăяo�����m���s�� 1:���o */
	uint8_t mOnlineLcdOnOff		:	1U;		/* �ڑ��ʐM�J�n����LCD�_�����A���s�������������p�r�ŏ]���d�l����OR�ǉ� 2022.9.16 */
}ST_RF_StsFlag_t;


/*
 *==============================================================================
 *	�\��
 *==============================================================================
 */
typedef struct
{
	ET_DispMode_t	mNowDisp;
	ET_DispMode_t	mPreDisp;
} ST_DispSts_t;


typedef struct ST_ActQue
{
	uint8_t		mUserID;					/* �ݒ�҂�ID */
	uint8_t		mNum;						/* �i�[�� */
	uint8_t		mItem[ 90U ];				/* �ݒ藚������ */
	uint8_t		mSetVal[ 90U ][ 3U ];		/* �ݒ�l */
} ST_ActQue_t;

#endif		/* INCLUDED_STRUCT_H */
