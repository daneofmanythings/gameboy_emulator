#include "gameboy.h"

#include <stdint.h>

void* display_driver_thread(void* args) {
  //
  return args;
}

bool display_driver_load_data(gameboy_t* gb, display_driver_t* dd) {
  //
  return true;
}

uint16_t screen_data_pixel_to_draw(uint8_t screen_pixel_num, uint8_t screen_data_x, uint8_t screen_data_y) {
  uint16_t result = 0;
  // factor in y offset
  result += (screen_data_y + (screen_pixel_num / SCREEN_WIDTH)) * SCREEN_DATA_WIDTH;
  // factor in x offset
  result += screen_data_x + (screen_pixel_num % SCREEN_WIDTH);
  return result;
}
