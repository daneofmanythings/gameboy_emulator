#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "gameboy.h"

bool gameboy_init(gameboy_t* gb) {
  //
  return true;
}
bool gameboy_load_rom(gameboy_t* gb, const char* rom_path) {
  //
  return true;
}

typedef struct {
  thread_event_t* te;
  uint32_t* clock_speed;
} clock_thread_args_t;
void* cpu_clock_thread(void* args);
typedef struct {
  gameboy_t* gb;
} display_thread_args_t;
void* gameboy_run_thread(void* args) {

  gameboy_t* gb = ((gameboy_thread_args_t*)args)->gb;

  // spawn cpu clock thread
  pthread_t clock_thread;
  clock_thread_args_t ctargs = {
      .te = &gb->clock_tick,
      .clock_speed = &gb->clock_speed,
  };
  if (!pthread_create(&clock_thread, NULL, cpu_clock_thread, &ctargs)) {
    // TODO:
  }
  // spawn display driver thread
  pthread_t display_thread;
  display_thread_args_t dtargs = {
      .gb = gb,
  };
  if (!pthread_create(&display_thread, NULL, display_driver_thread, &dtargs)) {
    // TODO:
  }

  // simluating cpu cycle delay
  num_cycles cpu_cycles_to_wait = 0;
  while (true) {
    thread_event_register(&gb->clock_tick);
    thread_event_wait(&gb->clock_tick);
    if (cpu_cycles_to_wait < 1) {
      cpu_cycles_to_wait = gameboy_emulate_cycle(gb);
    }
    thread_event_finish(&gb->clock_tick);
    cpu_cycles_to_wait -= 1;
  }

  return args;
}

void* cpu_clock_thread(void* args) {
  clock_thread_args_t* ctargs = (clock_thread_args_t*)args;
  uint32_t* clock_speed = ctargs->clock_speed;
  thread_event_t* te = ctargs->te;
  while (true) {
    usleep(1000000 / *clock_speed);
    thread_event_trigger(te);
  }
  return args;
}

num_cycles gameboy_emulate_cycle(gameboy_t* gb) {
  opcode op = gb->memory[PC];
  instruction_f instruction = fetch_instruction_from_opcode(op);

  num_cycles n = instruction(gb, op);

  return n;
}

instruction_f fetch_instruction_from_opcode(opcode oc) {
  switch (oc) {
  default:
    fprintf(stderr, "unknown opcode='0x%04x'\n", oc);
    return NULL;
  }
}
