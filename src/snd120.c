#include "snd120.h"

#define Z80_ADDRESS             0xA00000

#define SND120_COMMAND          0xA00080        // command from 68K
#define SND120_STATUS           0xA00081        // status from Z80
#define SND120_TIMER_COMMAND    0xA00082		// timer command from 68K
#define SND120_FADE_SPEED       0xA00083        // fade out speed

#define SND120_BGM_ADDR         0xA00090		// BGM data address of start point  address of BGM DATA (bit  0 --> bit 14)
#define SND120_BGM_BANK		    0xA00092		// BGM data bank    of start point  address of BGM DATA (bit 15 --> bit 22)
#define SND120_SE0_ADDR	        0xA00093		// SE0 data address of start point
#define SND120_SE0_BANK	        0xA00095		// SE0 data bank    of start point
#define SND120_SE1_ADDR	        0xA00096		// SE1 data address of start point
#define SND120_SE1_BANK	        0xA00098		// SE1 data bank    of start point
#define SND120_SE2_ADDR	        0xA00099		// SE2 data address of start point
#define SND120_SE2_BANK	        0xA0009B		// SE2 data bank    of start point

#define SND120_BGM_ADDR_L       0xA000A0		// BGM data address of loop point
#define SND120_BGM_BANK_L	    0xA000A2	    // BGM data bank    of loop point
#define SND120_SE0_ADDR_L       0xA000A3		// SE0 data address of loop point
#define SND120_SE0_BANK_L       0xA000A5		// SE0 data bank    of loop point
#define SND120_SE1_ADDR_L       0xA000A6		// SE1 data address of loop point
#define SND120_SE1_BANK_L       0xA000A8		// SE1 data bank    of loop point
#define SND120_SE2_ADDR_L       0xA000A9		// SE2 data address of loop point
#define SND120_SE2_BANK_L       0xA000AB		// SE2 data bank    of loop point

#define SND120_SE0_PRIO	        0xA0009C		// SE0 priority		0-255
#define SND120_SE1_PRIO         0xA0009D		// SE1 priority		0-255
#define SND120_SE2_PRIO         0xA0009E	    // SE2 priority		0-255


#define SND120_TIMER_A_VALUE    0xA0008D		// timer-A value 2 byte
#define SND120_TIMER_B_VALUE    0xA0008F		// timer-B value



static u16 SND120_freqTimerA = 19;
static u8  SND120_freqTimerB = 228;


u16  SND120_isPauseBGM()
{
    vu8 *pb;
    u16 flag;

    pb = (u8 *) SND120_STATUS;

    SYS_disableInts();

    Z80_requestBus(TRUE);

    flag = (*pb) & 0x20;             // read play status

    Z80_releaseBus();

    SYS_enableInts();

    if(flag) return TRUE;

    return FALSE;
}

u16  SND120_isPlayingBGM()
{
    vu8 *pb;
    u16 flag;

    pb = (u8 *) SND120_STATUS;

    SYS_disableInts();

    Z80_requestBus(TRUE);

    flag = (*pb) & 0x08;             // read play status

    Z80_releaseBus();

    SYS_enableInts();

    if(flag) return TRUE;

    return FALSE;

}

u16  SND120_isFadingBGM()
{
    vu8 *pb;
    u16 flag;

    pb = (u8 *) SND120_STATUS;

    SYS_disableInts();

    Z80_requestBus(TRUE);

    flag = (*pb) & 0x80;             // read play status

    Z80_releaseBus();

    SYS_enableInts();

    if(flag) return TRUE;

    return FALSE;

}

void SND120_loadZ80Driver()
{
    SYS_disableInts();

    Z80_clear(0,0x2000,TRUE);
    Z80_upload(0x0000, Z80_PROGRAM, sizeof(Z80_PROGRAM), TRUE);

    SYS_enableInts();

    SND120_setFreqTimerA(SND120_freqTimerA);
    SND120_setFreqTimerB(SND120_freqTimerB);
}

void SND120_getName(const u8 *data, char* str)
{
    u16 i;
    for(i=0;i<8;++i){
        str[i] = data[i];
    }
    str[8] = 0;
}

void SND120_startBGM(const u8 *data, u8 flag)
{

    vu8 *pb;

    u32 addr = (u32)data;
    u32 loopOffset = (u32)data[12] + ((u32)data[13] << 8) + ((u32)data[14] << 16);
    u32 loopAddress = addr + loopOffset;
    addr += 16;

    SYS_disableInts();

    Z80_requestBus(TRUE);

    pb = (vu8*)(SND120_BGM_ADDR);
    *pb       = (addr & 0xFF);
    *(pb + 1) = ((addr >>  8) & 0x7F) | 0x80;

    pb = (vu8*)(SND120_BGM_BANK);
    *pb = (addr >> 15) & 0x7F;

    pb = (vu8*)(SND120_BGM_ADDR_L);
    *pb       = (loopAddress & 0xFF);
    *(pb + 1) = ((loopAddress >>  8) & 0x7F) | 0x80;

    pb = (vu8*)(SND120_BGM_BANK_L);
    *pb = (loopAddress >> 15) & 0x7F;

    if(flag) {*(vu8*)SND120_COMMAND |= 0x10;}                     // Stop playback before starting BGM
    *(vu8*)SND120_COMMAND |= 0x08;

    *(vu8*)(SND120_TIMER_A_VALUE    ) = data[10];
    *(vu8*)(SND120_TIMER_A_VALUE + 1) = data[11];

    *(vu8*)SND120_TIMER_COMMAND |= 0x01;

    Z80_releaseBus();

    SYS_enableInts();
}

u16  SND120_getTimerAValueBGM(const u8 *data)
{
    return (u16)(data[10] * 4 + data[11]);
}

u16  SND120_startSE(const u8 *data, u8 ch, u8 priority)
{

    u32 addr = (u32)data;
    addr += 16;

    u16 flag = FALSE;

    u32 ADDR, BANK, PRIO;
    switch(ch){
    case 0: ADDR = SND120_SE0_ADDR; BANK = SND120_SE0_BANK; PRIO = SND120_SE0_PRIO; break;
    case 1: ADDR = SND120_SE1_ADDR; BANK = SND120_SE1_BANK; PRIO = SND120_SE1_PRIO;  break;
    case 2: ADDR = SND120_SE2_ADDR; BANK = SND120_SE2_BANK; PRIO = SND120_SE2_PRIO; }

    vu8 *pb = (vu8*)(PRIO);

    SYS_disableInts();

    Z80_requestBus(TRUE);

    if(*pb <= priority){
        *pb = priority;

        pb = (vu8*)(ADDR);
        *pb       = (addr & 0xFF);
        *(pb + 1) = ((addr >>  8) & 0x7F) | 0x80;

        pb = (vu8*)(BANK);
        *pb = (addr >> 15) & 0x7F;

        pb = (vu8*)(SND120_COMMAND);
        *pb |= (0x01 << ch);

        flag = TRUE;
    }

    Z80_releaseBus();



    SYS_enableInts();

    return flag;
}

void SND120_stopBGM()
{

    SYS_disableInts();

    Z80_requestBus(TRUE);

    *(vu8*)SND120_COMMAND |= 0x10;

    Z80_releaseBus();

    SYS_enableInts();
}

void SND120_pauseBGM()
{
    if(SND120_isPauseBGM()) return;

    SYS_disableInts();

    Z80_requestBus(TRUE);

    *(vu8*)SND120_COMMAND |= 0x20;

    Z80_releaseBus();

    SYS_enableInts();
}

void SND120_resumeBGM()
{
    SYS_disableInts();

    Z80_requestBus(TRUE);

    *(vu8*)SND120_COMMAND |= 0x40;

    Z80_releaseBus();

    SYS_enableInts();
}

void SND120_startFadeBGM(u8 speed)
{

    SYS_disableInts();

    Z80_requestBus(TRUE);

    *(vu8*)SND120_COMMAND |= 0x80;
    *(vu8*)SND120_FADE_SPEED = speed;

    Z80_releaseBus();

    SYS_enableInts();
}

void SND120_setFreqTimerA(u16 value)
{
    SYS_disableInts();

    Z80_requestBus(TRUE);

    *(vu8*)(SND120_TIMER_A_VALUE    ) = (u8)((value >> 2) & 0xFF);
    *(vu8*)(SND120_TIMER_A_VALUE + 1) = (u8)(value & 3);
    *(vu8*)SND120_TIMER_COMMAND |= 0x01;

    Z80_releaseBus();

    SYS_enableInts();
}

void SND120_setFreqTimerB(u8 value)
{
    SYS_disableInts();

    Z80_requestBus(TRUE);

    *(vu8*)(SND120_TIMER_B_VALUE) = value;
    *(vu8*)SND120_TIMER_COMMAND |= 0x02;

    Z80_releaseBus();

    SYS_enableInts();
}

u16 SND120_getFreqTimerA()
{
    return SND120_freqTimerA;
}

u8 SND120_getFreqTimerB()
{
    return SND120_freqTimerB;
}


