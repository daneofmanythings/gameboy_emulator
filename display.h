#ifndef EMULATOR_DISPLAY_H
#define EMULATOR_DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 640

#define SCREEN_DATA_WIDTH 256
#define SCREEN_DATA_HEIGHT 256
#define SCREEN_DATA_SIZE ((SCREEN_DATA_WIDTH) * (SCREEN_DATA_HEIGHT))

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_SIZE ((SCREEN_WIDTH) * (SCREEN_HEIGHT))

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} pixel_color_t;

typedef struct {
  uint8_t y;
  uint8_t x;
  pixel_color_t pixels[SCREEN_DATA_SIZE];
} screen_data_t;

#endif // !DEBUG
