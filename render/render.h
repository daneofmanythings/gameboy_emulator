#ifndef RENDER_RENDER_H
#define RENDER_RENDER_H

#include <SDL3/SDL.h>

#include "../display.h"

typedef struct {
  SDL_Renderer* renderer;
  SDL_Window* window;
  SDL_FRect pixel_stamp;
  size_t screen_start_x;
  size_t screen_start_y;
} render_state_t;

bool render_state_init(render_state_t* rs, const size_t width, const size_t height, const SDL_WindowFlags flags);
void draw_screen(render_state_t* rs, pixel_color_t screen[SCREEN_SIZE]);
SDL_FRect create_pixel_stamp(const size_t window_width, const size_t window_height);
void render_state_destroy(render_state_t* renderer);

#endif // !DEBUG
