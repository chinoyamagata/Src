/*
 *******************************************************************************
 *	File name	:	InitParameter.c
 *
 *	[内容]
 *		個別パラメータ初期化処理
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2017.11.19		Softex N.I		新規作成
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


#pragma section text MY_APP
/*
 *******************************************************************************
 *	個別パラメータ初期化処理
 *
 *	[内容]
 *		個別パラメータの初期化処理を行う。
 *******************************************************************************
 */
void InitParameter( void )
{
	ApiKey_Initial();															/* キー入力処理 初期化処理 */

	ApiAdc_Initial();															/* A/Dコンバータ処理 初期化処理 */

//	ApiBat_Initial();															/* バッテリ処理 初期化処理 */

#if (swSensorCom == imEnable)
	ApiModbus_Initial();														/* MODBUS通信 初期化処理 */
#endif

//	ApiHmi_initial();															/* ユーザインターフェース 初期化処理 */

	ApiLcd_Initial();															/* LCD表示 初期化処理 */

	ApiMeas_Initial();															/* 測定処理 初期化処理 */

//	ApiSleep_Initial();															/* スリープ処理 初期化処理 */

	ApiFlashDrv_Initial();														/* フラッシュドライバ処理 初期化処理 */

	ApiFlash_Initial();															/* フラッシュ処理 初期化処理 */

	ApiRFDrv_ParamInitial();													/* 無線通信処理 パラメータ初期化処理 */

//	ApiTimeComp_Initial();														/* 時刻補正処理 初期化処理 */

	ApiAlarm_Initial();															/* 警報処理 初期化処理 */

	R_INTC5_Start();															/* 電池残量低下割り込み許可 */

//	ApiFirmUpdateInit();														/* ファームアップデート処理 初期化処理 */
	
}
#pragma section text MY_APP

