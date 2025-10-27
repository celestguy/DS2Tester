//
// Created by theo on 27/10/2025.
//

#include "sprite.h"

struct pad_sprite_st createPadSprite(const int x, const int y, GSTEXTURE *spriteTexture, const int padKey) {
    struct pad_sprite_st ps;

    ps.x = x;
    ps.y = y;
    ps.spriteTexture = spriteTexture;
    ps.padKey = padKey;

    return ps;
}
