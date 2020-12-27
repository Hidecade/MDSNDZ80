/*------------------------------------------------------------------
   snd120.h
   120Hz YM2612 Sound Driver
  ------------------------------------------------------------------*/
#ifndef _SND120_H_
#define _SND120_H_


#include <genesis.h>
#include "sound.h"

void SND120_loadZ80Driver();
void SND120_startBGM(const u8 *data, u8 flag);
void SND120_stopBGM();
void SND120_pauseBGM();
void SND120_resumeBGM();
u16  SND120_isPauseBGM();
u16  SND120_isPlayingBGM();
u16  SND120_isFadingBGM();
u16  SND120_startSE(const u8 *data, u8 ch, u8 priority);
void SND120_startFadeBGM(u8 speed);
void SND120_getName(const u8 *data, char* str);
u16  SND120_getTimerAValueBGM(const u8 *data);
void SND120_setFreqTimerA(u16 value);
void SND120_setFreqTimerB(u8 value);
u16  SND120_getFreqTimerA();
u8   SND120_getFreqTimerB();

#endif
