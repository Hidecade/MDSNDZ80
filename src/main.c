

#include <genesis.h>
#include "snd120.h"
#include "joypad.h"
#include "sound.h"

void Tile_drawHex(VDPPlane plane, u32 value, u16 x, u16 y, u16 minisize)
{
    char num[16];
    intToHex(value, num, minisize);
    VDP_drawTextBG(plane, num, x, y);
}

void Tile_drawNum(VDPPlane plane, u32 value, u16 x, u16 y, u16 minisize)
{
    char num[16];
    intToStr(value, num, minisize);
    VDP_drawTextBG(plane, num, x, y);
}

static const u8* BGM_DATA[22] =
                          {BGM_00,BGM_01,BGM_02,BGM_03,BGM_04,BGM_05,BGM_06,BGM_07,
                           BGM_08,BGM_09,BGM_10,BGM_11,BGM_12,BGM_13,BGM_14,BGM_15,
                           BGM_16,BGM_17,BGM_18,BGM_19,BGM_20,BGM_21};

static const u8* SE_DATA[28] =
                         {SE_00,SE_01,SE_02,SE_03,SE_04,SE_05,SE_06,SE_07,
                          SE_08,SE_09,SE_10,SE_11,SE_12,SE_13,SE_14,SE_15,
                          SE_16,SE_17,SE_18,SE_19,SE_20,SE_21,SE_22,SE_23,
                          SE_24,SE_25_0,SE_25_1,SE_25_2};

int main()
{

    VDP_drawText("<Z80 YM2612 DRIVER TEST>", 5, 3);

    Joypad_setEnable();

    SND120_loadZ80Driver();

    VDP_drawText("BGM   ", 5,  5);
    VDP_drawText("SE 1ch", 5,  7);
    VDP_drawText("SE 2ch", 5,  9);
    VDP_drawText("SE 3ch", 5, 11);
    VDP_drawText("FREQ A : 0570       Hz", 5, 13);
    VDP_drawText("FREQ B : 0228  00120Hz", 5, 15);
    VDP_drawText("A:start BGM B:stop BGM", 5, 18);
    VDP_drawText("C+B:fade out BGM", 5, 20);
    VDP_drawText("C+UP or DOWN:change Tempo", 5, 22);
    VDP_drawText("X:SE 1ch Y:SE 2ch Z:SE 3ch", 5, 24);

    u16 freqTimerA = SND120_getFreqTimerA();
    u8 flagDrawFreqTimerA = TRUE;
    u8 flagDrawCursor     = TRUE;
    u8 flagDrawName[4]    = {TRUE, TRUE, TRUE, TRUE};
    u8 flagFade = FALSE;
    u8 flagPlay = FALSE;
    u8 flagPause = FALSE;
    u16 counter;
    u16 selectedList  = 0;
    u16 selectedItem[4] = {0};
    u16 indexLength[4] = {22,28,28,28};

    while(1)
    {

        // fade
        if(flagFade && !flagPause){
            switch(counter % 64){
            case  0: VDP_drawText("fade  ", 30, 5); break;
            case 32: VDP_drawText("      ", 30, 5);}
            if(!SND120_isFadingBGM() && counter >= 2){
                flagFade = FALSE;
                flagPlay = FALSE;
                VDP_drawText("stop  ", 30, 5);
            }
        }
        else if(flagPlay && !flagPause){
            switch(counter % 64){
            case  0: VDP_drawText("play  ", 30, 5); break;
            case 32: VDP_drawText("      ", 30, 5);}
            if(!SND120_isPlayingBGM() && counter >= 2){
                flagFade = FALSE;
                flagPlay = FALSE;
                VDP_drawText("stop  ", 30, 5);
            }
        }
        ++counter;

        if(Joypad_getPress(JOY_1, BUTTON_C)){
            if(Joypad_getPress(JOY_1, BUTTON_DOWN)){if(freqTimerA > 0    ) --freqTimerA; SND120_setFreqTimerA(freqTimerA); flagDrawFreqTimerA = TRUE;}
            if(Joypad_getPress(JOY_1, BUTTON_UP  )){if(freqTimerA < 0x3FF) ++freqTimerA; SND120_setFreqTimerA(freqTimerA); flagDrawFreqTimerA = TRUE;}
            if(Joypad_getHit(JOY_1, BUTTON_B) && SND120_isPlayingBGM() && !SND120_isPauseBGM()) {
                SND120_startFadeBGM(10);
                VDP_drawText("fade  ", 30, 5);
                flagFade = TRUE;
                counter = 0;
            }
        }
        else{

            if(Joypad_getHit(JOY_1, BUTTON_DOWN )){if(selectedList == 3) selectedList = 0;  else ++selectedList; flagDrawCursor = TRUE;}
            if(Joypad_getHit(JOY_1, BUTTON_UP   )){if(selectedList == 0) selectedList = 2;  else --selectedList; flagDrawCursor = TRUE;}
            if(Joypad_getHit(JOY_1, BUTTON_RIGHT)){
                if(selectedItem[selectedList] == indexLength[selectedList] - 1) selectedItem[selectedList] = 0;
                else ++selectedItem[selectedList];
                flagDrawName[selectedList] = TRUE;
            }
            if(Joypad_getHit(JOY_1, BUTTON_LEFT )){
                if(selectedItem[selectedList] == 0) selectedItem[selectedList] = indexLength[selectedList] - 1;
                else --selectedItem[selectedList];
                flagDrawName[selectedList] = TRUE;
            }
        }

        // start BGM
        if(Joypad_getHit(JOY_1, BUTTON_A)){
            SND120_startBGM(BGM_DATA[selectedItem[0]], TRUE);
            freqTimerA = SND120_getTimerAValueBGM(BGM_DATA[selectedItem[0]]);
            flagDrawName[0] = TRUE;
            flagDrawFreqTimerA = TRUE;
            flagPlay = TRUE;
            flagFade = FALSE;
            flagPause = FALSE;
            counter = 0;
        }

        // stop BGM
        if(Joypad_getHit(JOY_1, BUTTON_B) && !Joypad_getPress(JOY_1, BUTTON_C)) {
            SND120_stopBGM();
            VDP_drawText("stop  ", 30, 5);
            flagPlay = FALSE;
            flagFade = FALSE;
            flagPause = FALSE;
        }

        // start SE
        if(Joypad_getHit(JOY_1, BUTTON_X) && !SND120_isPauseBGM()) SND120_startSE(SE_DATA[selectedItem[1]], 0, 16);
        if(Joypad_getHit(JOY_1, BUTTON_Y) && !SND120_isPauseBGM()) SND120_startSE(SE_DATA[selectedItem[2]], 1, 16);
        if(Joypad_getHit(JOY_1, BUTTON_Z) && !SND120_isPauseBGM()) SND120_startSE(SE_DATA[selectedItem[3]], 2, 16);

        // pause BGM
        if(Joypad_getHit(JOY_1, BUTTON_START) && SND120_isPlayingBGM()) {
            if(SND120_isPauseBGM()) {
                SND120_resumeBGM();
                VDP_drawText("resume", 30, 5);
                flagPause = FALSE;
            }
            else{
                SND120_pauseBGM();
                VDP_drawText("pause ", 30, 5);
                flagPause = TRUE;
            }
        }


        // draw
        char tmp[9];
        if(flagDrawName[0]){SND120_getName(BGM_DATA[selectedItem[0]], tmp); VDP_drawText(tmp, 14,  5); flagDrawName[0] = FALSE;}
        if(flagDrawName[1]){SND120_getName(SE_DATA[selectedItem[1]],  tmp); VDP_drawText(tmp, 14,  7); flagDrawName[1] = FALSE;}
        if(flagDrawName[2]){SND120_getName(SE_DATA[selectedItem[2]],  tmp); VDP_drawText(tmp, 14,  9); flagDrawName[2] = FALSE;}
        if(flagDrawName[3]){SND120_getName(SE_DATA[selectedItem[3]],  tmp); VDP_drawText(tmp, 14, 11); flagDrawName[3] = FALSE;}

        if(flagDrawFreqTimerA){
            Tile_drawNum(BG_A, freqTimerA, 14, 13, 4);
            s32 value = (3835227 / 72) / (1024 - freqTimerA) ;
            Tile_drawNum(BG_A, value, 20, 13, 5);
            flagDrawFreqTimerA = FALSE;
        }
        if(flagDrawCursor){
            u16 i;
            for(i=0;i<4;++i){
                if(selectedList == i) VDP_drawTextBG(BG_A, ">", 12, 5 + i * 2);
                else                  VDP_drawTextBG(BG_A, " ", 12, 5 + i * 2);
            }
            flagDrawCursor = FALSE;
        }


        VDP_waitVSync();

    }
    return (0);
}
