/*
 *******************************************************************************
 *	File name	:	InitParameter.c
 *
 *	[���e]
 *		�ʃp�����[�^����������
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2017.11.19		Softex N.I		�V�K�쐬
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


#pragma section text MY_APP
/*
 *******************************************************************************
 *	�ʃp�����[�^����������
 *
 *	[���e]
 *		�ʃp�����[�^�̏������������s���B
 *******************************************************************************
 */
void InitParameter( void )
{
	ApiKey_Initial();															/* �L�[���͏��� ���������� */

	ApiAdc_Initial();															/* A/D�R���o�[�^���� ���������� */

//	ApiBat_Initial();															/* �o�b�e������ ���������� */

#if (swSensorCom == imEnable)
	ApiModbus_Initial();														/* MODBUS�ʐM ���������� */
#endif

//	ApiHmi_initial();															/* ���[�U�C���^�[�t�F�[�X ���������� */

	ApiLcd_Initial();															/* LCD�\�� ���������� */

	ApiMeas_Initial();															/* ���菈�� ���������� */

//	ApiSleep_Initial();															/* �X���[�v���� ���������� */

	ApiFlashDrv_Initial();														/* �t���b�V���h���C�o���� ���������� */

	ApiFlash_Initial();															/* �t���b�V������ ���������� */

	ApiRFDrv_ParamInitial();													/* �����ʐM���� �p�����[�^���������� */

//	ApiTimeComp_Initial();														/* �����␳���� ���������� */

	ApiAlarm_Initial();															/* �x�񏈗� ���������� */

	R_INTC5_Start();															/* �d�r�c�ʒቺ���荞�݋��� */

//	ApiFirmUpdateInit();														/* �t�@�[���A�b�v�f�[�g���� ���������� */
	
}
#pragma section text MY_APP
