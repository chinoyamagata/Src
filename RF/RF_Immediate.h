/*
 *******************************************************************************
 *	File name	:	RF_Macro.h
 *
 *	[���e]
 *		�����ʐM�� define�錾
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2020.4.13
 *******************************************************************************
 */

#ifndef	INCLUDED_RF_IMMEDIATE_H
#define	INCLUDED_RF_IMMEDIATE_H

#include "sx1272Regs-Fsk.h"
#include "sx1272Regs-LoRa.h"

/*
 *==============================================================================
 *	define��`
 *==============================================================================
 */

/*!
 * Board MCU pins definitions
 */
#define RADIO_RESET 								P5_bit.no3
#define RADIO_MOSI									P4_bit.no2
//#define RADIO_MISO									P4_bit.no3
#define RADIO_SCLK									P4_bit.no4

#define RADIO_NSS									P3_bit.no2

#define RADIO_DIO_0 								P5_bit.no7
#define RADIO_DIO_1 								P3_bit.no3
/* R60���t��&R63���O�� */
#define RADIO_DIO_2 								P3_bit.no1

#define RADIO_DIO_4 								P5_bit.no2
#define RADIO_SW									P4_bit.no3

#define RADIO_DIO_0_IO 								PM5_bit.no7
#define RADIO_DIO_1_IO 								PM3_bit.no3
#define	RADIO_DIO_2_IO 								PM3_bit.no1
#define RADIO_DIO_4_IO 								PM5_bit.no2
#define RADIO_RESET_IO 								PM5_bit.no3


/*!
 * \Radio hardware registers initialization definition
 */
#define RADIO_INIT_REGISTERS_VALUE	\
{	\
	{ MODEM_FSK , REG_BITRATEMSB	, RF_BITRATEMSB_240000_BPS },\
	{ MODEM_FSK , REG_BITRATELSB	, RF_BITRATELSB_240000_BPS },\
	{ MODEM_FSK , REG_FDEVMSB		, RF_FDEVMSB_120000_HZ },\
	{ MODEM_FSK , REG_FDEVLSB		, RF_FDEVLSB_120000_HZ },\
	{ MODEM_FSK , REG_FRFMSB		, RF_FRFMSB_923_MHZ },\
	{ MODEM_FSK , REG_FRFMID		, RF_FRFMID_923_MHZ },\
	{ MODEM_FSK , REG_FRFLSB		, RF_FRFLSB_923_MHZ },\
	{ MODEM_FSK , REG_PACONFIG		, RF_PACONFIG_PASELECT_RFO | RF_PACONFIG_MIN },\
	{ MODEM_FSK , REG_PARAMP		, RF_PARAMP_LOWPNTXPLL_ON | RF_PARAMP_0040_US },\
	{ MODEM_FSK , REG_OCP		, RF_OCP_ON | RF_OCP_TRIM_200_MA },\
	{ MODEM_FSK , REG_LNA		, RF_LNA_GAIN_G1 | RF_LNA_BOOST_ON },\
	{ MODEM_FSK , REG_RXCONFIG		, RF_RXCONFIG_RESTARTRXONCOLLISION_OFF | RF_RXCONFIG_AFCAUTO_ON | RF_RXCONFIG_AGCAUTO_ON | RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT},\
	{ MODEM_FSK , REG_RSSICONFIG	, RF_RSSICONFIG_OFFSET_P_00_DB | RF_RSSICONFIG_SMOOTHING_16 },\
	{ MODEM_FSK , REG_RSSICOLLISION	, RF_RSSICOLISION_THRESHOLD },\
	{ MODEM_FSK , REG_RSSITHRESH	, RF_RSSITHRESH_THRESHOLD },\
	{ MODEM_FSK , REG_RXBW		, RF_RXBW_MANT_20 | RF_RXBW_EXP_1 },\
	{ MODEM_FSK , REG_AFCBW		, RF_AFCBW_MANTAFC_16 | RF_AFCBW_EXPAFC_2 },\
	{ MODEM_FSK , REG_OOKPEAK		, RF_OOKPEAK_BITSYNC_ON | RF_OOKPEAK_OOKTHRESHTYPE_PEAK },\
	{ MODEM_FSK , REG_OOKFIX		, RF_OOKFIX_OOKFIXEDTHRESHOLD },\
	{ MODEM_FSK , REG_OOKAVG		, RF_OOKAVG_OOKPEAKTHRESHDEC_000 | RF_OOKAVG_OOKAVERAGETHRESHFILT_10 },\
	{ MODEM_FSK , REG_AFCFEI		, RF_AFCFEI_AFCAUTOCLEAR_ON },\
	{ MODEM_FSK , REG_PREAMBLEDETECT	, RF_PREAMBLEDETECT_DETECTOR_ON | RF_PREAMBLEDETECT_DETECTORSIZE_2 | RF_PREAMBLEDETECT_DETECTORTOL_10 },\
	{ MODEM_FSK , REG_RXTIMEOUT1	, RF_RXTIMEOUT1_TIMEOUTRXRSSI },\
	{ MODEM_FSK , REG_RXTIMEOUT2	, RF_RXTIMEOUT2_TIMEOUTRXPREAMBLE },\
	{ MODEM_FSK , REG_RXTIMEOUT3	, RF_RXTIMEOUT3_TIMEOUTSIGNALSYNC },\
	{ MODEM_FSK , REG_RXDELAY		, RF_RXDELAY_INTERPACKETRXDELAY },\
	{ MODEM_FSK , REG_OSC		, RF_OSC_CLKOUT_OFF },\
	{ MODEM_FSK , REG_PREAMBLEMSB	, RF_PREAMBLEMSB_SIZE },\
	{ MODEM_FSK , REG_PREAMBLELSB	, RF_PREAMBLELSB_SIZE },\
	{ MODEM_FSK , REG_SYNCCONFIG	, RF_SYNCCONFIG_AUTORESTARTRXMODE_WAITPLL_ON | RF_SYNCCONFIG_PREAMBLEPOLARITY_AA | RF_SYNCCONFIG_SYNC_ON | RF_SYNCCONFIG_FIFOFILLCONDITION_AUTO | RF_SYNCCONFIG_SYNCSIZE_8},\
	{ MODEM_FSK , REG_PACKETCONFIG1	, RF_PACKETCONFIG1_PACKETFORMAT_FIXED | RF_PACKETCONFIG1_DCFREE_OFF | RF_PACKETCONFIG1_CRC_OFF | RF_PACKETCONFIG1_CRCAUTOCLEAR_OFF | RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT },\
	{ MODEM_FSK , REG_PACKETCONFIG2	, RF_PACKETCONFIG2_DATAMODE_PACKET | RF_PACKETCONFIG2_IOHOME_OFF | RF_PACKETCONFIG2_BEACON_OFF },\
	{ MODEM_FSK , REG_NODEADRS		, RF_NODEADDRESS_ADDRESS },\
	{ MODEM_FSK , REG_BROADCASTADRS	, RF_BROADCASTADDRESS_ADDRESS },\
	{ MODEM_FSK , REG_FIFOTHRESH	, RF_FIFOTHRESH_TXSTARTCONDITION_FIFOTHRESH | RF_FIFOTHRESH_FIFOTHRESHOLD_THRESHOLD },\
	{ MODEM_FSK , REG_SEQCONFIG1	, 0x00U },\
	{ MODEM_FSK , REG_SEQCONFIG2	, 0x00U },\
	{ MODEM_FSK , REG_TIMERRESOL	, RF_TIMERRESOL_TIMER1RESOL_OFF | RF_TIMERRESOL_TIMER2RESOL_OFF },\
	{ MODEM_FSK , REG_TIMER1COEF	, RF_TIMER1COEF_TIMER1COEFFICIENT },\
	{ MODEM_FSK , REG_TIMER2COEF	, RF_TIMER2COEF_TIMER2COEFFICIENT },\
	{ MODEM_FSK , REG_IMAGECAL		, RF_IMAGECAL_AUTOIMAGECAL_OFF | RF_IMAGECAL_IMAGECAL_DONE | RF_IMAGECAL_TEMPCHANGE_LOWER | RF_IMAGECAL_TEMPTHRESHOLD_10 | RF_IMAGECAL_TEMPMONITOR_ON},\
	{ MODEM_FSK , REG_LOWBAT		, RF_LOWBAT_TRIM_1835 },\
	{ MODEM_FSK , REG_IRQFLAGS1		, RF_IRQFLAGS1_RSSI | RF_IRQFLAGS1_PREAMBLEDETECT },\
	{ MODEM_FSK , REG_IRQFLAGS2		, RF_IRQFLAGS2_FIFOOVERRUN | RF_IRQFLAGS2_LOWBAT },\
	{ MODEM_FSK , REG_DIOMAPPING1	, RF_DIOMAPPING1_DIO0_00 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_00 | RF_DIOMAPPING1_DIO3_00 },\
	{ MODEM_FSK , REG_DIOMAPPING2	, RF_DIOMAPPING2_DIO4_11 | RF_DIOMAPPING2_DIO5_00 | RF_DIOMAPPING2_MAP_PREAMBLEDETECT },\
	{ MODEM_FSK , REG_TCXO		, RF_TCXO_TCXOINPUT_OFF | 0x09U },\
	{ MODEM_FSK , REG_PADAC		, RF_PADAC_20DBM_OFF | 0x80 },\
	{ MODEM_FSK , REG_PLL		, RF_PLL_BANDWIDTH_300 | 0x10U },\
	{ MODEM_FSK , REG_PLLLOWPN		, RF_PLLLOWPN_BANDWIDTH_300 | 0x10U },\
	{ MODEM_FSK , REG_BITRATEFRAC	, 0x0BU },\
	{ MODEM_FSK , REG_AGCREF	, 0x10U },\
	{ MODEM_FSK , REG_AGCTHRESH3, RF_AGCTHRESH3_AGCSTEP4 | RF_AGCTHRESH3_AGCSTEP5 },\
	{ MODEM_LORA , REG_LR_FRFMSB , RFLR_FRFMSB_920_9_MHZ },\
	{ MODEM_LORA , REG_LR_FRFMID , RFLR_FRFMID_920_9_MHZ },\
	{ MODEM_LORA , REG_LR_FRFLSB , RFLR_FRFLSB_920_9_MHZ },\
	{ MODEM_LORA , REG_LR_PACONFIG , RFLR_PACONFIG_PASELECT_RFO | RFLR_PACONFIG_OUTPUTPOWER_MIN },\
	{ MODEM_LORA , REG_LR_PARAMP , RFLR_PARAMP_LOWPNTXPLL_OFF | RFLR_PARAMP_0040_US },\
	{ MODEM_LORA , REG_LR_OCP , RFLR_OCP_ON | RFLR_OCP_TRIM_100_MA },\
	{ MODEM_LORA , REG_LR_LNA , RFLR_LNA_GAIN_G1 | RFLR_LNA_BOOST_ON },\
	{ MODEM_LORA , REG_LR_FIFOADDRPTR , RFLR_FIFOADDRPTR },\
	{ MODEM_LORA , REG_LR_FIFOTXBASEADDR , RFLR_FIFOTXBASEADDR },\
	{ MODEM_LORA , REG_LR_FIFORXBASEADDR , RFLR_FIFORXBASEADDR },\
	{ MODEM_LORA , REG_LR_MODEMCONFIG1 , RFLR_MODEMCONFIG1_BW_125_KHZ | RFLR_MODEMCONFIG1_CODINGRATE_4_7 | RFLR_MODEMCONFIG1_IMPLICITHEADER_ON | RFLR_MODEMCONFIG1_RXPAYLOADCRC_OFF | RFLR_MODEMCONFIG1_LOWDATARATEOPTIMIZE_OFF},\
	{ MODEM_LORA , REG_LR_MODEMCONFIG2 , RFLR_MODEMCONFIG2_SF_8 | RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_OFF | RFLR_MODEMCONFIG2_AGCAUTO_ON | RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB},\
	{ MODEM_LORA , REG_LR_SYMBTIMEOUTLSB , RFLR_SYMBTIMEOUTLSB_SYMBTIMEOUT },\
	{ MODEM_LORA , REG_LR_PREAMBLEMSB , RFLR_PREAMBLELENGTHMSB },\
	{ MODEM_LORA , REG_LR_PREAMBLELSB , RFLR_PREAMBLELENGTHLSB },\
	{ MODEM_LORA , REG_LR_PAYLOADLENGTH , RFLR_PAYLOADLENGTH },\
	{ MODEM_LORA , REG_LR_PAYLOADMAXLENGTH , RFLR_PAYLOADMAXLENGTH },\
	{ MODEM_LORA , REG_LR_HOPPERIOD , RFLR_HOPPERIOD_FREQFOPPINGPERIOD },\
	{ MODEM_LORA , REG_LR_DETECTOPTIMIZE , 0x40 | RFLR_DETECTIONOPTIMIZE_SF7_TO_SF12},\
	{ MODEM_LORA , REG_LR_DETECTIONTHRESHOLD , RFLR_DETECTIONTHRESH_SF7_TO_SF12 },\
	{ MODEM_LORA , REG_LR_SYNCWORD , 0x58 },\
	{ MODEM_LORA , REG_LR_DIOMAPPING1 , RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00 },\
	{ MODEM_LORA , REG_LR_DIOMAPPING2 , RFLR_DIOMAPPING2_DIO4_11 | RFLR_DIOMAPPING2_DIO5_00 | RFLR_DIOMAPPING2_MAP_RSSI },\
}


/*!
 * \Radio Broadcast registers initialization definition
 *
 */

#define RADIO_BROADCAST_INIT_REGISTERS_VALUE	\
{	\
	{ MODEM_FSK , REG_DIOMAPPING1	, RF_DIOMAPPING1_DIO0_00 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_00 | RF_DIOMAPPING1_DIO3_00 },\
	{ MODEM_FSK , REG_DIOMAPPING2	, RF_DIOMAPPING2_DIO4_11 | RF_DIOMAPPING2_DIO5_00 | RF_DIOMAPPING2_MAP_PREAMBLEDETECT },\
	{ MODEM_FSK , REG_FIFOTHRESH  , RF_FIFOTHRESH_FIFOTHRESHOLD_TX },\
	{ MODEM_FSK , REG_BITRATEMSB	, RF_BITRATEMSB_100000_BPS },\
	{ MODEM_FSK , REG_BITRATELSB	, RF_BITRATELSB_100000_BPS },\
	{ MODEM_FSK , REG_FDEVMSB		, RF_FDEVMSB_50000_HZ },\
	{ MODEM_FSK , REG_FDEVLSB		, RF_FDEVLSB_50000_HZ },\
}


#define RADIO_BROADCAST_240K_INIT_REGISTERS_VALUE	\
{	\
	{ MODEM_FSK , REG_DIOMAPPING1	, RF_DIOMAPPING1_DIO0_00 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_00 | RF_DIOMAPPING1_DIO3_00 },\
	{ MODEM_FSK , REG_DIOMAPPING2	, RF_DIOMAPPING2_DIO4_11 | RF_DIOMAPPING2_DIO5_00 | RF_DIOMAPPING2_MAP_PREAMBLEDETECT },\
	{ MODEM_FSK , REG_FIFOTHRESH  , RF_FIFOTHRESH_FIFOTHRESHOLD_TX },\
	{ MODEM_FSK , REG_BITRATEMSB	, RF_BITRATEMSB_240000_BPS },\
	{ MODEM_FSK , REG_BITRATELSB	, RF_BITRATELSB_240000_BPS },\
	{ MODEM_FSK , REG_FDEVMSB		, RF_FDEVMSB_120000_HZ },\
	{ MODEM_FSK , REG_FDEVLSB		, RF_FDEVLSB_120000_HZ },\
}


#define RADIO_DATASEND_INIT_REGISTERS_VALUE		\
{	\
	{ MODEM_FSK , REG_DIOMAPPING1	, RF_DIOMAPPING1_DIO0_00 | RF_DIOMAPPING1_DIO1_00 | RF_DIOMAPPING1_DIO2_00 | RF_DIOMAPPING1_DIO3_00 },\
	{ MODEM_FSK , REG_DIOMAPPING2	, RF_DIOMAPPING2_DIO4_11 | RF_DIOMAPPING2_DIO5_00 | RF_DIOMAPPING2_MAP_PREAMBLEDETECT },\
	{ MODEM_FSK , REG_FIFOTHRESH	, RF_FIFOTHRESH_FIFOTHRESHOLD_TX },\
	{ MODEM_FSK , REG_BITRATEMSB	, RF_BITRATEMSB_300000_BPS },\
	{ MODEM_FSK , REG_BITRATELSB	, RF_BITRATELSB_300000_BPS },\
	{ MODEM_FSK , REG_FDEVMSB		, RF_FDEVMSB_150000_HZ },\
	{ MODEM_FSK , REG_FDEVLSB		, RF_FDEVLSB_150000_HZ },\
}


/*!
 * Constant values need to compute the RSSI value
*/
#define RSSI_OFFSET 								-139


/* FSK���[�h�ł�CALL�����WakeUP��M�� */
#define RF_HSMODE_BEACON_LENGTH						30U
/* FSK���[�h�ł�CALL�M���ւ�RESPONCE�� */
#define	RF_HSMODE_RES_STATUS_LENGTH					104U			/* �����Z�o�pID�ǉ� */

//#define	RF_HSMODE_RES_MEASURE_LENGTH				1385U
//#define RF_HSMODE_RES_MEASURE_LENGTH_HEADER			17U
#define	RF_HSMODE_RES_MEASURE_LENGTH				1392U
#define RF_HSMODE_RES_MEASURE_LENGTH_HEADER			24U
#define RF_HSMODE_RES_ALHIS_LENGTH					1232U
#define RF_HSMODE_RES_ALHIS_LENGTH_HEADER			12U
#define RF_HSMODE_RES_ABNORMAL_LENGTH				932U
#define RF_HSMODE_RES_ABNORMAL_LENGTH_HEADER		12U
#define RF_HSMODE_RES_EVENTHIS_LENGTH				1284U
#define RF_HSMODE_RES_EVENTHIS_LENGTH_HEADER		12U
#define RF_HSMODE_RES_OPEHIS_LENGTH					1380U
#define RF_HSMODE_RES_OPEHIS_LENGTH_HEADER			12U
#define RF_HSMODE_RES_SYSHIS_LENGTH					1380U
#define RF_HSMODE_RES_SYSHIS_LENGTH_HEADER			12U
#define RF_HSMODE_RES_SETREAD_LENGTH				234U
#define RF_HSMODE_RES_SETREAD_LENGTH_HEADER			9U
#define RF_HSMODE_RES_SETREAD_LENGTH_BCH			75U
#define RF_HSMODE_RES_SETQUERY_LENGTH				20U
#define RF_HSMODE_RES_SETWRITE_LENGTH				234U
#define RF_HSMODE_RES_SETWRITE_LENGTH_HEADER		9U
#define RF_HSMODE_RES_SETWRITE_LENGTH_BCH			75U
#define RF_HSMODE_RES_REQ_FIRM_LENGTH				20U			/* RF_HSMODE_ACK_LENGTH�Ɠ����p�P�b�g */			
#define RF_HSMODE_ACK_LENGTH						20U
#define RF_HSMODE_FIRM_HEX_LENGTH					1361U
#define RF_HSMODE_FIRM_HEX_LENGTH_HEX_CRC			900U
#define RF_HSMODE_FIRM_HEX_LENGTH_HEADER			9U
/* FSK���[�h�ł�CALL���m���� */
//#define RF_FSK_WAKEUP_WAIT_PRE						60U
//#define RF_FSK_WAKEUP_WAIT_SYNC 					255U
/* FSK���[�h�ł�Syncword�� 8byte*/
#define RF_FSK_SYNCWORD_LENGTH_FSK					6U
/* FSK���[�h��CALL����Preamble�� 32byte */
#define RF_FSK_PREAMBLE_LENGTH_CALL 				40U
/* FSK���[�h��CALL�M���ւ̉�������Preamble��  */
//#define RF_FSK_PREAMBLE_LENGTH_RESPONSE 			8U
//#define RF_FSK_PREAMBLE_LENGTH_RESPONSE 			12U
#define RF_FSK_PREAMBLE_LENGTH_RESPONSE 			25U
/* FSK���[�h��Responce�ւ�Ack��������Preamble�� */
#define RF_FSK_PREAMBLE_LENGTH_ACK					120U
#define RF_FSK_PREAMBLE_FIRM_HEX					120U

//#define RF_CAD_RSSI_TIME							1043U		/* CAD����RSSI�擾�҂����� 2.304msec((2^8 + 32)/125) + 240usec + 240usec */
#define RF_CAD_RSSI_TIME							1060U		/* CAD����RSSI�擾�҂����� 2.304msec((2^8 + 32)/125) + 240usec + 240usec */

#define RF_HSMODE_WAKEUP_TIME						6U			/* �����ʐM�ł�1set������̋N���� ������Sync����ւ��i���K�[���o / �v���E�ݒ� �̐؂�ւ��j */
#define RF_HSMODE_WAKEUP_SET						3U			/* �����ʐM�ł̐ݒ�E�v����Sync����ւ�����^�C�~���O RF_HSMODE_WAKEUP_SET����RF_HSMODE_WAKEUP_TIME�܂Őݒ�E�v�� */
#define RF_HSMODE_DECT_INT							44UL		/* �����ʐM�ł̌ďo���mSnif�̎��� msec */
/*
 * RF SW�̓d���_����`
*/
#if (swLoggerBord == imEnable)
#define RF_SW_ON									0U	/* Logger:HI��OFF�ALo��ON */
#define RF_SW_OFF									1U
#else
#define RF_SW_ON									1U	/* GW:HI��ON�ALo��OFF */
#define RF_SW_OFF									0U
#endif


/*
 *Test Port
*/
#ifndef P_TH_REF
#define	P_TH_REF		P2_bit.no0			/* ���x���t�@�����X�v�� */
#endif


#define _PREAMBLE_WAIT_ITMCP_VALUE				(97U)			/* 0.26msec */
//#define _PREAMBLE_WAIT_ITMCP_VALUE			(105U)			/* 0.26msec */
#define _PREAMBLE_RETRY_WAIT_ITMCP_VALUE		(396U)			/* 1.056msec */
#define _SYNC_WAIT_ITMCP_VALUE					(516U)			/* 1.376msec(1.376msec�ڕW) */
#define _SYNCMISS_RETRY_ITMCP_VALUE				(360U)			/* 0.96msec */
#define _HSMODE_DETECT_RETRY_ITMCP_VALUE		(18750U)		/* 50msec */
#define _MAIN_PROCESS_DELAY_ITMCP_VALUE			(0U)			/* 0msec */
#define _RESPONCE_SEND_PROCESS_ITMCP_VALUE		(1U)			/* 0msec */
#define _CALL_RESPONCE_SEND_TIMESLOT_NUM		(3000U)			/* 8msec */
//#define _CALL_RESPONCE_SEND_TIMESLOT_GRUPE		(37500U)		/* 100msec */
#define _CALL_RESPONCE_SEND_TIMESLOT_GRUPE		(37500UL + 375UL)	/* 101msec */
#define _CALL_RESPONCE_RECIEVE_ITMCP_VALUE		(556U)			/* 17msec */

/* �����ʐM�̌��� */
#define SUCCESS 		(0x00)
#define FAIL			(0x01)
#define ERROR			(0x02)
#define	WAIT			(0x03)
#define	NEXT			(0x04)
#define	NO_CARRIER		(0x05)
#define	SENS_CARRIER	(0x06)
#define END				(0x07)

/*** CRC�m�F/�Z�o ***/
#define	CRC_CHK		( 0U )			/* CRC�m�F */
#define	CRC_CAL		( 1U )			/* CRC�Z�o */
#define CRC_OK		( 0U )
#define CRC_NG		( 1U )

/* �R�[�f�B���O��BCH�L������ */
#define BCH_ON		( 1U )
#define BCH_OFF		( 0U )

/* �I�����C���F�؂�ON/OFF */
#define CENTRAL		( 1U )
#define LOCAL	( 0U )

/* Mode Select */
#define	RT_MODE		( 1U )
#define	HS_MODE		( 0U )
#define SCAN_MODE	( 2U )
#define OFF_MODE	( 3U )
#define TEST_MODE	( 4U )

/* Power Mode Select */
#define PW_H		( 1U )
#define PW_L		( 0U )

/* Power Change Rssi Threshold */
#define PW_TH_H_TO_L	( 85U )			/* PW H �� L */
#define PW_TH_L_TO_H	( 100U )			/* PW L �� H */

/* RTmode �L�����A�Z���X 5msec/126usec���� */
#define	CARRIRE_CLEAR		( 1U )
#define	CARRIRE_DETECT		( 0U )

/* �L�����A�Z���X��Timer�J�E���g */
#define CARRIRE_TIME		(85U)

/* �L�����A�Z���X�L��̏ꍇ�̃`���l���ύX */
#define CH_CHANGE			(1U)
#define CH_FIX				(0U)

/* �L�����A�Z���X�֐��ł�RTmode/HSmode���f */
#define CS_RT_CONNECT_MODE			(0U)
#define CS_RT_MEASURE_MODE			(1U)

/* GW�̐V�K���K�[�o�^���[�h */
#define LOGGER_TOROKU_ON	( 1U )
#define LOGGER_TOROKU_OFF	( 0U )

/* �d�r�����̌��o�t���O */
#define BATT_POWER_ON		( 1U )
#define BATT_POWER_OFF		( 0U )

/* GW�v���l�ʐM�̖����`���l���I�����[�h */
#define CH_AUTO				( 1U )
#define CH_MANU				( 0U )

/* RTmode Ch Loop Number */
#define	RT_MODE_CH_LOOP_NUM			( 2U )
/* RTmode CarrierSens ReTry Number */
#define RT_MODE_CS_RETRY_NUM		( 9U )
/* HSmode Preamble Detect */
#define HS_MODE_PREAMBLE_SUCCESS	( 0U )
#define HS_MODE_PREAMBLE_FAIL		( 1U )
#define HS_MODE_SYNC_SUCCESS		( 0U )
#define HS_MODE_SYNC_FAIL			( 1U )

/* ���g��Offset */
#if (swLoggerBord == imEnable)
#define RF_FREQ_OFFSET		(-200)
#else
#define RF_FREQ_OFFSET		(-250)
#endif

/* ���M�@�y�C���[�h��(CRC�܂�) */
#define RT_MODE_PAYLOAD_LENGTH_1		(7U)
#define RT_MODE_PAYLOAD_LENGTH_2		(27U)
#define RT_MODE_PAYLOAD_LENGTH_3		(91U)
#define RT_MODE_PAYLOAD_LENGTH_4		(11U)
#define RT_MODE_PAYLOAD_LENGTH_5		(95U)
#define RT_MODE_PAYLOAD_LENGTH_5_CRC	(32U)
#define RT_MODE_PAYLOAD_LENGTH_6		(11U)
#define RT_MODE_PAYLOAD_LENGTH_7_1		(43U)
//#define RT_MODE_PAYLOAD_LENGTH_7_2		(83U)
#define RT_MODE_PAYLOAD_LENGTH_7_2		(43U)
#define RT_MODE_PAYLOAD_LENGTH_7_3		(43U)
#define RT_MODE_PAYLOAD_LENGTH_8		(11U)
#define RT_MODE_PAYLOAD_LENGTH_10		(95U)
#define RT_MODE_PAYLOAD_LENGTH_10_CRC	(29U)

/* �ʐM�����ƃr�[�R����t�����Ԃ̐ݒ� */

#define	RT_INT_SHORT_MODE_TX_INT	(10U)
#define RT_INT_MID_MODE_TX_INT		(20U)
#define	RT_INT_LONG_MODE_TX_INT		(60U)
#define RT_INT_SHORT_MODE_SET_TIME	(5U)
#define RT_INT_MID_MODE_SET_TIME	(10U)
#define RT_INT_LONG_MODE_SET_TIME	(20U)
#define RT_LONG_MODE_CONNECT_NUM	(60U)
#define RT_MID_MODE_CONNECT_NUM		(15U)
#define RT_SHORT_MODE_CONNECT_NUM	(7U)

/* ���M���Ԃ̒�`(���M���Ԑ����p) */
#define RT_MODE_TX_TIME_1			(64U)
#define RT_MODE_TX_TIME_2			(157U)
#define RT_MODE_TX_TIME_3			(310U)
#define RT_MODE_TX_TIME_4			(75U)
#define RT_MODE_TX_TIME_5_MAX		(380U)
//#define RT_MODE_TX_TIME_6			(79U)
#define RT_MODE_TX_TIME_6			(0U)
#define RT_MODE_TX_TIME_7_1			(193U)
#define RT_MODE_TX_TIME_7_2			(193U)
#define RT_MODE_TX_TIME_7_3			(193U)
#define RT_MODE_TX_TIME_8			(79U)

/* App ID �̒�` */
#define APP_ID_LOGGER_INIT_UP		(0x0F)
#define APP_ID_LOGGER_INIT_LO		(0xFF)
#define APP_ID_LOGGER_INIT_UP_TOP_ALIGN		(0xFF)
#define APP_ID_LOGGER_INIT_LO_TOP_ALIGN		(0xF0)
#define APP_ID_GW_INIT_UP			(0xFF)			/* ��l�� */
#define APP_ID_GW_INIT_LO			(0xE0)			/* ��l�� */
#define APP_ID_LOCAL_UP				(0xFF)			/* ��l�� */
#define APP_ID_LOCAL_LO				(0xE0)			/* ��l�� */


/* HSmode ��M���Sleep���[�h */
#define HSMODE_SLEEP_MODE_NON		(0x00)			/* Sleep�Ȃ� */
#define HSMODE_SLEEP_MODE_CNTDWN	(0x01)			/* �K���HSmode�X�L�b�v */
#define HSMODE_SLEEP_MODE_TRIG		(0x02)			/* ���삳���܂�Sleep */

#define GW_FILTER_MAXNUM			10U				/* ���K�[��GWID�̃t�B���^�����O�ő�䐔 */

/* ����IC�������t���O��� */
#define RFIC_INI					0U
#define RFIC_RUN					1U

//#define RF_INI_CNT					12U				/* ��5min * 12(+1) => ��65min�ŏ������t���O�Z�b�g */
#define RF_INI_CNT					2U				/* 2022.9.16  ��5min * 2(+1) => ��15min�ŏ������t���O�Z�b�g */
#define RF_ERR_CNT					200U			/* �L�����A�Z���X���Ɉُ폈�����N�����񐔂ŃG���[���肷�邵�����l */

#define MEASURE_DATASET				0U
#define MEASALM_DATASET				1U


#endif