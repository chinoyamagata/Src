/*
 *******************************************************************************
 *	File name	:	immediate.h
 *
 *	[���e]
 *		immediate�萔��`
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2017.11.08
 *******************************************************************************
 */
#ifndef	INCLUDED_IMMEDIATE_H
#define	INCLUDED_IMMEDIATE_H

#include "switch.h"


/*
 ***************************************
 *	���
 ***************************************
 */
#define	imOFF				0U													/* OFF */
#define	imON				1U													/* ON */

#define	imLow				0U													/* Low */
#define	imHigh				1U													/* High */

#define	imDisable			0U													/* �֎~ */
#define	imEnable			1U													/* ���� */

#define	imNASI				0U													/* ���� */
#define	imARI				1U													/* �L�� */

#define	imUnUsed			0U													/* ���g�p */
#define	imUse				1U													/* �g�p */

#define	imIoOut				0U					/* �|�[�g�o�� */
#define	imIoIn				1U					/* �|�[�g���� */

#define	imRtcWrFirstVal		0xA5U				/* �O�t��RTC���A�N�Z�X�t���O */

/*
 ***************************************
 *	����Flash mParmFlg(�p�����[�^�t���O�Ǘ�)
 ***************************************
 */
#define imFactoryInitFlg	M_BIT0				/* Hi�ōH��o�׏��������s */
#define imRtStbyOffFlg		M_BIT1				/* Hi�Ȃ�HS�̂ݑҎ��ԁALo�Ȃ�HS-Rt�Ҏ��� */

/*
 ***************************************
 *	�d�r�d��
 ***************************************
 */
#define	imAdCnt_1600mV		174U				/* 1600mV��AD�J�E���g�l */
#define	imAdCnt_1800mV		196U				/* 1800mV��AD�J�E���g�l */
#define	imAdCnt_1820mV		198U				/* 1820mV��AD�J�E���g�l */
#define	imAdCnt_2300mV		251U				/* 2300mV��AD�J�E���g�l */

/*
 ***************************************
 *	LCD
 ***************************************
 */
#define	imUnder				0U
#define	imOver				1U
#define	imSnsrErr			2U
#define	imBar				3U

/*
 ***************************************
 *	�L�[�֘A
 ***************************************
 */
#define	imKeyChatCnt		4U					/* �`���^�����O����F5msec*4cnt�A�� */
#define	imKey1secPress		200U				/* 1�b����������J�E���g�F5ms*200cnt=1000ms */

#define	KEY_EVENT		(P7_bit.no4)
#define	KEY_DISP		(P7_bit.no5)
#define	KEY_RESET		(P13_bit.no7)

/*
 ***************************************
 *	�^�C�}7
 ***************************************
 */
#define	imTimer07_05msec	0x00BA
#define imTimer07_1msec		0x0176
#define	imTimer07_5300usec	0x07C2
#define	imTimer07_10msec	0x0EA5
#define	imTimer07_40msec	0x3A97

/*
 ***************************************
 *	MODBUS�ʐM
 ***************************************
 */
#define	imMod_RxBufSize		30U				/* Modbus��M�T�C�Y */
#define	imMod_TxBufSize		30U				/* Modbus���M�T�C�Y */

#define	imProcessModeCode	0xC3A5U


/*
 ***************************************
 *	�O�t���t���b�V��������
 ***************************************
 */
/* �X�e�[�^�X���W�X�^1 */
#define	imFlashDrv_Sts1Bit_BUSY		M_BIT0										/* ERASE/WRITE IN PROGRESS */
#define	imFlashDrv_Sts1Bit_WEL		M_BIT1										/* WRITE ENABLE LATCH */
#define	imFlashDrv_Sts1Bit_BP0		M_BIT2										/* BLOCK PROTECT BIT0 */
#define	imFlashDrv_Sts1Bit_BP1		M_BIT3										/* BLOCK PROTECT BIT1 */
#define	imFlashDrv_Sts1Bit_BP2		M_BIT4										/* BLOCK PROTECT BIT2 */
#define	imFlashDrv_Sts1Bit_TB		M_BIT5										/* TOP/BOTTOM PROTECT */
#define	imFlashDrv_Sts1Bit_SEC		M_BIT6										/* SECTOR PROTECT */
#define	imFlashDrv_Sts1Bit_SRP0		M_BIT7										/* WRITE ENABLE LATCH */

#define	imOddExFlashFlg			0xA5U											/* �s�ǃZ�N�^�Ǘ��e�[�u���t���O */

/* IndexNo.�ő�l */
#define imIndexMaxErrInf	65500U					/* �ُ��� */
#define imIndexMaxMeasAlm	65500U					/* �v���x�� */
#define imIndexMaxMeas1		53196U					/* �v���l1 */
#define imIndexMaxMeas2		65472U					/* �v���l2 */
#define imIndexMaxEvt		65400U					/* �C�x���g */
#define imIndexMaxActLog	65250U				/* ���엚�� */
#define imIndexMaxSysLog	65024U					/* �V�X�e�����O */

/* ���^�f�[�^��MAX�� */
#define	imDataNumErrInf			500U				/* �ُ��� */
#define	imDataNumMeasAlm		500U				/* �v���x�� */
#define	imDataNumMeas1			53000U				/* �v���l1 */
#define	imDataNumMeas2			1000U				/* �v���l2 */
#define	imDataNumEvt			300U				/* �C�x���g */
#define	imDataNumActLog			450U				/* ���엚�� */
#define	imDataNumSysLog			2000U				/* �V�X�e�����O */
#define	imDataNumAdrTbl			1536U				/* �A�h���X����e�[�u�� */


/*
 ***************************************
 *	�`�����l��
 ***************************************
 */
#define imChannelNum	4U							/* �`�����l���� */
#define imFlashQue_ChannelNum	3U					/* Flash�ۑ��`�����l���� */
#define imMeasChNum		3U							/* ���菈���̃`���l���� */

/*
 ***************************************
 *	����
 ***************************************
 */
/* ����ُ� */
#define imNonError		0x00U													/* �G���[�Ȃ� */
#define imTmpOver		M_BIT0													/* �I�[�o�[�����W */
#define imTmpUnder		M_BIT1													/* �A���_�[�����W */
#define imRefBurnError	M_BIT2													/* ���t�@�����X��R�o�[���A�E�g */
#define imThBurnError	M_BIT3													/* �T�[�~�X�^�o�[���A�E�g */
#define imPtBurnError	M_BIT3													/* Pt�o�[���A�E�g */
#define imRefShortError	M_BIT4													/* ���t�@�����X��R�V���[�g */
#define imThShortError	M_BIT5													/* �T�[�~�X�^�V���[�g */
#if (swSensorCom == imEnable)
#define imComTmpError	M_BIT6													/* �Z���T�ԒʐM�G���[ */
#define imSnsError		M_BIT7													/* �Z���T�@��ُ� */
#endif
#define imBurnShort			0x3CU												/* �o�[���A�E�g�A�V���[�g�}�X�N */
#define imBurnShortComSns	0xFCU												/* �o�[���A�E�g�A�V���[�g�}�X�N�A�Z���T�G���[ */
#define imTmpOverUnder		0x03U												/* �I�[�o�[�����W�A�A���_�[�����W�}�X�N */
#define imOvUnBrnShrt		0x3FU												/* �I�[�o�[�����W�A�A���_�[�����W�A�o�[���A�E�g�A�V���[�g�}�X�N */
#define imOvUnBrnShrtComSns	0xFFU												/* �I�[�o�[�����W�A�A���_�[�����W�A�o�[���A�E�g�A�V���[�g�A�Z���T�G���[�}�X�N */


/* �\���͈́i�����W�Ɋ֌W�j */
#define	imTmpDw_InThModel		-300		/* �����T�[�~�X�^�F-30.0�� */
#define	imTmpUp_InThModel		550			/* 55.0�� */
#define	imTmpDw_ExThModel		-500		/* �O�t���T�[�~�X�^�F-50.0�� */
#define	imTmpUp_ExThModel		700			/* 70.0�� */

#define	imTMPDw_PtModel			-2050		/* ���x�����l_-205.0�� */
#define	imTMPUp_PtModel			2050		/* ���x����l_205.0�� */
#define	imHUMDw_HumTmpModel		0			/* ���x�l�ŏ��l_0.0%rh */
#define	imHUMUp_HumTmpModel		1000		/* ���x�l�ő�l_100.0%rh */
#define	imTMPDw_HumTmpModel		-250		/* ���x�����l_-25.0�� */
#define	imTMPUp_HumTmpModel		650			/* ���x����l_65.0�� */

#define	imTMPDw_VolModel		-50			/* �d�������l_-0.050Vdc */
#define	imTMPUp_VolModel		8050		/* �d������l_8.050Vcd */
#define	imTMPDw_PulseModel		0			/* �p���X�����l_0cnt */
#define	imTMPUp_PulseModel		8000		/* �p���X����l_8000cnt */

/* �����x����ُ� */
#define	imComUp_HumTmpModel		32767		/* ���x����ŏ���I�[�o�[ */
#define	imComDw_HumTmpModel		-32767		/* ���x����ŉ����I�[�o�[ */
#define	imComDummy_HumTmpModel	-32765		/* ���x����Ŗ����l */

/* �Z���T���j�b�g����ُ� */
#define	imComUp_Sensor			32767		/* ����I�[�o�[ */
#define	imComDw_Sensor			-32767		/* �����I�[�o�[ */
#define	imComHard_Sensor		-32765		/* �n�[�h�ُ� */
#define	imComBurn_Sensor		32766		/* �o�[���A�E�g */

#if 0
/* Pt����ُ� */
#define	imComBurn_PtModel		32766		/* �o�[���A�E�g */
#define	imComUp_PtModel			32767		/* ����I�[�o�[ */
#define	imComDw_PtModel			-32767		/* �����I�[�o�[ */
#define	imComHard_PtModel		-32765		/* �n�[�h�ُ� */

/* �d������ُ� */
#define	imComUp_VolModel		32767		/* �d������ŏ���I�[�o�[ */
#define	imComDw_VolModel		-32767		/* �d������ŉ����I�[�o�[ */
#define imComHard_VolModel		-32765		/* �n�[�h�ُ� */

/* �p���X����ُ� */
#define	imComUp_PulseModel		32767		/* �p���X����ŏ���I�[�o�[ */
#define	imComDw_PulseModel		-32767		/* �p���X����ŉ����I�[�o�[ */
#define imComHard_PulseModel	-32765		/* �n�[�h�ُ� */
#endif

/* �x��ݒ�͈� */
#define	imAlmValDw_InThModel	-250		/* �����T�[�~�X�^�F-25.0�� */
#define	imAlmValUp_InThModel	500			/* 50.0�� */
#define	imAlmValDw_ExThModel	-400		/* �O�t���T�[�~�X�^�F-40.0�� */
#define	imAlmValUp_ExThModel	600			/* 60.0�� */

#define	imAlmValDw_PtModel		-1999		/* Pt_-199.9�� */
#define	imAlmValUp_PtModel		1999		/* 199.9�� */
#define	imAlmValDw_HumTmpModel_Tmp	-200	/* �����x�F-20.0�� */
#define	imAlmValUp_HumTmpModel_Tmp	600		/* 60.0�� */
#define	imAlmValDw_HumTmpModel_Hum	0		/* �����x�F0.0%RH */
#define	imAlmValUp_HumTmpModel_Hum	1000	/* 100.0%RH */

#define	imAlmValDw_VolModel 	 0		/* �d��_0.0Vdc */
#define	imAlmValUp_VolModel 	 8000		/* �d��_8.000Vdc */
#define	imAlmValDw_PulseModel	 0		/* �p���X_0cnt */
#define	imAlmValUp_PulseModel	 8000		/* �p���X_8000cnt */

/* ����l�v���X�I�t�Z�b�g */
#define imSndOffset_2100	2100
#define imSndOffset_100		100


/* ���M����l�G���[��� */
#define imSndTmpInit		-2100				/* -2100:�����l */
#define imSndTmpBurn		-2099				/* -2099:�o�[���A�E�g */
#define imSndTmpShort		-2098				/* -2098:�V���[�g */
#define imSndTmpOver		-2097				/* -2097:�I�[�o�[�����W */
#define imSndTmpUnder		-2096				/* -2096:�A���_�[�����W */
#define imSndSnsErr			-2093				/* -2093:�Z���T�G���[ */


/*
 ***************************************
 *	�x��
 ***************************************
 */
#define imAllAlmNum		4U					/* �S�x��(UU/U/L/LL) */


/* ��E�E�x�񔭐��r�b�g�Ǘ� */
#define imAlmFlgLL	M_BIT0						/* 0bit:������ */
#define imAlmFlgL	M_BIT1						/* 1bit:���� */
#define imAlmFlgH	M_BIT2						/* 2bit:��� */
#define imAlmFlgHH	M_BIT3						/* 3bit:���� */
#define imAlmFlgMsk	0x0FU						/* �t���O�}�X�N */


/* �x���E�N���A�_�@ */
//#define	imAlmClrPoint_Time		M_BIT0
//#define	imAlmClrPoint_FlgCng	M_BIT1


/*
 ***************************************
 *	�@��ُ�
 ***************************************
 */
/* �ُ��� */
#define	imAbnSts_BatEmpty	M_BIT0				/* 0bit:�d�r�c�ʋ� */
#define	imAbnSts_RF			M_BIT1				/* 1bit:����IC�̏� */
#define	imAbnSts_Dummy2		M_BIT2				/* 2bit:���g�p */
#define	imAbnSts_PMEM		M_BIT3				/* 3bit:�v���C�}���������̏� */
#define	imAbnSts_SMEM		M_BIT4				/* 4bit:�Z�J���_���������̏� */
#define	imAbnSts_ROM		M_BIT5				/* 5bit:ROM�̏� */
#define	imAbnSts_Dummy3		M_BIT6				/* 6bit:���g�p */
#define	imAbnSts_CLK		M_BIT7				/* 7bit:�N���b�N�̏� */
#define	imAbnSts_KEY		M_BIT8				/* 8bit:�L�[�̏� */
#define	imAbnSts_RTC		M_BIT9				/* 9bit:�O�t��RTC�̏� */
#define	imAbnSts_INFLSH		M_BIT10				/* 10bit:����Flash�̏� */
#define	imAbnSts_INRTC		M_BIT11				/* 11bit:����RTC�̏� */
#define	imAbnSts_BatLow		M_BIT12				/* 12bit:�d�r�c�ʒቺ */


/* ����n�ُ��� */
#define	imAbnSts_OvRngCh1	M_BIT0				/* 0bit:�I�[�o�[�����WCH1 */
#define	imAbnSts_OvRngCh2	M_BIT1				/* 1bit:�I�[�o�[�����WCH2 */
#define	imAbnSts_OvRngCh3	M_BIT2				/* 2bit:�I�[�o�[�����WCH3 */

#define	imAbnSts_UdRngCh1	M_BIT3				/* 3bit:�A���_�[�����WCH1 */
#define	imAbnSts_UdRngCh2	M_BIT4				/* 4bit:�A���_�[�����WCH2 */
#define	imAbnSts_UdRngCh3	M_BIT5				/* 5bit:�A���_�[�����WCH3 */

#define	imAbnSts_BurnCh1	M_BIT6				/* 6bit:�o�[���A�E�gCH1 */
#define	imAbnSts_BurnCh2	M_BIT7				/* 7bit:�o�[���A�E�gCH2 */
#define	imAbnSts_BurnCh3	M_BIT8				/* 8bit:�o�[���A�E�gCH3 */

#define	imAbnSts_ShortCh1	M_BIT9				/* 9bit:�V���[�gCH1 */
#define	imAbnSts_ShortCh2	M_BIT10				/* 10bit:�V���[�gCH2 */
#define	imAbnSts_ShortCh3	M_BIT11				/* 11bit:�V���[�gCH3 */

#define	imAbnSts_SnsrComErr	M_BIT12				/* 12bit:�Z���T�ڑ��ُ� */
#define	imAbnSts_CalErr		M_BIT13				/* 13bit:�Z���G���[ */
#define	imAbnSts_SnsrErr	M_BIT14				/* 14bit:�Z���T�@��ُ� */

#define	imAbnSts_OvUdBurnShortErrCh1	0x7249U
#define	imAbnSts_OvUdBurnShortCh2		0x3492U
#define	imAbnSts_OvUdBurnShortCh3		0x0924U


/* �@��ُ�`�F�b�N�X�e�[�^�X */
#define	imErrChk_Sleep		0U					/* �����l */
#define	imErrChk_KeyChk		M_BIT0				/* 0bit:�L�[�f�f���s */
#define	imErrChk_CycChk		M_BIT1				/* 1bit:�N���b�N�f�f���s */


/*
 ***************************************
 *	�O�t��Flash�`�b�v�Z���N�g�|�[�g
 ***************************************
 */
#define	P_CS_PRIM		(P12_bit.no5)				/* �`�b�v�Z���N�g(�v���C�}��) */
#define	P_CS_SECOND		(P6_bit.no1)				/* �`�b�v�Z���N�g(�Z�J���_��) */


/*
 ***************************************
 *	����Flash�A�h���X(Modbus�Ŏg�p)
 ***************************************
 */
//#define	imInFlashAdr_FirstAcsChk		0x0000U
//#define	imInFlashAdr_OpeCompanyID		0x0002U
//#define	imInFlashAdr_UniqueID			0x0003U
//#define	imInFlashAdr_SerialNo			0x0006U
//#define	imInFlashAdr_RfFreqOffset		0x000AU
//#define	imInFlashAdr_UpDate				0x000CU
//#define	imInFlashAdr_CalDate			0x000FU

/*
 ***************************************
 *	����
 ***************************************
 */
#define	imRfRTMeasDataTx_Meas1		0U
#define	imRfRTMeasDataTx_Meas2		1U

#define	imCareerFlgOff		2U
#define	imCarrerFlgOn		1U
#define	imRssiWrite			0U
#define	imRssiRead			1U
#define	imCarrerFlgWrite	2U
#define	imCarrerFlgRead		3U


/*
 ***************************************
 *	���̑�
 ***************************************
 */
#define imLoggerNameNum		20U		/* ���K�[���̃T�C�Y: 20byte */

/*
 ***************************************
 *	�t�@�[���A�b�v
 ***************************************
 */
#define	imFirmUp_FlashWrTopAddr		0xC0000U			/* �O�t���t���b�V���֏������ރA�h���X�̐擪�ʒu */




#endif