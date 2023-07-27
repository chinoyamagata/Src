/*
 *******************************************************************************
 *	File name	:	enum.h
 *
 *	[内容]
 *		列挙型の定義
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]			[概要]
 *	2017.11.08		Softex N.I			新規作成
 *******************************************************************************
 */
#ifndef	INCLUDED_ENUM_H
#define	INCLUDED_ENUM_H


/*
 *------------------------------------------------------------------------------
 *	エラー定義
 *------------------------------------------------------------------------------
 */
typedef enum ET_Error
{
	ecERR_OK = 0,						/* OK判定 */
	ecERR_NG,							/* NG判定 */

	ecERRMax							/* enum最大値 */
} ET_Error_t;


/*
 *------------------------------------------------------------------------------
 *	モード
 *------------------------------------------------------------------------------
 */
typedef enum ET_Mode
{
	ecMode_Normal = 0,					/* 通常モード */
	ecMode_Process,						/* 工程モード */
	ecMode_RfTxLoRa,					/* 無線送信実行:LoRa */
	ecMode_RfTxFSK,						/* 無線送信実行:FSK */
	ecMode_RfTxLoRaStop,
	ecMode_RfTxFSKStop,
	ecMode_RfRxLoRa,					/* 無線受信実行中:LoRa */
	ecMode_RfRxFSK,						/* 無線受信実行中:FSK */
	ecMode_RfRxLoRaStop,
	ecMode_RfRxFSKStop,
	ecMode_RfRxLoRaCons,				/* 無線受信消費電流 */

	ecModeMax							/* enum最大値 */
} ET_Mode_t;


/*
 *------------------------------------------------------------------------------
 *	センサ種
 *------------------------------------------------------------------------------
 */
typedef enum ET_SensType
{
	ecSensType_InTh = 0,				/* 内蔵サーミスタ */
	ecSensType_ExTh,					/* 外付けサーミスタ */
	ecSensType_HumTmp,					/* 温湿度 */
	ecSensType_Pt,						/* 測温抵抗体 */
	ecSensType_V,						/* 電圧 */
	ecSensType_Pulse,					/* パルス */
	ecSensType_KTc,						/* K熱電対 */
	ecSensType_TTc,						/* T熱電対 */
	
	ecSensTypeMax						/* enum最大値 */
} ET_SensType_t;


/*
 *------------------------------------------------------------------------------
 *	測定温度エラー
 *------------------------------------------------------------------------------
 */
typedef enum ET_TempErr
{
	ecTempErr_Non = 0,					/* 異常無し */
	ecTempErr_Ovr,						/* オーバーフロー */
	ecTempErr_Udr,						/* アンダーフロー */

	ecTempErrMax						/* enum最大値 */
} ET_TempErr_t;


/*
 *------------------------------------------------------------------------------
 *	測定チャンネル
 *------------------------------------------------------------------------------
 */
/* チャンネル */
typedef enum ET_Ch
{
	ecCh_1 = 0,							/* 1ch */
	ecCh_2,								/* 2ch */
	ecCh_3,								/* 3ch */
	ecCh_4,

	ecChMax								/* enum最大値 */
} ET_Ch_t;

/*
 *------------------------------------------------------------------------------
 *	通信定義
 *------------------------------------------------------------------------------
 */
#if (swSensorCom == imEnable)
typedef enum ET_ComSts
{
	ecComSts_Init = 0,
	ecComSts_Lcdoff,
	ecComSts_NullPre,															/* NULL送信準備 */
	ecComSts_NullSnd,															/* NULL送信中 */
	ecComSts_UnitPwnOnWait,														/* ユニット起動待ち */
	ecComSts_SndPre,															/* 送信準備 */
	ecComSts_Snd,																/* 送信中 */
	ecComSts_SndEnd,															/* 送信完了 */
	ecComSts_RcvWait,															/* 受信待ち */
	ecComSts_Rcv,																/* 受信中 */
	ecComSts_RcvAnalys,															/* 受信解析 */
	
	ecComStsMax																	/* enum最大値 */
} ET_ComSts_t;
#else
typedef enum ET_ComSts
{
	ecComSts_RcvWait = 0,														/* 受信待ち */
	ecComSts_RcvStart,															/* 受信開始待ち */
	ecComSts_Rcv,																/* 受信中 */
	ecComSts_RcvEnd,															/* 受信完了 */
	ecComSts_SndWait,															/* 送信待ち */
	ecComSts_Snd,																/* 送信中 */
	ecComSts_SndEnd,															/* 送信完了 */
	
	ecComStsMax																	/* enum最大値 */
} ET_ComSts_t;
#endif


/*
 *------------------------------------------------------------------------------
 *	A/Dコンバータ処理ステータス
 *------------------------------------------------------------------------------
 */
typedef enum ET_AdcSts
{
	ecAdcSts_Nop = 0,															/* 無処理 */
	ecAdcSts_DryStart,															/* 乾電池用A/D変換開始 */
	ecAdcSts_DryAct,															/* 乾電池用A/D変換中 */
	ecAdcSts_DryFin,															/* 乾電池用A/D変換完了 */

	ecAdcStsMax																	/* enum最大値 */
} ET_AdcSts_t;


/*
 *------------------------------------------------------------------------------
 *	日時情報
 *------------------------------------------------------------------------------
 */
typedef enum ET_DateTime
{
	ecDateTime_Year = 0,														/* 年 */
	ecDateTime_Month,															/* 月 */
	ecDateTime_Day,																/* 日 */
	ecDateTime_Week,															/* 曜 */
	ecDateTime_Hour,															/* 時 */
	ecDateTime_Min,																/* 分 */
	ecDateTime_Sec,																/* 秒 */
	
	ecDateTimeMax																/* enum最大値 */
} ET_DateTime_t;


/*
 *------------------------------------------------------------------------------
 *	RTC(S-35390A) 初期化ステータス
 *------------------------------------------------------------------------------
 */
typedef enum ET_RtcDrvInitSts
{
	ecRtcDrvInitSts_RegInit = 0,												/* レジスタ初期化 */
	ecRtcDrvInitSts_RealRead,													/* リアルタイムデータ読み出し */
	ecRtcDrvInitSts_InitEnd,													/* 初期化完了 */
	ecRtcDrvInitSts_24Write,

	ecRtcDrvInitStsMax
} ET_RtcDrvInitSts_t;


/*
 *------------------------------------------------------------------------------
 *	内蔵RTC状態
 *------------------------------------------------------------------------------
 */
typedef enum ET_RtcSts
{
	ecRtcSts_Non = 0,															/* 異常無し */
	ecRtcSts_Fast,																/* 内蔵RTC進み */
	ecRtcSts_Delay,																/* 内蔵RTC遅れ */

	ecRtcStsMax																	/* enum最大値 */
} ET_RtcSts_t;

/*
 *------------------------------------------------------------------------------
 *	外付けフラッシュメモリ種類
 *------------------------------------------------------------------------------
 */
typedef enum ET_FlashKind
{
	ecFlashKind_Prim = 0,														/* プライマリ */
	ecFlashKind_Second,															/* セカンダリ */
	
	ecFlashKindMax																/* enum最大値 */
} ET_FlashKind_t;


/*
 *------------------------------------------------------------------------------
 *	外付けフラッシュ状態
 *------------------------------------------------------------------------------
 */
typedef enum ET_FlashSts
{
	ecFlashSts_PowOn = 0,														/* Flash電源ON */
	ecFlashSts_PowOnWait,														/* Flash電源安定待ち */
	ecFlashSts_Init,															/* 初期状態 */
	ecFlashSts_SctrErase,														/* セクタ消去中状態 */
	ecFlashSts_FirstSctrWr,														/* 前半セクタ書き込み中状態 */

	ecFlashStsMax																/* enum最大値 */
} ET_FlashSts_t;


/*
 *------------------------------------------------------------------------------
 *	外付けFlash領域種
 *------------------------------------------------------------------------------
 */
typedef enum ET_RegionKind
{
	ecRegionKind_Non = 0,														/* 初期値 */
	ecRegionKind_ErrInfo,														/* 異常情報 */
	ecRegionKind_MeasAlm,														/* 計測警報 */
	ecRegionKind_Meas1,															/* 測定値1 */
	ecRegionKind_Event,															/* イベント */
	ecRegionKind_ActLog,														/* 動作履歴 */
	ecRegionKind_SysLog,														/* システムログ */
	ecRegionKind_Meas1Adr,
	ecRegionKind_SysLogAdr,

	ecRegionKindMax																/* enum最大値 */
} ET_RegionKind_t;


/*
 *------------------------------------------------------------------------------
 *	測定処理フェーズ
 *------------------------------------------------------------------------------
 */
typedef enum ET_MeasPhase
{
	ecMeasPhase_Init = 0,
	ecMeasPhase_Lcdoff,
	ecMeasPhase_InitRef,														/* リファレンス初期 */
	ecMeasPhase_ChrgRef,														/* リファレンス充電 */
	ecMeasPhase_MeasRef,														/* リファレンス計測 */
	ecMeasPhase_DisChrgRef,														/* リファレンス放電 */
	ecMeasPhase_DisChrgRefWait,
	ecMeasPhase_InitTh,															/* サーミスタ初期 */
	ecMeasPhase_ChrgTh,															/* サーミスタ充電 */
	ecMeasPhase_MeasTh,															/* サーミスタ計測 */
	ecMeasPhase_DisChrgTh,														/* サーミスタ放電 */
	ecMeasPhase_Wait,															/* 測定待ち */

	ecMeasPhaseMax																/* enum最大値 */
} ET_MeasPhase_t;


/*
 *------------------------------------------------------------------------------
 *	収録周期1
 *------------------------------------------------------------------------------
 */
typedef enum ET_RecKind1
{
	ecRecKind1_None = 0,
	ecRecKind1_5sec,															/* 5秒 */
	ecRecKind1_10sec,															/* 10秒 */
	ecRecKind1_30sec,															/* 30秒 */
	ecRecKind1_1min,															/* 1分 */
	ecRecKind1_5min,															/* 5分 */
	ecRecKind1_10min,															/* 10分 */
	ecRecKind1_15min,															/* 15分 */
	ecRecKind1_30min,															/* 30分 */
	ecRecKind1_60min,															/* 60分 */

	ecRecKind1Max																/* enum最大値 */
} ET_RecKind1_t;


/*
 *------------------------------------------------------------------------------
 *	収録周期2
 *------------------------------------------------------------------------------
 */
typedef enum ET_RecKind2
{
	ecRecKind2_None = 0,
	ecRecKind2_5sec,															/* 5秒 */
	ecRecKind2_10sec,															/* 10秒 */
	ecRecKind2_30sec,															/* 30秒 */
	ecRecKind2_1min,															/* 1分 */
	ecRecKind2_5min,															/* 5分 */
	ecRecKind2_10min,															/* 10分 */

	ecRecKind2Max																/* enum最大値 */
} ET_RecKind2_t;


/*
 *------------------------------------------------------------------------------
 *	無線通信 初期化ステータス
 *------------------------------------------------------------------------------
 */
typedef enum ET_RFDrvInitSts
{
	ecRFDrvInitSts_Init = 0,													/* 初期化開始 */
	ecRFDrvInitSts_InitPorWait,													/* 初期化実行中 */
	ecRfDrvInitSts_InitRstWait,													/* SX1272リセット実行ウェイト */
	ecRfDrvInitSts_InitRstEndWait,												/* SX1272リセット後のウェイト */
	ecRFDrvInitSts_InitEnd,														/* 初期化完了 */

	ecRFDrvInitStsMax
} ET_RFDrvInitSts_t;


/*
 *------------------------------------------------------------------------------
 *	ROM書き換えステータス
 *------------------------------------------------------------------------------
 */
typedef enum ET_FirmUpSts
{
	ecFirmUpSts_Init = 0,														/* 初期化 */
	ecFirmUpSts_BlankCheck,														/* ブランクチェック */
	ecFirmUpSts_Erase,															/* 消去 */
	ecFirmUpSts_Write,															/* 書込み */
	ecFirmUpSts_Verify,															/* ベリファイ */
	ecFirmUpSts_Close,															/* 終了処理 */
	ecFirmUpSts_End,															/* 終了 */

	ecFirmUpStsMax
} ET_FirmUpSts_t;


/*
 *------------------------------------------------------------------------------
 *	表示モード
 *------------------------------------------------------------------------------
 */
typedef enum ET_DispMode
{
	ecDispMode_UpVerDwNon = 0,
	ecDispMode_UpVerDwNon2,		/* 上段：Ver、下段：なし */
	ecDispMode_Up1CHDwClk,		/* 上段：1CH、下段：時計 */
	ecDispMode_Up1CHDw2CH,		/* 上段：1CH、下段：2CH */
	ecDispMode_Up1CHDw3CH,		/* 上段：1CH、下段：3CH */
	ecDispMode_UpNonDw2CH,		/* 上段：なし、下段：2CH */
	ecDispMode_UpNonDw3CH,		/* 上段：なし、下段：3CH */
	ecDispMode_UpNonDwClk,		/* 上段：なし、下段：時計 */
	ecDispMode_UpNonDwCon,		/* 上段：なし、下段：接点情報 */
	ecDispMode_UpErrDwRtc,		/* 上段：Err、下段：RTC */
	ecDispMode_AllSegOn,		/* 全セグメントON */
	ecDispMode_AllSegOff,		/* 全セグメントOFF */
	ecDispMode_Lock,			/* 上段：Lck、下段：なし */
	ecDispMode_Lock2,			/* 上段：Lck、下段：なし */
	ecDispMode_OddSeg,			/* 奇数セグメント */
	ecDispMode_EvenSeg,			/* 偶数セグメント */
	ecDispMode_UpComDwRssi,		/* 上段ComTarget、下段RSSI */
//	ecDispMode_UpPtDwNon,		/* 上段：Pt、下段：なし */
	ecDispMode_UpIntDwNon,		/* 上段：int、下段：なし */
	ecDispMode_HsCh,			/* 上段：'ch’、下段：高速通信ch */
	ecDispMode_Resf,			/* 上段：'rst’、下段：RESF */
	ecDispMode_Appid,			/* 上段：' id’、下段：事業会社ID */
	ecDispMode_Rssi,			/* 上段：'LEV’、下段：RSSI */
	ecDispMode_Gwid,			/* 上段：'con’、下段：GW ID */
	ecDispMode_AbnChkErr,		/* 上段：Err、下段：エラーNo. */
	ecDispMode_Ver,				/* 上段：Ver、下段：VerNo. */
	ecDispMode_Up1CHDw2CH_V,	/* 上段：整数部（integer part）、下段：小数部（decimal part）電圧モデル */
	ecDispMode_Up1CHDw2CH_P,	/* 上段:千の位、下段:一～百の位 パルスモデル */
	
	ecDispModeMax
} ET_DispMode_t;


typedef enum ET_DispSegSelect
{
	ecDispSegSelect_SegAllOn = 0,
	ecDispSegSelect_SegAllOff,
	ecDispSegSelect_SegOddOn,
	ecDispSegSelect_SegEvenOn,
	
	ecDispSegSelectMax
} ET_DispSegSelect_t;

/*
 *------------------------------------------------------------------------------
 *	異常情報項目
 *------------------------------------------------------------------------------
 */
typedef enum ET_ErrInfItm
{
	ecErrInfItm_Init = 0,					/* 0:初期 */
	ecErrInfItm_BatEmpty,					/* 1:電池残量(空) */
	
	ecErrInfItm_1chOvrRng,					/* 2:チャネル1オーバーレンジ */
	ecErrInfItm_2chOvrRng,					/* 3:チャネル2オーバーレンジ */
	ecErrInfItm_3chOvrRng,					/* 4:チャネル3オーバーレンジ */
	
	ecErrInfItm_1chUdrRng,					/* 5:チャネル1アンダーレンジ */
	ecErrInfItm_2chUdrRng,					/* 6:チャネル2アンダーレンジ */
	ecErrInfItm_3chUdrRng,					/* 7:チャネル3アンダーレンジ */
	
	ecErrInfItm_1chBrnOut,					/* 8:チャネル1バーンアウト */
	ecErrInfItm_2chBrnOut,					/* 9:チャネル2バーンアウト */
	ecErrInfItm_3chBrnOut,					/* 10:チャネル3バーンアウト */
	
	ecErrInfItm_1chSnsShrt,					/* 11:チャネル1センサ短絡 */
	ecErrInfItm_2chSnsShrt,					/* 12:チャネル2センサ短絡 */
	ecErrInfItm_3chSnsShrt,					/* 13:チャネル3センサ短絡 */
	
	ecErrInfItm_SnsComErr,					/* 14:センサ通信異常 */
	ecErrInfItm_4chCalErr,					/* 15:チャネル4演算エラー */
	
	ecErrInfItm_RfIcErr,					/* 16:無線IC異常 */
	ecErrInfItm_PrimMemErr,					/* 17:プライマリメモリ異常 */
	ecErrInfItm_SecondMemErr,				/* 18:セカンダリメモリ異常 */
	ecErrInfItm_MiComRomErr,				/* 19:マイコンROM異常 */
	ecErrInfItm_ClkErr,						/* 20:クロック発振異常 */
	ecErrInfItm_KeyBehErr,					/* 21:キー動作不良 */
	ecErrInfItm_ExRtcErr,					/* 22:外付けRTC異常 */
	ecErrInfItm_InRtcErr,					/* 23内蔵RTC異常 */
	ecErrInfItm_BatLow,						/* 24:電池残量低下 */
	
	ecErrInfItm_SnsErr,						/* 25:センサ機器異常 */

	ecErrInfItmMax							/* enum最大値 */
} ET_ErrInfItm_t;


typedef enum ET_AbnInfKind
{
	ecAbnInfKind_AbnSts = 0,				/* 機器異常情報 */
	ecAbnInfKind_MeasSts,					/* 測定関連の異常情報 */
	
	ecAbnInfKindMax							/* enum最大値 */
} ST_AbnInfKind_t;


/*
 *------------------------------------------------------------------------------
 *	計測警報項目
 *------------------------------------------------------------------------------
 */
typedef enum ET_MeasAlmItm
{
	ecMeasAlmItm_Init = 0,			/* 初期 */
	ecMeasAlmItm_MeasDev,			/* 計測値逸脱 */
	ecMeasAlmItm_MeasAlmDelayCnt,	/* 計測値警報(遅延回数経過) */
	ecMeasAlmItm_MeasAlmDevTime,	/* 計測値警報(逸脱許容時間経過) */

	ecMeasAlmItm_OvRng,
	ecMeasAlmItm_UdRng,
	ecMeasAlmItm_Burn,
	ecMeasAlmItm_Short,

	ecMeasAlmItmMax					/* enum最大値 */
} ET_MeasAlmItm_t;


/* 警報種類 */
typedef enum ET_AlmKind
{
	ecAlmKind_HH = 0,				/* 上上限 */
	ecAlmKind_H,					/* 上限 */
	ecAlmKind_L,					/* 下限 */
	ecAlmKind_LL,					/* 下下限 */
	
	ecAlmKind_Max
} ET_AlmKind_t;

/* 警報ステータス */
typedef enum ET_AlmSts
{
	ecAlmSts_Non = 0,				/* 未発生 */
	ecAlmSts_DelayCnt,				/* 遅延回数計測中 */
	ecAlmSts_ActDelayCnt,			/* 警報発生中(遅延回数経過) */
	ecAlmSts_ActDevTime,			/* 警報発生中(逸脱許容時間経過) */

	ecAlmStsMax
} ET_AlmSts_t;

/* 過去警報フラグ */
typedef enum ET_AlmPastSts
{
	ecAlmPastSts_Non = 0,			/* 過去警報未発生 */
	ecAlmPastSts_Pre,				/* 過去警報準備 */
	ecAlmPastSts_Set,				/* 過去警報発生 */
	
	ecAlmPastStsMax
} ET_AlmPastSts_t;

/* 無線送信用警報フラグ */
typedef enum ET_AlmFlg
{
	ecAlmFlg_Non = 0,				/* なし */
	ecAlmFlg_DevH,					/* 上限逸脱 */
	ecAlmFlg_AlmH,					/* 上限警報 */
	ecAlmFlg_DevHDevHH,				/* 上限逸脱上上限逸脱 */
	ecAlmFlg_AlmHDevHH,				/* 上限警報上上限逸脱 */
	ecAlmFlg_DevHAlmHH,				/* 上限逸脱上上限警報 */
	ecAlmFlg_AlmHAlmHH,				/* 上限警報上上限警報 */
	ecAlmFlg_DevL,					/* 下限逸脱 */
	ecAlmFlg_AlmL,					/* 下限警報 */
	ecAlmFlg_DevLDevLL,				/* 下限逸脱下下限逸脱 */
	ecAlmFlg_AlmLDevLL,				/* 下限警報下下限逸脱 */
	ecAlmFlg_DevLAlmLL,				/* 下限逸脱下下限警報 */
	ecAlmFlg_AlmLAlmLL,				/* 下限警報下下限警報 */
	
	ecAlmFlgMax
} ET_AlmFlg_t;


/*
 *------------------------------------------------------------------------------
 *	動作履歴項目
 *------------------------------------------------------------------------------
 */
typedef enum ET_ActLogItm
{
	ecActLogItm_Init = 0,				/* 初期 */
	ecActLogItm_Reserve1,				/* 1:未使用 */
	ecActLogItm_Boot,					/* 2:起動 */
	ecActLogItm_BattIn,					/* 3:電池交換 */
	ecActLogItm_BattBackup,				/* 4:電池交換バックアップ状態 */
	ecActLogItm_RfStopBatEmpty,			/* 5:無線停止(電池残量空) */
	ecActLogItm_RfAirplaneOn,			/* 6:機内モードON */
	ecActLogItm_RfAirplaneOff,			/* 7:機内モードOFF */
	ecActLogItm_RfStopBatLow,			/* 8:無線停止(電池残量0本) */
	ecActLogItm_Reserve9,
	ecActLogItm_FrmUpdate,				/* 10:ファームウェア更新 */
	ecActLogItm_DisSetReq,				/* 11:不正設定要求 */
	ecActLogItm_Reserve12,
	ecActLogItm_Reserve13,
	ecActLogItm_Reserve14,
	ecActLogItm_Reserve15,
	ecActLogItm_Reserve16,
	ecActLogItm_Reserve17,
	ecActLogItm_Reserve18,
	ecActLogItm_Reserve19,
	
	ecActLogItm_UUDevi1,				/* 20:上上限警報値(1ch) */
	ecActLogItm_UDevi1,					/* 21:上限警報値(1ch) */
	ecActLogItm_LDevi1,					/* 22:下限警報値(1ch) */
	ecActLogItm_LLDevi1,				/* 23:下下限警報値(1ch) */
	ecActLogItm_UUDelay1,				/* 24:上上限警報遅延値(1ch) */
	ecActLogItm_UDelay1,				/* 25:上限警報遅延値(1ch) */
	ecActLogItm_LDelay1,				/* 26:下限警報遅延値(1ch) */
	ecActLogItm_LLDelay1,				/* 27:下下限警報遅延値(1ch) */
	ecActLogItm_DeviEnaLv1,				/* 28:逸脱許容時間選択閾値(1ch) */
	ecActLogItm_DeviEnaTime1,			/* 29:逸脱許容時間(1ch) */
	ecActLogItm_Offset1,				/* 30:計測値オフセット(1ch) */
	ecActLogItm_Slope1,					/* 31:計測値一次傾き補正(1ch) */
	
	ecActLogItm_UUDevi2,				/* 32:上上限警報値(2ch) */
	ecActLogItm_UDevi2,					/* 33:上限警報値(2ch) */
	ecActLogItm_LDevi2,					/* 34:下限警報値(2ch) */
	ecActLogItm_LLDevi2,				/* 35:下下限警報値(2ch) */
	ecActLogItm_UUDelay2,				/* 36:上上限警報遅延値(2ch) */
	ecActLogItm_UDelay2,				/* 37:上限警報遅延値(2ch) */
	ecActLogItm_LDelay2,				/* 38:下限警報遅延値(2ch) */
	ecActLogItm_LLDelay2,				/* 39:下下限警報遅延値(2ch) */
	ecActLogItm_DeviEnaLv2,				/* 40:逸脱許容時間選択閾値(2ch) */
	ecActLogItm_DeviEnaTime2,			/* 41:逸脱許容時間(2ch) */
	ecActLogItm_Offset2,				/* 42:計測値オフセット(2ch) */
	ecActLogItm_Slope2,					/* 43:計測値一次傾き補正(2ch) */
	
	ecActLogItm_UUDevi3,				/* 44:上上限警報値(3ch) */
	ecActLogItm_UDevi3,					/* 45:上限警報値(3ch) */
	ecActLogItm_LDevi3,					/* 46:下限警報値(3ch) */
	ecActLogItm_LLDevi3,				/* 47:下下限警報値(3ch) */
	ecActLogItm_UUDelay3,				/* 48:上上限警報遅延値(3ch) */
	ecActLogItm_UDelay3,				/* 49:上限警報遅延値(3ch) */
	ecActLogItm_LDelay3,				/* 50:下限警報遅延値(3ch) */
	ecActLogItm_LLDelay3,				/* 51:下下限警報遅延値(3ch) */
	ecActLogItm_DeviEnaLv3,				/* 52:逸脱許容時間選択閾値(3ch) */
	ecActLogItm_DeviEnaTime3,			/* 53:逸脱許容時間(3ch) */
	ecActLogItm_Offset3,				/* 54:計測値オフセット(3ch) */
	ecActLogItm_Slope3,					/* 55:計測値一次傾き補正(3ch) */
	
	ecActLogItm_UUDevi4,				/* 56:上上限警報値(4ch) */
	ecActLogItm_UDevi4,					/* 57:上限警報値(4ch) */
	ecActLogItm_LDevi4,					/* 58:下限警報値(4ch) */
	ecActLogItm_LLDevi4,				/* 59:下下限警報値(4ch) */
	ecActLogItm_UUDelay4,				/* 60:上上限警報遅延値(4ch) */
	ecActLogItm_UDelay4,				/* 61:上限警報遅延値(4ch) */
	ecActLogItm_LDelay4,				/* 62:下限警報遅延値(4ch) */
	ecActLogItm_LLDelay4,				/* 63:下下限警報遅延値(4ch) */
	ecActLogItm_DeviEnaLv4,				/* 64:逸脱許容時間選択閾値(4ch) */
	ecActLogItm_DeviEnaTime4,			/* 65:逸脱許容時間(4ch) */
	ecActLogItm_Offset4,				/* 66:計測値オフセット(4ch) */
	ecActLogItm_Slope4,					/* 67:計測値一次傾き補正(4ch) */
	
	ecActLogItm_BatType,				/* 68:電池種 */
	ecActLogItm_KeyLock,				/* 69:キーロック */
	ecActLogItm_RealTimeCh,				/* 70:リアルタイム通信待受ch */
	ecActLogItm_LogCyc1,				/* 71:収録周期1 */
	ecActLogItm_LogCyc2,				/* 72:収録周期2 */
	ecActLogItm_LoggerName1,			/* 73:ロガー名称1/10 */
	ecActLogItm_LoggerName2,			/* 74:ロガー名称2/10 */
	ecActLogItm_LoggerName3,			/* 75:ロガー名称3/10 */
	ecActLogItm_LoggerName4,			/* 76:ロガー名称4/10 */
	ecActLogItm_LoggerName5,			/* 77:ロガー名称5/10 */
	ecActLogItm_LoggerName6,			/* 78:ロガー名称6/10 */
	ecActLogItm_LoggerName7,			/* 79:ロガー名称7/10 */
	ecActLogItm_LoggerName8,			/* 80:ロガー名称8/10 */
	ecActLogItm_LoggerName9,			/* 81:ロガー名称9/10 */
	ecActLogItm_LoggerName10,			/* 82:ロガー名称10/10 */
	ecActLogItm_FstConnCh,				/* 83:高速通信待受ch */
	ecActLogItm_FstConnBoot,			/* 84:高速通信起動のログデータ閾値 */
	ecActLogItm_AlertType,				/* 85:警報監視演算種 */
	ecActLogItm_AlertParm,				/* 86:警報監視演算パラメータ */
	ecActLogItm_AlertFlag,				/* 87:警報監視演算フラグ記録 */
	ecActLogItm_EventKeyFlg,			/* 88:イベントボタン制御 */
	ecActLogItm_DeviClrPoint,			/* 89:逸脱許可時間クリア手段 */
	ecActLogItm_DeviClrTime,			/* 90:逸脱許容時間クリア時刻 */
	ecActLogItm_CalDate,				/* 91:校正日 */
	ecActLogItm_ScalDecimal,			/* 92:スケーリング変換 小数点位置 */
	ecActLogItm_ScalYInt,				/* 93:スケーリング変換 切片 */
	ecActLogItm_ScalGrad,				/* 94:スケーリング変換 傾き */
	ecActLogItm_FlashSelect,			/* 95:プライマリフラッシュメモリ切り替え */
	ecActLogItm_CnctEnaGwId1,			/* 96:接続許可ゲートウェイID1 */
	ecActLogItm_CnctEnaGwId2,			/* 97:接続許可ゲートウェイID2 */
	ecActLogItm_CnctEnaGwId3,			/* 98:接続許可ゲートウェイID3 */
	ecActLogItm_CnctEnaGwId4,			/* 99:接続許可ゲートウェイID4 */
	ecActLogItm_CnctEnaGwId5,			/* 100:接続許可ゲートウェイID5 */
	ecActLogItm_CnctEnaGwId6,			/* 101:接続許可ゲートウェイID6 */
	ecActLogItm_CnctEnaGwId7,			/* 102:接続許可ゲートウェイID7 */
	ecActLogItm_CnctEnaGwId8,			/* 103:接続許可ゲートウェイID8 */
	ecActLogItm_CnctEnaGwId9,			/* 104:接続許可ゲートウェイID9 */
	ecActLogItm_CnctEnaGwId10,			/* 105:接続許可ゲートウェイID10 */
	ecActLogItm_GrId,					/* 106:グループID */
	
	
	ecActLogItmMax						/* enum最大値 */
} ET_ActLogItm_t;


/*
 *------------------------------------------------------------------------------
 *	システムログ項目
 *------------------------------------------------------------------------------
 */
typedef enum ET_SysLogItm
{
	ecSysLogItm_Init = 0U,				/* 初期 */
	ecSysLogItm_FastCom,				/* 1:高速通信 */
	ecSysLogItm_KeyDisp,				/* 2:DISPキー操作 */
	ecSysLogItm_EleWvTstMd,				/* 3:電波テストモード */
	ecSysLogItm_RealTmDisCom,			/* 4:長距離通信切断 */
	ecSysLogItm_FlashIcErr,				/* 5:フラッシュICエラー */
	ecSysLogItm_RealTmCom,				/* 6:長距離通信接続 */
	ecSysLogItm_TimeComp,				/* 7:時刻補正 */
	ecSysLogItm_IdDiff,					/* 7:ID書き換わり */

	ecSysLogItmMax						/* enum最大値 */
} ET_SysLogItm_t;


/*
 *------------------------------------------------------------------------------
 *	電池
 *------------------------------------------------------------------------------
 */
/* 電池ステータス */
typedef enum ET_BattSts
{
	ecBattSts_Init = 0U,			/* 電池初期 */
	ecBattSts_H,					/* 電池高 */
	ecBattSts_M,					/* 電池中 */
	ecBattSts_L,					/* 電池低 */
	ecBattSts_Empty,				/* 電池空 */
	ecBattSts_Non,					/* 電池なし */
	
	ecBattStsMax
} ET_BattSts_t;

/* 電池種類 */
typedef enum ET_BatType
{
	ecBatType_Init = 0U,
	ecBatType_Alk,					/* アルカリ電池 */
	ecBatType_Li,					/* リチウム電池 */
	ecBatType_NiHM,					/* ニッケル水素電池 */
	ecBatType_Large,				/* 大容量バッテリ */
	
	ecBatTypeMax
} ET_BatType_t;

/* 電池割込状態 */
typedef enum ET_BattInt
{
	ecBattInt_Init = 0U,
	ecBattInt_Low,					/* 低電圧割込発生 */
	ecBattInt_LowLoop,				/* 低電圧割込の繰返しAD */
	ecBattInt_High,					/* 電圧L→H割込発生 */
	ecBattInt_NonHigh,				/* 電池なしからの電圧H割込発生 */
	ecBattInt_LowFlashWr,			/* 低電圧LoでFlashライト直前 */
	ecBattInt_LowFlashWrFin,		/* 低電圧LoでFlashライト直前のAD完了 */
	
	ecBattIntMax
} ET_BattInt_t;

typedef enum ET_AdcPhase
{
	ecAdcPhase_Init = 0,
	ecAdcPhase_CtlWait,
	ecAdcPhase_AdExe,

	ecAdcPhaseMax
} ET_AdcPhase_t;

/*
 *------------------------------------------------------------------------------
 *	工程用ステータス
 *------------------------------------------------------------------------------
 */
/* 無線チャネル選択 */
typedef enum ET_SelRfCh
{
	ecSelRfCh_CurrentCh_LoRa = 0,
	ecSelRfCh_CurrentCh_FSK,
	ecSelRfCh_HsCh,
	ecSelRfCh_LoraChGrupe,

	ecSelRfChMax
} ET_SelRfCh_t;

/*
 *------------------------------------------------------------------------------
 *	各機能の動作状態遷移
 *------------------------------------------------------------------------------
 */

/* 測定状態 */
typedef enum ET_MeaModuleSts
{
	ecMeaModuleSts_Init = 0,
	ecMeaModuleSts_Run,
	ecMeaModuleSts_Wait,
	ecMeaModuleSts_Sleep,

	ecMeaModuleStsMax
} ET_MeaModuleSts_t;

/* RTC状態 */
typedef enum ET_RtcIntSts
{
	ecRtcIntSts_Init = 0,
	ecRtcIntSts_Run,
	ecRtcIntSts_Sleep,
	
	ecRtcIntStsMax
} ET_RtcIntSts_t;


/* 内蔵Flash書込み実行状態 */
typedef enum ET_InFlashWrExeSts
{
	ecInFlashWrExeSts_Init = 0,
	ecInFlashWrExeSts_Run,
	ecInFlashWrExeSts_Sleep,
	
	ecInFlashWrExeStsMax
} ET_InFlashStsWrExe_t;


/* 動作状態遷移 */
typedef enum ET_ExeModuleSts
{
	ecExeModuleSts_Init = 0,				/* 初期 */
	ecExeModuleSts_Meas,					/* 測定 */
	ecExeModuleSts_Log1,					/* 収録1 */
	ecExeModuleSts_Log2,					/* 収録2 */
	ecExeModuleSts_Alm1,					/* 警報1 */
	ecExeModuleSts_Alm2,					/* 警報2 */
	ecExeModuleSts_ExFlsWr1,				/* 外付けフラッシュ書き込み1 */
	ecExeModuleSts_Sleep,					/* スリープ */

	ecExeModuleStsMax
} ET_ExeModuleSts_t;

/* キー割込み状態 */
typedef enum ET_KeyIntModuleSts
{
	ecKeyIntModuleSts_Init = 0,				/* 初期 */
	ecKeyIntModuleSts_ChatEventMode,		/* Eventキー入力によるチャタリング測定中 */
	ecKeyIntModuleSts_ChatDispMode,			/* Displayキー入力によるチャタリング測定中 */
	ecKeyIntModuleSts_ChatResetMode,		/* Resetキー入力によるチャタリング測定中 */
	ecKeyIntModuleSts_EventMode,			/* Eventキー1秒長押しによるイベント実行 */
	ecKeyIntModuleSts_DispMode,				/* Dispキー単押しによる、表示切り替え実行 */
	ecKeyIntModuleSts_RfMode,				/* Dispキー1秒長押しによる無線モード切り替え実行 */
	ecKeyIntModuleSts_ResetMode,			/* Resetキー1秒長押しによる出荷状態初期化実行 */
//	ecKeyIntModuleSts_RtmSndPtResetDspMode,	/* Resetキー2秒長押しによるリアルタイム通信ポインタ初期化表示実行 */
//	ecKeyIntModuleSts_RtmSndPtResetMode,	/* Resetキー2秒長押しによるリアルタイム通信ポインタ初期化実行 */
	ecKeyIntModuleSts_Sleep,
	
	ecKeyModuleStsMax
} ET_KeyIntModuleSts_t;


/* Eventキー状態 */
typedef enum ET_EventKeyModuleSts
{
	ecEventKeyModuleSts_Init = 0,			/* 初期 */
	ecEventKeyModuleSts_Meas,
	ecEventKeyModuleSts_Alm,
	ecEventKeyModuleSts_ExFlsWr,
	ecEventKeyModuleSts_Sleep,
	
	ecEventKeyModuleStsMax
} ET_EventKeyModuleSts_t;


/* 電池AD状態 */
typedef enum ET_BatModuleSts
{
	ecBatModuleSts_Init = 0,
	ecBatModuleSts_Run,
	ecBatModuleSts_Wait,
	ecBatModuleSts_Sleep,

	ecBatModuleStsMax
} ET_BatModuleSts_t;


/* UART通信状態 */
typedef enum ET_ComModuleSts
{
	ecComModuleSts_Init = 0,
	ecComModuleSts_Run,
	ecComModuleSts_Wait,
	ecComModuleSts_Sleep,

	ecComModuleStsMax
} ET_ComModuleSts_t;



/* RF通信状態 */
typedef enum ET_RfModuleSts
{
	ecRfModuleSts_Init = 0,
	ecRfModuleSts_Run,
	ecRfModuleSts_Sleep,

	ecRfModuleStsMax
} ET_RfModuleSts_t;


/* RTC制御割り込み状態 */
typedef enum ET_RtcModuleSts
{
	ecRtcModuleSts_Init = 0,
	ecRtcModuleSts_Run,
	ecRtcModuleSts_Wait,
	ecRtcModuleSts_Update,
	ecRtcModuleSts_UpdateGain,
	ecRtcModuleSts_UpdateLose,
	ecRtcModuleSts_Sleep,

	ecRtcModuleStsMax
} ET_RtcModuleSts_t;


/* 外付けFlash状態 */
typedef enum ET_ExFlashModuleSts
{
	ecExFlashModuleSts_Init = 0,
	ecExFlashModuleSts_Run,
	ecExFlashModuleSts_Sleep,

	ecExFlashModuleStsMax
} ET_ExFlashModuleSts_t;


/* 液晶制御状態 */
typedef enum ET_LcdModuleSts
{
	ecLcdModuleSts_Init = 0,
	ecLcdModuleSts_Run,
	ecLcdModuleSts_Sleep,

	ecLcdModuleStsMax
} ET_LcdModuleSts_t;



/* 外付けFlashリード状態 */
typedef enum ET_ExFlashRdModuleSts
{
	ecExFlashRdModuleSts_Init = 0,
	ecExFlashRdModuleSts_Run,
	ecExFlashRdModuleSts_Sleep,
	
	ecExFlashRdModuleStsMax
} ET_ExFlashRdModuleSts_t;






/* 内蔵フラッシュ書込みステータス */
typedef enum ET_InFlashWrSts
{
	ecInFlashWrSts_ParmInit = 0,		/* 設定値のみ初期値でライト */
	ecInFlashWrSts_Process,				/* 工程プロセス、設定値を初期値でライト */
	ecInFlashWrSts_Process2,			/* 工程プロセス、設定値を初期値でライト */
	ecInFlashWrSts_ParmNow,				/* 現時点RAM変数をライト */

	ecInFlashWrStsMax
} ET_InFlashWrSts_t;


typedef enum ET_LowBatFlashSts
{
	ecLowBatFlashSts_Init = 0U,			/* 通常 */
	ecLowBatFlashSts_Now,														/* 低電圧中 */

	ecLowBatFlashStsMax															/* enum最大値 */
} ET_LowBatFlashSts_t;


/* 履歴格納状態 */
typedef enum ET_HistorySts
{
	ecHistoryModuleSts_Init = 0,
	ecHistoryModuleSts_Run,
	ecHistoryModuleSts_Sleep,

	ecHistoryModuleStsMax
} ET_HistorySts_t;

/* ファームアップ状態 */
typedef enum ET_FirmupSts
{
	ecFirmupModuleSts_Init = 0,
	ecFirmupModuleSts_Pre,
	ecFirmupModuleSts_Run,
	ecFirmupModuleSts_Sleep,

	ecFirmupModuleStsMax
} ET_FirmupSts_t;



#endif																			/* INCLUDED_ENUM_H */
