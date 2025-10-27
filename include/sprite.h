//
// Created by theo on 27/10/2025.
//

#ifndef DS2TESTER_SPRITE_H
#define DS2TESTER_SPRITE_H

#endif //DS2TESTER_SPRITE_H

#include <gsKit.h>

struct pad_sprite_st {
    int x;
    int y;
    GSTEXTURE *spriteTexture;
    int padKey;
};

struct pad_sprite_st createPadSprite(int x, int y, GSTEXTURE* spriteTexture, int padKey);