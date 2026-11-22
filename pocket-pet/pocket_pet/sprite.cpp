#include "sprite.h"
#include "config.h"

enum { T = 0, OUT, BODY, EYE, MOUTH, CHEEK, TEAR };
static uint8_t px[SPRITE_SRC][SPRITE_SRC];

static bool inside(int x, int y) {
  int dx = x - 16, dy = y - 18;
  return dx * dx * 121 + dy * dy * 144 <= 144 * 121;  // ellipse rx=12, ry=11
}

static void set(int x, int y, uint8_t c) {
  if (x >= 0 && x < SPRITE_SRC && y >= 0 && y < SPRITE_SRC) px[y][x] = c;
}
static void rect(int x, int y, int w, int h, uint8_t c) {
  for (int j = 0; j < h; j++)
    for (int i = 0; i < w; i++) set(x + i, y + j, c);
}

static void eyes(Mood m, bool blink) {
  const int ex[2] = {10, 19};
  for (int e = 0; e < 2; e++) {
    int x = ex[e];
    if (m == MOOD_DEAD || m == MOOD_SICK) {  // X eyes
      set(x, 13, EYE); set(x + 2, 13, EYE); set(x + 1, 14, EYE); set(x, 15, EYE); set(x + 2, 15, EYE);
    } else if (blink || m == MOOD_HAPPY) {   // closed / happy arcs
      if (m == MOOD_HAPPY && !blink) { set(x, 15, EYE); set(x + 1, 14, EYE); set(x + 2, 15, EYE); }
      else rect(x, 14, 3, 1, EYE);
    } else if (m == MOOD_SAD) {
      rect(x, 14, 3, 3, EYE);
      set(x + (e ? 2 : 0), 12, EYE);          // droopy brow
      set(x + 1, 13, EYE);
    } else {
      rect(x, 13, 3, 4, EYE);
    }
  }
}

static void mouth(Mood m) {
  switch (m) {
    case MOOD_HAPPY: rect(13, 21, 6, 1, MOUTH); set(12, 20, MOUTH); set(19, 20, MOUTH); rect(14, 22, 4, 1, MOUTH); break;
    case MOOD_OK: rect(14, 21, 4, 1, MOUTH); break;
    case MOOD_HUNGRY: rect(14, 20, 4, 4, MOUTH); break;           // open mouth
    case MOOD_SAD: rect(13, 22, 6, 1, MOUTH); set(12, 23, MOUTH); set(19, 23, MOUTH); break;
    case MOOD_SICK: set(12, 22, MOUTH); rect(13, 21, 2, 1, MOUTH); rect(15, 22, 2, 1, MOUTH); rect(17, 21, 2, 1, MOUTH); set(19, 22, MOUTH); break;
    default: rect(13, 22, 6, 1, MOUTH); break;
  }
}

void spriteDraw(Arduino_GFX *g, int x0, int y0, Mood m, bool blink, int bob) {
  static const uint16_t bodyCol[] = {C565(255, 214, 90), C565(150, 220, 130), C565(255, 170, 90), C565(120, 160, 230), C565(140, 200, 120), C565(110, 110, 110)};
  uint16_t pal[7] = {0, C565(30, 25, 40), bodyCol[m], C565(20, 15, 30), C565(90, 20, 40), C565(255, 120, 140), C565(120, 200, 255)};

  memset(px, 0, sizeof(px));
  for (int y = 0; y < SPRITE_SRC; y++)
    for (int x = 0; x < SPRITE_SRC; x++)
      if (inside(x, y)) px[y][x] = (inside(x - 1, y) && inside(x + 1, y) && inside(x, y - 1) && inside(x, y + 1)) ? BODY : OUT;
  rect(7, 6, 3, 4, OUT); rect(8, 7, 1, 2, BODY);   // ears
  rect(22, 6, 3, 4, OUT); rect(23, 7, 1, 2, BODY);
  eyes(m, blink);
  mouth(m);
  if (m != MOOD_DEAD && m != MOOD_SICK) { rect(7, 18, 2, 2, CHEEK); rect(23, 18, 2, 2, CHEEK); }
  if (m == MOOD_SAD) { rect(11, 17, 1, 3, TEAR); }
  rect(10, 29, 4, 2, OUT); rect(18, 29, 4, 2, OUT);  // feet

  for (int y = 0; y < SPRITE_SRC; y++) {
    int x = 0;
    while (x < SPRITE_SRC) {
      uint8_t c = px[y][x];
      int run = 1;
      while (x + run < SPRITE_SRC && px[y][x + run] == c) run++;
      if (c) g->fillRect(x0 + x * SPRITE_SCALE, y0 + y * SPRITE_SCALE + bob, run * SPRITE_SCALE, SPRITE_SCALE, pal[c]);
      x += run;
    }
  }
}

static bool eggInside(int x, int y) {
  int dx = x - 16, dy = y - 18;
  return dx * dx * 144 + dy * dy * 81 <= 81 * 144;  // ellipse rx=9, ry=12
}

void spriteDrawEgg(Arduino_GFX *g, int x0, int y0, float p, uint32_t ms) {
  static const uint16_t pal[] = {0, C565(60, 45, 40), C565(245, 238, 215), C565(205, 150, 120), C565(70, 40, 30), C565(255, 225, 110)};
  static const int8_t crack[][2] = {{16, 7}, {15, 8}, {16, 9}, {17, 10}, {16, 11}, {15, 12}, {16, 13}, {17, 14}, {18, 15}, {17, 16}, {16, 17}, {15, 18}, {14, 19}, {15, 20}, {16, 21}, {17, 22}, {18, 23}, {17, 24}, {16, 25}};
  static const int8_t crack2[][2] = {{23, 16}, {22, 17}, {21, 17}, {20, 18}, {19, 19}, {18, 19}, {9, 20}, {10, 21}, {11, 21}, {12, 22}, {13, 23}};
  static uint8_t src[SPRITE_SRC][SPRITE_SRC];
  memset(src, 0, sizeof(src));
  for (int y = 0; y < SPRITE_SRC; y++)
    for (int x = 0; x < SPRITE_SRC; x++)
      if (eggInside(x, y)) {
        bool edge = !(eggInside(x - 1, y) && eggInside(x + 1, y) && eggInside(x, y - 1) && eggInside(x, y + 1));
        src[y][x] = edge ? 1 : ((x * 7 + y * 13) % 17 == 0 ? 3 : 2);
      }
  int n1 = (int)(p * 1.6f * 19), n2 = (int)((p - 0.4f) * 1.6f * 11);
  for (int i = 0; i < n1 && i < 19; i++) src[(int)crack[i][1]][(int)crack[i][0]] = 4;
  for (int i = 0; i < n2 && i < 11; i++) if (i >= 0) src[(int)crack2[i][1]][(int)crack2[i][0]] = 4;

  bool breaking = p >= 0.92f;
  int lift = breaking ? (int)((p - 0.92f) / 0.08f * 9) : 0;
  int amp = p < 0.25f ? 0 : (p < 0.7f ? 1 : 2);
  int ox = amp ? (int)lroundf(amp * sinf(ms / (p < 0.7f ? 140.0f : 70.0f))) : 0;

  for (int y = 0; y < SPRITE_SRC; y++) {
    int x = 0;
    while (x < SPRITE_SRC) {
      uint8_t c = src[y][x];
      int run = 1;
      while (x + run < SPRITE_SRC && src[y][x + run] == c) run++;
      if (c) {
        int dy = (breaking && y < 18) ? -lift : 0;
        int dx = (breaking && y < 18) ? -lift / 3 : 0;
        g->fillRect(x0 + (x + ox + dx) * SPRITE_SCALE, y0 + (y + dy) * SPRITE_SCALE, run * SPRITE_SCALE, SPRITE_SCALE, pal[c]);
      }
      x += run;
    }
  }
  if (breaking) g->fillRect(x0 + 9 * SPRITE_SCALE, y0 + (18 - lift / 2) * SPRITE_SCALE, 14 * SPRITE_SCALE, 3 * SPRITE_SCALE, pal[5]);  // glow from inside
}
