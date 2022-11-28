
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "data.h"

#define COLS 100
#define ROWS 40

int main() {
  assert(COLS > ROWS * 2.1f);

  // Clear screen and hide cursor
  printf("\x1b[2J\x1b[?25l");

  float zb[COLS * ROWS];
  char clr[COLS * ROWS];

  float lightx = 0, lighty = -1.5f, lightz = 3;

  float t = 0;
  while (1) {
    memset(zb, 0, sizeof(zb));
    memset(clr, 0x20, sizeof(clr));

    float c = cos(t), s = sin(t);

    // lightx = cos(t) * 1.5f;
    // lighty = 0;
    // lightz = sin(t) * 1.5f;

    for (int i = 0; i < 128 * 128; i++) {
      if (data[i] == 0x20)
        continue;

      float x = (i % 128) / 128.0f - 0.5f;
      float y = (i >> 7) / 128.0f - 0.5f;

      float step = data[i] == '!' ? 0.1f : 0.005f;
      for (float z = -0.05f; z <= 0.05f; z+= step) {
        // Rotate
        float rx = x * c - z * s;
        float rz = x * s + z * c;
        float ry = y;

        // Perspective
        float p = 0.8f * (1 + rz);

        int tx = (int)((rx * p + 0.5f) * ROWS * 2) + (COLS / 2 - ROWS) / 2;
        int ty = (int)((y * p + 0.5f) * ROWS);
        int idx = tx + ty * COLS;
        if (idx < 0 || idx > sizeof(clr))
          continue;

        float tz = rz + 2;
        if (zb[idx] < tz) {
          zb[idx] = tz;

          // Find normal
          float nx = 0, ny = 0, nz = 0;
          if (data[i] != '!' && fabs(z) < 0.049f) {
            float f = M_PI * 2 * (data[i] - 0x30) / 32.0f;
            nx = cos(f);
            ny = sin(f);
          } else {
            nz = (z > 0) - (z < 0);
          }

          // Rotate normal
          float tnx = nx * c + nz * s;
          nz = nx * s + nz * c;
          nx = tnx;

          // Get light vector
          float vx = lightx - rx, vy = lighty - ry, vz = lightz - rz;
          float d = sqrt(vx * vx + vy * vy + vz * vz);
          vx/= d;
          vy/= d;
          vz/= d;

          // Get cos angle between normal vector and light vector
          float a = vx * nx + vy * ny + vz * nz;

          clr[idx] = " .,-~:;=!*#$@"[lroundf((a > 0) * a * 12)];
        }
      }
    }

    for (int i = 0; i < ROWS; i++)
      clr[COLS * i] = '\n';
    clr[sizeof(clr) - 1] = 0;
    printf("\x1b[H\n\n%s", clr);

    t+= 0.005f;
    usleep(3000);
  }
  return 0;
}
