#ifndef GUI_ACTIONS_H
#define GUI_ACTIONS_H

#include "stm32f4xx.h"

void actionPCM_mute(void);
void actionPCM_unmute(void);

void actionFMT0(void);
void actionFMT1(void);
void actionFMT2(void);
void actionFMT3(void);
void actionFMT4(void);
void actionFMT5(void);

void actionREV_normal(void);
void actionREV_inv(void);

void actionOS_32(void);
void actionOS_64(void);
void actionOS_128(void);

void actionsFLT_sharp(void);
void actionsFLT_slow(void);

void actionsDMF_disable(void);
void actionsDMF_32k(void);
void actionsDMF_44k1(void);
void actionsDMF_48k(void);

#endif
