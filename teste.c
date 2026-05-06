#include <ncurses.h>
#include "lut.h"

#define STEP (16 * 256 / 1000)

int main(void)
{
  int ents[12];
  int ecnt = 12;
  int e;

  initscr();
  cbreak();
  noecho();
  curs_set(0);

  for (e = 0; e < ecnt; e++) {
    ents[e] = e * 256 / 12;
  }

  for(;;) {
    clear();
    for (e = 0; e < ecnt; e++) {
      int x = 0;
      int y = 0;
      x = cos(ents[e]);
      y = sin(ents[e]);
      x = (x * 12 / 256) + COLS/4;
      y = (y * 12 / 256) + LINES/2;
      mvaddch(y, x*2, '@');
      ents[e] = (ents[e]+STEP)&255;
    }
    refresh();
    napms(16);
  }

  endwin();
}
