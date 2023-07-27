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
* File Name    : r_cg_sau.c
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
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
volatile uint8_t * gp_csi10_rx_address; 	   /* csi10 receive buffer address */
volatile uint16_t  g_csi10_rx_length;		   /* csi10 receive data length */
volatile uint16_t  g_csi10_rx_count;		   /* csi10 receive data count */
volatile uint8_t * gp_csi10_tx_address; 	   /* csi10 send buffer address */
volatile uint16_t  g_csi10_send_length; 	   /* csi10 send data length */
volatile uint16_t  g_csi10_tx_count;		   /* csi10 send data count */
volatile uint8_t   g_iic00_master_status_flag; /* iic00 start flag for send address check by master mode */
volatile uint8_t * gp_iic00_tx_address; 	   /* iic00 send data pointer by master mode */
volatile uint16_t  g_iic00_tx_count;		   /* iic00 send data size by master mode */
volatile uint8_t * gp_iic00_rx_address; 	   /* iic00 receive data pointer by master mode */
volatile uint16_t  g_iic00_rx_count;		   /* iic00 receive data size by master mode */
volatile uint16_t  g_iic00_rx_length;		   /* iic00 receive data length by master mode */
volatile uint8_t * gp_uart2_tx_address; 	   /* uart2 transmit buffer address */
volatile uint16_t  g_uart2_tx_count;		   /* uart2 transmit data number */
volatile uint8_t * gp_uart2_rx_address; 	   /* uart2 receive buffer address */
volatile uint16_t  g_uart2_rx_count;		   /* uart2 receive data number */
volatile uint16_t  g_uart2_rx_length;		   /* uart2 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_SAU0_Create
* Description  : This function initializes the SAU0 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAU0_Create(void)
{
	SAU0EN = 1U;	/* supply SAU0 clock */
	NOP();
	NOP();
	NOP();
	NOP();
	SPS0 = _0000_SAU_CK00_FCLK_0 | _0000_SAU_CK01_FCLK_0;
	R_CSI10_Create();
	R_IIC00_Create();
}
/***********************************************************************************************************************
* Function Name: R_CSI10_Create
* Description  : This function initializes the CSI10 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI10_Create(void)
{
	ST0 |= _0004_SAU_CH2_STOP_TRG_ON;	 /* disable CSI10 */
	CSIMK10 = 1U;	 /* disable INTCSI10 interrupt */
	CSIIF10 = 0U;	 /* clear INTCSI10 interrupt flag */
	/* Set INTCSI10 low priority */
	CSIPR110 = 1U;
	CSIPR010 = 1U;
	SIR02 = _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;	/* clear error flag */
	SMR02 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | 
			_0000_SAU_TRIGGER_SOFTWARE | _0000_SAU_MODE_CSI | _0000_SAU_TRANSFER_END;
	SCR02 = _C000_SAU_RECEPTION_TRANSMISSION | _3000_SAU_TIMING_4 | _0000_SAU_MSB | _0007_SAU_LENGTH_8;
	SDR02 = _0400_CSI10_DIVISOR;
	SO0 &= ~_0400_SAU_CH2_CLOCK_OUTPUT_1;	 /* CSI10 clock initial level */
	SO0 &= ~_0004_SAU_CH2_DATA_OUTPUT_1;	/* CSI10 SO initial level */
	SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;	/* enable CSI10 output */
	/* Set SI10 pin */
	PFSEG6 &= 0xFBU;
	PM0 |= 0x40U;
	/* Set SO10 pin */
	PFSEG6 &= 0xF7U;
	P0 |= 0x80U;
	PM0 &= 0x7FU;
	/* Set SCK10 pin */
	PFSEG6 &= 0xFDU;
	P0 |= 0x20U;
	PM0 &= 0xDFU;
}
/***********************************************************************************************************************
* Function Name: R_CSI10_Start
* Description  : This function starts the CSI10 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI10_Start(void)
{
	SO0 &= ~_0400_SAU_CH2_CLOCK_OUTPUT_1;	/* CSI10 clock initial level */
	SO0 &= ~_0004_SAU_CH2_DATA_OUTPUT_1;	/* CSI10 SO initial level */
	SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;	/* enable CSI10 output */
	SS0 |= _0004_SAU_CH2_START_TRG_ON;	   /* enable CSI10 */
	CSIIF10 = 0U;	 /* clear INTCSI10 interrupt flag */
	CSIMK10 = 0U;	 /* enable INTCSI10 */
}
/***********************************************************************************************************************
* Function Name: R_CSI10_Stop
* Description  : This function stops the CSI10 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_CSI10_Stop(void)
{
	CSIMK10 = 1U;	 /* disable INTCSI10 interrupt */
	ST0 |= _0004_SAU_CH2_STOP_TRG_ON;		 /* disable CSI10 */
	SOE0 &= ~_0004_SAU_CH2_OUTPUT_ENABLE;	 /* disable CSI10 output */
	CSIIF10 = 0U;	 /* clear INTCSI10 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_CSI10_Send_Receive
* Description  : This function sends and receives CSI10 data.
* Arguments    : tx_buf -
*					 transfer buffer pointer
*				 tx_num -
*					 buffer size
*				 rx_buf -
*					 receive buffer pointer
* Return Value : status -
*					 MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_CSI10_Send_Receive(uint8_t * const tx_buf, uint16_t tx_num, uint8_t * const rx_buf)
{
	MD_STATUS status = MD_OK;

	if (tx_num < 1U)
	{
		status = MD_ARGERROR;
	}
	else
	{
		g_csi10_tx_count = tx_num;		  /* send data count */
		gp_csi10_tx_address = tx_buf;	  /* send buffer pointer */
		gp_csi10_rx_address = rx_buf;	  /* receive buffer pointer */
		CSIMK10 = 1U;					  /* disable INTCSI10 interrupt */

		if (0U != gp_csi10_tx_address)
		{
			SIO10 = *gp_csi10_tx_address;	 /* started by writing data to SDR[7:0] */
			gp_csi10_tx_address++;
		}
		else
		{
			SIO10 = 0xFFU;
		}

		g_csi10_tx_count--;
		CSIMK10 = 0U;					  /* enable INTCSI10 interrupt */
	}

	return (status);
}
/***********************************************************************************************************************
* Function Name: R_IIC00_Create
* Description  : This function initializes the IIC00 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC00_Create(void)
{
	ST0 |= _0001_SAU_CH0_STOP_TRG_ON;	 /* disable IIC00 */
	IICMK00 = 1U;	 /* disable INTIIC00 interrupt */
	IICIF00 = 0U;	 /* clear INTIIC00 interrupt flag */
	/* Set INTIIC00 low priority */
	IICPR100 = 1U;
	IICPR000 = 1U;
	SIR00 = _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;	/* clear error flag */
	SMR00 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | 
			_0000_SAU_TRIGGER_SOFTWARE | _0000_SAU_EDGE_FALL | _0004_SAU_MODE_IIC | _0000_SAU_TRANSFER_END;
	SCR00 = _0000_SAU_TIMING_1 | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0000_SAU_MSB | _0010_SAU_STOP_1 | 
			_0007_SAU_LENGTH_8;
	SDR00 = _3A00_IIC00_DIVISOR;
	SO0 |= _0100_SAU_CH0_CLOCK_OUTPUT_1 | _0001_SAU_CH0_DATA_OUTPUT_1;
	/* Set SCL00, SDA00 pin */
	PFSEG5 &= 0xF3U;
	POM1 |= 0x80U;
	P1 |= 0xC0U;
	PM1 &= 0x3FU;
}
/***********************************************************************************************************************
* Function Name: R_IIC00_Master_Send
* Description  : This function starts transferring data for IIC00 in master mode.
* Arguments    : adr -
*					 set address for select slave
*				 tx_buf -
*					 transfer buffer pointer
*				 tx_num -
*					 buffer size
* Return Value : None
***********************************************************************************************************************/
void R_IIC00_Master_Send(uint8_t adr, uint8_t * const tx_buf, uint16_t tx_num)
{
	g_iic00_master_status_flag = _00_SAU_IIC_MASTER_FLAG_CLEAR;    /* clear IIC00 master status flag */
	adr &= 0xFEU;	 /* send mode */
	g_iic00_master_status_flag = _01_SAU_IIC_SEND_FLAG; 		   /* set master status flag */
	SCR00 &= ~_C000_SAU_RECEPTION_TRANSMISSION;
	SCR00 |= _8000_SAU_TRANSMISSION;
	/* Set paramater */
	g_iic00_tx_count = tx_num;
	gp_iic00_tx_address = tx_buf;
	/* Start condition */
	R_IIC00_StartCondition();
	IICIF00 = 0U;	/* clear INTIIC00 interrupt flag */
	IICMK00 = 0U;	/* enable INTIIC00 */
	SIO00 = adr;
}
/***********************************************************************************************************************
* Function Name: R_IIC00_Master_Receive
* Description  : This function starts receiving data for IIC00 in master mode.
* Arguments    : adr -
*					 set address for select slave
*				 rx_buf -
*					 receive buffer pointer
*				 rx_num -
*					 buffer size
* Return Value : None
***********************************************************************************************************************/
void R_IIC00_Master_Receive(uint8_t adr, uint8_t * const rx_buf, uint16_t rx_num)
{
	g_iic00_master_status_flag = _00_SAU_IIC_MASTER_FLAG_CLEAR;    /* clear master status flag */
	adr |= 0x01U;	 /* receive mode */
	g_iic00_master_status_flag = _02_SAU_IIC_RECEIVE_FLAG;		   /* set master status flag */
	SCR00 &= ~_C000_SAU_RECEPTION_TRANSMISSION;
	SCR00 |= _8000_SAU_TRANSMISSION;
	/* Set parameter */
	g_iic00_rx_length = rx_num;
	g_iic00_rx_count = 0U;
	gp_iic00_rx_address = rx_buf;
	/* Start condition */
	R_IIC00_StartCondition();
	IICIF00 = 0U;	/* clear INTIIC00 interrupt flag */
	IICMK00 = 0U;	/* enable INTIIC00 */
	SIO00 = adr;
}
/***********************************************************************************************************************
* Function Name: R_IIC00_Stop
* Description  : This function stops the IIC00 operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC00_Stop(void)
{
	/* Stop transfer */
	IICMK00 = 1U;	 /* disable INTIIC00 */
	ST0 |= _0001_SAU_CH0_STOP_TRG_ON;	 /* disable IIC00 */
	IICIF00 = 0U;	 /* clear INTIIC00 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_IIC00_StartCondition
* Description  : This function starts IIC00 condition.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC00_StartCondition(void)
{
	volatile uint8_t w_count;
	
	SO0 &= ~_0001_SAU_CH0_DATA_OUTPUT_1;	/* clear IIC00 SDA */
	
	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count <= IIC00_WAITTIME; w_count++)
	{
		NOP();
	}
	
	SO0 &= ~_0100_SAU_CH0_CLOCK_OUTPUT_1;	 /* clear IIC00 SCL */
	SOE0 |= _0001_SAU_CH0_OUTPUT_ENABLE;			/* enable IIC00 output */
	SS0 |= _0001_SAU_CH0_START_TRG_ON;				/* enable IIC00 */
}
/***********************************************************************************************************************
* Function Name: R_IIC00_StopCondition
* Description  : This function stops IIC00 condition.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_IIC00_StopCondition(void)
{
	volatile uint8_t w_count;
	
	ST0 |= _0001_SAU_CH0_STOP_TRG_ON;			/* disable IIC00 */
	SOE0 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;		/* disable IIC00 output */
	SO0 &= ~_0001_SAU_CH0_DATA_OUTPUT_1;		/* clear IIC00 SDA */
	SO0 |= _0100_SAU_CH0_CLOCK_OUTPUT_1; /* set IIC00 SCL */
	
	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count <= IIC00_WAITTIME; w_count++)
	{
		NOP();
	}
	
	SO0 |= _0001_SAU_CH0_DATA_OUTPUT_1; 		/* set IIC00 SDA */
}
/***********************************************************************************************************************
* Function Name: R_SAU1_Create
* Description  : This function initializes the SAU1 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_SAU1_Create(void)
{
	SAU1EN = 1U;	/* supply SAU1 clock */
	NOP();
	NOP();
	NOP();
	NOP();
	SPS1 = _0004_SAU_CK00_FCLK_4 | _0040_SAU_CK01_FCLK_4;
	R_UART2_Create();
}
/***********************************************************************************************************************
* Function Name: R_UART2_Create
* Description  : This function initializes the UART2 module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Create(void)
{
	ST1 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;	 /* disable UART2 receive and transmit */
	STMK2 = 1U;    /* disable INTST2 interrupt */
	STIF2 = 0U;    /* clear INTST2 interrupt flag */
	SRMK2 = 1U;    /* disable INTSR2 interrupt */
	SRIF2 = 0U;    /* clear INTSR2 interrupt flag */
	SREMK2 = 1U;   /* disable INTSRE2 interrupt */
	SREIF2 = 0U;   /* clear INTSRE2 interrupt flag */
	/* Set INTST2 level2 priority */
	STPR12 = 1U;
	STPR02 = 0U;
	/* Set INTSR2 level2 priority */
	SRPR12 = 1U;
	SRPR02 = 0U;
	SMR10 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE | 
			_0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR10 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | 
			_0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
	SDR10 = _9A00_UART2_TRANSMIT_DIVISOR;
	NFEN0 |= _10_SAU_RXD2_FILTER_ON;
	SIR11 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;	 /* clear error flag */
	SMR11 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | 
			_0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR11 = _4000_SAU_RECEPTION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | 
			_0007_SAU_LENGTH_8;
	SDR11 = _9A00_UART2_RECEIVE_DIVISOR;
	SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;
	SOL1 |= _0000_SAU_CHANNEL0_NORMAL;	  /* output level normal */
	SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;	/* enable UART2 output */
	/* Set RxD2 pin */
	PFSEG5 &= 0x7FU;
	PM0 |= 0x08U;
	/* Set TxD2 pin */
	PFSEG6 &= 0xFEU;
	P0 |= 0x10U;
	PM0 &= 0xEFU;
}
/***********************************************************************************************************************
* Function Name: R_UART2_Start
* Description  : This function starts the UART2 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Start(void)
{
	SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;    /* output level normal */
	SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;	/* enable UART2 output */
	SS1 |= _0002_SAU_CH1_START_TRG_ON | _0001_SAU_CH0_START_TRG_ON;    /* enable UART2 receive and transmit */
	STIF2 = 0U;    /* clear INTST2 interrupt flag */
	SRIF2 = 0U;    /* clear INTSR2 interrupt flag */
	STMK2 = 0U;    /* enable INTST2 interrupt */
	SRMK2 = 0U;    /* enable INTSR2 interrupt */
}
/***********************************************************************************************************************
* Function Name: R_UART2_Stop
* Description  : This function stops the UART2 module operation.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_UART2_Stop(void)
{
	STMK2 = 1U;    /* disable INTST2 interrupt */
	SRMK2 = 1U;    /* disable INTSR2 interrupt */
	ST1 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;	 /* disable UART2 receive and transmit */
	SOE1 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;	 /* disable UART2 output */
	STIF2 = 0U;    /* clear INTST2 interrupt flag */
	SRIF2 = 0U;    /* clear INTSR2 interrupt flag */
}
/***********************************************************************************************************************
* Function Name: R_UART2_Receive
* Description  : This function receives UART2 data.
* Arguments    : rx_buf -
*					 receive buffer pointer
*				 rx_num -
*					 buffer size
* Return Value : status -
*					 MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART2_Receive(uint8_t * const rx_buf, uint16_t rx_num)
{
	MD_STATUS status = MD_OK;

	if (rx_num < 1U)
	{
		status = MD_ARGERROR;
	}
	else
	{
		g_uart2_rx_count = 0U;
		g_uart2_rx_length = rx_num;
		gp_uart2_rx_address = rx_buf;
	}

	return (status);
}
/***********************************************************************************************************************
* Function Name: R_UART2_Send
* Description  : This function sends UART2 data.
* Arguments    : tx_buf -
*					 transfer buffer pointer
*				 tx_num -
*					 buffer size
* Return Value : status -
*					 MD_OK or MD_ARGERROR
***********************************************************************************************************************/
MD_STATUS R_UART2_Send(uint8_t * const tx_buf, uint16_t tx_num)
{
	MD_STATUS status = MD_OK;

	if (tx_num < 1U)
	{
		status = MD_ARGERROR;
	}
	else
	{
		gp_uart2_tx_address = tx_buf;
		g_uart2_tx_count = tx_num;
		STMK2 = 1U;    /* disable INTST2 interrupt */
		TXD2 = *gp_uart2_tx_address;
		gp_uart2_tx_address++;
		g_uart2_tx_count--;
		STMK2 = 0U;    /* enable INTST2 interrupt */
	}

	return (status);
}

/* Start user code for adding. Do not edit comment generated here */
#pragma section text MY_APP2
/*
 *******************************************************************************
 *	通信ポート送信開始
 *
 *	[内容]
 *		通信ポート送信開始処理を行う
 *******************************************************************************
 */
void R_UART2_SndStart( void )
{
	SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;											/* output level normal */
	SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;										/* enable UART2 output */
	SS1 |= _0001_SAU_CH0_START_TRG_ON;											/* enable UART2 transmit */
	STIF2 = 0U;																	/* clear INTST2 interrupt flag */
	STMK2 = 0U;																	/* enable INTST2 interrupt */
}


/*
 *******************************************************************************
 *	通信ポート送信停止
 *
 *	[内容]
 *		通信ポート送信停止処理を行う
 *******************************************************************************
 */
void R_UART2_SndStop( void )
{
	STMK2 = 1U;																	/* disable INTST2 interrupt */
	ST1 |= _0001_SAU_CH0_STOP_TRG_ON;											/* disable UART2 transmit */
	SOE1 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;										/* disable UART2 output */
	STIF2 = 0U;																	/* clear INTST2 interrupt flag */
}


/*
 *******************************************************************************
 *	通信ポート受信開始
 *
 *	[内容]
 *		通信ポート受信開始処理を行う
 *******************************************************************************
 */
void R_UART2_RcvStart( void )
{
	SS1 |= _0002_SAU_CH1_START_TRG_ON;											/* enable UART2 receive */
	SRIF2 = 0U;																	/* clear INTSR2 interrupt flag */
	SRMK2 = 0U;																	/* enable INTSR2 interrupt */
}


/*
 *******************************************************************************
 *	通信ポート受信停止
 *
 *	[内容]
 *		通信ポート受信停止処理を行う
 *******************************************************************************
 */
void R_UART2_RcvStop( void )
{
	SRMK2 = 1U;																	/* disable INTSR2 interrupt */
	ST1 |= _0002_SAU_CH1_STOP_TRG_ON;											/* disable UART2 receive */
	SRIF2 = 0U;																	/* clear INTSR2 interrupt flag */
}

#pragma section
/* End user code. Do not edit comment generated here */
