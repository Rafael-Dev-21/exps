#include <ncurses.h>
#include "rng32.h"

#define FIRE_W 40
#define FIRE_H 40

void
renderFire(
    uint8_t *fire,
    int w, int h,
    void (*drawchar)(int, int, char)
)
{
  const char sheet[] = " .,*oO#@";
  for (int row = 0; row < h; row++) {
    for (int column = 0; column < w; column++) {
      int index = column + row * w;
      uint8_t intensity = fire[index];
      int sprite = intensity * 7 / 36;
      drawchar(column, row, sheet[sprite]);
    }
  }
}

void
drawSprite(
    int x,
    int y,
    char c
)
{
  move(y, x*2);
  addch(c);
}

void
seedFireSource(
    uint8_t *fire,
    int w, int h
)
{
  for (int column = 0; column < w; column++) {
    int size = w * h;
    int index = size - w + column;
    fire[index] = 36;
  }
}

void
updateFireCell(
    uint8_t *fire,
    int w, int h, int idx
)
{
  int start = (idx / w) * w;
  int end = start + w;
  int belowIndex = idx + w;
  if (belowIndex >= w * h) return;
  uint8_t below = fire[belowIndex];
  uint8_t decay = rng32()&3;
  uint8_t newValue =
    below - decay > 0 ? below - decay : 0;
  int targetIndex = idx - decay + (rng32()&3);
  if (targetIndex < 0) targetIndex = 0;
  if (targetIndex >= end) targetIndex = end-1;
  fire[targetIndex] = newValue;
}

void
processFire(
    uint8_t *fire,
    int w, int h
)
{
  for (int column = 0; column < w; column++) {
    for (int row = 0; row < h; row++) {
      int index = column + row * w;
      if (row >= h) continue;
      updateFireCell(fire, w, h, index);
    }
  }
}

int main(void)
{
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  curs_set(0);

  uint8_t fire[FIRE_W * FIRE_H] = {0};
  seedFireSource(fire, FIRE_W, FIRE_H);

  while (getch()!='q') {
    clear();
    processFire(fire, FIRE_W, FIRE_H);
    renderFire(fire, FIRE_W, FIRE_H, &drawSprite);
    refresh();
    napms(50);
  }

  endwin();
  return 0;
}
