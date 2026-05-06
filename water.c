#include <stdint.h>
#include <stdio.h>
#include <ncurses.h>

#define ONE 256

#define MAXMASS 256
#define MAXCOMP 5
#define MINMASS 1
#define MINFLOW 3
#define MAXSPEED 256

#define MAP_W 16
#define MAP_H 16

#define max(a, b) ((a)>(b)?(a):(b))
#define min(a, b) ((a)<(b)?(a):(b))
#define clamp(a, mn, mx) max((mn), min((mx), (a)))

typedef enum {
  AIR=0,
  GROUND,
  WATER
} Block;

typedef int32_t fixed;

void loadLevel(Block *b, fixed *m, int w, int h)
{
  FILE *fp = fopen("water.txt", "r");
  char buf[1024];
  for (int row = 0; row < h && fgets(buf, 1023, fp); row++) {
    char *s = buf;
    for (int col = 0; col < w && *s; col++) {
      int idx = 1+col+(1+row)*(w+2);
      while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') s++;
      b[idx] = *s - '0';
      if (b[idx] == WATER) m[idx] = MAXMASS;
      s++;
    }
  }
  fclose(fp);
}

void renderLevel(Block *b, fixed *m, int w, int h, void (*draw)(int, int, Block, fixed))
{
  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      int idx = 1 + col + (row + 1) * (w+2);
      draw(col, row, b[idx], m[idx]);
    }
  }
}

void drawBlock(int x, int y, Block b, fixed m)
{
  char glyph[] = " #";
  int sprite;
  if (b < 2) {
    sprite = glyph[b];
  } else {
    sprite = '0' + m * 7 / ONE;
  }
  if (has_colors()) {
    sprite |= COLOR_PAIR(b);
  }
  move(y, x * 2);
  addch(sprite);
}

fixed getStableStateB(fixed totalMass)
{
  if (totalMass <= ONE) {
    return ONE;
  } else if (totalMass < 2 * MAXMASS + MAXCOMP) {
    return (MAXMASS*MAXMASS + totalMass*MAXCOMP)/(MAXMASS + MAXCOMP);
  } else {
    return (totalMass + MAXCOMP) / 2;
  }
}

void simulateCompression(Block *b, fixed *m, fixed *nm, int w, int h)
{
  fixed flow = 0;
  fixed remainingMass;

  int w2 = w + 2;
  int h2 = h + 2;

  for (int x = 1; x <= w; x++) {
    for (int y = 1; y <= h; y++) {
      int idx = x + w2 * y;
      if (b[idx] == GROUND) continue;
      flow = 0;
      remainingMass = m[idx];
      if (remainingMass <= 0) continue;
      if (b[idx+w2] != GROUND) {
        flow = getStableStateB(remainingMass + m[idx+w2]) - m[idx+w2];
        if (flow > MINFLOW) {
          flow /= 2;
        }
        flow = clamp(flow, 0, min(MAXSPEED, remainingMass));
        nm[idx] -= flow;
        nm[idx+w2] += flow;
        remainingMass -= flow;
      }
      if (remainingMass <= 0) continue;
      if (b[idx-1] != GROUND) {
        flow = (m[idx] - m[idx-1]) / 4;
        if (flow > MINFLOW) {
          flow /= 2;
        }
        flow = clamp(flow, 0, remainingMass);
        nm[idx] -= flow;
        nm[idx-1] += flow;
        remainingMass -= flow;
      }
      if (remainingMass <= 0) continue;
      if (b[idx+1] != GROUND) {
        flow = (m[idx] - m[idx+1]) / 4;
        if (flow > MINFLOW) {
          flow /= 2;
        }
        flow = clamp(flow, 0, remainingMass);
        nm[idx] -= flow;
        nm[idx+1] += flow;
        remainingMass -= flow;
      }
      if (remainingMass <= 0) continue;
      if (b[idx-w2] != GROUND) {
        flow = getStableStateB(remainingMass + m[idx-w2]);
        if (flow > MINFLOW) {
          flow /= 2;
        }
        flow = clamp(flow, 0, min(MAXSPEED, remainingMass));
        nm[idx] -= flow;
        nm[idx-w2] += flow;
        remainingMass -= flow;
      }
    }
  }
  for (int x = 0; x < w2; x++) {
    for (int y = 0; y < h2; y++) {
      int idx = x + y * w2;
      m[idx] = nm[idx];
    }
  }
  for (int x = 1; x <= w; x++) {
    for (int y = 1; y <= h; y++) {
      int idx = x + y * w2;
      if (b[idx] == GROUND) continue;
      if (m[idx] > MINMASS) {
        b[idx] = WATER;
      } else {
        b[idx] = AIR;
      }
    }
  }
  for (int x = 0; x < w2; x++) {
    m[x] = 0;
    m[x + w2 * h2 - w2] = 0;
  }
  for (int y = 1; y < h+1; y++) {
    m[y*w2] = 0;
    m[(y+1)*w2-1] = 0;
  }
}

int main(void)
{
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  nodelay(stdscr, TRUE);

  if (has_colors()) {
    start_color();
    init_pair(GROUND, COLOR_YELLOW, COLOR_BLACK);
    init_pair(WATER, COLOR_BLUE, COLOR_BLACK);
  }

  Block blocks[(MAP_W+2)*(MAP_H+2)] = {0};
  fixed mass[(MAP_W+2)*(MAP_H+2)] = {0};
  fixed newMass[(MAP_W+2)*(MAP_H+2)] = {0};

  loadLevel(blocks, mass, MAP_W, MAP_H);

  for (int x = 0; x < MAP_W+2; x++) {
    for (int y = 0; y < MAP_H+2; y++) {
      int idx = x + y * (MAP_W+2);
      newMass[idx] = mass[idx];
    }
  }

  while (getch()!='q') {
    clear();
    simulateCompression(blocks, mass, newMass, MAP_W, MAP_H);
    renderLevel(blocks, mass, MAP_W, MAP_H, &drawBlock);
    refresh();
    napms(16);
  }
  endwin();
  return 0;
}
