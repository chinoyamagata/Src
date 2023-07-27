/*
 *******************************************************************************
 *	File name	:	Flash.c
 *
 *	[���e]
 *		�t���b�V������
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2020.7
 *******************************************************************************
 */
#include "cpu_sfr.h"						/* SFR��`�w�b�_�[ */
#include "typedef.h"						/* �f�[�^�^��` */
#include "UserMacro.h"						/* ���[�U�[�쐬�}�N����` */
#include "immediate.h"						/* immediate�萔��` */
#include "switch.h"							/* �R���p�C���X�C�b�`��` */
#include "enum.h"							/* �񋓌^�̒�` */
#include "struct.h"							/* �\���̒�` */
#include "func.h"							/* �֐��̊O���Q��(extern)��` */
#include "glLabel.h"						/* �O���[�o���ϐ��E�萔(const)���x����` */
#include "RF_Struct.h"

#include <string.h>


/*
 *==============================================================================
 *	define��`
 *==============================================================================
 */
#define	P_FLASH_POWER		(P4_bit.no5)			/* �t���b�V���d������ */
#define	P_FLASH_POWER_IO	(PM4_bit.no5)			/* �t���b�V���d������|�[�gIO */

#define imRetryMax				5U					/* ���g���CMAX */
#define imEraseRetryMax			3000U				/* �C���[�X���g���CMAX�A261cnt:28.8msec=Max:300msec -> Max=2718cnt */


//#define imFlashQue_DatNum		24U					/* �f�[�^�L���[�̃f�[�^�� */
#define imFlashQue_DatNum		72U					/* �f�[�^�L���[�̃f�[�^�� */
//#define imFlashQueLowBat_DatNum	48U					/* ��d�����p�f�[�^�L���[�̃f�[�^�� */
#define imFlashQue_DatSize		13U					/* �f�[�^�L���[�̃f�[�^�T�C�Y(Byte) */
#define imFlashRdWr_DatSize		13U					/* ���[�h���C�g�f�[�^�T�C�Y(Byte) */
#define imFlashSctrSize			4096UL				/* �Z�N�^�T�C�Y(Byte) */

/* 1�Z�N�^�̃T�C�Y�� */
#define	imFlashErrInfoSctrSize	2500U				/* �ُ��� */
#define	imFlashMeasAlmSctrSize	3500U				/* �v���x�� */
#define	imFlashMeas1SctrSize	4092U				/* �v���l1 */
#define	imFlashMeas2SctrSize	4092U				/* �v���l2 */
#define	imFlashEvtSctrSize		3600U				/* �C�x���g */
#define	imFlashActLogSctrSize	4050U				/* ���엚�� */
#define	imFlashSysLogSctrSize	4096UL				/* �V�X�e�����O */
#define	imFlashAdrTblSctrSize	4096UL				/* �A�h���X����e�[�u�� */

/* 1�Z�N�^�̃f�[�^�� */
#define	imDataNumInSctrErrInf	500U				/* �ُ��� */
#define	imDataNumInSctrMeasAlm	500U				/* �v���x�� */
#define	imDataNumInSctrMeas1	341U				/* �v���l1 */
#define	imDataNumInSctrMeas2	341U				/* �v���l2 */
#define	imDataNumInSctrEvt		300U				/* �C�x���g */
#define	imDataNumInSctrActLog	450U				/* ���엚�� */
#define	imDataNumInSctrSysLog	512U				/* �V�X�e�����O */
#define	imDataNumInSctrAdrTbl	512U				/* �A�h���X����e�[�u�� */


/* �g�p�Z�N�^��MAX�l(�\���Z�N�^�͊܂߂�) */
#define imSctrNumErrInf			1U					/* �ُ��� */
#define imSctrNumMeasAlm		1U					/* �v���x�� */
#define imSctrNumMeas1			156U				/* �v���l1 */
#define imSctrNumMeas2			3U					/* �v���l2 */
#define imSctrNumEvt			1U					/* �C�x���g */
#define imSctrNumActLog			1U					/* ���엚�� */
#define imSctrNumSysLog			4U					/* �V�X�e�����O */
#define	imSctrNumAdrTbl			3U					/* �A�h���X����e�[�u��(�\���Z�N�^�܂�) */


/* �Z�N�^No. */
#define imSctrNoMinErrInf		0U					/* �ُ��� */
#define imSctrNoMaxErrInf		1U
#define imSctrNoMinMeasAlm		2U					/* �v���x�� */
#define imSctrNoMaxMeasAlm		3U
#define imSctrNoMinMeas1		4U					/* �v���l1 */
#define imSctrNoMaxMeas1		160U
#define imSctrNoMinMeas2		161U				/* �v���l2 */
#define imSctrNoMaxMeas2		164U
#define imSctrNoMinEvt			165U				/* �C�x���g */
#define imSctrNoMaxEvt			166U
#define imSctrNoMinActLog		167U				/* ���엚�� */
#define imSctrNoMaxActLog		168U
#define imSctrNoMinSysLog		169U				/* �V�X�e�����O */
#define imSctrNoMaxSysLog		173U
#define	imSctrNoMinMeas1Adr		174U				/* �A�h���X����e�[�u���F�v���l1 */
#define	imSctrNoMaxMeas1Adr		176U
#define	imSctrNoMinMeas2Adr		177U				/* �A�h���X����e�[�u���F�v���l2 */
#define	imSctrNoMaxMeas2Adr		179U
#define	imSctrNoMinSysLogAdr	180U				/* �A�h���X����e�[�u���F�V�X�e�����O */
#define	imSctrNoMaxSysLogAdr	182U


/* ��2�G���A�Z�N�^�J�n�A�h���X */
#define imSctr2ndStAdrErrInf	0x1000				/* �ُ��� */
#define imSctr2ndStAdrMeasAlm	0x3000				/* �v���x�� */
#define imSctr2ndStAdrMeas1		0xA0000				/* �v���l */
#define imSctr2ndStAdrEvt		0xA6000				/* �C�x���g */
#define imSctr2ndStAdrActLog	0xA8000				/* ���엚�� */
#define imSctr2ndStAdrSysLog	0xAD000				/* �V�X�e������ */


/* �f�[�^�T�C�Y */
#define imDataByteErrInf		5U					/* �ُ��� */
#define imDataByteMeasAlm		7U					/* �v���x�� */
#define imDataByteMeas1			12U					/* �v���l1 */
#define imDataByteMeas2			12U					/* �v���l2 */
#define imDataByteEvt			12U					/* �C�x���g */
#define imDataByteActLog		9U					/* ���엚�� */
#define imDataByteSysLog		8U					/* �V�X�e�����O */
#define	imDataByteAdrTbl		8U					/* �A�h���X����e�[�u�� */


/* �Z�N�^�I�[�A�h���X */
#define imEndAddrErrInf			0x9C3U				/* �ُ��� */
#define imEndAddrMeasAlm		0xDABU				/* �v���x�� */
#define imEndAddrMeas1			0xFFBU				/* �v���l1 */
#define imEndAddrMeas2			0xFFBU				/* �v���l2 */
#define imEndAddrEvt			0xE0FU				/* �C�x���g */
#define imEndAddrActLog			0xFD1U				/* ���엚�� */
#define imEndAddrSysLog			0xFFFU				/* �V�X�e�����O */
#define	imEndAddrAdrTbl			0xFFFU				/* �A�h���X����e�[�u�� */


/* �Z�N�^�I�[�̖��g�p�T�C�Y */
#define imRsvSizeErrInf		1596U					/* �ُ��� */
#define imRsvSizeMeasAlm	596U					/* �v���x�� */
#define imRsvSizeMeas1		4U						/* �v���l1 */
#define imRsvSizeMeas2		4U						/* �v���l2 */
#define imRsvSizeEvt		496U					/* �C�x���g */
#define imRsvSizeActLog		46U						/* ���엚�� */
#define imRsvSizeSysLog		0U						/* �V�X�e�����O */
#define	imRsvSizeAdrTbl		0U						/* �A�h���X����e�[�u�� */

#define imNonRetry			10U						/* ���g���C���� */



/*
 *==============================================================================
 *	�񋓌^��`
 *==============================================================================
 */
typedef enum ET_SrchKindLog
{
	ecSrchKindLog_ErrInfo = 0U,						/* �ُ��� */
	ecSrchKindLog_MeasAlm,							/* �v���x�� */
	ecSrchKindLog_Evt,								/* �C�x���g */
	ecSrchKindLog_ActLog,							/* ���엚�� */
	ecSrchKindLog_AdrTblMeas1,						/* �A�h���X�e�[�u���F�v���l1 */
	ecSrchKindLog_AdrTblSysLog,						/* �A�h���X�e�[�u���F�V�X�e�����O */
	
	ecSrchKindLogMax								/* enum�ő�l */
} ET_SrchKindLog_t;

typedef enum ET_ReadFlash
{
	ecReadFlash_OK = 0U,							/* ���� */
	ecReadFlash_IndexNoErr,							/* IndexNo�G���[ */
	ecReadFlash_ReadErr,							/* �ǂݏo���G���[ */
	
	ecReadFlashMax									/* enum�ő�l */
} ET_ReadFlash_t;


/*
 *==============================================================================
 *	�\���̒�`
 *==============================================================================
 */
/* �f�[�^�L���[ */
typedef struct ST_FlashQue
{
	uint8_t			mData[imFlashQue_DatNum][imFlashQue_DatSize];				/* �f�[�^ */
	uint8_t			mHead;														/* �i�[�ʒu�̐擪 */
	uint8_t			mNum;														/* �i�[�� */
} ST_FlashQue_t;

#if 0
/* ��d�����p�f�[�^�L���[ */
typedef struct ST_FlashQueLowBat
{
	uint8_t			mData[imFlashQueLowBat_DatNum][imFlashQue_DatSize];			/* �f�[�^ */
	uint8_t			mHead;														/* �i�[�ʒu�̐擪 */
	uint8_t			mNum;														/* �i�[�� */
} ST_FlashQueLowBat_t;
#endif

/* �t���b�V�����[�h */
typedef struct ST_FlashRead
{
	uint32_t		mStartTime;
	uint32_t		mEndTime;
	uint16_t		mIndex;
} ST_FlashRead_t;

/* �̈��f�[�^ */
typedef struct ST_RegionData
{
	uint32_t		mStartAddr;							/* �J�n�A�h���X */
	uint8_t			mWrData[imFlashRdWr_DatSize];		/* �������݃f�[�^ */
	ET_RegionKind_t	mRegionKind;						/* �̈�� */
	uint16_t		mWrNum;								/* �������݌��� */
	uint16_t		mDataByteNum;						/* �f�[�^�o�C�g�� */
	uint32_t		mWrAddrEnd;							/* �������ݏI�[�A�h���X */
	uint32_t		mWrAddrFin;							/* �������ݍŏI�A�h���X */
	uint16_t		mIndexNo;							/* IndexNo. */
} ST_RegionData_t;

/* ���[�h���N�G�X�g */
typedef struct ST_RequestReadData
{
	uint8_t			mReq;					/* ���[�h�v���t���O */
	uint16_t		mFin;					/* ���[�h�����t���O */
} ST_RequestReadData_t;


typedef struct ST_Region
{
	ET_RegionKind_t	mRegionKind;			/* �̈��� */
	uint16_t		mDataNumMax;			/* ���^�f�[�^Max�� */
	uint16_t		mDataByte;				/* �f�[�^�T�C�Y */
	uint16_t		mIndexMax;				/* IndexNo.�̍ő吔 */
	uint16_t		mRsvSize;				/* �Z�N�^�I�[�̖��g�p�T�C�Y */
	uint16_t		mSctrMin;				/* �ŏ��Z�N�^ */
	uint16_t		mSctrMax;				/* �ő�Z�N�^ */
	uint16_t		mSctrNum;				/* �g�p�Z�N�^�� */
	uint16_t		mDataNumInSctr;			/* 1�Z�N�^�̃f�[�^�� */
	uint32_t		m2ndStAdr;				/* ��2�G���A�Z�N�^�J�n�A�h���X */
} ST_Region_t;


/*
 *==============================================================================
 *	�ϐ���`
 *==============================================================================
 */
static ET_FlashSts_t			stFlashSts = ecFlashSts_PowOn;					/* �t���b�V��������� */

static ST_FlashQue_t			stFlashQue;										/* �f�[�^�L���[ */

//static ST_FlashQueLowBat_t		stFlashQueLowBat;								/* ��d�����p�f�[�^�L���[ */

/* Write�L���[�f�[�^�쐬�p */
static ST_FlashVal1Evt_t		stFlashWrMeasVal1;								/* �v���l1 */

/* Read�L���[�f�[�^�쐬�p */
static ST_FlashVal1Evt_t		stFlashRdMeasVal1[ 9U ];						/* �v���l1 */
static ST_FlashMeasAlm_t		stFlashRdMeasAlmVal[ 6U ];						/* �v���x�񗚗� */


static ST_RegionData_t			stRegionData;									/* �̈��f�[�^ */
static ST_RequestReadData_t		stFlashReqRd = { 0U, 0U };						/* Flash���[�h���N�G�X�g */

static uint16_t					vPrimRetryCnt = 0U;								/* �v���C�}���p���g���C�J�E���^ */
static uint16_t					vSecondRetryCnt = 0U;							/* �Z�J���_���p���g���C�J�E���^ */

static ET_ReadFlash_t			vReadFlash = ecReadFlash_OK;					/* �t���b�V���ǂݏo������ */


/*
 *==============================================================================
 *	�萔��`
 *==============================================================================
 */
 
const ST_Region_t	cRegionKindInfo[ 6U ] =
{
	{ ecRegionKind_ErrInfo,	imDataNumErrInf,	imDataByteErrInf,	imIndexMaxErrInf,	imRsvSizeErrInf,	imSctrNoMinErrInf,	imSctrNoMaxErrInf,	imSctrNumErrInf + 1U,	imDataNumInSctrErrInf,	imSctr2ndStAdrErrInf	},
	{ ecRegionKind_MeasAlm,	imDataNumMeasAlm,	imDataByteMeasAlm,	imIndexMaxMeasAlm,	imRsvSizeMeasAlm,	imSctrNoMinMeasAlm,	imSctrNoMaxMeasAlm,	imSctrNumMeasAlm + 1U,	imDataNumInSctrMeasAlm,	imSctr2ndStAdrMeasAlm	},
	{ ecRegionKind_Meas1,	imDataNumMeas1,		imDataByteMeas1,	imIndexMaxMeas1,	imRsvSizeMeas1,		imSctrNoMinMeas1,	imSctrNoMaxMeas1,	imSctrNumMeas1 + 1U,	imDataNumInSctrMeas1, 	imSctr2ndStAdrMeas1		},
	{ ecRegionKind_Event,	imDataNumEvt,		imDataByteEvt,		imIndexMaxEvt,		imRsvSizeEvt,		imSctrNoMinEvt,		imSctrNoMaxEvt,		imSctrNumEvt + 1U,		imDataNumInSctrEvt,		imSctr2ndStAdrEvt		},
	{ ecRegionKind_ActLog,	imDataNumActLog,	imDataByteActLog,	imIndexMaxActLog,	imRsvSizeActLog,	imSctrNoMinActLog,	imSctrNoMaxActLog,	imSctrNumActLog + 1U,	imDataNumInSctrActLog,	imSctr2ndStAdrActLog	},
	{ ecRegionKind_SysLog,	imDataNumSysLog,	imDataByteSysLog,	imIndexMaxSysLog,	imRsvSizeSysLog,	imSctrNoMinSysLog,	imSctrNoMaxSysLog,	imSctrNumSysLog + 1U,	imDataNumInSctrSysLog,	imSctr2ndStAdrSysLog	}
};


/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
 */
static uint16_t SubFlash_Retry( uint16_t arRetryMax, uint16_t arRetryNum );
static uint16_t SubFlash_ReadRetry( uint16_t arRetryNum, uint8_t arQueueData[] );
static void SubFlash_AbnSet( ET_FlashKind_t arFlashKind );
static void SubFlash_RetryCntUpDate( uint16_t arRetryMax, ET_Error_t arPrimSts, ET_Error_t arSecondSts );
static void SubFlash_NewestHistTimeSet( uint32_t arTime );
static void SubFlash_EnQueue( uint8_t *parData );								/* �G���L���[���� */
static void SubFlash_DeQueue( uint8_t *parData );								/* �f�L���[���� */
static uint8_t SubFlash_GetQueue( uint8_t *parData, uint8_t *parNum );			/* �L���[�擾���� */
static ET_RegionKind_t SubFlash_QueDataRegionKindGet( const uint8_t arQueData );
static void SubFlash_GetRegionData( ST_RegionData_t *parRegionData, ET_RegionKind_t arRegionKind );
static ET_Error_t SubFlash_ReadData( uint8_t *parQueData, ET_RegionKind_t arRegionData );
static uint16_t SubFlash_SetReadData( uint8_t arReadData[], ET_RegionKind_t arRegionKind, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex, uint8_t arRfMode );
static uint32_t SubFlash_MakeTime4Byte( const uint8_t *parTimeData );
static void SubFlash_MakeWriteData( const uint8_t *parQueData, uint8_t arWrData[], ET_RegionKind_t arRegionKind );
static uint8_t SubFlash_SctrErsJudge( uint32_t arAddrEnd, uint32_t arAddrFin, ET_RegionKind_t arRegionKind );
static void SubFlash_FinAdrSet( uint32_t arAddrEnd, uint32_t arAddrFin, ET_RegionKind_t arRegionKind );
static ET_Error_t SubFlash_Write( uint32_t arAddr, uint8_t *parWrDataBuff, uint32_t arDataSize);
static ET_Error_t SubFlash_Read( uint32_t arAddr, uint8_t *parRdDataBuff, uint32_t arDataSize );
static void SubFlash_WrEndAddrUpdate( uint32_t arWrFinAddr );
static void SubFlash_WrNumIndexUpdate( uint8_t arSctrChg );
static void SubFlash_RfRTMeasDataSet( void );
static uint32_t SubFlash_ReMakeTime( uint8_t arWrData[] );
static void SubFlash_ReadQueSetPastTime( ET_RegionKind_t arKind );


#pragma section text MY_APP2
/*
 *******************************************************************************
 *	�t���b�V������ ����������
 *
 *	[���e]
 *		�t���b�V������ ����������
 *******************************************************************************
 */
void ApiFlash_Initial( void )
{
	/* �f�[�^������ */
	memset( &stFlashQue, 0, sizeof(stFlashQue) );
	
	/* �v���l1�f�[�^������ */
	memset( &stFlashWrMeasVal1, 0, sizeof(stFlashWrMeasVal1) );
	
	/* ���A���^�C���ŐV�f�[�^������ */
	memset( &gvRfRTMeasData, 0, sizeof(gvRfRTMeasData) );
	
	P_FLASH_POWER_IO = (uint8_t)imLow;											/* �t���b�V���d������|�[�g�F�o�� */
	/* CS:Lo */
	ApiFlashDrv_CSCtl( ecFlashKind_Prim, imLow );
	ApiFlashDrv_CSCtl( ecFlashKind_Second, imLow );
	ApiFlash_FlashPowerCtl( imOFF, imOFF );
}


/*
 *******************************************************************************
 *	�t���b�V������ ���C������
 *
 *	[���e]
 *		�t���b�V������ ���C������
 *******************************************************************************
 */
void ApiFlash_Main( void )
{
	uint8_t			wkLoop;
	uint8_t			wkQueNum;													/* �L���[�f�[�^�� */
	uint8_t			wkStsPrim;													/* �v���C�}���̃X�e�[�^�X���W�X�^1 */
	uint8_t			wkStsSecond;												/* �Z�J���_���̃X�e�[�^�X���W�X�^1 */
	uint8_t			wkSctErsInfo;												/* �Z�N�^�������� */
	uint8_t			wkDataInfo;													/* �f�[�^��� */
	uint32_t		wkDataSize;													/* �f�[�^�T�C�Y��(�O���Z�N�^/�㔼�Z�N�^�������ݗp) */
	uint32_t		wkStartAddr;												/* �J�n�A�h���X */
	ET_RegionKind_t	wkRegionKind;												/* �̈�� */
	ET_Error_t		wkError;													/* �G���[��� */
	ET_Error_t		wkErrorSecond;												/* �G���[��� */
	ET_Error_t		wkPrimReg1Ret;												/* �v���C�}���̃G���[��� */
	ET_Error_t		wkSecReg1Ret;												/* �Z�J���_���̃G���[��� */
	
	static uint8_t	wkQueData[ imFlashQue_DatSize ];							/* �L���[�f�[�^ */
	static uint8_t	vRetryNum = 0U;												/* ���g���C�� */
	static uint16_t	vEraseRetryNum = 0U;										/* �C���[�X���g���C�� */
	static uint8_t	vWrData[ imFlashRdWr_DatSize ];								/* �������݃f�[�^ */
	
	switch( stFlashSts )
	{
		/* Flash�d��ON */
		case ecFlashSts_PowOn:
			vPrimRetryCnt = 0U;
			vSecondRetryCnt = 0U;
			vRetryNum = 0U;
			vEraseRetryNum = 0U;
			
			/* Flash�d��ON */
			ApiFlash_FlashPowerCtl( imON, imOFF );
			R_TAU0_Channel5_Start();
			stFlashSts = ecFlashSts_PowOnWait;
			break;
			
		/* Flash�d������҂� */
		case ecFlashSts_PowOnWait:
			break;
			
		/* ���� */
		case ecFlashSts_Init:
#if 0	/* �d�C�I��OFF���Ă���̂ł��Ȃ��Ă��悢 */
			/* Power-down�����[�X */
			wkError = ApiFlashDrv_ReleasePowerDown( ecFlashKind_Prim );
			wkError = ApiFlashDrv_ReleasePowerDown( ecFlashKind_Second );
#endif
			/* �L���[�擾 */
			wkDataInfo = SubFlash_GetQueue( &wkQueData[0], &wkQueNum );
			/* �L���[������ */
			if( wkDataInfo == imARI )
			{
				/* �̈��擾 */
				wkRegionKind = SubFlash_QueDataRegionKindGet( wkQueData[ imFlashQue_DatSize - 1U ] );
				
				/* �̈��f�[�^�擾 */
				SubFlash_GetRegionData( &stRegionData, wkRegionKind );
				
				/* �������݂̏ꍇ */
				if( M_TSTBIT( wkQueData[imFlashQue_DatSize - 1U], M_BIT0 ) )
				{
					/* �J�n�A�h���X�X�V */
					if(( stRegionData.mWrAddrEnd % imFlashSctrSize )== 0U )
					{
						stRegionData.mStartAddr = stRegionData.mWrAddrEnd;
					}
					else
					{
						stRegionData.mStartAddr = stRegionData.mWrAddrEnd + 1U;
					}
					
					/* �������݃f�[�^������ */
					for( wkLoop = 0U; wkLoop < imFlashRdWr_DatSize; wkLoop++ )
					{
						vWrData[ wkLoop ] = 0U;
					}
					/* �������݃f�[�^�쐬���� */
					SubFlash_MakeWriteData( wkQueData, &vWrData[0], stRegionData.mRegionKind );
					
					/* �Z�N�^�������菈�� */
					wkSctErsInfo = SubFlash_SctrErsJudge( stRegionData.mWrAddrEnd, stRegionData.mWrAddrFin, wkRegionKind );
					
					/* �Z�N�^�������� */
					if( wkSctErsInfo == imNASI )
					{
						/* �������ݏ������s */
						wkError = SubFlash_Write( stRegionData.mStartAddr, &vWrData[0], (uint32_t)stRegionData.mDataByteNum );
						/* �v���C�}���A�Z�J���_���ǂ��炩�������ݐ��� */
						if( wkError == ecERR_OK )
						{
							/* �̈�큁�v���l1�̏ꍇ */
							if( stRegionData.mRegionKind == ecRegionKind_Meas1 )
							{
								/* ��قǏ������񂾃f�[�^���玞�������擾 */
								gvInFlash.mData.mNewestLogTime = SubFlash_ReMakeTime( vWrData );
							}
							SubFlash_WrNumIndexUpdate( imNASI );					/* �������݌����AIndexNo.��̈�했�ɍX�V */
						}
						else
						{
							/* ���A�h���X�ւ̏������ݗp�Ɋe��X�V */
							stRegionData.mWrAddrEnd = (stRegionData.mStartAddr + (uint32_t)stRegionData.mDataByteNum) - 1U;
							SubFlash_WrEndAddrUpdate( stRegionData.mWrAddrEnd );	/* �������ݏI�[�A�h���X��̈�했�ɍX�V */
						}
						ApiFlash_FlashPowerCtl( imOFF, imOFF );						/* Flash�d��OFF */
						SubFlash_DeQueue( wkQueData );								/* �f�L���[���� */
						stFlashSts = ecFlashSts_PowOn;
					}
					/* �Z�N�^�����L�� */
					else
					{
						/* �������ݍŏI�A�h���X�̃Z�b�g */
						SubFlash_FinAdrSet( stRegionData.mWrAddrEnd, stRegionData.mWrAddrFin, wkRegionKind );
						
						/* �Z�N�^�������� */
						/* �v���C�}���̃Z�N�^������ */
						wkError = ApiFlashDrv_SectorErase( stRegionData.mWrAddrFin, ecFlashKind_Prim );
						
						/* �Z�J���_���̃Z�N�^������ */
						wkErrorSecond = ApiFlashDrv_SectorErase( stRegionData.mWrAddrFin, ecFlashKind_Second );
						
						if( (wkError == ecERR_OK) && (wkErrorSecond == ecERR_OK) )
						{
							stFlashSts = ecFlashSts_SctrErase;					/* ��Ԃ��Z�N�^�������� */
						}
						else
						{
							SubFlash_RetryCntUpDate( imRetryMax, wkError, wkErrorSecond );
							vRetryNum = SubFlash_Retry( imRetryMax, vRetryNum );
																				/* ���g���C�񐔊Ď� */
							if( vRetryNum >= imRetryMax )						/* �ő僊�g���C�񐔈ȏ� */
							{
								stFlashSts = ecFlashSts_SctrErase;				/* ��Ԃ��Z�N�^�������� */
							}
						}
					}
				}
				/* �ǂݏo���̏ꍇ */
				else
				{
					vReadFlash = ecReadFlash_OK;								/* ���� */
					wkError = SubFlash_ReadData( wkQueData, wkRegionKind );
					
					if( wkError == ecERR_OK )
					{
						/* Flash�d��OFF */
						ApiFlash_FlashPowerCtl( imOFF, imOFF );
						SubFlash_DeQueue( wkQueData );							/* �f�L���[���� */
						stFlashSts = ecFlashSts_PowOn;
					}
					else
					{
						/* �ǂݏo���G���[�̏ꍇ�̂݁A���g���C������s�� */
						if( vReadFlash == ecReadFlash_ReadErr )
						{
							/* ���g���C�񐔊Ď� */
							vRetryNum = SubFlash_ReadRetry( vRetryNum, wkQueData );
						}
						else if( vReadFlash == ecReadFlash_IndexNoErr ) 
						{														/* IndexNo�G���[ */
							M_SETBIT( stFlashReqRd.mFin, M_BIT15 );				/* Flash���[�h���s */
							ApiFlash_FlashPowerCtl( imOFF, imOFF );
							SubFlash_DeQueue( wkQueData );						/* �f�L���[���� */
							stFlashSts = ecFlashSts_PowOn;
						}
						/* ���邱�Ƃ��Ȃ����������O�ׁ̈A���������s�� */
						else
						{
							ApiFlash_FlashPowerCtl( imOFF, imOFF );
							SubFlash_DeQueue( wkQueData );						/* �f�L���[���� */
							stFlashSts = ecFlashSts_PowOn;
						}
					}
				}
			}
			/* �f�[�^���� */
			else
			{
				/* Flash�d��OFF */
				ApiFlash_FlashPowerCtl( imOFF, imOFF );
				gvModuleSts.mExFlash = ecExFlashModuleSts_Sleep;
				stFlashSts = ecFlashSts_PowOn;
			}
			break;
			
		/* �Z�N�^������ */
		case ecFlashSts_SctrErase:
			/* �v���C�}���̃X�e�[�^�X���W�X�^1�ǂݏo�� */
			wkPrimReg1Ret = ApiFlashDrv_ReadStatusReg1( &wkStsPrim, ecFlashKind_Prim );
			
			/* �Z�J���_���̃X�e�[�^�X���W�X�^1�ǂݏo�� */
			wkSecReg1Ret = ApiFlashDrv_ReadStatusReg1( &wkStsSecond, ecFlashKind_Second );
			
			/* �Z�N�^�������� */
			if( (!M_TSTBIT( wkStsPrim, imFlashDrv_Sts1Bit_WEL )) && (wkPrimReg1Ret == ecERR_OK) &&
				(!M_TSTBIT( wkStsSecond, imFlashDrv_Sts1Bit_WEL )) && (wkSecReg1Ret == ecERR_OK) )
			{
				stFlashSts = ecFlashSts_FirstSctrWr;							/* ��Ԃ�O���Z�N�^�������ݒ��� */
			}
			/* �Z�N�^���������� */
			else
			{
				/* �v���C�}������ */
				if( (M_TSTBIT( wkStsPrim, imFlashDrv_Sts1Bit_WEL )) || (wkPrimReg1Ret == ecERR_NG) )
				{
					wkError = ecERR_NG;
				}
				else
				{
					wkError = ecERR_OK;
				}
				
				/* �Z�J���_������ */
				if( (M_TSTBIT( wkStsSecond, imFlashDrv_Sts1Bit_WEL )) || (wkSecReg1Ret == ecERR_NG) )
				{
					wkErrorSecond = ecERR_NG;
				}
				else
				{
					wkErrorSecond = ecERR_OK;
				}
				
				SubFlash_RetryCntUpDate( imEraseRetryMax, wkError, wkErrorSecond );
				vEraseRetryNum = SubFlash_Retry( imEraseRetryMax, vEraseRetryNum );
																				/* �Z�N�^���g���C�񐔊Ď� */
				if( vEraseRetryNum >= imEraseRetryMax )							/* �ő僊�g���C�񐔈ȏ� */
				{
					stFlashSts = ecFlashSts_FirstSctrWr;						/* ��Ԃ�O���Z�N�^�������ݒ��� */
				}
			}
			break;
			
		/* �O���Z�N�^�������ݒ� */
		case ecFlashSts_FirstSctrWr:
			wkDataSize = stRegionData.mDataByteNum;
			wkStartAddr = stRegionData.mWrAddrFin;
			
			if( wkDataSize == 0U )
			{
				/* Flash�d��OFF */
				ApiFlash_FlashPowerCtl( imOFF, imOFF );
				stFlashSts = ecFlashSts_PowOn;
			}
			else
			{
				/* �������ݏ������s */
				wkError = SubFlash_Write( wkStartAddr, &vWrData[0], wkDataSize );
				/* �v���C�}���A�Z�J���_���ǂ��炩�������ݐ��� */
				if( wkError == ecERR_OK )
				{
					/* �̈�큁�v���l1�̏ꍇ */
					if( stRegionData.mRegionKind == ecRegionKind_Meas1 )
					{
						/* ��قǏ������񂾃f�[�^���玞�������擾 */
						gvInFlash.mData.mNewestLogTime = SubFlash_ReMakeTime( vWrData );
					}
					SubFlash_WrNumIndexUpdate( imARI );						/* �������݌����AIndexNo.��̈�했�ɍX�V */
				}
				else
				{
					/* ���A�h���X�ւ̏������ݗp�Ɋe��X�V */
					stRegionData.mWrAddrEnd = (wkStartAddr + (uint32_t)stRegionData.mDataByteNum) - 1U;
					SubFlash_WrEndAddrUpdate( stRegionData.mWrAddrEnd );	/* �������ݏI�[�A�h���X��̈�했�ɍX�V */
				}
				ApiFlash_FlashPowerCtl( imOFF, imOFF );						/* Flash�d��OFF */
				SubFlash_DeQueue( wkQueData );								/* �f�L���[���� */
				stFlashSts = ecFlashSts_PowOn;
			}
			break;
			
		default:
			break;
	}
}


/*
 *******************************************************************************
 * ���g���C�񐔊Ď�
 *
 *	[���e]
 *		���g���C�񐔊Ď�
 *	[����]
 *		uint16_t	arRetryMax: ���g���C�ő��
 *		uint16_t	arRetryNum: ���g���C��
 *	[�ߒl]
 *		uint8_t	arRetryNum: ���g���C��
 *******************************************************************************
 */
static uint16_t SubFlash_Retry( uint16_t arRetryMax, uint16_t arRetryNum )
{
	arRetryNum++;																/* ���g���C�񐔍X�V */
	
	/* ���g���C�񐔃I�[�o�[ */
	if( arRetryNum >= arRetryMax )
	{
		/* �O�t���t���b�V���G���[�Z�b�g���� */
		
		/* �v���C�}�������g���C */
		if( vPrimRetryCnt == M_UINT16_MAX )
		{
			/* �v���C�}���������ُ픭�� */
			ApiAbn_AbnStsSet( imAbnSts_PMEM, ecAbnInfKind_AbnSts );
		}
		
		/* �Z�J���_�������g���C */
		if( vSecondRetryCnt == M_UINT16_MAX )
		{
			/* �Z�J���_���������ُ픭�� */
			ApiAbn_AbnStsSet( imAbnSts_SMEM, ecAbnInfKind_AbnSts );
		}
	}
	else
	{
		;															/* ������ */
	}
	
	return arRetryNum;
}


/*
 *******************************************************************************
 * �ǂݏo���p���g���C�񐔊Ď�
 *
 *	[���e]
 *		�O�t���t���b�V���ւ̓ǂݏo�����g���C�񐔊Ď�
 *	[����]
 *		uint16_t	arRetryNum: ���g���C��
 *	[�ߒl]
 *		uint8_t	arRetryNum: ���g���C��
 *******************************************************************************
 */
static uint16_t SubFlash_ReadRetry( uint16_t arRetryNum, uint8_t arQueueData[] )
{
	arRetryNum++;																/* ���g���C�񐔍X�V */
	
	/* ���g���C�񐔃I�[�o�[ */
	if( arRetryNum >= imRetryMax )
	{
		M_SETBIT( stFlashReqRd.mFin, M_BIT15 );				/* Flash���[�h���s */
		SubFlash_DeQueue( arQueueData );					/* �f�L���[���� */
		ApiFlash_FlashPowerCtl( imOFF, imOFF );
		stFlashSts = ecFlashSts_PowOn;
		arRetryNum = 0U;									/* ���g���C�񐔏����� */
		
		/* �O�t���t���b�V���G���[�Z�b�g���� */
		
		/* �v���C�}�������g���C */
		if( vPrimRetryCnt == imRetryMax )
		{
			/* �v���C�}���������ُ픭�� */
			ApiAbn_AbnStsSet( imAbnSts_PMEM, ecAbnInfKind_AbnSts );
		}
		
		/* �Z�J���_�������g���C */
		if( vSecondRetryCnt == imRetryMax )
		{
			/* �Z�J���_���������ُ픭�� */
			ApiAbn_AbnStsSet( imAbnSts_SMEM, ecAbnInfKind_AbnSts );
		}
		
		vPrimRetryCnt = 0U;
		vSecondRetryCnt = 0U;
	}
	else
	{
		;													/* ������ */
	}
	
	return arRetryNum;
}


/*
 *******************************************************************************
 * �v���C�}���Z�J���_���p���g���C�񐔍X�V
 *
 *	[���e]
 *		�v���C�}���Z�J���_���p���g���C�񐔍X�V
 *	[����]
 *		uint16_t	arRetryMax: ���g���C�ő��
 *		ET_Error_t	arPrimSts: �v���C�}���������A�N�Z�X�̃G���[���
 *		ET_Error_t	arSecondSts: �Z�J���_���������A�N�Z�X�̃G���[���
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubFlash_RetryCntUpDate( uint16_t arRetryMax, ET_Error_t arPrimSts, ET_Error_t arSecondSts )
{
	/* �v���C�}������ */
	if( arPrimSts == ecERR_NG )
	{
		vPrimRetryCnt++;											/* �v���C�}���p���g���C�J�E���g�X�V */
		if( vPrimRetryCnt >= arRetryMax )
		{
			vPrimRetryCnt = M_UINT16_MAX;
		}
	}
	else
	{
		vPrimRetryCnt = 0U;											/* ���g���C������ԂɈڍs */
	}
	
	/* �Z�J���_������ */
	if( arSecondSts == ecERR_NG )
	{
		vSecondRetryCnt++;											/* �Z�J���_���p���g���C�J�E���g�X�V */
		if( vSecondRetryCnt >= arRetryMax )
		{
			vSecondRetryCnt = M_UINT16_MAX;
		}
	}
	else
	{
		vSecondRetryCnt = 0U;										/* ���g���C������ԂɈڍs */
	}
}


/*
 *******************************************************************************
 * �t���b�V���G���[�Z�b�g
 *
 *	[���e]
 *		�t���b�V���G���[�Z�b�g
 *	[����]
 *		ET_FlashKind_t	arFlashKind: �t���b�V��IC
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubFlash_AbnSet( ET_FlashKind_t arFlashKind )
{
	if( arFlashKind == ecFlashKind_Prim )
	{
		/* �v���C�}���������ُ픭�� */
		ApiAbn_AbnStsSet( imAbnSts_PMEM, ecAbnInfKind_AbnSts );
	}
	else
	{
		/* �Z�J���_���������ُ픭�� */
		ApiAbn_AbnStsSet( imAbnSts_SMEM, ecAbnInfKind_AbnSts );
	}
}


/*
 *******************************************************************************
 *	�ُ���̏������ݏ���
 *
 *	[���e]
 *		�ُ���̏������ݏ���
 *	[����]
 *		ET_ErrInfItm_t	arItem: �x�񍀖�
 *		uint8_t			arAlmFlg: �A���[���t���O
 *******************************************************************************
 */
void ApiFlash_WriteErrInfo( ET_ErrInfItm_t arItem, uint8_t arAlmFlg )
{
#if (swKouteiMode == imDisable)
	uint8_t		wkQueueData[ imFlashQue_DatSize ];								/* �L���[�f�[�^ */
	uint32_t			wkTimeInfo;												/* ������� */
	
	/* �������쐬(30bit) */
	
	/* �v���֘A�̋@��ُ�̂Ƃ� */
	if( arItem >= ecErrInfItm_1chOvrRng && arItem <= ecErrInfItm_4chCalErr )
	{
		if( gvModuleSts.mEventKey == ecEventKeyModuleSts_Meas )
		{
			wkTimeInfo = gvMeasPrm.mTimeEvt;									/* �������(�C�x���g���s��) */
		}
		else
		{
			wkTimeInfo = gvMeasPrm.mTime;										/* ������� */
		}
		if( wkTimeInfo == 0U )
		{
			wkTimeInfo = ApiRtcDrv_mktime( gvClock );
		}
	}
	else
	{
		wkTimeInfo = ApiTimeComp_GetLocalTime();
	}
	
	SubFlash_NewestHistTimeSet( wkTimeInfo );									/* �S���𒆂̍ŐV�������X�V */
	wkTimeInfo <<= 2U;
	wkQueueData[ 0U ] = (uint8_t)((wkTimeInfo >> 24U) & 0x000000FFU);			/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkTimeInfo >> 16U) & 0x000000FFU);			/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkTimeInfo >> 8U) & 0x000000FFU);			/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkTimeInfo & 0x000000FCU);					/* 3:11111100 */
	
	/* �x�񍀖ڍ쐬(5bit) */
	wkQueueData[ 3U ] |= ((uint8_t)arItem >> 3U) & 0x03U;						/* 3:00000011 */
	wkQueueData[ 4U ] = ((uint8_t)arItem << 5U) & 0xE0U;						/* 4:11100000 */
	
	/* �A���[���t���O�쐬(1bit) */
	wkQueueData[ 4U ] |= (arAlmFlg << 4U) & 0x10U;								/* 4:00010000 */
	
	/* �̈�� */
	wkQueueData[ 12U ] = ((uint8_t)ecRegionKind_ErrInfo << 1U) & 0x1EU;			/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 12:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* �f�[�^�L���[�Ƀf�[�^��ݒ� */
	
	SubFlash_ReadQueSetPastTime( ecRegionKind_ErrInfo );						/* �ŌÎ������[�h�̃L���[���Z�b�g */
	
#endif
}


/*
 *******************************************************************************
 *	�v���x��̏������ݏ���
 *
 *	[���e]
 *		�v���x��̏������ݏ���
 *	[����]
 *		uint8_t		arAlmFlg: �A���[���t���O
 *		uint8_t		arAlmChannel: �`���l��
 *		uint8_t		arLevel: ���x��
 *		sint16_t	arMeasVal: �v���l
 *		ET_MeasAlmItm_t	arItem: �x�񍀖�
 *******************************************************************************
 */
void ApiFlash_WriteMeasAlm( uint32_t arTime, uint8_t arAlmFlg, uint8_t arAlmChannel, uint8_t arLevel, sint16_t arMeasVal, ET_MeasAlmItm_t arItem )
{
	uint8_t		wkQueueData[ imFlashQue_DatSize ];								/* �L���[�f�[�^ */
	uint32_t			wkTimeInfo;												/* ������� */
	
	/* �������쐬(30bit) */
	if( arTime == 0U )
	{
		arTime = ApiRtcDrv_mktime( gvClock );
	}
	SubFlash_NewestHistTimeSet( arTime );										/* �S���𒆂̍ŐV�������X�V */
	wkTimeInfo = arTime << 2U;
	wkQueueData[ 0U ] = (uint8_t)((wkTimeInfo >> 24U) & 0x000000FFU);			/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkTimeInfo >> 16U) & 0x000000FFU);			/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkTimeInfo >> 8U) & 0x000000FFU);			/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkTimeInfo & 0x000000FCU);					/* 3:11111100 */
	
	/* �A���[���t���O�쐬(1bit) */
	wkQueueData[ 3U ] |= (arAlmFlg << 1U) & 0x02U;								/* 3:00000010 */
	
	/* �`���l���쐬 */
	wkQueueData[ 3U ] |= (arAlmChannel >> 1U) & 0x01U;							/* 3:00000001 */
	wkQueueData[ 4U ] = (arAlmChannel << 7U) & 0x80U;							/* 4:10000000 */
	
	/* ���x���쐬 */
	wkQueueData[ 4U ] |= (arLevel << 5U) & 0x60U;								/* 4:01100000 */
	
	/* �v���l�쐬(13bit) */
	wkQueueData[ 4U ] |= (uint8_t)(arMeasVal >> 8U) & 0x1FU;					/* 4:00011111 */
	wkQueueData[ 5U ] = (uint8_t)(arMeasVal & 0x00FF);							/* 5:11111111 */
	
	/* �x�񍀖ڍ쐬(5bit) */
	wkQueueData[ 6U ] = ((uint8_t)arItem << 3U) & 0xF8U;						/* 6:11111000 */
	
	/* �_��(1bit) */
	wkQueueData[ 6U ] |= (gvInFlash.mParam.mEventKeyFlg & 0x01) << 2U;			/* 6:00000100 */
	
	/* �̈�� */
	wkQueueData[ 12U ] = ((uint8_t)ecRegionKind_MeasAlm << 1U) & 0x1EU;			/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 13:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* �f�[�^�L���[�Ƀf�[�^��ݒ� */
	
	SubFlash_ReadQueSetPastTime( ecRegionKind_MeasAlm );						/* �ŌÎ������[�h�̃L���[���Z�b�g */
}


/*
 *******************************************************************************
 *	�v���l1/�C�x���g�̏������ݏ���
 *
 *	[���e]
 *		�v���l1�A�C�x���g�̏������ݏ���
 *	[����]
 *		uint8_t		arSel: 0�F�v���l1�A1�F�C�x���g
 *	[�߂�l]
 *		uint32_t	wkTime:�����ݎ���
 *******************************************************************************
 */
void ApiFlash_WriteMeasVal1Event( uint8_t arSel )
{
	uint8_t				wkLoop;
	uint8_t				wkQueueData[ imFlashQue_DatSize ];						/* �L���[�f�[�^ */
	
	uint16_t			wkMeasVal;
	uint32_t			wkTimeInfo;
	
	ST_FlashVal1Evt_t	wkWtData;
	ET_RegionKind_t		wkKind;
	
	for( wkLoop=0U ; wkLoop<imFlashQue_ChannelNum ; wkLoop++ )
	{
		wkWtData.mMeasVal[ wkLoop ] = gvMeasPrm.mMeasVal[ wkLoop ];				/* �v���l */
		wkWtData.mDevFlg[ wkLoop ] = gvMeasPrm.mDevFlg[ wkLoop ];				/* ��E�t���O */
		wkWtData.mAlmFlg[ wkLoop ] = gvMeasPrm.mAlmFlg[ wkLoop ];				/* �x��t���O */
	}
	wkWtData.mLogic = gvMeasPrm.mLogic;											/* �_�� */
	wkWtData.m4chDevAlmFlg = gvMeasPrm.mDevFlg[ 3U ];							/* 4ch��E�x��t���O */
	wkWtData.m4chDevAlmFlg |= gvMeasPrm.mAlmFlg[ 3U ] << 4U;
	
	/* �v���l1�͐ÓI�ϐ��Ɋi�[ */
	if( arSel == 0U )
	{
		wkWtData.mTimeInfo = gvMeasPrm.mTime;									/* ������� */
		stFlashWrMeasVal1 = wkWtData;
		wkKind = ecRegionKind_Meas1;
	}
	/* �C�x���g */
	else
	{
		wkWtData.mTimeInfo = gvMeasPrm.mTimeEvt;								/* �������(�C�x���g���s��) */
		SubFlash_NewestHistTimeSet( wkWtData.mTimeInfo );						/* �S���𒆂̍ŐV�������X�V */
		wkKind = ecRegionKind_Event;
	}
	
	
	/* �������(30bit) */
	wkTimeInfo = wkWtData.mTimeInfo << 2U;
	wkQueueData[ 0U ] = (uint8_t)((wkTimeInfo >> 24U) & 0x000000FFU);			/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkTimeInfo >> 16U) & 0x000000FFU);			/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkTimeInfo >> 8U) & 0x000000FFU);			/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkTimeInfo & 0x000000FCU);					/* 3:11111100 */
	
	/* 1ch�v���l1(13bit) */
	wkMeasVal = (wkWtData.mMeasVal[0] >> 11U) & 0x0003U;
	wkQueueData[ 3U ] |= (uint8_t)wkMeasVal;									/* 3:00000011 */
	wkMeasVal = (wkWtData.mMeasVal[0] >> 3U) & 0x00FFU;
	wkQueueData[ 4U ] = (uint8_t)wkMeasVal;										/* 4:11111111 */
	wkMeasVal = (wkWtData.mMeasVal[0] & 0x0007U) << 5U;
	wkQueueData[ 5U ] = (uint8_t)wkMeasVal;										/* 5:11100000 */
	
	/* �_��(1bit) */
	wkQueueData[ 5U ] |= (wkWtData.mLogic << 4U) & 0x10U;						/* 5:00010000 */
	
	/* 1ch��E�t���O(4bit) */
	wkQueueData[ 5U ] |= wkWtData.mDevFlg[0] & 0x0FU;							/* 5:00001111 */
	
	/* 1ch�x��t���O(4bit) */
	wkQueueData[ 6U ] = (wkWtData.mAlmFlg[0] << 4U) & 0xF0U;					/* 6:11110000 */
	
	/* 2ch�v���l1(13bit) */
	wkMeasVal = (wkWtData.mMeasVal[1] >> 9U) & 0x000FU;
	wkQueueData[ 6U ] |= (uint8_t)wkMeasVal;									/* 6:00001111 */
	wkMeasVal = (wkWtData.mMeasVal[1] >> 1U) & 0x00FFU;
	wkQueueData[ 7U ] = (uint8_t)wkMeasVal;										/* 7:11111111 */
	wkMeasVal = (wkWtData.mMeasVal[1] & 0x0001U) << 7U;
	wkQueueData[ 8U ] = (uint8_t)wkMeasVal;										/* 8:10000000 */
	
	/* 4ch��E�t���O(1bit) */
	wkQueueData[ 8U ] |= (wkWtData.m4chDevAlmFlg & 0x01U) << 6U;				/* 8:01000000 */
	
	/* 2ch��E�t���O(4bit) */
	wkQueueData[ 8U ] |= (wkWtData.mDevFlg[1] << 2U) & 0x3CU;					/* 8:00111100 */
	
	/* 2ch�x��t���O(4bit) */
	wkQueueData[ 8U ] |= (wkWtData.mAlmFlg[1] >> 2U) & 0x03U;					/* 8:00000011 */
	wkQueueData[ 9U ] = (wkWtData.mAlmFlg[1] & 0x03U) << 6U;					/* 9:11000000 */
	
	/* 3ch�v���l1(13bit) */
	wkMeasVal = (wkWtData.mMeasVal[2] >> 7U) & 0x003FU;
	wkQueueData[ 9U ] |= (uint8_t)wkMeasVal;									/* 9:00111111 */
	wkMeasVal = (wkWtData.mMeasVal[2] << 1U) & 0x00FEU;
	wkQueueData[ 10U ] = (uint8_t)wkMeasVal;									/* 10:11111110 */
	
	/* 4ch�x��t���O(1bit) */
	wkQueueData[ 10U ] |= wkWtData.m4chDevAlmFlg & 0x01U;						/* 10:00000001 */
	
	/* 3ch��E�t���O(4bit) */
	wkQueueData[ 11U ] = (wkWtData.mDevFlg[2] << 4U) & 0xF0U;					/* 11:11110000 */
	
	/* 3ch�x��t���O(4bit) */
	wkQueueData[ 11U ] |= wkWtData.mAlmFlg[2] & 0x0FU;							/* 11:00001111 */
	
	/* �̈�� */
	wkQueueData[ 12U ] = (wkKind << 1U) & 0x1EU;								/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 13:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* �f�[�^�L���[�Ƀf�[�^��ݒ� */
	
	SubFlash_ReadQueSetPastTime( wkKind );										/* �ŌÎ������[�h�̃L���[���Z�b�g */
}


/*
 *******************************************************************************
 *	���엚���̏������ݏ���
 *
 *	[���e]
 *		���엚���̏������ݏ���
 *******************************************************************************
 */
void ApiFlash_WriteActLog( ET_ActLogItm_t arItem, uint32_t arData, uint16_t arUserId )
{
#if (swKouteiMode == imDisable)
	uint8_t		wkQueueData[ imFlashQue_DatSize ];						/* �L���[�f�[�^ */
	uint32_t			wkTimeInfo;
	uint32_t			wkData;
	
	/* �������(30bit) */
	wkTimeInfo = ApiTimeComp_GetLocalTime();
	
	SubFlash_NewestHistTimeSet( wkTimeInfo );									/* �S���𒆂̍ŐV�������X�V */
	wkTimeInfo <<= 2U;
	wkQueueData[ 0U ] = (uint8_t)((wkTimeInfo >> 24U) & 0x000000FFU);			/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkTimeInfo >> 16U) & 0x000000FFU);			/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkTimeInfo >> 8U) & 0x000000FFU);			/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkTimeInfo & 0x000000FCU);					/* 3:11111100 */
	
	/* ��������(10bit) */
	wkQueueData[ 3U ] |= ((uint8_t)arItem >> 8U) & 0x03U;						/* 3:00000011 */
	wkQueueData[ 4U ] = arItem & 0xFFU;											/* 4:11111111*/
	
	/* �f�[�^(24bit) */
	wkData = arData;
	wkQueueData[ 5U ] = (uint8_t)((wkData >> 16U) & 0x000000FFU);				/* 5:11111111 */
	wkQueueData[ 6U ] = (uint8_t)((wkData >> 8U) & 0x000000FFU);				/* 6:11111111 */
	wkQueueData[ 7U ] = (uint8_t)(wkData & 0x000000FFU);						/* 7:11111111 */
	
	/* �ύX��ID */
	wkQueueData[ 8U ] = arUserId;												/* 8:11111111 */
	
	/* �̈�� */
	wkQueueData[ 12U ] = ((uint8_t)ecRegionKind_ActLog << 1U) & 0x1EU;			/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 12:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* �f�[�^�L���[�Ƀf�[�^��ݒ� */
	
	SubFlash_ReadQueSetPastTime( ecRegionKind_ActLog );							/* �ŌÎ������[�h�̃L���[���Z�b�g */
#endif
}


/*
 *******************************************************************************
 *	�V�X�e�����O�̏������ݏ���
 *
 *	[���e]
 *		�V�X�e�����O�̏������ݏ���
 *******************************************************************************
 */
void ApiFlash_WriteSysLog( ET_SysLogItm_t arItem, uint32_t arData )
{
#if (swKouteiMode == imDisable)
	uint8_t		wkQueueData[ imFlashQue_DatSize ];								/* �L���[�f�[�^ */
	uint32_t			wkTimeInfo;
	uint32_t			wkData;
	
	/* �������(30bit) */
	wkTimeInfo = ApiTimeComp_GetLocalTime();
	
	SubFlash_NewestHistTimeSet( wkTimeInfo );									/* �S���𒆂̍ŐV�������X�V */
	wkTimeInfo <<= 2U;
	wkQueueData[ 0U ] = (uint8_t)((wkTimeInfo >> 24U) & 0x000000FFU);			/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkTimeInfo >> 16U) & 0x000000FFU);			/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkTimeInfo >> 8U) & 0x000000FFU);			/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkTimeInfo & 0x000000FCU);					/* 3:11111100 */
	
	/* �V�X�e�����O����(4bit) */
	wkQueueData[ 3U ] |= ((uint8_t)arItem >> 2U) & 0x03U;						/* 3:00000011 */
	wkQueueData[ 4U ] = ((uint8_t)arItem << 6U) & 0xC0U;						/* 4:11000000 */
	
	/* �f�[�^(25bit����邪�������M���ŉ��ʂ���24bit�Ȃ̂ōŏ��bit�͖��������) */
	wkData = (arData << 5U) & 0x3FFFFFFFU;
	wkQueueData[ 4U ] |= (uint8_t)((wkData >> 24U) & 0x000000FFU);				/* 4:00111111 */
	wkQueueData[ 5U ] = (uint8_t)((wkData >> 16U) & 0x000000FFU);				/* 5:11111111 */
	wkQueueData[ 6U ] = (uint8_t)((wkData >> 8U) & 0x000000FFU);				/* 6:11111111 */
	wkQueueData[ 7U ] = (uint8_t)(wkData & 0x000000E0U);						/* 7:11100000 */
	
	/* �̈�� */
	wkQueueData[ 12U ] = ((uint8_t)ecRegionKind_SysLog << 1U) & 0x1EU;			/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 12:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* �f�[�^�L���[�Ƀf�[�^��ݒ� */
	
	SubFlash_ReadQueSetPastTime( ecRegionKind_SysLog );							/* �ŌÎ������[�h�̃L���[���Z�b�g */
#endif
}

/*
 *******************************************************************************
 *	�S���𒆂̍ŐV�������X�V
 *
 *	[���e]
 *		�S���𒆂̍ŐV�������X�V
 *	[����]
 *		uint32_t arTime: ���O�̎���
 *******************************************************************************
 */
static void SubFlash_NewestHistTimeSet( uint32_t arTime )
{
	if( (gvInFlash.mData.mNewestHistTime == 0xFFFFFFFF) ||
		(arTime > gvInFlash.mData.mNewestHistTime) )
	{
		gvInFlash.mData.mNewestHistTime = arTime;
	}
}

/*
 *******************************************************************************
 *	�v���l1�A�V�X�e�����O�̃A�h���X�e�[�u���̏������ݏ���
 *
 *	[���e]
 *		�v���l1�A�V�X�e�����O�̃A�h���X�e�[�u���̂̏������ݏ���
 *******************************************************************************
 */
void ApiFlash_WriteFlasAdrTbl( uint32_t arEndadr, uint16_t arNum, uint16_t arIndex, uint8_t arKind )
{
	uint8_t			wkQueueData[ imFlashQue_DatSize ];							/* �L���[�f�[�^ */
	uint32_t		wkEndAdr;
	
	/* �I�[�A�h���X(32bit) */
	wkEndAdr = arEndadr;
	wkQueueData[ 0U ] = (uint8_t)((wkEndAdr >> 24U) & 0x000000FFU);				/* 0:11111111 */
	wkQueueData[ 1U ] = (uint8_t)((wkEndAdr >> 16U) & 0x000000FFU);				/* 1:11111111 */
	wkQueueData[ 2U ] = (uint8_t)((wkEndAdr >> 8U) & 0x000000FFU);				/* 2:11111111 */
	wkQueueData[ 3U ] = (uint8_t)(wkEndAdr & 0x000000FFU);						/* 3:11111111 */
	
	/* �f�[�^��(16bit) */
	wkQueueData[ 4U ] = (uint8_t)((arNum >> 8U) & 0x00FFU);						/* 4:11111111 */
	wkQueueData[ 5U ] = (uint8_t)(arNum & 0x00FFU);								/* 5:11111111 */
	
	/* IndexNo.(16bit) */
	wkQueueData[ 6U ] = (uint8_t)((arIndex >> 8U) & 0x00FFU);					/* 6:11111111 */
	wkQueueData[ 7U ] = (uint8_t)(arIndex & 0x00FFU);							/* 7:11111111 */
	
	/* �̈�� */
	wkQueueData[ 12U ] = ((uint8_t)arKind << 1U) & 0x1EU;						/* 12:00011110 */
	
	/* R/W */
	wkQueueData[ 12U ] |= 0x01U;												/* 12:00000001 */
	
	SubFlash_EnQueue( wkQueueData );											/* �f�[�^�L���[�Ƀf�[�^��ݒ� */
}


/*
 *******************************************************************************
 *	Flash���[�h�̃L���[�Z�b�g
 *
 *	[����]
 *		uint32_t		arStartTime		:���[�h�J�n����
 *		uint32_t		arEndTime		:���[�h�I������
 *		uint16_t		arIndexNum		:���[�h�J�nIndexNo.
 *		uint8_t			arKosu			:���[�h��
 *		ET_RegionKind_t	arRegionKind	:���[�h���
 *		uint8_t			arReadStas		:0:�����ʐM�A1:�������ʐM�A2:�ŌÎ���
 *	[���e]
 *		Flash���烊�[�h����L���[���Z�b�g����
 *******************************************************************************
 */
ET_Error_t ApiFlash_ReadQueSet( uint32_t arStartTime, uint32_t arEndTime, uint16_t arIndexNum, uint8_t arKosu, ET_RegionKind_t arRegionKind, uint8_t arReadStas )
{
	uint8_t			wkQueueData[ imFlashQue_DatSize ];	/* �L���[�f�[�^ */
	uint32_t		wkStartTime;						/* �J�n���� */
	uint32_t		wkEndTime;							/* �I������ */
	ET_Error_t		wkRet = ecERR_OK;					/* �߂�l(�G���[���) */
	
	if( stFlashQue.mNum < imFlashQue_DatNum )
	{
		
		/* �J�n�����쐬(30bit) */
		wkStartTime = arStartTime << 2U;											/* �ŏ�ʃr�b�g�܂ŃV�t�g��A�������i�[ */
		wkQueueData[ 0U ] = (uint8_t)((wkStartTime & 0xFF000000U) >> 24U);			/* 0xFF000000��11111111000000000000000000000000 */
		wkQueueData[ 1U ] = (uint8_t)((wkStartTime & 0x00FF0000U) >> 16U);			/* 0x00FF0000��00000000111111110000000000000000 */
		wkQueueData[ 2U ] = (uint8_t)((wkStartTime & 0x0000FF00U) >> 8U);			/* 0x0000FF00��00000000000000001111111100000000 */
		wkQueueData[ 3U ] = (uint8_t)(wkStartTime & 0x000000FCU);					/* 0x000000FC��00000000000000000000000011111100 */
		
		/* ���[�h�X�e�[�^�X */
		wkQueueData[ 3U ] |= (uint8_t)(arReadStas & 0x03U);							/* 0x03��00000011 */
		
		/* �I�������쐬(30bit) */
		wkEndTime = arEndTime << 2U;												/* �ŏ�ʃr�b�g�܂ŃV�t�g��A�������i�[ */
		wkQueueData[ 4U ] = (uint8_t)((wkEndTime & 0xFF000000U) >> 24U);			/* 0xFF000000��11111111000000000000000000000000 */
		wkQueueData[ 5U ] = (uint8_t)((wkEndTime & 0x00FF0000U) >> 16U);			/* 0x00FF0000��00000000111111110000000000000000 */
		wkQueueData[ 6U ] = (uint8_t)((wkEndTime & 0x0000FF00U) >> 8U);				/* 0x0000FF00��00000000000000001111111100000000 */
		wkQueueData[ 7U ] = (uint8_t)(wkEndTime & 0x000000FCU);						/* 0x000000FC��00000000000000000000000011111100 */
		
		/* �C���f�b�N�XNo.(16bit) */
		wkQueueData[ 8U ] = (uint8_t)((arIndexNum & 0xFF00U) >> 8U);				/* 0xFF00��1111111100000000 */
		wkQueueData[ 9U ] = (uint8_t)(arIndexNum & 0x00FFU);						/* 0x00FF��0000000011111111 */
		
		/* ��(8bit) */
		wkQueueData[ 10U ] = arKosu;												/* 0xFF��11111111 */
		
		/* �̈��쐬(4bit) */
		wkQueueData[ 12U ] = ((uint8_t)arRegionKind << 1U) & 0x1EU;					/* 0x1E��00011110 */
		
		/* Read���쐬(1bit) */
		wkQueueData[ 12U ] &= 0xFEU;												/* Read���i�[ */
		
		SubFlash_EnQueue( wkQueueData );											/* �f�[�^�L���[�Ƀf�[�^��ݒ� */
	}
	else
	{
		wkRet = ecERR_NG;
	}
	return wkRet;
}



/*
 *******************************************************************************
 *	�G���L���[����
 *
 *	[���e]
 *		�f�[�^�L���[�Ɏw�肵���f�[�^��ݒ肷��B
 *		�Ȃ��A�i�[�����ő�l�ȏ�̎��́A�������Ƃ���B
 *	[���L]
 *		�w�肷��f�[�^�̊i�[�̈��imFlashQue_DatSize�ȏ�Ƃ��邱�ƁB
 *******************************************************************************
 */
static void SubFlash_EnQueue( uint8_t *parData )
{
	uint8_t		wkIndex;														/* �C���f�b�N�X */
	uint8_t		wkLoop;															/* ���[�v�J�E���^ */
	uint8_t		*pwkData;														/* �ݒ�f�[�^�ʒu */
	
	if( parData != M_NULL )														/* �A�h���X������ */
	{
		if( stFlashQue.mNum < imFlashQue_DatNum )							/* �i�[�����ő�l���� */
		{
			wkIndex = stFlashQue.mHead;
			wkIndex += stFlashQue.mNum;
			wkIndex %= imFlashQue_DatNum;
			
			/* �f�[�^�i�[ */
			pwkData = parData;
			for( wkLoop = 0U ; wkLoop < imFlashQue_DatSize ; wkLoop++ )
			{
				stFlashQue.mData[wkIndex][wkLoop] = *pwkData;
				pwkData++;
			}
			
			stFlashQue.mNum++;												/* �i�[�� + 1 */
		}
		else
		{
			/*
			 ***************************************
			 *	�i�[�����ő�l�𒴂����ꍇ
			 *	�o�b�t�@�ւ̊i�[�����Ȃ��B
			 ***************************************
			 */
			M_NOP;															/* ������ */
		}
	}
}


/*
 *******************************************************************************
 *	�f�L���[����
 *
 *	[���e]
 *		�f�[�^�L���[����f�[�^���擾����B
 *		�擾���@�́AFIFO�����Ŏ擾����B
 *		�f�[�^�L���[����̏ꍇ�A�������Ƃ���B
 *	[���L]
 *		�w�肷��f�[�^�̊i�[�̈��imFlashQue_DatSize�ȏ�Ƃ��邱�ƁB
 *******************************************************************************
 */
static void SubFlash_DeQueue( uint8_t *parData )
{
	uint8_t		wkIndex;														/* �C���f�b�N�X */
	
	if( parData != M_NULL )													/* �A�h���X���� */
	{
		if( stFlashQue.mNum > 0U )											/* �ʏ펞�p�L���[�̃`�F�b�N */
		{																	/* �f�[�^�L�� */
			wkIndex = stFlashQue.mHead + 1U;
			wkIndex %= imFlashQue_DatNum;
			stFlashQue.mHead = wkIndex;										/* �i�[�ʒu�̐擪���X�V */
			stFlashQue.mNum--;												/* �i�[�� - 1 */
			
		}
		else
		{
			M_NOP;															/* NG */
		}
	}
}


/*
 *******************************************************************************
 *	�L���[�擾����
 *
 *	[���e]
 *		�f�[�^�L���[�̊i�[��Ԃ��擾����B
 *******************************************************************************
 */
static uint8_t SubFlash_GetQueue( uint8_t *parData, uint8_t *parNum )
{
	uint8_t		wkRet;															/* �߂�l(�G���[���) */
	uint8_t		wkLoop;															/* ���[�v�J�E���^ */
	
	if( (parData != M_NULL) && (parNum != M_NULL) )
	{
		/* �f�[�^�L�� */
		if( stFlashQue.mNum > 0U )											/* �ʏ�̃L���[�̃`�F�b�N */
		{																	/* �f�[�^�L�� */
			for( wkLoop = 0U ; wkLoop < imFlashQue_DatSize ; wkLoop++ )
			{
				(*parData) = stFlashQue.mData[ stFlashQue.mHead ][ wkLoop ];
				parData++;
			}
			(*parNum) = stFlashQue.mNum;
			wkRet = imARI;													/* �߂�l�F�f�[�^�L�� */
		}
		else
		{																	/* �ǂ���̃L���[���󂾂����ꍇ */
			(*parData) = (uint8_t)M_NULL;
			(*parNum) = stFlashQue.mNum;
			wkRet = imNASI;													/* �߂�l�F�f�[�^���� */
		}
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�L���[�f�[�^�̈��擾
 *
 *	[���e]
 *		�L���[�f�[�^���̗̈����擾����
 *******************************************************************************
 */
static ET_RegionKind_t SubFlash_QueDataRegionKindGet( const uint8_t arQueData )
{
	ET_RegionKind_t	wkRet;
	uint8_t			wkData;
	
	wkData = (arQueData >> 1U) & 0x0FU;
	if( wkData < (uint8_t)ecRegionKindMax )
	{
		wkRet = (ET_RegionKind_t)wkData;
	}
	else
	{
		wkRet = ecRegionKindMax;
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�̈��f�[�^�擾
 *
 *	[���e]
 *		�̈��f�[�^�̎擾���s���B
 *******************************************************************************
 */
static void SubFlash_GetRegionData( ST_RegionData_t *parRegionData, ET_RegionKind_t arRegionKind )
{
	if( parRegionData != M_NULL )
	{
		parRegionData->mRegionKind = arRegionKind;								/* �̈�� */
		
		switch( arRegionKind )
		{
			/* �ُ��� */
			case ecRegionKind_ErrInfo:
				parRegionData->mWrNum = gvInFlash.mData.mErrInf_Num;					/* �������݌��� */
				parRegionData->mDataByteNum = imDataByteErrInf;							/* �f�[�^�o�C�g�� */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mErrInf_AddrEnd;			/* �������ݏI�[�A�h���X */
				parRegionData->mWrAddrFin = gvInFlash.mData.mErrInf_AddrEnd + imDataByteErrInf;	/* �������ݍŏI�A�h���X */
				parRegionData->mIndexNo = gvInFlash.mData.mErrInf_Index;				/* IndexNo. */
				break;
				
			/* �v���x�� */
			case ecRegionKind_MeasAlm:
				parRegionData->mWrNum = gvInFlash.mData.mMeasAlm_Num;					/* �������݌��� */
				parRegionData->mDataByteNum = imDataByteMeasAlm;						/* �f�[�^�o�C�g�� */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mMeasAlm_AddrEnd;			/* �������ݏI�[�A�h���X */
				parRegionData->mWrAddrFin = gvInFlash.mData.mMeasAlm_AddrEnd + imDataByteMeasAlm;	/* �������ݍŏI�A�h���X */
				parRegionData->mIndexNo = gvInFlash.mData.mMeasAlm_Index;				/* IndexNo. */
				break;
				
			/* �v���l1 */
			case ecRegionKind_Meas1:
				parRegionData->mWrNum = gvInFlash.mData.mMeas1_Num;						/* �������݌��� */
				parRegionData->mDataByteNum = imDataByteMeas1;							/* �f�[�^�o�C�g�� */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mMeas1_AddrEnd;				/* �������ݏI�[�A�h���X */
				parRegionData->mWrAddrFin = gvInFlash.mData.mMeas1_AddrEnd + imDataByteMeas1;		/* �������ݍŏI�A�h���X */
				parRegionData->mIndexNo = gvInFlash.mData.mMeas1_Index;					/* IndexNo. */
				break;
				
			/* �C�x���g */
			case ecRegionKind_Event:
				parRegionData->mWrNum = gvInFlash.mData.mEvt_Num;						/* �������݌��� */
				parRegionData->mDataByteNum = imDataByteEvt;							/* �f�[�^�o�C�g�� */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mEvt_AddrEnd;				/* �������ݏI�[�A�h���X */
				parRegionData->mWrAddrFin = gvInFlash.mData.mEvt_AddrEnd + imDataByteEvt;			/* �������ݍŏI�A�h���X */
				parRegionData->mIndexNo = gvInFlash.mData.mEvt_Index;					/* IndexNo. */
				break;
				
			/* ���엚�� */
			case ecRegionKind_ActLog:
				parRegionData->mWrNum = gvInFlash.mData.mActLog_Num;					/* �������݌��� */
				parRegionData->mDataByteNum = imDataByteActLog;							/* �f�[�^�o�C�g�� */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mActLog_AddrEnd;			/* �������ݏI�[�A�h���X */
				parRegionData->mWrAddrFin = gvInFlash.mData.mActLog_AddrEnd + imDataByteActLog;	/* �������ݍŏI�A�h���X */
				parRegionData->mIndexNo = gvInFlash.mData.mActLog_Index;				/* IndexNo. */
				break;
				
			/* �V�X�e�����O */
			case ecRegionKind_SysLog:
				parRegionData->mWrNum = gvInFlash.mData.mSysLog_Num;					/* �������݌��� */
				parRegionData->mDataByteNum = imDataByteSysLog;							/* �f�[�^�o�C�g�� */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mSysLog_AddrEnd;			/* �������ݏI�[�A�h���X */
				parRegionData->mWrAddrFin = gvInFlash.mData.mSysLog_AddrEnd + imDataByteSysLog;	/* �������ݍŏI�A�h���X */
				parRegionData->mIndexNo = gvInFlash.mData.mSysLog_Index;				/* IndexNo. */
				break;
			
			/* �v���l1�A�h���X */
			case ecRegionKind_Meas1Adr:
				parRegionData->mWrNum = gvInFlash.mData.mMeasVal1Adr_Num;				/* �������݌��� */
				parRegionData->mDataByteNum = imDataByteAdrTbl;							/* �f�[�^�o�C�g�� */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mMeasVal1Adr_AddrEnd;		/* �������ݏI�[�A�h���X */
				parRegionData->mWrAddrFin = gvInFlash.mData.mMeasVal1Adr_AddrEnd + imDataByteAdrTbl;	/* �������ݍŏI�A�h���X */
				parRegionData->mIndexNo = 0U;											/* IndexNo. */
				break;
				
			/* �V�X�e�����O�A�h���X */
			case ecRegionKind_SysLogAdr:
				parRegionData->mWrNum = gvInFlash.mData.mSysLogAdr_Num;					/* �������݌��� */
				parRegionData->mDataByteNum = imDataByteAdrTbl;							/* �f�[�^�o�C�g�� */
				parRegionData->mWrAddrEnd = gvInFlash.mData.mSysLogAdr_AddrEnd;			/* �������ݏI�[�A�h���X */
				parRegionData->mWrAddrFin = gvInFlash.mData.mSysLogAdr_AddrEnd + imDataByteAdrTbl;	/* �������ݍŏI�A�h���X */
				parRegionData->mIndexNo = 0U;											/* IndexNo. */
				break;
				
			default:
				break;
		}
	}
}


/*
 *******************************************************************************
 *	���[�h�L���[�̉�͏���
 *
 *	[����]
 *		uint8_t			*parQueData		:�L���[�f�[�^
 *		ET_RegionKind_t	arRegionKind	:���[�h���
 *	[���e]
 *		���[�h�L���[��IndexNo.����f�[�^�����[�h����
 *******************************************************************************
 */
static ET_Error_t SubFlash_ReadData( uint8_t *parQueData, ET_RegionKind_t arRegionKind )
{
	uint8_t			wkU16;
	uint8_t			wkLoop;
	uint32_t		wkU32;
	
	uint32_t		wkQueStartTime;			/* �v���J�n���� */
	uint16_t		wkQueIndex;				/* ����IndexNo. */
	uint8_t			wkQueRdStatus;			/* �v���ʐM���[�h 0:�����ʐM 1:�������ʐM */
	uint8_t			wkKosu;					/* ���[�h�� */
	uint32_t		wkRetAdr;				/* ����IndexNo.�̃A�h���X */
	
	ST_Region_t		wkRegionInfo;			/* �̈�했�̏�� */
	
	uint32_t		wkRngIndexMin;			/* �����\�͈�Min�l */
	uint32_t		wkRngIndexMax;			/* �����\�͈�Max�l */
	
	uint32_t		wkNowSctr;				/* ����IndexNo.���΃Z�N�^ */
	uint32_t		wkSrchSctr;				/* ����IndexNo.���΃Z�N�^ */
	
	uint32_t		wkSize1;				/* Index�ԃT�C�Y�v�Z�p */
	sint32_t		wkSize2;				/* Index�ԃT�C�Y�v�Z�p */
	
	uint32_t		wkIndexSctrNum;			/* Index�ԃZ�N�^���v�Z�p */
	
	sint32_t		wkStartAreaAdr;			/* �������擪�A�h���X */
	
	sint32_t		wkCal;					/* ����IndexNo.�v�Z�p */
	
	uint8_t			wkRdData[ imFlashRdWr_DatSize ];		/* Read�f�[�^ */
	
	ET_Error_t		wkError = ecERR_NG;
	
	for( wkLoop = 0U ; wkLoop < M_ArrayElement( cRegionKindInfo ) ; wkLoop++ )
	{
		if( arRegionKind == cRegionKindInfo[ wkLoop ].mRegionKind )
		{
			wkRegionInfo = cRegionKindInfo[ wkLoop ];
			break;
		}
	}
	
	/* �L���[�̊J�n�����AIndexNo.�A���A�������[�h�����[�h */
	wkQueStartTime = (uint32_t)parQueData[ 0U ] << 24U;
	wkQueStartTime |= (uint32_t)parQueData[ 1U ] << 16U;
	wkQueStartTime |= (uint32_t)parQueData[ 2U ] << 8U;
	wkQueStartTime |= (uint32_t)parQueData[ 3U ];
	wkQueStartTime >>= 2U;
	
	wkQueIndex = (uint16_t)parQueData[ 8U ] << 8U;
	wkQueIndex |= parQueData[ 9U ];
	
	wkKosu = parQueData[ 10U ];
	
	wkQueRdStatus = parQueData[ 3U ] & 0x03U;
	
	/* �f�[�^���Ȃ��Ƃ� */
	if( stRegionData.mIndexNo == 0U || stRegionData.mWrNum == 0U )
	{
		wkKosu = 1U;
		wkQueIndex = 1U;
	}
	else
	{
		/* �͈͊O��IndexNo. */
		if( wkQueIndex > wkRegionInfo.mIndexMax )
		{
			wkQueIndex = 1;
		}
		
		/* ���[�h������ */
		if( stRegionData.mIndexNo >= wkQueIndex )
		{
			wkU32 = wkQueIndex + wkKosu - 1U;
			if( wkU32 > stRegionData.mIndexNo )
			{
				wkKosu = stRegionData.mIndexNo + 1U - wkQueIndex;
			}
		}
		else
		{
			wkU32 = wkRegionInfo.mIndexMax + 1U - wkQueIndex;
			wkU32 += stRegionData.mIndexNo;
			
			if( wkKosu > wkU32 )
			{
				wkKosu = wkU32;
			}
		}
		
		/*** �����\�͈͂̊m�F ***/
		/* ���݂̎��^�f�[�^�����̈��̎��^�ő�l���� */
		if( stRegionData.mWrNum < wkRegionInfo.mDataNumMax )
		{
			/* 1 �` ���݂�IndexNo. */
			wkRngIndexMin = 1U;
			wkRngIndexMax = stRegionData.mIndexNo;
			
			if( wkQueIndex < wkRngIndexMin )
			{
				wkQueIndex = 1U;
				wkKosu = 1U;
			}
			else if( wkQueIndex > wkRngIndexMax )
			{
				wkQueIndex = wkRngIndexMax;
				wkKosu = 1U;
			}
		}
		/* ���^�f�[�^�����̈��̎��^�ő�l */
		else
		{
			/* ����IndexNo. >= ����IndexNo. */
			if( stRegionData.mIndexNo >= wkQueIndex )
			{
				if( stRegionData.mIndexNo <= wkRegionInfo.mDataNumMax )
				{
					wkRngIndexMin = 1U;
				}
				else
				{
					/* ����IndexNo. + 1 - �f�[�^MAX�l �` ����IndexNo. */
					wkRngIndexMin = (uint32_t)stRegionData.mIndexNo + 1U - (uint32_t)wkRegionInfo.mDataNumMax;
				}
				
				wkRngIndexMax = (uint32_t)stRegionData.mIndexNo;
				
				if( wkQueIndex < wkRngIndexMin )
				{
					wkQueIndex = wkRngIndexMin;
				}
				else if( wkQueIndex > wkRngIndexMax )
				{
					wkQueIndex = wkRngIndexMax + 1U - wkKosu;
				}
			}
			else
			{
				/* 1 �` ����IndexNo. */
				wkRngIndexMin = 1U;
				wkRngIndexMax = stRegionData.mIndexNo;
				if( (wkQueIndex < wkRngIndexMin) || (wkQueIndex > wkRngIndexMax) )
				{
					/* ((IndexNo.Max�l + 1) - �f�[�^Max�l) + ����IndexNo.  �` IndexNo.Max�l */
					wkRngIndexMin = (uint32_t)wkRegionInfo.mIndexMax + 1U - (uint32_t)wkRegionInfo.mDataNumMax;
					wkRngIndexMin += (uint32_t)stRegionData.mIndexNo;
					wkRngIndexMax = (uint32_t)wkRegionInfo.mIndexMax;
					
					if( (uint32_t)wkQueIndex < wkRngIndexMin )
					{
						wkQueIndex = wkRngIndexMin;
					}
					else if( (uint32_t)wkQueIndex > wkRngIndexMax )
					{
						wkQueIndex = wkRngIndexMax + 1U - wkKosu;
					}
				}
			}
		}
	}
	
	
	/* ���݂�IndexNo.���΃Z�N�^�v�Z: (����IndexNo. - 1) / �f�[�^MAX�l */
	wkNowSctr = stRegionData.mIndexNo - 1U;
	wkNowSctr /= wkRegionInfo.mDataNumInSctr;
	
	for( wkLoop = 0U ; wkLoop < wkKosu ; wkLoop++, wkQueIndex++ )
	{
		/* �L���[��IndexNo.���̈��̍ő�l���� */
		if( wkQueIndex > wkRegionInfo.mIndexMax )
		{
			wkQueIndex = 1U;
		}
		
		/* ������IndexNo.���΃Z�N�^�v�Z: (����IndexNo. - 1) / �f�[�^MAX�l */
		wkSrchSctr = wkQueIndex - 1U;
		wkSrchSctr /= wkRegionInfo.mDataNumInSctr;
		
		
		/*** ����IndexNo.�ƌ���IndexNo.�Ԃ̃T�C�Y���Z ***/
		
		/* ����IndexNo. >= ����IndexNo. */
		if( stRegionData.mIndexNo >= wkQueIndex )
		{
			/* (����IndexNo. + 1 - ����IndexNo.) * size */
			wkSize1 = (stRegionData.mIndexNo + 1U) - wkQueIndex;
		}
		else
		{
			/* (IndexNo.�ő吔 + 1 - ����IndexNo. + ����IndexNo.) * size */
			wkSize1 = (wkRegionInfo.mIndexMax + 1U) - wkQueIndex;
			wkSize1 += stRegionData.mIndexNo;
		}
		wkSize1 *= wkRegionInfo.mDataByte;
		
		
		/*** �A�h���X���Z ***/
		switch( arRegionKind )
		{
			case ecRegionKind_ErrInfo:
			case ecRegionKind_MeasAlm:
			case ecRegionKind_Event:
			case ecRegionKind_ActLog:
				
				/*** ����IndexNo.�̃A�h���X���Z ***/
				if( stRegionData.mIndexNo == 0U || stRegionData.mWrNum == 0U )
				{
					wkRetAdr = stRegionData.mWrAddrEnd;
				}
				else
				{
				/* ����IndexNo.�͓����Z�N�^�� */
				if( wkNowSctr == wkSrchSctr )
				{
					/* �����A�h���X = ���݂̏I�[�A�h���X + 1 - �T�C�Y */
					wkRetAdr = (stRegionData.mWrAddrEnd + 1U) - wkSize1;
				}
				else
				{
					/* ���݂̏I�[�A�h���X�ƌ��݂̐擪�A�h���X�Ԃ̃T�C�Y */
						wkSize2 = stRegionData.mWrAddrEnd / imFlashSctrSize;
						wkSize2 *= -1;
						wkSize2 *= imFlashSctrSize;
						wkSize2 += stRegionData.mWrAddrEnd + 1;
					
					/* ���݃Z�N�^����1�G���A */
					if( stRegionData.mWrAddrEnd < wkRegionInfo.m2ndStAdr )
					{
						/* ��2�Z�N�^�̍ŏI�A�h���X */
						wkRetAdr = (wkRegionInfo.mSctrMax * imFlashSctrSize) + imFlashSctrSize;
					}
					else
					{
						/* �O�Z�N�^�̍ŏI�A�h���X */
						wkRetAdr = (wkRegionInfo.mSctrMin * imFlashSctrSize) + imFlashSctrSize;
					}
					/* �O�Z�N�^�̍ŏI�A�h���X + ���݃Z�N�^�Ŏg�p���Ă���T�C�Y - Index�Ԃ̃T�C�Y - ���g�p�T�C�Y */
					wkRetAdr = wkRetAdr + wkSize2 - wkSize1 - wkRegionInfo.mRsvSize;
				}
				}
				break;
				
			case ecRegionKind_Meas1:
			case ecRegionKind_SysLog:
				
				if( stRegionData.mIndexNo == 0U || stRegionData.mWrNum == 0U )
				{
					wkRetAdr = stRegionData.mWrAddrEnd;
				}
				else
				{
				/*** ����Index�Z�N�^�ƌ���Index�Z�N�^�Ԃ̐� */
				
				/* �����Z�N�^ */
				if( wkNowSctr == wkSrchSctr )
				{
					/* ����IndexNo. < ����IndexNo. */
					if( stRegionData.mIndexNo < wkQueIndex )
					{
						/* 1�����Ă���̂ŁA���݂̃Z�N�^�������1�������A���݂̃Z�N�^�̎��̃Z�N�^�ɂ��� */
						wkIndexSctrNum = wkRegionInfo.mSctrNum - 1U;
					}
					else
					{
						wkIndexSctrNum = 0U;
					}
				}
				/* ���݃Z�N�^ > �����Z�N�^ */
				else if( wkNowSctr > wkSrchSctr )
				{
					/* ���݃Z�N�^ - �����Z�N�^ */
					wkIndexSctrNum = wkNowSctr - wkSrchSctr;
				}
				/* ���݃Z�N�^ < �����Z�N�^ */
				else
				{
					/* �Z�N�^�ő吔  - (�����Z�N�^ + 1U - ���݃Z�N�^) */
					wkIndexSctrNum = wkRegionInfo.mSctrNum - ( wkSrchSctr + 1U - wkNowSctr );
				}
				/* �g�p�Z�N�^�� * ���g�p�T�C�Y */
				wkIndexSctrNum *= wkRegionInfo.mRsvSize;
				
				/*** ����IndexNo.�̃A�h���X���Z ***/
				
				/* �I�[�A�h���X + 1 - �Z�N�^�ԃT�C�Y - ���g�p�̈�T�C�Y */
				wkCal = stRegionData.mWrAddrEnd + 1U - wkSize1;
				wkCal -= wkIndexSctrNum;
				
				wkStartAreaAdr = wkRegionInfo.mSctrMin * imFlashSctrSize;
				/* �g�p�Z�N�^�J�n�A�h���X�ȏ� */
				if( wkCal >= wkStartAreaAdr )
				{
					wkRetAdr = (uint32_t)wkCal;
				}
				/* �g�p�Z�N�^�J�n�A�h���X�����A0�ȏ� */
				else if( wkCal >= 0U )
				{
					wkRetAdr = (wkRegionInfo.mSctrMax + 1U) * imFlashSctrSize;
					wkRetAdr -= (wkStartAreaAdr - wkCal);
				}
				/* 0���� */
				else
				{
					wkCal *= -1;
					wkRetAdr = (wkRegionInfo.mSctrMax + 1U) * imFlashSctrSize;
					wkRetAdr -= (wkStartAreaAdr + wkCal);
				}
				}
				break;
				
			default:
				return wkError;
		}
		
		/*** �f�[�^���[�h ***/
		wkError = SubFlash_Read( wkRetAdr, &wkRdData[ 0U ], wkRegionInfo.mDataByte );
		if( wkError == ecERR_OK )
		{
			/* �����ʐM�A���J�n�������w�� */
			if( wkQueRdStatus == 0U && wkQueStartTime != 0U  )
			{
				/* ���[�h���������Ɣ�r���s��v�A���̓��[�h����1�ȉ� */
				if( wkQueStartTime != SubFlash_MakeTime4Byte( wkRdData ) || wkKosu <= 1U )
				{
					/* �S�f�[�^���[�h�̃L���[�쐬 */
					ApiRfDrv_MakeReadQue( arRegionKind, 0U, 0U );
					break;
				}
				else
				{
					wkQueIndex ++;
					if( wkQueIndex > wkRegionInfo.mIndexMax )
					{
						wkQueIndex = 1U;
					}
					ApiRfDrv_MakeReadQue( arRegionKind, 0U, wkQueIndex );
					break;
				}
			}
			/* �ŌÎ��� */
			else if( wkQueRdStatus == 2U )
			{
				wkU16 = SubFlash_SetReadData( wkRdData, arRegionKind, wkLoop, wkKosu, wkQueIndex, wkQueRdStatus );
			}
			/* �������ʐM�ƍ����ʐM�̎����w��Ȃ� */
			else
			{
				wkU16 = SubFlash_SetReadData( wkRdData, arRegionKind, wkLoop, wkKosu, wkQueIndex, wkQueRdStatus );
			if( wkU16 != 0U )
			{
				M_SETBIT( stFlashReqRd.mFin, wkU16 );							/* Flash���[�h���� */
			}
		}
		}
		else
		{
			vReadFlash = ecReadFlash_ReadErr;									/* �ǂݏo���G���[ */
			break;
		}
	}
	
	return wkError;
}


/*
 *******************************************************************************
 *	�ǂݏo���f�[�^�i�[
 *
 *	[����]
 *		uint8_t			arReadData[]	:���[�h�f�[�^
 *		ET_RegionKind_t	arRegionKind	:���[�h���
 *		uint8_t			arLoop			:���[�h���ԍ�
 *		uint8_t			arKosu			:���[�h��
 *		uint16_t		arIndex			:���[�h�擪IndexNo.
 *		uint8_t			arRdStatus		:0:�����ʐM�A1:�������ʐM�A2:�ŌÎ����̂��߂̃f�[�^���[�h
 *	[�߂�l]
 *		uint8_t			wkRet			:Flash���[�h����������ނ������t���O
 *	[���e]
 *		�v���C�}������ǂݏo�����f�[�^���i�[����B
 *******************************************************************************
 */
static uint16_t SubFlash_SetReadData( uint8_t arReadData[], ET_RegionKind_t arRegionKind, uint8_t arLoop, uint8_t arKosu, uint16_t arIndex, uint8_t arRdStatus )
{
	uint16_t	wkRet = 0;					/* �߂�l */
	uint32_t	wkTimeInfo;					/* ������� */
	ST_FlashVal1Evt_t	wkRdMeas;			/* �v���l */
	ST_FlashMeasAlm_t	wkRdMeasAlm;		/* �v���x�� */
	ST_FlashErrInfo_t	wkRdErrInfo;		/* �@��ُ�x�� */
	ST_FlashActLog_t	wkRdActLog;			/* ���엚�� */
	ST_FlashSysLog_t	wkRdSysLog;			/* �V�X�e�����O */
	
	/* �������̍쐬 */
	wkTimeInfo = SubFlash_MakeTime4Byte(arReadData);
	
	if( arRdStatus == 2U && wkTimeInfo == 0x3FFFFFFFU )
	{
		arIndex = 0U;
		wkTimeInfo = 0U;
	}
	
	switch( arRegionKind )
	{
		/* �ُ��� */
		case ecRegionKind_ErrInfo:
			
			/* �ŌÏ��Z�b�g */
			if( arRdStatus == 2U )
			{
				gvInFlash.mData.mErrInf_PastIndex = arIndex;							/* �Ō�IndexNo. */
				gvInFlash.mData.mErrInf_PastTime = wkTimeInfo;							/* �ŌÎ��� */
			}
			else
			{
				wkRdErrInfo.mTimeInfo = wkTimeInfo;									/* ������� */
				
				wkRdErrInfo.mItem = (ET_ErrInfItm_t)((arReadData[3] & 0x03U) << 3U);	/* �x�񍀖� */
				wkRdErrInfo.mItem |= (arReadData[4] & 0xE0U) >> 5U;
				
				wkRdErrInfo.mAlmFlg = (arReadData[4] & 0x10U) >> 4U;					/* �A���[���t���O */
				
				/* ���[�h�����ُ헚���𖳐��o�b�t�@�Ɋi�[ */
				ApiRfDrv_MeasErr_StorageRfBuff( &wkRdErrInfo, arLoop, arKosu, arIndex );
				/* ���[�h�������i�[���� */
				if( arLoop == arKosu - 1U )
				{
					wkRet = M_BIT0;
				}
			}
			break;
			
		/* �v���x�� */
		case ecRegionKind_MeasAlm:
			
			/* �ŌÏ��Z�b�g */
			if( arRdStatus == 2U )
			{
				gvInFlash.mData.mMeasAlm_PastIndex = arIndex;							/* �Ō�IndexNo. */
				gvInFlash.mData.mMeasAlm_PastTime = wkTimeInfo;							/* �ŌÎ��� */
			}
			else
			{
				wkRdMeasAlm.mTimeInfo = wkTimeInfo;									/* ������� */
			
				wkRdMeasAlm.mAlmFlg = (arReadData[3] & 0x02U) >> 1U;					/* �A���[���t���O */
				
				wkRdMeasAlm.mAlmChannel = (arReadData[3] & 0x01U) << 1U;				/* �`���l�� */
				wkRdMeasAlm.mAlmChannel |= (arReadData[4] & 0x80U) >> 7U;
				
				wkRdMeasAlm.mLevel = (arReadData[4] & 0x60U) >> 5U;						/* ���x�� */
				
				wkRdMeasAlm.mMeasVal = ((uint16_t)arReadData[4] & 0x001FU) << 8U;		/* �v���l */
				wkRdMeasAlm.mMeasVal |= ((uint16_t)arReadData[5] & 0x00FFU);
				
				wkRdMeasAlm.mItem = (ET_MeasAlmItm_t)((arReadData[6] & 0xF8U) >> 3U);	/* �x�񍀖� */
				
				wkRdMeasAlm.mEvent = (arReadData[6] & 0x04U) >> 2U;						/* �_�� */
				
				/* �����ʐM���A���[�h�����v���l�𖳐��o�b�t�@�Ɋi�[ */
				if( arRdStatus == 0U )
				{
					/* ���[�h�����x�񗚗��𖳐��o�b�t�@�Ɋi�[ */
					ApiRfDrv_MeasAlm_StorageRfBuff( &wkRdMeasAlm, arLoop, arKosu, arIndex );
					/* ���[�h�������i�[���� */
					if( arLoop == arKosu - 1U )
					{
						wkRet = M_BIT1;
					}
				}
				/* ���A���^�C���ʐM���A���[�h�����v���x�񗚗����p�ϐ��Ɋi�[ */
				else
				{
					stFlashRdMeasAlmVal[ arLoop ] = wkRdMeasAlm;
					wkRet = M_BIT1;
				}
			}
			break;
			
		/* �v���l1 */
		/* �C�x���g */
		case ecRegionKind_Meas1:
		case ecRegionKind_Event:
			
			/* �ŌÏ��Z�b�g */
			if( arRdStatus == 2U )
			{
				if( arRegionKind == ecRegionKind_Meas1 )
				{
					gvInFlash.mData.mMeas1_PastIndex = arIndex;								/* �Ō�IndexNo. */
					gvInFlash.mData.mMeas1_PastTime = wkTimeInfo;							/* �ŌÎ��� */
				}
				else
				{
					gvInFlash.mData.mEvt_PastIndex = arIndex;								/* �Ō�IndexNo. */
					gvInFlash.mData.mEvt_PastTime = wkTimeInfo;								/* �ŌÎ��� */
				}
			}
			else
			{
				wkRdMeas.mTimeInfo = wkTimeInfo;											/* ������� */
				
				wkRdMeas.mMeasVal[ 0U ] = ((uint16_t)arReadData[ 3U ] & 0x0003U) << 11U;	/* 1ch�v���l */
				wkRdMeas.mMeasVal[ 0U ] |= ((uint16_t)arReadData[ 4U ] & 0x00FFU) << 3U;
				wkRdMeas.mMeasVal[ 0U ] |= ((uint16_t)arReadData[ 5U ] & 0x00E0U) >> 5U;
				
				wkRdMeas.mLogic = (arReadData[ 5U ] & 0x10U) >> 4U;							/* �_�� */
				
				wkRdMeas.mDevFlg[ 0U ] = arReadData[ 5U ] & 0x0FU;							/* 1ch��E�t���O */
				
				wkRdMeas.mAlmFlg[ 0U ] = (arReadData[ 6U ] & 0xF0U) >> 4U;					/* 1ch�x��t���O */
				
				wkRdMeas.mMeasVal[ 1U ] = ((uint16_t)arReadData[ 6U ] & 0x000FU) << 9U;		/* 2ch�v���l */
				wkRdMeas.mMeasVal[ 1U ] |= ((uint16_t)arReadData[ 7U ] & 0x00FFU) << 1U;
				wkRdMeas.mMeasVal[ 1U ] |= ((uint16_t)arReadData[ 8U ] & 0x0080U) >> 7U;
				
				wkRdMeas.m4chDevAlmFlg = (arReadData[ 8U ] & 0x40) >> 6U;					/* 4ch��E�t���O */
				
				wkRdMeas.mDevFlg[ 1U ] = (arReadData[ 8U ] & 0x3CU) >> 2U;					/* 2ch��E�t���O */
				
				wkRdMeas.mAlmFlg[ 1U ] = (arReadData[ 8U ] & 0x03U) << 2U;					/* 2ch�x��t���O */
				wkRdMeas.mAlmFlg[ 1U ] |= (arReadData[ 9U ] & 0xC0U) >> 6U;
				
				wkRdMeas.mMeasVal[ 2U ] = ((uint16_t)arReadData[ 9U ] & 0x003FU) << 7U;		/* 3ch�v���l */
				wkRdMeas.mMeasVal[ 2U ] |= ((uint16_t)arReadData[ 10U ] & 0x00FE) >> 1U;
				
				wkRdMeas.m4chDevAlmFlg = (arReadData[ 10U ] & 0x01U) << 4U;					/* 4ch�x��t���O */
				
				wkRdMeas.mDevFlg[ 2U ] = (arReadData[ 11U ]) & 0xF0 >> 4U;					/* 3ch��E�t���O */
				
				wkRdMeas.mAlmFlg[ 2U ] = arReadData[ 11U ] & 0x0FU;							/* 3ch�x��t���O */
				
				if( arRegionKind == ecRegionKind_Meas1 )
				{
					/* �����ʐM���A���[�h�����v���l�𖳐��o�b�t�@�Ɋi�[ */
					if( arRdStatus == 0U )
					{
						ApiRfDrv_MeasData_StorageRfBuff( &wkRdMeas, arLoop, arKosu, arIndex );
						/* ���[�h�������i�[���� */
						if( arLoop == arKosu - 1U )
						{
							wkRet = M_BIT2;
						}
					}
					/* ���A���^�C���ʐM���A���[�h�����v���l���p�ϐ��Ɋi�[ */
					else
					{
						stFlashRdMeasVal1[ arLoop ] = wkRdMeas;
						wkRet = M_BIT2;
					}
				}
				else
				{
					ApiRfDrv_MeasEvt_StorageRfBuff( &wkRdMeas, arLoop, arKosu, arIndex );
					/* ���[�h�������i�[���� */
					if( arLoop == arKosu - 1U )
					{
						wkRet = M_BIT4;
					}
				}
			}
			break;
			
		/* ���엚�� */
		case ecRegionKind_ActLog:
			
			/* �ŌÏ��Z�b�g */
			if( arRdStatus == 2U )
			{
				gvInFlash.mData.mActLog_PastIndex = arIndex;						/* �Ō�IndexNo. */
				gvInFlash.mData.mActLog_PastTime = wkTimeInfo;						/* �ŌÎ��� */
			}
			else
			{
				wkRdActLog.mTimeInfo = wkTimeInfo;									/* ������� */
				
				wkRdActLog.mItem = (ET_ActLogItm_t)((arReadData[3] & 0x03U) << 8U);	/* �������� */
				wkRdActLog.mItem |= arReadData[4];
				
				wkRdActLog.mData = (uint32_t)arReadData[5] << 16U;					/* �f�[�^ */
				wkRdActLog.mData |= (uint32_t)arReadData[6] << 8U;
				wkRdActLog.mData |= (uint32_t)arReadData[7];
				
				wkRdActLog.mUserId = arReadData[8];									/* �ύX��ID */
				
				/* ���[�h�������엚���𖳐��o�b�t�@�Ɋi�[ */
				ApiRfDrv_ActLog_StorageRfBuff( &wkRdActLog, arLoop, arKosu, arIndex );
				/* ���[�h�������i�[���� */
				if( arLoop == arKosu - 1U )
				{
					wkRet = M_BIT5;
				}
			}
			break;
			
		/* �V�X�e�����O */
		case ecRegionKind_SysLog:
			
			/* �ŌÏ��Z�b�g */
			if( arRdStatus == 2U )
			{
				gvInFlash.mData.mSysLog_PastIndex = arIndex;						/* �Ō�IndexNo. */
				gvInFlash.mData.mSysLog_PastTime = wkTimeInfo;						/* �ŌÎ��� */
			}
			else
			{
				wkRdSysLog.mTimeInfo = wkTimeInfo;									/* ������� */
				
				wkRdSysLog.mItem = (ET_SysLogItm_t)((arReadData[3] & 0x03U) << 2U);	/* �������� */
				wkRdSysLog.mItem |= (arReadData[4] & 0xE0U) >> 6U;
				
				wkRdSysLog.mData = ((uint32_t)arReadData[4] & 0x1FU) << 19U;		/* �f�[�^ */
				wkRdSysLog.mData |= ((uint32_t)arReadData[5] & 0xFFU) << 11U;
				wkRdSysLog.mData |= ((uint32_t)arReadData[6] & 0xFFU) << 3U;
				wkRdSysLog.mData |= ((uint32_t)arReadData[7] & 0xE0U) >> 5U;
				
				/* ���[�h�����V�X�e�������𖳐��o�b�t�@�Ɋi�[ */
				ApiRfDrv_SysLog_StorageRfBuff( &wkRdSysLog, arLoop, arKosu, arIndex );
				/* ���[�h�������i�[���� */
				if( arLoop == arKosu - 1U )
				{
					wkRet = M_BIT6;
				}
			}
			break;
			
		default:
			break;
	}
	return wkRet;
}


/*
 *******************************************************************************
 *	1byte��4byte�f�[�^�ϊ�����
 *
 *	[���e]
 *		1byte���Ɏ����ԃf�[�^��4byte�f�[�^�ɕϊ�����B
 *******************************************************************************
 */
static uint32_t SubFlash_MakeTime4Byte( const uint8_t *parTimeData )
{
	uint32_t	wkRet;
	
	wkRet = (uint32_t)parTimeData[ 0U ] << 24U;
	wkRet |= (uint32_t)parTimeData[ 1U ] << 16U;
	wkRet |= (uint32_t)parTimeData[ 2U ] << 8U;
	wkRet |= (uint32_t)parTimeData[ 3U ];
	wkRet >>= 2U;
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�������݃f�[�^�쐬����
 *
 *	[���e]
 *		�f�[�^�L���[���̃f�[�^���������ݗp�o�b�t�@�ɒu��������B
 *******************************************************************************
 */
static void SubFlash_MakeWriteData( const uint8_t *parQueData, uint8_t arWrData[], ET_RegionKind_t arRegionKind )
{
	uint8_t		wkLoop;
	uint8_t		wkDataByte;
	
	/* �������݃f�[�^�o�b�t�@�������� */
	for( wkLoop = 0U ; wkLoop < M_ArrayElement(arWrData) ; wkLoop++ )
	{
		arWrData[wkLoop] = 0U;
	}
	
	switch( arRegionKind )
	{
		/* �ُ��� */
		case ecRegionKind_ErrInfo:
			wkDataByte = imDataByteErrInf;
			break;
			
		/* �v���x�� */
		case ecRegionKind_MeasAlm:
			wkDataByte = imDataByteMeasAlm;
			break;
			
		/* �v���l1 */
		case ecRegionKind_Meas1:
			wkDataByte = imDataByteMeas1;
			break;
			
		/* �C�x���g */
		case ecRegionKind_Event:
			wkDataByte = imDataByteEvt;
			break;
			
		/* ���엚�� */
		case ecRegionKind_ActLog:
			wkDataByte = imDataByteActLog;
			break;
			
		/* �V�X�e�����O */
		case ecRegionKind_SysLog:
			wkDataByte = imDataByteSysLog;
			break;
			
		/* �v���l1�A�V�X�e�����O�A�h���X */
		case ecRegionKind_Meas1Adr:
		case ecRegionKind_SysLogAdr:
			wkDataByte = imDataByteAdrTbl;
			break;
			
		default:
			return;
	}
	
	for( wkLoop = 0U; wkLoop < wkDataByte; wkLoop++ )
	{
		arWrData[wkLoop] = parQueData[wkLoop];
	}
}


/*
 *******************************************************************************
 *	�Z�N�^�������菈��
 *
 *	[���e]
 *		�Z�N�^����������s���B
 *		�߂�l�F0���Z�N�^���������B 1���Z�N�^�����L��B
 *******************************************************************************
 */
static uint8_t SubFlash_SctrErsJudge( uint32_t arAddrEnd, uint32_t arAddrFin, ET_RegionKind_t arRegionKind )
{
	uint8_t		wkRet = imNASI;
	uint32_t	wkWrEndSctrNum;					/* �������ݏI�[�Z�N�^�ԍ� */
	uint32_t	wkWrFinSctrNum;					/* �������ݍŏI�Z�N�^�ԍ� */
	uint32_t	wkSctrSize;						/* �Z�N�^�T�C�Y */
	uint32_t	wkStartSctrAddr;
	uint32_t	wkSctrNum;
	
	switch( arRegionKind )
	{
		/* �ُ��� */
		case ecRegionKind_ErrInfo:
			wkSctrSize = imFlashErrInfoSctrSize;
			break;
			
		/* �v���x�� */
		case ecRegionKind_MeasAlm:
			wkSctrSize = imFlashMeasAlmSctrSize;
			break;
			
		/* �v���l1 */
		case ecRegionKind_Meas1:
			wkSctrSize = imFlashMeas1SctrSize;
			break;
			
		/* �C�x���g */
		case ecRegionKind_Event:
			wkSctrSize = imFlashEvtSctrSize;
			break;
			
		/* ���엚�� */
		case ecRegionKind_ActLog:
			wkSctrSize = imFlashActLogSctrSize;
			break;
			
		/* �V�X�e�����O */
		case ecRegionKind_SysLog:
			wkSctrSize = imFlashSysLogSctrSize;
			break;
			
		/* �v���l1�A�V�X�e�����O�A�h���X */
		case ecRegionKind_Meas1Adr:
		case ecRegionKind_SysLogAdr:
			wkSctrSize = imFlashAdrTblSctrSize;
			break;
			
		default:
			break;
	}
	wkSctrNum = arAddrEnd / imFlashSctrSize;
	wkStartSctrAddr = wkSctrNum * imFlashSctrSize;
	
	wkWrEndSctrNum = (arAddrEnd - wkStartSctrAddr) / wkSctrSize;		/* �������ݏI�[�Z�N�^�ԍ� */
	wkWrFinSctrNum = (arAddrFin - wkStartSctrAddr) / wkSctrSize;		/* �������ݍŏI�Z�N�^�ԍ� */
	
	if( wkWrEndSctrNum != wkWrFinSctrNum )
	{
		wkRet = imARI;													/* �Z�N�^�����L�� */
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�����ݍŏI�A�h���X�Z�b�g
 *
 *	[���e]
 *		�f�[�^��ނŃZ�N�^�Ԃ��܂������̂��𔻒肵�A
 *		�����ݍŏI�A�h���X�̒l���Z�b�g����
 *******************************************************************************
 */
static void SubFlash_FinAdrSet( uint32_t arAddrEnd, uint32_t arAddrFin, ET_RegionKind_t arRegionKind )
{
	uint32_t	wkNextAreaAddrHead;
	uint32_t	wkAreaAddrEnd;
	uint32_t	wkSctrNum;
	
	wkSctrNum = arAddrEnd / imFlashSctrSize;
	
	switch( arRegionKind )
	{
		/* �ُ��� */
		case ecRegionKind_ErrInfo:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrErrInf;
			if( wkSctrNum == imSctrNoMaxErrInf )
			{
				wkNextAreaAddrHead = imSctrNoMinErrInf * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* �v���x�� */
		case ecRegionKind_MeasAlm:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrMeasAlm;
			if( wkSctrNum == imSctrNoMaxMeasAlm )
			{
				wkNextAreaAddrHead = imSctrNoMinMeasAlm * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* �v���l1 */
		case ecRegionKind_Meas1:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrMeas1;
			if( wkSctrNum == imSctrNoMaxMeas1 )
			{
				wkNextAreaAddrHead = imSctrNoMinMeas1 * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* �C�x���g */
		case ecRegionKind_Event:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrEvt;
			if( wkSctrNum == imSctrNoMaxEvt )
			{
				wkNextAreaAddrHead = imSctrNoMinEvt * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* ���엚�� */
		case ecRegionKind_ActLog:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrActLog;
			if( wkSctrNum == imSctrNoMaxActLog )
			{
				wkNextAreaAddrHead = imSctrNoMinActLog * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* �V�X�e�����O */
		case ecRegionKind_SysLog:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrSysLog;
			if( wkSctrNum == imSctrNoMaxSysLog )
			{
				wkNextAreaAddrHead = imSctrNoMinSysLog * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* �v���l1�A�h���X */
		case ecRegionKind_Meas1Adr:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrAdrTbl;
			if( wkSctrNum == imSctrNoMaxMeas1Adr )
			{
				wkNextAreaAddrHead = imSctrNoMinMeas1Adr * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		/* �V�X�e�����O�A�h���X */
		case ecRegionKind_SysLogAdr:
			wkAreaAddrEnd = (imFlashSctrSize * wkSctrNum) + imEndAddrAdrTbl;
			if( wkSctrNum == imSctrNoMaxSysLogAdr )
			{
				wkNextAreaAddrHead = imSctrNoMinSysLogAdr * imFlashSctrSize;
			}
			else
			{
				wkNextAreaAddrHead = imFlashSctrSize * (wkSctrNum + 1);
			}
			break;
			
		default:
			return;
	}
	
	/* �������ݍŏI�A�h���X���G���A�I�[�A�h���X�ȏ�̏ꍇ */
	if( stRegionData.mWrAddrFin > wkAreaAddrEnd )
	{
		stRegionData.mWrAddrFin = wkNextAreaAddrHead;
	}
}


/*
 *******************************************************************************
 *	�������ݏ���
 *
 *	[���e]
 *		�������ݏ������s���B
 *******************************************************************************
 */
static ET_Error_t SubFlash_Write( uint32_t arAddr, uint8_t *parWrDataBuff, uint32_t arDataSize )
{
	ET_Error_t		wkFlashRet[ 2U ] = {ecERR_OK, ecERR_OK};
	ET_Error_t		wkRet = ecERR_NG;
	uint8_t			wkRdData[ imFlashRdWr_DatSize ];
	int16_t			wkCmpRet;
	uint8_t			wkRetryCnt;
	
	
	if( arDataSize != 0U )
	{
		/* �v���C�}���������ݎ��s */
		for( wkRetryCnt = 0U ; wkRetryCnt < imRetryMax ; wkRetryCnt++ )
		{
			wkFlashRet[ ecFlashKind_Prim ] = ApiFlashDrv_WriteMemory( arAddr, &parWrDataBuff[0], (uint16_t)arDataSize, (ET_FlashKind_t)ecFlashKind_Prim );
			if( wkFlashRet[ ecFlashKind_Prim ] == ecERR_OK )
			{
				wkFlashRet[ ecFlashKind_Prim ] = ApiFlashDrv_ReadData( arAddr, &wkRdData[0], (uint16_t)arDataSize, (ET_FlashKind_t)ecFlashKind_Prim );
				if( wkFlashRet[ ecFlashKind_Prim ] == ecERR_OK )
				{
					wkCmpRet = memcmp( &parWrDataBuff[0], &wkRdData[0], arDataSize );
					if( wkCmpRet == 0U )
					{															/* �������ݐ���I�� */
						wkFlashRet[ ecFlashKind_Prim ] = ecERR_OK;				/* OK */
						break;
					}
					else
					{
						wkFlashRet[ ecFlashKind_Prim ] = ecERR_NG;				/* NG */
					}
				}
			}
		}
		if( wkFlashRet[ ecFlashKind_Prim ] != ecERR_OK )
		{
			SubFlash_AbnSet( ecFlashKind_Prim );
		}
		
		/* �Z�J���_���������ݎ��s */
		for( wkRetryCnt = 0U ; wkRetryCnt < imRetryMax ; wkRetryCnt++ )
		{
			wkFlashRet[ ecFlashKind_Second ] = ApiFlashDrv_WriteMemory( arAddr, &parWrDataBuff[0], (uint16_t)arDataSize, (ET_FlashKind_t)ecFlashKind_Second );
			if( wkFlashRet[ ecFlashKind_Second ] == ecERR_OK )
			{
				wkFlashRet[ ecFlashKind_Second ] = ApiFlashDrv_ReadData( arAddr, &wkRdData[0], (uint16_t)arDataSize, (ET_FlashKind_t)ecFlashKind_Second );
				if( wkFlashRet[ ecFlashKind_Second ] == ecERR_OK )
				{
					wkCmpRet = memcmp( &parWrDataBuff[0], &wkRdData[0], arDataSize );
					/* �������ݐ���I�� */
					if( wkCmpRet == 0U )
					{
						wkFlashRet[ ecFlashKind_Second ] = ecERR_OK;			/* OK */
						break;
					}
					else
					{
						wkFlashRet[ ecFlashKind_Second ] = ecERR_NG;			/* NG */
					}
				}
			}
		}
		if( wkFlashRet[ ecFlashKind_Second ] != ecERR_OK )
		{
			SubFlash_AbnSet( ecFlashKind_Second );
		}
		
		/* �v���C�}���A�Z�J���_���ǂ��炩���� */
		if(( wkFlashRet[ ecFlashKind_Prim ] == ecERR_OK )						/* �v���C�}���������ݐ��� */
		|| ( wkFlashRet[ ecFlashKind_Second ] == ecERR_OK ))					/* �Z�J���_���������ݐ��� */
		{
			stRegionData.mWrAddrEnd = (arAddr + arDataSize) - 1U;				/* �������ݍŏI�A�h���X�X�V */
			SubFlash_WrEndAddrUpdate( stRegionData.mWrAddrEnd );				/* �������ݏI�[�A�h���X��̈�했�ɍX�V */
			wkRet = ecERR_OK;													/* �߂�l�FOK */
		}
		
#if 0	/* �@��ُ한�A�����Ȃ� */
		/* �v���C�}���������ݐ��� */
		if( wkFlashRet[ ecFlashKind_Prim ] == ecERR_OK )
		{
			/* �v���C�}���������ُ한�A */
			ApiAbn_AbnStsClr( imAbnSts_PMEM, ecAbnInfKind_AbnSts );
		}
		
		/* �Z�J���_���������ݐ��� */
		if( wkFlashRet[ ecFlashKind_Second ] == ecERR_OK )
		{
			/* �Z�J���_���������ُ한�A */
			ApiAbn_AbnStsClr( imAbnSts_SMEM, ecAbnInfKind_AbnSts );
		}
#endif
	}
	else
	{
		wkRet = ecERR_OK;
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�Ǎ�����
 *
 *	[���e]
 *		�I��Flash����l��ǂݍ���
 *	[����]
 *		uint32_t	arAddr: �A�h���X
 *		uint8_t		*parRdDataBuff: ���[�h�o�b�t�@
 *		uint32_t	arDataSize: ���[�h�T�C�Y
 *	[�ߒl]
 *		ET_Error_t	:�G���[���
 *******************************************************************************
 */
static ET_Error_t SubFlash_Read( uint32_t arAddr, uint8_t *parRdDataBuff, uint32_t arDataSize )
{
	ET_Error_t		wkRet = ecERR_NG;
	ET_FlashKind_t	wkSel;
#if 0	/* �@��ُ한�A�����Ȃ� */
	uint16_t		wkAbnSts;
#endif
	
	if( arDataSize != 0U )
	{
#if 0	/* �@��ُ한�A�����Ȃ� */
		if( gvInFlash.mParam.mFlashSelect == (uint8_t)ecFlashKind_Prim )
		{
			wkSel = ecFlashKind_Prim;
			wkAbnSts = imAbnSts_PMEM;
		}
		else
		{
			wkSel = ecFlashKind_Second;
			wkAbnSts = imAbnSts_SMEM;
		}
#else
		if( gvInFlash.mParam.mFlashSelect == (uint8_t)ecFlashKind_Prim )
		{
			wkSel = ecFlashKind_Prim;
		}
		else
		{
			wkSel = ecFlashKind_Second;
		}
#endif
		wkRet = ApiFlashDrv_ReadData( arAddr, &parRdDataBuff[0], (uint16_t)arDataSize, wkSel );
		
#if 0	/* �@��ُ한�A�����Ȃ� */
		/* �t���b�V���G���[���� */
		if( wkRet == ecERR_OK )
		{
			/* �v���C�}��or�Z�J���_���������ُ한�A */
			ApiAbn_AbnStsClr( wkAbnSts, ecAbnInfKind_AbnSts );
			}
		else
		{
			/* �Z�J���_���p�̃��g���C�񐔍X�V */
			if( wkSel == ecFlashKind_Prim )
			{																	/* �v���C�}���I���� */
				SubFlash_RetryCntUpDate( imRetryMax, wkRet, ecERR_OK );			/* ���g���C�J�E���^�X�V */
		}
		else
			{																	/* �Z�J���_���I���� */
				SubFlash_RetryCntUpDate( imRetryMax, ecERR_OK, wkRet );			/* ���g���C�J�E���^�X�V */
			}
		}
#else
		/* �t���b�V���G���[���� */
		if( wkRet != ecERR_OK )
		{
			/* �Z�J���_���p�̃��g���C�񐔍X�V */
			if( wkSel == ecFlashKind_Prim )
			{																	/* �v���C�}���I���� */
				SubFlash_RetryCntUpDate( imRetryMax, wkRet, ecERR_OK );			/* ���g���C�J�E���^�X�V */
			}
			else
			{																	/* �Z�J���_���I���� */
				SubFlash_RetryCntUpDate( imRetryMax, ecERR_OK, wkRet );			/* ���g���C�J�E���^�X�V */
			}
		}
#endif
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�������݌����AIndexNo.�X�V
 *
 *	[���e]
 *		�������݌����AIndexNo.�̍X�V�������s���B
 *	[����]
 *		uint8_t		arSctrChg: �Z�N�^�ύX�L��
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubFlash_WrNumIndexUpdate( uint8_t arSctrChg )
{
	uint16_t	wkNum;
	uint16_t	wkPastPt_Min;
	
	switch( stRegionData.mRegionKind )
	{
		/* �ُ��� */
		case ecRegionKind_ErrInfo:
			if( gvInFlash.mData.mErrInf_Num < imDataNumErrInf )
			{
				gvInFlash.mData.mErrInf_Num++;						/* �������݌����X�V */
			}
			
			gvInFlash.mData.mErrInf_Index++;						/* IndexNo.�X�V */
			if( gvInFlash.mData.mErrInf_Index > imIndexMaxErrInf )
			{
				gvInFlash.mData.mErrInf_Index = 1U;
			}
			break;
			
		/* �v���x�� */
		case ecRegionKind_MeasAlm:
			if( gvInFlash.mData.mMeasAlm_Num < imDataNumMeasAlm )
			{
				gvInFlash.mData.mMeasAlm_Num++;						/* �������݌����X�V */
			}
			
			gvInFlash.mData.mMeasAlm_Index++;						/* IndexNo.�X�V */
			if( gvInFlash.mData.mMeasAlm_Index > imIndexMaxMeasAlm )
			{
				gvInFlash.mData.mMeasAlm_Index = 1U;
			}
			break;
			
		/* �v���l1 */
		case ecRegionKind_Meas1:
			if( gvInFlash.mData.mMeas1_Num < imDataNumMeas1 )
			{
				gvInFlash.mData.mMeas1_Num++;						/* �������݌����X�V */
			}
			
			gvInFlash.mData.mMeas1_Index++;							/* IndexNo.�X�V */
			if( gvInFlash.mData.mMeas1_Index > imIndexMaxMeas1 )
			{
				gvInFlash.mData.mMeas1_Index = 1U;
				gvInFlash.mData.mMeas1_IndexNextLap = imON;			/* IndexNo.��1������ */
				gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;		/* ����Flash�����ݍX�V */
			}
			
			/* ���^�����A���^�C�����M�|�C���^�ɒǂ������Ƃ��̓|�C���^�X�V */
			if( gvInFlash.mData.mMeas1_Num == imDataNumMeas1 )
			{
//				wkPastPt_Min = imIndexMaxMeas1 + 1U - imDataNumMeas1 + gvInFlash.mData.mMeas1_Index;
				wkPastPt_Min = 197U + gvInFlash.mData.mMeas1_Index;				/* (�ő�IndexNo. + 1 - �ő�� -> 197) + ���݂�IndexNo. */
				
				if( wkPastPt_Min <= imIndexMaxMeas1 )
				{
					if( gvInFlash.mData.mMeas1_OldPtrIndex >= gvInFlash.mData.mMeas1_Index && gvInFlash.mData.mMeas1_OldPtrIndex <= wkPastPt_Min )
					{
						gvInFlash.mData.mMeas1_OldPtrIndex = wkPastPt_Min;
						
						/* �C�x���g�L�����瑗�M */
						if( gvInFlash.mParam.mRealTimeSndPt == 510U )
						{
							/* �C�x���g�L���|�C���^�ɉߋ����M�|�C���^���� */
							gvInFlash.mData.mMeas1_EvtPtrIndex = gvInFlash.mData.mMeas1_OldPtrIndex;
						}
					}
				}
				else
				{
					wkPastPt_Min -= imIndexMaxMeas1;
					if( (gvInFlash.mData.mMeas1_OldPtrIndex >= gvInFlash.mData.mMeas1_Index && gvInFlash.mData.mMeas1_OldPtrIndex <= imIndexMaxMeas1) ||
						(gvInFlash.mData.mMeas1_OldPtrIndex <= wkPastPt_Min) )
					{
						gvInFlash.mData.mMeas1_OldPtrIndex = wkPastPt_Min;
						
						/* �C�x���g�L�����瑗�M */
						if( gvInFlash.mParam.mRealTimeSndPt == 510U )
						{
							/* �C�x���g�L���|�C���^�ɉߋ����M�|�C���^���� */
							gvInFlash.mData.mMeas1_EvtPtrIndex = gvInFlash.mData.mMeas1_OldPtrIndex;
						}
					}
				}
			}
			
			/* Flash�̍ŐV����f�[�^�𖳐����M�p��RAM�Ɋi�[ */
			SubFlash_RfRTMeasDataSet();								/* �v���f�[�^�̃��A���^�C�����M�p�f�[�^���� */
			
			/* �Z�N�^�ύX���� */
			if( arSctrChg == imARI )
			{
				wkNum = gvInFlash.mData.mMeasVal1Adr_Num;
				if( wkNum < imDataNumAdrTbl )
				{
					wkNum++;
				}
				else
				{
					wkNum = 1U;
				}
				
				/* ����l1�A�h���X�X�V */
				ApiFlash_WriteFlasAdrTbl( gvInFlash.mData.mMeas1_AddrEnd, wkNum, gvInFlash.mData.mMeas1_Index, ecRegionKind_Meas1Adr );
			}
			
			/* �����ʐM�N��(�ݒ�f�[�^�������܂����獂���ʐM�Ҏ�) */
			if( gvFstConnBootCnt < imDataNumMeas1 )
			{
				gvFstConnBootCnt++;									/* �����ʐM�N���f�[�^���X�V */
			}
			
#if 0
			/* 100�f�[�^��RAM��ۑ� */
			if( gvInFlash.mData.mMeas1_Index % 100U == 1U )
			{
				gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;			/* ����Flash�����ݍX�V */
			}
#endif
			break;
			
		/* �C�x���g */
		case ecRegionKind_Event:
			if( gvInFlash.mData.mEvt_Num < imDataNumEvt )
			{
				gvInFlash.mData.mEvt_Num++;							/* �������݌����X�V */
			}
			
			gvInFlash.mData.mEvt_Index++;							/* IndexNo.�X�V */
			if( gvInFlash.mData.mEvt_Index > imIndexMaxEvt )
			{
				gvInFlash.mData.mEvt_Index = 1U;
			}
			break;
			
		/* ���엚�� */
		case ecRegionKind_ActLog:
			if( gvInFlash.mData.mActLog_Num < imDataNumActLog )
			{
				gvInFlash.mData.mActLog_Num++;						/* �������݌����X�V */
			}
			
			gvInFlash.mData.mActLog_Index++;						/* IndexNo.�X�V */
			if( gvInFlash.mData.mActLog_Index > imIndexMaxActLog )
			{
				gvInFlash.mData.mActLog_Index = 1U;
			}
			break;
			
		/* �V�X�e�����O */
		case ecRegionKind_SysLog:
			if( gvInFlash.mData.mSysLog_Num < imDataNumSysLog )
			{
				gvInFlash.mData.mSysLog_Num++;						/* �V�X�e�����O�̏������݌����X�V */
			}
			
			gvInFlash.mData.mSysLog_Index++;						/* IndexNo.�X�V */
			if( gvInFlash.mData.mSysLog_Index > imIndexMaxSysLog )
			{
				gvInFlash.mData.mSysLog_Index = 1U;
			}
			
			/* �Z�N�^�ύX���� */
			if( arSctrChg == imARI )
			{
				wkNum = gvInFlash.mData.mSysLogAdr_Num;
				if( gvInFlash.mData.mSysLogAdr_Num < imDataNumAdrTbl )
				{
					wkNum++;
				}
				else
				{
					wkNum = 1U;
				}
				
				/* �V�X�e�����O�A�h���X�X�V */
				ApiFlash_WriteFlasAdrTbl( gvInFlash.mData.mSysLog_AddrEnd, wkNum, gvInFlash.mData.mSysLog_Index, ecRegionKind_SysLogAdr );
			}
			break;
			
		/* �v���l1�A�h���X */
		case ecRegionKind_Meas1Adr:
			if( gvInFlash.mData.mMeasVal1Adr_Num < imDataNumAdrTbl )
			{
				gvInFlash.mData.mMeasVal1Adr_Num++;
			}
			else
			{
				gvInFlash.mData.mMeasVal1Adr_Num = 1U;
			}
			break;
			
		/* �V�X�e�����O�A�h���X */
		case ecRegionKind_SysLogAdr:
			if( gvInFlash.mData.mSysLogAdr_Num < imDataNumAdrTbl )
			{
				gvInFlash.mData.mSysLogAdr_Num++;
			}
			else
			{
				gvInFlash.mData.mSysLogAdr_Num = 1U;
			}
			break;
			
		default:
			break;
	}
	
	/* �Z�N�^�ύX���� */
	if( arSctrChg == imARI )
	{
		gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;			/* ����Flash�����ݍX�V */
	}
}


/*
 *******************************************************************************
 *	�������ݏI�[�A�h���X�X�V
 *
 *	[���e]
 *		�������ݏI�[�A�h���X�̍X�V�������s���B
 *	[����]
 *		uint32_t		arWrEndAddr: �������ݏI�[�A�h���X
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubFlash_WrEndAddrUpdate( uint32_t arWrEndAddr )
{
	switch(stRegionData.mRegionKind)
	{
		/* �ُ��� */
		case ecRegionKind_ErrInfo:
			gvInFlash.mData.mErrInf_AddrEnd = arWrEndAddr;				/* �������ݏI�[�A�h���X */
			break;
			
		/* �v���x�� */
		case ecRegionKind_MeasAlm:
			gvInFlash.mData.mMeasAlm_AddrEnd = arWrEndAddr;				/* �������ݏI�[�A�h���X */
			break;
			
		/* �v���l1 */
		case ecRegionKind_Meas1:
			gvInFlash.mData.mMeas1_AddrEnd = arWrEndAddr;				/* �������ݏI�[�A�h���X */
			break;
			
		/* �C�x���g */
		case ecRegionKind_Event:
			gvInFlash.mData.mEvt_AddrEnd = arWrEndAddr;					/* �������ݏI�[�A�h���X */
			break;
			
		/* ���엚�� */
		case ecRegionKind_ActLog:
			gvInFlash.mData.mActLog_AddrEnd = arWrEndAddr;				/* �������ݏI�[�A�h���X */
			break;
			
		/* �V�X�e�����O */
		case ecRegionKind_SysLog:
			gvInFlash.mData.mSysLog_AddrEnd = arWrEndAddr;				/* �������ݏI�[�A�h���X */
			break;
			
		/* �v���l1�A�h���X */
		case ecRegionKind_Meas1Adr:
			gvInFlash.mData.mMeasVal1Adr_AddrEnd = arWrEndAddr;			/* �������ݏI�[�A�h���X */
			break;
			
		/* �V�X�e�����O�A�h���X */
		case ecRegionKind_SysLogAdr:
			gvInFlash.mData.mSysLogAdr_AddrEnd = arWrEndAddr;			/* �������ݏI�[�A�h���X */
			break;
			
		default:
			break;
	}
}


/*
 *******************************************************************************
 *	�O�t���t���b�V����Ԏ擾����
 *
 *	[���e]
 *	�O�t���t���b�V����Ԃ�Ԃ��B
 *******************************************************************************
 */
ET_FlashSts_t ApiGetFlashSts( void )
{
	return stFlashSts;
}

/*
 *******************************************************************************
 *	�O�t���t���b�V����ԃZ�b�g
 *
 *	[���e]
 *	�O�t���t���b�V����Ԃ��Z�b�g����B
 *******************************************************************************
 */
void ApiSetFlashSts( ET_FlashSts_t arSts )
{
	stFlashSts = arSts;
}


/*
 *******************************************************************************
 *	�f�[�^�L���[�̃f�[�^�L�����擾
 *
 *	[���e]
 *		�f�[�^�L���[�Ƀf�[�^�����邩�ǂ������擾����B
 *******************************************************************************
 */
uint8_t ApiFlash_GetQueSts( void )
{
	uint8_t wkRet = imNASI;
	
	if( stFlashQue.mNum > 0U )
	{
		wkRet = imARI;				/* �f�[�^�L�� */
	}
	
	return wkRet;
}

#if 0
/*
 *******************************************************************************
 *	��d���p�̃f�[�^�L���[�̃f�[�^�L�����擾
 *
 *	[���e]
 *		��d���p�̃f�[�^�L���[�Ƀf�[�^�����邩�ǂ������擾����B
 *******************************************************************************
 */
uint8_t ApiFlash_GetQueLowBatSts( void )
{
	uint8_t wkRet = imNASI;
	
	if( stFlashQueLowBat.mNum > 0U )
	{
		wkRet = imARI;				/* �f�[�^�L�� */
	}
	
	return wkRet;
}
#endif

/*
 *******************************************************************************
 *	�O�t���t���b�V���̃��[�h�v�����͊�����Ԃ̃��[�h
 *
 *	[���e]
 *		�O�t���t���b�V���̃��[�h�v�����͊�����Ԃ̃��[�h
 *	[����]
 *		uint8_t			arSelect: ���[�h:0�A�v��:1(�����t���O�N���A��)
 *		ET_RegionKind_t	arRegion: �̈���
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
uint8_t ApiSetFlash_ReqRdFlg( uint8_t arSelect, ET_RegionKind_t arRegion )
{
	uint8_t	wkBit;
	uint8_t	wkRet = imNASI;
	
	switch( arRegion )
	{
		/* �ُ��� */
		case ecRegionKind_ErrInfo:
			wkBit = M_BIT0;
			break;
			
		/* �v���x�� */
		case ecRegionKind_MeasAlm:
			wkBit = M_BIT1;
			break;
			
		/* ����l1 */
		case ecRegionKind_Meas1:
			wkBit = M_BIT2;
			break;
			
		/* �C�x���g */
		case ecRegionKind_Event:
			wkBit = M_BIT4;
			break;
			
		/* ���엚�� */
		case ecRegionKind_ActLog:
			wkBit = M_BIT5;
			break;
			
		/* �V�X�e�����O */
		case ecRegionKind_SysLog:
			wkBit = M_BIT6;
			break;
			
		default:
			break;
	}
	
	switch( arSelect )
	{
		case 0U:
			if( M_TSTBIT( stFlashReqRd.mFin, wkBit) )
			{
				wkRet = imARI;
				M_CLRBIT( stFlashReqRd.mFin, wkBit );			/* Flash���[�h�����N���A */
			}
			break;
		case 1U:
			M_CLRBIT( stFlashReqRd.mFin, wkBit );				/* Flash���[�h�����N���A */
			M_SETBIT( stFlashReqRd.mReq, wkBit );				/* Flash���[�h�v���Z�b�g */
		gvModuleSts.mExFlash = ecExFlashModuleSts_Run;
			break;
		default:
			break;
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	Flash���烊�[�h�������������𔻒f����
 *
 *	[���e]
 *	Flash���烊�[�h�������������𔻒f����
 *******************************************************************************
 */
void ApiFlash_FinReadData( void )
{
	/* �ُ��� */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT0) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT0) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT0 );			/* Flash���[�h�v���N���A */
		}
	}
	
	/* �v���x�� */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT1) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT1) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT1 );			/* Flash���[�h�v���N���A */
		}
	}
	
	/* ����l1 */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT2) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT2) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT2 );			/* Flash���[�h�v���N���A */
		}
	}
	
	/* ����l2 */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT3) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT3) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT3 );			/* Flash���[�h�v���N���A */
		}
	}
	
	/* �C�x���g */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT4) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT4) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT4 );			/* Flash���[�h�v���N���A */
		}
	}
	
	/* ���엚�� */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT5) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT5) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT5 );			/* Flash���[�h�v���N���A */
		}
	}
	
	/* �V�X�e�����O */
	if( M_TSTBIT( stFlashReqRd.mReq, M_BIT6) )
	{
		if( M_TSTBIT( stFlashReqRd.mFin, M_BIT6) )
		{
			M_CLRBIT( stFlashReqRd.mReq, M_BIT6 );			/* Flash���[�h�v���N���A */
		}
	}
	
	/* �S�Ẵ��[�h�v�����Ȃ��Ȃ�����X���[�v */
	if( stFlashReqRd.mReq == 0U )
	{
		gvModuleSts.mExFlashRd = ecExFlashRdModuleSts_Sleep;
	}
}


/*
 *******************************************************************************
 *	Flash�������݂̑�������������
 *
 *	[���e]
 *		�ǂ��܂�Flash�ŏ����݂���Ă��邩���������A
 *		�I�[�A�h���X�A�f�[�^���A�C���f�b�N�XNo.�ɃZ�b�g����B
 *******************************************************************************
 */
void ApiFlash_SrchFlashAdr( void )
{
	ET_SrchKindLog_t	wkKindLoop;
	ET_FlashKind_t		wkSel;
	
	uint8_t				vPrimRdData[ imFlashRdWr_DatSize ];		/* Flash���[�h�f�[�^ */
	uint8_t				wkSctrLoop;
	uint8_t				wkDataByte;			/* �����݃o�C�g�� */
	uint8_t				wkSctrNoMinAdr;		/* �A�h���X����e�[�u���̍ŏ��Z�N�^No. */
	uint8_t				wkSctrNoMin;		/* �����ݗ̈�ŏ��Z�N�^No. */
	uint8_t				wkSctrNoMax;		/* �����ݗ̈�ő�Z�N�^No. */
	uint8_t				wkRsvSize;			/* �Z�N�^�Ԃ̖��g�p�T�C�Y */
	uint8_t				wkSctrNumFlash;		/* Flash�Ō������ē���ꂽ�Z�N�^No. */
	uint8_t				wkSctrNumGlobal;	/* global�ϐ��Ŏ����Ă���Z�N�^No. */
	
	uint16_t			wkNum;				/* �A�h���X�����e�[�u���̌��� */
	uint16_t			wkNumPre;
	uint16_t			wkIndexMax;			/* ������IndexNo. */
	uint16_t			wkDataNum;			/* �����݌��� */
	uint16_t			wkSrchNum;			/* �������ꂽ���� */
	uint16_t			wkSrchIndex;		/* �������ꂽIndexNo. */
	
	uint32_t			wkAdrLoop;
	uint32_t			wkAdr;
	uint32_t			wkSctrDiff;
	uint32_t			wkSrchAddrEnd;		/* �������ꂽ�G���h�A�h���X */
	uint32_t			wkSrchAdr;
	uint32_t			wkSrchAdrPre;
	uint32_t			wkClock;			/* Flash���[�h�����f�[�^ */
	uint32_t			wkPreClock;
	uint32_t			wkU32;
	
	/* Flash�d��ON */
	ApiFlash_FlashPowerCtl( imON, imON );
	
	if( gvInFlash.mParam.mFlashSelect == (uint8_t)ecFlashKind_Prim )			/* �v���C�}���I���� */
	{
		wkSel = ecFlashKind_Prim;
	}
	else																		/* �Z�J���_���I���� */
	{
		wkSel = ecFlashKind_Second;
	}
	
	/*** �ُ���A�v���x��A�C�x���g�A���엚�� ***/
	for( wkKindLoop = ecSrchKindLog_ErrInfo ; wkKindLoop <= ecSrchKindLog_ActLog ; wkKindLoop++ )
	{
		switch( wkKindLoop )
		{
			/* �ُ��� */
			case ecSrchKindLog_ErrInfo:
				wkDataNum = imDataNumInSctrErrInf;
				wkDataByte = imDataByteErrInf;
				wkSctrNoMin = imSctrNoMinErrInf;
				wkSctrNoMax = imSctrNoMaxErrInf;
				wkIndexMax = imIndexMaxErrInf;
				wkSrchAddrEnd = gvInFlash.mData.mErrInf_AddrEnd;
				wkSrchNum = gvInFlash.mData.mErrInf_Num;
				wkSrchIndex = gvInFlash.mData.mErrInf_Index;
				break;
				
			/* �v���x�� */
			case ecSrchKindLog_MeasAlm:
				wkDataNum = imDataNumInSctrMeasAlm;
				wkDataByte = imDataByteMeasAlm;
				wkSctrNoMin = imSctrNoMinMeasAlm;
				wkSctrNoMax = imSctrNoMaxMeasAlm;
				wkIndexMax = imIndexMaxMeasAlm;
				wkSrchAddrEnd = gvInFlash.mData.mMeasAlm_AddrEnd;
				wkSrchNum = gvInFlash.mData.mMeasAlm_Num;
				wkSrchIndex = gvInFlash.mData.mMeasAlm_Index;
				break;
				
			/* �C�x���g */
			case ecSrchKindLog_Evt:
				wkDataNum = imDataNumInSctrEvt;
				wkDataByte = imDataByteEvt;
				wkSctrNoMin = imSctrNoMinEvt;
				wkSctrNoMax = imSctrNoMaxEvt;
				wkIndexMax = imIndexMaxEvt;
				wkSrchAddrEnd = gvInFlash.mData.mEvt_AddrEnd;
				wkSrchNum = gvInFlash.mData.mEvt_Num;
				wkSrchIndex = gvInFlash.mData.mEvt_Index;
				break;
				
			/* ���엚�� */
			case ecSrchKindLog_ActLog:
				wkDataNum = imDataNumInSctrActLog;
				wkDataByte = imDataByteActLog;
				wkSctrNoMin = imSctrNoMinActLog;
				wkSctrNoMax = imSctrNoMaxActLog;
				wkIndexMax = imIndexMaxActLog;
				wkSrchAddrEnd = gvInFlash.mData.mActLog_AddrEnd;
				wkSrchNum = gvInFlash.mData.mActLog_Num;
				wkSrchIndex = gvInFlash.mData.mActLog_Index;
				break;
				
			default:
				break;
		}
		
		wkClock = 0U;
		wkPreClock = 0U;
		wkSrchAdr = wkSctrNoMin * imFlashSctrSize;
		for( wkSctrLoop = 0U ; wkSctrLoop < 2U ; wkSctrLoop++ )
		{
			wkPreClock = wkClock;
			wkAdr = (wkSctrNoMin * imFlashSctrSize) + (wkSctrLoop * imFlashSctrSize);
			for( wkAdrLoop = 0U ; wkAdrLoop < wkDataNum ; wkAdrLoop++ )
			{
				ApiFlashDrv_ReadData( wkAdr, &vPrimRdData[ 0 ], wkDataByte, wkSel );
				
				wkClock = (uint32_t)vPrimRdData[ 0U ] << 24U;
				wkClock |= (uint32_t)vPrimRdData[ 1U ] << 16U;
				wkClock |= (uint32_t)vPrimRdData[ 2U ] << 8U;
				wkClock |= vPrimRdData[ 3U ];
				wkClock >>= 2U;
				
				if( wkClock == 0x3FFFFFFF )
				{
					if( wkAdrLoop == 0U )										/* �ŏ��̃A�h���X���� */
					{
						if( wkSctrLoop != 0U )									/* ��Z�N�^�������̏ꍇ */
						{
							wkSrchAdr = (wkSctrNoMin * imFlashSctrSize) + (wkDataByte * wkDataNum);
																				/* �O�Z�N�^���S�A�h���X�Ƀf�[�^�������ݍς݁B�O�Z�N�^�̏I���A�h���X(�f�[�^����)���i�[ */
						}
					}
					else														/* �ŏ��ȊO�̃A�h���X���� */
					{
						wkSrchAdr = wkAdr;										/* �ŏI�A�h���X(�f�[�^�L��)���i�[ */
					}
					wkSctrLoop = 2U;
					break;
				}
				if(( wkAdrLoop == 0U )											/* �ŏ��̃A�h���X���� */
				&& ( wkSctrLoop != 0U )											/* ��Z�N�^������ */
				)
				{
					if( wkPreClock > wkClock )									/* �O�Z�N�^�̍ŏI�f�[�^�̎������ �� ��Z�N�^�̍ŏ��f�[�^�̎������ */
					{															/* �ŐV�̃f�[�^���O�Z�N�^�̍ŏI�f�[�^�̎�����񂾂����ꍇ */
						wkSrchAdr = (wkSctrNoMin * imFlashSctrSize) + (wkDataByte * wkDataNum);
						break;
					}
				}
				wkAdr += wkDataByte;
			}
		}
		
		/* �S�Z�N�^�Ƀf�[�^�������Ă���ꍇ */
		if( wkAdr >= ((wkSctrNoMax * imFlashSctrSize) + (wkDataByte * wkDataNum)) )
		{
			wkSrchAdr = (wkSctrNoMax * imFlashSctrSize) + (wkDataByte * wkDataNum) - wkDataByte;
		}
		/* �����݌����AIndexNo.�Z�o */
		wkSctrNumFlash = wkSrchAdr / imFlashSctrSize;
		wkSctrNumGlobal = wkSrchAddrEnd / imFlashSctrSize;
		
		/* Flash���烊�[�h�����O���[�o���ϐ��̃A�h���X�ƌ��������ŏI�A�h���X�̍����Z�o */
		if( wkSctrNumFlash == wkSctrNumGlobal )
		{
			if( wkSrchAdr >= wkSrchAddrEnd )
			{
				wkU32 = wkSrchAdr - wkSrchAddrEnd + 1U;
			}
			else
			{
				/* error */
				wkU32 = 0U;
			}
		}
		else
		{
			wkU32 = (wkSctrNumGlobal * imFlashSctrSize) + (wkDataNum * wkDataByte) - wkSrchAddrEnd;
			wkU32 += wkSrchAdr - (wkSctrNumFlash * imFlashSctrSize) + 1U;
		}
		wkU32 /= wkDataByte;
		
		/* �������݌��� */
		wkSrchNum += (uint16_t)wkU32;
		if( wkSrchNum > wkDataNum )
		{
			wkSrchNum = wkDataNum;
		}
		
		/* ��������IndexNo. */
		wkSrchIndex += (uint16_t)wkU32;
		if( wkSrchIndex > wkIndexMax )
		{
			wkSrchIndex -= wkIndexMax;
		}
		
		/* �������ݏI�[�A�h���X */
		if( wkSrchAdr != wkSrchAddrEnd )
		{
			wkSrchAddrEnd = wkSrchAdr - 1U;
		}
		
		switch( wkKindLoop )
		{
			/* �ُ��� */
			case ecSrchKindLog_ErrInfo:
				gvInFlash.mData.mErrInf_Num = wkSrchNum;
				gvInFlash.mData.mErrInf_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mErrInf_Index = wkSrchIndex;
				break;
				
			/* �v���x�� */
			case ecSrchKindLog_MeasAlm:
				gvInFlash.mData.mMeasAlm_Num = wkSrchNum;
				gvInFlash.mData.mMeasAlm_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mMeasAlm_Index = wkSrchIndex;
				break;
				
			/* �C�x���g */
			case ecSrchKindLog_Evt:
				gvInFlash.mData.mEvt_Num = wkSrchNum;
				gvInFlash.mData.mEvt_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mEvt_Index = wkSrchIndex;
				break;
				
			/* ���엚�� */
			case ecSrchKindLog_ActLog:
				gvInFlash.mData.mActLog_Num = wkSrchNum;
				gvInFlash.mData.mActLog_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mActLog_Index = wkSrchIndex;
				break;
				
			default:
				break;
		}
	}
	
	
	/*** �v���l1�A�V�X�e�����O ***/
	for( wkKindLoop = ecSrchKindLog_AdrTblMeas1 ; wkKindLoop <= ecSrchKindLog_AdrTblSysLog ; wkKindLoop++ )
	{
		/* �A�h���X�e�[�u������ */
		switch( wkKindLoop )
		{
			/* �A�h���X�e�[�u���F�v���l1 */
			case ecSrchKindLog_AdrTblMeas1:
				wkDataNum = imDataNumMeas1;
				wkDataByte = imDataByteAdrTbl;
				wkSctrNoMinAdr = imSctrNoMinMeas1Adr;
				wkSctrNoMin = imSctrNoMinMeas1;
				wkIndexMax = imIndexMaxMeas1;
				break;
				
			/* �A�h���X�e�[�u���F�V�X�e�����O */
			case ecSrchKindLog_AdrTblSysLog:
				wkDataNum = imDataNumSysLog;
				wkDataByte = imDataByteAdrTbl;
				wkSctrNoMinAdr = imSctrNoMinSysLogAdr;
				wkSctrNoMin = imSctrNoMinSysLog;
				wkIndexMax = imIndexMaxSysLog;
				break;
			default:
				break;
		}
		
		wkNumPre = 0U;
		wkAdr = wkSctrNoMinAdr * imFlashSctrSize;
		for( wkAdrLoop = wkAdr ; wkAdrLoop < (wkAdr + 0x3000) ; wkAdrLoop += wkDataByte )
		{
			ApiFlashDrv_ReadData( wkAdrLoop, &vPrimRdData[ 0 ], wkDataByte, wkSel );
			
			wkSrchAdr = (uint32_t)vPrimRdData[ 0U ] << 24U;
			wkSrchAdr |= (uint32_t)vPrimRdData[ 1U ] << 16U;
			wkSrchAdr |= (uint32_t)vPrimRdData[ 2U ] << 8U;
			wkSrchAdr |= vPrimRdData[ 3U ];
			
			wkNum = (uint16_t)vPrimRdData[ 4U ] << 8U;
			wkNum |= vPrimRdData[ 5U ];
			
			if( (wkNum == 0xFFFF) ||
				( (wkNum != wkNumPre + 1U) && !(wkNumPre == imDataNumAdrTbl) ) ||
				( (wkNum != 1U) && (wkNumPre == imDataNumAdrTbl) ) )
			{
				/* �擪�Z�N�^�̐擪�A�h���X�̏ꍇ */
				if( (wkAdrLoop == wkAdr) && (wkSrchAdr == 0xFFFFFFFFU) )
				{
					/* �Ō�̃A�h���X�Ƀf�[�^�����邩���m�F */
					ApiFlashDrv_ReadData( wkAdr + 0x2FF8U, &vPrimRdData[ 0 ], wkDataByte, wkSel );
					
					wkSrchAdr = (uint32_t)vPrimRdData[ 0U ] << 24U;
					wkSrchAdr |= (uint32_t)vPrimRdData[ 1U ] << 16U;
					wkSrchAdr |= (uint32_t)vPrimRdData[ 2U ] << 8U;
					wkSrchAdr |= vPrimRdData[ 3U ];
					
					wkNum = (uint16_t)vPrimRdData[ 4U ] << 8U;
					wkNum |= vPrimRdData[ 5U ];
					
					/* �Ō�̃A�h���X�ɂ��f�[�^���Ȃ��Ƃ��̓A�h���X�e�[�u���Ƀf�[�^�Ȃ� */
					if( wkNum == 0xFFFF )
					{
						wkSrchAdr = wkSctrNoMin * imFlashSctrSize;
						wkNum = 0U;
					}
					/* �Ō�̃A�h���X�Ƀf�[�^������Ƃ��̓��[�h�������e���̗p */
					else
					{
						wkAdr += 0x2FF8U;
					}
				}
				else
				{
					wkAdr = wkAdrLoop - 1U;
					wkSrchAdr = wkSrchAdrPre;
					wkNum = wkNumPre;
				}
				break;
			}
			wkNumPre = wkNum;
			wkSrchAdrPre = wkSrchAdr;
		}
		
		if( wkAdrLoop >= ((wkSctrNoMinAdr * imFlashSctrSize) + (imSctrNumAdrTbl * imFlashSctrSize)) )
		{																		/* �S�Z�N�^�Ƀf�[�^�������Ă���ꍇ */
			wkAdr = wkAdrLoop - 1U;
		}
		
		/* �A�h���X�e�[�u�������ƂɁA�v���l1�A�V�X�e�����O���� */
		switch( wkKindLoop )
		{
			/* �A�h���X�e�[�u���F�v���l1 */
			case ecSrchKindLog_AdrTblMeas1:
				gvInFlash.mData.mMeasVal1Adr_Num = wkNum;
				gvInFlash.mData.mMeasVal1Adr_AddrEnd = wkAdr;
				wkDataByte = imDataByteMeas1;
				wkSrchAddrEnd = gvInFlash.mData.mMeas1_AddrEnd;
				wkSrchNum = gvInFlash.mData.mMeas1_Num;
				wkSrchIndex = gvInFlash.mData.mMeas1_Index;
				wkRsvSize = imRsvSizeMeas1;
				break;
				
			/* �A�h���X�e�[�u���F�V�X�e�����O */
			case ecSrchKindLog_AdrTblSysLog:
				gvInFlash.mData.mSysLogAdr_Num = wkNum;
				gvInFlash.mData.mSysLogAdr_AddrEnd = wkAdr;
				wkDataByte = imDataByteSysLog;
				wkSrchAddrEnd = gvInFlash.mData.mSysLog_AddrEnd;
				wkSrchNum = gvInFlash.mData.mSysLog_Num;
				wkSrchIndex = gvInFlash.mData.mSysLog_Index;
				wkRsvSize = imRsvSizeSysLog;
				break;
				
			default:
				break;
		}
		
		wkAdr = wkSrchAdr / imFlashSctrSize;
		M_NOP;			/* �œK���h�~ */
		wkAdr *= imFlashSctrSize;
		wkSrchAdr = wkAdr;
		for( wkAdrLoop = wkAdr ; wkAdrLoop < (wkAdr + 0x1000) ; wkAdrLoop += wkDataByte )
		{
			ApiFlashDrv_ReadData( wkAdrLoop, &vPrimRdData[ 0 ], wkDataByte, wkSel );
			
			wkClock = (uint32_t)vPrimRdData[ 0U ] << 24U;
			wkClock |= (uint32_t)vPrimRdData[ 1U ] << 16U;
			wkClock |= (uint32_t)vPrimRdData[ 2U ] << 8U;
			wkClock |= vPrimRdData[ 3U ];
			wkClock >>= 2U;
			
			if( wkClock == 0x3FFFFFFF )
			{
				if( wkAdrLoop == wkAdr )
				{
					wkSrchAdr = wkAdrLoop;
				}
				else
				{
					wkSrchAdr = wkAdrLoop - 1U;
				}
				break;
			}
			else
			{
				if(( wkAdrLoop + wkDataByte ) >= ( wkAdr + 0x1000 ))
				{																/* �����������ʁA�S�ẴA�h���X�Ƀf�[�^���������܂�Ă����ꍇ */
					wkSrchAdr = wkAdrLoop + wkDataByte - 1U;
					break;
				}
			}
		}
		
		/* �����݌����AIndexNo.�Z�o */
		if( wkSrchAdr >= wkSrchAddrEnd )
		{
			/* �Z�N�^�Ԃ̖��g�p�T�C�Y */
			wkSctrDiff = (wkSrchAdr / imFlashSctrSize) - (wkSrchAddrEnd / imFlashSctrSize);
			wkSctrDiff *= wkRsvSize;
			/* �����AIndexNo.�Z�o */
			wkU32 = wkSrchAdr - wkSrchAddrEnd - wkSctrDiff + 1U;
			wkU32 /= wkDataByte;
		}
		else
		{
			/* error */
			wkU32 = 0U;
		}
		
		/* �������݌��� */
		wkSrchNum += (uint16_t)wkU32;
		if( wkSrchNum >= wkDataNum )
		{
			wkSrchNum = wkDataNum;
		}
		
		/* ��������IndexNo. */
		wkSrchIndex += (uint16_t)wkU32;
		
		if( wkSrchIndex > wkIndexMax )
		{
			wkSrchIndex -= wkIndexMax;
		}
		
		/* �������ݏI�[�A�h���X */
		wkSrchAddrEnd = wkSrchAdr;
		
		
		switch( wkKindLoop )
		{
			/* �A�h���X�e�[�u���F�v���l1 */
			case ecSrchKindLog_AdrTblMeas1:
				gvInFlash.mData.mMeas1_Num = wkSrchNum;
				gvInFlash.mData.mMeas1_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mMeas1_Index = wkSrchIndex;
				break;
				
			/* �A�h���X�e�[�u���F�V�X�e�����O */
			case ecSrchKindLog_AdrTblSysLog:
				gvInFlash.mData.mSysLog_Num = wkSrchNum;
				gvInFlash.mData.mSysLog_AddrEnd = wkSrchAddrEnd;
				gvInFlash.mData.mSysLog_Index = wkSrchIndex;
				break;
				
			default:
				break;
		}
	}
	/* Flash�d��OFF */
	ApiFlash_FlashPowerCtl( imOFF, imOFF );
}


/*
 *******************************************************************************
 * �v���f�[�^�̃��A���^�C�����M�p�f�[�^����
 *
 *	[���e]
 *		�v���f�[�^�̃��A���^�C�����M�p�f�[�^����
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubFlash_RfRTMeasDataSet( void )
{
	uint8_t			wkLoop;
	uint32_t		wkU32;
	int16_t			wkS16Array[ 4U ];
	
	/* �ϐ������� */
	memset( &gvRfRTMeasData, 0, sizeof(gvRfRTMeasData) );
	
	/* �ُ�X�e�[�^�X�A�d�r�c�� */
	ApiFlash_RfRTAbnStatusSet();
	
	/* �ŐV�v���l */
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		wkS16Array[ wkLoop ] = stFlashWrMeasVal1.mMeasVal[ wkLoop ];
		wkS16Array[ wkLoop ] += gvInFlash.mProcess.mModelOffset;		/* ����l�𑗐M�p�ɃI�t�Z�b�g */
	}
	/* �����p�v���g�R���Ɍv���l���������� */
	ApiRfDrv_SetMeaAlmValArray( &wkS16Array[ 0U ], &gvRfRTMeasData.mRTMeasData_MeasVal[ 0U ] );
	
	/* �ŐV�f�[�^���� */
	for( wkLoop = 0U ; wkLoop < 4U; wkLoop++ )
	{
		wkU32 = stFlashWrMeasVal1.mTimeInfo >> (24U - wkLoop * 8U);
		gvRfRTMeasData.mRTMeasData_MeasTime[ wkLoop ] = (uint8_t)(wkU32 & 0x000000FF);
	}
	
	/* �ŐV�v���l�f�[�^���n��ID */
	gvRfRTMeasData.mRTMeasData_MeasID[ 0U ] = (uint8_t)(gvInFlash.mData.mMeas1_Index & 0x00FF);
	gvRfRTMeasData.mRTMeasData_MeasID[ 1U ] = (uint8_t)((gvInFlash.mData.mMeas1_Index >> 8U) & 0x00FF);
	
	
	/* �_�� */
	gvRfRTMeasData.mRTMeasData_MeasVal[ 4U ] &= 0x7F;
	gvRfRTMeasData.mRTMeasData_MeasVal[ 4U ] |= (stFlashWrMeasVal1.mLogic << 7U);
	
	
	/* �ŐV�x��t���O */
	ApiFlash_RfRTAlarmFlagSet( gvMeasPrm.mAlmFlg, gvMeasPrm.mDevFlg, gvRfRTMeasData.mRTMeasData_AlarmFlag );
	for( wkLoop = 0U ; wkLoop < 4U ; wkLoop++ )
	{
		gvHsAlmFlag[ wkLoop ] = gvMeasPrm.mDevFlg[ wkLoop ];
		gvHsAlmFlag[ wkLoop ] |= gvMeasPrm.mAlmFlg[ wkLoop ] << 4U;
	}
	
	/* �d���N�����ɍŐV����l�Ƃ��č̗p���邽�߁A����Flash�������̈�Ɋi�[ */
	gvInFlash.mData.mNewTimeInfo = stFlashWrMeasVal1.mTimeInfo;				/* �ŐV���� */
	gvInFlash.mData.mNewMeasVal[ 0U ] = stFlashWrMeasVal1.mMeasVal[ 0U ];	/* �ŐV����l */
	gvInFlash.mData.mNewMeasVal[ 1U ] = stFlashWrMeasVal1.mMeasVal[ 1U ];
	gvInFlash.mData.mNewMeasVal[ 2U ] = stFlashWrMeasVal1.mMeasVal[ 2U ];
	gvInFlash.mData.mNewLogic = stFlashWrMeasVal1.mLogic;					/* �ŐV�_�� */
}


/*
 *******************************************************************************
 * Flash���烊�[�h�����v���l1���Z�b�g���ĕԂ�
 *
 *	[���e]
 *		Flash���烊�[�h�����v���l1���Z�b�g���ĕԂ�
 *	[����]
 *		ST_FlashVal1Evt_t	arRfRtMeasData[]: ���A���^�C�����[�h�v���l1
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiFlash_RfRTMeasDataGet( ST_FlashVal1Evt_t arRfRtMeasData[] )
{
	uint8_t wkLoop;
	
	for( wkLoop = 0U ; wkLoop < 9U ; wkLoop++ )
	{
		arRfRtMeasData[ wkLoop ] = stFlashRdMeasVal1[ wkLoop ];
	}
}

/*
 *******************************************************************************
 * Flash���烊�[�h�����v���x�񗚗����Z�b�g���ĕԂ�
 *
 *	[���e]
 *		Flash���烊�[�h�����v���x�񗚗����Z�b�g���ĕԂ�
 *	[����]
 *		ST_FlashMeasAlm_t	arRfRtMeasAlmData[]: �v���x�񗚗�(�������ʐM�p)
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiFlash_RfRTMeasAlmDataGet( ST_FlashMeasAlm_t arRfRtMeasAlmData[] )
{
	uint8_t wkLoop;
	
	for( wkLoop = 0U ; wkLoop < 6U ; wkLoop++ )
	{
		arRfRtMeasAlmData[ wkLoop ] = stFlashRdMeasAlmVal[ wkLoop ];
	}
}


/*
 *******************************************************************************
 * Flash�d���|�[�gON/OFF
 *
 *	[���e]
 *		Flash�d���|�[�g��ON/OFF����
 *	[����]
 *		uint8_t	arSel:	imOFF:OFF,imON:ON
 *		uint8_t	arWait:	imOFF:Wait�������Ŏ��{�AimON:Wait������{
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiFlash_FlashPowerCtl( uint8_t arSel, uint8_t arWait )
{
	uint32_t wkLoop;
	
	if( arSel == imON )
	{
		P_FLASH_POWER = (uint8_t)imLow;
		if( arWait == imON )
		{
			for( wkLoop = 0U ; wkLoop < 18400U; wkLoop++ )
			{
				M_NOP;						/* 10�X�e�[�g(=10/(18.4*10^6)=0.54us, 0.54us * 18400 = 10ms) */
			}
		}
		/* CS:Hi */
		ApiFlashDrv_CSCtl( ecFlashKind_Prim, imHigh );
		ApiFlashDrv_CSCtl( ecFlashKind_Second, imHigh );
	}
	else
	{
		P_FLASH_POWER = (uint8_t)imHigh;
	}
}


/*
 *******************************************************************************
 * �������̍Č`��
 *
 *	[���e]
 *		�O�t���t���b�V���ɏ������񂾃f�[�^��񂩂�A�������݂̂��Č`������B
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�O�t���t���b�V���ɏ������񂾍ŐV�̎������
 *******************************************************************************
 */
static uint32_t SubFlash_ReMakeTime( uint8_t arWrData[] )
{
	uint32_t wkRet;
	
	wkRet = ((uint32_t)arWrData[ 0U ] << 24U) & 0xFF000000U;
	wkRet |= ((uint32_t)arWrData[ 1U ] << 16U) & 0x00FF0000U;
	wkRet |= ((uint32_t)arWrData[ 2U ] << 8U) & 0x0000FF00U;
	wkRet |= arWrData[ 3U ];
	wkRet >>= 2U;
	
	return wkRet;
}


/*
 *******************************************************************************
 * �󂫃f�[�^�L���[����
 *
 *	[���e]
 *		�f�[�^�L���[�ɋ󂫂����邩���肷��B
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�󂫗L��FecERR_OK
 *		�󂫖����FecERR_NG
 *******************************************************************************
 */
ET_Error_t ApiFlash_GetEmptyQueueJdg( void )
{
	ET_Error_t wkRet = ecERR_NG;
	
	/* �󂫗L�� */
	if( stFlashQue.mNum < imFlashQue_DatNum )
	{
		wkRet = ecERR_OK;
	}
	
	return wkRet;
}


/*
 *******************************************************************************
 *	�ݒ�ύX���e���L���[�ɂ��߂�
 *
 *	[���e]
 *		�ݒ�ύX�����������A���̓��e��ݒ�ύX�����̃L���[�ɂ��߂�
 *	[����]
 *		ET_ActLogItm_t	arItem: �ݒ荀��
 *		uint32_t		arNowVal: ���݂̐ݒ�l
 *		uint32_t		arSetVal: �ݒ�l
 *		uint8_t			arUser: ���[�U�[ID
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiFlash_QueActHist( ET_ActLogItm_t arItem, uint32_t arNowVal, uint32_t arSetVal, uint8_t arUser )
{
	if( arNowVal != arSetVal )
	{
		if( gvActQue.mNum < 90U )
		{
			gvActQue.mUserID = arUser;
			gvActQue.mItem[ gvActQue.mNum ] = (uint8_t)arItem;
			gvActQue.mSetVal[ gvActQue.mNum ][ 0U ] = (uint8_t)((arSetVal >> 16U) & 0x000000FFU);
			gvActQue.mSetVal[ gvActQue.mNum ][ 1U ] = (uint8_t)((arSetVal >> 8U) & 0x000000FFU);
			gvActQue.mSetVal[ gvActQue.mNum ][ 2U ] = (uint8_t)(arSetVal & 0x000000FFU);
			gvActQue.mNum ++;
			gvModuleSts.mHistory = ecHistoryModuleSts_Run;
		}
	}
}


/*
 *******************************************************************************
 *	Flash���C�g�L���[�ɐݒ�ύX���e�𓮍엚���Ƃ��ăZ�b�g����
 *
 *	[���e]
 *		Flash���C�g�L���[�ɐݒ�ύX���e�𓮍엚���Ƃ��ăZ�b�g����
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiFlash_StoreActHist( void )
{
	uint32_t wkVal;
	static uint8_t wkNum = 0U;
	
	/* �i�[�����ő�l���� */
	if( stFlashQue.mNum < imFlashQue_DatNum )
	{
		wkVal = (uint32_t)gvActQue.mSetVal[ wkNum ][ 0U ] << 16U;
		wkVal += (uint32_t)gvActQue.mSetVal[ wkNum ][ 1U ] << 8U;
		wkVal += gvActQue.mSetVal[ wkNum ][ 2U ];
		
		ApiFlash_WriteActLog( (ET_ActLogItm_t)gvActQue.mItem[ wkNum ], wkVal, gvActQue.mUserID );
		
		wkNum++;
		if( wkNum >= gvActQue.mNum )
		{
			wkNum = 0U;
			gvActQue.mNum = 0U;
			
			/* ���� */
			gvModuleSts.mHistory = ecHistoryModuleSts_Sleep;
		}
	}
}

/*
 *******************************************************************************
 *	�ُ�X�e�[�^�X�A�d�r�c�ʂ𒷋����ʐM�̕ϐ��Ɋi�[����
 *
 *	[���e]
 *		�ُ�X�e�[�^�X�A�d�r�c�ʂ𒷋����ʐM�̕ϐ��Ɋi�[����
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiFlash_RfRTAbnStatusSet( void )
{
	uint16_t		wkU16;
	
	/* �ُ�X�e�[�^�X */
	gvRfRTMeasData.mRTMeasData_AbnStatus = 0U;
	
	/* �d�r�c�� */
	switch( gvBatAd.mBattSts )
	{
		case ecBattSts_Init:
		case ecBattSts_Non:
		case ecBattSts_Empty:
		default:
			gvRfRTMeasData.mRTMeasData_AbnStatus |= 0x00;
			break;
		case ecBattSts_L:
			gvRfRTMeasData.mRTMeasData_AbnStatus |= 0x01;
			break;
		case ecBattSts_M:
			gvRfRTMeasData.mRTMeasData_AbnStatus |= 0x02;
			break;
		case ecBattSts_H:
			gvRfRTMeasData.mRTMeasData_AbnStatus |= 0x03;
			break;
	}
	
	/* �Z���T�ُ� */
	wkU16 = ApiAbn_AbnStsGet( ecAbnInfKind_MeasSts );
#if (swSensorCom == imEnable)
	if( wkU16 & imAbnSts_OvUdBurnShortErrCh1 )
#else
	if( wkU16 & imAbnSts_OvUdBurnShortCh1 )
#endif
	{
		gvRfRTMeasData.mRTMeasData_AbnStatus |= M_BIT3;
	}
	if( wkU16 & imAbnSts_OvUdBurnShortCh2 )
	{
		gvRfRTMeasData.mRTMeasData_AbnStatus |= M_BIT4;
	}
	if( wkU16 & imAbnSts_OvUdBurnShortCh3 )
	{
		gvRfRTMeasData.mRTMeasData_AbnStatus |= M_BIT5;
	}
	
	/* �������ُ� */
	wkU16 = ApiAbn_AbnStsGet( ecAbnInfKind_AbnSts );
	if( wkU16 & 0x001C )
	{
		gvRfRTMeasData.mRTMeasData_AbnStatus |= M_BIT6;
	}
	/* ���̑��ُ�(�d�r�c��0�A�ቺ������) */
	if( wkU16 & 0x0FE2 )
	{
		gvRfRTMeasData.mRTMeasData_AbnStatus |= M_BIT7;
	}
}


/*
 *******************************************************************************
 *	�ŐV�x��t���O�𒷋����ʐM�̕ϐ��Ɋi�[����
 *
 *	[���e]
 *		�ŐV�x��t���O�𒷋����ʐM�̕ϐ��Ɋi�[����
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiFlash_RfRTAlarmFlagSet( uint8_t arAlmFlg[], uint8_t arDevFlg[], uint8_t arSetFlg[] )
{
	uint8_t			wkLoop;
	uint8_t			wkBitShift;
	uint8_t			wkNum;
	

	arSetFlg[ 0U ] = 0x00U;
	arSetFlg[ 1U ] = 0x00U;
	for( wkLoop = 0U ; wkLoop < 3U ; wkLoop++ )
	{
		switch( wkLoop )
		{
			case 0U:
				wkNum = 0U;
				wkBitShift = 0U;
				break;
			case 1U:
				wkNum = 0U;
				wkBitShift = 4U;
				break;
			case 2U:
				wkNum = 1U;
				wkBitShift = 0U;
				break;
		}
		
		if( arAlmFlg[ wkLoop ] & imAlmFlgHH )
		{
			if( arAlmFlg[ wkLoop ] & imAlmFlgH )
			{
				/* ����x������x�� */
				arSetFlg[ wkNum ] |= (ecAlmFlg_AlmHAlmHH << wkBitShift);
			}
			else if( arDevFlg[ wkLoop ] & imAlmFlgH )
			{
				/* �����E�����x�� */
				arSetFlg[ wkNum ] |= (ecAlmFlg_DevHAlmHH << wkBitShift);
			}
		}
		else if( arDevFlg[ wkLoop ] & imAlmFlgHH )
		{
			if( arAlmFlg[ wkLoop ] & imAlmFlgH )
			{
				/* ����x�������E */
				arSetFlg[ wkNum ] |= (ecAlmFlg_AlmHDevHH << wkBitShift);
			}
			else if( arDevFlg[ wkLoop ] & imAlmFlgH )
			{
				/* �����E������E */
				arSetFlg[ wkNum ] |= (ecAlmFlg_DevHDevHH << wkBitShift);
			}
		}
		else if( arAlmFlg[ wkLoop ] & imAlmFlgH )
		{
			/* ����x�� */
			arSetFlg[ wkNum ] |= (ecAlmFlg_AlmH << wkBitShift);
		}
		else if( arDevFlg[ wkLoop ] & imAlmFlgH )
		{
			/* �����E */
			arSetFlg[ wkNum ] |= (ecAlmFlg_DevH << wkBitShift);
		}
		
		else if( arAlmFlg[ wkLoop ] & imAlmFlgLL )
		{
			if( arAlmFlg[ wkLoop ] & imAlmFlgL )
			{
				/* �����x�񉺉����x�� */
				arSetFlg[ wkNum ] |= (ecAlmFlg_AlmLAlmLL << wkBitShift);
			}
			else if( arDevFlg[ wkLoop ] & imAlmFlgL )
			{
				/* ������E�������x�� */
				arSetFlg[ wkNum ] |= (ecAlmFlg_DevLAlmLL << wkBitShift);
			}
		}
		else if( arDevFlg[ wkLoop ] & imAlmFlgLL )
		{
			if( arAlmFlg[ wkLoop ] & imAlmFlgL )
			{
				/* �����x�񉺉�����E */
				arSetFlg[ wkNum ] |= (ecAlmFlg_AlmLDevLL << wkBitShift);
			}
			else if( arDevFlg[ wkLoop ] & imAlmFlgL )
			{
				/* ������E��������E */
				arSetFlg[ wkNum ] |= (ecAlmFlg_DevLDevLL << wkBitShift);
			}
		}
		else if( arAlmFlg[ wkLoop ] & imAlmFlgL )
		{
			/* �����x�� */
			arSetFlg[ wkNum ] |= (ecAlmFlg_AlmL << wkBitShift);
		}
		else if( arDevFlg[ wkLoop ] & imAlmFlgL )
		{
			/* ������E */
			arSetFlg[ wkNum ] |= (ecAlmFlg_DevL << wkBitShift);
		}
	}
}


/*
 *******************************************************************************
 *	���^�f�[�^�̍ŌÎ��^�������[�h�̃L���[(����p)���Z�b�g
 *
 *	[���e]
 *		���^�f�[�^�̍ŌÎ��^�������[�h�̃L���[(����p)���Z�b�g
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiFlash_ReadQueSetPastTime( void )
{
	SubFlash_ReadQueSetPastTime( ecRegionKind_ErrInfo );
	SubFlash_ReadQueSetPastTime( ecRegionKind_MeasAlm );
	SubFlash_ReadQueSetPastTime( ecRegionKind_Meas1 );
	SubFlash_ReadQueSetPastTime( ecRegionKind_Event );
	SubFlash_ReadQueSetPastTime( ecRegionKind_ActLog );
	SubFlash_ReadQueSetPastTime( ecRegionKind_SysLog );
}


/*
 *******************************************************************************
 *	���^�f�[�^�̍ŌÎ��^�������[�h�̃L���[���Z�b�g
 *
 *	[���e]
 *		���^�f�[�^�̍ŌÎ��^�������[�h�̃L���[���Z�b�g
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
static void SubFlash_ReadQueSetPastTime( ET_RegionKind_t arKind )
{
	uint16_t	wkPastIndex;		/* �Z�o�����Ō�IndexNo. */
	
	uint16_t	wkNumMax;
	uint16_t	wkIndexMax;
	
	uint16_t	wkNowNum;
	uint16_t	wkNowIndex;
	
	ET_Error_t	wkError;
	
	switch( arKind )
	{
		/* �ُ��� */
		case ecRegionKind_ErrInfo:
			wkNumMax = imDataNumErrInf;
			wkIndexMax = imIndexMaxErrInf;
			wkNowNum = gvInFlash.mData.mErrInf_Num;
			wkNowIndex = gvInFlash.mData.mErrInf_Index;
			break;
			
		/* �v���x�� */
		case ecRegionKind_MeasAlm:
			wkNumMax = imDataNumMeasAlm;
			wkIndexMax = imIndexMaxMeasAlm;
			wkNowNum = gvInFlash.mData.mMeasAlm_Num;
			wkNowIndex = gvInFlash.mData.mMeasAlm_Index;
			break;
			
		/* ����l1 */
		case ecRegionKind_Meas1:
			wkNumMax = imDataNumMeas1;
			wkIndexMax = imIndexMaxMeas1;
			wkNowNum = gvInFlash.mData.mMeas1_Num;
			wkNowIndex = gvInFlash.mData.mMeas1_Index;
			break;
			
		/* �C�x���g */
		case ecRegionKind_Event:
			wkNumMax = imDataNumEvt;
			wkIndexMax = imIndexMaxEvt;
			wkNowNum = gvInFlash.mData.mEvt_Num;
			wkNowIndex = gvInFlash.mData.mEvt_Index;
			break;
			
		/* ���엚�� */
		case ecRegionKind_ActLog:
			wkNumMax = imDataNumActLog;
			wkIndexMax = imIndexMaxActLog;
			wkNowNum = gvInFlash.mData.mActLog_Num;
			wkNowIndex = gvInFlash.mData.mActLog_Index;
			break;
			
		/* �V�X�e�����O */
		case ecRegionKind_SysLog:
			wkNumMax = imDataNumSysLog;
			wkIndexMax = imIndexMaxSysLog;
			wkNowNum = gvInFlash.mData.mSysLog_Num;
			wkNowIndex = gvInFlash.mData.mSysLog_Index;
			break;
	}
	
	
	/* �Ō�IndexNo.�Z�o */
	/* �f�[�^��:1�`�ő�l-1 */
	if( wkNowNum < wkNumMax )
	{
		wkPastIndex = 1U;
	}
	/* �f�[�^���F�ő�l */
	else
	{
		if( wkNowIndex >= wkNumMax )
		{
			wkPastIndex = wkNowIndex - wkNumMax + 1;
		}
		else
		{
			/* IndexNo.�����O�o�b�t�@�܂����v�Z */
			wkPastIndex = wkIndexMax + 1 - (wkNumMax - wkNowIndex);
		}
	}
	
	/* Flash�Ƀ��[�h�L���[���Z�b�g */
	wkError = ApiFlash_ReadQueSet( 0U, 0U, wkPastIndex, 1U, arKind, 2U );
	if( wkError != ecERR_OK )
	{
		/* �L���[�������ς�:debug */
		M_NOP;
	}
}

/*
 *******************************************************************************
 *	�d���������A���^����Ă���ŐV���^�f�[�^�𖳐����M�p�ɃZ�b�g����
 *
 *	[���e]
 *		�d���������A���^����Ă���ŐV���^�f�[�^�𖳐����M�p�ɃZ�b�g����
 *	[����]
 *		�Ȃ�
 *	[�ߒl]
 *		�Ȃ�
 *******************************************************************************
 */
void ApiFlash_SetNewMeasVal( void )
{
	/* ����Flash�Ƀ��������Ă����ŐV�f�[�^���Z�b�g */
	if( gvInFlash.mData.mNewTimeInfo != 0xFFFFFFFF )
	{
		stFlashWrMeasVal1.mTimeInfo = gvInFlash.mData.mNewTimeInfo;				/* �ŐV���� */
		memcpy( stFlashWrMeasVal1.mMeasVal, gvInFlash.mData.mNewMeasVal, 3U );
//		stFlashWrMeasVal1.mMeasVal[ 0U ] = gvInFlash.mData.mNewMeasVal[ 0U ];	/* �ŐV����l */
//		stFlashWrMeasVal1.mMeasVal[ 1U ] = gvInFlash.mData.mNewMeasVal[ 1U ];
//		stFlashWrMeasVal1.mMeasVal[ 2U ] = gvInFlash.mData.mNewMeasVal[ 2U ];
		stFlashWrMeasVal1.mLogic = gvInFlash.mData.mNewLogic;					/* �ŐV�_�� */
	}
	
	/* Flash�̍ŐV����f�[�^�𖳐����M�p��RAM�Ɋi�[ */
	SubFlash_RfRTMeasDataSet();
}
#pragma section


