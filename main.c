#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdio.h>

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

  // initialize key structures
  //
  // appstate
  //
  // engine
  //
  // renderer
  //
  // audio

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  // render here
  return SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  //
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  //
  printf("-- complete --\n");
}
