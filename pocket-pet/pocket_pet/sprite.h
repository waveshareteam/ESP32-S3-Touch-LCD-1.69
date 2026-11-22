#pragma once
#include <Arduino_GFX_Library.h>
#include "pet.h"

// Pet art is authored on a 32x32 palette grid and drawn at 4x (each source pixel = 4x4 block): 128x128 on screen.
#define SPRITE_SRC 32
#define SPRITE_SCALE 4
#define SPRITE_PX (SPRITE_SRC * SPRITE_SCALE)

// Egg for the first 60 s of a new pet: wobbles, cracks grow with progress (0..1), breaks apart at the end.
void spriteDrawEgg(Arduino_GFX *g, int x, int y, float progress, uint32_t ms);
void spriteDraw(Arduino_GFX *g, int x, int y, Mood mood, bool blink, int bob);
