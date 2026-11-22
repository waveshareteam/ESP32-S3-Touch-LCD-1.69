#pragma once
#include <Arduino_GFX_Library.h>

// "Star Catch": tap the stars as they appear. 20 s round.
void gameStart();
bool gameTap(int x, int y);   // true on a hit
void gameUpdate(uint32_t ms);
bool gameFinished();
int gameScore();
void gameDraw(Arduino_GFX *g);
