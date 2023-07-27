/*
 *******************************************************************************
 *	File name	:	extern_def.h
 *
 *	[内容]
 *		グローバル変数・定数用 extern宣言の許可/禁止定義
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2017.11.08		Softex N.I		新規作成
 *******************************************************************************
 */
#ifndef	INCLUDED_EXTERN_DEF_H
#define	INCLUDED_EXTERN_DEF_H

#ifdef	idfGLOBAL_DEF

#define	M_GLOBAL					extern										/* extern定義 */

#else																			/* extern宣言禁止 */

#define	M_GLOBAL					/**/										/* extern定義なし */

#endif

#endif																			/* INCLUDED_EXTERN_DEF_H */
