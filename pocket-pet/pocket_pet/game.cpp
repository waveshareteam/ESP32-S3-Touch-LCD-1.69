#include "game.h"
#include "config.h"

static const uint32_t ROUND_MS = 20000, STAR_MS = 1100;
static uint32_t startMs, starMs;
static int sx, sy, score;
static bool done;

static void newStar(uint32_t ms) {
  sx = random(40, 200);
  sy = random(64, 214);
  starMs = ms;
}

void gameStart() {
  score = 0;
  done = false;
  startMs = millis();
  newStar(startMs);
}

bool gameTap(int x, int y) {
  if (done) return false;
  int dx = x - sx, dy = y - sy;
  if (dx * dx + dy * dy <= 32 * 32) {
    score++;
    newStar(millis());
    return true;
  }
  return false;
}

void gameUpdate(uint32_t ms) {
  if (done) return;
  if (ms - startMs >= ROUND_MS) done = true;
  else if (ms - starMs >= STAR_MS) newStar(ms);
}

bool gameFinished() { return done; }
int gameScore() { return score; }

void gameDraw(Arduino_GFX *g) {
  g->fillScreen(C565(10, 12, 40));
  g->setTextColor(C565(255, 255, 255));
  g->setTextSize(2);
  g->setCursor(46, 22);
  g->printf("Star Catch  %d", score);
  if (done) {
    g->setCursor(40, 120);
    g->printf("Score: %d", score);
    g->setCursor(40, 160);
    g->print("Tap to finish");
    return;
  }
  uint32_t left = (ROUND_MS - (millis() - startMs)) / 1000;
  g->setCursor(100, 42);
  g->printf("%lus", (unsigned long)left);
  g->fillCircle(sx, sy, 22, C565(255, 220, 60));
  g->fillCircle(sx, sy, 12, C565(255, 250, 180));
}
