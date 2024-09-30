#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>
#include "../raylib/src/raylib.h"

#define NUM_THREADS 16

typedef struct {
  int startX, startY, endX, endY;
} ChunkData;

unsigned maxX = 1920, maxY = 1080;
ChunkData data[NUM_THREADS];
Color mat[1920][1080];

int mandelbrot(double real, double imag) {
  double realPart = 0.0, imagPart = 0.0;
  int max_iterations = 100;
  for (int i = 0; i < max_iterations; i++) {
    double realSquared = realPart * realPart;
    double imagSquared = imagPart * imagPart;
    if (realSquared + imagSquared > 4.0) return i;

    imagPart = 2.0 * realPart * imagPart + imag;
    realPart = realSquared - imagSquared + real;
  }
  return max_iterations;
}

void render_mandelbrot(){
  for (size_t x = 0; x < maxX; x++) {
    for (size_t y = 0; y < maxY; y++) {
      double real = (x - (double)maxX / 2) / ((double)maxX / 2 * 1.5) - 0.75;
      double imag = (y - (double)maxY / 2) / ((double)maxY / 2);
      int iterations = mandelbrot(real, imag);
      float hue = iterations / 100.0f;
      Color color = ColorFromHSV(hue * 360.0f, 1.0f, (iterations < 100) ? 1.0f : 0.0f);
      mat[x][y] = color;
    }
  }
}

void draw_mandelbrot() {
  for (int y = 0; y < maxY; y++) {
    for (int x = 0; x < maxX; x++) {
      DrawPixel(x, y, mat[x][y]);
    }
  }
}


int main(void) {
  size_t factor = 50;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(factor * 16, factor * 9, "GraphV");
  SetTargetFPS(60);

  maxX = factor * 16;
  maxY = factor * 9;
  render_mandelbrot();

  while (!WindowShouldClose()) {
    if (IsKeyPressed(KEY_ESCAPE)) break;
    if (IsWindowResized()) {
      int width = GetScreenWidth();
      int height = GetScreenHeight();
      if (width > 0 && height > 0) {
        maxX = width;
        maxY = height;
        render_mandelbrot();
      }
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);
    draw_mandelbrot();
    DrawFPS(10, 10);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
