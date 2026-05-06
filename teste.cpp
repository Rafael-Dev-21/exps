#include <ncurses.h>
extern "C" {
#include "lut.h"
}
#include "fixed.hpp"

int main(void)
{
  q24_8 ents[12];
  int ecnt = 12;
  int e;

  initscr();
  cbreak();
  noecho();
  curs_set(0);

  constexpr q24_8 step(1/60.0);

  for (e = 0; e < ecnt; e++) {
    ents[e] = q24_8(e/12.0);
  }

  for(;;) {
    clear();
    for (e = 0; e < ecnt; e++) {
      int x = 0;
      int y = 0;
      x = cosq16(ents[e].raw()<<8);
      y = sinq16(ents[e].raw()<<8);
      x = (x * 12 / 256) + COLS/4;
      y = (y * 12 / 256) + LINES/2;
      mvaddch(y, x*2, '@');
      ents[e] = ents[e] + step;
      ents[e] = q24_8::from_raw(ents[e].raw()&255);
    }
    refresh();
    napms(16);
  }

  endwin();
}
