/*
 *******************************************************************************
 *	File name	:	cpu_sfr.h
 *
 *	[���e]
 *		CPU���ɈقȂ�ASFR��`�w�b�_�[�t�@�C�������A
 *		�{�t�@�C���ɃC���N���[�h���Acpu_sfr.h�ɓ��ꂷ��B
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2017.11.08		Softex N.I		�V�K�쐬
 *******************************************************************************
 */
#ifndef	INCLUDED_CPU_SFR_H
#define	INCLUDED_CPU_SFR_H


/*
 ***************************************
 *	RL78/L13�p(Renesas�������R���p�C��)
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




#define	M_NOP				__nop()												/* NOP���� */
#define	M_STOP				__stop()											/* STOP���� */
#define	M_HALT				__halt()											/* HALT���� */


#endif																			/* INCLUDED_CPU_SFR_H */
