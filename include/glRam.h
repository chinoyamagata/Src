/*
 *******************************************************************************
 *	File name	:	glRam.h
 *
 *	[内容]
 *		グローバル変数定義ファイル
 *	[注記]
 *
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2017.11.08		Softex N.I		新規作成
 *******************************************************************************
 */

#include	"typedef.h"										/* データ型定義 */
#include	"enum.h"										/* 列挙型の定義 */
#include	"struct.h"										/* 構造体定義 */
#include	"switch.h"

/* ROMバージョン */
extern const uint8_t	cRomVer[ 4U ];

/* ROM図番 */
//extern const uint8_t	cRomFigNo;

/* センサ種 */
//extern const ET_SensType_t	cSensType;


extern ET_Mode_t	gvMode;									/* モード */

/* ファームアップモード */
extern uint8_t gvFirmupMode;

/* 時刻 */
extern ST_RTC_t		gvClock;								/* RTCデータ */
extern uint32_t		gvLocalTime;							/* ローカル時刻 */

/* MODBUS通信 */
extern ST_ModInf_t	gvModInf;								/* MODBUS通信情報 */


/* 外付けフラッシュメモリ */
extern uint8_t		gvCsi10SndEndFlg;						/* CSI10送信完了フラグ */

/* リアルタイムクロック */
extern uint8_t		gvIicSndEndFlg;							/* IIC送信完了フラグ */
extern uint8_t		gvIicRcvEndFlg;							/* IIC受信完了フラグ */

/* 測定処理 */
//extern ET_MeasPhase_t	gvMeasPhase;						/* 測定処理フェーズ */
//extern ST_MeasTmCnt_t	gvMeasRefTmCnt;						/* リファレンス用タイマカウント */
//extern ST_MeasTmCnt_t	gvMeasThTmCnt;						/* サーミスタ用タイマカウント */
extern ST_MeasPrm_t	gvMeasPrm;								/* 測定処理用パラメータ */
extern uint8_t	gvHsAlmFlag[ 4U ];

/* 警報状態 */
extern ET_AlmSts_t gvAlmSts[ imChannelNum ][ imAllAlmNum ];	/* 警報状態 */

/* 警報逸脱クリア契機 */
//extern uint8_t		gvAlmClrPoint;						/* 警報逸脱クリア契機 */

/* Hmi */
extern ST_DispSts_t	gvDispMode;								/* 表示モード */
extern uint16_t gvLcdComGwId;								/* 接続GWIDの下3桁(LCD表示用) */


/* 割り込み */
extern uint32_t		gvRecCycRtcTm;							/* 収録周期到達時の内蔵RTCの時刻 */

/* 電池 */
extern uint8_t		gvAdConvFinFlg;							/* 電池AD測定完了フラグ */
extern ET_LowBatFlashSts_t		gvBatLowSts;				/* 低電圧用状態 */
extern ET_BattInt_t				gvBatt_Int;					/* 電池低下割込状態 */

extern ST_BatAd_t				gvBatAd;					/* 電池状態 */


/* 無線部ポート割り込みフラグ */
extern ST_RF_INT_t	gvRfIntFlg;								/* 無線IC DIO割り込みフラグ */

/* モジュール動作ステータス */
extern	ST_ModuleSts_t	gvModuleSts;

/* 無線タイミングTimer Counter */
extern uint16_t gvRfTimingCounterL;
extern uint16_t gvRfTimingCounterH;
extern uint8_t gvRfTimingTempCorrect;						/* gvRfTimingCounterの温度補正 ppm(マイナス) */
extern uint8_t gvRfTimingCorrectCnt;						/* gvRfTimingCounterの温度補正を実行する秒カウンタ 3secごとに実行 */
extern ST_RF_StsFlag_t gvrfStsEventFlag;;
extern uint16_t vrfITSyncCount;
extern uint8_t vrfRTmodeCommFailNum;
extern uint16_t vrfTest;

extern sint8_t		gvInTemp;								/* マイコン測定の内蔵温度 */


/* 無線送信データ */
extern ST_RTMeasData_t gvRfRTMeasData;						/* 計測データのリアルタイム送信内容 */
extern ST_RTMeasAlm2Data_t gvRfRTMeasAlm2Data;				/* 計測周期2の長距離送信内容 */

extern uint16_t gvFstConnBootCnt;							/* 高速通信起動のための収録データカウンタ */


/* 逸脱許容時間クリアパラメータ */
extern	uint8_t		gvDeviEnaFlg;

/* 内蔵Flash格納パラメータ */
extern ST_InFlashTbl_t			gvInFlash;

extern ST_ActQue_t gvActQue;								/* 設定変更履歴の一次格納キュー */

extern uint8_t gvCycTestStart;								/* クロック発振の自己診断開始 */

extern ET_RtcSts_t 				gvRtcSts;					/* 時刻補正ステータス */

extern uint16_t	gvMeasAlmNum;								/* 計測警報発生/解除回数 */
extern uint8_t gvHsMeasAlmFlg;								/* 計測警報フラグ */

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


