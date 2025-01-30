#include "render.h"
#include <stdlib.h>

bool render_state_init(render_state_t* rs, const size_t width, const size_t height, const SDL_WindowFlags flags) {
  if (width < DISPLAY_WIDTH || height < DISPLAY_HEIGHT) {
    SDL_Log("Display width/height too small, width: min=%d, got=%zu, height: min=%d, got=%zu", SCREEN_WIDTH, width, SCREEN_HEIGHT, height);
    return false;
  }

  bool is_successful = SDL_CreateWindowAndRenderer("gameboy emulator", width, height, flags, &rs->window, &rs->renderer);
  if (is_successful == false) {
    return false;
  }

  rs->pixel_stamp = create_pixel_stamp(width, height);

  rs->screen_start_x = (width - rs->pixel_stamp.w * SCREEN_WIDTH) / 2;
  rs->screen_start_y = (height - rs->pixel_stamp.h * SCREEN_HEIGHT) / 2;

  return true;
}

SDL_FRect create_pixel_stamp(const size_t window_width, const size_t window_height) {
  size_t side_len;
  size_t x_len = window_width / SCREEN_WIDTH;
  size_t y_len = window_height / SCREEN_HEIGHT;
  if (x_len < y_len) {
    side_len = x_len;
  } else {
    side_len = y_len;
  }
  return (SDL_FRect){
      .w = side_len,
      .h = side_len,
      .x = 0,
      .y = 0,
  };
}

void pixel_stamp_update_draw_coordinates(render_state_t* rs, size_t i) {
  rs->pixel_stamp.x = (i % SCREEN_WIDTH) * rs->pixel_stamp.w + rs->screen_start_x;
  rs->pixel_stamp.y = (i / SCREEN_WIDTH) * rs->pixel_stamp.h + rs->screen_start_y;
}

void draw_screen(render_state_t* rs, pixel_color_t screen[SCREEN_SIZE]) {
  static const pixel_color_t background = {200, 50, 50};
  SDL_SetRenderDrawColor(rs->renderer, background.r, background.g, background.b, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(rs->renderer, NULL);
  uint16_t pixel_num;
  for (size_t i = 0; i < SCREEN_SIZE; ++i) {
    pixel_stamp_update_draw_coordinates(rs, i);
    SDL_SetRenderDrawColor(rs->renderer, screen[i].r, screen[i].g, screen[i].b, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(rs->renderer, &rs->pixel_stamp);
  }
  SDL_RenderPresent(rs->renderer);
}

void renderer_destroy(render_state_t* rs) {
  SDL_DestroyRenderer(rs->renderer);
  rs->renderer = NULL;
  SDL_DestroyWindow(rs->window);
  rs->window = NULL;

  free(rs);
  rs = NULL;
}
