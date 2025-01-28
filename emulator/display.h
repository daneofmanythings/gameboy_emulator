#ifndef EMULATOR_DISPLAY_H
#define EMULATOR_DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

#include "gameboy.h"

typedef struct {
  uint8_t lcdc;
  uint8_t char_data[CHAR_DATA_SIZE];
  uint8_t oam[OAM_SIZE];
} display_driver_t;

typedef uint16_t dot_data;

typedef struct {
  dot_data dots[16];
  bool is_16_bit;
} character_datum_t;

typedef struct {
  uint8_t y;
  uint8_t x;
  uint8_t character_code;
  uint8_t atribute_data;
} obj_display_data_t;

typedef struct {
  dot_t dots[256 * 256];
} screen_data_t;

void* display_driver_run_thread(void* args);
bool display_driver_load_data(gameboy_t* gb, display_driver_t* dd);

#endif // !DEBUG
