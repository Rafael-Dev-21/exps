#include <ncurses.h>
#include "lut.h"

#define ONE 256

static uint32_t hash(int32_t x, int32_t y)
{
  uint32_t a = x, b = y;
  a *= 16777619; b ^= (a >> 16) | (a << 16);
  b *= 16777619; a ^= (b >> 16) | (b << 16);
  a *= 16777619;
  return a;
}

struct fixed2 {
  int x;
  int y;
};

struct fixed3 {
  int x;
  int y;
  int z;
};

static int fixed_mul(int a, int b)
{ return (a * b)/ONE; }

static int fixed_div(int a, int b)
{ return (a * ONE)/b; }

int smoother(int t)
{
  return fixed_mul(t, fixed_mul(t, fixed_mul(t, fixed_mul(t, fixed_mul(t, 6*ONE) - 15*ONE) + 10*ONE)));
}

int lerp(int a, int b, int t)
{
  return fixed_mul(ONE - t, a) + fixed_mul(t, b);
}

struct fixed2 gradient2(int x, int y)
{
  uint64_t angle = hash(x, y) * 65536LLU / UINT32_MAX;
  struct fixed2 v;
  v.x = cosq16(angle);
  v.y = sinq16(angle);
  return v;
}

struct fixed3 gradient3(int x, int y, int z)
{
  uint64_t yaw = hash(x, z) * 65536LLU / UINT32_MAX;
  uint64_t pitch = hash(y, z) * 65536LLU / UINT32_MAX;
  uint64_t roll = hash(z, y) * 65536LLU / UINT32_MAX;
  int spr = fixed_mul(sinq16(pitch), sinq16(roll));
  int cr = cosq16(roll);
  int sr = sinq16(roll);
  int cy = cosq16(yaw);
  int sy = sinq16(yaw);
  struct fixed3 v;
  v.x = fixed_mul(-cy, spr) - fixed_mul(sy, cr);
  v.y = fixed_mul(-sy, spr) + fixed_mul(cy, cr);
  v.z = fixed_mul(cosq16(pitch), sinq16(roll));
  return v;
}

int gdot2(int ix, int iy, int fx, int fy)
{
  struct fixed2 g = gradient2(ix, iy);
  return fixed_mul(g.x, fx) + fixed_mul(g.y, fy);
}

int gdot3(
    int ix, int iy, int iz,
    int fx, int fy, int fz
)
{
  struct fixed3 g = gradient3(ix, iy, iz);
  return fixed_mul(g.x, fx) + fixed_mul(g.y, fy) + fixed_mul(g.z, fz);
}

int perlin2(
    int x, int y)
{
  int ix = x >> 8;
  int iy = y >> 8;
  int fx = x & 255;
  int fy = y & 255;
  int u = smoother(fx);
  int v = smoother(fy);
  int n0, n1, x0, x1, value;
  n0 = gdot2(ix, iy, fx, fy);
  n1 = gdot2(ix+1, iy, fx-ONE, fy);
  x0 = lerp(n0, n1, u);
  n0 = gdot2(ix, iy+1, fx, fy-ONE);
  n1 = gdot2(ix+1, iy+1, fx-ONE, fy-ONE);
  x1 = lerp(n0, n1, u);
  value = lerp(x0, x1, v);
  return fixed_div(value, 180);
}

int perlin3(
    int x, int y, int z)
{
  int ix = x >> 8;
  int iy = y >> 8;
  int iz = z >> 8;
  int fx = x & 255;
  int fy = y & 255;
  int fz = z & 255;
  int u = smoother(fx);
  int v = smoother(fy);
  int w = smoother(fz);
  int n0, n1, x0, x1, y0, y1, value;
  n0 = gdot3(ix, iy, iz, fx, fy, fz);
  n1 = gdot3(ix+1, iy, iz, fx-ONE, fy, fz);
  x0 = lerp(n0, n1, u);
  n0 = gdot3(ix, iy+1, iz, fx, fy-ONE, fz);
  n1 = gdot3(ix+1, iy+1, iz, fx-ONE, fy-ONE, fz);
  x1 = lerp(n0, n1, u);
  y0 = lerp(x0, x1, v);
  n0 = gdot3(ix, iy, iz+1, fx, fy, fz-ONE);
  n1 = gdot3(ix+1, iy, iz+1, fx-ONE, fy, fz-ONE);
  x0 = lerp(n0, n1, u);
  n0 = gdot3(ix, iy+1, iz+1, fx, fy-ONE, fz-ONE);
  n1 = gdot3(ix+1, iy+1, iz+1, fx-ONE, fy-ONE, fz-ONE);
  x1 = lerp(n0, n1, u);
  y1 = lerp(x0, x1, v);
  value = lerp(y0, y1, w);
  return fixed_div(value, 221);
}

int fbm2(int x, int y, int (*fn)(int, int))
{
  int xa = x, ya = y;
  int amp = ONE;
  int res = 0, div = 0;
  for (int i = 0; i < 10; i++) {
    res += fixed_mul(fn(xa, ya), amp);
    div += amp;
    xa = fixed_mul(xa, 441);
    ya = fixed_mul(ya, 441);
    amp = fixed_mul(amp, 148);
  }
  return fixed_div(res, div);
}

int fbm3(int x, int y, int z, int (*fn)(int, int, int))
{
  int xa = x, ya = y, za = z;
  int amp = ONE;
  int res = 0, div = 0;
  for (int i = 0; i < 10; i++) {
    res += fixed_mul(fn(xa, ya, za), amp);
    div += amp;
    xa = fixed_mul(xa, 441);
    ya = fixed_mul(ya, 441);
    za = fixed_mul(za, 441);
    amp = fixed_mul(amp, 148);
  }
  return fixed_div(res, div);
}

int perlin2o(int x, int y)
{
  return fbm2(x, y, &perlin2);
}

int perlin3o(int x, int y)
{
  int sample = fixed_div((fbm2(x, y, &perlin2)+ONE), 16);
  return fbm3(x, sample, y, &perlin3);
}

void drawNoise(
    int x, int y,
    int w, int h,
    int (*fn)(int, int),
    void (*draw)(int, int, int)
)
{
  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      int nx = fixed_div(col, 16) - 128;
      int ny = fixed_div(row, 16) - 128;
      int n  = fn(nx, ny);
      draw(x+col, y+row, smoother((n+ONE)/2));
    }
  }
}

enum PAIR {
  START = 1,
  WATER = 1,
  SAND,
  GRASS,
  SNOW,
  PAIR_COUNT
};

void drawTile(
    int x, int y, int n
) {
  static char sheet[] = "~~.,*oO#";
  static int pal[] = { WATER, SAND,GRASS, SNOW };
  int spriteId = fixed_mul(n, 8);
  int colorId = fixed_mul(n, 4);
  int sprite = sheet[spriteId];
  if (has_colors()) { sprite |= COLOR_PAIR(pal[colorId]); };
  move(y, x * 2);
  addch(sprite);
}

int main(void)
{
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  if (has_colors()) {
    start_color();
    init_pair(GRASS, COLOR_GREEN, COLOR_BLACK);
    init_pair(SAND, COLOR_YELLOW, COLOR_BLACK);
    init_pair(SNOW, COLOR_WHITE, COLOR_BLACK);
    init_pair(WATER, COLOR_BLUE, COLOR_BLACK);
  }
  const int w = 16, h = 16;
  drawNoise(1, 1, w, h, &perlin2o, &drawTile);
  drawNoise(18, 1, w, h, &perlin3o, &drawTile);
  while(getch()!='q');
  endwin();
  return 0;
}
