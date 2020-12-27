/*------------------------------------------------------------------
   joypad.h
  ------------------------------------------------------------------*/
#ifndef _JOYPAD_H_
#define _JOYPAD_H_

#include <genesis.h>

void    Joypad_init();
void    Joypad_joyEvent(u16 joy, u16 changed, u16 state);
u16     Joypad_getPress(u16 joy, u16 key);
u16     Joypad_getHit(u16 joy, u16 key);
u16     Joypad_getPress1Por2P(u16 key);
u16     Joypad_getHit1Por2P(u16 key);
void    Joypad_setDisable();
void    Joypad_setEnable();
void    Joypad_setState(u16 joy, u16 state);

#endif
