/*
 *******************************************************************************
 *	File name	:	Lcd.c
 *
 *	[内容]
 *		LCD表示
 *------------------------------------------------------------------------------
 *	[変更日]		[変更者]		[概要]
 *	2017.12.04		Softex N.I		新規作成
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
#include "Lcd_enum.h"															/* LCD表示処理用列挙型定義 */


/*
 *==============================================================================
 *	define定義
 *==============================================================================
 */
#define imLcdDspDataNum		19U													/* LCD表示データ数 */
#define imLcd7SegKetaNum	3U													/* 7Seg表示桁数 */


/*
 *==============================================================================
 *	列挙型定義
 *==============================================================================
 */
typedef enum ET_TimeDispKind
{
	ecTimeDispKind_JCWA = 0,													/* 日本時計協会式表示 */
	ecTimeDispKind_EngUsa,														/* 英米式表示 */
	
	ecTimeDispKindMax															/* enum最大値 */
} ET_TimeDispKind_t;

/*
 *==============================================================================
 *	構造体定義
 *==============================================================================
 */
/* LCDセグメント情報テーブル用構造体 */
typedef struct ST_LcdTbl
{
	ET_LcdSeg_t			mLcdSeg;												/* LCDセグメント種類 */
	volatile uint8_t	*pmSegAdr;												/* セグメントレジスタのアドレス */
	uint8_t				mBit;													/* セグメントの対象ビット */
} ST_LcdTbl_t;

/* LCD表示データコード用構造体 */
typedef struct ST_LcdCode
{
	uint8_t			mData1;
	uint8_t			mData2;
} ST_LcdCode_t;


/*
 *==============================================================================
 *	変数定義
 *==============================================================================
 */
 
/* LCD表示データ */
static uint8_t	vLcdDspData[ imLcdDspDataNum ] =
	{	0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
		0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U
	};
#if (swDebugLcd == imEnable)
/* デバッグ用LCD表示カウンタ */
static uint8_t	vLcdDebugCnt = 0U;
#endif

/*
 *==============================================================================
 *	定数定義
 *==============================================================================
 */
/* LCDセグメント対応レジスタ */
static const volatile uint8_t	*pcLcdSegReg[ imLcdDspDataNum ] =
{
	&SEG33,																		/* LCDのSEG1 */
	&SEG34,																		/* LCDのSEG2 */
	&SEG35,																		/* LCDのSEG3 */
	&SEG36,																		/* LCDのSEG4 */
	&SEG37,																		/* LCDのSEG5 */
	&SEG38,																		/* LCDのSEG6 */
	&SEG39,																		/* LCDのSEG7 */
	&SEG40,																		/* LCDのSEG8 */
	&SEG43,																		/* LCDのSEG9 */
	&SEG45,																		/* LCDのSEG10 */
	&SEG20,																		/* LCDのSEG11 */
	&SEG19,																		/* LCDのSEG12 */
	&SEG18,																		/* LCDのSEG13 */
	&SEG12,																		/* LCDのSEG14 */
	&SEG8,																		/* LCDのSEG15 */
	&SEG3,																		/* LCDのSEG16 */
	&SEG2,																		/* LCDのSEG17 */
	&SEG1,																		/* LCDのSEG18 */
	&SEG0																		/* LCDのSEG19 */
};

/* LCDセグメント情報テーブル */
static const ST_LcdTbl_t	cLcdTbl[ ecLcdSegMax ] =
{
	{ ecLcdSeg_S1,		&SEG0,		M_BIT0	},		/* RF OFF */
	{ ecLcdSeg_S2,		&SEG0,		M_BIT1	},		/* ONLINE */
	{ ecLcdSeg_S3,		&SEG0,		M_BIT2	},		/* COLLECT */
	{ ecLcdSeg_S4,		&SEG0,		M_BIT3	},		/* EVENT */
	{ ecLcdSeg_S5,		&SEG20,		M_BIT0	},		/* 外付けサーミスタ */
	{ ecLcdSeg_S6,		&SEG20,		M_BIT2	},		/* MAX */
	{ ecLcdSeg_S7,		&SEG45,		M_BIT0	},		/* アラーム(上段) */
	{ ecLcdSeg_S8,		&SEG45,		M_BIT1	},		/* H(上段左) */
	{ ecLcdSeg_S9,		&SEG45,		M_BIT2	},		/* H(上段右) */
	{ ecLcdSeg_S10,		&SEG45,		M_BIT3	},		/* L(上段左) */
	{ ecLcdSeg_S11,		&SEG20,		M_BIT3	},		/* L(上段右) */
	{ ecLcdSeg_S12,		&SEG3,		M_BIT3	},		/* %(上段) */
	{ ecLcdSeg_S13,		&SEG33,		M_BIT3	},		/* ℃(上段) */
	{ ecLcdSeg_S14,		&SEG35,		M_BIT0	},		/* AM */
	{ ecLcdSeg_S15,		&SEG35,		M_BIT1	},		/* PM */
	{ ecLcdSeg_S16,		&SEG35,		M_BIT3	},		/* MIN */
	{ ecLcdSeg_S17,		&SEG34,		M_BIT3	},		/* アラーム(下段) */
	{ ecLcdSeg_S18,		&SEG34,		M_BIT2	},		/* H(下段左) */
	{ ecLcdSeg_S19,		&SEG34,		M_BIT1	},		/* H(下段右) */
	{ ecLcdSeg_S20,		&SEG34,		M_BIT0	},		/* L(下段左) */
	{ ecLcdSeg_S21,		&SEG33,		M_BIT0	},		/* L(下段右) */
	{ ecLcdSeg_S22,		&SEG33,		M_BIT1	},		/* %(下段) */
	{ ecLcdSeg_S23,		&SEG33,		M_BIT2	},		/* ℃(下段) */
	
	{ ecLcdSeg_T1,		&SEG20,		M_BIT1	},		/* -(上段) */
	{ ecLcdSeg_T2,		&SEG19,		M_BIT3	},		/* 1(上段) */
	{ ecLcdSeg_T3,		&SEG12,		M_BIT3	},		/* 小数点(上段) */
	{ ecLcdSeg_T4,		&SEG35,		M_BIT2	},		/* -(下段) */
	{ ecLcdSeg_T5,		&SEG36,		M_BIT3	},		/* 1(下段) */
	{ ecLcdSeg_T6,		&SEG38,		M_BIT3	},		/* col */
	{ ecLcdSeg_T7,		&SEG40,		M_BIT3	},		/* 小数点(下段) */
	
	{ ecLcdSeg_T8,		&SEG1,		M_BIT1	},		/* バッテリ左 */
	{ ecLcdSeg_T9,		&SEG1,		M_BIT0	},		/* バッテリ中 */
	{ ecLcdSeg_T10,		&SEG1,		M_BIT2	},		/* バッテリ右 */
	{ ecLcdSeg_T11,		&SEG1,		M_BIT3	},		/* バッテリ枠 */
	
	{ ecLcdSeg_1A,		&SEG18,		M_BIT0	},		/* 3桁目(上段) */
	{ ecLcdSeg_1B,		&SEG18,		M_BIT1	},
	{ ecLcdSeg_1C,		&SEG18,		M_BIT2	},
	{ ecLcdSeg_1D,		&SEG18,		M_BIT3	},
	{ ecLcdSeg_1E,		&SEG19,		M_BIT2	},
	{ ecLcdSeg_1F,		&SEG19,		M_BIT0	},
	{ ecLcdSeg_1G,		&SEG19,		M_BIT1	},
	
	{ ecLcdSeg_2A,		&SEG8,		M_BIT0	},		/* 2桁目(上段) */
	{ ecLcdSeg_2B,		&SEG8,		M_BIT1	},
	{ ecLcdSeg_2C,		&SEG8,		M_BIT2	},
	{ ecLcdSeg_2D,		&SEG8,		M_BIT3	},
	{ ecLcdSeg_2E,		&SEG12,		M_BIT2	},
	{ ecLcdSeg_2F,		&SEG12,		M_BIT0	},
	{ ecLcdSeg_2G,		&SEG12,		M_BIT1	},
	
	{ ecLcdSeg_3A,		&SEG2,		M_BIT0	},		/* 1桁目(上段) */
	{ ecLcdSeg_3B,		&SEG2,		M_BIT1	},
	{ ecLcdSeg_3C,		&SEG2,		M_BIT2	},
	{ ecLcdSeg_3D,		&SEG2,		M_BIT3	},
	{ ecLcdSeg_3E,		&SEG3,		M_BIT2	},
	{ ecLcdSeg_3F,		&SEG3,		M_BIT0	},
	{ ecLcdSeg_3G,		&SEG3,		M_BIT1	},
	
	{ ecLcdSeg_4A,		&SEG37,		M_BIT0	},		/* 3桁目(下段) */
	{ ecLcdSeg_4B,		&SEG37,		M_BIT1	},
	{ ecLcdSeg_4C,		&SEG37,		M_BIT2	},
	{ ecLcdSeg_4D,		&SEG37,		M_BIT3	},
	{ ecLcdSeg_4E,		&SEG36,		M_BIT2	},
	{ ecLcdSeg_4F,		&SEG36,		M_BIT0	},
	{ ecLcdSeg_4G,		&SEG36,		M_BIT1	},
	
	{ ecLcdSeg_5A,		&SEG39,		M_BIT0	},		/* 2桁目(下段) */
	{ ecLcdSeg_5B,		&SEG39,		M_BIT1	},
	{ ecLcdSeg_5C,		&SEG39,		M_BIT2	},
	{ ecLcdSeg_5D,		&SEG39,		M_BIT3	},
	{ ecLcdSeg_5E,		&SEG38,		M_BIT2	},
	{ ecLcdSeg_5F,		&SEG38,		M_BIT0	},
	{ ecLcdSeg_5G,		&SEG38,		M_BIT1	},
	
	{ ecLcdSeg_6A,		&SEG43,		M_BIT0	},		/* 1桁目(下段) */
	{ ecLcdSeg_6B,		&SEG43,		M_BIT1	},
	{ ecLcdSeg_6C,		&SEG43,		M_BIT2	},
	{ ecLcdSeg_6D,		&SEG43,		M_BIT3	},
	{ ecLcdSeg_6E,		&SEG40,		M_BIT2	},
	{ ecLcdSeg_6F,		&SEG40,		M_BIT0	},
	{ ecLcdSeg_6G,		&SEG40,		M_BIT1	}
};

/* 7seg用：数値コードテーブル */
static const ST_LcdCode_t	cSeg_Num[ 10U ] =
{
	/* mData1, mData2 */
	{ 0x05U, 0x0FU },					/* '0' */
	{ 0x00U, 0x06U },					/* '1' */
	{ 0x06U, 0x0BU },					/* '2' */
	{ 0x02U, 0x0FU },					/* '3' */
	{ 0x03U, 0x06U },					/* '4' */
	{ 0x03U, 0x0DU },					/* '5' */
	{ 0x07U, 0x0DU },					/* '6' */
	{ 0x00U, 0x07U },					/* '7' */
	{ 0x07U, 0x0FU },					/* '8' */
	{ 0x03U, 0x0FU }					/* '9' */
};

/* 7seg用：文字コードテーブル（A〜Z） */
static const ST_LcdCode_t	cSeg_AtoZ[ 26U ] =
{
	/* mData1, mData2 */
	{ 0x07U, 0x07U },					/* 'A' */
	{ 0x07U, 0x0CU },					/* 'B(b)' */
	{ 0x05U, 0x09U },					/* 'C' */
	{ 0x06U, 0x0EU },					/* 'D(d)' */
	{ 0x07U, 0x09U },					/* 'E' */
	{ 0x07U, 0x01U },					/* 'F' */
	{ 0x05U, 0x0DU },					/* 'G' */
	{ 0x07U, 0x06U },					/* 'H' */
	{ 0x05U, 0x00U },					/* 'I' */
	{ 0x04U, 0x0EU },					/* 'J' */
	{ 0x07U, 0x05U },					/* 'K' */
	{ 0x05U, 0x08U },					/* 'L' */
	{ 0x06U, 0x05U },					/* 'M' */
//	{ 0x06U, 0x04U },					/* 'n' */
	{ 0x05U, 0x07U },					/* 'N' */
	{ 0x05U, 0x0FU },					/* 'O' */
	{ 0x07U, 0x03U },					/* 'P' */
	{ 0x03U, 0x07U },					/* 'Q(q)' */
	{ 0x06U, 0x00U },					/* 'R(r)' */
	{ 0x03U, 0x0DU },					/* 'S' */
	{ 0x07U, 0x08U },					/* 'T(t)' */
	{ 0x05U, 0x0EU },					/* 'U' */
	{ 0x07U, 0x0EU },					/* 'V' */
	{ 0x03U, 0x0AU },					/* 'W' */
	{ 0x03U, 0x04U },					/* 'X' */
	{ 0x03U, 0x0EU },					/* 'Y(y)' */
	{ 0x02U, 0x09U }					/* 'Z' */
};

/* 7seg用：文字コードテーブル_電圧パルス単位表示（c,n,v） */
static const ST_LcdCode_t	cSeg_cnv_VP[ 5U ] =
{
	/* mData1, mData2 */
	{ 0x06U, 0x08U },					/* 'c' */
	{ 0x06U, 0x04U },					/* 'n' */
	{ 0x04U, 0x0CU },					/* 'v'(u) */
	{ 0x07U, 0x0EU },					/* 'v'(Aの逆さま) */
	{ 0x04U, 0x08U },					/* 'v'(|＿) */
};

static const uint8_t	cLcdDatIdxUpp[ imLcd7SegKetaNum ] = { 11U, 13U, 15U };	/* 上段7Seg表示用データ格納先インデックス */
static const uint8_t	cLcdDatIdxLow[ imLcd7SegKetaNum ] = { 3U, 5U, 7U };		/* 下段7Seg表示用データ格納先インデックス */


/*
 *==============================================================================
 *	プロトタイプ宣言
 *==============================================================================
 */



#pragma section text MY_APP2
/*
 *******************************************************************************
 *	LCD表示初期化処理
 *
 *	[内容]
 *		LCD表示の初期化処理を行う。
 *******************************************************************************
 */
void ApiLcd_Initial( void )
{
	R_LCD_Voltage_On();							/* 容量分割回路動作許可 */
	R_LCD_Start();								/* LCDコントローラ／ドライバ動作開始 */
	
	/* Ver.表示 */
	ApiHmi_Main();
	ApiLcd_Main();
}



/*
 *******************************************************************************
 *	LCD表示メイン処理
 *
 *	[内容]
 *		LCDドライバーへ表示データを送信する。
 *******************************************************************************
 */
void ApiLcd_Main( void )
{
	uint8_t				wkLoop;
	volatile uint8_t	*pwkSegReg;
	
	for( wkLoop = 0U; wkLoop < imLcdDspDataNum; wkLoop++ )
	{
		pwkSegReg = (volatile uint8_t*)pcLcdSegReg[wkLoop];
		*pwkSegReg = vLcdDspData[wkLoop];
	}
}


/*
 *******************************************************************************
 *	セグメント表示
 *
 *	[引数]
 *		ET_LcdSeg_t	arLcdSeg	：対象のセグメント種類
 *		uint8_t		arOnOff		：点灯(imON)／消灯(imOFF)
 *	[内容]
 *		個別セグメントの、点灯、消灯を行う。
 *******************************************************************************
 */
void ApiLcd_SegDsp( ET_LcdSeg_t arLcdSeg, uint8_t arOnOff )
{
	uint8_t	wkLoop;
	
	if( arLcdSeg < ecLcdSegMax )
	{
		for( wkLoop = 0U ; wkLoop < imLcdDspDataNum ; wkLoop++ )
		{
			if( pcLcdSegReg[wkLoop] == cLcdTbl[arLcdSeg].pmSegAdr )
			{
				break;
			}
		}
		
		if( arOnOff == imON )
		{
			vLcdDspData[wkLoop] |= cLcdTbl[arLcdSeg].mBit;				/* 点灯 */
		}
		else
		{
			vLcdDspData[wkLoop] &= ~(cLcdTbl[arLcdSeg].mBit);			/* 消灯 */
		}
	}
}


/*
 *******************************************************************************
 *	上段側7Seg数値表示
 *
 *	[引数]
 *		sint16_t	arNum	：数値(-1999〜1999)
 *		uint8_t		arDp1	：小数点1 ON／OFF
 *		uint8_t		arDp2	：小数点2 ON／OFF
 *		uint8_t		minus1	：マイナス判定 ON／OFF
 *	[内容]
 *		上段側の7Segに数値を表示する
 *		電圧モデルは一の位〜小数点第一位を2桁で表示する(例：0.019の内の0.0)
 *		パルスモデルは最大3桁を表示する(例：7059の内の705,0010では10)
 *******************************************************************************
 */
void ApiLcd_Upp7SegNumDsp( sint16_t arNum, uint8_t arDp1, uint8_t arDp2, uint8_t minus1 )
{
	uint16_t	wkNum;
	uint8_t		wkKeta1000;
	uint8_t		wkKeta100;
	uint8_t		wkKeta10;
	uint8_t		wkKeta1;
	uint8_t		wkMinusFlg;
	
	/* 最大値/最小値チェック */
	if( (arNum <= 1999) && (arNum >= -1999) )
	{
		/*
		 ***************************************
		 *	マイナス判定
		 ***************************************
		 */
		if( arNum < 0 || minus1 == imON )
		{
			wkNum = (uint16_t)((-1) * arNum);
			wkMinusFlg = imON;
		}
		else
		{
			wkNum = (uint16_t)arNum;
			wkMinusFlg = imOFF;
		}
		
		/*
		 ***************************************
		 *	各桁の数値算出
		 ***************************************
		 */
		wkKeta1000 = (uint8_t)(wkNum / 1000U);
		wkNum %= 1000U;
		wkKeta100 = (uint8_t)(wkNum / 100U);
		wkNum %= 100U;
		wkKeta10 = (uint8_t)(wkNum / 10U);
		wkKeta1 = (uint8_t)(wkNum % 10U);
		
		/*
		 ***************************************
		 *	数値表示
		 ***************************************
		 */
		
		/* 100の位 */
		if( (wkKeta1000 > 0U)												/* 1000の位が0でない */
		||	(wkKeta100 > 0U)												/* 100の位が0ではない */
		&& (gvInFlash.mProcess.mModelCode != ecSensType_V)			/* かつ電圧モデルではないとき */
		)
		{
			vLcdDspData[ cLcdDatIdxUpp[0] ] = cSeg_Num[wkKeta100].mData1;
			vLcdDspData[ cLcdDatIdxUpp[0] + 1U ] = cSeg_Num[wkKeta100].mData2;
		}
		else
		{
			/* 非表示 */
			vLcdDspData[ cLcdDatIdxUpp[0] ] = 0x00U;
			vLcdDspData[ cLcdDatIdxUpp[0] + 1U ] = 0x00U;
		}
		
		/* 10の位 */
		if( arDp2 == imON )											/* 小数点2あり 電圧モデルも対象*/
		{
			vLcdDspData[ cLcdDatIdxUpp[1] ] = cSeg_Num[wkKeta10].mData1;
			vLcdDspData[ cLcdDatIdxUpp[1] + 1U ] = cSeg_Num[wkKeta10].mData2;
		}
		else																/* 小数点なし */
		{
			if( (wkKeta1000 > 0U)											/* 1000の位が0でない */
			||	(wkKeta100 > 0U)											/* 100の位が0でない */
			||	(wkKeta10 > 0U)												/* 10の位が0ではない */
//			&&	(gvInFlash.mProcess.mModelCode != ecSensType_Pulse)	/* かつパルスモデルではないとき */
			)
			{
				vLcdDspData[ cLcdDatIdxUpp[1] ] = cSeg_Num[wkKeta10].mData1;
				vLcdDspData[ cLcdDatIdxUpp[1] + 1U ] = cSeg_Num[wkKeta10].mData2;
			}
			else
			{
				/* 非表示 */
				vLcdDspData[ cLcdDatIdxUpp[1] ] = 0x00U;
				vLcdDspData[ cLcdDatIdxUpp[1] + 1U ] = 0x00U;
			}
		}
		
		/* 1の位：「パルスモデルで下段1桁のみの場合以外」は全て表示させる　上段全ての桁が0でパルスモデルのときfalse */
#if 0
		if( (wkKeta1000 > 0U)										/* 1000の位が0でない */
		||	(wkKeta100 > 0U)										/* 100の位が0でない */
		||	(wkKeta10 > 0U)										/* 10の位が0ではない */
		||	(wkKeta1 > 0U)										/* 1の位が0ではない */
		||	(gvInFlash.mProcess.mModelCode != ecSensType_Pulse)						/* パルスモデルではないとき */
		)
		{
		
			vLcdDspData[ cLcdDatIdxUpp[2] ] = cSeg_Num[wkKeta1].mData1;
			vLcdDspData[ cLcdDatIdxUpp[2] + 1U ] = cSeg_Num[wkKeta1].mData2;
		}
		else
		{
			/* 非表示：パルスモデルで1桁のみの場合があるため */
			vLcdDspData[ cLcdDatIdxUpp[1] ] = 0x00U;
			vLcdDspData[ cLcdDatIdxUpp[1] + 1U ] = 0x00U;
		}
#endif
		/* 1の位：上段は常に表示 */
		vLcdDspData[ cLcdDatIdxUpp[2] ] = cSeg_Num[wkKeta1].mData1;
		vLcdDspData[ cLcdDatIdxUpp[2] + 1U ] = cSeg_Num[wkKeta1].mData2;
		
		/* 1000の位 */
		if( wkKeta1000 == 1U )												/* 1000の位が1 */
		{
			ApiLcd_SegDsp(ecLcdSeg_T2, imON);
		}
		else																/* 1000の位が0 */
		{
			ApiLcd_SegDsp(ecLcdSeg_T2, imOFF);
		}
		/*
		 ***************************************
		 *	マイナス符号表示
		 ***************************************
		 */
		if( wkMinusFlg == imON )											/* 負の値 */
		{
			/* 最大桁を算出 */
			if( wkKeta1000 == 1U )											/* 1000の位が1 */
			{
				ApiLcd_SegDsp(ecLcdSeg_T1, imON);							/* 1000の位にマイナス符号設定 */
			}
			else if( wkKeta100 > 0U )										/* 100の位が0でない */
			{
				ApiLcd_SegDsp(ecLcdSeg_T1, imON);							/* 1000の位にマイナス符号設定 */
			}
			else if( wkKeta10 > 0U )										/* 10の位が0でない */
			{
				ApiLcd_SegDsp(ecLcdSeg_1G, imON);							/* 100の位にマイナス符号設定 */
			}
			else
			{
				/* 小数点有り時は最大桁を変更 */
				if( arDp2 == imON )											/* 小数点2ON */
				{
					ApiLcd_SegDsp(ecLcdSeg_1G, imON);						/* 100の位にマイナス符号設定 */
				}
				else
				{
					ApiLcd_SegDsp(ecLcdSeg_2G, imON);						/* 10の位にマイナス符号設定 */
				}
			}
		}
		else
		{
			/* 1000の位は個別にOFF */
			ApiLcd_SegDsp(ecLcdSeg_T1, imOFF);								/* 1000の位にマイナス符号設定 */
		}
		
		/*
		 ***************************************
		 *	小数点表示
		 ***************************************
		 */
		ApiLcd_SegDsp(ecLcdSeg_T3, arDp2);
	}
}


/*
 *******************************************************************************
 *	下段側7Seg数値表示
 *
 *	[引数]
 *		sint16_t	arNum	：数値(-1999〜1999)
 *		uint8_t		arDp	：小数点 ON／OFF
 *	[内容]
 *		下段側の7Segに数値を表示する
 *		電圧モデルは小数点第二〜三位を表示する(例：0.019の内の19)
 *		パルスモデルは千の位があるとき、一の位を表示する(例：7059の内の9,0100では数値は非表示,0001の内の1)
 *******************************************************************************
 */
void ApiLcd_Low7SegNumDsp( sint16_t arNum, uint8_t arDp )
{
	uint16_t	wkNum;
	uint8_t		wkKeta1000;
	uint8_t		wkKeta100;
	uint8_t		wkKeta10;
	uint8_t		wkKeta1;
	uint8_t		wkMinusFlg;
	
	/* 最大値/最小値チェック */
	if( (arNum <= 1999) && (arNum >= -1999) )
	{
		/*
		 ***************************************
		 *	マイナス判定
		 ***************************************
		 */
		/* マイナス記号 */
		if( arNum < 0 )
		{
			wkNum = (uint16_t)((-1) * arNum);
			wkMinusFlg = imON;
		}
		else
		{
			wkNum = (uint16_t)arNum;
			wkMinusFlg = imOFF;
		}
		
		/*
		 ***************************************
		 *	各桁の数値算出
		 ***************************************
		 */
		wkKeta1000 = (uint8_t)(wkNum / 1000U);
		wkNum %= 1000U;
		wkKeta100 = (uint8_t)(wkNum / 100U);
		wkNum %= 100U;
		wkKeta10 = (uint8_t)(wkNum / 10U);
		wkKeta1 = (uint8_t)(wkNum % 10U);
		
		/*
		 ***************************************
		 *	数値表示
		 ***************************************
		 */
		
		/* 100の位 */
		if ((wkKeta1000 > 0U)						/* 1000の位が0ではない */
		||	(wkKeta100 > 0U)												/* 100の位が0ではない */
		)
		{
			vLcdDspData[ cLcdDatIdxLow[ 0U ] ] = cSeg_Num[wkKeta100].mData1;
			vLcdDspData[ cLcdDatIdxLow[ 0U ] + 1U ] = cSeg_Num[wkKeta100].mData2;
		}
		else
		{
			/* 非表示 */
			vLcdDspData[ cLcdDatIdxLow[ 0U ] ] = 0x00U;
			vLcdDspData[ cLcdDatIdxLow[ 0U ] + 1U ] = 0x00U;
		}
		
		/* 10の位 */
		if (arDp == imON)													/* 小数点あり */
		{
			vLcdDspData[ cLcdDatIdxLow[ 1U ] ] = cSeg_Num[wkKeta10].mData1;
			vLcdDspData[ cLcdDatIdxLow[ 1U ] + 1U ] = cSeg_Num[wkKeta10].mData2;
		}
		else																/* 小数点なし */
		{
			if ((wkKeta1000 > 0U)											/* 1000の位が0でない */
			||	(wkKeta100 > 0U)											/* 100の位が0でない */
			||	(wkKeta10 > 0U)												/* 10の位が0ではない */
			)
			{
				vLcdDspData[ cLcdDatIdxLow[ 1U ] ] = cSeg_Num[wkKeta10].mData1;
				vLcdDspData[ cLcdDatIdxLow[ 1U ] + 1U ] = cSeg_Num[wkKeta10].mData2;
			}
			else
			{
				/* 非表示 */
				vLcdDspData[ cLcdDatIdxLow[ 1U ] ] = 0x00U;
				vLcdDspData[ cLcdDatIdxLow[ 1U ] + 1U ] = 0x00U;
			}
		}
		
		/* 1の位 */
		vLcdDspData[ cLcdDatIdxLow[ 2U ] ] = cSeg_Num[wkKeta1].mData1;
		vLcdDspData[ cLcdDatIdxLow[ 2U ] + 1U ] = cSeg_Num[wkKeta1].mData2;
		
		/* 1000の位 */
		if( wkKeta1000 == 1U )												/* 1000の位が1 */
		{
			ApiLcd_SegDsp(ecLcdSeg_T5, imON);
		}
		else																/* 1000の位が0 */
		{
			ApiLcd_SegDsp(ecLcdSeg_T5, imOFF);
		}
		
		/*
		 ***************************************
		 *	マイナス符号表示
		 ***************************************
		 */
		if( wkMinusFlg == imON )											/* 負の値 */
		{
			/* 最大桁を算出 */
			if( wkKeta1000 == 1U )											/* 1000の位が1 */
			{
				ApiLcd_SegDsp(ecLcdSeg_T4, imON);							/* 1000の位にマイナス符号設定 */
			}
			else if( wkKeta100 > 0U )										/* 100の位が0でない */
			{
				ApiLcd_SegDsp(ecLcdSeg_T4, imON);							/* 1000の位にマイナス符号設定 */
			}
			else if( wkKeta10 > 0U )										/* 10の位が0でない */
			{
				ApiLcd_SegDsp(ecLcdSeg_4G, imON);							/* 100の位にマイナス符号設定 */
			}
			else
			{
				/* 小数点有り時は最大桁を変更 */
				if( arDp == imON )											/* 小数点ON */
				{
					ApiLcd_SegDsp(ecLcdSeg_4G, imON);						/* 100の位にマイナス符号設定 */
				}
				else
				{
					ApiLcd_SegDsp(ecLcdSeg_5G, imON);						/* 10の位にマイナス符号設定 */
				}
			}
		}
		
		/*
		 ***************************************
		 *	小数点表示
		 ***************************************
		 */
		ApiLcd_SegDsp(ecLcdSeg_T7, arDp);
	}
}

/*
 *******************************************************************************
 *	下段側7Seg数値文字混合表示(電圧パルス)
 *
 *	[引数]
 *		sint16_t	arNum	：数値(-1999〜1999)
 *		uint8_t		arflg	：千の位を検知するフラグ
 *		
 *	[内容]
 *		下段側の7Segに数値を表示する
 *		電圧モデルは小数点第二〜三位とvを表示する(例：0.019vの内の19v)
 *		パルスモデルは千の位があるとき、一の位とcnを表示する(例：7059cnの内の9cn,0101や0001では数値は非表示でcnのみ)
 *******************************************************************************
 */
void ApiLcd_Low7SegVPDsp( sint16_t arNum, uint8_t arflg )
{
	uint8_t		wkData1;
	uint8_t		wkData2;
	uint8_t		wkData3;
	uint8_t		wkData4;
	uint16_t	wkNum;
	uint8_t		wkKeta100;
	uint8_t		wkKeta10;
	
	/* 最大値/最小値チェック */
	if( (arNum <= 1999) && (arNum >= -1999) )
	{
		/*
		 ***************************************
		 *	マイナス判定
		 ***************************************
		 */
		/* マイナス記号 */
		if( arNum < 0 )
		{
			wkNum = (uint16_t)((-1) * arNum);
		}
		else
		{
			wkNum = (uint16_t)arNum;
		}
		
		/*
		 ***************************************
		 *	各桁の数値算出
		 ***************************************
		 */
		// wkNum %= 1000U;
		wkKeta100 = (uint8_t)(wkNum / 100U);		/* 7segの3桁目の値取得 */
		if ( gvInFlash.mProcess.mModelCode == ecSensType_V )	/* 電圧モデルのとき、十の位を3桁目に表示（一の位を2桁目、表示1桁目はv） */
		{
			wkNum %= 100U;
		}
		else							/* パルスモデルのとき、一の位を3桁目に表示（1,2桁目はcn） */
		{
			wkNum %= 10U;
		}
		wkKeta10 = (uint8_t)(wkNum / 10U);		/* 7segの2桁目の値取得（電圧モデルのとき、一の位を2桁目に表示） */
		
		/* 下段の7Segデータを一旦クリア */
		vLcdDspData[cLcdDatIdxLow[ 0U ]] = 0x00U;
		vLcdDspData[cLcdDatIdxLow[ 0U ] + 1U] = 0x00U;
		vLcdDspData[cLcdDatIdxLow[ 1U ]] = 0x00U;
		vLcdDspData[cLcdDatIdxLow[ 1U ] + 1U] = 0x00U;
		vLcdDspData[cLcdDatIdxLow[ 2U ]] = 0x00U;
		vLcdDspData[cLcdDatIdxLow[ 2U ] + 1U] = 0x00U;
		
		/*
		 ***************************************
		 *	数値文字表示
		 ***************************************
		 */
		
		/* 100の位 */
		if (( gvInFlash.mProcess.mModelCode == ecSensType_V )							/* 電圧モデルのとき */
		||	( arflg == imON ) && ( gvInFlash.mProcess.mModelCode == ecSensType_Pulse )	/* パルス測定値の千の位が0以上のとき */
		)
		{
			vLcdDspData[ cLcdDatIdxLow[ 0U ] ] = cSeg_Num[wkKeta100].mData1;
			vLcdDspData[ cLcdDatIdxLow[ 0U ] + 1U ] = cSeg_Num[wkKeta100].mData2;
		}
		else
		{
			/* 非表示 */
			vLcdDspData[ cLcdDatIdxLow[ 0U ] ] = 0x00U;
			vLcdDspData[ cLcdDatIdxLow[ 0U ] + 1U ] = 0x00U;
		}
		
		/* 10の位 */
		if ( gvInFlash.mProcess.mModelCode == ecSensType_V )		/* 電圧モデルのとき */
		{
			vLcdDspData[ cLcdDatIdxLow[ 1U ] ] = cSeg_Num[wkKeta10].mData1;
			vLcdDspData[ cLcdDatIdxLow[ 1U ] + 1U ] = cSeg_Num[wkKeta10].mData2;
		}
		else														/* パルスモデルのとき 単位cn：c */
		{
			// arTextTbl[wkLoop] = 'c';
			//wkIndex1 = (uint8_t)wkText1 - (uint8_t)'c';
			wkData1 = cSeg_cnv_VP[0].mData1;
			wkData2 = cSeg_cnv_VP[0].mData2;
			
			vLcdDspData[ cLcdDatIdxLow[ 1U ] ] = wkData1;
			vLcdDspData[ cLcdDatIdxLow[ 1U ] + 1U ] = wkData2;
		}
		
		/* 1の位 */
		if( gvInFlash.mProcess.mModelCode == ecSensType_V )			/* 電圧モデルのとき 単位v */
		{
			// arTextTbl[wkLoop] = 'v';
			// wkIndex2 = (uint8_t)wkText2 - (uint8_t)'c';
			
			/*cSeg_cnv_VP[2]*/
#if 1		/* v(u) */
			wkData3 = cSeg_cnv_VP[2].mData1;
			wkData4 = cSeg_cnv_VP[2].mData2;
#else
			#if 0	/* v(Aの逆さま) */
				wkData3 = cSeg_cnv_VP[3].mData1;
				wkData4 = cSeg_cnv_VP[3].mData2;
			#else	/* v(|_) */
				wkData3 = cSeg_cnv_VP[4].mData1;
				wkData4 = cSeg_cnv_VP[4].mData2;
			#endif
#endif
		}
		else														/* パルスモデルのとき 単位cn：n */
		{
			// arTextTbl[wkLoop] = 'n';
			// wkIndex2 = (uint8_t)wkText2 - (uint8_t)'c';
			wkData3 = cSeg_cnv_VP[1].mData1;
			wkData4 = cSeg_cnv_VP[1].mData2;
		}
		vLcdDspData[ cLcdDatIdxLow[ 2U ] ] = wkData3;
		vLcdDspData[ cLcdDatIdxLow[ 2U ] + 1U ] = wkData4;
		
		/* 1000の位 */
		ApiLcd_SegDsp(ecLcdSeg_T5, imOFF);							/* 1000の位が0 */
		
	}
}

/*
 *******************************************************************************
 *	上段/下段7Seg文字表示
 *
 *	[引数]
 *		char_t	arTextTbl	：表示文字列格納テーブル
 *		uint8_t	arSelect	：imHigh…上段表示、imLow…下段表示
 *	[内容]
 *		上段/下段の7Seg(最上位桁は除く)に文字を表示する。
 *******************************************************************************
 */
void ApiLcd_UppLow7SegDsp( const char_t arTextTbl[], uint8_t arSelect )
{
	uint8_t		wkLoop;
	char_t		wkText;
	uint8_t		wkIndex;
	uint8_t		wkData1;
	uint8_t		wkData2;
	uint8_t		wkDatIdx;
	
	if( arTextTbl != M_NULL )
	{
		/* 上段/下段の7Segデータを一旦クリア */
		for( wkLoop = 0U; wkLoop < imLcd7SegKetaNum; wkLoop++ )
		{
			if( arSelect == imHigh )
			{
				wkDatIdx = cLcdDatIdxUpp[wkLoop];
			}
			else
			{
				wkDatIdx = cLcdDatIdxLow[wkLoop];
			}
			vLcdDspData[wkDatIdx] = 0x00U;
			vLcdDspData[wkDatIdx + 1U] = 0x00U;
		}
		
		for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
		{
			wkText = arTextTbl[wkLoop];
			if ((wkText >= '0') && (wkText <= '9'))								/* 数値 */
			{
				wkIndex = (uint8_t)wkText - (uint8_t)'0';
				wkData1 = cSeg_Num[wkIndex].mData1;
				wkData2 = cSeg_Num[wkIndex].mData2;
			}
			else if ((wkText >= 'A') && (wkText <= 'Z'))						/* 大文字 */
			{
				wkIndex = (uint8_t)wkText - (uint8_t)'A';
				wkData1 = cSeg_AtoZ[wkIndex].mData1;
				wkData2 = cSeg_AtoZ[wkIndex].mData2;
			}
			else if ((wkText >= 'a') && (wkText <= 'z'))						/* 小文字 */
			{
				wkIndex = (uint8_t)wkText - (uint8_t)'a';
				wkData1 = cSeg_AtoZ[wkIndex].mData1;
				wkData2 = cSeg_AtoZ[wkIndex].mData2;
			}
			else if (wkText == '-')												/* ハイフン */
			{
				wkData1 = 0x02U;
				wkData2 = 0x00U;
			}
			else
			{
				/* スペース(非表示) */
				wkData1 = 0x00U;
				wkData2 = 0x00U;
			}
			
			if( arSelect == imHigh )
			{
				wkDatIdx = cLcdDatIdxUpp[wkLoop];
			}
			else
			{
				wkDatIdx = cLcdDatIdxLow[wkLoop];
			}
			vLcdDspData[wkDatIdx] = wkData1;
			vLcdDspData[wkDatIdx + 1U] = wkData2;
		}
	}
}


/*
 *******************************************************************************
 *	時刻表示
 *
 *	[引数]
 *		uint8_t	arHour		：時（24時間制）(0〜23)
 *		uint8_t	arMinute	：分(0〜59)
 *	[内容]
 *		下段側に12時間制で時刻（時、分）を表示する。
 *******************************************************************************
 */
void ApiLcd_TimeDsp( uint8_t arHour, uint8_t arMinute )
{
	uint16_t	wkHour;
	uint16_t	wkNum;
	uint8_t		wkKeta1000;
	uint8_t		wkKeta100;
	uint8_t		wkKeta10;
	uint8_t		wkKeta1;
	
	
	if( (arHour < 24U) && (arMinute < 60U) )
	{
		
		/* 00:00、12:00 */
		if( (arHour == 0U) || (arHour == 12U) )
		{
			if( ecTimeDispKind_JCWA == (ET_TimeDispKind_t)gvInFlash.mParam.mTimDispKind )
			{
				wkHour = 0U;													/* AM/PM 0:00 */
			}
			else
			{
				wkHour = 12U;													/* AM/PM 12:00 */
			}
		}
		else if( arHour > 12U )													/* 13時以降 */
		{
			wkHour = arHour - 12U;
		}
		else
		{
			wkHour = arHour;
		}
		
		wkNum = (wkHour * 100U) + arMinute;										/* 時刻を数値表示に変換 */
		
		wkKeta1000 = (uint8_t)(wkNum / 1000U);
		wkNum %= 1000U;
		wkKeta100 = (uint8_t)(wkNum / 100U);
		wkNum %= 100U;
		wkKeta10 = (uint8_t)(wkNum / 10U);
		wkKeta1 = (uint8_t)(wkNum % 10U);
		
		vLcdDspData[ cLcdDatIdxLow[0] ] = cSeg_Num[wkKeta100].mData1;
		vLcdDspData[ cLcdDatIdxLow[0] + 1U ] = cSeg_Num[wkKeta100].mData2;
		
		vLcdDspData[ cLcdDatIdxLow[1] ] = cSeg_Num[wkKeta10].mData1;
		vLcdDspData[ cLcdDatIdxLow[1] + 1U ] = cSeg_Num[wkKeta10].mData2;
		
		vLcdDspData[ cLcdDatIdxLow[2] ] = cSeg_Num[wkKeta1].mData1;
		vLcdDspData[ cLcdDatIdxLow[2] + 1U ] = cSeg_Num[wkKeta1].mData2;
		
		if (wkKeta1000 == 1U)													/* 1000の位が1 */
		{
			ApiLcd_SegDsp(ecLcdSeg_T5, imON);
		}
		else																	/* 1000の位が0 */
		{
			ApiLcd_SegDsp(ecLcdSeg_T5, imOFF);
		}
		
		if( arHour < 12U )
		{
			ApiLcd_SegDsp(ecLcdSeg_S14, imON);									/* AM表示 */
		}
		else
		{
			ApiLcd_SegDsp(ecLcdSeg_S15, imON);									/* PM表示 */
		}
		ApiLcd_SegDsp(ecLcdSeg_T6, imON);										/* コロン表示 */
	}
	else																		/* 引数異常 */
	{
		/* --:--を表示 */
		ApiLcd_SegDsp(ecLcdSeg_T4, imON);										/* 1000の位にハイフン表示 */
		ApiLcd_SegDsp(ecLcdSeg_4G, imON);										/* 100の位にハイフン表示 */
		ApiLcd_SegDsp(ecLcdSeg_5G, imON);										/* 10の位にハイフン表示 */
		ApiLcd_SegDsp(ecLcdSeg_6G, imON);										/* 1の位にハイフン表示 */
		ApiLcd_SegDsp(ecLcdSeg_T6, imON);										/* コロン表示 */
	}
}


/*
 *******************************************************************************
 *	全点灯/全消灯/奇数セグメント点灯/偶数セグメント点灯処理
 *
 *	[引数]
 *		ET_DispSegSelect_t arSelect：セグメント選択
 *	[戻値]
 *		なし
 *	[内容]
 *		全点灯/全消灯/奇数セグメント点灯/偶数セグメント点灯を行う
 *******************************************************************************
 */
void ApiLcd_SegSelectOnOff( ET_DispSegSelect_t arSelect )
{
	uint8_t		wkLoop;
	
	for( wkLoop = 0U ; wkLoop < M_ArrayElement(vLcdDspData) ; wkLoop++ )
	{
		switch( arSelect )
		{
			case ecDispSegSelect_SegAllOn:
				vLcdDspData[ wkLoop ] = 0x0FU;
				break;
			case ecDispSegSelect_SegAllOff:
				vLcdDspData[ wkLoop ] = 0x00U;
				break;
			case ecDispSegSelect_SegOddOn:
				if( wkLoop % 2U )
				{
					vLcdDspData[ wkLoop ] = 0x0FU;
				}
				else
				{
					vLcdDspData[ wkLoop ] = 0x00U;
				}
				break;
			case ecDispSegSelect_SegEvenOn:
				if( wkLoop % 2U )
				{
					vLcdDspData[ wkLoop ] = 0x00U;
				}
				else
				{
					vLcdDspData[ wkLoop ] = 0x0FU;
				}
				break;
		}
	}
}



/*
 *******************************************************************************
 *	上段側オーバーフロー/アンダーフロー文字表示
 *
 *	[引数]
 *		uint8_t arSelect：選択
 *	[戻値]
 *		なし
 *	[内容]
 *		上段側の7Segにオーバーフロー/アンダーフローを表示する。
 *******************************************************************************
 */
void ApiLcd_Upp7SegOvrUdrDsp( uint8_t arSelect )
{
	uint8_t		wkLoop;
	
	/* 上段の7Segデータを一旦クリア */
	for( wkLoop = 0U; wkLoop < imLcd7SegKetaNum; wkLoop++ )
	{
		if( cLcdDatIdxUpp[wkLoop] == 10U )
		{
			vLcdDspData[ cLcdDatIdxUpp[wkLoop] ] = 0x00U;
		}
		else
		{
			vLcdDspData[ cLcdDatIdxUpp[wkLoop] ] = 0x00U;
			vLcdDspData[ cLcdDatIdxUpp[wkLoop] + 1U ] = 0x00U;
		}
	}
	
	switch( arSelect )
	{
		case imUnder:
			ApiLcd_SegDsp(ecLcdSeg_1D, imON);					/* 上段3桁目「＿」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_2D, imON);					/* 上段2桁目「＿」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_3D, imON);					/* 上段1桁目「＿」点灯 */
			break;
		case imOver:
			ApiLcd_SegDsp(ecLcdSeg_1A, imON);					/* 上段3桁目「￣」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_2A, imON);					/* 上段2桁目「￣」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_3A, imON);					/* 上段1桁目「￣」点灯 */
			break;
		case imSnsrErr:
		case imBar:
			ApiLcd_SegDsp(ecLcdSeg_1G, imON);					/* 上段3桁目「---」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_2G, imON);					/* 上段2桁目「---」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_3G, imON);					/* 上段1桁目「---」点灯 */
			break;
	}
}

#if (swSensorCom == imEnable)
/*
 *******************************************************************************
 *	下段側オーバーフロー/アンダーフロー文字表示
 *
 *	[引数]
 *		uint8_t arSelect：選択
 *	[戻値]
 *		なし
 *	[内容]
 *		下段側の7Segにオーバーフロー/アンダーフローを表示する。
 *******************************************************************************
 */
void ApiLcd_Low7SegOvrUdrDsp( uint8_t arSelect )
{
	uint8_t		wkLoop;
	
	/* 下段の7Segデータを一旦クリア */
	for( wkLoop = 0U; wkLoop < imLcd7SegKetaNum; wkLoop++ )
	{
		if( cLcdDatIdxLow[wkLoop] == 10U )
		{
			vLcdDspData[ cLcdDatIdxLow[wkLoop] ] = 0x00U;
		}
		else
		{
			vLcdDspData[ cLcdDatIdxLow[wkLoop] ] = 0x00U;
			vLcdDspData[ cLcdDatIdxLow[wkLoop] + 1U ] = 0x00U;
		}
	}
	
	switch( arSelect )
	{
		case imUnder:
			ApiLcd_SegDsp(ecLcdSeg_4D, imON);					/* 下段3桁目「＿」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_5D, imON);					/* 下段2桁目「＿」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_6D, imON);					/* 下段1桁目「＿」点灯 */
			break;
		case imOver:
			ApiLcd_SegDsp(ecLcdSeg_4A, imON);					/* 下段3桁目「￣」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_5A, imON);					/* 下段2桁目「￣」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_6A, imON);					/* 下段1桁目「￣」点灯 */
			break;
		case imSnsrErr:
		case imBar:
			ApiLcd_SegDsp(ecLcdSeg_4G, imON);					/* 下段3桁目「---」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_5G, imON);					/* 下段2桁目「---」点灯 */
			ApiLcd_SegDsp(ecLcdSeg_6G, imON);					/* 下段1桁目「---」点灯 */
			break;
	}
}
#endif

/*
 *******************************************************************************
 *	LCDドライバ停止処理
 *
 *	[内容]
 *		LCDドライバの停止を行う。
 *******************************************************************************
 */
void ApiLcd_LcdStop( void )
{
	/* LCDドライバON→OFF */
	if( LCDON == 1U )
	{
		R_LCD_Stop();															/* LCDコントローラ／ドライバ動作停止 */
		R_LCD_Voltage_Off();													/* 容量分割回路動作停止 */
	}
}

#if 0
/*
 *******************************************************************************
 *	LCDドライバ開始処理
 *
 *	[内容]
 *		LCDドライバの開始を行う。
 *******************************************************************************
 */
void ApiLcd_LcdStart( void )
{
	/* LCDドライバOFF→ON */
	if( LCDON == 0U )
	{
		R_LCD_Start();															/* LCDコントローラ／ドライバ動作開始 */
		R_LCD_Voltage_On();														/* 容量分割回路動作許可 */
	}
}
#endif

#if 0
/*
 *******************************************************************************
 *	ファームアップ表示処理
 *
 *	[内容]
 *		ファームアップ中の表示処理を行う。
 *******************************************************************************
 */
void ApiLcd_FirmUpDisp( void )
{
	ApiLcd_SegSelectOnOff( ecDispSegSelect_SegAllOff );
	ApiLcd_UppLow7SegDsp("FIR", imHigh);
	ApiLcd_UppLow7SegDsp("Up", imLow);
	ApiLcd_Main();
}
#endif

#if (swDebugLcd == imEnable)
/*
 *******************************************************************************
 *	LCD動作確認
 *
 *	[内容]
 *		LCDのセグメントを順に点灯させる。
 *******************************************************************************
 */
void ApiLcd_LcdDebug( void )
{
	static sint16_t wkNumData;
	gvModuleSts.mLcd = ecLcdModuleSts_Sleep;
	ApiLcd_SegAllOnOff( ecDispSegSelect_SegAllOff );

	if (vLcdDebugCnt == 0)
	{
		ApiLcd_SegDsp(ecLcdSeg_S1, imON);
	}
	else if (vLcdDebugCnt == 1U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S2, imON);
	}
	else if (vLcdDebugCnt == 2U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S3, imON);
	}
	else if (vLcdDebugCnt == 3U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S4, imON);
	}
	else if (vLcdDebugCnt == 4U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S5, imON);
	}
	else if (vLcdDebugCnt == 5U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S6, imON);
	}
	else if (vLcdDebugCnt == 6U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S7, imON);
	}
	else if (vLcdDebugCnt == 7U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S8, imON);
	}
	else if (vLcdDebugCnt == 8U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S9, imON);
	}
	else if (vLcdDebugCnt == 9U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S10, imON);
	}
	else if (vLcdDebugCnt == 10U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S11, imON);
	}
	else if (vLcdDebugCnt == 11U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S12, imON);
	}
	else if (vLcdDebugCnt == 12U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S13, imON);
	}
	else if (vLcdDebugCnt == 13U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S14, imON);
	}
	else if (vLcdDebugCnt == 14U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S15, imON);
	}
	else if (vLcdDebugCnt == 15U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S16, imON);
	}
	else if (vLcdDebugCnt == 16U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S17, imON);
	}
	else if (vLcdDebugCnt == 17U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S18, imON);
	}
	else if (vLcdDebugCnt == 18U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S19, imON);
	}	
	else if (vLcdDebugCnt == 19U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S20, imON);
	}
	else if (vLcdDebugCnt == 20U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S21, imON);
	}
	else if (vLcdDebugCnt == 21U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S22, imON);
	}
	else if (vLcdDebugCnt == 22U)
	{
		ApiLcd_SegDsp(ecLcdSeg_S23, imON);
	}
	else if (vLcdDebugCnt == 23U)
	{
		ApiLcd_SegDsp(ecLcdSeg_T1, imON);
	}
	else if (vLcdDebugCnt == 24U)
	{
		ApiLcd_SegDsp(ecLcdSeg_T2, imON);
	}
	else if (vLcdDebugCnt == 25U)
	{
		ApiLcd_SegDsp(ecLcdSeg_T3, imON);
	}
	else if (vLcdDebugCnt == 26U)
	{
		ApiLcd_SegDsp(ecLcdSeg_T4, imON);
	}
	else if (vLcdDebugCnt == 27U)
	{
		ApiLcd_SegDsp(ecLcdSeg_T5, imON);
	}
	else if (vLcdDebugCnt == 28U)
	{
		ApiLcd_SegDsp(ecLcdSeg_T6, imON);
	}
	else if (vLcdDebugCnt == 29U)
	{
		ApiLcd_SegDsp(ecLcdSeg_T7, imON);
	}
	else if (vLcdDebugCnt == 30U)
	{
		ApiLcd_Upp7SegNumDsp(-1999, imOFF, imOFF);
		ApiLcd_Low7SegNumDsp(-1999, imOFF);
	}
	else if ((vLcdDebugCnt >= 31U)
	&&		 (vLcdDebugCnt <= 69)
	)
	{
		if (vLcdDebugCnt == 31U)
		{
			wkNumData = -1900;
		}
		ApiLcd_Upp7SegNumDsp(wkNumData, imOFF, imOFF);
		ApiLcd_Low7SegNumDsp(wkNumData, imOFF);
		wkNumData += 100;
	}
	else if (vLcdDebugCnt == 70U)
	{
		ApiLcd_Upp7SegNumDsp(1999, imOFF, imOFF);
		ApiLcd_Low7SegNumDsp(1999, imOFF);
	}
	else if (vLcdDebugCnt == 71U)
	{
		ApiLcd_UppLow7SegDsp("AAA", imHigh);
		ApiLcd_UppLow7SegDsp("AAA", imLow);
	}
	else if (vLcdDebugCnt == 72U)
	{
		ApiLcd_UppLow7SegDsp("BBB", imHigh);
		ApiLcd_UppLow7SegDsp("BBB", imLow);
	}
	else if (vLcdDebugCnt == 73U)
	{
		ApiLcd_UppLow7SegDsp("CCC", imHigh);
		ApiLcd_UppLow7SegDsp("CCC", imLow);
	}
	else if (vLcdDebugCnt == 74U)
	{
		ApiLcd_UppLow7SegDsp("DDD", imHigh);
		ApiLcd_UppLow7SegDsp("DDD", imLow);
	}
	else if (vLcdDebugCnt == 75U)
	{
		ApiLcd_UppLow7SegDsp("EEE", imHigh);
		ApiLcd_UppLow7SegDsp("EEE", imLow);
	}
	else if (vLcdDebugCnt == 76U)
	{
		ApiLcd_UppLow7SegDsp("FFF", imHigh);
		ApiLcd_UppLow7SegDsp("FFF", imLow);
	}
	else if (vLcdDebugCnt == 77U)
	{
		ApiLcd_UppLow7SegDsp("GGG", imHigh);
		ApiLcd_UppLow7SegDsp("GGG", imLow);
	}
	else if (vLcdDebugCnt == 78U)
	{
		ApiLcd_UppLow7SegDsp("HHH", imHigh);
		ApiLcd_UppLow7SegDsp("HHH", imLow);
	}
	else if (vLcdDebugCnt == 79U)
	{
		ApiLcd_UppLow7SegDsp("III", imHigh);
		ApiLcd_UppLow7SegDsp("III", imLow);
	}
	else if (vLcdDebugCnt == 80U)
	{
		ApiLcd_UppLow7SegDsp("JJJ", imHigh);
		ApiLcd_UppLow7SegDsp("JJJ", imLow);
	}
	else if (vLcdDebugCnt == 81U)
	{
		ApiLcd_UppLow7SegDsp("KKK", imHigh);
		ApiLcd_UppLow7SegDsp("KKK", imLow);
	}
	else if (vLcdDebugCnt == 82U)
	{
		ApiLcd_UppLow7SegDsp("LLL", imHigh);
		ApiLcd_UppLow7SegDsp("LLL", imLow);
	}
	else if (vLcdDebugCnt == 83U)
	{
		ApiLcd_UppLow7SegDsp("MMM", imHigh);
		ApiLcd_UppLow7SegDsp("MMM", imLow);
	}
	else if (vLcdDebugCnt == 84U)
	{
		ApiLcd_UppLow7SegDsp("NNN", imHigh);
		ApiLcd_UppLow7SegDsp("NNN", imLow);
	}
	else if (vLcdDebugCnt == 85U)
	{
		ApiLcd_UppLow7SegDsp("OOO", imHigh);
		ApiLcd_UppLow7SegDsp("OOO", imLow);
	}
	else if (vLcdDebugCnt == 86U)
	{
		ApiLcd_UppLow7SegDsp("PPP", imHigh);
		ApiLcd_UppLow7SegDsp("PPP", imLow);
	}
	else if (vLcdDebugCnt == 87U)
	{
		ApiLcd_UppLow7SegDsp("QQQ", imHigh);
		ApiLcd_UppLow7SegDsp("QQQ", imLow);
	}
	else if (vLcdDebugCnt == 88U)
	{
		ApiLcd_UppLow7SegDsp("RRR", imHigh);
		ApiLcd_UppLow7SegDsp("RRR", imLow);
	}
	else if (vLcdDebugCnt == 89U)
	{
		ApiLcd_UppLow7SegDsp("SSS", imHigh);
		ApiLcd_UppLow7SegDsp("SSS", imLow);
	}
	else if (vLcdDebugCnt == 90U)
	{
		ApiLcd_UppLow7SegDsp("TTT", imHigh);
		ApiLcd_UppLow7SegDsp("TTT", imLow);
	}
	else if (vLcdDebugCnt == 91U)
	{
		ApiLcd_UppLow7SegDsp("UUU", imHigh);
		ApiLcd_UppLow7SegDsp("UUU", imLow);
	}
	else if (vLcdDebugCnt == 92U)
	{
		ApiLcd_UppLow7SegDsp("VVV", imHigh);
		ApiLcd_UppLow7SegDsp("VVV", imLow);
	}
	else if (vLcdDebugCnt == 93U)
	{
		ApiLcd_UppLow7SegDsp("WWW", imHigh);
		ApiLcd_UppLow7SegDsp("WWW", imLow);
	}
	else if (vLcdDebugCnt == 94U)
	{
		ApiLcd_UppLow7SegDsp("XXX", imHigh);
		ApiLcd_UppLow7SegDsp("XXX", imLow);
	}
	else if (vLcdDebugCnt == 95U)
	{
		ApiLcd_UppLow7SegDsp("YYY", imHigh);
		ApiLcd_UppLow7SegDsp("YYY", imLow);
	}
	else if (vLcdDebugCnt == 96U)
	{
		ApiLcd_UppLow7SegDsp("ZZZ", imHigh);
		ApiLcd_UppLow7SegDsp("ZZZ", imLow);
	}
	else if (vLcdDebugCnt == 97U)
	{
		ApiLcd_TimeDsp(0, 0);													/* 00:00→AM12：00 */
	}
	else if (vLcdDebugCnt == 98U)
	{
		ApiLcd_TimeDsp(0, 1);													/* 00:01→AM12：01 */
	}
	else if (vLcdDebugCnt == 99U)
	{
		ApiLcd_TimeDsp(11, 59);													/* 11:59→AM11：59 */
	}
	else if (vLcdDebugCnt == 100U)
	{
		ApiLcd_TimeDsp(12, 0);													/* 12:00→PM12：00 */
	}
	else if (vLcdDebugCnt == 101U)
	{
		ApiLcd_TimeDsp(12, 1);													/* 12:01→PM12：01 */
	}
	else if (vLcdDebugCnt == 102U)
	{
		ApiLcd_TimeDsp(23, 59);													/* 23:59→PM11：59 */
	}
	else if (vLcdDebugCnt == 103U)
	{
		ApiLcd_TimeDsp(23, 60);													/* 分 異常データ */
	}
	else if (vLcdDebugCnt == 104U)
	{
		ApiLcd_TimeDsp(24, 00);													/* 時 異常データ */
	}
	else if (vLcdDebugCnt == 105U)
	{
		ApiLcd_SegAllOnOff( ecDispSegSelect_SegAllOn );							/* 全点灯 */
	}
	else
	{
		ApiLcd_SegAllOnOff( ecDispSegSelect_SegAllOff );						/* 全消灯 */
	}

	vLcdDebugCnt++;

	if (vLcdDebugCnt >= 110U)
	{
		vLcdDebugCnt = 0U;
	}
	else
	{
		;
	}
}

#endif

#pragma section
