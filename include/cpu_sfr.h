/*
 *******************************************************************************
 *	File name	:	cpu_sfr.h
 *
 *	[内容]
 *		CPU毎に異なる、SFR定義ヘッダーファイル等を、
 *		本ファイルにインクルードし、cpu_sfr.hに統一する。
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2017.11.08		Softex N.I		新規作成
 *******************************************************************************
 */
#ifndef	INCLUDED_CPU_SFR_H
#define	INCLUDED_CPU_SFR_H


/*
 ***************************************
 *	RL78/L13用(Renesas製純正コンパイラ)
 ***************************************
 */
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_tau.h"
#include "r_cg_rtc.h"
#include "r_cg_adc.h"
#include "r_cg_comp.h"
#include "r_cg_sau.h"
#include "r_cg_lcd.h"
#include "r_cg_intp.h"
#include "r_cg_key.h"
#include "r_cg_it.h"
#include "r_cg_wdt.h"




#define	M_NOP				__nop()												/* NOP命令 */
#define	M_STOP				__stop()											/* STOP命令 */
#define	M_HALT				__halt()											/* HALT命令 */


#endif																			/* INCLUDED_CPU_SFR_H */
