/*
 *******************************************************************************
 *	File name	:	struct.h
 *
 *	[内容]
 *		構造体定義
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2017.11.08		Softex N.I		新規作成
 *******************************************************************************
 */
#ifndef	INCLUDED_STRUCT_H
#define	INCLUDED_STRUCT_H


#include "immediate.h"															/* immediate定数定義 */
#include "enum.h"																/* 列挙型の定義 */



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
 *	MODBUS通信
 *==============================================================================
 */
/* MODBUS通信情報 */
typedef struct ST_ModInf
{
	uint8_t			mSndBuf[imMod_TxBufSize];									/* 送信バッファ */
	uint8_t			mRcvBuf[imMod_RxBufSize];									/* 受信バッファ */
	uint16_t		mSndLen;													/* 送信データ長 */
	uint16_t		mRcvLen;													/* 受信データ長 */
	uint16_t		mRcvPos;													/* 受信データ位置 */
	ET_ComSts_t		mComSts;													/* 通信状態 */
	uint8_t			mRcvTmCnt;													/* 受信タイマカウンタ */
#if (swSensorCom == imEnable)
	uint8_t			mUnitPwrOnTmCnt;											/* ユニット起動時間タイマカウンタ */
	uint8_t			mRetryCnt;													/* ユニット通信リトライカウンタ */
#endif
} ST_ModInf_t;

/*
 *==============================================================================
 *	RTC処理
 *==============================================================================
 */
/* RTCデータ */
typedef struct ST_RTC
{
	uint8_t		mYear;															/* 年 */
	uint8_t		mMonth;															/* 月 */
	uint8_t		mDay;															/* 日 */
	uint8_t		mWeek;															/* 曜日 */
	uint8_t		mHour;															/* 時 */
	uint8_t		mMin;															/* 分 */
	uint8_t		mSec;															/* 秒 */
} ST_RTC_t;

/*
 *==============================================================================
 *	測定処理
 *==============================================================================
 */
/* 測定処理用タイマカウント */
typedef struct ST_MeasTmCnt
{
	uint16_t	mHigh;															/* 上位2byte */
	uint16_t	mLow;															/* 下位2byte */
} ST_MeasTmCnt_t;

/* 測定処理用パラメータ */
typedef struct ST_MeasPrm
{
	uint32_t		mTime;									/* 測定時刻 */
	uint32_t		mTimeEvt;								/* 測定時刻(イベント時) */
	sint16_t		mMeasVal[ imChannelNum ];				/* 測定値 */
	uint8_t			mMeasError[ imChannelNum ];				/* 測定エラー */
	uint8_t			mLogic;									/* 論理 */
	uint8_t			mDevFlg[ imChannelNum ];				/* 逸脱フラグ */
	uint8_t			mAlmFlg[ imChannelNum ];				/* 警報フラグ */
} ST_MeasPrm_t;


/*
 *==============================================================================
 *	電池
 *==============================================================================
 */
/* 電池状態 */
typedef struct ST_BatAd
{
	uint16_t		mDryBattAdCnt;			/* 電池用A/Dカウント値 */
	uint16_t		mRetryCnt;				/* 電池用ADリトライ回数 */
	ET_BattSts_t	mBattSts;				/* 電池ステータス */
	ET_AdcPhase_t	mPhase;					/* AD動作のステータス */
} ST_BatAd_t;


/*
 *==============================================================================
 *	外付けフラッシュ
 *==============================================================================
 */
 
/* 保存領域種:異常情報 */
typedef struct ST_FlashErrInfo
{
	uint32_t		mTimeInfo;								/* 時刻情報 */
	ET_ErrInfItm_t	mItem;									/* 警報項目 */
	uint8_t			mAlmFlg;								/* アラームフラグ */
} ST_FlashErrInfo_t;


/* 保存領域種:計測警報 */
typedef struct ST_FlashMeasAlm
{
	uint32_t		mTimeInfo;								/* 時刻情報 */
	uint8_t			mAlmFlg;								/* アラームフラグ */
	uint8_t			mAlmChannel;							/* チャネル */
	uint8_t			mLevel;									/* レベル */
	uint16_t		mMeasVal;								/* 計測値 */
	ET_MeasAlmItm_t	mItem;									/* 警報項目 */
	uint8_t			mEvent;									/* 論理 */
} ST_FlashMeasAlm_t;


/* 保存領域種:計測値1、計測値2、イベント */
typedef struct ST_FlashMeasVal1Event
{
	uint32_t		mTimeInfo;								/* 時刻情報 */
	uint16_t		mMeasVal[ imFlashQue_ChannelNum ];		/* 1~3ch計測値 */
	uint8_t			mLogic;									/* 1ch論理 */
	uint8_t			mDevFlg[ imFlashQue_ChannelNum ];		/* 1~3ch逸脱フラグ */
	uint8_t			mAlmFlg[ imFlashQue_ChannelNum ];		/* 1~3ch警報フラグ */
	uint8_t			m4chDevAlmFlg;							/* 4ch逸脱警報フラグ */
} ST_FlashVal1Evt_t;


/* 保存領域種:動作履歴 */
typedef struct ST_FlashActLog
{
	uint32_t		mTimeInfo;								/* 時刻情報 */
	ET_ActLogItm_t	mItem;									/* 履歴項目 */
	uint32_t		mData;									/* 最大24bitデータ */
	uint8_t			mUserId;								/* 変更者ID */
} ST_FlashActLog_t;


/* 保存領域種:システムログ */
typedef struct ST_FlashSysLog
{
	uint32_t		mTimeInfo;								/* 時刻情報 */
	ET_SysLogItm_t	mItem;									/* システムログ項目 */
	uint32_t		mData;									/* 最大25bitデータ */
} ST_FlashSysLog_t;


/* 保存領域種:計測値1、計測値2、システムログのアドレステーブル */
typedef struct ST_FlashAdrTbl
{
	uint32_t	mEndAdr;
	uint16_t	mNum;
	uint16_t	mIndex;
} ST_FlashAdrTbl_t;



/*
 *==============================================================================
 *	内蔵フラッシュ保存領域種
 *==============================================================================
 */
/* 保存領域種 */
typedef struct ST_FlashRegionData
{
	uint32_t		mWrNum;									/* 書き込み件数 */
	uint32_t		mWrAddrEnd;								/* 書き込み終端アドレス */
	uint16_t		mIndexNo;
} ST_FlashRegionData_t;



/* 格納データ */
#define imChNum		4U

/* 構造体メンバのアライメントを1にする */
#pragma pack
typedef struct ST_InFlashTblProcess
{
	uint8_t		mFirstAcsChk;					/* 内蔵Flashファーストアクセス */
	uint8_t		mRtcFirstWrChk;					/* 外部RTCファーストライト */
	uint8_t		mOpeCompanyID;					/* 事業会社ID */
	uint8_t		mUniqueID[ 3U ];				/* ユニークID */
	
	uint32_t	mSerialNo;						/* 製造番号 */
	sint16_t	mRfFreqOffset;					/* 無線周波数調整値 */
	uint8_t		mUpDate[ 3U ];					/* ファームウェア更新日時 */
	uint8_t		mCalWrDate[ 3U ];				/* 校正値書込み日 */
	uint8_t		mOffset[ imChNum ];				/* 校正値：オフセット */
	uint8_t		mSlope[ imChNum ];				/* 校正値：傾き */
	sint8_t		mRfTmpHosei;					/* 無線時の温度補正 */
	uint8_t		mModelCode;						/* 機種別コード */
	uint8_t		mUpVer[ 3U ];					/* ファームVer. */
	uint16_t	mUpUserId;						/* ファーム更新ユーザID */
	uint16_t	mModelOffset;					/* センサ種毎のオフセット量 */
	uint8_t		mDummy[ 3U ];
} ST_InFlashTblProcess_t;

typedef struct ST_InFlashTblParam
{
	uint8_t		mParmFlg;						/* パラメータフラグ管理 */
												/* 0bit:Hiで工場出荷初期化実行、1bit:HiならHSのみ待受状態、LoならHS-Rt待受状態 */
	uint8_t		mTimDispKind;					/* 正午、正子表現 */
	
	uint8_t		mGroupID;						/* グループID */
	uint8_t		mAppID[ 2U ];					/* 利用会社ID(アプリID) */
	uint8_t		mOnCertLmt;						/* オンライン認証限定 */
	
	/* 設定値:start */
	sint16_t	mDeviUU[ imChNum ];				/* 警報値-上上限 */
	sint16_t	mDeviU[ imChNum ];				/* 警報値-上限 */
	sint16_t	mDeviL[ imChNum ];				/* 警報値-下限 */
	sint16_t	mDeviLL[ imChNum ];				/* 警報値-下下限 */
	uint8_t		mDelayUU[ imChNum ];			/* 遅延回数-上上限 */
	uint8_t		mDelayU[ imChNum ];				/* 遅延回数-上限 */
	uint8_t		mDelayL[ imChNum ];				/* 遅延回数-下限 */
	uint8_t		mDelayLL[ imChNum ];			/* 遅延回数-下下限 */
	uint8_t		mDeviEnaLv[ imChNum ];			/* 逸脱許容時間選択閾値 */
	uint16_t	mDeviEnaTime[ imChNum ];		/* 逸脱許容時間 */
	sint8_t		mOffset[ imChNum ];				/* 計測値オフセット */
	uint8_t		mSlope[ imChNum ];				/* 計測値一次傾き補正 */
	uint8_t		mBatType;						/* 電池種 */
	uint8_t		mKeyLock;						/* キーロック */
	uint8_t		mrfLoraChGrupeCnt;				/* リアルタイム通信待受Ch */
	uint8_t		mLogCyc1;						/* 収録周期1 */
	uint8_t		mLogCyc2;						/* 収録周期2 */
	uint8_t		mLoggerName[ imLoggerNameNum ];	/* ロガー名称 */
	uint8_t		mrfHsCh;						/* 高速通信待受Ch */
	uint16_t	mFstConnBoot;					/* 高速通信起動のログデータ閾値 */
	uint8_t		mAlertType;						/* 警報監視演算設定(演算種) */
	uint8_t		mAlertParmFlag;					/* 警報監視演算設定(パラメータ) */
	uint8_t		mEventKeyFlg;					/* イベントボタン制御 */
	uint8_t		mDummy1;						/* 予約 */
	uint16_t	mRealTimeSndPt;					/* リアルタイム通信転送始点データ */
	uint8_t		mDeviClrPoint;					/* 逸脱許容時間のクリア契機 */
	uint8_t		mDeviClrTimeHour;				/* 逸脱許容時間のクリア時刻(時) */
	uint8_t		mDeviClrTimeMin;				/* 逸脱許容時間のクリア時刻(分) */
	uint16_t	mCalDate;						/* 校正日 */
	uint8_t		mScalDecimal;					/* スケーリング変換-小数点位置 */
	uint16_t	mScalYInt;						/* スケーリング変換-切片 */
	uint16_t	mScalGrad;						/* スケーリング変換-傾き */
	uint8_t		mFlashSelect;					/* フラッシュメモリ切り替え */
	uint8_t		mCnctEnaGwId[ 10U ][ 3U ];		/* 接続許可ゲートウェイID */
	/* 設定値:end */
	uint8_t		mSetvalChgTime[ 4U ];			/* 設定変更時刻 */
	uint8_t		mDummy[ 10U ];

} ST_InFlashTblParam_t;


typedef struct ST_InFlashTblData
{
	uint32_t	mErrInf_AddrEnd;				/* 異常情報：終端アドレス */
	uint16_t	mErrInf_Num;					/* 異常情報：データ数 */
	uint16_t	mErrInf_Index;					/* 異常情報：IndexNo. */
	
	uint32_t	mMeasAlm_AddrEnd;				/* 計測警報：終端アドレス */
	uint16_t	mMeasAlm_Num;					/* 計測警報：データ数 */
	uint16_t	mMeasAlm_Index;					/* 計測警報：IndexNo. */
	
	uint32_t	mMeas1_AddrEnd;					/* 測定値1：終端アドレス */
	uint16_t	mMeas1_Num;						/* 測定値1：データ数 */
	uint16_t	mMeas1_Index;					/* 測定値1：IndexNo. */
	uint16_t	mMeas1_OldPtrIndex;				/* 測定値1：過去計測値送信ポインタIndexNo. */
	uint8_t		mMeas1_IndexNextLap;			/* 測定値1：IndexNo.が1周したかどうかのフラグ */
	uint16_t	mMeas1_EvtPtrIndex;						/* 測定値1：過去計測値送信ポインタ用のEvent有効時のIndexNo. */
	
	uint32_t	mEvt_AddrEnd;					/* イベント：終端アドレス */
	uint16_t	mEvt_Num;						/* イベント：データ数 */
	uint16_t	mEvt_Index;						/* イベント：IndexNo. */
	
	uint32_t	mActLog_AddrEnd;				/* 動作履歴：終端アドレス */
	uint16_t	mActLog_Num;					/* 動作履歴：データ数 */
	uint16_t	mActLog_Index;					/* 動作履歴：IndexNo. */
	
	uint32_t	mSysLog_AddrEnd;				/* システムログ：終端アドレス */
	uint16_t	mSysLog_Num;					/* システムログ：データ数 */
	uint16_t	mSysLog_Index;					/* システムログ：IndexNo. */
	
	uint32_t	mMeasVal1Adr_AddrEnd;					/* 計測値1のアドレステーブル：終端アドレス */
	uint16_t	mMeasVal1Adr_Num;						/* 計測値1のアドレステーブル：データ数 */
	uint32_t	mMeasVal2Adr_AddrEnd;					/* 計測値2のアドレステーブル：終端アドレス */
	uint16_t	mMeasVal2Adr_Num;						/* 計測値2のアドレステーブル：データ数 */
	uint32_t	mSysLogAdr_AddrEnd;						/* システムログのアドレステーブル：終端アドレス */
	uint16_t	mSysLogAdr_Num;							/* システムログのアドレステーブル：データ数 */
	
	uint16_t	mMeasAlm_OldPtrIndex;					/* 計測警報：過去計測警報送信ポインタIndexNo. */
	
	uint16_t	mErrInf_PastIndex;						/* 異常情報：最古IndexNo. */
	uint32_t	mErrInf_PastTime;						/* 異常情報：最古時刻 */
	uint16_t	mMeasAlm_PastIndex;						/* 計測警報：最古IndexNo. */
	uint32_t	mMeasAlm_PastTime;						/* 計測警報：最古時刻 */
	uint16_t	mMeas1_PastIndex;						/* 測定値1：最古IndexNo. */
	uint32_t	mMeas1_PastTime;						/* 測定値1：最古時刻 */
	uint16_t	mEvt_PastIndex;							/* イベント：最古IndexNo. */
	uint32_t	mEvt_PastTime;							/* イベント：最古時刻 */
	uint16_t	mActLog_PastIndex;						/* 動作履歴：最古IndexNo. */
	uint32_t	mActLog_PastTime;						/* 動作履歴：最古時刻 */
	uint16_t	mSysLog_PastIndex;						/* システム履歴：最古IndexNo. */
	uint32_t	mSysLog_PastTime;						/* システム履歴：最古時刻 */
	
	uint16_t	mAbnInf_Sts;							/* 機器異常情報 */
	
	uint32_t	mNewTimeInfo;							/* 最新時刻 */
	uint16_t	mNewMeasVal[ imFlashQue_ChannelNum ];	/* 最新計測値1~3ch */
	uint8_t		mNewLogic;								/* 最新論理 */
	
	uint8_t		mAlmPast[ 5U ];							/* 過去警報フラグ(1~4ch, 機器異常) */
	
	uint32_t	mNewestHistTime;						/* 全履歴中の最新時刻 */
	uint32_t	mNewestLogTime;							/* 計測値の最新時刻 */
	
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
 *	無線
 *==============================================================================
 */
/* ポート割り込みフラグ */
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


/* 計測リアルタイム送信データ(Flashに書き込まれている最新データ) */
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
	uint8_t mMuskMultiEvent : 1U;			/* Idle時の多重イベント防止 */
	uint8_t mMuskCadSetting : 1U;			/* CAD多重セッテング防止フラグ */
	uint8_t mTimerSync : 1U;				/* GWとのタイミング同期中フラグ */
	uint8_t mSetReserve : 1U;				/* GWからの設定変更予約受信フラグ */
//	uint8_t mTxTimeLimit : 1U;				/* GWの累計送信時間制限フラグ */
	uint8_t mModeSelect	:	1U;				/* GWのモード情報 1:RTmode 0:HSmode */
	uint8_t mHsmodeDetectLoop	:	1U;		/* Logger HS Modeのビーコン検知リトライ用フラグ */
	uint8_t mHsmodeInit	:	1U;				/* GW HSmodeでの初期化有無情報 */
//	uint8_t mHsmodeRxRetry : 1U;			/* GW HSmodeでの計測値等の受信CRCエラーでのリトライ受信フラグ */
//	uint8_t mHsmodeRxEnd : 1U;				/* GW HSmodeでの計測値等の受信終了フラグ */
	uint8_t mHsmodeRxEnter : 1U;			/* GW HSmodeでのデータ受信開始フラグ */
	uint8_t mHsmodeTxTurn : 1U;				/* GW HSmodeでのパケット送信回数が奇数か偶数かのフラグ */
	uint8_t mHsmodeFirmSet	: 1U;
//	uint8_t mRtmodeBeaconInit	:	1U;		/* RTmodeでのBeacon送信初期化情報 */
//	uint8_t mRtmodeMeasureInit	:	1U;		/* RTmodeでの計測値送信初期化情報 */
	uint8_t mRtmodePwmode		:	1U;		/* RTmodeでの送信出力選択「PW_H」「PW_L」 */
	uint8_t mPowerInt			:	1U;		/* 電池抜時の割り込みフラグ */
	uint8_t mTimingCorrect		:	1U;		/* 無線通信タイミングの温度補正フラグ 1secごとに実行 */
	uint8_t mTempMeasure		:	1U;
	uint8_t mTempAdc			:	1U;		/* マイコン内蔵温度AD完了フラグ */
	uint8_t mReset				:	1U;		/* キャリアセンス動作エラー時の無線IC初期化フラグ */
	uint8_t mHsmodeRandom		:	1U;
	uint8_t mHsmodeSyncDetect	:	1U;		/* 呼出しビーコンのSyncが検出か計測・設定か 1:検出 */
	uint8_t mHsmodeeResCmdStatus	:	1U;		/* 高速通信モード 呼び出し検知を行う 1:検出 */
	uint8_t mOnlineLcdOnOff		:	1U;		/* 接続通信開始時にLCD点灯し、失敗したら消灯する用途で従来仕様からOR追加 2022.9.16 */
}ST_RF_StsFlag_t;


/*
 *==============================================================================
 *	表示
 *==============================================================================
 */
typedef struct
{
	ET_DispMode_t	mNowDisp;
	ET_DispMode_t	mPreDisp;
} ST_DispSts_t;


typedef struct ST_ActQue
{
	uint8_t		mUserID;					/* 設定者のID */
	uint8_t		mNum;						/* 格納数 */
	uint8_t		mItem[ 90U ];				/* 設定履歴項目 */
	uint8_t		mSetVal[ 90U ][ 3U ];		/* 設定値 */
} ST_ActQue_t;

#endif		/* INCLUDED_STRUCT_H */
