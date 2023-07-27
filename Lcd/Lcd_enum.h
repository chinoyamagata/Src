/*
 *******************************************************************************
 *	File name	:	Lcd_enum.h
 *
 *	[内容]
 *		LCD表示処理用列挙型定義
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2017.12.04		Softex N.I		新規作成
 *******************************************************************************
 */
#ifndef	INCLUDED_LCD_ENUM_H
#define	INCLUDED_LCD_ENUM_H

/*
 *------------------------------------------------------------------------------
 *	LCDセグメント種類
 *------------------------------------------------------------------------------
 */
typedef enum ET_LcdSeg
{
	ecLcdSeg_S1 = 0U,
	ecLcdSeg_S2,
	ecLcdSeg_S3,
	ecLcdSeg_S4,
	ecLcdSeg_S5,
	ecLcdSeg_S6,
	ecLcdSeg_S7,
	ecLcdSeg_S8,
	ecLcdSeg_S9,
	ecLcdSeg_S10,
	ecLcdSeg_S11,
	ecLcdSeg_S12,
	ecLcdSeg_S13,
	ecLcdSeg_S14,
	ecLcdSeg_S15,
	ecLcdSeg_S16,
	ecLcdSeg_S17,
	ecLcdSeg_S18,
	ecLcdSeg_S19,
	ecLcdSeg_S20,
	ecLcdSeg_S21,
	ecLcdSeg_S22,
	ecLcdSeg_S23,
	ecLcdSeg_T1,
	ecLcdSeg_T2,
	ecLcdSeg_T3,
	ecLcdSeg_T4,
	ecLcdSeg_T5,
	ecLcdSeg_T6,
	ecLcdSeg_T7,
	ecLcdSeg_T8,
	ecLcdSeg_T9,
	ecLcdSeg_T10,
	ecLcdSeg_T11,
	ecLcdSeg_1A,
	ecLcdSeg_1B,
	ecLcdSeg_1C,
	ecLcdSeg_1D,
	ecLcdSeg_1E,
	ecLcdSeg_1F,
	ecLcdSeg_1G,
	ecLcdSeg_2A,
	ecLcdSeg_2B,
	ecLcdSeg_2C,
	ecLcdSeg_2D,
	ecLcdSeg_2E,
	ecLcdSeg_2F,
	ecLcdSeg_2G,
	ecLcdSeg_3A,
	ecLcdSeg_3B,
	ecLcdSeg_3C,
	ecLcdSeg_3D,
	ecLcdSeg_3E,
	ecLcdSeg_3F,
	ecLcdSeg_3G,
	ecLcdSeg_4A,
	ecLcdSeg_4B,
	ecLcdSeg_4C,
	ecLcdSeg_4D,
	ecLcdSeg_4E,
	ecLcdSeg_4F,
	ecLcdSeg_4G,
	ecLcdSeg_5A,
	ecLcdSeg_5B,
	ecLcdSeg_5C,
	ecLcdSeg_5D,
	ecLcdSeg_5E,
	ecLcdSeg_5F,
	ecLcdSeg_5G,
	ecLcdSeg_6A,
	ecLcdSeg_6B,
	ecLcdSeg_6C,
	ecLcdSeg_6D,
	ecLcdSeg_6E,
	ecLcdSeg_6F,
	ecLcdSeg_6G,

	ecLcdSegMax																	/* enum最大値 */
} ET_LcdSeg_t;


#endif
