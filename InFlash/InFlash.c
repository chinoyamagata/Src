/*
 *******************************************************************************
 *	File name	:	Flash.c
 *
 *	[���e]
 *		�����t���b�V������
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2019
 *******************************************************************************
 */
#include "cpu_sfr.h"							/* SFR��`�w�b�_�[ */
#include "typedef.h"							/* �f�[�^�^��` */
#include "UserMacro.h"							/* ���[�U�[�쐬�}�N����` */
#include "immediate.h"							/* immediate�萔��` */
#include "switch.h"								/* �R���p�C���X�C�b�`��` */
#include "enum.h"								/* �񋓌^�̒�` */
#include "struct.h"								/* �\���̒�` */
#include "func.h"								/* �֐��̊O���Q��(extern)��` */
#include "glLabel.h"							/* �O���[�o���ϐ��E�萔(const)���x����` */
#include "InFlash.h"
#include "RF_Immediate.h"

#include "pfdl.h"								/* Flash Data Library T04 */
#include "pfdl_types.h"


/*
 *==============================================================================
 *	define��`
 *==============================================================================
 */

/* ����Flash�t�@�[�X�g�A�N�Z�X�l */
#define	imFirstAcsChkVal	0xA5U

/* �u���b�NNo. */
#define	imBlock0			0U
#define	imBlock1			1U
#define	imBlock2			2U
#define	imBlock3			3U

/* �������A�h���X */
#define	imAddrStartBlk0		0x0000
#define	imAddrStartBlk1		0x0400
#define	imAddrStartBlk2		0x0800
#define	imAddrStartBlk3		0x0C00

/* �A�h���X�I�t�Z�b�g */
#define	imAddrProcessOfs	0x0000
#define	imAddrParamOfs		0x0080
#define	imAddrDataOfs		0x0190

/* �u���b�N�T�C�Y */
#define	imBlkSize			0x0400


/*
 *==============================================================================
 *	�񋓌^��`
 *==============================================================================
 */
/* ���N�G�X�g��� */
typedef enum ET_InFlashReq
{
	ecInFlashReq_Init = 0,				/* ���� */
	ecInFlashReq_Wr,					/* �������݃��N�G�X�g */
	ecInFlashReq_Rd,					/* �ǂݍ��݃��N�G�X�g */

	ecInFlashReqMax						/* enum�ő�l */
} ET_InFlashReq_t;

/* �����t���b�V����� */
typedef enum ET_InFlashSts
{
	ecInFlashSts_Init = 0,				/* ���� */
	ecInFlashSts_BlankChkPre,			/* �u�����N�`�F�b�N���� */
	ecInFlashSts_BlankChk,				/* �u�����N�`�F�b�N�� */
	ecInFlashSts_ErasePre,				/* �u���b�N�������� */
	ecInFlashSts_Erase,					/* �u���b�N������ */
	ecInFlashSts_Write,					/* �������ݒ� */
	ecInFlashSts_Verify,				/* �x���t�@�C */
	ecInFlashSts_ReadCheck,				/* ���[�h�`�F�b�N */
	ecInFlashSts_Close,					/* �N���[�Y */

	ecInFlashStsMax						/* enum�ő�l */
} ET_InFlashSts_t;

/* �ݒ�l���X�g */
typedef enum ET_SetParamList
{
	ecSetParamList_UUDevi = 0,			/* ��E�l */
	ecSetParamList_UDevi,
	ecSetParamList_LDevi,
	ecSetParamList_LLDevi,
	
	ecSetParamList_UUDelay,				/* �x���l */
	ecSetParamList_UDelay,
	ecSetParamList_LDelay,
	ecSetParamList_LLDelay,
	
	ecSetParamList_DeviEnaLv,			/* ��E���e���ԑI��臒l */
	ecSetParamList_DeviEnaTime,			/* ��E���e���� */
	
	ecSetParamList_Offset,				/* �v���l�I�t�Z�b�g */
	ecSetParamList_Slope,				/* �v���l�ꎟ�X���␳ */
	
	ecSetParamList_BatType,				/* �d�r�� */
	ecSetParamList_KeyLock,				/* �L�[���b�N */
	
	ecSetParamList_RealTimeCh,			/* ���A���^�C���Ҏ�ch */
	
	ecSetParamList_LogCyc1,				/* ���^����1 */
	ecSetParamList_LogCyc2,				/* ���^����2 */
	
	ecSetParamList_LoggerName,			/* ���K�[���� */
	
	ecSetParamList_FstConnCh,			/* �����ʐM�Ҏ�ch */
	ecSetParamList_FstConnBoot,			/* �����ʐM�N���̃��O�f�[�^臒l */
	
	ecSetParamList_AlertType,			/* �x��Ď����Z�ݒ�(���Z��) */
	ecSetParamList_AlertParm,			/* �x��Ď����Z�ݒ�(�p�����[�^) */
	ecSetParamList_AlertFlag,			/* �x��Ď����Z�ݒ�(�t���O�L�^) */
	
	ecSetParamList_EventKeyFlg,			/* �C�x���g�{�^������ */
	
	ecSetParamList_RealTimeDisConn,		/* ���A���^�C���ʐM�ؒf���莞�� */
	ecSetParamList_RealTimeSndPtn,		/* ���A���^�C���ʐM�]���n�_�f�[�^ */
	
	ecSetParamList_DeviClrPoint,		/* ��E���e���Ԃ̃N���A�_�@ */
	ecSetParamList_DeviClrTime,			/* ��E���e���Ԃ̃N���A���� */
	
	ecSetParamList_CalDate,				/* �Z���� */
	
	ecSetParamList_ScalDecimal,			/* �X�P�[�����O�����_�ʒu */
	ecSetParamList_ScalYInt,			/* �X�P�[�����O�ؕ� */
	ecSetParamList_ScalGrad,			/* �X�P�[�����O�X�� */
	
	ecSetParamList_FlashSelect,			/* �t���b�V���������ؑւ� */
	
	ecSetParamList_CnctEnaGwId,			/* �ڑ����eGW */
	
	ecSetParamList_GrId,				/* �O���[�vID */
	
	ecSetParamList_MAX
} ET_SetParamList_t;


/*
 *==============================================================================
 *	�\���̒�`
 *==============================================================================
 */
 
/* �����t���b�V�������p�����[�^ */
typedef struct ST_InFlashPrm
{
	ET_InFlashReq_t	mReqSts;			/* ���N�G�X�g��� */
	ET_InFlashSts_t	mSts;				/* ��� */
} ST_InFlashPrm_t;

/* �̈��f�[�^ */
typedef struct ST_InFlashRegionData
{
	uint32_t		mWrNum;				/* �������݌��� */
	uint32_t		mWrAddrEnd;			/* �������ݏI�[�A�h���X */
	uint32_t		mWrAddrFin;			/* �������ݍŏI�A�h���X */
} ST_InFlashRegionData_t;

/* �ݒ�l�������X�g�p */
typedef struct ST_SetValWriteLimit
{
	ET_SetParamList_t	mParam;			/* �ݒ�l�� */
	uint16_t			mMin;			/* �ݒ�ŏ��l */
	uint16_t			mMax;			/* �ݒ�ő�l */
	uint16_t			mKeep;			/* �ݒ�l�p�� */
	ET_ActLogItm_t		mLogItm;		/* ����No. */
} ST_SetValWriteLimit_t;

/* �ݒ�l�������X�g�p */
typedef struct ST_AlmSetCh
{
	ET_SensType_t		mSensr;			/* �Z���T�� */
	uint8_t				mCh;			/* �x��ݒ�CH */
} ST_AlmSetCh_t;

/*
 *==============================================================================
 *	�ϐ���`
 *==============================================================================
 */

static ST_InFlashPrm_t	vInFlashPrm = { ecInFlashReq_Init, ecInFlashSts_Init };		/* �����t���b�V�������p�����[�^ */

/*
 *==============================================================================
 *	�萔��`
 *==============================================================================
 */
/* �ݒ�l�������X�g */
const ST_SetValWriteLimit_t cSetValWriteLimitTbl[ 33U ] =
{
	/* mParam,							mMin,	mMax,	mKeep,	mLogItm */
	{ ecSetParamList_UUDevi,			0,		8191,	1,		ecActLogItm_UUDevi1			},
	{ ecSetParamList_UDevi,				0,		8191,	1,		ecActLogItm_UDevi1			},
	{ ecSetParamList_LDevi,				0,		8191,	1,		ecActLogItm_LDevi1			},
	{ ecSetParamList_LLDevi,			0,		8191,	1,		ecActLogItm_LLDevi1			},
	{ ecSetParamList_UUDelay,			0,		60,		63,		ecActLogItm_UUDelay1		},
	{ ecSetParamList_UDelay,			0,		60,		63,		ecActLogItm_UDelay1			},
	{ ecSetParamList_LDelay,			0,		60,		63,		ecActLogItm_LDelay1			},
	{ ecSetParamList_LLDelay,			0,		60,		63,		ecActLogItm_LLDelay1		},
	{ ecSetParamList_DeviEnaLv,			0,		15,		255,	ecActLogItm_DeviEnaLv1		},
	{ ecSetParamList_DeviEnaTime,		1,		4000,	0,		ecActLogItm_DeviEnaTime1	},
	{ ecSetParamList_Offset,			0,		250,	255,	ecActLogItm_Offset1			},
	{ ecSetParamList_Slope,				0,		60,		63,		ecActLogItm_Slope1			},
	{ ecSetParamList_BatType,			1,		3,		0,		ecActLogItm_BatType			},
	{ ecSetParamList_KeyLock,			0,		15,		255,	ecActLogItm_KeyLock			},
	{ ecSetParamList_RealTimeCh,		0,		25,		255,	ecActLogItm_RealTimeCh		},
	{ ecSetParamList_LogCyc1,			0,		9,		15,		ecActLogItm_LogCyc1			},
	{ ecSetParamList_LogCyc2,			0,		8,		15,		ecActLogItm_LogCyc2			},
	{ ecSetParamList_LoggerName,		0,		255,	255,	ecActLogItm_LoggerName1		},
	{ ecSetParamList_FstConnCh,			0,		4,		15,		ecActLogItm_FstConnCh		},
	{ ecSetParamList_FstConnBoot,		0,		53000,	65535,	ecActLogItm_FstConnBoot		},
	{ ecSetParamList_AlertType,			0,		4,		7,		ecActLogItm_AlertType		},
	{ ecSetParamList_AlertParm,			0,		30,		31,		ecActLogItm_AlertParm		},
	{ ecSetParamList_AlertFlag,			0,		1,		3,		ecActLogItm_AlertFlag		},
	{ ecSetParamList_EventKeyFlg,		0,		1,		3,		ecActLogItm_EventKeyFlg		},
//	{ ecSetParamList_RealTimeDisConn,	0,		50,		63		},
//	{ ecSetParamList_RealTimeSndPtn,	0,		500,	511		},
	{ ecSetParamList_DeviClrPoint,		0,		1,		3,		ecActLogItm_DeviClrPoint	},
	{ ecSetParamList_DeviClrTime,		0,		1439,	2047,	ecActLogItm_DeviClrTime		},
	{ ecSetParamList_CalDate,			0,		32767,	65535,	ecActLogItm_CalDate			},
	{ ecSetParamList_ScalDecimal,		0,		3,		15,		ecActLogItm_ScalDecimal		},
	{ ecSetParamList_ScalYInt,			0,		60000,	65535,	ecActLogItm_ScalYInt		},
	{ ecSetParamList_ScalGrad,			0,		60000,	65535,	ecActLogItm_ScalGrad		},
	{ ecSetParamList_FlashSelect,		0,		1,		15,		ecActLogItm_FlashSelect		},
	{ ecSetParamList_CnctEnaGwId,		0,		255,	255,	ecActLogItm_CnctEnaGwId1	},
	{ ecSetParamList_GrId,				1,		255,	0,		ecActLogItm_GrId			}
};

/* �x��֘A�̐ݒ�CH�� */
#if (swSensorCom == imEnable)
const ST_AlmSetCh_t cAlmSetChTbl[ 6U ] =
{
	{ ecSensType_InTh,		1U },
	{ ecSensType_ExTh,		1U },
	{ ecSensType_HumTmp,	2U },
	{ ecSensType_Pt,		1U },
	{ ecSensType_V,			1U },
	{ ecSensType_Pulse,		1U }
};
#else
const ST_AlmSetCh_t cAlmSetChTbl[ 2U ] =
{
	{ ecSensType_InTh,		1U },
	{ ecSensType_ExTh,		1U }
};
#endif
/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
 */
void SubInFlash_Write( ET_FlashKind_t arKind, ET_InFlashWrSts_t arSelect, uint16_t arAddr );
static uint16_t SubInFlash_SetValWrite_ValChk( ET_SetParamList_t arParam, uint16_t arSetVal, uint16_t arNowVal, uint8_t arUser, uint8_t arLoop );
static sint16_t SubInFlash_AlmVal_CnttoVal( uint16_t arAlmValCnt );
static sint8_t SubInFlash_OftVal_CnttoVal( uint8_t arOftValCnt );
static sint16_t SubInFlash_ScaleVal_CnttoVal( uint16_t arScaleValCnt );
static sint16_t SubInFlash_UpdateAlmVal( ET_SetParamList_t arParmList, uint8_t *parSetVal, sint16_t arNowVal, uint8_t arUserID, uint8_t arLoop );
static uint8_t SubInFlash_UpdateAlmDelay( ET_SetParamList_t arParmList, uint8_t *parSetVal, uint8_t arNowVal, uint8_t arUserID, uint8_t arLoop );
void SubInFlash_SetLoggerName( uint8_t *ptrLoggerName );
void SubInFlash_SetCommGwID( uint8_t (*ptrCommGwID)[3] );
void SubInFlash_SetScale( uint8_t *ptrOffset, uint8_t *ptrZerospan );

static void SubInFlash_SetValTime( void );


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	�����t���b�V������ �N�����ǂݏo������
 *
 *	[���e]
 *		�N�����A�����t���b�V������ݒ�l��ǂݏo���ARAM�ɓW�J����B
 *******************************************************************************
 */
void ApiInFlash_PwrOnRead( void )
{
	uint8_t			wkRdData;					/* ���[�h�f�[�^ */
	uint8_t			*pwkData;					/* RAM�i�[�A�h���X */
	uint8_t			wkLoop;
	uint8_t			wkLoopTbl;
	uint8_t			wkRetryCnt;
	uint16_t		wkStrtAddr;					/* �J�n�A�h���X */
	uint16_t		wkEndAddr;					/* �I���A�h���X */
	
	uint8_t			wkAcsChk[ 2U ] = { 0xFFU, 0xFFU };
	
	
	/* �����t���b�V���h���C�o�J�n */
	if( PFDL_OK == ApiInFlashDrv_Open() )
	{
		/* ����Flash�t�@�[�X�g�A�N�Z�X�`�F�b�N */
		if( PFDL_OK == ApiInFlashDrv_RdCmdSnd( imAddrStartBlk0, 1U, &wkRdData ) )
		{
			wkAcsChk[ 0U ] = wkRdData;
		}
		if( PFDL_OK == ApiInFlashDrv_RdCmdSnd( imAddrStartBlk3, 1U, &wkRdData ) )
		{
			wkAcsChk[ 1U ] = wkRdData;
		}
		
		/* �t�@�[�X�g�A�N�Z�X���� */
		if( wkAcsChk[ 0U ] == imFirstAcsChkVal || wkAcsChk[ 1U ] == imFirstAcsChkVal )
		{
			/* ����Flash����f�[�^���[�h��RAM�Ɋi�[ */
			for( wkLoop = 0U ; wkLoop < 2U ; wkLoop++ )
			{
				for( wkLoopTbl = 0U ; wkLoopTbl < 3U ; wkLoopTbl++ )
				{
					if( wkLoop == 0U )
					{
						wkStrtAddr = imAddrStartBlk0;
					}
					else
					{
						wkStrtAddr = imAddrStartBlk3;
					}
					
					switch( wkLoopTbl )
					{
						case 0U:
							wkStrtAddr += imAddrProcessOfs;
							wkEndAddr = sizeof( gvInFlash.mProcess ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mProcess;
							break;
						case 1U:
							wkStrtAddr += imAddrParamOfs;
							wkEndAddr = sizeof( gvInFlash.mParam ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mParam;
							break;
						case 2U:
							wkStrtAddr += imAddrDataOfs;
							wkEndAddr = sizeof( gvInFlash.mData ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mData;
							break;
					}
					
					for( ; wkStrtAddr < wkEndAddr; wkStrtAddr++, pwkData++ )
					{
						for( wkRetryCnt = 0U ; wkRetryCnt < 3U ; wkRetryCnt++ )
						{
							if( PFDL_OK == ApiInFlashDrv_RdCmdSnd( wkStrtAddr, 1U, &wkRdData ) )
							{
								if( wkLoop == 0U )
								{
									*pwkData = wkRdData;							/* RAM�Ɋi�[ */
									break;
								}
								else
								{
									/* �l��r */
									if( *pwkData == wkRdData )
									{
										break;
									}
								}
							}
						}
					}
				}
			}
			ApiInFlashDrv_Close();
		}
		/* �t�@�[�X�g�A�N�Z�X�Ȃ� */
		else
		{
			ApiInFlashDrv_Close();
			
			/* �H���v���Z�X�A�ݒ�l�������l�Ń��C�g */
			ApiInFlash_ParmWrite( ecInFlashWrSts_Process, imAddrStartBlk0 );
		}
	}
}


/*
 *******************************************************************************
 *	����Flash�Ƀp�����[�^�����C�g
 *
 *	[����]
 *		ET_InFlashWrSts_t	arSelect	�F�ݒ�l�������A�H��+�ݒ�l�������A�����_RAM
 *	[���e]
 *		����Flash�Ƀp�����[�^�����C�g����
 *******************************************************************************
 */
void ApiInFlash_ParmWrite( ET_InFlashWrSts_t arSelect, uint16_t arAddr )
{
	ET_FlashKind_t	wkKind;
	
	switch( arSelect )
	{
		/* Key or Modbus�ɂ�鏉���� */
		case ecInFlashWrSts_ParmInit:
			gvInFlash.mParam = cInFlashParmTbl;
			gvInFlash.mData = cInFlashDataTbl;
			gvInFlash.mProcess.mModelCode = 0xFFU;
			gvrfStsEventFlag.mReset = RFIC_INI;
			break;
			
		/* ��Ԃ͂��߂̓d������ */
		case ecInFlashWrSts_Process:
		case ecInFlashWrSts_Process2:
			
			/* ecInFlashWrSts_Process2: �H���p�e�[�u���͏㏑�����Ȃ� */
			if( arSelect != ecInFlashWrSts_Process2 )
			{
				gvInFlash.mProcess = cInFlashProcessTbl;
				gvInFlash.mProcess.mUpVer[ 0U ] = cRomVer[ 0U ];
				gvInFlash.mProcess.mUpVer[ 1U ] = cRomVer[ 1U ] * 10U + cRomVer[ 2U ];
				gvInFlash.mProcess.mUpVer[ 2U ] = cRomVer[ 3U ];
			}
			
			gvInFlash.mParam = cInFlashParmTbl;
			gvInFlash.mData = cInFlashDataTbl;
			gvInFlash.mParam.mLogCyc1 = ecRecKind1_1min;		/* �H���ł�1������(ResetKey�H��o�׏�������5������) */
			gvInFlash.mParam.mLogCyc2 = ecRecKind2_1min;
			gvInFlash.mParam.mAppID[ 0U ] = 0x00U;				/* �H���ł̓A�v��ID�u1�v */
			gvInFlash.mParam.mAppID[ 1U ] = 0x01U;
			gvInFlash.mParam.mOnCertLmt = CENTRAL;
			break;
			
		/* ���݂�RAM�ϐ������Flash�Ƀ��C�g����Ƃ� */
		case ecInFlashWrSts_ParmNow:
		default:
			break;
	}
	
	/* �����t���b�V���h���C�o�J�n */
	if( PFDL_OK == ApiInFlashDrv_Open() )
	{
		/* ����Flash(primary, secondary)�֏����l�����C�g */
		for( wkKind = ecFlashKind_Prim ; wkKind < 2U ; wkKind++ )
		{
			while( 1 )
			{
				SubInFlash_Write( wkKind, arSelect, arAddr );
				if( vInFlashPrm.mSts == ecInFlashSts_Init )
				{
					NOP();
					break;
				}
			}
		}
	}
	
	/* �����t���b�V���h���C�o�I�� */
	ApiInFlashDrv_Close();
	
	/* ������p�Ƃ���Flash�Ƀ����������ɃA�v��ID���u0�v�ɂ��� */
	if( arSelect == ecInFlashWrSts_Process )
	{
		gvInFlash.mParam.mAppID[ 1U ] = 0x00U;
	}
}


/*
 *******************************************************************************
 *	�����t���b�V������ �����ݏ���
 *
 *	[���e]
 *		�����t���b�V������ �����ݏ���
 *******************************************************************************
 */
void SubInFlash_Write( ET_FlashKind_t arKind, ET_InFlashWrSts_t arSelect, uint16_t arAddr )
{
	pfdl_status_t	wkRet;					/* �G���[�߂�l */
	uint8_t			wkRdData;
	uint8_t			*pwkData;
	uint8_t			wkLoopTbl;
	uint8_t			wkRetryCnt;
	uint16_t		wkStrtAddr;				/* �J�n�A�h���X */
	uint16_t		wkEndAddr;				/* �I���A�h���X */
	uint16_t		wkSize;					/* �T�C�Y */
	
	if( vInFlashPrm.mReqSts != ecInFlashReq_Rd )
	{
		switch( vInFlashPrm.mSts )
		{
			/* ����Flash�I�[�v�� */
			case ecInFlashSts_Init:
				vInFlashPrm.mReqSts = ecInFlashReq_Wr;
				vInFlashPrm.mSts = ecInFlashSts_ErasePre;
				break;
				
			/* �u���b�N�������� */
			case ecInFlashSts_ErasePre:
				
				if( arKind == ecFlashKind_Prim )
				{
					/* �u���b�N�����R�}���h���M */
					wkRet = ApiInFlashDrv_BlkErsCmdSnd( imBlock0 );
				}
				else
				{
					/* �u���b�N�����R�}���h���M */
					wkRet = ApiInFlashDrv_BlkErsCmdSnd( imBlock3 );
				}
				vInFlashPrm.mSts = ecInFlashSts_Erase;
				break;
				
			/* �u���b�N������ */
			case ecInFlashSts_Erase:
				for( wkRetryCnt = 0U ; wkRetryCnt < 3U ; wkRetryCnt++ )
				{
					wkRet = ApiInFlashDrv_HandlerCmdSnd();
					if( wkRet == PFDL_OK )
					{
						vInFlashPrm.mSts = ecInFlashSts_BlankChkPre;
						break;
					}
					else if( wkRet == PFDL_BUSY )
					{
						/* �R�}���h���s�� */
						NOP();
						break;
					}
					else
					{
						if( wkRetryCnt == 2U )
						{
							ApiAbn_AbnStsSet( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );	/* �����t���b�V���A�N�Z�X�G���[ */
							vInFlashPrm.mSts = ecInFlashSts_Close;
						}
					}
				}
				break;
				
			/* �u�����N�`�F�b�N���� */
			case ecInFlashSts_BlankChkPre:
				if( arKind == ecFlashKind_Prim )
				{
					wkStrtAddr = imAddrStartBlk0;
				}
				else
				{
					wkStrtAddr = imAddrStartBlk3;
				}
				
				/* �u�����N�`�F�b�N�R�}���h���M */
				wkRet = ApiInFlashDrv_BlnkChkCmdSnd( wkStrtAddr, imBlkSize );
				vInFlashPrm.mSts = ecInFlashSts_BlankChk;
				break;
				
			/* �u�����N�`�F�b�N�� */
			case ecInFlashSts_BlankChk:
				for( wkRetryCnt = 0U ; wkRetryCnt < 3U ; wkRetryCnt++ )
				{
					wkRet = ApiInFlashDrv_HandlerCmdSnd();
					if( wkRet == PFDL_OK )
					{
						vInFlashPrm.mSts = ecInFlashSts_Write;
						break;
					}
					else if( wkRet == PFDL_ERR_MARGIN )
					{
						/* �u�����N��ԂłȂ� */
						vInFlashPrm.mSts = ecInFlashSts_ErasePre;
						break;
					}
					else if( wkRet == PFDL_BUSY )
					{
						/* �R�}���h���s�� */
						NOP();
						break;
					}
					else
					{
						if( wkRetryCnt == 2U )
						{
							ApiAbn_AbnStsSet( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );	/* �����t���b�V���A�N�Z�X�G���[ */
							vInFlashPrm.mSts = ecInFlashSts_Close;
						}
					}
				}
				break;
				
			/* �������݁A�x���t�@�C */
			case ecInFlashSts_Write:
			case ecInFlashSts_Verify:
				for( wkLoopTbl = 0U ; wkLoopTbl < 3U ; wkLoopTbl++ )
				{
					if( arKind == ecFlashKind_Prim )
					{
						wkStrtAddr = imAddrStartBlk0;
					}
					else
					{
						wkStrtAddr = imAddrStartBlk3;
					}
					
					switch( wkLoopTbl )
					{
						case 0U:
							if( vInFlashPrm.mSts == ecInFlashSts_Write )
							{
								pwkData = (uint8_t *)&gvInFlash.mProcess;
							}
							wkSize = sizeof( gvInFlash.mProcess );
							wkStrtAddr += imAddrProcessOfs;
							break;
						case 1U:
							if( vInFlashPrm.mSts == ecInFlashSts_Write )
							{
								pwkData = (uint8_t *)&gvInFlash.mParam;
							}
							wkSize = sizeof( gvInFlash.mParam );
							wkStrtAddr += imAddrParamOfs;
							break;
						case 2U:
							if( vInFlashPrm.mSts == ecInFlashSts_Write )
							{
								pwkData = (uint8_t *)&gvInFlash.mData;
							}
							wkSize = sizeof( gvInFlash.mData );
							wkStrtAddr += imAddrDataOfs;
							break;
					}
					
					if( vInFlashPrm.mSts == ecInFlashSts_Write )
					{
						/* �����݃R�}���h���M */
						wkRet = ApiInFlashDrv_WrCmdSnd( wkStrtAddr, wkSize, pwkData );
					}
					else
					{
						/* �x���t�@�C�R�}���h���M */
						wkRet = ApiInFlashDrv_VerifyCmdSnd( wkStrtAddr, wkSize );
					}
					
					wkRetryCnt = 0U;
					while( 1 )
					{
						wkRet = ApiInFlashDrv_HandlerCmdSnd();
						if( wkRet == PFDL_OK )
						{
							break;
						}
						else if( wkRet == PFDL_BUSY )
						{
							/* �R�}���h���s�� */
							NOP();
						}
						else
						{
							if( wkRetryCnt == 2U )
							{
								ApiAbn_AbnStsSet( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );			/* �����t���b�V���A�N�Z�X�G���[ */
								vInFlashPrm.mSts = ecInFlashSts_Close;
								break;
							}
							wkRetryCnt ++;
						}
					}
				}
				
				if( vInFlashPrm.mSts != ecInFlashSts_Close )
				{
					if( vInFlashPrm.mSts == ecInFlashSts_Write )
					{
						vInFlashPrm.mSts = ecInFlashSts_Verify;
					}
					else
					{
						vInFlashPrm.mSts = ecInFlashSts_ReadCheck;
					}
				}
				break;
				
			/* ���[�h�`�F�b�N */
			case ecInFlashSts_ReadCheck:
				for( wkLoopTbl = 0U ; wkLoopTbl < 3U ; wkLoopTbl++ )
				{
					if( arKind == ecFlashKind_Prim )
					{
						wkStrtAddr = imAddrStartBlk0;
					}
					else
					{
						wkStrtAddr = imAddrStartBlk3;
					}
					
					switch( wkLoopTbl )
					{
						case 0U:
							wkStrtAddr += imAddrProcessOfs;
							wkEndAddr= sizeof( gvInFlash.mProcess ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mProcess;
							break;
						case 1U:
							wkStrtAddr += imAddrParamOfs;
							wkEndAddr = sizeof( gvInFlash.mParam ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mParam;
							break;
						case 2U:
							wkStrtAddr += imAddrDataOfs;
							wkEndAddr = sizeof( gvInFlash.mData ) + wkStrtAddr;
							pwkData = (uint8_t *)&gvInFlash.mData;
							break;
					}
					
					for( ; wkStrtAddr < wkEndAddr; wkStrtAddr++,pwkData++ )
					{
						for( wkRetryCnt = 0U ; wkRetryCnt < 3U ; wkRetryCnt++ )
						{
							/* �ǂݍ��݃R�}���h���M */
							wkRet = ApiInFlashDrv_RdCmdSnd( wkStrtAddr, 1U, &wkRdData );
							if( *pwkData != wkRdData )
							{
								if( wkRetryCnt == 2U )
								{
									ApiAbn_AbnStsSet( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );		/* �����t���b�V���A�N�Z�X�G���[ */
								}
							}
							else
							{
#if 0	/* �@��ُ한�A�����Ȃ� */
								ApiAbn_AbnStsClr( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );			/* ����Flas�G���[���� */
#endif
								break;
							}
						}
					}
				}
				vInFlashPrm.mSts = ecInFlashSts_Close;
				break;
				
			/* ����Flash�N���[�Y */
			case ecInFlashSts_Close:
				vInFlashPrm.mSts = ecInFlashSts_Init;							/* �����t���b�V����ԏ��� */
				vInFlashPrm.mReqSts = ecInFlashReq_Init;						/* ���N�G�X�g��ԏ��� */
				break;
		}
	}
}


/*
 *******************************************************************************
 *	����Flash����p�����[�^�����[�h
 *
 *	[����]
 *		uint16_t arAddr�F���[�h��A�h���X
 *	[�ߒl]
 *		uint8_t	wkRdDataPrim: ���[�h�l
 *	[���e]
 *		����Flash����A�h���X�w��Œl�����[�h����
 *******************************************************************************
 */
uint8_t ApiInFlash_Read( uint16_t arAddr )
{
	uint16_t		wkStrtAddr;
	uint8_t			wkRdDataPrim;
	uint8_t			wkRdDataSecond;
	uint8_t			wkRetryCnt;
	
	/* �����t���b�V���h���C�o�J�n */
	if( PFDL_OK == ApiInFlashDrv_Open() )
	{
		for( wkRetryCnt = 0U ; wkRetryCnt < 3U ; wkRetryCnt++ )
		{
			wkStrtAddr = arAddr;
			ApiInFlashDrv_RdCmdSnd( wkStrtAddr, 1U, &wkRdDataPrim );
			
			if( wkStrtAddr < imAddrStartBlk1 )
			{
				wkStrtAddr += 0xC00U;
			}
			else
			{
				wkStrtAddr += 0x400U;
			}
			ApiInFlashDrv_RdCmdSnd( wkStrtAddr, 1U, &wkRdDataSecond );
			
			/* �~���[�̈�ƒl��r */
			if( wkRdDataPrim != wkRdDataSecond )
			{
				if( wkRetryCnt == 2U )
				{
					ApiAbn_AbnStsSet( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );		/* �����t���b�V���G���[ */
				}
			}
			else
			{
#if 0	/* �@��ُ한�A�����Ȃ� */
				ApiAbn_AbnStsClr( imAbnSts_INFLSH, ecAbnInfKind_AbnSts );			/* ����Flas�G���[���� */
#endif
				break;
			}
		}
	}
	
	ApiInFlashDrv_Close();
	
	return wkRdDataPrim;
}


/*
 *******************************************************************************
 *	�d���������̃t���O�ɂ��e����
 *
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *	[���e]
 *		����Flash���烊�[�h�����t���O���m�F���A�t���OON�Ȃ�e���������s����
 *******************************************************************************
 */
void ApiInFlash_ParmInitFlgChk( void )
{
	uint8_t wkLoop;
	
	/* �H��o�׏������t���O���� */
	if( M_TSTBIT( gvInFlash.mParam.mParmFlg, imFactoryInitFlg ) )
	{
		/* ����Flash������ */
		ApiInFlash_ParmWrite( ecInFlashWrSts_ParmInit, 0U );
		
		/* �O�t��Flash�d��ON */
		ApiFlash_FlashPowerCtl( imON, imON );
		
		/* �O�t��Flash�C���[�X */
		ApiFlashDrv_ChipErase( ecFlashKind_Prim );
		ApiFlashDrv_ChipErase( ecFlashKind_Second );
	}
	
	/* ���^�Ԋu2�͎��^�Ԋu1�ȉ��̊Ԋu�ł��邱�� */
	if( gvInFlash.mParam.mLogCyc1 < gvInFlash.mParam.mLogCyc2 )
	{
		gvInFlash.mParam.mLogCyc2 = gvInFlash.mParam.mLogCyc1;
	}
	
	/* �x�񔭐����ɓd��OFF���ꂽ�Ƃ��A�ߋ��x��t���O��ON�ɂ��� */
	for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop++ )
	{
		if( gvInFlash.mData.mAlmPast[ wkLoop ] == ecAlmPastSts_Pre )
		{
			gvInFlash.mData.mAlmPast[ wkLoop ] = ecAlmPastSts_Set;
		}
	}
	
	/* �����ʐM�Ԋu������ */
	ApiRfDrv_RtStbyOnOff( 2U );
	
	gvFstConnBootCnt = gvInFlash.mData.mMeas1_Num;
}

/*
 *******************************************************************************
 *	�ݒ�l������
 *
 *	[���e]
 *		�����ʐM���[�h�Ŏ�M�������K�[�ݒ�l����������
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiInFlash_SetValWrite( ST_RF_Logger_SettingWt_t *arRfBuff )
{
	uint8_t		wkLoop;
	uint8_t		wkU8;
	uint8_t		wkSetVal;
	uint16_t	wkSetData;
	uint16_t	wkNowDataCnt;
	uint16_t	wkNowData;
	
	gvActQue.mNum = 0U;						/* �i�[���J�E���^�N���A */
	gvActQue.mUserID = arRfBuff->mUserID;	/* �ݒ�ύX�҂�ID */
	
	for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop++ )
	{
		/* �x��֘A�̐ݒ�CH������ */
		if( wkLoop >= cAlmSetChTbl[ gvInFlash.mProcess.mModelCode ].mCh )
		{
			break;
		}
		
		/* �����x��l */
		wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mChSetting[ wkLoop ].mUUAlarmValue[ 0U ] );
		wkNowDataCnt = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviUU[ wkLoop ] );
		gvInFlash.mParam.mDeviUU[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_UUDevi, wkSetData, wkNowDataCnt, gvActQue.mUserID, wkLoop );
		
		/* ����x��l */
		wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mChSetting[ wkLoop ].mUAlarmValue[ 0U ] );
		wkNowDataCnt = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviU[ wkLoop ] );
		gvInFlash.mParam.mDeviU[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_UDevi, wkSetData, wkNowDataCnt, gvActQue.mUserID, wkLoop );
		
		/* �����x��l */
		wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mChSetting[ wkLoop ].mLAlarmValue[ 0U ] );
		wkNowDataCnt = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviL[ wkLoop ] );
		gvInFlash.mParam.mDeviL[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_LDevi, wkSetData, wkNowDataCnt, gvActQue.mUserID, wkLoop );
		
		/* �������x��l */
		wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mChSetting[ wkLoop ].mLLAlarmValue[ 0U ] );
		wkNowDataCnt = ApiInFlash_AlmVal_ValtoCnt( gvInFlash.mParam.mDeviLL[ wkLoop ] );
		gvInFlash.mParam.mDeviLL[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_LLDevi, wkSetData, wkNowDataCnt, gvActQue.mUserID, wkLoop );
		
		/* �����x���l */
		gvInFlash.mParam.mDelayUU[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_UUDelay, arRfBuff->mChSetting[ wkLoop ].mUUAlarmDelay, gvInFlash.mParam.mDelayUU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* ����x���l */
		gvInFlash.mParam.mDelayU[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_UDelay, arRfBuff->mChSetting[ wkLoop ].mUAlarmDelay, gvInFlash.mParam.mDelayU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* �����x���l */
		gvInFlash.mParam.mDelayL[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LDelay, arRfBuff->mChSetting[ wkLoop ].mLAlarmDelay, gvInFlash.mParam.mDelayL[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* �������x���l */
		gvInFlash.mParam.mDelayLL[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LLDelay, arRfBuff->mChSetting[ wkLoop ].mLLAlarmDelay, gvInFlash.mParam.mDelayLL[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* ��E���e���ԑI��臒l */
		gvInFlash.mParam.mDeviEnaLv[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviEnaLv, arRfBuff->mChSetting[ wkLoop ].mToleranceLevel, gvInFlash.mParam.mDeviEnaLv[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* ��E���e���� */
		wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mChSetting[ wkLoop ].mToleranceTime[ 0U ] );
		gvInFlash.mParam.mDeviEnaTime[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviEnaTime, wkSetData, gvInFlash.mParam.mDeviEnaTime[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* �v���l�I�t�Z�b�g */
		wkNowDataCnt = ApiInFlash_OftVal_ValtoCnt( gvInFlash.mParam.mOffset[ wkLoop ] );			/* ���ݒl���J�E���g�ϊ� */
		gvInFlash.mParam.mOffset[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Offset, arRfBuff->mChSetting[ wkLoop ].mMeasureOffset, wkNowDataCnt, gvActQue.mUserID, wkLoop );
		
		/* �v���l�ꎟ�X���␳ */
		gvInFlash.mParam.mSlope[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Slope, arRfBuff->mChSetting[ wkLoop ].mMeasureZeroSpan, (gvInFlash.mParam.mSlope[ wkLoop ] - 70), gvActQue.mUserID, wkLoop );
	}
	
	/* �d�r�� */
	gvInFlash.mParam.mBatType = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_BatType, arRfBuff->mBattCode, gvInFlash.mParam.mBatType, gvActQue.mUserID, 0U );
	
	/* �L�[���b�N */
	gvInFlash.mParam.mKeyLock = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_KeyLock, arRfBuff->mKeylock, gvInFlash.mParam.mKeyLock, gvActQue.mUserID, 0U );
	
	/* �������ʐM����CH(�ڑ��p) */
	gvInFlash.mParam.mrfLoraChGrupeCnt = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_RealTimeCh, arRfBuff->mRtModeCh, gvInFlash.mParam.mrfLoraChGrupeCnt, gvActQue.mUserID, 0U ) - 1U;
	
	/* ���^����1 */
	wkU8 = gvInFlash.mParam.mLogCyc1;
	gvInFlash.mParam.mLogCyc1 = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LogCyc1, arRfBuff->mRecordIntMain, gvInFlash.mParam.mLogCyc1, gvActQue.mUserID, 0U );
	
	/* ���^����2(�x��Ď�����) */
	wkSetVal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LogCyc2, arRfBuff->mRecordIntAlarm, gvInFlash.mParam.mLogCyc2, gvActQue.mUserID, 0U );
	
	/* ���^����1�ȉ��ł��邱�ƁA���^����1�Ɠ����ݒ�Ȃ�ꏏ�ɕω����邱�� */
	if( gvInFlash.mParam.mLogCyc1 < wkSetVal || (arRfBuff->mRecordIntAlarm == 15 && wkU8 == gvInFlash.mParam.mLogCyc2) )
	{
		wkSetVal = gvInFlash.mParam.mLogCyc1;
	}
	ApiFlash_QueActHist( ecActLogItm_LogCyc2, gvInFlash.mParam.mLogCyc2, wkSetVal, gvActQue.mUserID );								/* �ݒ�ύX���� */
	gvInFlash.mParam.mLogCyc2 = wkSetVal;
	
	/* ���K�[���� */
	SubInFlash_SetLoggerName( arRfBuff->mLoggerName );
//	for( wkLoop = 0U ; wkLoop < imLoggerNameNum ; wkLoop++ )
//	{
		/* �ݒ�l�֍X�V */
//		if( SubInFlash_SetValWrite_ValChk( ecSetParamList_LoggerName, arRfBuff->mLoggerName[ wkLoop ], gvInFlash.mParam.mLoggerName[ wkLoop ], gvActQue.mUserID, 0U ) == 1U )
//		{
//			for( wkLoop = 0U, wkLogItmNo = ecActLogItm_LoggerName1 ; wkLoop < imLoggerNameNum ; wkLoop += 2U )
//			{
//				wkU16 = (gvInFlash.mParam.mLoggerName[ wkLoop ] << 8U) | (gvInFlash.mParam.mLoggerName[ wkLoop + 1U ]);
//				wkSetData = (arRfBuff->mLoggerName[ wkLoop ] << 8U) | (arRfBuff->mLoggerName[ wkLoop  + 1U ]);
				
//				ApiFlash_QueActHist( wkLogItmNo, wkU16, wkSetData, gvActQue.mUserID );												/* �ݒ�ύX���� */
//				wkLogItmNo++;
				
//				gvInFlash.mParam.mLoggerName[ wkLoop ] = arRfBuff->mLoggerName[ wkLoop ];
//				gvInFlash.mParam.mLoggerName[ wkLoop + 1U ] = arRfBuff->mLoggerName[ wkLoop + 1U ];
//			}
//			break;
//		}
//	}
	
	/* �����ʐM�ڑ�CH */
	gvInFlash.mParam.mrfHsCh  = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_FstConnCh, arRfBuff->mHsModeCh, gvInFlash.mParam.mrfHsCh, gvActQue.mUserID, 0U ) - 1U;
	
	/* �����ʐM�N���̃��O�f�[�^臒l */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mHSmodeWakeUpLogValue[ 0U ] );
	gvInFlash.mParam.mFstConnBoot = SubInFlash_SetValWrite_ValChk( ecSetParamList_FstConnBoot, wkSetData, gvInFlash.mParam.mFstConnBoot, gvActQue.mUserID, 0U );
	
	/* �x��Ď����Z�� */
	gvInFlash.mParam.mAlertType = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertType, arRfBuff->mAlarmCulcCode, gvInFlash.mParam.mAlertType, gvActQue.mUserID, 0U );
	
	/* �x��Ď����Z�p�����[�^�A�t���O�L�^ */
	/* �p�����[�^ */
	gvInFlash.mParam.mAlertParmFlag = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertParm, (arRfBuff->mAlarmCulcPrmFlag & 0x1FU), (gvInFlash.mParam.mAlertParmFlag & 0x1FU), gvActQue.mUserID, 0U );
	
	/* �t���O */
	wkSetVal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertFlag, ((arRfBuff->mAlarmCulcPrmFlag & 0x20U) >> 5U), ((gvInFlash.mParam.mAlertParmFlag & 0x20U) >> 5U), gvActQue.mUserID, 0U );
	gvInFlash.mParam.mAlertParmFlag += wkSetVal << 5U;
	
	/* �C�x���g�{�^������ */
	wkU8 = gvInFlash.mParam.mEventKeyFlg;
	gvInFlash.mParam.mEventKeyFlg = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_EventKeyFlg, arRfBuff->mEventCtrl, gvInFlash.mParam.mEventKeyFlg, gvActQue.mUserID, 0U );
	if( gvInFlash.mParam.mEventKeyFlg != wkU8 )
	{
		gvModuleSts.mKeyInt = ecKeyIntModuleSts_EventMode;		/* Event�L�[1s�����������̓�������s */
	}
	
	/* ��E���e���ԃN���A��i */
	gvInFlash.mParam.mDeviClrPoint = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviClrPoint, arRfBuff->mToleranceClarMeth, gvInFlash.mParam.mDeviClrPoint, gvActQue.mUserID, 0U );
	
	/* ��E���e���ԃN���A���� */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mToleranceClarTime[ 0U ] );
	wkNowData = (gvInFlash.mParam.mDeviClrTimeHour * 60U) + gvInFlash.mParam.mDeviClrTimeMin;
	wkSetData = SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviClrTime, wkSetData, wkNowData, gvActQue.mUserID, 0U );
	gvInFlash.mParam.mDeviClrTimeHour = wkSetData / 60U;
	gvInFlash.mParam.mDeviClrTimeMin = wkSetData % 60U;
	
	/* �Z���� */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mCalDate[ 0U ] );
	gvInFlash.mParam.mCalDate = SubInFlash_SetValWrite_ValChk( ecSetParamList_CalDate, wkSetData, gvInFlash.mParam.mCalDate, gvActQue.mUserID, 0U );
	
	/* �X�P�[�����O�ϊ������_�ʒu */
	gvInFlash.mParam.mScalDecimal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_ScalDecimal, arRfBuff->mScalDecimal, gvInFlash.mParam.mScalDecimal, gvActQue.mUserID, 0U );
	
	/* �X�P�[�����O�ϊ��ؕЁA�X�� */
	SubInFlash_SetScale( arRfBuff->mScalYInt, arRfBuff->mScalGrad );
	
	/* �v���C�}���t���b�V���������؂�ւ� */
	gvInFlash.mParam.mFlashSelect = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_FlashSelect, arRfBuff->mMemorySelect, gvInFlash.mParam.mFlashSelect, gvActQue.mUserID, 0U );
	
	/* �ڑ����Q�[�g�E�F�CID */
	SubInFlash_SetCommGwID( arRfBuff->mCommGwID );
//	wkLogItmNo = ecActLogItm_CnctEnaGwId1;
//	for( wkLoop = 0U ; wkLoop < 10U ; wkLoop++, wkLogItmNo++ )
//	{
//		for( wkLoop2 = 0U ; wkLoop2 < 3U ; wkLoop2++ )
//		{
			/* �ݒ�l�֍X�V */
//			if( SubInFlash_SetValWrite_ValChk( ecSetParamList_CnctEnaGwId, arRfBuff->mCommGwID[ wkLoop ][ wkLoop2 ], gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ], gvActQue.mUserID, 0U ) == 1U )
//			{
//				for( wkLoop2 = 0U, wkCnctId_now = 0U, wkCnctId_set = 0U ; wkLoop2 < 3U ; wkLoop2++ )
//				{
//					wkCnctId_now += gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
//					wkCnctId_set += arRfBuff->mCommGwID[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
					
//					gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] = arRfBuff->mCommGwID[ wkLoop ][ wkLoop2 ];
//				}
//				wkCnctId_now >>= 4U;
//				wkCnctId_set >>= 4U;
				
//				ApiFlash_QueActHist( wkLogItmNo, wkCnctId_now, wkCnctId_set, gvActQue.mUserID );										/* �ݒ�ύX���� */
//				break;
//			}
//		}
//	}
	
	/* �O���[�vID */
	gvInFlash.mParam.mGroupID = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_GrId, arRfBuff->mGroupID, gvInFlash.mParam.mGroupID, gvActQue.mUserID, 0U );
	ApiRfDrv_GrID_Chg();
	
	/* �ݒ�ύX�������L�^ */
	SubInFlash_SetValTime();
	
	gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
}

/*
 *******************************************************************************
 *	�������ʐM�ݒ�l������1
 *
 *	[���e]
 *		�������ʐM���[�h�Ŏ�M�������K�[�ݒ�l����������
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiInFlash_RtSetValWrite1( ST_RF_RT_ChangeSet_Prm1_t *arRfBuff )
{
	uint8_t wkKeySet = 0xFFU;
	
	gvActQue.mNum = 0U;						/* �i�[���J�E���^�N���A */
	gvActQue.mUserID = arRfBuff->mUserID;	/* �ݒ�ύX�҂�ID */
	
	/* �ڑ����Q�[�g�E�F�CID */
	SubInFlash_SetCommGwID( arRfBuff->mPermitGwID );
//	wkLogItmNo = ecActLogItm_CnctEnaGwId1;
//	for( wkLoop = 0U ; wkLoop < 10U ; wkLoop++, wkLogItmNo++ )
//	{
//		for( wkLoop2 = 0U ; wkLoop2 < 3U ; wkLoop2++ )
//		{
			/* �ݒ�l�֍X�V */
//			if( SubInFlash_SetValWrite_ValChk( ecSetParamList_CnctEnaGwId, arRfBuff->mPermitGwID[ wkLoop ][ wkLoop2 ], gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ], gvActQue.mUserID, 0U ) == 1U )
//			{
//				for( wkLoop2 = 0U, wkCnctId_now = 0U, wkCnctId_set = 0U ; wkLoop2 < 3U ; wkLoop2++ )
//				{
//					wkCnctId_now += gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
//					wkCnctId_set += arRfBuff->mPermitGwID[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
					
//					gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] = arRfBuff->mPermitGwID[ wkLoop ][ wkLoop2 ];
//				}
//				wkCnctId_now >>= 4U;
//				wkCnctId_set >>= 4U;
				
//				ApiFlash_QueActHist( wkLogItmNo, wkCnctId_now, wkCnctId_set, gvActQue.mUserID );									/* �ݒ�ύX���� */
//				break;
//			}
//		}
//	}
	
	/* �O���[�vID */
	gvInFlash.mParam.mGroupID = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_GrId, arRfBuff->mGroupID, gvInFlash.mParam.mGroupID, gvActQue.mUserID, 0U );
	ApiRfDrv_GrID_Chg();
	
	/* �d�r�� */
	gvInFlash.mParam.mBatType = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_BatType, (arRfBuff->mOnline_Batt_Key & 0xE0U) >> 5U, gvInFlash.mParam.mBatType, gvActQue.mUserID, 0U );
	
	/* �L�[���b�N */
	if( !(arRfBuff->mOnline_Batt_Key & 0x10U) )
	{
		wkKeySet = arRfBuff->mOnline_Batt_Key & 0x1FU;
	}
	gvInFlash.mParam.mKeyLock = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_KeyLock, wkKeySet, gvInFlash.mParam.mKeyLock, gvActQue.mUserID, 0U );
}

/*
 *******************************************************************************
 *	�������ʐM�ݒ�l������2
 *
 *	[���e]
 *		�������ʐM���[�h�Ŏ�M�������K�[�ݒ�l����������
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiInFlash_RtSetValWrite2( ST_RF_RT_ChangeSet_Prm2_t *arRfBuff )
{
	uint8_t		wkLoop;
	
	/* �������ʐM����CH(�ڑ��p) */
	gvInFlash.mParam.mrfLoraChGrupeCnt = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_RealTimeCh, arRfBuff->mRtModeCh, gvInFlash.mParam.mrfLoraChGrupeCnt, gvActQue.mUserID, 0U ) - 1U;
	
	for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop++ )
	{
		/* �x��֘A�̐ݒ�CH������ */
		if( wkLoop >= cAlmSetChTbl[ gvInFlash.mProcess.mModelCode ].mCh )
		{
			break;
		}
		
		/* �����x��l */
		gvInFlash.mParam.mDeviUU[ wkLoop ] = SubInFlash_UpdateAlmVal( ecSetParamList_UUDevi, &arRfBuff->mUUAlarmValue[ 0U ], gvInFlash.mParam.mDeviUU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* ����x��l */
		gvInFlash.mParam.mDeviU[ wkLoop ] = SubInFlash_UpdateAlmVal( ecSetParamList_UDevi, &arRfBuff->mUAlarmValue[ 0U ], gvInFlash.mParam.mDeviU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* �����x��l */
		gvInFlash.mParam.mDeviL[ wkLoop ] = SubInFlash_UpdateAlmVal( ecSetParamList_LDevi, &arRfBuff->mLAlarmValue[ 0U ], gvInFlash.mParam.mDeviL[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* �������x��l */
		gvInFlash.mParam.mDeviLL[ wkLoop ] = SubInFlash_UpdateAlmVal( ecSetParamList_LLDevi, &arRfBuff->mLLAlarmValue[ 0U ], gvInFlash.mParam.mDeviLL[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* �����x���l */
		gvInFlash.mParam.mDelayUU[ wkLoop ] = SubInFlash_UpdateAlmDelay( ecSetParamList_UUDelay, &arRfBuff->mUUAlarmDelay[ 0U ], gvInFlash.mParam.mDelayUU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* ����x���l */
		gvInFlash.mParam.mDelayU[ wkLoop ] = SubInFlash_UpdateAlmDelay( ecSetParamList_UDelay, &arRfBuff->mUAlarmDelay[ 0U ], gvInFlash.mParam.mDelayU[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* �����x���l */
		gvInFlash.mParam.mDelayL[ wkLoop ] = SubInFlash_UpdateAlmDelay( ecSetParamList_LDelay, &arRfBuff->mLAlarmDelay[ 0U ], gvInFlash.mParam.mDelayL[ wkLoop ], gvActQue.mUserID, wkLoop );
		
		/* �������x���l */
		gvInFlash.mParam.mDelayLL[ wkLoop ] = SubInFlash_UpdateAlmDelay( ecSetParamList_LLDelay, &arRfBuff->mLLAlarmDelay[ 0U ], gvInFlash.mParam.mDelayLL[ wkLoop ], gvActQue.mUserID, wkLoop );
	}
}



/*
 *******************************************************************************
 *	�������ʐM�ݒ�l������3
 *
 *	[���e]
 *		�������ʐM���[�h�Ŏ�M�������K�[�ݒ�l����������
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiInFlash_RtSetValWrite3( ST_RF_RT_ChangeSet_Prm3_t *arRfBuff )
{
	uint8_t		wkLoop;
	uint8_t		wkU8;
	uint8_t		wkSetVal;
	uint16_t	wkSetData;
	uint16_t	wkNowData;
	uint16_t	wkNowDataCnt;
	
	for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop++ )
	{
		/* �ݒ�CH������ */
		if( wkLoop >= cAlmSetChTbl[ gvInFlash.mProcess.mModelCode ].mCh )
		{
			break;
		}
		
		/* ��E���e���ԑI��臒l */
		wkSetData = (arRfBuff->mToleranceTime[ 2U ] << 4U) + ((arRfBuff->mToleranceTime[ 1U ] & 0xF0U) >> 4U);
		gvInFlash.mParam.mDeviEnaLv[ wkLoop ] = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviEnaLv, wkSetData, gvInFlash.mParam.mDeviEnaLv[ wkLoop ], gvActQue.mUserID, 0U );
		
		/* ��E���e���� */
		wkSetData = ((arRfBuff->mToleranceTime[ 1U ] & 0x0FU) << 8U) + arRfBuff->mToleranceTime[ 0U ];
		gvInFlash.mParam.mDeviEnaTime[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviEnaTime, wkSetData, gvInFlash.mParam.mDeviEnaTime[ wkLoop ], gvActQue.mUserID, 0U );
		
		/* �v���l�I�t�Z�b�g */
		wkNowDataCnt = ApiInFlash_OftVal_ValtoCnt( gvInFlash.mParam.mOffset[ wkLoop ] );
		gvInFlash.mParam.mOffset[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Offset, arRfBuff->mMeasureOffset[ wkLoop ], wkNowDataCnt, gvActQue.mUserID, 0U );
		
		/* �v���l�ꎟ�X���␳ */
		gvInFlash.mParam.mSlope[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Slope, arRfBuff->mMeasureZeroSpan[ wkLoop ], (gvInFlash.mParam.mSlope[ wkLoop ] - 70), gvActQue.mUserID, 0U );
	}
	
	/* ��E���e���ԃN���A��i_0:�C�x���g�{�^���A1�F�ݒ莞�� */
	wkSetData = 1U;
	if( arRfBuff->mToleranceTime[ 3U ] == 0xFFU )
	{
		wkSetData = 0U;
	}
	gvInFlash.mParam.mDeviClrPoint = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviClrPoint, wkSetData, gvInFlash.mParam.mDeviClrPoint, gvActQue.mUserID, 0U );
	
	/* ��E���e���ԃN���A���� */
	wkSetData = ((arRfBuff->mToleranceTime[ 3U ] & 0xF8U) >> 3U) * 60U + (arRfBuff->mToleranceTime[ 3U ] & 0x07U);
	wkNowData = (gvInFlash.mParam.mDeviClrTimeHour * 60U) + gvInFlash.mParam.mDeviClrTimeMin;
	wkSetData = SubInFlash_SetValWrite_ValChk( ecSetParamList_DeviClrTime, wkSetData, wkNowData, gvActQue.mUserID, 0U );
	gvInFlash.mParam.mDeviClrTimeHour = wkSetData / 60U;
	gvInFlash.mParam.mDeviClrTimeMin = wkSetData % 60U;
	
	/* ���^����1 */
	wkU8 = gvInFlash.mParam.mLogCyc1;
	gvInFlash.mParam.mLogCyc1 = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LogCyc1, arRfBuff->mRecordInt & 0x0FU, gvInFlash.mParam.mLogCyc1, gvActQue.mUserID, 0U );
	
	/* ���^����2(�x��Ď�����) */
	wkSetVal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_LogCyc2, arRfBuff->mRecordInt >> 4U, gvInFlash.mParam.mLogCyc2, gvActQue.mUserID, 0U );
	
	/* ���^����1�ȉ��ł��邱�ƁA���^����1�Ɠ����ݒ�Ȃ�ꏏ�ɕω����邱�� */
	if( gvInFlash.mParam.mLogCyc1 < wkSetVal || (arRfBuff->mRecordInt == 0xF0U && wkU8 == gvInFlash.mParam.mLogCyc2) )
	{
		wkSetVal = gvInFlash.mParam.mLogCyc1;
	}
	ApiFlash_QueActHist( ecActLogItm_LogCyc2, gvInFlash.mParam.mLogCyc2, wkSetVal, gvActQue.mUserID );
	gvInFlash.mParam.mLogCyc2 = wkSetVal;
	
//	for( wkLoop = 0U ; wkLoop < imChannelNum ; wkLoop++ )
//	{
		/* �ݒ�CH������ */
//		if( wkLoop >= cAlmSetChTbl[ gvInFlash.mProcess.mModelCode ].mCh )
//		{
//			break;
//		}
		
		/* �v���l�I�t�Z�b�g */
//		wkNowDataCnt = ApiInFlash_OftVal_ValtoCnt( gvInFlash.mParam.mOffset[ wkLoop ] );										/* ���ݒl���J�E���g�ϊ� */
//		gvInFlash.mParam.mOffset[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Offset, arRfBuff->mMeasureOffset[ wkLoop ], wkNowDataCnt, gvActQue.mUserID, 0U );
		
		/* �v���l�ꎟ�X���␳ */
//		gvInFlash.mParam.mSlope[ wkLoop ] = SubInFlash_SetValWrite_ValChk( ecSetParamList_Slope, arRfBuff->mMeasureZeroSpan[ wkLoop ], (gvInFlash.mParam.mSlope[ wkLoop ] - 70), gvActQue.mUserID, 0U );
//	}
	
	/* ���K�[���� */
	SubInFlash_SetLoggerName( arRfBuff->mLoggerName );
//	for( wkLoop = 0U ; wkLoop < imLoggerNameNum ; wkLoop++ )
//	{
		/* �ݒ�l�֍X�V */
//		if( SubInFlash_SetValWrite_ValChk( ecSetParamList_LoggerName, arRfBuff->mLoggerName[ wkLoop ], gvInFlash.mParam.mLoggerName[ wkLoop ], gvActQue.mUserID, 0U ) == 1U )
//		{
//			for( wkLoop = 0U, wkLogItmNo = ecActLogItm_LoggerName1 ; wkLoop < imLoggerNameNum ; wkLoop++ )
//			{
//				wkU16 = (gvInFlash.mParam.mLoggerName[ wkLoop ] << 8U) | (gvInFlash.mParam.mLoggerName[ wkLoop + 1U ]);
//				wkSetData = (arRfBuff->mLoggerName[ wkLoop ] << 8U) | (arRfBuff->mLoggerName[ wkLoop + 1U ]);
				
//				ApiFlash_QueActHist( wkLogItmNo, wkU16, wkSetData, gvActQue.mUserID );												/* �ݒ�ύX���� */
//				wkLogItmNo++;
				
//				gvInFlash.mParam.mLoggerName[ wkLoop ] = arRfBuff->mLoggerName[ wkLoop ];
//				gvInFlash.mParam.mLoggerName[ wkLoop + 1U ] = arRfBuff->mLoggerName[ wkLoop + 1U ];
//			}
//			break;
//		}
//	}
	
	/* �����ʐM�҂���ch */
	gvInFlash.mParam.mrfHsCh = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_FstConnCh, arRfBuff->mHSmodeCh & 0x0FU, gvInFlash.mParam.mrfHsCh, gvActQue.mUserID, 0U ) - 1U;
	
	/* �X�P�[�����O�ϊ������_�ʒu */
	gvInFlash.mParam.mScalDecimal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_ScalDecimal, ((arRfBuff->mHSmodeCh & 0xF0U) >> 4U), gvInFlash.mParam.mScalDecimal, gvActQue.mUserID, 0U );
	
	/* �����ʐM�N���̃��O�f�[�^臒l */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &arRfBuff->mHSmodeWakeUpLogValue[ 0U ] );
	gvInFlash.mParam.mFstConnBoot = SubInFlash_SetValWrite_ValChk( ecSetParamList_FstConnBoot, wkSetData, gvInFlash.mParam.mFstConnBoot, gvActQue.mUserID, 0U );
	
	/* �x��Ď����Z�p�����[�^ */
	gvInFlash.mParam.mAlertParmFlag = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertParm, (arRfBuff->mAlarmCulcSet[ 0U ]), (gvInFlash.mParam.mAlertParmFlag & 0x1FU), gvActQue.mUserID, 0U );
	
	/* �x��Ď����Z�� */
	gvInFlash.mParam.mAlertType = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertType, arRfBuff->mAlarmCulcSet[ 1U ], gvInFlash.mParam.mAlertType, gvActQue.mUserID, 0U );
	
	/* �x��Ď����Z�t���O�L�^ */
	wkSetVal = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_AlertFlag, (arRfBuff->mAlCulcFlag_EventCont_MemorySet & 0x02U), ((gvInFlash.mParam.mAlertParmFlag & 0x20U) >> 5U), gvActQue.mUserID, 0U );
	gvInFlash.mParam.mAlertParmFlag += wkSetVal << 5U;
	
	/* �C�x���g�{�^������ */
	wkU8 = gvInFlash.mParam.mEventKeyFlg;
	gvInFlash.mParam.mEventKeyFlg = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_EventKeyFlg, ((arRfBuff->mAlCulcFlag_EventCont_MemorySet & 0x0CU) >> 2U), gvInFlash.mParam.mEventKeyFlg, gvActQue.mUserID, 0U );
	if( gvInFlash.mParam.mEventKeyFlg != wkU8 )
	{
		gvModuleSts.mKeyInt = ecKeyIntModuleSts_EventMode;		/* Event�L�[1s�����������̓�������s */
	}
	
	/* �v���C�}���t���b�V���������؂�ւ� */
	wkSetVal = (arRfBuff->mAlCulcFlag_EventCont_MemorySet & 0x30U) >> 4U;
	/* �������ʐM�ł̒l�p��3 �� �����ʐM�ł̒l�p��15�ɕϊ� */
	if( wkSetVal == 3U )
	{
		wkSetVal = 15U;
	}
	gvInFlash.mParam.mFlashSelect = (uint8_t)SubInFlash_SetValWrite_ValChk( ecSetParamList_FlashSelect, wkSetVal, gvInFlash.mParam.mFlashSelect, gvActQue.mUserID, 0U );
	
	/* �X�P�[�����O�ϊ��ؕЁA�X�� */
	SubInFlash_SetScale( arRfBuff->mCalOffset, arRfBuff->mCalZeroSpan );
	
	/* �ݒ�ύX�������L�^ */
	SubInFlash_SetValTime();
	
	gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
}


/*
 *******************************************************************************
 *	�ݒ�l������(��E�l�ݒ�̃T�u�֐�)
 *
 *	[���e]
 *		�����o�b�t�@�����E�l���p�[�X���Đݒ�l�ւ̕ϊ��A�����L�^����
 *	[����]
 *		
 *	[�ߒl]
 *		
 *******************************************************************************
 */
static sint16_t SubInFlash_UpdateAlmVal( ET_SetParamList_t arParmList, uint8_t *parSetVal, sint16_t arNowVal, uint8_t arUserID, uint8_t arLoop )
{
	sint16_t	wkRet;
	uint16_t 	wkSetVal;
	
	switch( arLoop )
	{
		default:
		case 0U:
			/* ������E�l1CH */
			wkSetVal = ((parSetVal[ 1U ] & 0x1FU) << 8U) + parSetVal[ 0U ];
			break;
		case 1U:
			/* ������E�l2CH */
			wkSetVal = ((parSetVal[ 3U ] & 0x03U) << 11U) + (parSetVal[ 2U ] << 3U) + (parSetVal[ 1U ] >> 5U);
			break;
		case 2U:
			/* ������E�l3CH */
			wkSetVal = ((parSetVal[ 4U ] & 0x7FU) << 6U) + (parSetVal[ 3U ] >> 2U);
			break;
		case 3U:
			/* ��������E�l4CH */
			wkSetVal = ((parSetVal[ 6U ] & 0x0FU) << 9U) + (parSetVal[ 5U ] << 1U) + (parSetVal[ 4U ] >> 7U);
			break;
	}
	
	arNowVal = ApiInFlash_AlmVal_ValtoCnt( arNowVal );
	wkRet = (sint16_t)SubInFlash_SetValWrite_ValChk( arParmList, wkSetVal, arNowVal, arUserID, arLoop );
	
	return wkRet;
}

/*
 *******************************************************************************
 *	�ݒ�l������(��E�x���񐔐ݒ�̃T�u�֐�)
 *
 *	[���e]
 *		�����o�b�t�@�����E�x���񐔂��p�[�X���Đݒ�l�ւ̕ϊ��A�����L�^����
 *	[����]
 *		
 *	[�ߒl]
 *		sint16_t	wkS16: �̗p�l
 *******************************************************************************
 */
static uint8_t SubInFlash_UpdateAlmDelay( ET_SetParamList_t arParmList, uint8_t *parSetVal, uint8_t arNowVal, uint8_t arUserID, uint8_t arLoop )
{
	uint8_t wkSetVal;
	
	switch( arLoop )
	{
		case 0U:
			/* �����x����1CH */
			wkSetVal = parSetVal[ 0U ] & 0x3FU;
			break;
		case 1U:
			/* �����x����2CH */
			wkSetVal = ((parSetVal[ 1U ] & 0x3CU) << 2U) + (parSetVal[ 0U ] >> 6U);
			break;
		case 2U:
			/* �����x����3CH */
			wkSetVal = ((parSetVal[ 2U ] & 0x03U) << 4U) + (parSetVal[ 1U ] >> 4U);
			break;
		case 3U:
			/* �������x����4CH */
			wkSetVal = (parSetVal[ 2U ] & 0x0FU) >> 2U;
			break;
	}
	
	wkSetVal = (uint8_t)SubInFlash_SetValWrite_ValChk( arParmList, wkSetVal, arNowVal, arUserID, arLoop );
	
	return wkSetVal;
}


/*
 *******************************************************************************
 *	�ݒ�l������(���ݒl�Ɩ����ݒ�l���r����)
 *
 *	[���e]
 *		�ݒ�l���m�F���A�ݒ�l���͌��ݒl��Ԃ��A�����L��
 *		�������͈͊O�A���ݒ�l�p���łȂ��ꍇ�A���ݒl��Ԃ�
 *	[����]
 *		ET_SetParamList_t	arParmList: �ݒ�ύX�̃p�����[�^
 *		uint16_t			arSetVal: �ݒ�l
 *		uint16_t			arNowVal: ���݂̐ݒ�l
 *	[�ߒl]
 *		uint16_t	wkVal: �̗p�l
 *******************************************************************************
 */
static uint16_t SubInFlash_SetValWrite_ValChk( ET_SetParamList_t arParmList, uint16_t arSetVal, uint16_t arNowVal, uint8_t arUser, uint8_t arLoop )
{
	uint8_t		wkLoop;
	sint16_t	wkS16;
	sint16_t	wkUpVal;
	sint16_t	wkDwVal;
	uint16_t	wkVal = 0U;
	
	ET_ActLogItm_t wkLogItmNo;
	
	/* �ݒ�l�������X�g����ݒ�ύX����l���m�肷�� */
	for( wkLoop = 0U ; wkLoop < M_ArrayElement( cSetValWriteLimitTbl ) ; wkLoop++ )
	{
		if( cSetValWriteLimitTbl[ wkLoop ].mParam == arParmList )
		{
			break;
		}
	}
	
	/* �x�񗚗�No. */
	wkLogItmNo = cSetValWriteLimitTbl[ wkLoop ].mLogItm;
	
	switch( arParmList )
	{
		/* ���K�[����/�ڑ�����GW */
		case ecSetParamList_LoggerName:
		case ecSetParamList_CnctEnaGwId:
			/* �S�̈撆�A�p���l�łȂ��̂��܂܂�Ă�����ݒ�l�֍X�V */
			if( arSetVal != cSetValWriteLimitTbl[ wkLoop ].mKeep )
			{
				wkVal = 1U;			/* �ݒ�l�֍X�V */
			}
			break;
			
		default:
			/* ���ݒl���ꎞ�I�Ɋi�[ */
			wkVal = arNowVal;
			
			/* �ݒ�l���� */
			if( arSetVal >= cSetValWriteLimitTbl[ wkLoop ].mMin && arSetVal <= cSetValWriteLimitTbl[ wkLoop ].mMax )
			{
				wkVal = arSetVal;
			}
			
			/* ���݂̐ݒ�l���p�� */
			if( arSetVal == cSetValWriteLimitTbl[ wkLoop ].mKeep )
			{
				wkVal = arNowVal;
			}
			break;
	}
	
	switch( arParmList )
	{
		case ecSetParamList_UUDevi:
		case ecSetParamList_UDevi:
		case ecSetParamList_LDevi:
		case ecSetParamList_LLDevi:
			/* �J�E���g�������ϊ� */
			wkS16 = SubInFlash_AlmVal_CnttoVal( wkVal );
			arNowVal = SubInFlash_AlmVal_CnttoVal( arNowVal );
			
			/* ���f�����Ƃ̈�E�l���� */
			switch( gvInFlash.mProcess.mModelCode )
			{
				/* �����T�[�~�X�^ */
				case ecSensType_InTh:
					wkUpVal = imAlmValUp_InThModel;
					wkDwVal = imAlmValDw_InThModel;
					break;
				/* �O�t���T�[�~�X�^ */
				case ecSensType_ExTh:
					wkUpVal = imAlmValUp_ExThModel;
					wkDwVal = imAlmValDw_ExThModel;
					break;
#if (swSensorCom == imEnable)
				/* �����x */
				case ecSensType_HumTmp:
					if( arLoop == 0U )
					{
						wkUpVal = imAlmValUp_HumTmpModel_Tmp;
						wkDwVal = imAlmValDw_HumTmpModel_Tmp;
					}
					else
					{
						wkUpVal = imAlmValUp_HumTmpModel_Hum;
						wkDwVal = imAlmValDw_HumTmpModel_Hum;
					}
					break;
				/* Pt */
				case ecSensType_Pt:
					wkUpVal = imAlmValUp_PtModel;
					wkDwVal = imAlmValDw_PtModel;
					break;
				/* �d�� */
				case ecSensType_V:
					wkUpVal = imAlmValUp_VolModel;
					wkDwVal = imAlmValDw_VolModel;
					break;
				/* �p���X */
				case ecSensType_Pulse:
					wkUpVal = imAlmValUp_PulseModel;
					wkDwVal = imAlmValDw_PulseModel;
					break;
#endif
			}
			
			if( wkS16 != M_SINT16_MIN )
			{
				if( wkS16 < wkDwVal )
				{
					wkS16 = wkDwVal;
				}
				else if( wkS16 > wkUpVal )
				{
					wkS16 = wkUpVal;
				}
			}
			wkVal = (uint16_t)wkS16;
			
			wkLogItmNo += (12U * arLoop);
			break;
			
		case ecSetParamList_UUDelay:
		case ecSetParamList_UDelay:
		case ecSetParamList_LDelay:
		case ecSetParamList_LLDelay:
		case ecSetParamList_DeviEnaLv:
		case ecSetParamList_DeviEnaTime:
		case ecSetParamList_Offset:
		case ecSetParamList_Slope:
			wkLogItmNo += (12U * arLoop);
			if( arParmList == ecSetParamList_Offset )
			{
				wkVal = SubInFlash_OftVal_CnttoVal( wkVal );	/* �J�E���g�������ϊ� */
				arNowVal = SubInFlash_OftVal_CnttoVal( arNowVal );
			}
			else if( arParmList == ecSetParamList_Slope )
			{
				wkVal += 70;
				arNowVal += 70;
			}
			break;
			
		case ecSetParamList_RealTimeCh:
		case ecSetParamList_FstConnCh:
			wkVal += 1U;
			arNowVal += 1U;
			break;
			
		case ecSetParamList_ScalYInt:
		case ecSetParamList_ScalGrad:
			wkVal = SubInFlash_ScaleVal_CnttoVal( wkVal );		/* �J�E���g�������ϊ� */
			if( arParmList == ecSetParamList_ScalYInt )
			{
				arNowVal = gvInFlash.mParam.mScalYInt;
			}
			else
			{
				arNowVal = gvInFlash.mParam.mScalGrad;
			}
			break;
			
		/* �����L�^�͂��̊֐��𔲂��Ă���s��(���^����2�A���K�[���́A�ڑ�����M��) */
		case ecSetParamList_LogCyc2:
		case ecSetParamList_LoggerName:
		case ecSetParamList_CnctEnaGwId:
			return wkVal;
	}
	
	/* �ݒ�ύX���� */
	ApiFlash_QueActHist( wkLogItmNo, (uint32_t)arNowVal, (uint32_t)wkVal, arUser );
	
	return wkVal;
}


/*
 *******************************************************************************
 *	���K�[���̐ݒ�
 *
 *	[���e]
 *		���K�[���̐ݒ�
 *	[����]
 *		uint8_t *ptrLoggerName: �����Ŏ�M�������K�[����
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void SubInFlash_SetLoggerName( uint8_t *ptrLoggerName )
{
	uint8_t		wkLoop;
	uint16_t	wkU16;
	uint16_t	wkSetData;
	ET_ActLogItm_t	wkLogItmNo;

	/* ���K�[���� */
	for( wkLoop = 0U ; wkLoop < imLoggerNameNum ; wkLoop++ )
	{
		/* �ݒ�l�֍X�V */
		if( SubInFlash_SetValWrite_ValChk( ecSetParamList_LoggerName, ptrLoggerName[ wkLoop ], gvInFlash.mParam.mLoggerName[ wkLoop ], gvActQue.mUserID, 0U ) == 1U )
		{
			for( wkLoop = 0U, wkLogItmNo = ecActLogItm_LoggerName1 ; wkLoop < imLoggerNameNum ; wkLoop +=2 )
			{
				wkU16 = (gvInFlash.mParam.mLoggerName[ wkLoop ] << 8U) | (gvInFlash.mParam.mLoggerName[ wkLoop + 1U ]);
				wkSetData = (ptrLoggerName[ wkLoop ] << 8U) | (ptrLoggerName[ wkLoop + 1U ]);
				
				ApiFlash_QueActHist( wkLogItmNo, wkU16, wkSetData, gvActQue.mUserID );												/* �ݒ�ύX���� */
				wkLogItmNo++;
				
				gvInFlash.mParam.mLoggerName[ wkLoop ] = ptrLoggerName[ wkLoop ];
				gvInFlash.mParam.mLoggerName[ wkLoop + 1U ] = ptrLoggerName[ wkLoop + 1U ];
			}
			break;
		}
	}
}


/*
 *******************************************************************************
 *	�ڑ�����M��ݒ�
 *
 *	[���e]
 *		�ڑ�����M��ݒ�
 *	[����]
 *		uint8_t *ptrCommGwID[3]: �����Ŏ�M�����ڑ�����GWID
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void SubInFlash_SetCommGwID( uint8_t (*ptrCommGwID)[3] )
{
	uint8_t		wkLoop;
	uint8_t		wkLoop2;
	uint32_t	wkCnctId_set;
	uint32_t	wkCnctId_now;
	ET_ActLogItm_t	wkLogItmNo;
	
	/* �ڑ����Q�[�g�E�F�CID */
	wkLogItmNo = ecActLogItm_CnctEnaGwId1;
	for( wkLoop = 0U ; wkLoop < 10U ; wkLoop++, wkLogItmNo++ )
	{
		for( wkLoop2 = 0U ; wkLoop2 < 3U ; wkLoop2++ )
		{
			/* �ݒ�l�֍X�V */
			if( SubInFlash_SetValWrite_ValChk( ecSetParamList_CnctEnaGwId, ptrCommGwID[ wkLoop ][ wkLoop2 ], gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ], gvActQue.mUserID, 0U ) == 1U )
			{
				for( wkLoop2 = 0U, wkCnctId_now = 0U, wkCnctId_set = 0U ; wkLoop2 < 3U ; wkLoop2++ )
				{
					wkCnctId_now += (uint32_t)gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
					wkCnctId_set += (uint32_t)ptrCommGwID[ wkLoop ][ wkLoop2 ] << (16U - (wkLoop2 * 8U));
					
					gvInFlash.mParam.mCnctEnaGwId[ wkLoop ][ wkLoop2 ] = ptrCommGwID[ wkLoop ][ wkLoop2 ];
				}
				wkCnctId_now >>= 4U;
				wkCnctId_set >>= 4U;
				
				ApiFlash_QueActHist( wkLogItmNo, wkCnctId_now, wkCnctId_set, gvActQue.mUserID );										/* �ݒ�ύX���� */
				break;
			}
		}
	}
}


/*
 *******************************************************************************
 *	�X�P�[�����O�ݒ�
 *
 *	[���e]
 *		�X�P�[�����O�ݒ�
 *	[����]
 *		uint8_t *ptrOffset: �����Ŏ�M�����X�P�[�����O�I�t�Z�b�g
 *		uint8_t *ptrZerospan: �[���X�p��
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void SubInFlash_SetScale( uint8_t *ptrOffset, uint8_t *ptrZerospan )
{
	uint16_t	wkSetData;
	uint16_t	wkNowDataCnt;
	
	/* �X�P�[�����O�ϊ��ؕ� */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &ptrOffset[ 0U ] );
	wkNowDataCnt = ApiInFlash_ScaleVal_ValtoCnt( gvInFlash.mParam.mScalYInt );
	gvInFlash.mParam.mScalYInt = SubInFlash_SetValWrite_ValChk( ecSetParamList_ScalYInt, wkSetData, wkNowDataCnt, gvActQue.mUserID, 0U );
	
	/* �X�P�[�����O�ϊ��X�� */
	wkSetData = ApiInFlash_1ByteToLittle2Byte( &ptrZerospan[ 0U ] );
	wkNowDataCnt = ApiInFlash_ScaleVal_ValtoCnt( gvInFlash.mParam.mScalGrad );
	gvInFlash.mParam.mScalGrad = SubInFlash_SetValWrite_ValChk( ecSetParamList_ScalGrad, wkSetData, wkNowDataCnt, gvActQue.mUserID, 0U );
}


/*
 *******************************************************************************
 *	�ݒ�ύX�������L�^
 *
 *	[���e]
 *		�ݒ�ύX�������L�^
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubInFlash_SetValTime( void )
{
	uint32_t wkU32;
	
	/* �ݒ�ύX�������L�^ */
	wkU32 = ApiTimeComp_GetLocalTime();
	
	gvInFlash.mParam.mSetvalChgTime[ 0U ] = (uint8_t)((wkU32 >> 24U) & 0x000000FFU);
	gvInFlash.mParam.mSetvalChgTime[ 1U ] = (uint8_t)((wkU32 >> 16U) & 0x000000FFU);
	gvInFlash.mParam.mSetvalChgTime[ 2U ] = (uint8_t)((wkU32 >> 8U) & 0x000000FFU);
	gvInFlash.mParam.mSetvalChgTime[ 3U ] = (uint8_t)(wkU32 & 0x000000FFU);
}

/*
 *******************************************************************************
 *	�x��l�ϊ�(�J�E���^������)
 *
 *	[���e]
 *		�x��l�ϊ�
 *			10�`8191cnt��(-215.0�`603.1��)
 *			����ȊO(-32768�F�x��Ȃ�)
 *	[����]
 *		uint16_t	arAlmValCnt: �x��l�ݒ�J�E���g
 *	[�ߒl]
 *		sint16_t	wkAlmVal: �x��l�ݒ�l
 *******************************************************************************
 */
static sint16_t SubInFlash_AlmVal_CnttoVal( uint16_t arAlmValCnt )
{
	sint16_t wkAlmVal = M_SINT16_MIN;
	
	/* 10�`8191cnt(-215.0�`603.1��) */
	if( arAlmValCnt >= 10U && arAlmValCnt <= 8191 )
	{
		if( gvInFlash.mProcess.mModelCode == ecSensType_V ||
			gvInFlash.mProcess.mModelCode == ecSensType_Pulse )
		{
			wkAlmVal = arAlmValCnt - 50;
		}
		else
		{
			wkAlmVal = arAlmValCnt - 2160;
		}
	}
	
	return wkAlmVal;
}

/*
 *******************************************************************************
 *	�I�t�Z�b�g�l�ϊ�(�������J�E���^)
 *
 *	[���e]
 *		�I�t�Z�b�g�l�ϊ�
 *			-12.5�`12.5����(0�`250cnt)
 *	[����]
 *		sint8_t		arOftVal: �I�t�Z�b�g�l�ݒ�l
 *	[�ߒl]
 *		uint8_t		wkOftValCnt: �I�t�Z�b�g�ݒ�J�E���g
 *******************************************************************************
 */
uint8_t ApiInFlash_OftVal_ValtoCnt( sint8_t arOftVal )
{
	/* -12.5�`12.5��(0�`250cnt) */
	return (uint8_t)(arOftVal + 125);
}

/*
 *******************************************************************************
 *	�I�t�Z�b�g�l�ϊ�(�J�E���^������)
 *
 *	[���e]
 *		�I�t�Z�b�g�l�ϊ�
 *			0�`250cnt��(-12.5�`12.5��)
 *	[����]
 *		uint8_t		arOftValCnt: �I�t�Z�b�g�ݒ�J�E���g
 *	[�ߒl]
 *		sint8_t		wkOftVal: �I�t�Z�b�g�l�ݒ�l
 *******************************************************************************
 */
static sint8_t SubInFlash_OftVal_CnttoVal( uint8_t arOftValCnt )
{
	/* 0�`250cnt(-12.5�`12.5��) */
	return (sint8_t)(arOftValCnt - 125);
}

/*
 *******************************************************************************
 *	�X�P�[�����O�l�ϊ�(�J�E���^������)
 *
 *	[���e]
 *		�X�P�[�����O�l�ϊ�
 *			0�`60000cnt��(-30000�`30000)
 *	[����]
 *		uint16_t	arScaleValCnt: �X�P�[�����O�ݒ�J�E���g
 *	[�ߒl]
 *		sint16_t	wkScaleVal: �X�P�[�����O�l�ݒ�l
 *******************************************************************************
 */
static sint16_t SubInFlash_ScaleVal_CnttoVal( uint16_t arScaleValCnt )
{
	/* 0�`60000cnt(-30000�`30000) */
	return (sint16_t)(arScaleValCnt - 30000);
}

/*
 *******************************************************************************
 *	�x��l�ϊ�(�������J�E���^)
 *
 *	[���e]
 *		�x��l�ϊ�
 *			-215.0�`603.1����(10�`8191cnt)
 *			-32768(�x��Ȃ�)��0cnt
 *	[����]
 *		sint16_t	arAlmVal: �x��l�ݒ�l
 *	[�ߒl]
 *		uint16_t	wkAlmValCnt: �x��l�ݒ�J�E���g
 *******************************************************************************
 */
uint16_t ApiInFlash_AlmVal_ValtoCnt( sint16_t arAlmVal )
{
	uint16_t wkAlmValCnt = 0U;
	
	/* -215.0�`603.1��(10�`8191cnt) */
	if( arAlmVal != M_SINT16_MIN )
	{
		if( gvInFlash.mProcess.mModelCode == ecSensType_V ||
			gvInFlash.mProcess.mModelCode == ecSensType_Pulse )
		{
			wkAlmValCnt = arAlmVal + 50;
		}
		else
		{
			wkAlmValCnt = arAlmVal + 2160;
		}
	}
	
	return wkAlmValCnt;
}

/*
 *******************************************************************************
 *	�X�P�[�����O�l�ϊ�(�������J�E���^)
 *
 *	[���e]
 *		�X�P�[�����O�l�ϊ�
 *			-30000�`30000��(0�`60000cnt)
 *	[����]
 *		sint16_t	arScaleVal: �X�P�[�����O�l�ݒ�l
 *	[�ߒl]
 *		uint16_t	wkScaleValCnt: �X�P�[�����O�ݒ�J�E���g
 *******************************************************************************
 */
uint16_t ApiInFlash_ScaleVal_ValtoCnt( sint16_t arScaleVal )
{
	/* -30000�`30000(0�`60000cnt) */
	return (uint16_t)(30000 + arScaleVal);
}

/*
 *******************************************************************************
 *	1byte�z��f�[�^��Big�G���f�B�A����2byte�ɕϊ�
 *
 *	[���e]
 *		1byte�z��f�[�^��Big�G���f�B�A����2byte�ɕϊ�
 *	[����]
 *		uint8_t *parRfData		:1�o�C�g�f�[�^�擪�A�h���X
 *	[�ߒl]
 *		uint16_t wkRet			:2�o�C�g�f�[�^
 *******************************************************************************
 */
uint16_t ApiInFlash_1ByteToBig2Byte( uint8_t *parRfData )
{
	uint16_t	wkRet;
	
	wkRet = *parRfData << 8U;
	*parRfData++;
	wkRet |= *parRfData;
	
	return wkRet;
}


/*
 *******************************************************************************
 *	1byte�z��f�[�^��little�G���f�B�A����2byte�ɕϊ�
 *
 *	[���e]
 *		1byte�z��f�[�^��little�G���f�B�A����2byte�ɕϊ�
 *	[����]
 *		uint8_t *parRfData		:1�o�C�g�f�[�^�擪�A�h���X
 *	[�ߒl]
 *		uint16_t wkRet			:2�o�C�g�f�[�^
 *******************************************************************************
 */
uint16_t ApiInFlash_1ByteToLittle2Byte( uint8_t *parRfData )
{
	uint16_t	wkRet;
	
	wkRet = *parRfData;
	*parRfData++;
	wkRet |= *parRfData << 8U;
	
	return wkRet;
}

#pragma section
