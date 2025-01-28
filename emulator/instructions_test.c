#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "gameboy.h"

Test(macros, dotheywork) {
  gameboy_t gb = {0};
  for (size_t i = 0; i < MEMORY_SIZE; ++i) {
    gb.memory[i] = i % 0xFF;
  }

  gb.cpu.a = 0xAA;
  gb.cpu.b = 0xBB;
  gb.cpu.c = 0xCC;

  LD_A__BC(&gb, 0x0);

  cr_assert(eq(u8, gb.cpu.a, gb.memory[0xBBCC]));
}
