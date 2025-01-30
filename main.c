#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>

#include "emulator/gameboy.h"
#include "render/render.h"

#define TEST_ROM "./test.gb"
#define ROM_PATH TEST_ROM

typedef struct {
  pthread_t gb_thread;
  gameboy_thread_args_t gb_thread_args;
  gameboy_t* gb;
  render_state_t* rs;
} appstate_t;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

  // appstate
  appstate_t* as = SDL_malloc(sizeof(appstate_t));
  if (as == NULL) {
    // TODO:
    return SDL_APP_FAILURE;
  }
  *appstate = as;

  as->gb = SDL_malloc(sizeof(gameboy_t));
  if (as->gb == NULL) {
    // TODO:
    return SDL_APP_FAILURE;
  }
  if (!gameboy_init(as->gb)) {
    // TODO:
    return SDL_APP_FAILURE;
  }
  if (!gameboy_load_rom(as->gb, ROM_PATH)) {
    // TODO:
    return SDL_APP_FAILURE;
  }
  as->gb_thread_args = (gameboy_thread_args_t){
      .gb = as->gb,
  };
  if (!pthread_create(&as->gb_thread, NULL, gameboy_run_thread, &as->gb_thread_args)) {
    // TODO:
  }

  as->rs = SDL_malloc(sizeof(render_state_t));
  if (as->rs == NULL) {
    // TODO:
    return SDL_APP_FAILURE;
  }
  if (!render_state_init(as->rs, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0)) {
    // TODO:
  }

  // TODO: sound?

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  appstate_t* as = (appstate_t*)appstate;
  // render here
  // TODO: mutex locking here?
  draw_screen(as->rs, as->gb->screen);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  appstate_t* as = (appstate_t*)appstate;
  switch (event->type) {
  case SDL_EVENT_KEY_DOWN:
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  //
  printf("-- complete --\n");
}
