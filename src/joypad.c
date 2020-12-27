
/*------------------------------------------------------------------
    joypad
  ------------------------------------------------------------------*/

#include "joypad.h"

static u16 Pad_changed[2], Pad_state[2];

void    Joypad_init()
{
    Pad_changed[0] = 0;
    Pad_state[0] = 0;
    Pad_changed[1] = 0;
    Pad_state[1] = 0;
}

u16     Joypad_getPress(u16 joy, u16 key)
{
    if(Pad_state[joy] & key) return TRUE;
    return FALSE;
}

u16     Joypad_getHit(u16 joy, u16 key)
{
    if(Pad_state[joy] & Pad_changed[joy] & key) {Pad_changed[joy] &= ~key; return TRUE;}
    else return FALSE;
}

u16     Joypad_getPress1Por2P(u16 key)
{
    if     (Pad_state[JOY_1] & key) return TRUE;
    else if(Pad_state[JOY_2] & key) return TRUE;
    return FALSE;
}

u16     Joypad_getHit1Por2P(u16 key)
{
    if     (Pad_state[JOY_1] & Pad_changed[JOY_1] & key) {Pad_changed[JOY_1] &= ~key; return TRUE;}
    else if(Pad_state[JOY_2] & Pad_changed[JOY_2] & key) {Pad_changed[JOY_2] &= ~key; return TRUE;}
    else return FALSE;
}

void    Joypad_joyEvent(u16 joy, u16 changed, u16 state)
{
    Pad_changed[joy] = changed;
    Pad_state[joy] = state;
}

void    Joypad_setDisable()
{
    JOY_setEventHandler(NULL);
    Joypad_init();
}

void    Joypad_setEnable()
{
    JOY_init();
    JOY_setEventHandler(&Joypad_joyEvent);
    Joypad_init();
}

void    Joypad_setState(u16 joy, u16 state)
{
    Pad_state[joy] = state;
    Pad_changed[joy] = state;
}
