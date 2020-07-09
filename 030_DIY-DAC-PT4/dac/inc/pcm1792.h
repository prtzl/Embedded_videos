#ifndef PCM1792_H
#define PCM1792_H

#include "stm32f4xx.h"

/** Device register address map **/
#define PCM1792_BASE 16 // base address of PCM1792 registers (16)
#define PCM1792_REG16_ADDR (PCM1792_BASE)
#define PCM1792_REG17_ADDR (PCM1792_BASE + 1)
#define PCM1792_REG18_ADDR (PCM1792_BASE + 2)
#define PCM1792_REG19_ADDR (PCM1792_BASE + 3)
#define PCM1792_REG20_ADDR (PCM1792_BASE + 4)
#define PCM1792_REG21_ADDR (PCM1792_BASE + 5)
#define PCM1792_REG22_ADDR (PCM1792_BASE + 6)
#define PCM1792_REG23_ADDR (PCM1792_BASE + 7)

/** Status of device **/
typedef enum
{
	OFF,
	ON
} Status_t;

typedef enum
{
	PCMOK,
	ErrorPcmSpi,
	ErrorPcm
} PCMError;

/** Device handle struct **/
typedef struct
{
	Status_t status; // device operation status
	Status_t connection; // device connection status
	uint8_t REG16; // ATL register, offset = 0x00
	uint8_t REG17; // ATR register, offset = 0x01
	uint8_t REG18; // ATLD, FMT, DMF, DME, MUTE register, offset = 0x02
	uint8_t REG19; // REV, ATS, OPE, RSV, DFMS, FLT, INZD register, offset = 0x03
	uint8_t REG20; // SRST, DSD, DFTH, MONO, CHSL, OS register, offset = 0x04
	uint8_t REG21; // DZ, PCMZ register, offset = 0x05
	uint8_t REG22; // ZFGR register, offset = 0x06
	uint8_t REG23; // ID register, offset = 0x07
}PCM1792_TypeDef;


/** Structure for holding PCM1792 init configuration **/
typedef struct
{
	uint8_t ATL;			 				/** Left channel attenuation level ( default 0dB = 0xff)
																Values from 0 to 0xff**/

	uint8_t ATR;							/** Left channel attenuation level ( default 0dB = 0xff)
																Values from 0 to 0xff**/

	uint8_t ATLD;							/** Attenuation level enable ( default disabled = 0 )
																Values from PCM1792_ATLD_Msk **/

	uint8_t FMT;							/** Audio data format ( default 24-bit I2S format = 0x05)
																Values of PCM1792_FMT_Msk**/

	uint8_t DMF;							/** De-Emphasis Sampling Frequency Selection ( default Disabled = 0)
																Values of PCM1792_DMF_Msk **/

	uint8_t DME;							/** De-Emphasis enable ( default disabled = 0)
	 	 	 	 	 	 	 	 	 	 	 	 	 			Values of PCM1792_DME_Msk **/

	uint8_t MUTE;							/** Mute control ( default disabled = 0)
																Values of PCM1792_Mute_Msk **/

	uint8_t REV;							/** Output Phase Reversal (default Normal output = 0)
																Values of PCM1792_Rev_Msk **/

	uint8_t ATS;							/** Attenuation Rate Select ( default LRCK/1 = 0)
																Values of PCM1792_ATS_Msk **/

	uint8_t OPE;							/** Dac operation control ( default enabled = 0)
																Values of PCM1792_OPE_Msk **/

	uint8_t DFMS;							/** Stereo DF Bypass Mode Select ( default Monaural = 0)
																Values of PCM1792_DFMS_Msk **/

	uint8_t FLT;							/** Digital Filter Rolloff Controll (default Sharp = 0)
																Values of PCM1792_FLT_Msk **/

	uint8_t INZD;							/** Infinite Zero Detect Mute ( default disabled = 0)
																Values of PCM1792_INZD_Msk **/

	uint8_t SRST;							/** System Reset Control ( default Normal = 0)
																Values of PCM1792_SRST_Msk **/

	uint8_t DSD;							/** DSD mode ( default disabled = 0)
	 	 	 	 	 	 	 	 	 	 	 	 	 			Values of PCM1792_DSD_Msk **/

	uint8_t DFTH;							/** Digital Filter Bypass (or Through Mode) Control ( default enabled = 0)
																Values of PCM1792_DFTH_Msk **/

	uint8_t MONO;							/** Monaural Mode ( default Stereo = 0)
																Values of PCM1792_MONO_Msk **/

	uint8_t CHSL;							/** Channel Selection for Monaural Mode ( default Left = 0)
																Values of PCM1792_CHSL_Msk **/

	uint8_t OS;								/** Delta-Sigma Oversampling Rate Selection ( default 64*fs = 0)
																Values of PCM1792_OS_Msk **/

	uint8_t DZ;								/** DSD Zero Output Flag Enable ( default disabled = 0)
																Values of PCM1792_DZ_Msk **/

	uint8_t PCMZ;							/** PCM Zero Output Flag Enable ( default disabled = 0)
																Values of PCM1792_PCMZ_Msk **/

	uint8_t ZFGx;							/** Zero-Detection Flags - READ ONLY
														**/

	uint8_t ID;								/** Device ID - READ ONLY
														**/

}PCM1792_InitTypeDef;


/** Idea of library function

..._Pos : LSB position of settings
..._Msk : (setting_size_mask << Pos) -> mask (all 1 in all places of setting size on place starting at Pos)

Example:
8b register: A A A B B C D D
bit no:			 7 6 5 4 3 2 1 0

settings A:
Pos = (5) : starts at bit 5
Msk = (0x7 << Pos) : mask for 3 bits is 7 or 111 + it starts at bit Pos => (111 <<< 5bits) => mask = 1 1 1 0 0 0 0 0

Setting:
Number of setting from 0 to setting_size : A A A -> from 0 to 111 starting at Pos
Example settings for setting A:
Setting 0: (0x1 << Pos) : 0 0 1 0 0 0 0 0
		.
		.
		.
Setting 5: (0x5 << Pos) : 1 0 1 0 0 0 0 0

Write these settings like |= set in order to not fuck up other bits in register


When "setting" 0th setting, so all bits for seting are 0 -> A A A = 0 0 0 is actualy ~_Msk
When applying this bit setting in order to not fuck up other bits in register write it as &= ~_Msk
OR use first item which is defined as ~_Msk and write it like &= Set0

**/


/** Setting macros **/

/**
 * REG18
 **/
/* Attenuation control */
#define PCM1792_ATLD_Pos												(7U)
#define PCM1792_ATLD_Msk												(0x01U << PCM1792_ATLD_Pos) 	/* 0x80 */
#define PCM1792_ATLD_Dis												(0x00)												/* Disable loading of attenuation values (default) */
#define PCM1792_ATLD_En													(0x01 << PCM1792_ATLD_Pos)		/* Enable loading of attenuation values */

/* Audio data format */
#define PCM1792_FMT_Pos													(4)
#define PCM1792_FMT_Msk													(0x07 << PCM1792_FMT_Pos)			/* 0x70 */
#define PCM1792_FMT_Std_16b_Right								(0x00)												/* 16-bit standard, right-justified format data */
#define PCM1792_FMT_Std_20b_Right								(0x1 << PCM1792_FMT_Pos)			/* 20-bit standard, right-justified format data */
#define PCM1792_FMT_Std_24b_Right								(0x2 << PCM1792_FMT_Pos)			/* 24-bit standard, right-justified format data */
#define PCM1792_FMT_MSB_24b_Left								(0x3 << PCM1792_FMT_Pos)			/* 24-bit MSB-first, left-justified format data */
#define PCM1792_FMT_I2S_16b											(0x4 << PCM1792_FMT_Pos)			/* 16-bit I2S format data */
#define PCM1792_FMT_I2S_24b											(0x5 << PCM1792_FMT_Pos)			/* 24-bit I2S format data (default) */

/* De-emphesys filter frequency */
#define PCM1792_DMF_Pos													(2)
#define PCM1792_DMF_Msk													(0x02 << PCM1792_DMF_Pos)			/* 0x0C */
#define PCM1792_DMF_Dis													(0x00)												/* Disabled (default) */
#define PCM1792_DMF_48kHz												(0x1 << PCM1792_DMF_Pos) 			/* 48kHz */
#define PCM1792_DMF_44k1Hz											(0x2 << PCM1792_DMF_Pos)			/* 44.1kHz */
#define PCM1792_DMF_32kHz												(0x3 << PCM1792_DMF_Pos)			/* 32kHz */

/* De-emphesys control */
#define PCM1792_DME_Pos													(1)
#define PCM1792_DME_Msk													(0x01 << PCM1792_DME_Pos)			/* 0x02 */
#define PCM1792_DME_Dis													(0x00)						 						/* De-emphasis disabled (default) */
#define PCM1792_DME_En													(0x01 << PCM1792_DME_Msk) 		/* De-emphasis enabled */

/* Mute control */
#define PCM1792_Mute_Pos												(0)
#define PCM1792_Mute_Msk												(0x01 << PCM1792_Mute_Pos)		/* 0x01 */
#define PCM1792_Mute_Dis												(0x00)												/* MUTE disabled (default) */
#define PCM1792_Mute_En													(0x01 << PCM1792_Mute_Pos)	 	/* MUTE enabled */

/**
 * REG19
**/
/* Output phase */
#define PCM1792_REV_Pos													(7)
#define PCM1792_Rev_Msk													(0x01 << PCM1792_REV_Pos)			/* 0x80 */
#define PCM1792_REV_Norm												(0x00)												/* Normal output (default) */
#define PCM1792_REV_Inv													(0x01 << PCM1792_REV_Pos)			/* Inverted output */

/* Attenuation rate sellect */
#define PCM1792_ATS_Pos													(5)
#define PCM1792_ATS_Msk													(0x03 << PCM1792_ATS_Pos)			/* 0x60 */
#define PCM1792_ATS_1														(0x00)												/* LRCK /1 (default) */
#define PCM1792_ATS_2														(0x01 << PCM1792_ATS_Pos) 		/* LRCK /2 */
#define PCM1792_ATS_4														(0x02 << PCM1792_ATS_Pos) 		/* LRCK /4 */
#define PCM1792_ATS_8														(0x03 << PCM1792_ATS_Pos) 		/* LRCK /8 */

/* DAC control */
#define PCM1792_OPE_Pos													(4)
#define PCM1792_OPE_Msk													(0x01 << PCM1792_OPE_Pos)			/* 0x10 */
#define PCM1792_OPE_En													(0x00)												/* DAC operation enabled (default) */
#define PCM1792_OPE_Dis													(0x01 << PCM1792_OPE_Pos) 		/* DAC operation disabled */

/* Stereo DF bypass control */
#define PCM1792_DFMS_Pos												(2)
#define PCM1792_DFMS_Msk												(0x01 << PCM1792_DFMS_Pos)		/* 0x04 */
#define PCM1792_DFMS_En													(0x00)												/* Monaural (default) */
#define PCM1792_DFMS_Dis												(0x01 << PCM1792_DFMS_Pos)		/* Stereo input enabled */

/* Digital filter rolloff */
#define PCM1792_FLT_Pos													(1)
#define PCM1792_FLT_Msk													(0x01 << PCM1792_FLT_Pos)			/* 0x02 */
#define PCM1792_FLT_Sharp												(0x00)												/* Sharp rolloff (default) */
#define PCM1792_FLT_Slow												(0x01 << PCM1792_FLT_Pos) 		/* Slow rolloff */

/* Infinite zero mute control */
#define PCM1792_INZD_Pos												(0)
#define PCM1792_INZD_Msk												(0x01 << PCM1792_INZD_Pos)		/* 0x01 */
#define PCM1792_INZD_Dis												(0x00)												/* Infinite zero detect mute disabled (default) */
#define PCM1792_INZD_En													(0x01 << PCM1792_INZD_Pos) 		/* Infinite zero detect mute enabled */

/**
 * REG20
**/
/* DAC reset control */
#define PCM1792_SRST_Pos												(6)
#define PCM1792_SRST_Msk												(0x01 << PCM1792_SRST_Pos)		/* 0x40 */
#define PCM1792_SRST_Norm												(0x00)												/* Normal operation (default) */
#define PCM1792_SRST_Rst												(0x01 << PCM1792_SRST_Pos)		/* System reset operation */

/* DSD mode control */
#define PCM1792_DSD_Pos													(5)
#define PCM1792_DSD_Msk													(0x01 << PCM1792_DSD_Pos)			/* 0x20 */
#define PCM1792_DSD_Dis													(0x00)												/* DSD interface mode disabled (default) */
#define PCM1792_DSD_En													(0x01 << PCM1792_DSD_Pos) 		/* DSD interface mode enabled */

/* Digital filter bypass control */
#define PCM1792_DFTH_Pos												(4)
#define PCM1792_DFTH_Msk												(0x01 << PCM1792_DFTH_Pos)		/* 0x10 */
#define PCM1792_DFTH_Int												(0x00)												/* Digital filter enabled (default) */
#define PCM1792_DFTH_Ext												(0x01 << PCM1792_DFTH_Pos) 		/* Digital filter bypassed for external */

/* Monaural mode control */
#define PCM1792_MONO_Pos												(3)
#define PCM1792_MONO_Msk												(0x01 << PCM1792_MONO_Pos)		/* 0x08 */
#define PCM1792_MONO_Ster												(0x00)							 					/* Stereo mode (default) */
#define PCM1792_MONO_Mon												(0x01 << PCM1792_MONO_Pos) 		/* Monaural mode */

/* Monaural mode channel */
#define PCM1792_CHSL_Pos												(2)
#define PCM1792_CHSL_Msk												(0x01 << PCM1792_CHSL_Pos)		/* 0x04 */
#define PCM1792_CHSL_L													(0x00)							 					/* L-channel selected (default) */
#define PCM1792_CHSL_R													(0x01 << PCM1792_CHSL_Pos) 		/* R-channel selected */

/* Delta-Sigma Oversampling Rate  */
#define PCM1792_OS_Pos													(0)
#define PCM1792_OS_Msk													(0x03 << PCM1792_OS_Pos)			/* 0x03 */
#define PCM1792_OS_64														(0x00)												/* 64 times fs (default) */
#define PCM1792_OS_32														(0x01 << PCM1792_OS_Pos) 			/* 32 times fs */
#define PCM1792_OS_128													(0x02 << PCM1792_OS_Pos) 			/* 128 times fs */

/**
 * REG21
 **/
/* DSD Zero Output */
#define PCM1792_DZ_Pos													(1)
#define PCM1792_DZ_Msk													(0x03 << PCM1792_DZ_Pos)			/* 0x0e */
#define PCM1792_DZ_Dis													(0x00)												/* Disabled (default) */
#define PCM1792_DZ_1														(0x01 << PCM1792_DZ_Pos) 			/* Even pattern detect */
#define PCM1792_DZ_2														(0x02 << PCM1792_DZ_Pos) 			/* 96H pattern detect */

/* PCM Zero Output */
#define PCM1792_PCMZ_Pos												(0)
#define PCM1792_PCMZ_Msk												(0x01 << PCM1792_PCMZ_Pos)		/* 0x01 */
#define PCM1792_PCMZ_Dis												(0x00)												/* PCM zero output disabled (default) */
#define PCM1792_PCMZ_En													(0x01 << PCM1792_PCMZ_Pos) 		/* PCM zero output enabled */

/**********************************************************************/
/** I/O protoypes **/
ErrorStatus pcm1792_writeReg(uint8_t regAddr, uint8_t data);
ErrorStatus pcm1792_readReg(uint8_t regAddr, uint8_t *pData);

/**
 * Configuration functions.
 * Writes PCM1792 setting to PCM1792_TypeDef
 * @param write if 1, also write to device register
**/

/* Reg 16, REG 17 && malo Reg 18 */

__STATIC_INLINE ErrorStatus pcm1792_AT_config(PCM1792_TypeDef *PCM1792, uint8_t ATL, uint8_t ATR, uint8_t atld, uint8_t write)
{
	WRITE_REG(PCM1792->REG16, ATL); // set new L-attenuation
	WRITE_REG(PCM1792->REG17, ATR); // set new R-attenuation
	MODIFY_REG(PCM1792->REG18, PCM1792_ATLD_Msk, atld); // set attenuation bit

	// Write settings ?
	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG16_ADDR, PCM1792->REG16);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
		ret = pcm1792_writeReg(PCM1792_REG17_ADDR, PCM1792->REG17);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
		ret = pcm1792_writeReg(PCM1792_REG18_ADDR, PCM1792->REG18);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

/* Reg 18 */

__STATIC_INLINE ErrorStatus pcm1792_FMT_conf(PCM1792_TypeDef *PCM1792, uint8_t FMT, uint8_t write)
{
	MODIFY_REG( PCM1792->REG18, PCM1792_FMT_Msk, FMT); // set audio data format

	// Write settings ?
	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG18_ADDR, PCM1792->REG18);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_DMF_conf(PCM1792_TypeDef *PCM1792, uint8_t DMF, uint8_t DME, uint8_t write)
{
	MODIFY_REG( PCM1792->REG18, PCM1792_DMF_Msk, DMF );
	MODIFY_REG( PCM1792->REG18, PCM1792_DME_Msk, DME );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG18_ADDR, PCM1792->REG18);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_MUTE_conf(PCM1792_TypeDef *PCM1792, uint8_t MUTE, uint8_t write)
{
	MODIFY_REG( PCM1792->REG18, PCM1792_Mute_Msk, MUTE );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG18_ADDR, PCM1792->REG18);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

/* Reg 19 */
__STATIC_INLINE ErrorStatus pcm1792_REV_conf(PCM1792_TypeDef *PCM1792, uint8_t REV, uint8_t write)
{
	MODIFY_REG( PCM1792->REG19, PCM1792_Rev_Msk, REV );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG19_ADDR, PCM1792->REG19);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_ATS_conf(PCM1792_TypeDef *PCM1792, uint8_t ATS, uint8_t write)
{
	MODIFY_REG( PCM1792->REG19, PCM1792_ATS_Msk, ATS );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG19_ADDR, PCM1792->REG19);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_OPE_conf(PCM1792_TypeDef *PCM1792, uint8_t OPE, uint8_t write)
{
	MODIFY_REG( PCM1792->REG19, PCM1792_OPE_Msk, OPE );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG19_ADDR, PCM1792->REG19);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_DFMS_conf(PCM1792_TypeDef *PCM1792, uint8_t DFMS, uint8_t write)
{
	MODIFY_REG( PCM1792->REG19, PCM1792_DFMS_Msk, DFMS );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG19_ADDR, PCM1792->REG19);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_FLT_conf(PCM1792_TypeDef *PCM1792, uint8_t FLT, uint8_t write)
{
	MODIFY_REG( PCM1792->REG19, PCM1792_FLT_Msk, FLT );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG19_ADDR, PCM1792->REG19);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_INZD_conf(PCM1792_TypeDef *PCM1792, uint8_t INZD, uint8_t write)
{
	MODIFY_REG( PCM1792->REG19, PCM1792_INZD_Msk, INZD );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG19_ADDR, PCM1792->REG19);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

/* Reg 20 */

__STATIC_INLINE ErrorStatus pcm1792_SRST_conf(PCM1792_TypeDef *PCM1792, uint8_t SRST, uint8_t write)
{
	MODIFY_REG( PCM1792->REG20, PCM1792_SRST_Msk, SRST );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG20_ADDR, PCM1792->REG20);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_DSD_conf(PCM1792_TypeDef *PCM1792, uint8_t DSD, uint8_t write)
{
	MODIFY_REG( PCM1792->REG20, PCM1792_DSD_Msk, DSD );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG20_ADDR, PCM1792->REG20);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_DFTH_conf(PCM1792_TypeDef *PCM1792, uint8_t DFTH, uint8_t write)
{
	MODIFY_REG( PCM1792->REG20, PCM1792_DFTH_Msk, DFTH );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG20_ADDR, PCM1792->REG20);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_MONO_conf(PCM1792_TypeDef *PCM1792, uint8_t MONO, uint8_t write)
{
	MODIFY_REG( PCM1792->REG20, PCM1792_MONO_Msk, MONO );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG20_ADDR, PCM1792->REG20);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_CHSL_conf(PCM1792_TypeDef *PCM1792, uint8_t CHSL, uint8_t write)
{
	MODIFY_REG( PCM1792->REG20, PCM1792_CHSL_Msk, CHSL );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG20_ADDR, PCM1792->REG20);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_OS_conf(PCM1792_TypeDef *PCM1792, uint8_t OS, uint8_t write)
{
	MODIFY_REG( PCM1792->REG20, PCM1792_OS_Msk, OS );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG20_ADDR, PCM1792->REG20);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

/* Reg 21 */

__STATIC_INLINE ErrorStatus pcm1792_DZ_conf(PCM1792_TypeDef *PCM1792, uint8_t DZ, uint8_t write)
{
	MODIFY_REG( PCM1792->REG21, PCM1792_DZ_Msk, DZ );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG21_ADDR, PCM1792->REG21);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

__STATIC_INLINE ErrorStatus pcm1792_PCMZ_conf(PCM1792_TypeDef *PCM1792, uint8_t PCMZ, uint8_t write)
{
	MODIFY_REG( PCM1792->REG21, PCM1792_PCMZ_Msk, PCMZ );

	if( write == 1 )
	{
		ErrorStatus ret;

		ret = pcm1792_writeReg(PCM1792_REG21_ADDR, PCM1792->REG21);
		if(ret != SUCCESS)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

/* Reg 22 */

__STATIC_INLINE ErrorStatus pcm1792_ZFGx_read(PCM1792_TypeDef *PCM1792)
{
	ErrorStatus ret;

	ret = pcm1792_readReg(PCM1792_REG22_ADDR, &(PCM1792->REG22) );
	if( ret != SUCCESS )
	{
		return ERROR;
	}

	return SUCCESS;
}

/* Reg 23 */

__STATIC_INLINE ErrorStatus pcm1792_ID_read(PCM1792_TypeDef *PCM1792)
{
	ErrorStatus ret;

	ret = pcm1792_readReg(PCM1792_REG23_ADDR, &(PCM1792->REG23));
	if( ret != SUCCESS )
	{
		return ERROR;
	}

	return SUCCESS;
}


/** Local system functions **/

__STATIC_INLINE ErrorStatus PCM1792_StructInit(PCM1792_TypeDef *DEV, PCM1792_InitTypeDef *INIT)
{

	/** Init REG16, REG17 and one bit of REG18  **/
	pcm1792_AT_config(DEV, INIT->ATL, INIT->ATR, INIT->ATLD, 0);

	/** Init REG 18 **/
	pcm1792_FMT_conf(DEV, INIT->FMT, 0);
	pcm1792_DMF_conf(DEV, INIT->DMF, INIT->DME, 0);
	pcm1792_MUTE_conf(DEV, INIT->MUTE, 0);

	/** Init REG19 **/
	pcm1792_REV_conf(DEV, INIT->REV, 0);
	pcm1792_ATS_conf(DEV, INIT->ATS, 0);
	pcm1792_OPE_conf(DEV, INIT->OPE, 0);
	pcm1792_DFMS_conf(DEV, INIT->DFMS, 0);
	pcm1792_FLT_conf(DEV, INIT->FLT, 0);
	pcm1792_INZD_conf(DEV, INIT->INZD, 0);

	/** Init REG20 **/
	pcm1792_SRST_conf(DEV, INIT->SRST, 0);
	pcm1792_DSD_conf(DEV, INIT->DSD, 0);
	pcm1792_DFTH_conf(DEV, INIT->DFTH, 0);
	pcm1792_MONO_conf(DEV, INIT->MONO, 0);
	pcm1792_CHSL_conf(DEV, INIT->CHSL, 0);
	pcm1792_OS_conf(DEV, INIT->OS, 0);

	/** Init REG21 **/
	pcm1792_DZ_conf(DEV, INIT->DZ, 0);
	pcm1792_PCMZ_conf(DEV, INIT->PCMZ, 0);

	/** Init REG22 **/

	/** Init REG23 **/

	/** Write new register settings to device **/

	if (pcm1792_writeReg(PCM1792_REG16_ADDR, DEV->REG16) != SUCCESS) return ERROR;
	if (pcm1792_writeReg(PCM1792_REG17_ADDR, DEV->REG17) != SUCCESS) return ERROR;
	if (pcm1792_writeReg(PCM1792_REG18_ADDR, DEV->REG18) != SUCCESS) return ERROR;
	if (pcm1792_writeReg(PCM1792_REG19_ADDR, DEV->REG19) != SUCCESS) return ERROR;
	if (pcm1792_writeReg(PCM1792_REG20_ADDR, DEV->REG20) != SUCCESS) return ERROR;
	if (pcm1792_writeReg(PCM1792_REG21_ADDR, DEV->REG21) != SUCCESS) return ERROR;

	return SUCCESS;
}

/**********************************************************************/
/** Function prototypes **/
void PCM1792_Start();
void PCM1792_Stop();

PCMError PCM1792_Init(PCM1792_TypeDef *DEV);

void PCM1792_MuteCtrl(PCM1792_TypeDef *DEV, uint8_t mute);
void pcm1792_FMTSelect(PCM1792_TypeDef *DEV, uint32_t fmt);
void pcm1792_REVSelect(PCM1792_TypeDef *DEV, uint32_t rev);
void pcm1792_OSSelect(PCM1792_TypeDef *DEV, uint32_t os);
void pcm1792_FLTSelect(PCM1792_TypeDef *DEV, uint32_t flt);
void pcm1792_DMFSelect(PCM1792_TypeDef *DEV, uint32_t dmf);

#endif
