/*
 *******************************************************************************
 *	File name	:	FirmUpMain.c
 *
 *	[���e]
 *		�w�{�t�@�C���̏����T�v���L�q�x
 *------------------------------------------------------------------------------
 *	[�ύX��]		[�ύX��]		[�T�v]
 *	2018.10.25		Softex T.K		�V�K�쐬
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

/* �t���b�V���E�Z���t�E�v���O���~���O�E���C�u����Type01 Ver.2.21B */
#include "fsl.h" 																/* ���C�u�����E�w�b�_�[�t�@�C�� */
#include "fsl_types.h"															/* ���C�u�����E�w�b�_�[�t�@�C�� */

/*
 *==============================================================================
 *	define��`
 *==============================================================================
 */
/* �R�}���h */
#define	imFlashDrv_RDSR1		0x05U											/* Read Status Register-1 */
#define	imFlashDrv_RDDT			0x03U											/* Read Data */

#define	P_CS_PRIM				(P12_bit.no5)									/* �`�b�v�Z���N�g(�v���C�}��) */
#define	P_CS_SECOND				(P6_bit.no1)									/* �`�b�v�Z���N�g(�Z�J���_��) */
#define	P_SDO					(P0_bit.no7)									/* �V���A���ʐM DataOut(Q) */
#define	P_SDI					(P0_bit.no6)									/* �V���A���ʐM DataInput(D) */
#define	P_SCLK					(P0_bit.no5)									/* SCLK(�V���A���N���b�N) */

#define	imFirmUp_SctAddr		0xE7000U										/* �Z�N�^�A�h���X�̐擪�ʒu */
#define	imFirmUp_FlashErsBlkTopAddr		0xD0000U								/* �u���b�N�����A�h���X�̐擪�ʒu */


#define	imFirmUp_TopAddr		0xE7000U										/* �A�h���X�̐擪�ʒu */
#define	imFirmUp_RomMaxAddr		0xFEFFFU										/* �����f�[�^�ۑ��A�h���X�̍ő�l */
#define	imFrimUp_ChkSumTopAddr	0xFF000U										/* �`�F�b�N�T���������ݐ�擪�A�h���X */
#define	imFirmUp_MaxAddr		0xFFFFFU										/* �A�h���X�̍ő�l */

#define	imFirmUp_MaxWriteNum	256U											/* �����݃f�[�^�o�C�g���̍ő�l */
#define	imFirmUp_MaxReadNum		256U											/* �Ǎ��݃f�[�^�o�C�g���̍ő�l */
#define	imFirmUp_ChkSumReadMaxNum	2U											/* �`�F�b�N�T���Ǎ��݃f�[�^�o�C�g���̍ő�l */

#define	imFirmUp_SctrErsMaxCnt	6U												/* �Z�N�^(4KB)�����ő�J�E���g(6cnt�~50ms������300ms) */
#define	imFirmUp_BlkErsMaxCnt	20U												/* �u���b�N(64KB)�����ő�J�E���g(20cnt�~50ms������1000ms) */

#define	imFirmUp_BlkWrMaxNum	95U												/* �t�@�[���A�b�v��ROM�u���b�N�T�C�Y */

#define	FLASH_OK				0U												/* OK */
#define	FLASH_SUCCESS			1U												/* ���� */
#define	FLASH_FAILURE			2U												/* �ُ� */
#define	FLASH_BUSY				3U												/* �r�W�[ */
#define	FLASH_COMPLETE			4U												/* ���� */

#define FL_HEX_ADDR_SIZE		2												/* HEX�f�[�^ �A�h���X�T�C�Y */
#define FL_HEX_TYP_SIZE 		1												/* HEX�f�[�^ �^�C�v�T�C�Y */
#define FL_HEX_SUM_SIZE 		1												/* HEX�f�[�^ ���v�T�C�Y */


/*
 *==============================================================================
 *	�񋓌^��`
 *==============================================================================
 */


/*
 *==============================================================================
 *	�\���̒�`
 *==============================================================================
 */

/* buffer for HEX format data */
typedef struct {
	uint8_t type[2];															/* ":"0", ":"1" and so on */
	uint8_t len[2]; 															/* "0-255" */
	uint8_t addr_data_sum[520];
} Fl_prg_hex_t;

/* buffer for write data
	(this data is the converted data from HEX format data) */
typedef struct {
	uint8_t len;
	uint32_t addr;
	uint8_t data[260];
} Fl_prg_hex_binary_t;


/* buffer for writing flash */
typedef struct {
	uint32_t addr;
	uint8_t data[260];
} Fl_prg_writing_data_t;

/*
 *==============================================================================
 *	�ϐ���`
 *==============================================================================
 */

#pragma section bss FIRMUP_DATA
/* Flag for HEX format complete */
static	uint8_t				fl_hex_data_flag;									/* HEX�f�[�^�t���O */
static	uint32_t			flash_addr;							 				/* �t���b�V�����C�g���̃A�h���X */
static	uint16_t			fl_hex_total_data_cnt;								/* HEX�f�[�^���v�f�[�^�J�E���g */
static	uint8_t				fl_hex_length;										/* HEX�f�[�^�� */
static	uint32_t			sAdr_upper;											/* �A�h���X(���) */
static	uint8_t				vSurplusCnt;										/* �]�萔 */

/* Buffer for the surplus data.  */
static	Fl_prg_hex_binary_t fl_wr_hex_bin_surplus;

/* Writing address */
static	Fl_prg_writing_data_t fl_writing_data;

/* Buffer for HEX format data */
static	Fl_prg_hex_t fl_wr_hex;

/* Buffer for HEX format data which is converted to Binary */
static	Fl_prg_hex_binary_t fl_wr_hex_bin;

#pragma section

/*
 *==============================================================================
 *	�萔��`
 *==============================================================================
 */



/*
 *==============================================================================
 *	�v���g�^�C�v�錾
 *==============================================================================
 */
static void SubStartFirmUpdate( void );
static __far void SubFirmUpdate( void );
static __far fsl_u08 SubFSLStart( void );
static __far ET_Error_t SubFirmUpMain_SubFlash_ReadData( uint32_t arAddress, uint8_t *parRdData, uint16_t arNum );
static __far void SubFSLCmdSnd( uint8_t wkSndData );
static __far uint8_t SubFSLCmdRcv( void );

static __far uint8_t SubR_Fl_PrgTrgtArea( void );
static __far uint8_t SubR_Fl_Prg_StoreHEX( uint8_t hex_data );
static __far void SubR_Fl_Prg_ClearHEXVariables( void );
static __far uint8_t SubR_Fl_AsciiToHexByte( uint8_t in_upper, uint8_t in_lower );
static __far uint8_t SubR_Fl_Prg_ProcessForHEX_data( void );
static __far uint8_t SubR_Fl_Prg_HEX_AsciiToBinary(
						Fl_prg_hex_t *tmp_hex,
						Fl_prg_hex_binary_t *tmp_hex_binary );
static __far uint8_t SubR_Fl_Prg_WriteData( void );
static __far uint8_t SubR_Fl_Prg_MakeWriteData( void );
static __far uint8_t SubR_Fl_Prg_SurplusWriteData( void );

#pragma section text MY_APP2
#if 0
/*
 *******************************************************************************
 *	�t�@�[���A�b�v�f�[�g����������
 *
 *	[���e]
 *	�t�@�[���A�b�v�f�[�g�p�ϐ��̏��������s���B
 *******************************************************************************
 */
void ApiFirmUpdateInit( void )
{
	M_NOP;
	}
#endif

/*
 *******************************************************************************
 *	�t�@�[���A�b�v�f�[�g���C������
 *
 *	[���e]
 *	�V�[�P���X��Ԃɉ����Ċe���������s����B
 *******************************************************************************
 */
void ApiFirmUpdateMain( void )
{
#if 0
			ApiLcd_FirmUpDisp();					/* �t�@�[���A�b�v�\������ */
#else
	ApiLcd_SegSelectOnOff( ecDispSegSelect_SegAllOff );
	ApiLcd_UppLow7SegDsp("FIR", imHigh);
	ApiLcd_UppLow7SegDsp("Up", imLow);
	ApiLcd_Main();
#endif
	ApiInFlash_ParmWrite( ecInFlashWrSts_ParmNow, 0U );		/* ���݂̕ϐ������Flash�ɏ��� */
	SubStartFirmUpdate();
}

/*
 *******************************************************************************
 *	�t�@�[���X�V����
 *
 *	[���e]
 *	�N������Ver�m�F���A�X�V����Ă����痚�����c��
 *******************************************************************************
 */
void ApiFirmUpdate_History( void )
{
	uint32_t wkU32;

	/* �t�@�[���X�V���� */
	if( (gvInFlash.mProcess.mUpVer[ 0U ] != cRomVer[ 0U ]) ||
		(gvInFlash.mProcess.mUpVer[ 1U ] != cRomVer[ 1U ] * 10U + cRomVer[ 2U ]) ||
		(gvInFlash.mProcess.mUpVer[ 2U ] != cRomVer[ 3U ]) )
	{
		gvInFlash.mProcess.mUpVer[ 0U ] = cRomVer[ 0U ];
		gvInFlash.mProcess.mUpVer[ 1U ] = cRomVer[ 1U ] * 10U + cRomVer[ 2U ];
		gvInFlash.mProcess.mUpVer[ 2U ] = cRomVer[ 3U ];
		
		wkU32 = ((uint32_t)cRomVer[ 0U ] << 16U) + ((uint32_t)cRomVer[ 1U ] << 8U) + cRomVer[ 2U ];
		
		/* ���엚��(�t�@�[���E�F�A�X�V: Ver.�A��������) */
		ApiFlash_WriteActLog( ecActLogItm_FrmUpdate, wkU32, gvInFlash.mProcess.mUpUserId );
		gvModuleSts.mInFlash = ecInFlashWrExeSts_Run;
	}
}

/*
 *******************************************************************************
 *	ROM���������J�n����
 *
 *	[���e]
 *	ROM�����������J�n����B
 *******************************************************************************
 */
static void SubStartFirmUpdate( void )
{
	uint8_t __far	*pSrc;
	uint8_t __far	*pDst;
	
	/* �����݋֎~ */
	DI();
	
	/* SPI�|�[�g���蓮�ɐ؂�ւ� */
	R_CSI10_Stop();
	SO0 |= _0400_SAU_CH2_CLOCK_OUTPUT_1;
	SO0 |= _0004_SAU_CH2_DATA_OUTPUT_1;
	PFSEG6 = _00_PFSEG50_PORT | _00_PFSEG49_PORT | _00_PFSEG48_PORT | _01_PFSEG47_SEG;
	P0 = _00_Pn5_OUTPUT_0 | _00_Pn7_OUTPUT_0;
	PM0 = _01_PMn0_NOT_USE | _02_PMn1_NOT_USE | _04_PMn2_NOT_USE | _08_PMn3_NOT_USE | _10_PMn4_NOT_USE | 
		  _00_PMn5_MODE_OUTPUT | _40_PMn6_MODE_INPUT | _00_PMn7_MODE_OUTPUT;
	
	/* �t�@�[���X�V������ROM����RAM�ɃR�s�[ */
	pSrc = (uint8_t __far *)( __sectop( "FIRMUP_APP_f" ) );						/* FIRMUP_APP��ROM����RAM�ɃR�s�[	*/
	pDst = (uint8_t __far *)( __sectop( "FIRMUP_APP_RAM" ) );
	
	for( ; pSrc < (uint8_t __far *)( __secend( "FIRMUP_APP_f" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDT�̃J�E���g���t���b�V��		*/
	}
	
	/* FSL���C�u�����g�p�̈��ROM����RAM�ɃR�s�[ */
	pSrc = (uint8_t __far *)( __sectop( "FSL_FCD" ) ); 							/* FSL_FCD��ROM����RAM�ɃR�s�[ 		*/
	pDst = (uint8_t __far *)( __sectop( "FSL_FCD_RAM" ) );
	
	for( ; pSrc < (uint8_t __far *)( __secend( "FSL_FCD" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDT�̃J�E���g���t���b�V��		*/
	}
	
#if 0
	pSrc = (uint8_t __far *)( __sectop( "FSL_FECD" ) ); 						/* FSL_FECD��ROM����RAM�ɃR�s�[		*/
	pDst = (uint8_t __far *)( __sectop( "FSL_FECD_RAM" ) );

	for( ; pSrc < (uint8_t __far *)( __secend( "FSL_FECD" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDT�̃J�E���g���t���b�V��		*/
	}
#endif
	
	pSrc = (uint8_t __far *)( __sectop( "FSL_RCD" ) ); 							/* FSL_RCD��ROM����RAM�ɃR�s�[		*/
	pDst = (uint8_t __far *)( __sectop( "FSL_RCD_RAM" ) );
	
	for( ; pSrc < (uint8_t __far *)( __secend( "FSL_RCD" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDT�̃J�E���g���t���b�V��		*/
	}
	
	pSrc = (uint8_t __far *)( __sectop( "FSL_BCD" ) ); 							/* FSL_BCD��ROM����RAM�ɃR�s�[		*/
	pDst = (uint8_t __far *)( __sectop( "FSL_BCD_RAM" ) );
	
	for( ; pSrc < (uint8_t __far *)( __secend( "FSL_BCD" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDT�̃J�E���g���t���b�V��		*/
	}
	
	pSrc = (uint8_t __far *)( __sectop( "FSL_BECD" ) ); 						/* FSL_BECD��ROM����RAM�ɃR�s�[		*/
	pDst = (uint8_t __far *)( __sectop( "FSL_BECD_RAM" ) );
	
	for( ; pSrc < (uint8_t __far *)( __secend( "FSL_BECD" ) ); pSrc++, pDst++ )
	{
		*pDst = *pSrc;
		WDTE = 0xACU;															/* WDT�̃J�E���g���t���b�V��		*/
	}
	
	SubFirmUpdate();															/* ROM���������s					*/
}
#pragma section

/* **** Section **** */
#pragma section text FIRMUP_APP
/*
 *******************************************************************************
 *	ROM������������
 *
 *	[���e]
 *	���ۂ�ROM�������������{����B
 *	������A�r���Ń��Z�b�g��d���f�����邱�Ƃ�z�肵�Ă��Ȃ��B 
 *******************************************************************************
 */
static __far void SubFirmUpdate( void )
{
	fsl_u08		wkResult = FSL_OK;
	uint8_t		wkRetryCnt = 0U;
	ET_FirmUpSts_t	wkFirmUpSts = ecFirmUpSts_Init;
	fsl_u16		wkBlock = 0U;
	uint8_t			wkRet = FLASH_SUCCESS;
	
	while( 1 )
	{
		WDTE = 0xACU;													/* WDT�̃J�E���g���t���b�V�� */
		if( wkResult != FSL_OK )
		{
			wkRetryCnt++;
			
			if( wkRetryCnt > 2U )
			{
				/* �A��3�񎸔s�ŋ������Z�b�g */
				WDTE = 0xFFU;											/* �������Z�b�g */
			}
			else
			{
				wkBlock = 0U;
				wkFirmUpSts = ecFirmUpSts_Erase;						/* �ŏ��̃u���b�N�����������蒼�� */
			}
		}
		
		switch( wkFirmUpSts )
		{
			/* ������ */
			case ecFirmUpSts_Init :
				wkResult = SubFSLStart();								/* FSL���C�u�����g�p���� */
				if( wkResult == FSL_OK )
				{
					wkFirmUpSts = ecFirmUpSts_Erase;
					wkBlock = 0U;
					wkRetryCnt = 0U;
				}
				break;
			/* ���� */
			case ecFirmUpSts_Erase :
				wkResult = FSL_Erase( wkBlock );
				if( wkResult == FSL_OK )
				{														/* ����I�� */
					wkBlock++;											/* ���u���b�N�ɍX�V */
					if( wkBlock > imFirmUp_BlkWrMaxNum )
					{
						wkBlock = 0U;									/* �u�����N�`�F�b�N�p�ɏ����� */
						wkRetryCnt = 0U;								/* ���g���C�񐔏����� */
						/* �ŏI�u���b�N�܂Ő���ɏ������I��������u�����N�`�F�b�N�Ɉڍs */
						wkFirmUpSts = ecFirmUpSts_BlankCheck;
					}
				}
				else
				{														/* �ُ�I�� */
					;
				}
				break;													/* break(���g���C����) */
			/* �u�����N�`�F�b�N */
			case ecFirmUpSts_BlankCheck :
				wkResult = FSL_BlankCheck( wkBlock );
				if( wkResult == FSL_OK )
				{														/* ����I�� */
					wkBlock++;											/* ���u���b�N�ɍX�V */
					if( wkBlock > imFirmUp_BlkWrMaxNum )
					{
						wkBlock = 0U;									/* �u�����N�`�F�b�N�p�ɏ����� */
						wkRetryCnt = 0U;								/* ���g���C�񐔏����� */
						/* �ŏI�u���b�N�܂Ő���Ƀu�����N����Ă����珑�����݂Ɉڍs */
						wkFirmUpSts = ecFirmUpSts_Write;
					}
				}
				else
				{														/* �ُ�I�� */
					;
				}
				break;
			/* ������ */
			case ecFirmUpSts_Write :
				wkRet = SubR_Fl_PrgTrgtArea();
				if( wkRet == FLASH_COMPLETE )
				{														/* ����I�� */
					/* ����I���Ȃ�I�������Ɉڍs */
					wkFirmUpSts = ecFirmUpSts_Close;
					wkRetryCnt = 0U;
				}
				else
				{
					wkResult = FSL_ERR_WRITE;							/* �������ُ݈� */
				}
				break;
			/* �I������ */
			case ecFirmUpSts_Close :
				FSL_Close();
				wkFirmUpSts = ecFirmUpSts_End;
				break;
				
			/* �I�� */
			case ecFirmUpSts_End :
				WDTE = 0xFFU;					/* �������Z�b�g */
				while( 1U );
				
			default :
				break;
		}
	}
}


/*
 *******************************************************************************
 *	FSL�g�p��������
 *
 *	[���e]
 *	FSL���C�u�����̎g�p�������s���B 
 *******************************************************************************
 */
static __far fsl_u08 SubFSLStart( void )
{
#if (swKouteiMode == imDisable)
	/* �t���X�s�[�h���[�h�A����N���b�N 24MHz�A�X�e�[�^�X�`�F�b�N�C���^�[�i�����[�h�ݒ� */
	const __far fsl_descriptor_t cFSL_Descriptor_pStr = { 0x00, 0x18, 0x01 };
	fsl_u08 wkResult;
	
	WDTE = 0xACU;																/* WDT�̃J�E���g���t���b�V�� */
	/* �t���b�V���E�Z���t�E�v���O���~���O�E���C�u�������������s */
	wkResult = FSL_Init( &cFSL_Descriptor_pStr );
	
	/* ������������ɏI�������ꍇ */
	if( wkResult == FSL_OK )
	{
		/* �t���b�V���E�Z���t�E�v���O���~���O�E���C�u�����J�n���� */
		FSL_Open();
		FSL_PrepareFunctions();
#if 0
		FSL_PrepareExtFunctions();
#endif
	}
	
	return wkResult;
#endif
}


/*
 *******************************************************************************
 *	�f�[�^�ǂݏo��
 *
 *	[���e]
 *		�w�肵���A�h���X�̃f�[�^���v���C�}������������ǂݏo���܂��B
 *	[����]
 *		uint32_t arAddress�F�A�h���X
 *		uint8_t arRdData�F�f�[�^�i�[��
 *		uint16_t arNum�F�f�[�^��
 *	[���ӎ���]
 *		��x�ɓǂݏo����f�[�^��256Byte�ł��B
 *******************************************************************************
 */
static __far ET_Error_t SubFirmUpMain_SubFlash_ReadData( uint32_t arAddress, uint8_t *parRdData, uint16_t arNum )
{
#if (swKouteiMode == imDisable)
	ET_Error_t			wkError;												/* �G���[��� */
	uint8_t				wkLoop;													/* ���[�v�J�E���^ */
	uint8_t				wkReg1Data;												/* ���W�X�^�f�[�^ */
	uint8_t				wkRcvData;												/* ��M�f�[�^ */
	uint16_t			wkNum;													/* �ǂݏo���f�[�^�� */
	uint32_t			wkAddr;													/* �A�h���X */
	uint32_t			wkTestBit;												/* �r�b�g�e�X�g�ʒu */
	uint32_t			cBitTable;
	
	/*
	 ***************************************
	 *	const��`�ɂ���ƃR���p�C���ɂ��
	 *	memcpy�ɕϊ�����Ă��܂����߁A
	 *	�蓮�Œl���i�[���Ă���B
	 ***************************************
	 */
	cBitTable  = M_BIT23;
	
	/* �X�e�[�^�X���W�X�^1�ǂݏo�� */
	
	/* �`�b�v�Z���N�g��Low */
	P_CS_PRIM = (uint8_t)imLow;													/* �`�b�v�Z���N�g(�v���C�}��)��Low */
	
	M_NOP;																		/* 1�X�e�[�g(=1/(24*10^6)=41ns) */
	M_NOP;																		/* 1�X�e�[�g(=1/(24*10^6)=41ns) */
	
	/* �R�}���h���M */
	/* �V���A���N���b�N��Low */
	P_SCLK = imLow;																/* SCLK(�V���A���N���b�N)��Low */
	
	/* RDSR1�R�}���h���M */
	SubFSLCmdSnd( imFlashDrv_RDSR1 );
	
	/* �X�e�[�^�X���W�X�^��M */
	wkReg1Data = SubFSLCmdRcv();
	
	M_NOP;																		/* 1�X�e�[�g(=1/(24*10^6)=41ns) */
	M_NOP;																		/* 1�X�e�[�g(=1/(24*10^6)=41ns) */
	
	/* �`�b�v�Z���N�g��High */
	P_CS_PRIM = imHigh;															/* �`�b�v�Z���N�g(�v���C�}��)��High */
	
	
	if ( ( M_TSTBIT( wkReg1Data, imFlashDrv_Sts1Bit_BUSY ) )					/* BUSY */
	||	 ( parRdData == M_NULL )												/* NULL */
	||	 ( arAddress > imFirmUp_RomMaxAddr )									/* �A�h���X�w��NG */
	||	 ( ( arAddress + arNum ) > ( imFirmUp_RomMaxAddr + 1U ) )				/* �f�[�^���w��NG */
	||	 ( arNum > imFirmUp_MaxReadNum )										/* �f�[�^���w��NG */
	)
	{
		wkError = ecERR_NG;														/* �G���[���NG */
	}
	else
	{
		/* �ϐ������� */
		wkError = ecERR_OK;														/* �G���[��񏉊��� */
		
		/* �`�b�v�Z���N�g��Low */
		P_CS_PRIM = imLow;														/* �`�b�v�Z���N�g(�v���C�}��)��Low */
		
		M_NOP;																	/* 1�X�e�[�g(=1/(24*10^6)=41ns) */
		M_NOP;																	/* 1�X�e�[�g(=1/(24*10^6)=41ns) */
		
		/* �R�}���h���M */
		/* �V���A���N���b�N��Low */
		P_SCLK = imLow;															/* SCLK(�V���A���N���b�N)��Low */
		
		/* READ�R�}���h���M */
		SubFSLCmdSnd( imFlashDrv_RDDT );
		/* EEPROM�A�h���X�̑��M */
		wkAddr = arAddress;														/* EEPROM�A�h���X */
		for( wkLoop = 0U; wkLoop < 24U; wkLoop++ )
		{
			/*
			 ***************************************
			 *	2byte�𒴂���V�t�g���Z�̓����^�C��
			 *	���C�u�����Ăяo���ƂȂ��Ă��܂����߁A
			 *	���Z���Ȃ��悤�Œ�l����Ƃ��Ă���B
			 ***************************************
			 */
			if( wkLoop == 0U )
			{
				wkTestBit = cBitTable;
			}
			else
			{
				wkTestBit /= 2U;
			}
			
			if( (wkAddr & wkTestBit) == 0U )
			{
				P_SDO = imLow;													/* �V���A���ʐM�f�[�^�o�� */
			}
			else
			{
				P_SDO = imHigh;													/* �V���A���ʐM�f�[�^�o�� */
			}
			
			P_SCLK = imHigh;													/* SCLK(�V���A���N���b�N)��High */
			
			M_NOP;																/* 1�X�e�[�g(=1/(24*10^6)=41ns) */
			
			P_SCLK = imLow;														/* SCLK(�V���A���N���b�N)��Low */
		}
		
		/* �f�[�^��M */
		for( wkNum = 0U; wkNum < arNum; wkNum++ )
		{
			wkRcvData = SubFSLCmdRcv();
			*parRdData = wkRcvData;												/* EEPROM�f�[�^�i�[ */
		}
		
		M_NOP;																	/* 1�X�e�[�g(=1/(24*10^6)=41ns) */
		M_NOP;																	/* 1�X�e�[�g(=1/(24*10^6)=41ns) */
		
		/* �`�b�v�Z���N�g��High */
		P_CS_PRIM = imHigh;														/* �`�b�v�Z���N�g(�v���C�}��)��High */
	}

	return wkError;
#endif
}


/*
 *******************************************************************************
 *	Flash�ւ̃f�[�^���M
 *
 *	[���e]
 *		Flash�փf�[�^�𑗐M����B
 *	[����]
 *		uint8_t arSndData�F���M�f�[�^
 *	[���ӎ���]
 *		���M�ł���f�[�^�T�C�Y��1Byte
 *******************************************************************************
 */
static __far void SubFSLCmdSnd( uint8_t arSndData )
{
#if (swKouteiMode == imDisable)
	uint8_t				wkLoop;												/* ���[�v�J�E���^ */
	uint8_t				wkTestBit;											/* �r�b�g�e�X�g�ʒu */
	
	for( wkLoop = 0U; wkLoop < 8U; wkLoop++ )
	{
		wkTestBit = M_BIT7 >> wkLoop;										/* �r�b�g�e�X�g�ʒu */
		if( (arSndData & wkTestBit) == 0U )
		{
			P_SDO = imLow;													/* �V���A���ʐM�f�[�^�o�� */
		}
		else
		{
			P_SDO = imHigh;													/* �V���A���ʐM�f�[�^�o�� */
		}
		
		P_SCLK = imHigh;													/* SCLK(�V���A���N���b�N)��High */
		
		M_NOP;																/* 1�X�e�[�g(=1/(24*10^6)=41ns) */
		
		P_SCLK = imLow;														/* SCLK(�V���A���N���b�N)��Low */
	}
#endif
}


/*
 *******************************************************************************
 *	Flash����f�[�^���[�h
 *
 *	[���e]
 *		Flash����f�[�^�����[�h����B
 *	[����]
 *		uint8_t wkRcvData�F���[�h�f�[�^
 *	[���ӎ���]
 *		���[�h����f�[�^�T�C�Y��1Byte
 *******************************************************************************
 */
static __far uint8_t SubFSLCmdRcv( void )
{
#if (swKouteiMode == imDisable)
	uint8_t				wkLoop;												/* ���[�v�J�E���^ */
	uint8_t				wkRcvData;											/* ��M�f�[�^ */
	
	wkRcvData = 0U;
	for( wkLoop = 0U; wkLoop < 8U; wkLoop++ )
	{
		wkRcvData <<= 1U;
		wkRcvData |= (uint8_t)P_SDI;										/* EEPROM�̃V���A���ʐM�f�[�^���� */
		P_SCLK = imHigh;													/* SCLK(�V���A���N���b�N)��High */
		
		M_NOP;																/* 1�X�e�[�g(=1/(24*10^6)=41ns) */
		
		P_SCLK = imLow;														/* SCLK(�V���A���N���b�N)��Low */
	}
	return wkRcvData;
#endif
}

/******************************************************************************
* [Ӽޭ�ٖ�] : R_Fl_PrgTrgtArea
*	[���{��] : ROM�G���A�ւ̃v���O������������
*	  [�T�v] : �V
*	  [�@�\] : �V
*	  [����] : �Ȃ�
*	  [�o��] : FLASH_COMPLETE�AFLASH_FAILURE
*	  [���l] : �Ȃ�
*	  [���] : Flash_main();
*	  [����] :	SubFirmUpMain_SubFlash_ReadData();				Flash IC����f�[�^���[�h
*				SubR_Fl_Prg_StoreHEX();
*				SubR_Fl_Prg_ProcessForHEX_data();
*				SubR_Fl_Prg_MakeWriteData();
*				SubR_Fl_Prg_WriteData();
******************************************************************************/
static __far uint8_t SubR_Fl_PrgTrgtArea( void )
{
#if (swKouteiMode == imDisable)
	uint8_t		tmp_ret;
	uint8_t		tmp_data;
	sint16_t	ret = FLASH_OK;

	/* **** Clear all data for HEX format **** */
	SubR_Fl_Prg_ClearHEXVariables();
	
	while( 1 )
	{
		if( 0 == fl_hex_data_flag )
		{
			/* **** If there is no HEX format data. **** */
			while( 1 )
			{
				if( (flash_addr > 0xFFFFF) || (ret != FLASH_OK) ){
					tmp_ret = FLASH_FAILURE;									/* Flash�A�h���X�I�[�o�[�G���[	*/
					break;
				}
				ret = SubFirmUpMain_SubFlash_ReadData( flash_addr, &tmp_data, 1u );
				flash_addr++;

				tmp_ret = SubR_Fl_Prg_StoreHEX( tmp_data );

				/* SubR_Fl_Prg_StoreHEX����̕Ԃ�l��FLASH_BUSY����FLASH_SUCCESS�܂�Loop���� */
				if( FLASH_SUCCESS == tmp_ret )
				{
					/* ==== If complete 1 format, process for HEX format ==== */
					tmp_ret = SubR_Fl_Prg_ProcessForHEX_data();
					/* After analyze, break loop */
					break;
				}
				WDTE = 0xACU;													/* WDT�̃J�E���g���t���b�V�� */
			}
		}
		else
		{
		/* **** Prepare the write data **** */
			tmp_ret = SubR_Fl_Prg_MakeWriteData();
			if( FLASH_SUCCESS == tmp_ret )
			{
				/* ==== If complete preparation, write data to flash ==== */
				SubR_Fl_Prg_WriteData();
			}
		}

		if( (tmp_ret == FLASH_COMPLETE ) || (tmp_ret == FLASH_FAILURE) )
		{
			break;
		}
	}

	return tmp_ret;
#endif
}


/******************************************************************************
* [Ӽޭ�ٖ�] : SubR_Fl_Prg_StoreHEX
*	[���{��] : HEX�t�H�[�}�b�g�f�[�^���i�[
*	  [�T�v] : �V
*	  [�@�\] : �V
*	  [����] : HEX�f�[�^
*	  [�o��] : FLASH_SUCCESS�AFLASH_FAILURE�AFLASH_BUSY
*	  [���l] : �Ȃ�
*	  [���] : SubR_Fl_PrgTrgtArea();
*	  [����] : SubR_Fl_AsciiToHexByte();
******************************************************************************/
static __far uint8_t SubR_Fl_Prg_StoreHEX( uint8_t hex_data )
{
#if (swKouteiMode == imDisable)
	uint8_t ret_code = FLASH_BUSY;
	
	if( fl_hex_total_data_cnt == 0 )
	{
		/* **** Header field (1byte) **** */
		if(':' == hex_data)
		{
			/* ==== If data is ':', store the data. ==== */
			fl_wr_hex.type[ 0 ] = hex_data;

			/* Increment counter */
			fl_hex_total_data_cnt++;
		}
		else
		{
			/* ==== If type field is not ':', ignore and clear data ==== */
				ret_code = FLASH_FAILURE;
		}
	}
	else if( (fl_hex_total_data_cnt == 1) || (fl_hex_total_data_cnt ==2) )
	{
		/* **** Length field (2byte) **** */

		/* Store the length */
		fl_wr_hex.len[ fl_hex_total_data_cnt - 1 ] = hex_data;

		/* Increment counter */
		fl_hex_total_data_cnt++;

		if( fl_hex_total_data_cnt == 3 ){
			/* Convert the length data from ascii code to binary */
			fl_hex_length = SubR_Fl_AsciiToHexByte( fl_wr_hex.len[ 0 ], fl_wr_hex.len[ 1 ] );
		}
	}
	else if(	(fl_hex_total_data_cnt == 3) || (fl_hex_total_data_cnt == 4) ||
				(fl_hex_total_data_cnt == 5) || (fl_hex_total_data_cnt == 6) )
	{
		/* **** Address (4byte) **** */

		/* Store the data */
		fl_wr_hex.addr_data_sum[ fl_hex_total_data_cnt - 3 ] = hex_data;

		/* Increment counter */
		fl_hex_total_data_cnt++;
	}
	else if( (fl_hex_total_data_cnt == 7) || (fl_hex_total_data_cnt == 8) )
	{
		/* **** ���R�[�h�^�C�v(2byte) **** */
		fl_wr_hex.type[ 1 ] = hex_data; 										/*����1byte���̗p	*/

		/* Store the data */
		fl_wr_hex.addr_data_sum[ fl_hex_total_data_cnt - 3 ] = hex_data;

		/* ==== Increment counter ==== */
		fl_hex_total_data_cnt++;
	}
	else{
		/* **** Data or Sum **** */

		/* Store the data */
		fl_wr_hex.addr_data_sum[ fl_hex_total_data_cnt - 3 ] = hex_data;

		/* Increment counter */
		fl_hex_total_data_cnt++;

		/* Received all data? */
		if( fl_hex_total_data_cnt - 9 >= (fl_hex_length * 2 + FL_HEX_SUM_SIZE * 2) )
		{
			/* Clear counter */
			fl_hex_total_data_cnt = 0;
			/* When received all data, return OK */
			ret_code = FLASH_SUCCESS;
		}
	}

	return ret_code;
#endif
}


/******************************************************************************
* [Ӽޭ�ٖ�] : SubR_Fl_Prg_ClearHEXVariables
*	[���{��] : ���g���[��S�ɂ�����ϐ��̃N���A
*	  [�T�v] : �V
*	  [�@�\] : �V
*	  [����] : �Ȃ�
*	  [�o��] : �Ȃ�
*	  [���l] : �Ȃ�
*	  [���] : SubR_Fl_Prg_WriteData();
*	  [����] : �Ȃ�
******************************************************************************/
static __far void SubR_Fl_Prg_ClearHEXVariables( void )
{
#if (swKouteiMode == imDisable)
	uint16_t	wkLoop;

	fl_hex_data_flag = 0;
	flash_addr = 0xC0000;
	fl_hex_total_data_cnt = 0;
	fl_hex_length = 0;
	sAdr_upper = 0U;
	vSurplusCnt = 0U;
	
	fl_wr_hex_bin_surplus.len = 0U;
	fl_wr_hex_bin_surplus.addr = 0U;
	fl_writing_data.addr = 0;
	fl_wr_hex_bin.len = 0U;
	fl_wr_hex_bin.addr = 0U;
	for( wkLoop = 0U; wkLoop < 260U; wkLoop++ )
	{
		fl_wr_hex_bin_surplus.data[ wkLoop ] = 0xFFU;
		fl_writing_data.data[ wkLoop ] = 0xFFU;
		fl_wr_hex_bin.data[ wkLoop ] = 0xFFU;
	}
	
	fl_wr_hex.type[0] = 0;
	fl_wr_hex.type[1] = 0;
	fl_wr_hex.len[0] = 0;
	fl_wr_hex.len[1] = 0;
	for( wkLoop = 0U; wkLoop < 520U; wkLoop++ )
	{
		fl_wr_hex.addr_data_sum[ wkLoop ] = 0xFFU;
	}
#endif
}


/******************************************************************************
* [Ӽޭ�ٖ�] : SubR_Fl_AsciiToHexByte
*	[���{��] : ASCII�f�[�^��HEX�f�[�^�֕ϊ�
*	  [�T�v] : �V
*	  [�@�\] : �V
*	  [����] :	in_upper		ASCII�R�[�h���
*				in_lower		ASCII�R�[�h����
*	  [�o��] : �o�C�i���f�[�^
*	  [���l] : �Ȃ�
*	  [���] :	SubR_Fl_Prg_StoreHEX();
*				SubR_Fl_Prg_HEX_AsciiToBinary();
*	  [����] : �Ȃ�
******************************************************************************/
static __far uint8_t SubR_Fl_AsciiToHexByte( uint8_t in_upper, uint8_t in_lower )
{
#if (swKouteiMode == imDisable)
	uint8_t in_buff8;

	/* **** Convert upper 4 bits **** */
	if( in_upper <= 0x39 )
	{
		/* The data is 0 to 9 */
		in_upper = ( in_upper & 0x0F );
	}
	else
	{
		/* The data is A to F */
		in_upper = ( in_upper - 0x37 );
	}

	/* **** Convert lower 4 bits **** */
	if( in_lower <= 0x39 )
	{
		/* The data is 0 to 9 */
		in_lower = ( in_lower & 0x0F );
	}
	else
	{
		/* The data is A to F */
		in_lower = ( in_lower - 0x37 );
	}

	/* **** Combine upper and lower **** */
	in_buff8 = (in_upper << 4) | in_lower;

	return in_buff8;
#endif
}


/******************************************************************************
* [Ӽޭ�ٖ�] : SubR_Fl_Prg_ProcessForHEX_data
*	[���{��] : HEX�w�b�_�̉�͂ƃo�C�i���f�[�^�ւ̕ϊ�
*	  [�T�v] : �V
*	  [�@�\] : �V
*	  [����] : �Ȃ�
*	  [�o��] : FLASH_SUCCESS�AFLASH_FAILURE�AFLASH_COMPLETE
*	  [���l] : �Ȃ�
*	  [���] : SubR_Fl_PrgTrgtArea();
*	  [����] :	SubR_Fl_Prg_HEX_AsciiToBinary();
*				SubR_Fl_Prg_ClearHEXVariables();
*				SubR_Fl_Prg_WriteData();
******************************************************************************/
static __far uint8_t SubR_Fl_Prg_ProcessForHEX_data( void )
{
#if (swKouteiMode == imDisable)
	uint8_t tmp_ret = FLASH_SUCCESS;
	uint8_t wk[ 4 ],i;

	switch( fl_wr_hex.type[ 1 ] )
	{
		case '4':
			/* **** 04:�g�����j�A�A�h���X���R�[�h **** */
			for( i=0;i<4;i++ )
			{
				wk[ i ] = fl_wr_hex.addr_data_sum[ i + 6 ];
			}
			sAdr_upper = SubR_Fl_AsciiToHexByte( wk[ 0 ], wk[ 1 ] );
			sAdr_upper <<= 8;
			sAdr_upper |= SubR_Fl_AsciiToHexByte( wk[ 2 ], wk[ 3 ] );
			sAdr_upper <<= 16;
		break;

		case '2':
			/* **** 02:�Z�O�����g�A�h���X���R�[�h **** */
			for( i=0;i<4;i++ )
			{
				wk[ i ] = fl_wr_hex.addr_data_sum[ i + 6 ];
			}
			sAdr_upper = SubR_Fl_AsciiToHexByte( wk[ 0 ], wk[ 1 ] );
			sAdr_upper <<= 8;
			sAdr_upper |= SubR_Fl_AsciiToHexByte( wk[ 2 ], wk[ 3 ] );
			sAdr_upper <<= 4;
		break;

		case '0':
			/* **** 00:�f�[�^���R�[�h **** */
			/* Convert all HEX data from Ascii to Binary */
			tmp_ret = SubR_Fl_Prg_HEX_AsciiToBinary( &fl_wr_hex, &fl_wr_hex_bin );
			if( FLASH_SUCCESS == tmp_ret )
			{
				/* If data complete, set HEX format flag */
				fl_hex_data_flag = 1;
			}
		break;

		case '5':
			/* **** 05:�X�^�[�g���j�A�A�h���X **** */
			break;

		case '1':
			/* **** 01:�G���h���R�[�h **** */
			tmp_ret = FLASH_COMPLETE;

			SubR_Fl_Prg_WriteData();

			/* Clear all data for Mot S format */
			SubR_Fl_Prg_ClearHEXVariables();
		break;

		default:
			/* **** If receive others **** */
			tmp_ret = FLASH_FAILURE;			/* �����f�[�^		*/

			/* Clear all data for Mot S format */
			SubR_Fl_Prg_ClearHEXVariables();
		break;
	}

	return tmp_ret;
#endif
}


/******************************************************************************
* [Ӽޭ�ٖ�] : SubR_Fl_Prg_HEX_AsciiToBinary
*	[���{��] : ASCII�R�[�h���o�C�i���f�[�^�ɕϊ�
*	  [�T�v] : �V
*	  [�@�\] : �V
*	  [����] :	Fl_prg_hex_t *tmp_hex : ASCII�f�[�^
*				Fl_prg_hex_binary_t *tmp_hex_binary : �o�C�i���f�[�^
*	  [�o��] : FLASH_SUCCESS�AFLASH_FAILURE
*	  [���l] : �Ȃ�
*	  [���] : SubR_Fl_Prg_ProcessForHEX_data();
*	  [����] : SubR_Fl_AsciiToHexByte();
*				memcpy();
******************************************************************************/
static __far uint8_t SubR_Fl_Prg_HEX_AsciiToBinary(
						Fl_prg_hex_t *tmp_hex,
						Fl_prg_hex_binary_t *tmp_hex_binary )
{
#if (swKouteiMode == imDisable)
	uint8_t 		ret_code = FLASH_FAILURE;
	uint32_t		cnt;
	uint16_t		tmp_len;
	uint8_t 		tmp_sum;
	uint8_t 		fl_wr_hex_bin_tmp[ 256 + FL_HEX_ADDR_SIZE + FL_HEX_SUM_SIZE ];
	uint16_t		wkLoop;

	/* **** Store length **** */
	tmp_len = SubR_Fl_AsciiToHexByte( tmp_hex->len[ 0 ], tmp_hex->len[ 1 ] );
	tmp_sum = tmp_len;

	/* **** Store all data **** */
	for( cnt = 0; cnt < (tmp_len + FL_HEX_ADDR_SIZE + FL_HEX_TYP_SIZE + FL_HEX_SUM_SIZE); cnt++ )
	{
		fl_wr_hex_bin_tmp[ cnt ] = SubR_Fl_AsciiToHexByte(
							tmp_hex->addr_data_sum[ cnt * 2 ],
							tmp_hex->addr_data_sum[ cnt * 2 + 1 ] );
		tmp_sum += fl_wr_hex_bin_tmp[ cnt ];
	}

	/* **** Check sum **** */
	if( 0x00 == tmp_sum )
	{
		/* ==== Copy length ==== */
		tmp_hex_binary->len = tmp_len;

		/* ==== Copy address ==== */
		tmp_hex_binary->addr = 0;
		for(cnt = 0; cnt < FL_HEX_ADDR_SIZE; cnt++)
		{
			/* Shift data */
			tmp_hex_binary->addr = tmp_hex_binary->addr << 8;
			/* Read data */
			tmp_hex_binary->addr |= fl_wr_hex_bin_tmp[ cnt ];
		}
		tmp_hex_binary->addr |= sAdr_upper;

		/* ==== Copy data ==== */
		for( wkLoop = 0U; wkLoop < tmp_hex_binary->len; wkLoop++ )
		{
			tmp_hex_binary->data[ wkLoop ] = fl_wr_hex_bin_tmp[ FL_HEX_ADDR_SIZE + FL_HEX_TYP_SIZE + wkLoop ];
		}
		ret_code = FLASH_SUCCESS;
	}
	else
	{
		/* ==== Clear all data for Mot S format ==== */
		SubR_Fl_Prg_ClearHEXVariables();
	}

	return ret_code;
#endif
}


/******************************************************************************
* [Ӽޭ�ٖ�] : SubR_Fl_Prg_WriteData
*	[���{��] : ROM�G���A�Ƀf�[�^�����C�g����
*	  [�T�v] : �V
*	  [�@�\] : �V
*	  [����] : �Ȃ�
*	  [�o��] : FLASH_SUCCESS�AFLASH_FAILURE
*	  [���l] : �Ȃ�
*	  [���] :	SubR_Fl_PrgTrgtArea();
*				SubR_Fl_Prg_ProcessForHEX_data();
*	  [����] :	SubR_FlashWrite();
*				SubR_Fl_Prg_ClearHEXVariables();
*				memset();
*				memcpy();
******************************************************************************/
static __far uint8_t SubR_Fl_Prg_WriteData( void )
{
#if (swKouteiMode == imDisable)
	uint8_t 	ret_code = FLASH_SUCCESS;
	uint8_t 	ret_tmp;
	uint16_t	cnt;
	uint32_t	tmp_addr_cnt;
	fsl_write_t	wkstFSLwrite;													/* ���C�u���������p�\���� */
	uint16_t	wkLoop;

	wkstFSLwrite.fsl_data_buffer_p_u08 = (fsl_u08 *)&fl_writing_data.data[ 0 ];	/* �����ރf�[�^�̐擪�A�h���X���w�� */
	wkstFSLwrite.fsl_destination_address_u32 = (fsl_u32)fl_writing_data.addr;	/* �����ݐ�̐擪�A�h���X */
	wkstFSLwrite.fsl_word_count_u08 = 64U;										/* �����ރf�[�^��(64���[�h = 256�o�C�g) */

	/* **** Write data to Flash **** */
	ret_tmp = FSL_Write( &wkstFSLwrite );										/* ROM������ */

	/* ==== Check for errors ==== */
	if( 0 != ret_tmp )
	{
		/* Clear all data for HEX format */
		SubR_Fl_Prg_ClearHEXVariables();
		ret_code = FLASH_FAILURE;
		cnt = 0;
	}
	else
	{
		/* **** If no error, read back written area **** */
		for ( cnt = 0; cnt < 0x0100; cnt++ )
		{
			/* Verify */
			if ( *wkstFSLwrite.fsl_data_buffer_p_u08 != fl_writing_data.data[ cnt ] )
			{
				/* Clear all data for HEX format */
				SubR_Fl_Prg_ClearHEXVariables();
				ret_code = FLASH_FAILURE;
				break;
			}
			wkstFSLwrite.fsl_data_buffer_p_u08++;
		}

		/* ==== Clear base address ==== */
		fl_writing_data.addr = 0;
		
		/* ==== Clear writing data ==== */
		for( wkLoop = 0U; wkLoop < 260U; wkLoop++ )
		{
			fl_writing_data.data[ wkLoop ] = 0xFFU;
		}

		if( 0 != fl_wr_hex_bin_surplus.addr )
		{
			/* **** If there is surplus data, set the data to write buffer */

			/* ==== Set base address ==== */
			fl_writing_data.addr = fl_wr_hex_bin_surplus.addr & 0xFFFFFF00;

			/* ==== Copy data ==== */
			tmp_addr_cnt = fl_wr_hex_bin_surplus.addr - fl_writing_data.addr;
			
			for( wkLoop = 0U; wkLoop < fl_wr_hex_bin_surplus.len; wkLoop++ )
			{
				if( tmp_addr_cnt + wkLoop > 255U )
				{
					/* �]��(surplus.data)���c���Ă���ꍇ�A�������ݎ��s */
					fl_wr_hex_bin_surplus.len = fl_wr_hex_bin_surplus.len - wkLoop + 1U;
					vSurplusCnt = wkLoop;
					ret_code = SubR_Fl_Prg_SurplusWriteData();
					break;
				}
				else
				{
					fl_writing_data.data[ tmp_addr_cnt + wkLoop ] = fl_wr_hex_bin_surplus.data[ wkLoop ];
				}
			}
			
			/* ==== Clear HEX format complete flag and buffer ==== */
			fl_hex_data_flag = 0;
			fl_wr_hex_bin_surplus.addr = 0;
		}
	}

	return ret_code;
#endif
}


/******************************************************************************
* [Ӽޭ�ٖ�] : SubR_Fl_Prg_MakeWriteData
*	[���{��] : ROM�G���A�Ƀ��C�g����f�[�^����
*	  [�T�v] : �V
*	  [�@�\] : �V
*	  [����] : �Ȃ�
*	  [�o��] : FLASH_SUCCESS�AFLASH_BUSY
*	  [���l] : �Ȃ�
*	  [���] : SubR_Fl_PrgTrgtArea();
*	  [����] : memcpy();
******************************************************************************/
static __far uint8_t SubR_Fl_Prg_MakeWriteData( void )
{
#if (swKouteiMode == imDisable)
	uint8_t 		ret_code = FLASH_SUCCESS;
	uint32_t		tmp_addr_cnt;
	uint32_t		read_cnt;
	uint32_t		cnt;
	uint16_t		wkLoop;

	/* **** If there is not surplus data, set next data */
	if( 0 == fl_writing_data.addr )
	{
		/* ==== If there is no base address, set base address ==== */
		fl_writing_data.addr = fl_wr_hex_bin.addr & 0xFFFFFF00;
	}

	tmp_addr_cnt = fl_wr_hex_bin.addr - fl_writing_data.addr;
	if( tmp_addr_cnt + fl_wr_hex_bin.len > 0x00000100 )
	{
		/* ==== If receive data more than blank size ==== */

		/* Copy data until write buffer is full */
		for( cnt = 0; tmp_addr_cnt < 0x00000100; cnt++ )
		{
			fl_writing_data.data[ tmp_addr_cnt++ ] = fl_wr_hex_bin.data[ cnt ];
		}
		read_cnt = cnt;

		/* ==== Store the surplus data ==== */
		fl_wr_hex_bin_surplus.addr = fl_wr_hex_bin.addr + read_cnt;
		fl_wr_hex_bin_surplus.len = fl_wr_hex_bin.len - read_cnt;

		for( wkLoop = 0U; wkLoop < fl_wr_hex_bin_surplus.len; wkLoop++ )
		{
			fl_wr_hex_bin_surplus.data[ wkLoop ] = fl_wr_hex_bin.data[ read_cnt + wkLoop ];
		}
	}
	else
	{
		/* ==== If receive data less than blank size ==== */
		/* Store all data */
		for( wkLoop = 0U; wkLoop < fl_wr_hex_bin.len; wkLoop++ )
		{
			fl_writing_data.data[ tmp_addr_cnt + wkLoop ] = fl_wr_hex_bin.data[ wkLoop ];
		}

		/* Clear HEX format complete flag */
		fl_hex_data_flag = 0;
	}

	if ( tmp_addr_cnt < 0x0100 )
	{
		/* **** If writing data buffer is not full, return NG **** */
		ret_code = FLASH_BUSY;
	}

	return ret_code;
#endif
}


/******************************************************************************
* [Ӽޭ�ٖ�] : SubR_Fl_Prg_SurplusWriteData
*	[���{��] : ROM�G���A�ɗ]��f�[�^�����C�g����
*	  [�T�v] : �]��f�[�^��surplus.data�Ɏc���Ă���ꍇ�̂ݖ{�֐������s
*			   surplus.data������ɂ���ׁA�O�t���t���b�V���ǂݏo���O��
*			   �A�����ď������ށB
*	  [�@�\] : �V
*	  [����] : �Ȃ�
*	  [�o��] : FLASH_SUCCESS�AFLASH_FAILURE
*	  [���l] : �Ȃ�
*	  [���] :	SubR_Fl_Prg_WriteData();
******************************************************************************/
static uint8_t SubR_Fl_Prg_SurplusWriteData( void )
{
#if (swKouteiMode == imDisable)
	uint8_t 	ret_code = FLASH_SUCCESS;
	uint8_t 	ret_tmp;
	uint16_t	cnt;
	fsl_write_t	wkstFSLwrite;													/* ���C�u���������p�\���� */
	uint16_t	wkLoop;

	wkstFSLwrite.fsl_data_buffer_p_u08 = (fsl_u08 *)&fl_writing_data.data[ 0 ];	/* �����ރf�[�^�̐擪�A�h���X���w�� */
	wkstFSLwrite.fsl_destination_address_u32 = (fsl_u32)fl_writing_data.addr;	/* �����ݐ�̐擪�A�h���X */
	wkstFSLwrite.fsl_word_count_u08 = 64U;										/* �����ރf�[�^��(64���[�h = 256�o�C�g) */

	/* **** Write data to Flash **** */
	ret_tmp = FSL_Write( &wkstFSLwrite );										/* ROM������ */
	/* ==== Check for errors ==== */
	if( 0 != ret_tmp )
	{
		/* Clear all data for HEX format */
		SubR_Fl_Prg_ClearHEXVariables();
		ret_code = FLASH_FAILURE;
		cnt = 0;
	}
	else
	{
		/* **** If no error, read back written area **** */
		for ( cnt = 0; cnt < 0x0100; cnt++ )
		{
			/* Verify */
			if ( *wkstFSLwrite.fsl_data_buffer_p_u08 != fl_writing_data.data[ cnt ] )
			{
				/* Clear all data for HEX format */
				SubR_Fl_Prg_ClearHEXVariables();
				ret_code = FLASH_FAILURE;
				break;
			}
			wkstFSLwrite.fsl_data_buffer_p_u08++;
		}

		/* ==== Clear base address ==== */
		fl_writing_data.addr = 0;
		/* ==== Clear writing data ==== */
		for( wkLoop = 0U; wkLoop < 260U; wkLoop++ )
		{
			fl_writing_data.data[ wkLoop ] = 0xFFU;
		}

		if( 0 != fl_wr_hex_bin_surplus.addr )
		{
			/* **** If there is surplus data, set the data to write buffer */

			/* ==== Set base address ==== */
			fl_writing_data.addr = fl_wr_hex_bin_surplus.addr & 0xFFFFFF00;
			fl_writing_data.addr += 0x00000100U;								/* ����̃x�[�X�A�h���X�ɍX�V */

			for( wkLoop = 0U; wkLoop < fl_wr_hex_bin_surplus.len; wkLoop++ )
			{
				fl_writing_data.data[ wkLoop ] = fl_wr_hex_bin_surplus.data[ vSurplusCnt + wkLoop ];
			}
			/* ==== Clear HEX format complete flag and buffer ==== */
			fl_hex_data_flag = 0;
			fl_wr_hex_bin_surplus.addr = 0;
			vSurplusCnt = 0U;
		}
	}
	return ret_code;
#endif
}

#pragma section 