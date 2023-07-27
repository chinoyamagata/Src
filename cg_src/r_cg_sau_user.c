/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2013, 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_sau_user.c
* Version	   : Code Generator for RL78/L13 V1.04.02.03 [24 May 2018]
* Device(s)    : R5F10WLF
* Tool-Chain   : CCRL
* Description  : This file implements device driver for SAU module.
* Creation Date: 
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_sau.h"
/* Start user code for include. Do not edit comment generated here */
#include "glLabel.h"															/* グローバル変数・定数(const)ラベル定義 */
#include "UserMacro.h"
#include "func.h"
#if (swSensorCom == imEnable)
#include "r_cg_tau.h"
#include "r_cg_rtc.h"
#endif
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_csi10_interrupt(vect=INTCSI10)
#pragma interrupt r_iic00_interrupt(vect=INTIIC00)
#pragma interrupt r_uart2_interrupt_send(vect=INTST2)
#pragma interrupt r_uart2_interrupt_receive(vect=INTSR2)
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint8_t * gp_csi10_rx_address;		   /* csi10 receive buffer address */
extern volatile uint16_t  g_csi10_rx_length;		   /* csi10 receive data length */
extern volatile uint16_t  g_csi10_rx_count; 		   /* csi10 receive data count */
extern volatile uint8_t * gp_csi10_tx_address;		   /* csi10 send buffer address */
extern volatile uint16_t  g_csi10_send_length;		   /* csi10 send data length */
extern volatile uint16_t  g_csi10_tx_count; 		   /* csi10 send data count */
extern volatile uint8_t   g_iic00_master_status_flag;  /* iic00 start flag for send address check by master mode */
extern volatile uint8_t * gp_iic00_tx_address;		   /* iic00 send data pointer by master mode */
extern volatile uint16_t  g_iic00_tx_count; 		   /* iic00 send data size by master mode */
extern volatile uint8_t * gp_iic00_rx_address;		   /* iic00 receive data pointer by master mode */
extern volatile uint16_t  g_iic00_rx_count; 		   /* iic00 receive data size by master mode */
extern volatile uint16_t  g_iic00_rx_length;		   /* iic00 receive data length by master mode */
extern volatile uint8_t * gp_uart2_tx_address;		   /* uart2 send buffer address */
extern volatile uint16_t  g_uart2_tx_count; 		   /* uart2 send data number */
extern volatile uint8_t * gp_uart2_rx_address;		   /* uart2 receive buffer address */
extern volatile uint16_t  g_uart2_rx_count; 		   /* uart2 receive data number */
extern volatile uint16_t  g_uart2_rx_length;		   /* uart2 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_csi10_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_csi10_interrupt(void)
{
	volatile uint8_t err_type;
	volatile uint8_t sio_dummy;

	err_type = (uint8_t)(SSR02 & _0001_SAU_OVERRUN_ERROR);
	SIR02 = (uint16_t)err_type;

	if (1U == err_type)
	{
		r_csi10_callback_error(err_type);	 /* overrun error occurs */
	}
	else
	{
		if (g_csi10_tx_count > 0U)
		{
			if (0U != gp_csi10_rx_address)
			{
				*gp_csi10_rx_address = SIO10;
				gp_csi10_rx_address++;
			}
			else
			{
				sio_dummy = SIO10;
			}

			if (0U != gp_csi10_tx_address)
			{
				SIO10 = *gp_csi10_tx_address;
				gp_csi10_tx_address++;
			}
			else
			{
				SIO10 = 0xFFU;
			}

			g_csi10_tx_count--;
		}
		else 
		{
			if (0U == g_csi10_tx_count)
			{
				if (0U != gp_csi10_rx_address)
				{
					*gp_csi10_rx_address = SIO10;
				}
				else
				{
					sio_dummy = SIO10;
				}
			}

			r_csi10_callback_sendend();    /* complete send */
			r_csi10_callback_receiveend();	  /* complete receive */
		}
	}
}
/***********************************************************************************************************************
* Function Name: r_csi10_callback_receiveend
* Description  : This function is a callback function when CSI10 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_csi10_callback_receiveend(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_csi10_callback_error
* Description  : This function is a callback function when CSI10 reception error occurs.
* Arguments    : err_type -
*					 error type value
* Return Value : None
***********************************************************************************************************************/
static void r_csi10_callback_error(uint8_t err_type)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	if( M_TSTBIT( SSR02, _0001_SAU_OVERRUN_ERROR ) )
	{
		M_CLRBIT( SIR02, _0001_SAU_OVERRUN_ERROR );
	}
	
	if( M_TSTBIT( SSR02, _0002_SAU_PARITY_ERROR ) )
	{
		M_CLRBIT( SIR02, _0002_SAU_PARITY_ERROR );
	}
	
	if( M_TSTBIT( SSR02, _0004_SAU_FRAM_ERROR ) )
	{
		M_CLRBIT( SIR02, _0004_SAU_FRAM_ERROR );
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_csi10_callback_sendend
* Description  : This function is a callback function when CSI10 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_csi10_callback_sendend(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	gvCsi10SndEndFlg = imON;													/* CSI10送信完了フラグON */
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_iic00_interrupt
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_iic00_interrupt(void)
{
	if (((SSR00 & _0002_SAU_PARITY_ERROR) == 0x0002U) && (g_iic00_tx_count != 0U))
	{
		r_iic00_callback_master_error(MD_NACK);
	}
	else if(((SSR00 & _0001_SAU_OVERRUN_ERROR) == 0x0001U) && (g_iic00_tx_count != 0U))
	{
		r_iic00_callback_master_error(MD_OVERRUN);
	}
	else
	{
		/* Control for master send */
		if ((g_iic00_master_status_flag & _01_SAU_IIC_SEND_FLAG) == 1U)
		{
			if (g_iic00_tx_count > 0U)
			{
				SIO00 = *gp_iic00_tx_address;
				gp_iic00_tx_address++;
				g_iic00_tx_count--;
			}
			else
			{
				R_IIC00_StopCondition();
				r_iic00_callback_master_sendend();
			}
		}
		/* Control for master receive */
		else 
		{
			if ((g_iic00_master_status_flag & _04_SAU_IIC_SENDED_ADDRESS_FLAG) == 0U)
			{
				ST0 |= _0001_SAU_CH0_STOP_TRG_ON;
				SCR00 &= ~_C000_SAU_RECEPTION_TRANSMISSION;
				SCR00 |= _4000_SAU_RECEPTION;
				SS0 |= _0001_SAU_CH0_START_TRG_ON;
				g_iic00_master_status_flag |= _04_SAU_IIC_SENDED_ADDRESS_FLAG;
				
				if (g_iic00_rx_length == 1U)
				{
					SOE0 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;	 /* disable IIC00 out */
				}
				
				SIO00 = 0xFFU;
			}
			else
			{
				if (g_iic00_rx_count < g_iic00_rx_length)
				{
					*gp_iic00_rx_address = SIO00;
					gp_iic00_rx_address++;
					g_iic00_rx_count++;
					
					if (g_iic00_rx_count == (g_iic00_rx_length - 1U))
					{
						SOE0 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;	 /* disable IIC00 out */
						SIO00 = 0xFFU;
					}
					else if (g_iic00_rx_count == g_iic00_rx_length)
					{
						R_IIC00_StopCondition();
						r_iic00_callback_master_receiveend();
					}
					else
					{
						SIO00 = 0xFFU;
					}
				}
			}
		}
	}
}
/***********************************************************************************************************************
* Function Name: r_iic00_callback_master_error
* Description  : This function is a callback function when IIC00 master error occurs.
* Arguments    : flag -
*					 status flag
* Return Value : None
***********************************************************************************************************************/
static void r_iic00_callback_master_error(MD_STATUS flag)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	if( M_TSTBIT( SSR00, _0001_SAU_OVERRUN_ERROR ) )
	{
		M_CLRBIT( SIR00, _0001_SAU_OVERRUN_ERROR );
	}
	
	if( M_TSTBIT( SSR00, _0002_SAU_PARITY_ERROR ) )
	{
		M_CLRBIT( SIR00, _0002_SAU_PARITY_ERROR );
	}
	
	if( M_TSTBIT( SSR00, _0004_SAU_FRAM_ERROR ) )
	{
		M_CLRBIT( SIR00, _0004_SAU_FRAM_ERROR );
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_iic00_callback_master_receiveend
* Description  : This function is a callback function when IIC00 finishes master reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_iic00_callback_master_receiveend(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	gvIicRcvEndFlg = imON;														/* IIC送信完了フラグON */
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_iic00_callback_master_sendend
* Description  : This function is a callback function when IIC00 finishes master transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_iic00_callback_master_sendend(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	gvIicSndEndFlg = imON;														/* IIC送信完了フラグON */
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart2_interrupt_receive
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_uart2_interrupt_receive(void)
{
	volatile uint8_t rx_data;
	volatile uint8_t err_type;
	
	err_type = (uint8_t)(SSR11 & 0x0007U);
	SIR11 = (uint16_t)err_type;

	if (err_type != 0U)
	{
		r_uart2_callback_error(err_type);
	}
	
	rx_data = RXD2;

	if (g_uart2_rx_length > g_uart2_rx_count)
	{
		*gp_uart2_rx_address = rx_data;
		gp_uart2_rx_address++;
		g_uart2_rx_count++;

		if (g_uart2_rx_length == g_uart2_rx_count)
		{
			r_uart2_callback_receiveend();
		}
	}
	else
	{
		r_uart2_callback_softwareoverrun(rx_data);
	}
}
/***********************************************************************************************************************
* Function Name: r_uart2_interrupt_send
* Description  : None
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_uart2_interrupt_send(void)
{
	if (g_uart2_tx_count > 0U)
	{
		TXD2 = *gp_uart2_tx_address;
		gp_uart2_tx_address++;
		g_uart2_tx_count--;
	}
	else
	{
		r_uart2_callback_sendend();
	}
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_receiveend
* Description  : This function is a callback function when UART2 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_receiveend(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	gvModInf.mRcvTmCnt = 0U;												/* 受信タイマカウンタクリア */
	gvModInf.mRcvPos++;														/* 受信データ位置更新 */
	
	gvModInf.mComSts = ecComSts_Rcv;										/* 受信中に状態遷移 */
#if (swSensorCom == imEnable)
	R_TAU0_Channel3_Init();
	gvModuleSts.mCom = ecComModuleSts_Wait;									/* タイマ割込み待ち状態に移行 */
#endif
	
	ApiComDrv_Rcv();														/* 受信開始 */
	
	if( gvMode == ecMode_RfTxLoRa )
	{
		gvMode = ecMode_RfTxLoRaStop;
	}
	else if( gvMode == ecMode_RfTxFSK )
	{
		gvMode = ecMode_RfTxFSKStop;
	}
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_softwareoverrun
* Description  : This function is a callback function when UART2 receives an overflow data.
* Arguments    : rx_data -
*					 receive data
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_softwareoverrun(uint16_t rx_data)
{
	/* Start user code. Do not edit comment generated here */
	EI();
#if (swSensorCom == imEnable)
	gvModInf.mComSts = ecComSts_SndPre;									/* コマンド送信準備状態に遷移 */
#else
	gvModInf.mComSts = ecComSts_RcvWait;								/* 受信待ち */
#endif
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_sendend
* Description  : This function is a callback function when UART2 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_sendend(void)
{
	/* Start user code. Do not edit comment generated here */
	EI();
#if (swSensorCom == imEnable)
	switch( gvModInf.mComSts )
	{
		case ecComSts_NullSnd:
			gvModInf.mComSts = ecComSts_UnitPwnOnWait;					/* ユニット起動待ちに状態遷移 */
			R_TAU0_Channel3_Init();
			gvModuleSts.mCom = ecComModuleSts_Wait;						/* タイマ割込み待ち状態に移行 */
			break;
			
		case ecComSts_Snd:
			ApiComDrv_SwitchRx();										/* 受信切り替え処理 */
			ApiComDrv_Rcv();											/* 受信開始 */
			gvModInf.mComSts = ecComSts_RcvWait;						/* 受信待ちに状態遷移 */
			gvModuleSts.mCom = ecComModuleSts_Wait;						/* タイマ割込み待ち状態に移行 */
			break;
			
		default:
			gvModInf.mComSts = ecComSts_SndPre;							/* コマンド送信準備状態に遷移 */
			break;
	}
#else
	gvModInf.mComSts = ecComSts_SndEnd;									/* 通信状態変更：送信完了 */
#endif
	/* End user code. Do not edit comment generated here */
}
/***********************************************************************************************************************
* Function Name: r_uart2_callback_error
* Description  : This function is a callback function when UART2 reception error occurs.
* Arguments    : err_type -
*					 error type value
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_error(uint8_t err_type)
{
	/* Start user code. Do not edit comment generated here */
	EI();
	if( M_TSTBIT( SSR11, _0001_SAU_OVERRUN_ERROR ) )
	{
		M_CLRBIT( SIR11, _0001_SAU_OVERRUN_ERROR );
	}
	
	if( M_TSTBIT( SSR11, _0002_SAU_PARITY_ERROR ) )
	{
		M_CLRBIT( SIR11, _0002_SAU_PARITY_ERROR );
	}
	
	if( M_TSTBIT( SSR11, _0004_SAU_FRAM_ERROR ) )
	{
		M_CLRBIT( SIR11, _0004_SAU_FRAM_ERROR );
	}
#if (swSensorCom == imEnable)
	gvModInf.mComSts = ecComSts_SndPre;									/* コマンド送信準備状態に遷移 */
#else
	gvModInf.mComSts = ecComSts_RcvWait;								/* 受信待ち */
#endif
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
