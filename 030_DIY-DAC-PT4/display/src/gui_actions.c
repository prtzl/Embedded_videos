#include "gui_actions.h"
#include "pcm1792.h"

extern PCM1792_TypeDef PCM1792;

/** MUTE **/
void actionPCM_mute(void)
{
	PCM1792_MuteCtrl(&PCM1792, 1);
}

void actionPCM_unmute(void)
{
	PCM1792_MuteCtrl(&PCM1792, 0);
}

/** FMT **/
void actionFMT0(void)
{
	pcm1792_FMTSelect(&PCM1792, PCM1792_FMT_Std_16b_Right);
}

void actionFMT1(void)
{
	pcm1792_FMTSelect(&PCM1792, PCM1792_FMT_Std_20b_Right);
}

void actionFMT2(void)
{
	pcm1792_FMTSelect(&PCM1792, PCM1792_FMT_Std_24b_Right);
}

void actionFMT3(void)
{
	pcm1792_FMTSelect(&PCM1792, PCM1792_FMT_MSB_24b_Left);
}

void actionFMT4(void)
{
	pcm1792_FMTSelect(&PCM1792, PCM1792_FMT_I2S_16b);
}

void actionFMT5(void)
{
	pcm1792_FMTSelect(&PCM1792, PCM1792_FMT_I2S_24b);
}

/** REV **/
void actionREV_normal(void)
{
	pcm1792_REVSelect(&PCM1792, PCM1792_REV_Norm);
}

void actionREV_inv(void)
{
	pcm1792_REVSelect(&PCM1792, PCM1792_REV_Inv);
}

/** OS **/
void actionOS_32(void)
{
	pcm1792_OS_conf(&PCM1792, PCM1792_OS_32, 1);
}

void actionOS_64(void)
{
	pcm1792_OS_conf(&PCM1792, PCM1792_OS_64, 1);
}

void actionOS_128(void)
{
	pcm1792_OS_conf(&PCM1792, PCM1792_OS_128, 1);
}

/** FLT **/
void actionsFLT_sharp(void)
{
	pcm1792_FLTSelect(&PCM1792, PCM1792_FLT_Sharp);
}

void actionsFLT_slow(void)
{
	pcm1792_FLTSelect(&PCM1792, PCM1792_FLT_Slow);
}

/** DMF **/
void actionsDMF_disable(void)
{
	pcm1792_DMFSelect(&PCM1792, PCM1792_DMF_Dis);
}

void actionsDMF_32k(void)
{
	pcm1792_DMFSelect(&PCM1792, PCM1792_DMF_32kHz);
}

void actionsDMF_44k1(void)
{
	pcm1792_DMFSelect(&PCM1792, PCM1792_DMF_44k1Hz);
}

void actionsDMF_48k(void)
{
	pcm1792_DMFSelect(&PCM1792, PCM1792_DMF_48kHz);
}

