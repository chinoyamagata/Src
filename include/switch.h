/*
 *******************************************************************************
 *	File name	:	switch.h
 *
 *	[���e]
 *		�R���p�C���X�C�b�`��`
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2017.11.08		Softex N.I		�V�K�쐬
 *******************************************************************************
 */
#ifndef	INCLUDED_SWITCH_H
#define	INCLUDED_SWITCH_H

#define swDebugLcd			imDisable		/* LCD�_���m�F�i����:imEnable/�֎~:imDisable�j */

#define	swLoggerBord		imEnable		/* ���������؂�ւ��AimEnable�FLogger�AimDisable�FGW */

#define swRfTxTest			imDisable

#define swRfTestLcd			imDisable		/* �����ʐM�^�C�~���ODebug�A�^�C�~���O�␳�p���x�l�\�� */

#define swRfRxTest			imDisable

#define	swRfPortDebug		imDisable		/* �������Ƀ|�[�g�o��:�f�o�b�O�p */

#define swRssiLogDebug		imEnable		/* ��i��GWID�A���i��RSSI�\���ACH2��RSSI�ACH3��GWID���^ */

#define swRESFDisp			imDisable		/* ���Z�b�g����RESF��\�� */

#define swKouteiMode		imDisable		/* �H���p�̋@�\ */

#define swRfOffMode 		imEnable		/* RF OFF���[�h�ł̍����ʐM�L������ Enable�Ŗ���(�܂�SOFF) */

#define swAdCntLog			imDisable		/* ���^1ch�ɖ�������AD�J�E���g�l�����^ */

#define swSensorCom			imEnable		/* �Z���T�ԒʐM */

#define swRTmodeSetting		imEnable

#endif