/*
 *******************************************************************************
 *	File name	:	ComDrv.c
 *
 *	[内容]
 *		通信ドライバ
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2017.11.13		Softex N.I		新規作成
 *******************************************************************************
 */
#include "cpu_sfr.h"															/* SFR定義ヘッダー */

#include "typedef.h"															/* データ型定義 */
#include "glLabel.h"															/* グローバル変数・定数(const)ラベル定義 */

#include "r_cg_sau.h"

/*
 *==============================================================================
 *	define定義
 *==============================================================================
 */


/*
 *==============================================================================
 *	列挙型定義
 *==============================================================================
 */


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



#pragma section text MY_APP2
/*
 *******************************************************************************
 *	送信処理
 *
 *	[内容]
 *		上位へコマンドをシリアル送信する｡
 *******************************************************************************
 */
void ApiComDrv_Snd( void )
{
	R_UART2_Send(&gvModInf.mSndBuf[0], gvModInf.mSndLen);
}


/*
 *******************************************************************************
 *	通信受信処理
 *
 *	[内容]
 *		上位からコマンドをシリアル受信する｡
 *******************************************************************************
 */
void ApiComDrv_Rcv( void )
{
	R_UART2_Receive(&gvModInf.mRcvBuf[gvModInf.mRcvPos], 1U);					/* 1Byte受信 */
}


/*
 *******************************************************************************
 *	送信切り替え処理
 *
 *	[内容]
 *		半二重通信の送信側に切り替える
 *******************************************************************************
 */
void ApiComDrv_SwitchTx( void )
{
	R_UART2_RcvStop();															/* 通信ポート受信停止 */
	R_UART2_SndStart();															/* 通信ポート送信開始 */
}


/*
 *******************************************************************************
 *	受信切り替え処理
 *
 *	[内容]
 *		半二重通信の受信側に切り替える
 *******************************************************************************
 */
void ApiComDrv_SwitchRx( void )
{
	R_UART2_SndStop();															/* 通信ポート送信停止 */
	R_UART2_RcvStart();															/* 通信ポート受信開始 */
}
#pragma section
