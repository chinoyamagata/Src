/*
 *******************************************************************************
 *	File name	:	FlashDrv.c
 *
 *	[���e]
 *		�O�t���t���b�V���������h���C�o����
 *	[�K��]
 *		winbond�� W25Q80DV
 *		��1Mbyte
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2017.12.22		Softex N.I		�V�K�쐬
 *******************************************************************************
 */
#include "cpu_sfr.h"															/* SFR��`�w�b�_�[ */

#include "typedef.h"															/* �f�[�^�^��` */
#include "UserMacro.h"															/* ���[�U�[�쐬�}�N����` */
#include "immediate.h"															/* immediate�萔��` */
#include "switch.h"																/* �R���p�C���X�C�b�`��` */
#include "enum.h"																/* �񋓌^�̒�` */
#include "struct.h"																/* �\���̒�` */
#include "func.h"																/* �֐��̊O���Q��(extern)��` */
#include "glLabel.h"															/* �O���[�o���ϐ��E�萔(const)���x����` */

#include <string.h>

/*
 *==============================================================================
 *	define��`
 *==============================================================================
 */
/* �R�}���h */
#define	imFlashDrv_WREN		0x06U												/* Write Enable */
#define	imFlashDrv_RDSR1	0x05U												/* Read Status Register-1 */
#define	imFlashDrv_RDDT		0x03U												/* Read Data */
#define	imFlashDrv_PGPR		0x02U												/* Page Program */
#define	imFlashDrv_SCER		0x20U												/* Sector Erase */
#define	imFlashDrv_BLER		0xD8U												/* Block Erase */
#define	imFlashDrv_CSER		0xC7U												/* Chip Erase */
#define	imFlashDrv_ENRE		0x66U												/* Enable Reset */
#define	imFlashDrv_REST		0x99U												/* Reset */
#define	imFlashDrv_PDWN		0xB9U												/* Power-down */
#define	imFlashDrv_RPDWN	0xABU												/* Release Power-down */


#define	imFlashDrv_FlashSize	0x100000U										/* Flash�̑��T�C�Y */
#define	imFlashDrv_MaxAddr		0xFFFFFU										/* �A�h���X�̍ő�l */
#define	imFlashDrv_DummySnd		0x00U											/* �_�~�[���M�f�[�^ */
#define	imFlashDrv_MaxWrNum		256U											/* �������݃f�[�^�o�C�g���̍ő�l */

#define imReadStatusReg1CmdNum	2U												/* �X�e�[�^�X���W�X�^1�ǂݏo���R�}���h�� */
#define imWriteMemoryCmdNum		260U											/* �f�[�^�������݃R�}���h�� */
#define imReadDataCmdNum		260U											/* �f�[�^�ǂݏo���R�}���h�� */
#define imSectorEraseCmdNum		4U												/* �Z�N�^�����R�}���h�� */
#define imBlockEraseCmdNum		4U												/* �u���b�N�����R�}���h�� */

/* �����I���`�b�v�I�V���[�^�֕ύX */
#define imRetryTimerMax			24000U											/* ���g���C1ms�҂�(1�X�e�[�g�F1/24MHz��41.67ns�A1ms��41.67ns��24000��) */
#define imBlErTimerMax			24000000U										/* �u���b�N����1s�҂�(1�X�e�[�g�F1/24MHz��41.67ns�A1s��41.67ns��24000000��) */
#define imCSErTimerMax			144000000U										/* �`�b�v����6s�҂�(1�X�e�[�g�F1/24MHz��41.67ns�A6s��41.67ns��144000000��) */
#define imResetWaitTm			720U											/* ���Z�b�g30��s�҂�(1�X�e�[�g�F1/24MHz��41.67ns�A30��s��41.67ns��720��) */

#define	imFlashPageSize		0x100U

/*
 *==============================================================================
 *	�\���̒�`
 *==============================================================================
 */



/*
 *==============================================================================
 *	�ϐ���`
 *==============================================================================
 */



/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
 */
//static ET_Error_t SubFlashDrv_WriteEnable( ET_FlashKind_t arKind );				/* �������݋���(Write Enable)�R�}���h���M */
static ET_Error_t SubFlashDrv_CmdSnd( ET_FlashKind_t arKind, uint8_t arCmd );
static ET_Error_t SubFlashDrv_EraseCmdSnd( uint32_t arAddress, ET_FlashKind_t arKind, uint8_t arCmd );
static ET_Error_t SubFlashDrv_SndFlgChk( void );


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	�t���b�V���h���C�o���� ����������
 *
 *	[���e]
 *		�t���b�V���h���C�o���� ����������
 *******************************************************************************
 */
void ApiFlashDrv_Initial( void )
{
	R_CSI10_Start();															/* CSI10�J�n */
	ApiFlashDrv_CSCtl( ecFlashKind_Prim, imLow );
	ApiFlashDrv_CSCtl( ecFlashKind_Second, imLow );
}

/*
 *******************************************************************************
 *	�p���[�_�E���R�}���h���M
 *
 *	[���e]
 *		�p���[�_�E���R�}���h�𑗐M���܂��B
 *	[����]
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_PowerDown( ET_FlashKind_t arKind )
{
	ET_Error_t	wkRet;					/* �߂�l(�G���[���) */
	
	wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_PDWN );
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�p���[�_�E���J���R�}���h���M
 *
 *	[���e]
 *		�p���[�_�E���J���R�}���h�𑗐M���܂��B
 *	[����]
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_ReleasePowerDown( ET_FlashKind_t arKind )
{
	ET_Error_t	wkRet;					/* �߂�l(�G���[���) */
	
	wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_RPDWN );
	
	return wkRet;
}

#if 0
/*
 *******************************************************************************
 *	�������݋���(Write Enable)�R�}���h���M
 *
 *	[���e]
 *		�������݋��R�}���h�𑗐M���܂��B
 *	[����]
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *******************************************************************************
 */
ET_Error_t SubFlashDrv_WriteEnable( ET_FlashKind_t arKind )
{
	ET_Error_t	wkRet;					/* �߂�l(�G���[���) */
	
	wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_WREN );
	
	return wkRet;
}
#endif


/*
 *******************************************************************************
 *	�X�e�[�^�X���W�X�^1�ǂݏo��
 *
 *	[���e]
 *		�X�e�[�^�X���W�X�^1�̓��e��ǂݏo���܂��B
 *	[����]
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_ReadStatusReg1( uint8_t *parRegData, ET_FlashKind_t arKind )
{
	ET_Error_t			wkRet;													/* �߂�l(�G���[���) */
	uint8_t 			wkSndData[imReadStatusReg1CmdNum];						/* ���M�f�[�^ */
	uint8_t				wkRcvData[imReadStatusReg1CmdNum];						/* ��M�f�[�^ */
	uint16_t 			wkSndNum;												/* ���M�R�}���h�� */
	
	/* �ϐ������� */
	wkRet = ecERR_OK;															/* �G���[��񏉊��� */
	wkSndNum = 0U;																/* ���M�R�}���h�������� */
	
	/* �`�b�v�Z���N�g��Low */
	ApiFlashDrv_CSCtl( arKind, imLow );
	
	M_NOP;																		/* 1�X�e�[�g(=1/(18.4*10^6)=54ns) */
	
	/* �R�}���h�쐬 */
	wkSndData[wkSndNum] = imFlashDrv_RDSR1;										/* Read Status Register-1 */
	wkSndNum++;
	wkSndData[wkSndNum] = imFlashDrv_DummySnd;									/* �_�~�[���M�f�[�^ */
	wkSndNum++;
	
	/* �R�}���h���M */
	gvCsi10SndEndFlg = imOFF;													/* ���M�����t���OOFF */
	R_CSI10_Send_Receive(&wkSndData[0], wkSndNum, &wkRcvData[0]);				/* ���M */
	wkRet = SubFlashDrv_SndFlgChk();											/* ���M�����t���O��ON�ɂȂ�܂� */
	
	if( wkRet == ecERR_OK )														/* �G���[�`�F�b�N */
	{
		*parRegData = wkRcvData[1];												/* ��M�f�[�^�i�[ */
	}
	
	/* �`�b�v�Z���N�g��High */
	ApiFlashDrv_CSCtl( arKind, imHigh );
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�f�[�^�ǂݏo��
 *
 *	[���e]
 *		�w�肵���A�h���X�̃f�[�^��ǂݏo���܂��B
 *	[����]
 *		uint32_t arAddress�F�A�h���X
 *		uint8_t arDataTable[]�F�f�[�^�̊i�[��e�[�u��
 *		uint16_t arNum�F�f�[�^��
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *	[���ӎ���]
 *		��x�ɓǂݏo����f�[�^��256Byte�ł��B
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_ReadData( uint32_t arAddress, uint8_t arDataTable[], uint16_t arNum, ET_FlashKind_t arKind )
{
	ET_Error_t			wkRet;													/* �߂�l(�G���[���) */
	uint16_t			wkLoop;													/* ���[�v�J�E���^ */
	uint32_t			wkAddr;													/* �A�h���X */
	ET_Error_t			wkReg1Ret;												/* �G���[��� */
	uint8_t				wkReg1Data;												/* ���W�X�^�f�[�^ */
	uint8_t 			wkSndData[imReadDataCmdNum];							/* ���M�f�[�^ */
	uint8_t				wkRcvData[imReadDataCmdNum];							/* ��M�f�[�^ */
	uint16_t 			wkSndNum;												/* ���M�R�}���h�� */
	
	/* �ϐ������� */
	wkRet = ecERR_OK;															/* �G���[��񏉊��� */
	wkSndNum = 0U;																/* ���M�R�}���h�������� */
	
	wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);				/* �X�e�[�^�X���W�X�^1�ǂݏo�� */
	
	if ((M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY))							/* BUSY */
	||	(arDataTable == M_NULL)												/* NULL */
	||	(arAddress > imFlashDrv_MaxAddr)										/* �A�h���X�w��NG */
	||	((arAddress + arNum) > (imFlashDrv_MaxAddr + 1U))						/* �f�[�^���w��NG */
	||	(wkReg1Ret == ecERR_NG)													/* �X�e�[�^�X���W�X�^1�ǂݏo��NG */
	)
	{
		wkRet = ecERR_NG;														/* �G���[���NG */
	}
	else
	{
		/* �`�b�v�Z���N�g��Low */
		ApiFlashDrv_CSCtl( arKind, imLow );
		
		M_NOP;																	/* 1�X�e�[�g(=1/(18.4*10^6)=54ns) */
		
		/* �R�}���h�쐬 */
		wkSndData[wkSndNum] = imFlashDrv_RDDT;									/* Read Data */
		wkSndNum++;
		wkAddr = arAddress;
		wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x00FF0000U) >> 16U);
		wkSndNum++;
		wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x0000FF00U) >> 8U);
		wkSndNum++;
		wkSndData[wkSndNum] = (uint8_t)(wkAddr & 0x000000FFU);
		wkSndNum++;
		for (wkLoop = 0U; wkLoop < arNum; wkLoop++)
		{
			wkSndData[wkSndNum] = imFlashDrv_DummySnd;							/* �_�~�[���M�f�[�^ */
			wkSndNum++;
		}
		
		/* �R�}���h���M */
		gvCsi10SndEndFlg = imOFF;												/* ���M�����t���OOFF */
		R_CSI10_Send_Receive(&wkSndData[0], wkSndNum, &wkRcvData[0]);			/* ���M */
		wkRet = SubFlashDrv_SndFlgChk();										/* ���M�����t���O��ON�ɂȂ�܂� */
		
		if (wkRet == ecERR_OK)													/* �G���[�`�F�b�N */
		{
			/* �ǂݏo���f�[�^�i�[ */
			for (wkLoop = 0U; wkLoop < arNum; wkLoop++)
			{
				arDataTable[wkLoop] = wkRcvData[wkLoop + 4U];					/* �f�[�^�i�[ */
			}
		}
		
		/* �`�b�v�Z���N�g��High */
		ApiFlashDrv_CSCtl( arKind, imHigh );
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�f�[�^��������
 *
 *	[���e]
 *		�w�肵���A�h���X�Ƀf�[�^���������݂܂��B
 *	[����]
 *		uint32_t arAddress�F�A�h���X
 *		const uint8_t arDataTable[]�F�������݃f�[�^�i�[�e�[�u��
 *		uint8_t arNum�F�������݃f�[�^��
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *	[���ӎ���]
 *		��x�ɏ������߂�f�[�^��256Byte�ł��B
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_WriteMemory( uint32_t arAddress, const uint8_t arDataTable[], uint16_t arNum, ET_FlashKind_t arKind )
{
	ET_Error_t			wkRet;													/* �߂�l(�G���[���) */
	uint16_t			wkLoop;													/* ���[�v�J�E���^ */
	uint32_t			wkAddr;													/* �A�h���X */
	ET_Error_t			wkReg1Ret;												/* �G���[��� */
	uint8_t				wkReg1Data;												/* ���W�X�^�f�[�^ */
	uint8_t 			wkSndData[imWriteMemoryCmdNum];							/* ���M�f�[�^ */
	uint8_t				wkRcvData[imWriteMemoryCmdNum];							/* ��M�f�[�^ */
	uint16_t 			wkSndNum;												/* ���M�R�}���h�� */
	uint16_t			wkRetryTimer;											/* ���g���C�^�C�}�[ */
	uint32_t			wkPage1;
	uint32_t			wkPage2;
	uint8_t				wkPageCnt;
	uint32_t			wkVal;
	uint8_t				wkLoop2;
	uint16_t			wkNum;
	uint16_t			wkNum1;
	
	/* �ϐ������� */
	wkRet = ecERR_OK;															/* �G���[��񏉊��� */
	wkSndNum = 0U;																/* ���M�R�}���h�������� */
	wkRetryTimer = 0U;															/* ���g���C�^�C�}�[������ */
	
	wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);				/* �X�e�[�^�X���W�X�^1�ǂݏo�� */
	
	if ((M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY))							/* Busy */
	||	(arDataTable == M_NULL)													/* NULL */
	||	(arAddress > imFlashDrv_MaxAddr)										/* �A�h���X�w��NG */
	||	(arNum > imFlashDrv_MaxWrNum)											/* �������݃f�[�^���w��NG */
	||	((arAddress + arNum) > (imFlashDrv_MaxAddr + 1U))						/* �������݃f�[�^���w��NG */
	||	(wkReg1Ret == ecERR_NG)													/* �X�e�[�^�X���W�X�^1�ǂݏo��NG */
	)
	{
		wkRet = ecERR_NG;														/* �G���[���NG */
	}
	else
	{
		wkPage1 = arAddress / imFlashPageSize;
		wkPage2 = ( arAddress + arNum ) / imFlashPageSize;
		if( wkPage1 != wkPage2 )
		{
			wkPageCnt = 2U;
			wkPage2 *= imFlashPageSize;
			wkVal = wkPage2 - arAddress;
			wkNum = (uint16_t)wkVal;
			wkNum1 = wkNum;
		}
		else
		{
			wkPageCnt = 1U;
			wkNum = arNum;
		}
		wkAddr = arAddress;
		
		for( wkLoop2 = 0U; wkLoop2 < wkPageCnt; wkLoop2++ )
		{
#if 0
			wkRet = SubFlashDrv_WriteEnable(arKind);								/* �������݋���(Write Enable)�R�}���h���M */
#else
			wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_WREN );					/* �������݋���(Write Enable)�R�}���h���M */
#endif
			
			/* �`�b�v�Z���N�g��Low */
			ApiFlashDrv_CSCtl( arKind, imLow );
			
			M_NOP;																	/* 1�X�e�[�g(=1/(18.4*10^6)=54ns) */
			
			if( wkLoop2 == 1U )
			{
				wkAddr = wkPage2;
				wkNum = arNum - wkNum1;
				wkSndNum = 0U;
			}
			/* �R�}���h�쐬 */
			wkSndData[wkSndNum] = imFlashDrv_PGPR;									/* Page Program */
			wkSndNum++;
			wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x00FF0000U) >> 16U);
			wkSndNum++;
			wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x0000FF00U) >> 8U);
			wkSndNum++;
			wkSndData[wkSndNum] = (uint8_t)(wkAddr & 0x000000FFU);
			wkSndNum++;
			for( wkLoop = 0U; wkLoop < wkNum; wkLoop++ )
			{
				if( wkLoop2 == 1U )
				{
					wkSndData[wkSndNum] = arDataTable[ wkLoop + wkNum1 ];			/* ���M�f�[�^ */
				}
				else
				{
					wkSndData[wkSndNum] = arDataTable[ wkLoop ];					/* ���M�f�[�^ */
				}
				wkSndNum++;
			}
			
			/* �R�}���h���M */
			gvCsi10SndEndFlg = imOFF;												/* ���M�����t���OOFF */
			R_CSI10_Send_Receive(&wkSndData[0], wkSndNum, &wkRcvData[0]);			/* ���M */
			wkRet = SubFlashDrv_SndFlgChk();										/* ���M�����t���O��ON�ɂȂ�܂� */
			
			/* �`�b�v�Z���N�g��High */
			ApiFlashDrv_CSCtl( arKind, imHigh );
			
			/* busy��0�ɂȂ�܂ő҂� */
			while( 1 )
			{
				wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);		/* �X�e�[�^�X���W�X�^1�ǂݏo�� */
				if( !M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY) )
				{
					M_NOP;
					break;
				}
				if( wkRetryTimer >= imRetryTimerMax )								/* 1m���߂����ꍇ */
				{
					wkRet = ecERR_NG;												/* �G���[���NG */
					break;
				}
				wkRetryTimer++;														/* ���g���C�^�C�}�[�X�V */
			}
		}
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�Z�N�^����
 *
 *	[���e]
 *		�w�肵���Z�N�^�̏������s���܂��B
 *	[����]
 *		uint32_t arAddress�F�A�h���X
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
*******************************************************************************
 */
ET_Error_t ApiFlashDrv_SectorErase( uint32_t arAddress, ET_FlashKind_t arKind )
{
	ET_Error_t	wkRet;						/* �߂�l(�G���[���) */
	
	wkRet = SubFlashDrv_EraseCmdSnd( arAddress, arKind, imFlashDrv_SCER );
	
	return wkRet;
}

#if 0
/*
 *******************************************************************************
 *	�u���b�N����
 *
 *	[���e]
 *		�w�肵���u���b�N(64KB)�̏������s���܂��B
 *	[����]
 *		uint32_t arAddress�F�A�h���X
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_BlockErase( uint32_t arAddress, ET_FlashKind_t arKind )
{
	ET_Error_t	wkRet;						/* �߂�l(�G���[���) */
	
	wkRet = SubFlashDrv_EraseCmdSnd( arAddress, arKind, imFlashDrv_BLER );
	
	return wkRet;
}
#endif

/*
 *******************************************************************************
 *	�`�b�v�C���[�X���M
 *
 *	[���e]
 *		�`�b�v�C���[�X�R�}���h�𑗐M���܂��B
 *	[����]
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_ChipErase( ET_FlashKind_t arKind )
{
	ET_Error_t			wkRet;													/* �߂�l(�G���[���) */
	ET_Error_t			wkReg1Ret;												/* �G���[��� */
	uint8_t				wkReg1Data;												/* ���W�X�^�f�[�^ */
	uint8_t 			wkSndData;												/* ���M�f�[�^ */
	uint8_t				wkRcvData;												/* ��M�f�[�^ */
	uint16_t 			wkSndNum;												/* ���M�R�}���h�� */
	uint32_t			wkRetryTimer;											/* ���g���C�^�C�}�[ */
	
	/* �ϐ������� */
	wkRet = ecERR_OK;															/* �G���[��񏉊��� */
	wkSndNum = 0U;																/* ���M�R�}���h�������� */
	wkRetryTimer = 0U;															/* ���g���C�^�C�}�[������ */
	
	wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);				/* �X�e�[�^�X���W�X�^1�ǂݏo�� */
	
	if( (M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY))							/* Busy = 1 */
	||	(wkReg1Ret == ecERR_NG)													/* �X�e�[�^�X���W�X�^1�ǂݏo��NG */
	)
	{
		wkRet = ecERR_NG;														/* �G���[���NG */
	}
	else
	{
#if 0
		wkRet = SubFlashDrv_WriteEnable(arKind);								/* �������݋���(Write Enable)�R�}���h���M */
#else
		wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_WREN );					/* �������݋���(Write Enable)�R�}���h���M */
#endif
		
		/* �`�b�v�Z���N�g��Low */
		ApiFlashDrv_CSCtl( arKind, imLow );
		
		M_NOP;																	/* 1�X�e�[�g(=1/(18.4*10^6)=54ns) */
		
		/* �R�}���h�쐬 */
		wkSndData = imFlashDrv_CSER;
		wkSndNum++;
		
		/* �R�}���h���M */
		gvCsi10SndEndFlg = imOFF;												/* ���M�����t���OOFF */
		R_CSI10_Send_Receive(&wkSndData, wkSndNum, &wkRcvData);					/* ���M */
		wkRet = SubFlashDrv_SndFlgChk();										/* ���M�����t���O��ON�ɂȂ�܂� */
		
		/* �`�b�v�Z���N�g��High */
		ApiFlashDrv_CSCtl( arKind, imHigh );
		
		/* busy��0�ɂȂ�܂ő҂� */
		while( 1 )
		{
			wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);		/* �X�e�[�^�X���W�X�^1�ǂݏo�� */
			if( !M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY) )
			{
				M_NOP;
				break;
			}
			if( wkRetryTimer >= imCSErTimerMax )								/* 6s���߂����ꍇ */
			{
				wkRet = ecERR_NG;												/* �G���[���NG */
				break;
			}
			wkRetryTimer++;														/* ���g���C�^�C�}�[�X�V */
			
			R_WDT_Restart();
		}
	}
	
	return wkRet;
}


/*
 ***************************************************************************************
 *	�����Z�b�g(66h)�����Z�b�g(99h)�R�}���h���M
 *
 *	[���e]
 *		�i�s���̓�������͏I�����A�f�o�C�X�̓f�t�H���g�̃p���[�I����Ԃɖ߂�A
 *		�������X�e�[�^�X���W�X�^�r�b�g�A���C�g�C�l�[�u�����b�`�iWEL�j�X�e�[�^�X�Ȃǂ�
 *		���݂̂��ׂĂ̊������ݒ�������B
 *	[����]
 *		uint32_t arAddress�F�A�h���X
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *	[���ӎ���]
 *		�u�����Z�b�g(66h)�v�R�}���h�̌�Ɂu���Z�b�g(99h)�v�ȊO�̃R�}���h���g�p����ƁA
 *		�u�����Z�b�g�v��Ԃ������ƂȂ�ׁA���ӁB
 ***************************************************************************************
 */
ET_Error_t ApiFlashDrv_Reset( ET_FlashKind_t arKind )
{
	uint16_t			wkLoop;													/* ���[�v�J�E���^ */
	uint8_t 			wkSndData;												/* ���M�f�[�^ */
	uint8_t				wkRcvData;												/* ��M�f�[�^ */
	uint16_t 			wkSndNum;												/* ���M�R�}���h�� */
	ET_Error_t	wkRet;															/* �߂�l(�G���[���) */
	
	/* �ϐ������� */
	wkSndNum = 0U;
 	
	/*** �����Z�b�g�R�}���h���M�J�n ***/
	/* �`�b�v�Z���N�g��Low */
	ApiFlashDrv_CSCtl( arKind, imLow );
	
	M_NOP;																		/* 1�X�e�[�g(=1/(18.4*10^6)=54ns) */
	
	/* �R�}���h�쐬 */
	wkSndData = imFlashDrv_ENRE;												/* Enable Reset */
	wkSndNum++;
	
	/* �R�}���h���M */
	gvCsi10SndEndFlg = imOFF;													/* ���M�����t���OOFF */
	R_CSI10_Send_Receive(&wkSndData, wkSndNum, &wkRcvData);						/* ���M */
	wkRet = SubFlashDrv_SndFlgChk();											/* ���M�����t���O��ON�ɂȂ�܂� */
	
	/* �`�b�v�Z���N�g��High */
	ApiFlashDrv_CSCtl( arKind, imHigh );
	
	/*** �����Z�b�g�R�}���h���M�I�� ***/
	
	
	for (wkLoop = 0U; wkLoop < imResetWaitTm; wkLoop++)
	{
		M_NOP;
	}
	
	/*** ���Z�b�g�R�}���h���M�J�n ***/
	/* �`�b�v�Z���N�g��Low */
	ApiFlashDrv_CSCtl( arKind, imLow );
	
	M_NOP;																		/* 1�X�e�[�g(=1/(18.4*10^6)=54ns) */
	
	/* �R�}���h�쐬 */
	wkSndData = imFlashDrv_REST;												/*	Reset */
	wkSndNum++;
	
	/* �R�}���h���M */
	gvCsi10SndEndFlg = imOFF;													/* ���M�����t���OOFF */
	R_CSI10_Send_Receive(&wkSndData, wkSndNum, &wkRcvData);						/* ���M */
	wkRet = SubFlashDrv_SndFlgChk();											/* ���M�����t���O��ON�ɂȂ�܂� */
	
	/* �`�b�v�Z���N�g��High */
	ApiFlashDrv_CSCtl( arKind, imHigh );
	
	/*** �����Z�b�g�R�}���h���M�I�� ***/
	
	return wkRet;
}




/*
 *******************************************************************************
 *	�R�}���h���M
 *
 *	[���e]
 *		�R�}���h�𑗐M���܂��B
 *	[����]
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *		uint8_t arCmd: ���M�R�}���h
 *******************************************************************************
 */
static ET_Error_t SubFlashDrv_CmdSnd( ET_FlashKind_t arKind, uint8_t arCmd )
{
	uint8_t 	wkSndData;														/* ���M�f�[�^ */
	uint8_t 	wkRcvData;														/* ��M�f�[�^ */
	uint16_t 	wkSndNum;														/* ���M�R�}���h�� */
	ET_Error_t	wkRet;															/* �߂�l(�G���[���) */
	
	/* �ϐ������� */
	wkRet = ecERR_OK;															/* �G���[��񏉊��� */
	wkSndNum = 0U;																/* ���M�R�}���h�������� */
	
	/* �`�b�v�Z���N�g��Low */
	ApiFlashDrv_CSCtl( arKind, imLow );
	
	M_NOP;																		/* 1�X�e�[�g(=1/(18.4*10^6)=54ns) */
	
	/* �R�}���h�쐬 */
	wkSndData = arCmd;
	wkSndNum++;
	
	/* �R�}���h���M */
	gvCsi10SndEndFlg = imOFF;													/* ���M�����t���OOFF */
	R_CSI10_Send_Receive(&wkSndData, wkSndNum, &wkRcvData);						/* ���M */
	wkRet = SubFlashDrv_SndFlgChk();											/* ���M�����t���O��ON�ɂȂ�܂� */
	
	/* �`�b�v�Z���N�g��High */
	ApiFlashDrv_CSCtl( arKind, imHigh );
	
	return wkRet;
}

/*
 *******************************************************************************
 *	�C���[�X�R�}���h���M
 *
 *	[���e]
 *		�C���[�X�R�}���h�𑗐M���܂��B
 *	[����]
 *		uint32_t	arAddress: �C���[�X�̃A�h���X
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *		uint8_t arCmd: �C���[�X���M�R�}���h
 *******************************************************************************
 */
static ET_Error_t SubFlashDrv_EraseCmdSnd( uint32_t arAddress, ET_FlashKind_t arKind, uint8_t arCmd )
{
	ET_Error_t			wkRet;													/* �߂�l(�G���[���) */
	uint32_t			wkAddr;													/* �A�h���X */
	ET_Error_t			wkReg1Ret;												/* �G���[��� */
	uint8_t				wkReg1Data;												/* ���W�X�^�f�[�^ */
	uint8_t 			wkSndData[imSectorEraseCmdNum];							/* ���M�f�[�^ */
	uint8_t				wkRcvData[imSectorEraseCmdNum];							/* ��M�f�[�^ */
	uint16_t 			wkSndNum;												/* ���M�R�}���h�� */
	
	/* �ϐ������� */
	wkRet = ecERR_OK;															/* �G���[��񏉊��� */
	wkSndNum = 0U;																/* ���M�R�}���h�������� */
	
	wkReg1Ret = ApiFlashDrv_ReadStatusReg1(&wkReg1Data, arKind);				/* �X�e�[�^�X���W�X�^1�ǂݏo�� */
	
	if( (M_TSTBIT(wkReg1Data, imFlashDrv_Sts1Bit_BUSY))							/* Busy = 1 */
	||	(arAddress > imFlashDrv_MaxAddr)										/* �A�h���X�w��NG */
	||	(wkReg1Ret == ecERR_NG)													/* �X�e�[�^�X���W�X�^1�ǂݏo��NG */
	)
	{
		wkRet = ecERR_NG;														/* �G���[���NG */
	}
	else
	{
#if 0
		wkRet = SubFlashDrv_WriteEnable(arKind);								/* �������݋���(Write Enable)�R�}���h���M */
#else
		wkRet = SubFlashDrv_CmdSnd( arKind, imFlashDrv_WREN );					/* �������݋���(Write Enable)�R�}���h���M */
#endif
		
		/* �`�b�v�Z���N�g��Low */
		ApiFlashDrv_CSCtl( arKind, imLow );
		
		M_NOP;																	/* 1�X�e�[�g(=1/(18.4*10^6)=54ns) */
		
		/* �R�}���h�쐬 */
		wkSndData[wkSndNum] = arCmd;
		wkSndNum++;
		wkAddr = arAddress;
		wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x00FF0000U) >> 16U);
		wkSndNum++;
		wkSndData[wkSndNum] = (uint8_t)((wkAddr & 0x0000FF00U) >> 8U);
		wkSndNum++;
		wkSndData[wkSndNum] = (uint8_t)(wkAddr & 0x000000FFU);
		wkSndNum++;
		
		/* �R�}���h���M */
		gvCsi10SndEndFlg = imOFF;												/* ���M�����t���OOFF */
		R_CSI10_Send_Receive(&wkSndData[0], wkSndNum, &wkRcvData[0]);			/* ���M */
		wkRet = SubFlashDrv_SndFlgChk();										/* ���M�����t���O��ON�ɂȂ�܂� */
		
		/* �`�b�v�Z���N�g��High */
		ApiFlashDrv_CSCtl( arKind, imHigh );
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�`�b�v�Z���N�g����
 *
 *	[���e]
 *		�`�b�v�Z���N�g�𐧌䂷��
 *	[����]
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *******************************************************************************
 */
void ApiFlashDrv_CSCtl( ET_FlashKind_t arKind, uint8_t arCtl )
{
	uint16_t	wkLoop;
	
	/* �`�b�v�Z���N�g��High */
	if( arCtl == imHigh )
	{
		/* �Ώۂ̃t���b�V�����v���C�}�� */
		if( arKind == ecFlashKind_Prim )
		{
			P_CS_PRIM = (uint8_t)imHigh;				/* �`�b�v�Z���N�g(�v���C�}��)��High */
		}
		else
		{
			P_CS_SECOND = (uint8_t)imHigh;				/* �`�b�v�Z���N�g(�Z�J���_��)��High */
			for( wkLoop = 0U ; wkLoop < 85U; wkLoop++ )
			{
				M_NOP;												/* 1�X�e�[�g(=1/(18.4*10^6)=54ns, 54ns * 926 = 50us) */
			}
		}
	}
	/* �`�b�v�Z���N�g��Low */
	else
	{
		/* �Ώۂ̃t���b�V�����v���C�}�� */
		if( arKind == ecFlashKind_Prim )
		{
			P_CS_PRIM = (uint8_t)imLow;					/* �`�b�v�Z���N�g(�v���C�}��)��Low */
		}
		else
		{
			P_CS_SECOND = (uint8_t)imLow;				/* �`�b�v�Z���N�g(�Z�J���_��)��Low */
		}
	}
}

/*
 *******************************************************************************
 *	�O�t��Flash�ʐM���M�t���O�Ď�
 *
 *	[���e]
 *		�O�t��Flash�Ƃ̒ʐM���̑��M�t���O�Ď�
 *	[����]
 *		�Ȃ�
 *	[�߂�l]
 *		ET_Error_t	wkRet: �G���[���
 *******************************************************************************
 */
static ET_Error_t SubFlashDrv_SndFlgChk( void )
{
	uint16_t	wkErrJdgCnt = 0U;
	ET_Error_t	wkRet = ecERR_OK;
	
	while( gvCsi10SndEndFlg == imOFF )										/* ���M�����t���O��ON�ɂȂ�܂� */
	{
		wkErrJdgCnt++;
		if( wkErrJdgCnt > imRetryTimerMax )				/* 1ms���߂ŊO�t��Flash�ւ̃A�N�Z�X���s */
		{
			wkRet = ecERR_NG;												/* RTC�ُ� */
			break;
		}
	}
	return wkRet;
}


/*
 *******************************************************************************
 *	�t�@�[���̈�C���[�X
 *
 *	[���e]
 *		�O�t��Flash�̃t�@�[���i�[�̈����������
 *	[����]
 *		ET_FlashKind_t arKind�F�t���b�V�����������
 *								ecFlashKind_Prim�F�v���C�}��
 *								ecFlashKind_Second�F�Z�J���_��
 *	[�߂�l]
 *		ET_Error_t	wkRet: �G���[���
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_FirmMemErase( ET_FlashKind_t arKind )
{
	uint8_t		wkLoop;
	uint8_t		wkReg1Data;
	uint16_t	wkCnt;
	uint32_t	wkAdr;
	ET_Error_t	wkRet;
	
	/* Flash�d��ON */
//	ApiFlash_FlashPowerCtl( imON, imON );
	
	/* Block�C���[�X */
	for( wkLoop = 0U, wkAdr = 0xC0000U ; wkLoop < 4U ; wkLoop++, wkAdr += 0x10000U )
	{
//		wkRet = ApiFlashDrv_BlockErase( wkAdr, arKind );
		wkRet = SubFlashDrv_EraseCmdSnd( wkAdr, arKind, imFlashDrv_BLER );
		
		wkCnt = 0;
		do
		{
			/* �X�e�[�^�X���W�X�^1�ǂݏo�� */
			wkRet = ApiFlashDrv_ReadStatusReg1( &wkReg1Data, arKind );
			
			wkCnt++;
			if( wkCnt == 0xFFFF )
			{
				wkRet = ecERR_NG;
				break;
			}
		}
		while( M_TSTBIT( wkReg1Data, imFlashDrv_Sts1Bit_BUSY ) );
	}
	
	/* Flash�d��OFF */
//	ApiFlash_FlashPowerCtl( imOFF, imON );
	
	return wkRet;
}

#if (swKouteiMode == imEnable)
/*
 *******************************************************************************
 *	�O�t��Flash���쌟��
 *
 *	[���e]
 *		�O�t��Flash�̑S�̈�̓������������
 *			�l�����C�g���A���[�h���Ĕ�r����
 *	[����]
 *		 ET_FlashKind_t arKind�F�t���b�V�����������
 *						ecFlashKind_Prim�F�v���C�}��
 *						ecFlashKind_Second�F�Z�J���_��
 *	[�߂�l]
 *		ET_Error_t	wkRet: �G���[���
 *******************************************************************************
 */
ET_Error_t ApiFlashDrv_TestFlash( ET_FlashKind_t arKind )
{
	uint32_t		wkLoop;
	ET_Error_t		wkRet;
	int16_t			wkCmpRet;
	uint8_t			wkRdData[ imFlashDrv_MaxWrNum ];
	uint8_t			wkWrData[ imFlashDrv_MaxWrNum ];
	
	for( wkLoop = 0 ; wkLoop < imFlashDrv_MaxWrNum ; wkLoop++ ){
		wkWrData[ wkLoop ] = wkLoop;
	}
	
	wkRet = ApiFlashDrv_ChipErase( arKind );
	
	if( wkRet == ecERR_OK )
	{
		for( wkLoop = 0 ; wkLoop < imFlashDrv_FlashSize/imFlashDrv_MaxWrNum ; wkLoop++ ){
			R_WDT_Restart();
			wkRet = ApiFlashDrv_WriteMemory( (wkLoop * imFlashDrv_MaxWrNum), &wkWrData[ 0U ], imFlashDrv_MaxWrNum, arKind );
			if( wkRet == ecERR_OK )
			{
				wkRet = ApiFlashDrv_ReadData( (wkLoop * imFlashDrv_MaxWrNum), &wkRdData[ 0U ], imFlashDrv_MaxWrNum, arKind );
				if( wkRet == ecERR_OK )
				{
					wkCmpRet = memcmp( &wkWrData[ 0U ], &wkRdData[ 0U ], imFlashDrv_MaxWrNum );
					if( wkCmpRet != 0U )
					{
						wkRet = ecERR_NG;
					}
				}
			}
			if( wkRet == ecERR_NG )
			{
				break;
			}
		}
	}
	
	ApiFlashDrv_ChipErase( arKind );
	
	return wkRet;
}
#endif

#pragma section
