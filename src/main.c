#include <stdio.h>

#include <gsKit.h>
#include <dmaKit.h>
#include <gsToolkit.h>

#include "sprite.h"
#include "pad.h"

static char padBuf[256] __attribute__((aligned(64)));

#define BLACK GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00)

GSTEXTURE layoutTexture;
GSTEXTURE upTexture;
GSTEXTURE rightTexture;
GSTEXTURE leftTexture;
GSTEXTURE downTexture;
GSTEXTURE buttonTexture;
GSTEXTURE startTexture;
GSTEXTURE selectTexture;
GSTEXTURE side1Texture;
GSTEXTURE side2Texture;
GSTEXTURE joyTexture;
GSTEXTURE joyPressedTexture;

void initSprites(GSGLOBAL *gsGlobal) {
    gsKit_texture_png(gsGlobal, &layoutTexture, "host:/assets/layout.png");
    gsKit_texture_png(gsGlobal, &upTexture, "host:/assets/up.png");
    gsKit_texture_png(gsGlobal, &downTexture, "host:/assets/down.png");
    gsKit_texture_png(gsGlobal, &rightTexture, "host:/assets/right.png");
    gsKit_texture_png(gsGlobal, &leftTexture, "host:/assets/left.png");
    gsKit_texture_png(gsGlobal, &buttonTexture, "host:/assets/button.png");
    gsKit_texture_png(gsGlobal, &startTexture, "host:/assets/start.png");
    gsKit_texture_png(gsGlobal, &selectTexture, "host:/assets/select.png");
    gsKit_texture_png(gsGlobal, &side1Texture, "host:/assets/side1.png");
    gsKit_texture_png(gsGlobal, &side2Texture, "host:/assets/side2.png");
    gsKit_texture_png(gsGlobal, &joyTexture, "host:/assets/joy.png");
    gsKit_texture_png(gsGlobal, &joyPressedTexture, "host:/assets/joy_pressed.png");
}

void draw(const u32 x, const u32 y, GSGLOBAL *gsGlobal, const GSTEXTURE *texture) {
    constexpr u64 TexCol = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0xFF,0xFF);

    gsKit_prim_sprite_texture(gsGlobal, texture,	x,  // X1
                            y,  // Y2
                            0.0f,  // U1
                            0.0f,  // V1
                            texture->Width + x, // X2
                            texture->Height +  y, // Y2
                            texture->Width, // U2
                            texture->Height, // V2
                            3,
                            TexCol);
}

void initAll() {
    sceSifInitRpc(0);

    loadModules();

    padInit(0);

    padPortOpen(PAD_DEFAULT_PORT, PAD_DEFAULT_PORT, padBuf);

    initPad();
}

GSGLOBAL* initGs() {
    GSGLOBAL *gsGlobal = gsKit_init_global();

    gsGlobal->PSM = GS_PSM_CT32;
    gsGlobal->PSMZ = GS_PSMZ_32;
    //gsGlobal->ZBuffering = GS_SETTING_ON;

    dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
            D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

    // Initialize the DMAC
    dmaKit_chan_init(DMA_CHANNEL_GIF);

    gsKit_init_screen(gsGlobal);

    gsKit_mode_switch(gsGlobal, GS_PERSISTENT);

    gsKit_clear(gsGlobal, BLACK);

    return gsGlobal;
}

int main(void) {
    struct padButtonStatus buttons;

    initAll();

    GSGLOBAL *gsGlobal = initGs();
    initSprites(gsGlobal);

    const int count = 16;

    const struct pad_sprite_st test[] = {
        createPadSprite(13, 72, &upTexture, PAD_UP),
        createPadSprite(13, 96, &downTexture, PAD_DOWN),
        createPadSprite(0, 85, &leftTexture, PAD_LEFT),
        createPadSprite(24, 85, &rightTexture, PAD_RIGHT),
        createPadSprite(152, 103, &buttonTexture, PAD_CROSS),
        createPadSprite(171, 84, &buttonTexture, PAD_CIRCLE),
        createPadSprite(152, 65, &buttonTexture, PAD_TRIANGLE),
        createPadSprite(133, 84, &buttonTexture, PAD_SQUARE),
        createPadSprite(106, 88, &startTexture, PAD_START),
        createPadSprite(62, 89, &selectTexture, PAD_SELECT),
        createPadSprite(4, 36, &side1Texture, PAD_L1),
        createPadSprite(145, 36, &side1Texture, PAD_R1),
        createPadSprite(4, 0, &side2Texture, PAD_L2),
        createPadSprite(145, 0, &side2Texture, PAD_R2),
        createPadSprite(37, 109, &joyPressedTexture, PAD_L3),
        createPadSprite(108, 109, &joyPressedTexture, PAD_R3)
    };

    int exit = 0;
    int ret = 0;

    const u32 layoutX = gsGlobal->Width / 2 - layoutTexture.Width / 2;
    const u32 layoutY = gsGlobal->Height / 2 - layoutTexture.Height / 2;

    while (!exit) {
        gsKit_queue_reset(gsGlobal->CurQueue);

        gsKit_clear(gsGlobal, BLACK);

        draw(layoutX, layoutY, gsGlobal, &layoutTexture);

        ret = padGetState(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT);

        while(ret != PAD_STATE_STABLE && ret != PAD_STATE_FINDCTP1) {
            if(ret == PAD_STATE_DISCONN) {
                printf("Pad(%d, %d) is disconnected\n", PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT);
            }

            ret=padGetState(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT);
        }

        ret = padRead(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT, &buttons);

        if (ret != 0) {
            const int padData = 0xffff ^ buttons.btns;

            const int lJoyX = (int) ((buttons.ljoy_h - 0x7F) / 127. * 15.);
            const int lJoyY = (int) ((buttons.ljoy_v - 0x7F) / 127. * 15.);
            const int rJoyX = (int) ((buttons.rjoy_h - 0x7F) / 127. * 15.);
            const int rJoyY = (int) ((buttons.rjoy_v - 0x7F) / 127. * 15.);

            draw(layoutX + 37 + lJoyX, layoutY + 109 + lJoyY, gsGlobal, &joyTexture);
            draw(layoutX + 108 + rJoyX, layoutY + 109 + rJoyY, gsGlobal, &joyTexture);

            for (int i = 0; i < count; i++) {
                if (padData & test[i].padKey) {
                    u32 x = layoutX + test[i].x;
                    u32 y = layoutY + test[i].y;

                    if (test[i].padKey & PAD_L3) {
                        x += lJoyX;
                        y += lJoyY;
                    } else if (test[i].padKey & PAD_R3) {
                        x += rJoyX;
                        y += rJoyY;
                    }

                    draw(x, y, gsGlobal, test[i].spriteTexture);
                }
            }
        } else {
            exit = 1;
        }

        gsKit_sync_flip(gsGlobal);
        gsKit_queue_exec(gsGlobal);
    }

    return 0;
}