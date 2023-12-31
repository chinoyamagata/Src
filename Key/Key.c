/*
 *******************************************************************************
 *	File name	:	Key.c
 *
 *	[内容]
 *		キー入力処理
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2017.11.10		Softex N.I		新規作成
 *******************************************************************************
 */
#include "cpu_sfr.h"															/* SFR定義ヘッダー */

#include "typedef.h"															/* データ型定義 */
#include "UserMacro.h"															/* ユーザー作成マクロ定義 */
#include "immediate.h"															/* immediate定数定義 */
#include "switch.h"																/* コンパイルスイッチ定義 */
#include "enum.h"																/* 列挙型の定義 */
#include "struct.h"																/* 構造体定義 */
#include "func.h"																/* 関数の外部参照(extern)定義 */
#include "glLabel.h"															/* グローバル変数・定数(const)ラベル定義 */

/*
 *==============================================================================
 *	define定義
 *==============================================================================
 */
#define imKeyBufIndex			3U											/* キーバッファ要素数 */
#define im100msCnt				240000U										/* 1ステート(=1/(24*10^6)=41ns) */

/* キーロック */
#define	imLockCh		M_BIT0
#define	imLockEvent		M_BIT1
#define	imLockReset		M_BIT2
#define	imLockSelRf		M_BIT3
#define	imLockAllKey	(imLockCh | imLockEvent | imLockSelRf | imLockReset)

/*
 *==============================================================================
 *	構造体定義
 *==============================================================================
 */



/*
 *==============================================================================
 *	変数定義
 *==============================================================================
 */



/*
 *==============================================================================
 *	定数定義
 *==============================================================================
 */



/*
 *==============================================================================
 *	プロトタイプ宣言
 *==============================================================================
 */
static void SubKey_Disp( void );


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	キー入力処理 初期化処理
 *
 *	[内容]
 *		キー入力処理の初期化処理
 *******************************************************************************
 */
void ApiKey_Initial( void )
{
	PU7 &= 0xCFU;			/* デフォルト:PullUPあり、消費電流に影響するのでPullUpなしに設定 */
	
	/* Key入力許可 */
	R_KEY_Start();
	R_INTC0_Start();
}


/*
 *******************************************************************************
 *	キー入力処理
 *
 *	[内容]
 *		キー入力処理
 *******************************************************************************
 */
void ApiKey( void )
{
	uint8_t		wkU8;
	uint8_t		wkResetCnt = 0U;
	uint8_t		wkBufCnt = 0U;
	uint8_t		wkKeyBuf[ imKeyBufIndex ] = { imON, imON, imON };
	static uint8_t	vAppIdClrSts = 0U;
	
	
	/* Eventキー1s長押し */
	if( gvModuleSts.mKeyInt == ecKeyIntModuleSts_EventMode )
	{
		gvModuleSts.mKeyInt = ecKeyIntModuleSts_Sleep;
		
		/* キーロック設定あり */
		if( M_TSTBIT(gvInFlash.mParam.mKeyLock, imLockEvent) )
		{
			gvDispMode.mNowDisp = ecDispMode_Lock;
			gvModuleSts.mLcd = ecLcdModuleSts_Run;
		}
		else
		{
#if (swSensorCom == imEnable)
			if( gvInFlash.mProcess.mModelCode != 0xFFU )
			{
				if( gvInFlash.mParam.mEventKeyFlg == imOFF )
				{
					/* Event有効 */
					gvInFlash.mParam.mEventKeyFlg = imON;
					
					/* Event有効時のIndexNo.に現在の収録IndexNo.をセット */
					gvInFlash.mData.mMeas1_EvtPtrIndex = gvInFlash.mData.mMeas1_Index;
					if( gvInFlash.mData.mMeas1_EvtPtrIndex > imIndexMaxMeas1 )
					{
						gvInFlash.mData.mMeas1_EvtPtrIndex = 1U;
					}
				}
				else
				{
					/* Event無効 */
					gvInFlash.mParam.mEventKeyFlg = imOFF;
					
					/* Event無効時はEvent有効時IndexNo.をクリアする */
					gvInFlash.mData.mMeas1_EvtPtrIndex = 0U;
				}
				gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
				
				gvModuleSts.mEventKey = ecEventKeyModuleSts_Meas;
				/* 通信処理 */
				if( gvModuleSts.mCom == ecComModuleSts_Sleep )
				{
					gvModuleSts.mCom = ecComModuleSts_Run;
				}
			}
#else
			if( gvInFlash.mParam.mEventKeyFlg == imOFF )
			{
				/* Event有効 */
				gvInFlash.mParam.mEventKeyFlg = imON;
				
				/* Event有効時のIndexNo.に現在の収録IndexNo.をセット */
				gvInFlash.mData.mMeas1_EvtPtrIndex = gvInFlash.mData.mMeas1_Index;
				if( gvInFlash.mData.mMeas1_EvtPtrIndex > imIndexMaxMeas1 )
				{
					gvInFlash.mData.mMeas1_EvtPtrIndex = 1U;
				}
			}
			else
			{
				/* Event無効 */
				gvInFlash.mParam.mEventKeyFlg = imOFF;
				
				/* Event無効時はEvent有効時IndexNo.をクリアする */
				gvInFlash.mData.mMeas1_EvtPtrIndex = 0U;
			}
			gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
			
			gvModuleSts.mEventKey = ecEventKeyModuleSts_Meas;
			if( gvModuleSts.mMea == ecMeaModuleSts_Sleep )
			{
				gvModuleSts.mMea = ecMeaModuleSts_Run;
			}
#endif
		}
	}
	
	/* Dispキー単押し */
	if( gvModuleSts.mKeyInt == ecKeyIntModuleSts_DispMode )
	{
		gvModuleSts.mKeyInt = ecKeyIntModuleSts_Sleep;
		
		SubKey_Disp();
		gvModuleSts.mLcd = ecLcdModuleSts_Run;
	}
	
	/* Dispキー1s長押し */
	if( gvModuleSts.mKeyInt == ecKeyIntModuleSts_RfMode )
	{
		gvModuleSts.mKeyInt = ecKeyIntModuleSts_Sleep;

#if (swSensorCom == imEnable)		
		if( gvDispMode.mNowDisp == ecDispMode_Up1CHDwClk ||
			gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH ||
			gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH_V||
			gvDispMode.mNowDisp == ecDispMode_Up1CHDw2CH_P )
#else
		if( gvDispMode.mNowDisp == ecDispMode_Up1CHDwClk )
#endif
		{
			/* キーロック設定あり */
			if( M_TSTBIT(gvInFlash.mParam.mKeyLock, imLockSelRf) )
			{
				gvDispMode.mNowDisp = ecDispMode_Lock;
			}
			else
			{
				ApiRfDrv_RtStbyOnOff( 1U );
			}
			gvModuleSts.mLcd = ecLcdModuleSts_Run;
		}
		else if( gvDispMode.mNowDisp == ecDispMode_HsCh )
		{
			/* キーロック設定あり */
			if( M_TSTBIT(gvInFlash.mParam.mKeyLock, imLockCh) )
			{
				gvDispMode.mNowDisp = ecDispMode_Lock;
			}
			else
			{
				wkU8 = gvInFlash.mParam.mrfHsCh;
				gvInFlash.mParam.mrfHsCh ++;
				if( gvInFlash.mParam.mrfHsCh > 2U )
				{
					gvInFlash.mParam.mrfHsCh = 0U;
				}
				gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
				ApiFlash_QueActHist( ecActLogItm_FstConnCh, wkU8, gvInFlash.mParam.mrfHsCh + 1U, 4 );				/* 設定変更履歴 */
			}
			gvModuleSts.mLcd = ecLcdModuleSts_Run;
		}
		else if( gvDispMode.mNowDisp == ecDispMode_Appid )
		{
			/* アプリID初期化の隠しキー操作 */
			
			/* EVENTキー:OFF、DISPキー:OFF */
			if( !M_TSTBIT(P7, M_BIT4) && !M_TSTBIT(P7, M_BIT5) )
			{
				vAppIdClrSts = 0U;
			}
			else
			{
				gvModuleSts.mKeyInt = ecKeyIntModuleSts_RfMode;
				switch( vAppIdClrSts )
				{
					case 0U:
						/* EVENTキー:ON、DISPキー:ON */
						if( M_TSTBIT(P7, M_BIT4) )
						{
							vAppIdClrSts++;
						}
						break;
					case 1U:
						/* EVENTキー:ON、DISPキー:OFF */
						if( M_TSTBIT(P7, M_BIT4) && !M_TSTBIT(P7, M_BIT5) )
						{
							vAppIdClrSts++;
						}
						break;
					case 2U:
						/* EVENTキー:ON、DISPキー:ON */
						if( M_TSTBIT(P7, M_BIT4) && M_TSTBIT(P7, M_BIT5) )
						{
							vAppIdClrSts++;
						}
						break;
					case 3U:
						/* EVENTキー:OFF、DISPキー:ON、バッテリHi */
						if( !M_TSTBIT(P7, M_BIT4) && M_TSTBIT(P7, M_BIT5) && M_TSTBIT(P0, M_BIT1) )
						{
							vAppIdClrSts++;
						}
						break;
					case 4U:
						/* DISPキー:ON、バッテリLo */
						if( M_TSTBIT(P7, M_BIT5) && !M_TSTBIT(P0, M_BIT1) )
						{
							vAppIdClrSts++;
						}
						break;
					case 5U:
						/*	DISPキー:ON、バッテリHi */
						if( M_TSTBIT(P7, M_BIT5) && M_TSTBIT(P0, M_BIT1) )
						{
							gvInFlash.mParam.mAppID[ 0U ] = 0x0F;
							gvInFlash.mParam.mAppID[ 1U ] = 0xFF;
							gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
							
							vAppIdClrSts = 0U;
							gvModuleSts.mLcd = ecLcdModuleSts_Run;
							gvModuleSts.mKeyInt = ecKeyIntModuleSts_Sleep;
						}
						break;
					default:
						break;
				}
			}
		}
	}
	
	/* Resetキー1s長押し */
	if( gvModuleSts.mKeyInt == ecKeyIntModuleSts_ResetMode )
	{
		/* キーロック設定あり */
		if( M_TSTBIT(gvInFlash.mParam.mKeyLock, imLockReset) )
		{
			gvDispMode.mNowDisp = ecDispMode_Lock;
			gvModuleSts.mKeyInt = ecKeyIntModuleSts_Sleep;
			gvModuleSts.mLcd = ecLcdModuleSts_Run;
		}
		else
		{
			/* 起動時の工場出荷初期化フラグをON */
			M_SETBIT( gvInFlash.mParam.mParmFlg, imFactoryInitFlg );
			
			ApiInFlash_ParmWrite( ecInFlashWrSts_ParmNow, 0U );
			
			/* 「int」表示 */
			ApiLcd_SegSelectOnOff( ecDispSegSelect_SegAllOff );
			ApiLcd_UppLow7SegDsp("int", imHigh);
			ApiLcd_UppLow7SegDsp("   ", imLow);
			ApiLcd_Main();
			
			/* RESETキーのチャタリング対応 */
			while( 1U )
			{
				R_WDT_Restart();									/* WDTリスタート */
				wkResetCnt ++;
				if( wkResetCnt > 20U )
				{
					wkResetCnt = 0U;
					
					if( KEY_RESET == imHigh )						/* RESETキーON/OFF判定 */
					{												/* OFFの場合 */
						wkKeyBuf[wkBufCnt] = imON;
					}
					else
					{												/* ONの場合 */
						wkKeyBuf[wkBufCnt] = imOFF;
					}
					
					if( (wkKeyBuf[ 0U ] == imOFF) && (wkKeyBuf[ 1U ] == imOFF) && (wkKeyBuf[ 2U ] == imOFF) )
					{												/* 3回連続でOFFが続いた場合 */
						break;
					}
					else
					{												/* 何れかがONだった場合 */
						wkBufCnt++;									/* バッファカウンタ更新 */
						if( wkBufCnt >= imKeyBufIndex )				/* バッファ要素数判定 */
						{
							wkBufCnt = 0U;							/* バッファカウンタ初期化 */
						}
					}
				}
			}
			
			R_INTC5_Start();
			while( 1U )
			{
				/* 電池投入割込発生 */
				if( gvBatt_Int == ecBattInt_NonHigh || gvBatt_Int == ecBattInt_High )
				{
					break;
				}
				M_HALT;
			}
			WDTE = 0xFFU;											/* 強制リセット */
		}
	}
}


/*
 *******************************************************************************
 *	DISPキー入力処理
 *
 *	[内容]
 *		DISPキー入力処理
 *******************************************************************************
 */
static void SubKey_Disp( void )
{
	switch( gvDispMode.mNowDisp )
	{
		default:
		case ecDispMode_Up1CHDwClk:
			if( imOFF == ApiHmi_BellMarkClrDsp() )
			{
#if (swSensorCom == imEnable)
				if( gvInFlash.mProcess.mModelCode == ecSensType_Pt )
				{
					gvDispMode.mNowDisp = ecDispMode_UpNonDwCon;
				}
				else
				{
					gvDispMode.mNowDisp = ecDispMode_Appid;
				}
#else
				gvDispMode.mNowDisp = ecDispMode_Appid;
#endif
			}
			break;
#if (swSensorCom == imEnable)
		case ecDispMode_Up1CHDw2CH:
		case ecDispMode_Up1CHDw2CH_V:							/* 電圧 */
		case ecDispMode_Up1CHDw2CH_P:							/* パルス */
			if( imOFF == ApiHmi_BellMarkClrDsp() )
			{
				gvDispMode.mNowDisp = ecDispMode_UpNonDwClk;
			}
			break;
		case ecDispMode_UpNonDwClk:
			gvDispMode.mNowDisp = ecDispMode_Appid;
			break;
		case ecDispMode_UpNonDwCon:
			gvDispMode.mNowDisp = ecDispMode_Appid;
			break;
#endif
		case ecDispMode_Appid:
			gvDispMode.mNowDisp = ecDispMode_HsCh;
			break;
			
#if (swRssiLogDebug == imEnable)
		case ecDispMode_UpComDwRssi:
			gvDispMode.mNowDisp = ecDispMode_HsCh;
			break;
#endif
		case ecDispMode_HsCh:
#if (swRESFDisp == imEnable)
			gvDispMode.mNowDisp = ecDispMode_Resf;
#else
			gvDispMode.mNowDisp = ecDispMode_Rssi;
#endif
			break;
		case ecDispMode_Rssi:
			gvDispMode.mNowDisp = ecDispMode_Gwid;
			break;
		case ecDispMode_Gwid:
			gvDispMode.mNowDisp = ecDispMode_Ver;
			break;
		case ecDispMode_Resf:
			gvDispMode.mNowDisp = ecDispMode_Up1CHDwClk;
			break;
		case ecDispMode_AbnChkErr:
			gvDispMode.mNowDisp = gvDispMode.mPreDisp;
			break;
		case ecDispMode_Ver:
#if (swSensorCom == imEnable)		
			switch( gvInFlash.mProcess.mModelCode )
			{
				default:
				case ecSensType_Pt:
					gvDispMode.mNowDisp = ecDispMode_Up1CHDwClk;
					break;
				case ecSensType_HumTmp:
					gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH;
					break;
				case ecSensType_V:										/* 電圧 */
					gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH_V;
					break;
				case ecSensType_Pulse:									/* パルス */
					gvDispMode.mNowDisp = ecDispMode_Up1CHDw2CH_P;
					break;
			}
#else
			gvDispMode.mNowDisp = ecDispMode_Up1CHDwClk;
#endif			
			break;
	}
	
#if swRfTestLcd == imEnable
	if( M_TSTFLG(gvRfTestLcdFlg) )
	{
		M_CLRFLG(gvRfTestLcdFlg);
	}
	else
	{
		M_SETFLG(gvRfTestLcdFlg);
		gvDispMode.mNowDisp = ecDispMode_Up1CHDwClk;
	}
#endif
}

#pragma section




