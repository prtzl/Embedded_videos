#include "pcm1792.h"
#include "mll_spi.h"
#include "timer.h"

/** PCM1792 I/O functions **/
/**
* @brief  Write data to PCM1792 register
* @param  regAddr address of register we want to read from
* @param  data input data to write into device register
**/
ErrorStatus pcm1792_writeReg(uint8_t regAddr, uint8_t data)
{
	ErrorStatus ret;

	LL_GPIO_ResetOutputPin(PCM1792_SPI_CS_PORT, PCM1792_SPI_CS_PIN);
	ret = mll_spi_write2Byte(PCM1792_SPI, regAddr, data);
	LL_GPIO_SetOutputPin(PCM1792_SPI_CS_PORT, PCM1792_SPI_CS_PIN);

	if(ret != SUCCESS)
	{
		return ERROR;
	}
	return SUCCESS;
}

/**
* @brief  Read data from PCM1792 register
* @param  regAddr address of register we want to read from
* @param  pData data pointer for device register contents
* @retval ErrorStatus
**/
ErrorStatus pcm1792_readReg(uint8_t regAddr, uint8_t *pData)
{
	ErrorStatus ret;

	LL_GPIO_ResetOutputPin(PCM1792_SPI_CS_PORT, PCM1792_SPI_CS_PIN);
	ret = mll_spi_readByte(PCM1792_SPI, regAddr | 0x80, pData);
	LL_GPIO_SetOutputPin(PCM1792_SPI_CS_PORT, PCM1792_SPI_CS_PIN);

	if(ret != SUCCESS)
	{
		return ERROR;
	}
	return SUCCESS;
}

/** Handle for PCM1792 configuration, state ... **/
PCM1792_TypeDef PCM1792;


/** PCM1792 device function **/
/**
* @brief  Start PCM1792 operation
* @param  DEV hadle for PCM1792
**/
void PCM1792_Start(PCM1792_TypeDef *DEV)
{
	/** Enable DAC operation and disable MUTE **/
	pcm1792_OPE_conf(DEV, PCM1792_OPE_En, 1);
	pcm1792_MUTE_conf(DEV, PCM1792_Mute_Dis, 1);

	DEV->status = ON;
}

/**
* @brief  Stop PCM1792 operation
* @param  DEV hadle for PCM1792
**/
void PCM1792_Stop(PCM1792_TypeDef *DEV)
{
	/** Enable MUTE and disable DAC operation **/
	pcm1792_MUTE_conf(DEV, PCM1792_Mute_En, 1);
	pcm1792_OPE_conf(DEV, PCM1792_OPE_Dis, 1);
	DEV->status = OFF;
}

/**
* @brief  Init PCM1792 with default values
* 				OP disabled, MUTE enabled
* @param  DEV hadle for PCM1792
**/
PCMError PCM1792_Init(PCM1792_TypeDef *DEV)
{
	ErrorStatus ret;

	/** Start PCM1792 connected SPI peripheral **/
	if( !LL_SPI_IsEnabled(PCM1792_SPI) )
	{
		LL_SPI_Enable(PCM1792_SPI);
	}

	/** Get ID of PCM1792 to check if alive **/
	ret = pcm1792_ID_read(DEV);
	if( ret != SUCCESS )
	{
		LL_SPI_Disable(PCM1792_SPI);
		return ErrorPcmSpi;
	}
	// Check if ID valid
	if( DEV->REG23 == 0 || DEV->REG23 == 0xff)
	{
		LL_SPI_Disable(PCM1792_SPI);
		return ErrorPcm;
	}
	DEV->connection = ON;

	/** Stop DAC before configuring **/
	PCM1792_Stop(DEV);

	// Init structure for default settings
	PCM1792_InitTypeDef PCM1792_InitStruct = {0};

	PCM1792_InitStruct.ATL = 0xff;
	PCM1792_InitStruct.ATR = 0xff;
	PCM1792_InitStruct.ATLD = PCM1792_ATLD_Dis;		/* Attenuation control disabled */
	PCM1792_InitStruct.FMT = PCM1792_FMT_I2S_24b;	/* 24-bit I2S audio format */
	PCM1792_InitStruct.DMF = PCM1792_DMF_Dis;			/* De-emphesys filter disabled */
	PCM1792_InitStruct.DME = PCM1792_DME_Dis;			/* De-emphesys control disabled */
	PCM1792_InitStruct.MUTE = PCM1792_Mute_En;		/* MUTE enabled */
	PCM1792_InitStruct.REV = PCM1792_REV_Norm;		/* Phase polarity normal */
	PCM1792_InitStruct.ATS = PCM1792_ATS_1;				/* Attenuation rate = LRCK */
	PCM1792_InitStruct.OPE = PCM1792_OPE_Dis;			/* DAC disabled */
	PCM1792_InitStruct.DFMS = PCM1792_DFMS_En;		/* Stereo DF bypass monoural */
	PCM1792_InitStruct.FLT = PCM1792_FLT_Sharp;		/* Sharp digital filter roll off */
	PCM1792_InitStruct.INZD = PCM1792_INZD_Dis;		/* Infinite zero mute disabled */
	PCM1792_InitStruct.SRST = PCM1792_SRST_Norm;	/* DAC operation Normal */
	PCM1792_InitStruct.DSD = PCM1792_DSD_Dis;			/* DSD mode disabled */
	PCM1792_InitStruct.DFTH = PCM1792_DFTH_Int;		/* Internal digital filters */
	PCM1792_InitStruct.MONO = PCM1792_MONO_Ster;	/* Stereo mode */
	PCM1792_InitStruct.CHSL = PCM1792_CHSL_L;			/* Mono mode Left */
	PCM1792_InitStruct.OS = PCM1792_OS_64;				/* Delta-Sigma Oversampling = 64*fs */
	PCM1792_InitStruct.DZ = PCM1792_DZ_Dis;				/* DSD Zero output flag disabled */
	PCM1792_InitStruct.PCMZ = PCM1792_PCMZ_Dis;		/* PCM Zero output flag disabled */
	if ( PCM1792_StructInit(DEV, &PCM1792_InitStruct) != SUCCESS )
	{
		return ErrorPcmSpi;
	}

	return PCMOK;
}

/**
* @brief  Mute control PCM1792
* @param  DEV hadle for PCM1792
* @param	mute controls mute, 1 for mute, 0 for un-mute
**/
void PCM1792_MuteCtrl(PCM1792_TypeDef *DEV, uint8_t mute)
{
	if( mute == 1 )
	{
		pcm1792_MUTE_conf(DEV, PCM1792_Mute_En, 1);
	}
	else
	{
		pcm1792_MUTE_conf(DEV, PCM1792_Mute_Dis, 1);
	}
}
