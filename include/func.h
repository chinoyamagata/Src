/*
 *******************************************************************************
 *	File name	:	func.h
 *
 *	[内容]
 *		関数の外部参照(extern)定義
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2019
 *******************************************************************************
 */
#ifndef	INCLUDED_FUNC_H
#define	INCLUDED_FUNC_H


#include "typedef.h"															/* データ型定義 */
#include "struct.h"																/* 構造体定義 */
#include "switch.h"
#include "Lcd_enum.h"															/* LCD表示処理用列挙型定義 */
#include "pfdl.h"																/* Flash Data Library T04 */
#include "pfdl_types.h"
#include "RF_Struct.h"

/*
 ******************************************************************************
 *	main.c
 ******************************************************************************
 */
extern void ApiMain( void );


/*
 ******************************************************************************
 *	Adc.c
 ******************************************************************************
 */
extern void ApiAdc_Initial( void );					/* A/Dコンバータ処理 初期化処理 */
extern void ApiAdc_Main( uint8_t arSel );			/* A/Dコンバータ処理 メイン処理 */
//extern uint16_t ApiAdc_AdCntGet( void );			/* A/Dカウント値取得 */
//extern void ApiAdc_AdStart( void );					/* A/Dスタート */
//ET_BattSts_t ApiAdc_BattStsGet( void );				/* 電池のステータス取得 */
//extern void ApiAdc_BattStsSet( ET_BattSts_t arSts );
extern void ApiAdc_AdExe( void );
extern void ApiAdc_LowStopMotionJdg( void );			/* 低電圧時の動作停止判定処理 */

/*
 ******************************************************************************
 *	Bat.c
 ******************************************************************************
 */
//extern void ApiBat_Initial( void );					/* バッテリ処理 初期化処理 */
//extern void ApiBatChgMain( void );					/* バッテリ変化時の処理 */
//extern void ApiBatLowStopMotionJdg( void );			/* 低電圧時の動作停止判定処理 */


/*
 ******************************************************************************
 *	Modbus.c
 ******************************************************************************
 */
extern void ApiModbus_Initial( void );				/* MODBUS通信 初期化処理 */
extern void ApiModbus_Main( void );					/* MODBUS通信メイン */
#if (swSensorCom == imEnable)
extern void ApiModbus_NoReplyJudge( void );			/* センサ間通信無応答判定 */
#endif


/*
 ******************************************************************************
 *	ComDrv.c
 ******************************************************************************
 */
extern void ApiComDrv_Snd( void );					/* 送信処理 */
extern void ApiComDrv_Rcv( void );					/* 受信処理 */
extern void ApiComDrv_SwitchTx( void );				/* 送信切り替え処理 */
extern void ApiComDrv_SwitchRx( void );				/* 受信切り替え処理 */


/*
 ******************************************************************************
 *	key.c
 ******************************************************************************
 */
extern void ApiKey_Initial( void );					/* キー入力処理 初期化処理 */
extern void ApiKey( void );							/* キー入力処理 */


/*
 ******************************************************************************
 *	Flash.c
 ******************************************************************************
 */
extern void ApiFlash_Initial( void );				/* フラッシュ処理 初期化処理 */
extern void ApiFlash_Main( void );					/* フラッシュ処理メイン */

/* 異常情報の書き込み処理 */
extern void ApiFlash_WriteErrInfo( ET_ErrInfItm_t arItem, uint8_t arAlmFlg );
/* 計測警報の書き込み処理 */
extern void ApiFlash_WriteMeasAlm( uint32_t arTime, uint8_t arAlmFlg, uint8_t arAlmChannel, uint8_t arLevel, sint16_t arMeasVal, ET_MeasAlmItm_t arItem );
/* 計測値1/イベントの書き込み処理 */
extern void ApiFlash_WriteMeasVal1Event( uint8_t arSel );
/* 動作履歴の書き込み処理 */
extern void ApiFlash_WriteActLog( ET_ActLogItm_t arItem, uint32_t arData, uint16_t arUserId );
/* システムログの書き込み処理 */
extern void ApiFlash_WriteSysLog( ET_SysLogItm_t arItem, uint32_t arData );
/* 計測値1、計測値2、システムログのアドレステーブルの書き込み処理 */
extern void ApiFlash_WriteFlasAdrTbl( uint32_t arEndadr, uint16_t arNum, uint16_t arIndex, uint8_t arKind );


extern ET_Error_t ApiFlash_ReadQueSet( uint32_t arStartTime, uint32_t arEndTime, uint16_t arIndexNum, uint8_t arKosu, ET_RegionKind_t arRegionKind, uint8_t arRfMode );

extern void ApiSetFlashSts( ET_FlashSts_t arSts );				/* 外付けフラッシュ状態セット */
extern ET_FlashSts_t ApiGetFlashSts( void );					/* 外付けフラッシュ状態取得処理 */

extern uint8_t ApiFlash_GetQueSts( void );						/* データキューのデータ有無を取得 */
//extern uint8_t ApiFlash_GetQueLowBatSts( void );				/* 低電圧用のデータキューのデータ有無を取得 */

extern uint8_t ApiSetFlash_ReqRdFlg( uint8_t arSelect, ET_RegionKind_t arRegion );
extern void ApiFlash_FinReadData( void );						/* Flashからリードが完了したかを判断する */

/* 内蔵フラッシュへの領域種保存データ取得 */
extern void ApiFlash_GetRegionData( ST_FlashRegionData_t *parRegionData, ET_RegionKind_t arRegionKind );
/* 内蔵フラッシュへの領域種保存データセット */
extern void ApiFlash_SetRegionData( ST_FlashRegionData_t *parRegionData, ET_RegionKind_t arRegionKind );

extern void ApiFlash_SrchFlashAdr( void );						/* 電源投入時にFlash書き込みの続きを検索する */

/* Flashからリードした計測値1をセットして返す */
extern void ApiFlash_RfRTMeasDataGet( ST_FlashVal1Evt_t arRfRtMeasData[] );
extern void ApiFlash_RfRTMeasAlmDataGet( ST_FlashMeasAlm_t arRfRtMeasAlmData[] );

extern void ApiFlash_FlashPowerCtl( uint8_t arSel, uint8_t arWait );	/* Flash電源ポートON/OFF */
extern ET_Error_t ApiFlash_GetEmptyQueueJdg( void );					/* 空きデータキューの判定 */

extern void ApiFlash_QueActHist( ET_ActLogItm_t arItem, uint32_t arNowVal, uint32_t arSetVal, uint8_t arUser );
extern void ApiFlash_StoreActHist( void );
extern void ApiFlash_RfRTAbnStatusSet( void );
extern void ApiFlash_RfRTAlarmFlagSet( uint8_t arAlmFlg[], uint8_t arDevFlg[], uint8_t arSetFlg[] );
extern void ApiFlash_ReadQueSetPastTime( void );
extern void ApiFlash_SetNewMeasVal( void );

/*
 ******************************************************************************
 *	FlashDrv.c
 ******************************************************************************
 */
extern void ApiFlashDrv_Initial( void );			/* フラッシュドライバ処理 初期化処理 */

/* パワーダウンコマンド送信 */
extern ET_Error_t ApiFlashDrv_PowerDown( ET_FlashKind_t arKind );
/* パワーダウン開放コマンド送信 */
extern ET_Error_t ApiFlashDrv_ReleasePowerDown( ET_FlashKind_t arKind );

/* ステータスレジスタ1読み出し */
extern ET_Error_t ApiFlashDrv_ReadStatusReg1( uint8_t *parRegData, ET_FlashKind_t arKind );

/* データ読み出し */
extern ET_Error_t ApiFlashDrv_ReadData( uint32_t arAddress, uint8_t arDataTable[], uint16_t arNum, ET_FlashKind_t arKind );
/* データ書き込み */
extern ET_Error_t ApiFlashDrv_WriteMemory( uint32_t arAddress, const uint8_t arDataTable[], uint16_t arNum, ET_FlashKind_t arKind );
/* セクタ消去 */
extern ET_Error_t ApiFlashDrv_SectorErase( uint32_t arAddress, ET_FlashKind_t arKind );
/* ブロック消去 */
//extern ET_Error_t ApiFlashDrv_BlockErase( uint32_t arAddress, ET_FlashKind_t arKind );
/* チップイレース送信 */
extern ET_Error_t ApiFlashDrv_ChipErase( ET_FlashKind_t arKind );
/* 許可リセット&リセット */
extern ET_Error_t ApiFlashDrv_Reset( ET_FlashKind_t arKind );
extern void ApiFlashDrv_CSCtl( ET_FlashKind_t arKind, uint8_t arCtl );

extern ET_Error_t ApiFlashDrv_FirmMemErase( ET_FlashKind_t arKind );
#if (swKouteiMode == imEnable)
extern ET_Error_t ApiFlashDrv_TestFlash( ET_FlashKind_t arKind );	/* 外付けFlash動作検査 */
#endif

/*
 ******************************************************************************
 *	HmiMain.c
 ******************************************************************************
 */
//extern void ApiHmi_initial( void );					/* ユーザインターフェース 初期化処理 */
extern void ApiHmi_Main( void );					/* ユーザインターフェース処理メイン */
extern uint8_t ApiHmi_BellMarkClrDsp( void );			/* ベルマーククリア */


/*
 ******************************************************************************
 *	InitParameter.c
 ******************************************************************************
 */
extern void InitParameter( void );					/* 個別パラメータ初期化処理 */


/*
 ******************************************************************************
 *	Lcd.c
 ******************************************************************************
 */
extern void ApiLcd_Initial( void );												/* LCD表示初期化処理 */
extern void ApiLcd_Main( void );												/* LCD表示メイン処理 */
extern void ApiLcd_SegDsp( ET_LcdSeg_t arLcdSeg, uint8_t arOnOff );				/* セグメント表示 */

/* 上段側7Seg数値表示 */
extern void ApiLcd_Upp7SegNumDsp( sint16_t arNum, uint8_t arDp1, uint8_t arDp2, uint8_t minus1 );
/* 下段側7Seg数値表示 */
extern void ApiLcd_Low7SegNumDsp( sint16_t arNum, uint8_t arDp );
/* 下段側7Seg数値文字混合表示(電圧パルス) */
extern void ApiLcd_Low7SegVPDsp( sint16_t arNum, uint8_t arflg );
/* 上段/下段7Seg文字表示 */
extern void ApiLcd_UppLow7SegDsp( const char_t arTextTbl[], uint8_t arSelect );
/* 時刻表示 */
extern void ApiLcd_TimeDsp( uint8_t arHour, uint8_t arMinute );
/* 全点灯/全消灯/奇数セグメント点灯/偶数セグメント点灯処理 */
extern void ApiLcd_SegSelectOnOff( ET_DispSegSelect_t arSelect );
/* 上段側オーバーフロー/アンダーフロー文字表示 */
extern void ApiLcd_Upp7SegOvrUdrDsp( uint8_t arSelect );
extern void ApiLcd_Low7SegOvrUdrDsp( uint8_t arSelect );

/* LCD動作確認 */
extern void ApiLcd_LcdDebug( void );

extern void ApiLcd_LcdStop( void );												/* LCDドライバ停止処理 */
//extern void ApiLcd_LcdStart( void );											/* LCDドライバ開始処理 */
//extern void ApiLcd_FirmUpDisp( void );											/* ファームアップ表示処理 */


/*
 ******************************************************************************
 *	Meas.c
 ******************************************************************************
 */
extern void ApiMeas_Initial( void );				/* 測定処理 初期化処理 */
extern void ApiMeas_Main( void );					/* 測定処理メイン */
extern void ApiMeas_MeasFin( void );				/* 測定完了時処理 */


/*
 ******************************************************************************
 *	RfDrv.c
 ******************************************************************************
 */
extern void ApiRFDrv_ParamInitial( void );										/* 無線通信処理 パラメータ初期化処理 */
extern void ApiRFDrv_Initial( void );											/* 無線通信処理 初期化処理 */
extern void ApiRFDrv_Main( void );												/* 無線通信処理メイン処理 */
extern ET_RFDrvInitSts_t ApiRFDrv_GetInitSts( void );							/* Rf(無線通信) 初期化ステータス取得処理 */
extern uint16_t ApiRFDrv_GetSleepLoop( void );
//extern uint8_t ApiRFDrv_BroadLoop( uint8_t arKind );
extern ET_RFDrvInitSts_t ApiRFDrv_GetInitSts( void );
extern void ApiSX1272Reset( uint8_t arRstExe );
extern void ApiRfDrv_SetMeaAlmValArray( sint16_t *arFromAlmParmArray, uint8_t *arToAlmParmArray );	/* 無線用プロトコルに測定値、警報値を書き込む */
#if (swKouteiMode == imEnable)
extern void ApiRfDrv_SetCh( uint16_t arVal, ET_SelRfCh_t arSel );	/* 無線送信チャネル設定 */
#endif
#if (swKouteiMode == imEnable)
extern uint16_t ApiRfDrv_GetCh( ET_SelRfCh_t arSel );				/* 無線送信チャネル読み出し */
#endif
extern uint8_t ApiRfDrv_GetRtConInfo( void );						/* リアルタイム接続情報読み出し */
extern uint8_t ApiRfDrv_HsStsDisp( uint8_t arSel );
extern uint8_t ApiRfDrv_GetRssi( void );
#if (swKouteiMode == imEnable)
extern uint8_t ApiRfDrv_GetSetRssiCareerFlg( uint8_t arSel, uint8_t arVal );		/* 無線受信テストのRSSI、キャリアセンス判定の読み書き */
#endif
extern void ApiRfDrv_MakeReadQue( ET_RegionKind_t arRegionKind, uint32_t arReqStartTime, uint16_t arReqIndexNo );

extern int8_t ApiRfDrv_Temp( void );					/* 無線ICから温度取得 */
extern void ApiRfDrv_BattOffInt( void );

extern void ApiRfDrv_SetReqIndexNo( uint16_t wkQueIndex );

extern void ApiRfDrv_ForcedSleep( void );				/* 電池抜時の無線停止処理 */
extern void ApiRfDrv_MeasData_StorageRfBuff( ST_FlashVal1Evt_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_MeasAlm_StorageRfBuff( ST_FlashMeasAlm_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_MeasErr_StorageRfBuff( ST_FlashErrInfo_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_MeasEvt_StorageRfBuff( ST_FlashVal1Evt_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_ActLog_StorageRfBuff( ST_FlashActLog_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_SysLog_StorageRfBuff( ST_FlashSysLog_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_GrID_Chg( void );
extern uint16_t ApiRfDrv_RtStbyOnOff( uint8_t arSel );


/*
 ******************************************************************************
 *	RtcDrv.c
 ******************************************************************************
 */
extern void ApiRtcDrv_Initial( void );								/* RTC(S-35390A) 初期化処理 */
//extern void ApiRtcDrv_RegInitial( void );							/* RTC(S-35390A) レジスタ初期化処理 */

//extern void ApiRtcDrv_1HzOut( uint8_t arSelect );					/* RTC(S-35390A) 1Hz出力処理 */
//extern uint8_t ApiRtcDrv_1HzOutRead( void );						/* RTC(S-35390A) 1Hz出力制御リード */

extern void ApiRtcDrv_InRtcToExRtc( void );							/* 内蔵RTCの時刻を外付けRTCに書き込む処理 */
extern void ApiRtcDrv_ExRtcToInRtc( void );							/* 外付けRTCの時刻を内蔵RTCに書き込む処理 */
//extern ST_RTC_t ApiRtcDrv_ReadInRtc( void );						/* 内蔵RTC時刻リード */
extern void ApiRtcDrv_ReadInRtc( void );							/* 内蔵RTC時刻リード */
extern void ApiRtcDrv_SetInRtc( ST_RTC_t arClock );					/* 内蔵RTCの時刻をリードしてRAMに格納する処理 */
extern uint32_t ApiRtcDrv_mktime( ST_RTC_t arClock );				/* 経過秒数算出 */
extern ST_RTC_t ApiRtcDrv_localtime( uint32_t arTotalSec );			/* 経過秒数から時刻データへ変換 */


/*
 ******************************************************************************
 *	Sleep.c
 ******************************************************************************
 */
//extern void ApiSleep_Initial( void );				/* スリープ処理 初期化処理 */
extern void ApiSleep_Main( void );					/* スリープ処理メイン */


/*
 ******************************************************************************
 *	TimeComp.c
 ******************************************************************************
 */
extern void ApiRtcInt( void );							/* RTC割込み処理 */
//extern void ApiTimeComp_Initial( void );				/* 時刻補正処理 初期化処理 */
extern void ApiTimeComp_first( uint32_t arRfClock, uint8_t arSize );	/* 無線時刻と内蔵RTCの時刻を比較 */
extern uint8_t ApiIntMeasExist( uint32_t arSec );		/* 収録データ間の時刻差が収録周期の設定と一致するかを判定 */
extern void ApiTimeComp_TimeCompFlgClr( void );			/* 時計補正フラグの無効 */
extern void ApiTimeComp_LimitRtcClock( void );
extern uint32_t ApiTimeComp_GetLocalTime( void );


/*
 ******************************************************************************
 *	Alarm.c
 ******************************************************************************
 */
extern void ApiAlarm_Initial( void );						/* 警報処理 初期化処理 */
extern void ApiAlarm_Main( void );							/* 警報処理 メイン処理 */
extern uint8_t ApiAlarm_ReadMeasAlmFlg( void );

/*
 ******************************************************************************
 *	Abnormal.c
 ******************************************************************************
 */
//extern void ApiAbn_Initial( void );								/* 機器異常処理 初期化処理 */
extern uint16_t ApiAbn_AbnStsGet( ST_AbnInfKind_t arKind );		/* 機器異常状態取得 */
extern uint8_t ApiAbn_AbnNumGet( void );						/* 直近で発生した機器異常No.を取得 */
extern void ApiAbn_AbnNumSet( void );

/* 機器異常セット */
extern void ApiAbn_AbnStsSet( uint16_t arAbnSts, ST_AbnInfKind_t arKind );
/* 機器異常クリア */
extern void ApiAbn_AbnStsClr( uint16_t arAbnSts, ST_AbnInfKind_t arKind );

extern void ApiAbn_Chk( void );				/* ハードエラーの確認 */

extern void ApiAbn_ChkCRC( void );			/* ROM CRC演算処理 */


/*
 ******************************************************************************
 *	FirmUpMain.c
 ******************************************************************************
 */
//extern void ApiFirmUpdateInit( void );										/* ファームアップデート初期化処理 */
extern void ApiFirmUpdateMain( void );											/* ファームアップデートメイン処理 */
extern void ApiFirmUpdate_History( void );


/*
 ******************************************************************************
 *	InFlash.c
 ******************************************************************************
 */
extern void ApiInFlash_PwrOnRead( void );											/* 内蔵フラッシュ処理 起動時読み出し処理 */
extern void ApiInFlash_ParmWrite( ET_InFlashWrSts_t arSelect, uint16_t arAddr );	/* 内蔵Flashにパラメータをライト */
extern uint8_t ApiInFlash_Read( uint16_t arAddr );									/* 内蔵Flashからパラメータをリード */
extern void ApiInFlash_ParmInitFlgChk( void );										/* 工場出荷初期化のフラグ確認 */

extern void ApiInFlash_SetValWrite( ST_RF_Logger_SettingWt_t *arRfBuff );
extern void ApiInFlash_RtSetValWrite1( ST_RF_RT_ChangeSet_Prm1_t *arRfBuff );
extern void ApiInFlash_RtSetValWrite2( ST_RF_RT_ChangeSet_Prm2_t *arRfBuff );
extern void ApiInFlash_RtSetValWrite3( ST_RF_RT_ChangeSet_Prm3_t *arRfBuff );
extern uint16_t ApiInFlash_AlmVal_ValtoCnt( sint16_t arAlmVal );
extern uint8_t ApiInFlash_OftVal_ValtoCnt( sint8_t arOftVal );
extern uint16_t ApiInFlash_ScaleVal_ValtoCnt( sint16_t arScaleVal );

extern uint16_t ApiInFlash_1ByteToBig2Byte( uint8_t *parRfData );
extern uint16_t ApiInFlash_1ByteToLittle2Byte( uint8_t *parRfData );

/*
 ******************************************************************************
 *	内蔵フラッシュメモリドライバ処理
 ******************************************************************************
 */
extern pfdl_status_t ApiInFlashDrv_Open( void );								/* 内蔵フラッシュドライバ 開始処理 */
extern void ApiInFlashDrv_Close( void );										/* 内蔵フラッシュドライバ 終了処理 */

/* 内蔵フラッシュドライバ 読み出しコマンド送信処理 */
extern pfdl_status_t ApiInFlashDrv_RdCmdSnd( uint16_t arRdAddr, uint16_t arRdSize, uint8_t *parRdData );
/* 内蔵フラッシュドライバ 書き込みコマンド送信処理 */
extern pfdl_status_t ApiInFlashDrv_WrCmdSnd( uint16_t arWrAddr, uint16_t arWrSize, uint8_t *parWrData );
/* 内蔵フラッシュドライバ ブランクチェックコマンド送信処理 */
extern pfdl_status_t ApiInFlashDrv_BlnkChkCmdSnd( uint16_t arStrtAddr, uint16_t arExRng );
/* 内蔵フラッシュドライバ ブロック消去コマンド送信処理 */
extern pfdl_status_t ApiInFlashDrv_BlkErsCmdSnd( uint16_t arBlkNo );
/* 内蔵フラッシュドライバ ベリファイコマンド送信処理 */
extern pfdl_status_t ApiInFlashDrv_VerifyCmdSnd( uint16_t arStrtAddr, uint16_t arExRng );
/* 内蔵フラッシュドライバ ハンドラコマンド送信処理 */
extern pfdl_status_t ApiInFlashDrv_HandlerCmdSnd( void );


/*
 ******************************************************************************
 *	r_cg_tau_user.c
 ******************************************************************************
 */
extern uint32_t ApiTau0_GetTau0Ch1Cnt( void );			/* タイマ0チャネル1のカウンタ取得 */
extern uint16_t ApiTau0_GetTau0Ch0Time( void );
extern void ApiTau0_GetTau0Ch0TimeClr( void );
extern void ApiTau0_WaitTimer( uint16_t arCnt );

/*
 ******************************************************************************
 *	r_cg_rtc_user.c
 ******************************************************************************
 */
extern void ApiRtc1HzCtl( uint8_t arCtl );				/* サブクロックによる1Hz出力制御 */
extern uint8_t ApiRtc1HzCtlRead( void );				/* サブクロックによる1Hz出力制御状態リード */


#endif																			/* INCLUDED_FUNC_H */
