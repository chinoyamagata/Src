/*
 *******************************************************************************
 *	File name	:	InFlashDrv.c
 *
 *	[内容]
 *		内蔵フラッシュドライバ処理
 *	[適応]
 *		ブロック0~3(各ブロック1KB)
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2018.11.07		Softex K.U		新規作成
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

#include "pfdl.h"																/* Flash Data Library T04 */
#include "pfdl_types.h"

/*
 *==============================================================================
 *	define定義
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

#pragma section text MY_APP2
/*
 *******************************************************************************
 *	内蔵フラッシュドライバ 開始処理
 *
 *	[内容]
 *		内蔵フラッシュドライバ 開始処理
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_Open( void )
{
	pfdl_descriptor_t	vInit;
	pfdl_status_t		wkResult;												/* 戻り値 */
	
	vInit.fx_MHz_u08 = 24; 														/* CPU frequency : 24MHz */
	vInit.wide_voltage_mode_u08 = 0;											/* Voltage mode : full speed mode */ 
	
	NOP();
	wkResult = PFDL_Open(&vInit);
	NOP();
	
	return wkResult;
}


/*
 *******************************************************************************
 *	内蔵フラッシュドライバ 終了処理
 *
 *	[内容]
 *		内蔵フラッシュドライバ 終了処理
 *******************************************************************************
 */
void ApiInFlashDrv_Close( void )
{
	PFDL_Close();
}


/*
 *******************************************************************************
 *	内蔵フラッシュドライバ 読み出しコマンド送信処理
 *
 *	[内容]
 *		内蔵フラッシュドライバ 読み出しコマンド送信処理
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_RdCmdSnd( uint16_t arRdAddr, uint16_t arRdSize, uint8_t *parRdData )
{
	pfdl_request_t	vInFlashDrvPrm;
	pfdl_status_t	wkResult;												/* 戻り値 */

	/* 読み出しコマンド初期化 */
	vInFlashDrvPrm.index_u16 = arRdAddr;										/* 読み込み開始アドレス */
	vInFlashDrvPrm.bytecount_u16 = arRdSize;									/* 読み込みサイズ */
	vInFlashDrvPrm.data_pu08 = parRdData;										/* 読み込みデータ入力バッファのアドレス */
	vInFlashDrvPrm.command_enu = PFDL_CMD_READ_BYTES;							/* 読み出しコマンド */

	wkResult = PFDL_Execute(&vInFlashDrvPrm);

	return wkResult;
}


/*
 *******************************************************************************
 *	内蔵フラッシュドライバ 書き込みコマンド送信処理
 *
 *	[内容]
 *		内蔵フラッシュドライバ 書き込みコマンド送信処理
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_WrCmdSnd( uint16_t arWrAddr, uint16_t arWrSize, uint8_t *parWrData )
{
	pfdl_request_t	vInFlashDrvPrm;
	pfdl_status_t	wkResult;													/* 戻り値 */

	/* 読み出しコマンド初期化 */
	vInFlashDrvPrm.index_u16 = arWrAddr;										/* 書き込み開始アドレス */
	vInFlashDrvPrm.bytecount_u16 = arWrSize;									/* 書き込みサイズ */
	vInFlashDrvPrm.data_pu08 = parWrData;										/* 書き込みデータ入力バッファのアドレス */
	vInFlashDrvPrm.command_enu = PFDL_CMD_WRITE_BYTES;							/* 書き込みコマンド */

	wkResult = PFDL_Execute(&vInFlashDrvPrm);

	return wkResult;
}


/*
 *******************************************************************************
 *	内蔵フラッシュドライバ ブランクチェックコマンド送信処理
 *
 *	[内容]
 *		内蔵フラッシュドライバ ブランクチェックコマンド送信処理
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_BlnkChkCmdSnd( uint16_t arStrtAddr, uint16_t arExRng )
{
	pfdl_request_t	vInFlashDrvPrm;
	pfdl_status_t	wkResult;													/* 戻り値 */

	/* 読み出しコマンド初期化(不要パラメータには0を設定する) */
	vInFlashDrvPrm.index_u16 = arStrtAddr;										/* ブランクチェック開始アドレス */
	vInFlashDrvPrm.bytecount_u16 = arExRng;										/* ブランクチェックサイズ */
	vInFlashDrvPrm.data_pu08 = 0;												/* 不要パラメータ */
	vInFlashDrvPrm.command_enu = PFDL_CMD_BLANKCHECK_BYTES;						/* ブランクチェックコマンド */

	wkResult = PFDL_Execute(&vInFlashDrvPrm);

	return wkResult;
}


/*
 *******************************************************************************
 *	内蔵フラッシュドライバ ブロック消去コマンド送信処理
 *
 *	[内容]
 *		内蔵フラッシュドライバ ブロック消去コマンド送信処理
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_BlkErsCmdSnd( uint16_t arBlkNo )
{
	pfdl_request_t	vInFlashDrvPrm;
	pfdl_status_t	wkResult;													/* 戻り値 */

	/* 読み出しコマンド初期化(不要パラメータには0を設定する) */
	vInFlashDrvPrm.index_u16 = arBlkNo;											/* ブロック番号 */
	vInFlashDrvPrm.bytecount_u16 = 0;											/* 不要パラメータ */
	vInFlashDrvPrm.data_pu08 = 0;												/* 不要パラメータ */
	vInFlashDrvPrm.command_enu = PFDL_CMD_ERASE_BLOCK;							/* ブロック消去コマンド */

	wkResult = PFDL_Execute(&vInFlashDrvPrm);

	return wkResult;
}


/*
 *******************************************************************************
 *	内蔵フラッシュドライバ ベリファイコマンド送信処理
 *
 *	[内容]
 *		内蔵フラッシュドライバ ベリファイコマンド送信処理
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_VerifyCmdSnd( uint16_t arStrtAddr, uint16_t arExRng )
{
	pfdl_request_t	vInFlashDrvPrm;
	pfdl_status_t	wkResult;													/* 戻り値 */

	/* 読み出しコマンド初期化(不要パラメータには0を設定する) */
	vInFlashDrvPrm.index_u16 = arStrtAddr;										/* 開始アドレス */
	vInFlashDrvPrm.bytecount_u16 = arExRng;										/* 開始アドレスからの実行範囲 */
	vInFlashDrvPrm.data_pu08 = 0;												/* 不要パラメータ */
	vInFlashDrvPrm.command_enu = PFDL_CMD_IVERIFY_BYTES;						/* ベリファイコマンド */

	wkResult = PFDL_Execute(&vInFlashDrvPrm);

	return wkResult;
}


/*
 *******************************************************************************
 *	内蔵フラッシュドライバ ハンドラコマンド送信処理
 *
 *	[内容]
 *		内蔵フラッシュドライバ ハンドラコマンド送信処理
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_HandlerCmdSnd( void )
{
	pfdl_status_t	wkResult;													/* 戻り値 */

	wkResult = PFDL_Handler();

	return wkResult;
}
#pragma section


