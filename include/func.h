/*
 *******************************************************************************
 *	File name	:	func.h
 *
 *	[���e]
 *		�֐��̊O���Q��(extern)��`
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2019
 *******************************************************************************
 */
#ifndef	INCLUDED_FUNC_H
#define	INCLUDED_FUNC_H


#include "typedef.h"															/* �f�[�^�^��` */
#include "struct.h"																/* �\���̒�` */
#include "switch.h"
#include "Lcd_enum.h"															/* LCD�\�������p�񋓌^��` */
#include "pfdl.h"																/* Flash Data Library T04 */
#include "pfdl_types.h"
#include "RF_Struct.h"

/*
 ******************************************************************************
 *	main.c
 ******************************************************************************
 */
extern void ApiMain( void );


/*
 ******************************************************************************
 *	Adc.c
 ******************************************************************************
 */
extern void ApiAdc_Initial( void );					/* A/D�R���o�[�^���� ���������� */
extern void ApiAdc_Main( uint8_t arSel );			/* A/D�R���o�[�^���� ���C������ */
//extern uint16_t ApiAdc_AdCntGet( void );			/* A/D�J�E���g�l�擾 */
//extern void ApiAdc_AdStart( void );					/* A/D�X�^�[�g */
//ET_BattSts_t ApiAdc_BattStsGet( void );				/* �d�r�̃X�e�[�^�X�擾 */
//extern void ApiAdc_BattStsSet( ET_BattSts_t arSts );
extern void ApiAdc_AdExe( void );
extern void ApiAdc_LowStopMotionJdg( void );			/* ��d�����̓����~���菈�� */

/*
 ******************************************************************************
 *	Bat.c
 ******************************************************************************
 */
//extern void ApiBat_Initial( void );					/* �o�b�e������ ���������� */
//extern void ApiBatChgMain( void );					/* �o�b�e���ω����̏��� */
//extern void ApiBatLowStopMotionJdg( void );			/* ��d�����̓����~���菈�� */


/*
 ******************************************************************************
 *	Modbus.c
 ******************************************************************************
 */
extern void ApiModbus_Initial( void );				/* MODBUS�ʐM ���������� */
extern void ApiModbus_Main( void );					/* MODBUS�ʐM���C�� */
#if (swSensorCom == imEnable)
extern void ApiModbus_NoReplyJudge( void );			/* �Z���T�ԒʐM���������� */
#endif


/*
 ******************************************************************************
 *	ComDrv.c
 ******************************************************************************
 */
extern void ApiComDrv_Snd( void );					/* ���M���� */
extern void ApiComDrv_Rcv( void );					/* ��M���� */
extern void ApiComDrv_SwitchTx( void );				/* ���M�؂�ւ����� */
extern void ApiComDrv_SwitchRx( void );				/* ��M�؂�ւ����� */


/*
 ******************************************************************************
 *	key.c
 ******************************************************************************
 */
extern void ApiKey_Initial( void );					/* �L�[���͏��� ���������� */
extern void ApiKey( void );							/* �L�[���͏��� */


/*
 ******************************************************************************
 *	Flash.c
 ******************************************************************************
 */
extern void ApiFlash_Initial( void );				/* �t���b�V������ ���������� */
extern void ApiFlash_Main( void );					/* �t���b�V���������C�� */

/* �ُ���̏������ݏ��� */
extern void ApiFlash_WriteErrInfo( ET_ErrInfItm_t arItem, uint8_t arAlmFlg );
/* �v���x��̏������ݏ��� */
extern void ApiFlash_WriteMeasAlm( uint32_t arTime, uint8_t arAlmFlg, uint8_t arAlmChannel, uint8_t arLevel, sint16_t arMeasVal, ET_MeasAlmItm_t arItem );
/* �v���l1/�C�x���g�̏������ݏ��� */
extern void ApiFlash_WriteMeasVal1Event( uint8_t arSel );
/* ���엚���̏������ݏ��� */
extern void ApiFlash_WriteActLog( ET_ActLogItm_t arItem, uint32_t arData, uint16_t arUserId );
/* �V�X�e�����O�̏������ݏ��� */
extern void ApiFlash_WriteSysLog( ET_SysLogItm_t arItem, uint32_t arData );
/* �v���l1�A�v���l2�A�V�X�e�����O�̃A�h���X�e�[�u���̏������ݏ��� */
extern void ApiFlash_WriteFlasAdrTbl( uint32_t arEndadr, uint16_t arNum, uint16_t arIndex, uint8_t arKind );


extern ET_Error_t ApiFlash_ReadQueSet( uint32_t arStartTime, uint32_t arEndTime, uint16_t arIndexNum, uint8_t arKosu, ET_RegionKind_t arRegionKind, uint8_t arRfMode );

extern void ApiSetFlashSts( ET_FlashSts_t arSts );				/* �O�t���t���b�V����ԃZ�b�g */
extern ET_FlashSts_t ApiGetFlashSts( void );					/* �O�t���t���b�V����Ԏ擾���� */

extern uint8_t ApiFlash_GetQueSts( void );						/* �f�[�^�L���[�̃f�[�^�L�����擾 */
//extern uint8_t ApiFlash_GetQueLowBatSts( void );				/* ��d���p�̃f�[�^�L���[�̃f�[�^�L�����擾 */

extern uint8_t ApiSetFlash_ReqRdFlg( uint8_t arSelect, ET_RegionKind_t arRegion );
extern void ApiFlash_FinReadData( void );						/* Flash���烊�[�h�������������𔻒f���� */

/* �����t���b�V���ւ̗̈��ۑ��f�[�^�擾 */
extern void ApiFlash_GetRegionData( ST_FlashRegionData_t *parRegionData, ET_RegionKind_t arRegionKind );
/* �����t���b�V���ւ̗̈��ۑ��f�[�^�Z�b�g */
extern void ApiFlash_SetRegionData( ST_FlashRegionData_t *parRegionData, ET_RegionKind_t arRegionKind );

extern void ApiFlash_SrchFlashAdr( void );						/* �d����������Flash�������݂̑������������� */

/* Flash���烊�[�h�����v���l1���Z�b�g���ĕԂ� */
extern void ApiFlash_RfRTMeasDataGet( ST_FlashVal1Evt_t arRfRtMeasData[] );
extern void ApiFlash_RfRTMeasAlmDataGet( ST_FlashMeasAlm_t arRfRtMeasAlmData[] );

extern void ApiFlash_FlashPowerCtl( uint8_t arSel, uint8_t arWait );	/* Flash�d���|�[�gON/OFF */
extern ET_Error_t ApiFlash_GetEmptyQueueJdg( void );					/* �󂫃f�[�^�L���[�̔��� */

extern void ApiFlash_QueActHist( ET_ActLogItm_t arItem, uint32_t arNowVal, uint32_t arSetVal, uint8_t arUser );
extern void ApiFlash_StoreActHist( void );
extern void ApiFlash_RfRTAbnStatusSet( void );
extern void ApiFlash_RfRTAlarmFlagSet( uint8_t arAlmFlg[], uint8_t arDevFlg[], uint8_t arSetFlg[] );
extern void ApiFlash_ReadQueSetPastTime( void );
extern void ApiFlash_SetNewMeasVal( void );

/*
 ******************************************************************************
 *	FlashDrv.c
 ******************************************************************************
 */
extern void ApiFlashDrv_Initial( void );			/* �t���b�V���h���C�o���� ���������� */

/* �p���[�_�E���R�}���h���M */
extern ET_Error_t ApiFlashDrv_PowerDown( ET_FlashKind_t arKind );
/* �p���[�_�E���J���R�}���h���M */
extern ET_Error_t ApiFlashDrv_ReleasePowerDown( ET_FlashKind_t arKind );

/* �X�e�[�^�X���W�X�^1�ǂݏo�� */
extern ET_Error_t ApiFlashDrv_ReadStatusReg1( uint8_t *parRegData, ET_FlashKind_t arKind );

/* �f�[�^�ǂݏo�� */
extern ET_Error_t ApiFlashDrv_ReadData( uint32_t arAddress, uint8_t arDataTable[], uint16_t arNum, ET_FlashKind_t arKind );
/* �f�[�^�������� */
extern ET_Error_t ApiFlashDrv_WriteMemory( uint32_t arAddress, const uint8_t arDataTable[], uint16_t arNum, ET_FlashKind_t arKind );
/* �Z�N�^���� */
extern ET_Error_t ApiFlashDrv_SectorErase( uint32_t arAddress, ET_FlashKind_t arKind );
/* �u���b�N���� */
//extern ET_Error_t ApiFlashDrv_BlockErase( uint32_t arAddress, ET_FlashKind_t arKind );
/* �`�b�v�C���[�X���M */
extern ET_Error_t ApiFlashDrv_ChipErase( ET_FlashKind_t arKind );
/* �����Z�b�g&���Z�b�g */
extern ET_Error_t ApiFlashDrv_Reset( ET_FlashKind_t arKind );
extern void ApiFlashDrv_CSCtl( ET_FlashKind_t arKind, uint8_t arCtl );

extern ET_Error_t ApiFlashDrv_FirmMemErase( ET_FlashKind_t arKind );
#if (swKouteiMode == imEnable)
extern ET_Error_t ApiFlashDrv_TestFlash( ET_FlashKind_t arKind );	/* �O�t��Flash���쌟�� */
#endif

/*
 ******************************************************************************
 *	HmiMain.c
 ******************************************************************************
 */
//extern void ApiHmi_initial( void );					/* ���[�U�C���^�[�t�F�[�X ���������� */
extern void ApiHmi_Main( void );					/* ���[�U�C���^�[�t�F�[�X�������C�� */
extern uint8_t ApiHmi_BellMarkClrDsp( void );			/* �x���}�[�N�N���A */


/*
 ******************************************************************************
 *	InitParameter.c
 ******************************************************************************
 */
extern void InitParameter( void );					/* �ʃp�����[�^���������� */


/*
 ******************************************************************************
 *	Lcd.c
 ******************************************************************************
 */
extern void ApiLcd_Initial( void );												/* LCD�\������������ */
extern void ApiLcd_Main( void );												/* LCD�\�����C������ */
extern void ApiLcd_SegDsp( ET_LcdSeg_t arLcdSeg, uint8_t arOnOff );				/* �Z�O�����g�\�� */

/* ��i��7Seg���l�\�� */
extern void ApiLcd_Upp7SegNumDsp( sint16_t arNum, uint8_t arDp1, uint8_t arDp2, uint8_t minus1 );
/* ���i��7Seg���l�\�� */
extern void ApiLcd_Low7SegNumDsp( sint16_t arNum, uint8_t arDp );
/* ���i��7Seg���l���������\��(�d���p���X) */
extern void ApiLcd_Low7SegVPDsp( sint16_t arNum, uint8_t arflg );
/* ��i/���i7Seg�����\�� */
extern void ApiLcd_UppLow7SegDsp( const char_t arTextTbl[], uint8_t arSelect );
/* �����\�� */
extern void ApiLcd_TimeDsp( uint8_t arHour, uint8_t arMinute );
/* �S�_��/�S����/��Z�O�����g�_��/�����Z�O�����g�_������ */
extern void ApiLcd_SegSelectOnOff( ET_DispSegSelect_t arSelect );
/* ��i���I�[�o�[�t���[/�A���_�[�t���[�����\�� */
extern void ApiLcd_Upp7SegOvrUdrDsp( uint8_t arSelect );
extern void ApiLcd_Low7SegOvrUdrDsp( uint8_t arSelect );

/* LCD����m�F */
extern void ApiLcd_LcdDebug( void );

extern void ApiLcd_LcdStop( void );												/* LCD�h���C�o��~���� */
//extern void ApiLcd_LcdStart( void );											/* LCD�h���C�o�J�n���� */
//extern void ApiLcd_FirmUpDisp( void );											/* �t�@�[���A�b�v�\������ */


/*
 ******************************************************************************
 *	Meas.c
 ******************************************************************************
 */
extern void ApiMeas_Initial( void );				/* ���菈�� ���������� */
extern void ApiMeas_Main( void );					/* ���菈�����C�� */
extern void ApiMeas_MeasFin( void );				/* ���芮�������� */


/*
 ******************************************************************************
 *	RfDrv.c
 ******************************************************************************
 */
extern void ApiRFDrv_ParamInitial( void );										/* �����ʐM���� �p�����[�^���������� */
extern void ApiRFDrv_Initial( void );											/* �����ʐM���� ���������� */
extern void ApiRFDrv_Main( void );												/* �����ʐM�������C������ */
extern ET_RFDrvInitSts_t ApiRFDrv_GetInitSts( void );							/* Rf(�����ʐM) �������X�e�[�^�X�擾���� */
extern uint16_t ApiRFDrv_GetSleepLoop( void );
//extern uint8_t ApiRFDrv_BroadLoop( uint8_t arKind );
extern ET_RFDrvInitSts_t ApiRFDrv_GetInitSts( void );
extern void ApiSX1272Reset( uint8_t arRstExe );
extern void ApiRfDrv_SetMeaAlmValArray( sint16_t *arFromAlmParmArray, uint8_t *arToAlmParmArray );	/* �����p�v���g�R���ɑ���l�A�x��l���������� */
#if (swKouteiMode == imEnable)
extern void ApiRfDrv_SetCh( uint16_t arVal, ET_SelRfCh_t arSel );	/* �������M�`���l���ݒ� */
#endif
#if (swKouteiMode == imEnable)
extern uint16_t ApiRfDrv_GetCh( ET_SelRfCh_t arSel );				/* �������M�`���l���ǂݏo�� */
#endif
extern uint8_t ApiRfDrv_GetRtConInfo( void );						/* ���A���^�C���ڑ����ǂݏo�� */
extern uint8_t ApiRfDrv_HsStsDisp( uint8_t arSel );
extern uint8_t ApiRfDrv_GetRssi( void );
#if (swKouteiMode == imEnable)
extern uint8_t ApiRfDrv_GetSetRssiCareerFlg( uint8_t arSel, uint8_t arVal );		/* ������M�e�X�g��RSSI�A�L�����A�Z���X����̓ǂݏ��� */
#endif
extern void ApiRfDrv_MakeReadQue( ET_RegionKind_t arRegionKind, uint32_t arReqStartTime, uint16_t arReqIndexNo );

extern int8_t ApiRfDrv_Temp( void );					/* ����IC���牷�x�擾 */
extern void ApiRfDrv_BattOffInt( void );

extern void ApiRfDrv_SetReqIndexNo( uint16_t wkQueIndex );

extern void ApiRfDrv_ForcedSleep( void );				/* �d�r�����̖�����~���� */
extern void ApiRfDrv_MeasData_StorageRfBuff( ST_FlashVal1Evt_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_MeasAlm_StorageRfBuff( ST_FlashMeasAlm_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_MeasErr_StorageRfBuff( ST_FlashErrInfo_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_MeasEvt_StorageRfBuff( ST_FlashVal1Evt_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_ActLog_StorageRfBuff( ST_FlashActLog_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_SysLog_StorageRfBuff( ST_FlashSysLog_t *arRdData, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex );
extern void ApiRfDrv_GrID_Chg( void );
extern uint16_t ApiRfDrv_RtStbyOnOff( uint8_t arSel );


/*
 ******************************************************************************
 *	RtcDrv.c
 ******************************************************************************
 */
extern void ApiRtcDrv_Initial( void );								/* RTC(S-35390A) ���������� */
//extern void ApiRtcDrv_RegInitial( void );							/* RTC(S-35390A) ���W�X�^���������� */

//extern void ApiRtcDrv_1HzOut( uint8_t arSelect );					/* RTC(S-35390A) 1Hz�o�͏��� */
//extern uint8_t ApiRtcDrv_1HzOutRead( void );						/* RTC(S-35390A) 1Hz�o�͐��䃊�[�h */

extern void ApiRtcDrv_InRtcToExRtc( void );							/* ����RTC�̎������O�t��RTC�ɏ������ޏ��� */
extern void ApiRtcDrv_ExRtcToInRtc( void );							/* �O�t��RTC�̎��������RTC�ɏ������ޏ��� */
//extern ST_RTC_t ApiRtcDrv_ReadInRtc( void );						/* ����RTC�������[�h */
extern void ApiRtcDrv_ReadInRtc( void );							/* ����RTC�������[�h */
extern void ApiRtcDrv_SetInRtc( ST_RTC_t arClock );					/* ����RTC�̎��������[�h����RAM�Ɋi�[���鏈�� */
extern uint32_t ApiRtcDrv_mktime( ST_RTC_t arClock );				/* �o�ߕb���Z�o */
extern ST_RTC_t ApiRtcDrv_localtime( uint32_t arTotalSec );			/* �o�ߕb�����玞���f�[�^�֕ϊ� */


/*
 ******************************************************************************
 *	Sleep.c
 ******************************************************************************
 */
//extern void ApiSleep_Initial( void );				/* �X���[�v���� ���������� */
extern void ApiSleep_Main( void );					/* �X���[�v�������C�� */


/*
 ******************************************************************************
 *	TimeComp.c
 ******************************************************************************
 */
extern void ApiRtcInt( void );							/* RTC�����ݏ��� */
//extern void ApiTimeComp_Initial( void );				/* �����␳���� ���������� */
extern void ApiTimeComp_first( uint32_t arRfClock, uint8_t arSize );	/* ���������Ɠ���RTC�̎������r */
extern uint8_t ApiIntMeasExist( uint32_t arSec );		/* ���^�f�[�^�Ԃ̎����������^�����̐ݒ�ƈ�v���邩�𔻒� */
extern void ApiTimeComp_TimeCompFlgClr( void );			/* ���v�␳�t���O�̖��� */
extern void ApiTimeComp_LimitRtcClock( void );
extern uint32_t ApiTimeComp_GetLocalTime( void );


/*
 ******************************************************************************
 *	Alarm.c
 ******************************************************************************
 */
extern void ApiAlarm_Initial( void );						/* �x�񏈗� ���������� */
extern void ApiAlarm_Main( void );							/* �x�񏈗� ���C������ */
extern uint8_t ApiAlarm_ReadMeasAlmFlg( void );

/*
 ******************************************************************************
 *	Abnormal.c
 ******************************************************************************
 */
//extern void ApiAbn_Initial( void );								/* �@��ُ폈�� ���������� */
extern uint16_t ApiAbn_AbnStsGet( ST_AbnInfKind_t arKind );		/* �@��ُ��Ԏ擾 */
extern uint8_t ApiAbn_AbnNumGet( void );						/* ���߂Ŕ��������@��ُ�No.���擾 */
extern void ApiAbn_AbnNumSet( void );

/* �@��ُ�Z�b�g */
extern void ApiAbn_AbnStsSet( uint16_t arAbnSts, ST_AbnInfKind_t arKind );
/* �@��ُ�N���A */
extern void ApiAbn_AbnStsClr( uint16_t arAbnSts, ST_AbnInfKind_t arKind );

extern void ApiAbn_Chk( void );				/* �n�[�h�G���[�̊m�F */

extern void ApiAbn_ChkCRC( void );			/* ROM CRC���Z���� */


/*
 ******************************************************************************
 *	FirmUpMain.c
 ******************************************************************************
 */
//extern void ApiFirmUpdateInit( void );										/* �t�@�[���A�b�v�f�[�g���������� */
extern void ApiFirmUpdateMain( void );											/* �t�@�[���A�b�v�f�[�g���C������ */
extern void ApiFirmUpdate_History( void );


/*
 ******************************************************************************
 *	InFlash.c
 ******************************************************************************
 */
extern void ApiInFlash_PwrOnRead( void );											/* �����t���b�V������ �N�����ǂݏo������ */
extern void ApiInFlash_ParmWrite( ET_InFlashWrSts_t arSelect, uint16_t arAddr );	/* ����Flash�Ƀp�����[�^�����C�g */
extern uint8_t ApiInFlash_Read( uint16_t arAddr );									/* ����Flash����p�����[�^�����[�h */
extern void ApiInFlash_ParmInitFlgChk( void );										/* �H��o�׏������̃t���O�m�F */

extern void ApiInFlash_SetValWrite( ST_RF_Logger_SettingWt_t *arRfBuff );
extern void ApiInFlash_RtSetValWrite1( ST_RF_RT_ChangeSet_Prm1_t *arRfBuff );
extern void ApiInFlash_RtSetValWrite2( ST_RF_RT_ChangeSet_Prm2_t *arRfBuff );
extern void ApiInFlash_RtSetValWrite3( ST_RF_RT_ChangeSet_Prm3_t *arRfBuff );
extern uint16_t ApiInFlash_AlmVal_ValtoCnt( sint16_t arAlmVal );
extern uint8_t ApiInFlash_OftVal_ValtoCnt( sint8_t arOftVal );
extern uint16_t ApiInFlash_ScaleVal_ValtoCnt( sint16_t arScaleVal );

extern uint16_t ApiInFlash_1ByteToBig2Byte( uint8_t *parRfData );
extern uint16_t ApiInFlash_1ByteToLittle2Byte( uint8_t *parRfData );

/*
 ******************************************************************************
 *	�����t���b�V���������h���C�o����
 ******************************************************************************
 */
extern pfdl_status_t ApiInFlashDrv_Open( void );								/* �����t���b�V���h���C�o �J�n���� */
extern void ApiInFlashDrv_Close( void );										/* �����t���b�V���h���C�o �I������ */

/* �����t���b�V���h���C�o �ǂݏo���R�}���h���M���� */
extern pfdl_status_t ApiInFlashDrv_RdCmdSnd( uint16_t arRdAddr, uint16_t arRdSize, uint8_t *parRdData );
/* �����t���b�V���h���C�o �������݃R�}���h���M���� */
extern pfdl_status_t ApiInFlashDrv_WrCmdSnd( uint16_t arWrAddr, uint16_t arWrSize, uint8_t *parWrData );
/* �����t���b�V���h���C�o �u�����N�`�F�b�N�R�}���h���M���� */
extern pfdl_status_t ApiInFlashDrv_BlnkChkCmdSnd( uint16_t arStrtAddr, uint16_t arExRng );
/* �����t���b�V���h���C�o �u���b�N�����R�}���h���M���� */
extern pfdl_status_t ApiInFlashDrv_BlkErsCmdSnd( uint16_t arBlkNo );
/* �����t���b�V���h���C�o �x���t�@�C�R�}���h���M���� */
extern pfdl_status_t ApiInFlashDrv_VerifyCmdSnd( uint16_t arStrtAddr, uint16_t arExRng );
/* �����t���b�V���h���C�o �n���h���R�}���h���M���� */
extern pfdl_status_t ApiInFlashDrv_HandlerCmdSnd( void );


/*
 ******************************************************************************
 *	r_cg_tau_user.c
 ******************************************************************************
 */
extern uint32_t ApiTau0_GetTau0Ch1Cnt( void );			/* �^�C�}0�`���l��1�̃J�E���^�擾 */
extern uint16_t ApiTau0_GetTau0Ch0Time( void );
extern void ApiTau0_GetTau0Ch0TimeClr( void );
extern void ApiTau0_WaitTimer( uint16_t arCnt );

/*
 ******************************************************************************
 *	r_cg_rtc_user.c
 ******************************************************************************
 */
extern void ApiRtc1HzCtl( uint8_t arCtl );				/* �T�u�N���b�N�ɂ��1Hz�o�͐��� */
extern uint8_t ApiRtc1HzCtlRead( void );				/* �T�u�N���b�N�ɂ��1Hz�o�͐����ԃ��[�h */


#endif																			/* INCLUDED_FUNC_H */
