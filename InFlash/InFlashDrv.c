/*
 *******************************************************************************
 *	File name	:	InFlashDrv.c
 *
 *	[���e]
 *		�����t���b�V���h���C�o����
 *	[�K��]
 *		�u���b�N0~3(�e�u���b�N1KB)
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2018.11.07		Softex K.U		�V�K�쐬
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

#include "pfdl.h"																/* Flash Data Library T04 */
#include "pfdl_types.h"

/*
 *==============================================================================
 *	define��`
 *==============================================================================
 */


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

#pragma section text MY_APP2
/*
 *******************************************************************************
 *	�����t���b�V���h���C�o �J�n����
 *
 *	[���e]
 *		�����t���b�V���h���C�o �J�n����
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_Open( void )
{
	pfdl_descriptor_t	vInit;
	pfdl_status_t		wkResult;												/* �߂�l */
	
	vInit.fx_MHz_u08 = 24; 														/* CPU frequency : 24MHz */
	vInit.wide_voltage_mode_u08 = 0;											/* Voltage mode : full speed mode */ 
	
	NOP();
	wkResult = PFDL_Open(&vInit);
	NOP();
	
	return wkResult;
}


/*
 *******************************************************************************
 *	�����t���b�V���h���C�o �I������
 *
 *	[���e]
 *		�����t���b�V���h���C�o �I������
 *******************************************************************************
 */
void ApiInFlashDrv_Close( void )
{
	PFDL_Close();
}


/*
 *******************************************************************************
 *	�����t���b�V���h���C�o �ǂݏo���R�}���h���M����
 *
 *	[���e]
 *		�����t���b�V���h���C�o �ǂݏo���R�}���h���M����
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_RdCmdSnd( uint16_t arRdAddr, uint16_t arRdSize, uint8_t *parRdData )
{
	pfdl_request_t	vInFlashDrvPrm;
	pfdl_status_t	wkResult;												/* �߂�l */

	/* �ǂݏo���R�}���h������ */
	vInFlashDrvPrm.index_u16 = arRdAddr;										/* �ǂݍ��݊J�n�A�h���X */
	vInFlashDrvPrm.bytecount_u16 = arRdSize;									/* �ǂݍ��݃T�C�Y */
	vInFlashDrvPrm.data_pu08 = parRdData;										/* �ǂݍ��݃f�[�^���̓o�b�t�@�̃A�h���X */
	vInFlashDrvPrm.command_enu = PFDL_CMD_READ_BYTES;							/* �ǂݏo���R�}���h */

	wkResult = PFDL_Execute(&vInFlashDrvPrm);

	return wkResult;
}


/*
 *******************************************************************************
 *	�����t���b�V���h���C�o �������݃R�}���h���M����
 *
 *	[���e]
 *		�����t���b�V���h���C�o �������݃R�}���h���M����
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_WrCmdSnd( uint16_t arWrAddr, uint16_t arWrSize, uint8_t *parWrData )
{
	pfdl_request_t	vInFlashDrvPrm;
	pfdl_status_t	wkResult;													/* �߂�l */

	/* �ǂݏo���R�}���h������ */
	vInFlashDrvPrm.index_u16 = arWrAddr;										/* �������݊J�n�A�h���X */
	vInFlashDrvPrm.bytecount_u16 = arWrSize;									/* �������݃T�C�Y */
	vInFlashDrvPrm.data_pu08 = parWrData;										/* �������݃f�[�^���̓o�b�t�@�̃A�h���X */
	vInFlashDrvPrm.command_enu = PFDL_CMD_WRITE_BYTES;							/* �������݃R�}���h */

	wkResult = PFDL_Execute(&vInFlashDrvPrm);

	return wkResult;
}


/*
 *******************************************************************************
 *	�����t���b�V���h���C�o �u�����N�`�F�b�N�R�}���h���M����
 *
 *	[���e]
 *		�����t���b�V���h���C�o �u�����N�`�F�b�N�R�}���h���M����
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_BlnkChkCmdSnd( uint16_t arStrtAddr, uint16_t arExRng )
{
	pfdl_request_t	vInFlashDrvPrm;
	pfdl_status_t	wkResult;													/* �߂�l */

	/* �ǂݏo���R�}���h������(�s�v�p�����[�^�ɂ�0��ݒ肷��) */
	vInFlashDrvPrm.index_u16 = arStrtAddr;										/* �u�����N�`�F�b�N�J�n�A�h���X */
	vInFlashDrvPrm.bytecount_u16 = arExRng;										/* �u�����N�`�F�b�N�T�C�Y */
	vInFlashDrvPrm.data_pu08 = 0;												/* �s�v�p�����[�^ */
	vInFlashDrvPrm.command_enu = PFDL_CMD_BLANKCHECK_BYTES;						/* �u�����N�`�F�b�N�R�}���h */

	wkResult = PFDL_Execute(&vInFlashDrvPrm);

	return wkResult;
}


/*
 *******************************************************************************
 *	�����t���b�V���h���C�o �u���b�N�����R�}���h���M����
 *
 *	[���e]
 *		�����t���b�V���h���C�o �u���b�N�����R�}���h���M����
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_BlkErsCmdSnd( uint16_t arBlkNo )
{
	pfdl_request_t	vInFlashDrvPrm;
	pfdl_status_t	wkResult;													/* �߂�l */

	/* �ǂݏo���R�}���h������(�s�v�p�����[�^�ɂ�0��ݒ肷��) */
	vInFlashDrvPrm.index_u16 = arBlkNo;											/* �u���b�N�ԍ� */
	vInFlashDrvPrm.bytecount_u16 = 0;											/* �s�v�p�����[�^ */
	vInFlashDrvPrm.data_pu08 = 0;												/* �s�v�p�����[�^ */
	vInFlashDrvPrm.command_enu = PFDL_CMD_ERASE_BLOCK;							/* �u���b�N�����R�}���h */

	wkResult = PFDL_Execute(&vInFlashDrvPrm);

	return wkResult;
}


/*
 *******************************************************************************
 *	�����t���b�V���h���C�o �x���t�@�C�R�}���h���M����
 *
 *	[���e]
 *		�����t���b�V���h���C�o �x���t�@�C�R�}���h���M����
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_VerifyCmdSnd( uint16_t arStrtAddr, uint16_t arExRng )
{
	pfdl_request_t	vInFlashDrvPrm;
	pfdl_status_t	wkResult;													/* �߂�l */

	/* �ǂݏo���R�}���h������(�s�v�p�����[�^�ɂ�0��ݒ肷��) */
	vInFlashDrvPrm.index_u16 = arStrtAddr;										/* �J�n�A�h���X */
	vInFlashDrvPrm.bytecount_u16 = arExRng;										/* �J�n�A�h���X����̎��s�͈� */
	vInFlashDrvPrm.data_pu08 = 0;												/* �s�v�p�����[�^ */
	vInFlashDrvPrm.command_enu = PFDL_CMD_IVERIFY_BYTES;						/* �x���t�@�C�R�}���h */

	wkResult = PFDL_Execute(&vInFlashDrvPrm);

	return wkResult;
}


/*
 *******************************************************************************
 *	�����t���b�V���h���C�o �n���h���R�}���h���M����
 *
 *	[���e]
 *		�����t���b�V���h���C�o �n���h���R�}���h���M����
 *******************************************************************************
 */
pfdl_status_t ApiInFlashDrv_HandlerCmdSnd( void )
{
	pfdl_status_t	wkResult;													/* �߂�l */

	wkResult = PFDL_Handler();

	return wkResult;
}
#pragma section


