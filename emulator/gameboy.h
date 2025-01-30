#ifndef EMULATOR_GAMEBOY_H
#define EMULATOR_GAMEBOY_H

#include <stdbool.h>
#include <stdint.h>

#include "../display.h"
#include "../events/thread_events.h"

#define MEMORY_SIZE 0xFFFF

/*
 * The CPU houses the registers used for computations
 *
 * Accumulator A:
 *    An 8-bit register for storing the results of arithmetic and logical operations.
 *
 * Flag register F:
 *    Consists of 4 flags that are set and reset according to the results of instruction
 *    execution. Flags CY and Z are tested by various condition branch instructions.
 *
 *     7   6   5    4   3   2   1   0
 *    --------------------------------
 *   | Z | N | H | CY | - | - | - | - |
 *   ---------------------------------
 *
 * Auxiliary registers B, C, D, E, H, L:
 *    These serve as auxiliary registers to the accumulator. As register pairs
 *    (BC, DE, HL), they are 16-bit registers that function as data pointers
 *
 * Program counter PC:
 *    A 16-bit register that holds the address data of the program to be executed
 *    next. Usually incremented automatically according to the byte count of the
 *    fetched instructions. When an instruction with branching is executed, immediate
 *    data and register contents are loaded.
 *
 * Stack pointer SP:
 *    A 16-bit pointer that holds the starting address of the stack area of memory.
 *    The contents of the stack pointer are DECREMENTED when subroutine CALL/PUSH
 *    instructions are executed or when an interrupt occurs. The contents are
 *    INCREMENTED when RET/POP instructions are executed
 *
 */
typedef struct {
  // the ordering of these registers matters for this implementation
  uint8_t a;
  uint8_t f;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t h;
  uint8_t l;
  uint16_t pc;
  uint16_t sp;
} cpu_t;

typedef struct {
  uint8_t memory[MEMORY_SIZE];
  pixel_color_t screen[SCREEN_SIZE];
  uint32_t clock_speed;
  thread_event_t clock_tick;
  cpu_t cpu;
} gameboy_t;

bool gameboy_init(gameboy_t* gb);
bool gameboy_load_rom(gameboy_t* gb, const char* rom_path);
void* gameboy_run_thread(void* args);

typedef uint8_t opcode;
typedef uint8_t num_cycles;
typedef uint8_t memory_at;
typedef uint8_t* register8_p;
typedef uint16_t* register16_p;

#define A (gb->cpu.a)
#define F (gb->cpu.f)
#define B (gb->cpu.b)
#define C (gb->cpu.c)
#define D (gb->cpu.d)
#define E (gb->cpu.e)
#define H (gb->cpu.h)
#define L (gb->cpu.l)
#define PC (gb->cpu.pc)
#define SP (gb->cpu.sp)
#define AF (*(uint16_t*)&gb->cpu.a)
#define BC (*(uint16_t*)&gb->cpu.b)
#define DE (*(uint16_t*)&gb->cpu.d)
#define HL (*(uint16_t*)&gb->cpu.h)
#define N (gb->memory[gb->cpu.pc + 1])
#define NN (*(uint16_t*)&gb->memory[gb->cpu.pc + 1])
#define MEMORY_AT(X) (gb->memory[(X)])

/*
 * Important Registers
 */

/*
 * LCDC bits:
 *   0 - [DMG mode] 0: BG display off, 1: BG display on [CGB mode] BG display always on
 *   1 - [OBJ on flag] 0: off, 1: on
 *   2 - [OBJ block composition selection flag] 0: 8x8 dots, 1: 8x16 dots
 *   3 - [BG code area selection flag] 0: BG_DATA_1, 1: BG_DATA_2
 *   4 - [BG character selection flag] 0: 0x8800 - 0x97FF, 1: 0x8000 - 87FF
 *   5 - [Windowing on flag] 0: off, 1: on
 *   6 - [Window code area selection flag] 0: 0x9800 - 0x9BFF, 1: 0x9C00 - 0x9FFF
 *   7 - [LCD controller operation stop flag] 0: LCDC off, 1: LCDC on
 */
#define LCDC 0xFF40

#define STAT 0xFF41 // may not need

/*
 * Starting y-coordinate of where to render the screen
 */
#define SCY 0xFF42

/*
 * Starting x-coordinate of where to render the screen
 */
#define SCX 0xFF43

#define LY 0xFF44  // may not need
#define LYC 0xFF45 //

#define BGP 0xFF47
#define OBP0 0xFF48
#define OBP1 0xFF49

#define CHAR_DATA_START 0x8000
#define CHAR_DATA_END 0x97FF
#define CHAR_DATA_SIZE ((CHAR_DATA_END) - (CHAR_DATA_START))

#define BG_DATA_1_START 0x9800
#define BG_DATA_1_END 0x9BFF
#define BG_DATA_1_SIZE ((BG_DATA_1_END) - (BG_DATA_1_START))

#define BG_DATA_2_START 0x9C00
#define BG_DATA_2_END 0x9FFF
#define BG_DATA_2_SIZE ((BG_DATA_2_END) - (BG_DATA_2_START))

#define OAM_START 0xFE00
#define OAM_END 0xFE9F
#define OAM_SIZE ((OAM_END) - (OAM_START))

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

void* display_driver_thread(void* args);

typedef struct {
  gameboy_t* gb;
} gameboy_thread_args_t;
void* gameboy_run_thread(void* args);
num_cycles gameboy_emulate_cycle(gameboy_t* gb);

typedef num_cycles (*instruction_f)(gameboy_t* gb, opcode op8);
instruction_f fetch_instruction_from_opcode(opcode oc8);

/*
 * Transfer and I/O instructions
 *
 * N references the byte following the opcode, and NN the two bytes
 * following (n and nn in the instruction reference). A specified
 * pair of registers (ex: HL, BC) represent a pair of registers
 * acting as a 16bit memory address.
 *
 * For some of the 8-bit IO transfer codes, the opcode encodes
 * some of the register information according to the following table
 * in bits 012 and/or 345.
 * --------------------
 * | Register | r, r' |
 * --------------------
 *      A        111
 *      B        000
 *      C        001
 *      D        010
 *      E        011
 *      H        100
 *      L        101
 *
 *
 * For some of the 16-bit IO transfer codes, the opcode encodes
 * some of the register information according to the following two tables
 * in bits 45.
 * -----------------
 * | Register | dd |
 * -----------------
 *     BC      00
 *     DD      01
 *     HL      10
 *     SP      11
 *
 * -----------------
 * | Register | qq |
 * -----------------
 *     BC       00
 *     DE       01
 *     HL       10
 *     AF       11
 */

/*
 * 8-Bit transfer and I/O instructions
 */

/*
 * Instruction   Action     CY    H     N     Z    Cycles     7 6   5 4 3   2 1 0
 * -----------------------------------------------------------------------------------
 *  LD r, r'   | r <- r' |  -  |  -  |  -  |  -  |   1    |   0 1     r      r'
 * -----------------------------------------------------------------------------------
 */

/*
 * Instruction: LD A, B
 *
 * Action: A <- B
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x78
 */
num_cycles LD_A_B(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD A, C
 *
 * Action: A <- C
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x79
 */
num_cycles LD_A_C(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD A, D
 *
 * Action: A <- D
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x7A
 */
num_cycles LD_A_D(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD A, E
 *
 * Action: A <- E
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x7B
 */
num_cycles LD_A_E(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD A, H
 *
 * Action: A <- H
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x7C
 */
num_cycles LD_A_H(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD A, L
 *
 * Action: A <- L
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x7D
 */
num_cycles LD_A_L(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD B, A
 *
 * Action: B <- A
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x47
 */
num_cycles LD_B_A(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD B, C
 *
 * Action: B <- C
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x41
 */
num_cycles LD_B_C(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD B, D
 *
 * Action: B <- D
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x42
 */
num_cycles LD_B_D(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD B, E
 *
 * Action: B <- E
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x43
 */
num_cycles LD_B_E(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD B, H
 *
 * Action: B <- H
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x44
 */
num_cycles LD_B_H(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD B, L
 *
 * Action: B <- L
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x45
 */
num_cycles LD_B_L(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD C, B
 *
 * Action: C <- B
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x48
 */
num_cycles LD_C_B(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD C, D
 *
 * Action: C <- D
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x4A
 */
num_cycles LD_C_D(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD C, E
 *
 * Action: C <- E
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x4B
 */
num_cycles LD_C_E(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD C, H
 *
 * Action: C <- H
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x4C
 */
num_cycles LD_C_H(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD C, L
 *
 * Action: C <- L
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode 0x4D
 */
num_cycles LD_C_L(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD D, A
 *
 * Action: D <- A
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x57
 */
num_cycles LD_D_A(gameboy_t* gb, opcode op8); // 0x57

/*
 * Instruction: LD D, B
 *
 * Action: D <- B
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x50
 */
num_cycles LD_D_B(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD D, C
 *
 * Action: D <- C
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x51
 */
num_cycles LD_D_C(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD D, E
 *
 * Action: D <- E
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x53
 */
num_cycles LD_D_E(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD D, H
 *
 * Action: D <- H
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x54
 */
num_cycles LD_D_H(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD D, L
 *
 * Action: D <- L
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode 0x55
 */
num_cycles LD_D_L(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD E, A
 *
 * Action: E <- A
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode 0x5F
 */
num_cycles LD_E_A(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD E, B
 *
 * Action: E <- B
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x58
 */
num_cycles LD_E_B(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD E, C
 *
 * Action: E <- C
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode 0x59
 */
num_cycles LD_E_C(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD E, D
 *
 * Action: E <- D
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x5A
 */
num_cycles LD_E_D(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD E, H
 *
 * Action: E <- H
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x5C
 */
num_cycles LD_E_H(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD E, L
 *
 * Action: E <- L
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x5D
 */
num_cycles LD_E_L(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD H, A
 *
 * Action: H <- A
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x67
 */
num_cycles LD_H_A(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD H, B
 *
 * Action: H <- B
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x60
 */
num_cycles LD_H_B(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD H, C
 *
 * Action: H <- C
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x61
 */
num_cycles LD_H_C(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD H, D
 *
 * Action: H <- D
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x62
 */
num_cycles LD_H_D(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD H, E
 *
 * Action: H <- E
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x63
 */
num_cycles LD_H_E(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD H, L
 *
 * Action: H <- L
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x65
 */
num_cycles LD_H_L(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD L, A
 *
 * Action: L <- A
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x6F
 */
num_cycles LD_L_A(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD L, B
 *
 * Action: L <- B
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x68
 */
num_cycles LD_L_B(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD L, C
 *
 * Action: L <- C
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x69
 */
num_cycles LD_L_C(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD L, D
 *
 * Action: L <- D
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x6A
 */
num_cycles LD_L_D(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD L, E
 *
 * Action: L <- E
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x6B
 */
num_cycles LD_L_E(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD L, H
 *
 * Action: L <- H
 *
 * Flags: None
 *
 * Cycles: 1
 *
 * Opcode: 0x6C
 */
num_cycles LD_L_H(gameboy_t* gb, opcode op8);

/*
 * Instruction   Action     CY    H     N     Z    Cycles     7 6   5 4 3   2 1 0
 * -----------------------------------------------------------------------------------
 *  LD r, n    | r <- n  |  -  |  -  |  -  |  -  |   2    |   0 0     r     1 1 0
 * -----------------------------------------------------------------------------------
 *                                              (next byte)  |        n         |
 */

/*
 * Instruction: LD A, n
 *
 * Action: A <- n
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x3E
 */
num_cycles LD_A_n(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD B, n
 *
 * Action: B <- n
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x06
 */
num_cycles LD_B_n(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD C, n
 *
 * Action: C <- n
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x0E
 */
num_cycles LD_C_n(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD D, n
 *
 * Action: D <- n
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x16
 */
num_cycles LD_D_n(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD E, n
 *
 * Action: E <- n
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x1E
 */
num_cycles LD_E_n(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD H, n
 *
 * Action: H <- n
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x26
 */
num_cycles LD_H_n(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD L, n
 *
 * Action: L <- n
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0xEE
 */
num_cycles LD_L_n(gameboy_t* gb, opcode op8);

/*
 * Instruction   Action        CY    H     N     Z    Cycles     7 6   5 4 3   2 1 0
 * -----------------------------------------------------------------------------------
 *  LD r, (HL)  | r <- (HL) |  -  |  -  |  -  |  -  |   2    |   0 1     r     1 1 0
 * -----------------------------------------------------------------------------------
 */

/*
 * Instruction: LD A, (HL)
 *
 * Action: A <- (HL)
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x7E
 */
num_cycles LD_A_HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD B, (HL)
 *
 * Action: B <- (HL)
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x46
 */
num_cycles LD_B_HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD C, (HL)
 *
 * Action: C <- (HL)
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0xCE
 */
num_cycles LD_C_HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD D, (HL)
 *
 * Action: D <- (HL)
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x56
 */
num_cycles LD_D_HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD E, (HL)
 *
 * Action: D <- (HL)
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x5E
 */
num_cycles LD_E_HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD H, (HL)
 *
 * Action: H <- (HL)
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x66
 */
num_cycles LD_H_HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD L, (HL)
 *
 * Action: L <- (HL)
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x6E
 */
num_cycles LD_L_HL(gameboy_t* gb, opcode op8);

/*
 * Instruction   Action        CY    H     N     Z    Cycles     7 6   5 4 3   2 1 0
 * -----------------------------------------------------------------------------------
 *  LD (HL), r  | (HL) <- r |  -  |  -  |  -  |  -  |   2    |   0 1   1 1 0     r
 * -----------------------------------------------------------------------------------
 */

/*
 * Instruction: LD (HL), A
 *
 * Action: (HL) <- A
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x77
 */
num_cycles LD_HL_A(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (HL), B
 *
 * Action: (HL) <- B
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x70
 */
num_cycles LD_HL_B(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (HL), C
 *
 * Action: (HL) <- C
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x71
 */
num_cycles LD_HL_C(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (HL), D
 *
 * Action: (HL) <- D
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x72
 */
num_cycles LD_HL_D(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (HL), E
 *
 * Action: (HL) <- E
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x73
 */
num_cycles LD_HL_E(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (HL), H
 *
 * Action: (HL) <- H
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x74
 */
num_cycles LD_HL_H(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (HL), L
 *
 * Action: (HL) <- L
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x75
 */
num_cycles LD_HL_L(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (HL), n
 *
 * Action: (HL) <- n
 *
 * Flags: None
 *
 * Cycles: 3
 *
 * Opcode: 0x36
 */
num_cycles LD__HL_n(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD A, (BC)
 *
 * Action: A <- (BC)
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x0A
 */
num_cycles LD_A__BC(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD A, (DE)
 *
 * Action: A <- (DE)
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x1A
 */
num_cycles LD_A__DE(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD A, (C)
 *
 * Action: A <- (0xFF00 + C)
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0xF2
 */
num_cycles LD_A__C(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (C), A
 *
 * Action: (0xFF00 + C) <- A
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0xE2
 */
num_cycles LD__C_A(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD A, (n)
 *
 * Action: A <- (0xFF00 + n)
 *
 * Flags: None
 *
 * Cycles: 3
 *
 * Opcode: 0xF0
 */
num_cycles LD_A__n(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (n), A
 *
 * Action: (0xFF00 + n) <- A
 *
 * Flags: None
 *
 * Cycles: 3
 *
 * Opcode: 0xE0
 */
num_cycles LD__n_A(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD A, (nn)
 *
 * Action: A <- (nn)
 *
 * Flags: None
 *
 * Cycles: 4
 *
 * Opcode: 0xFA
 */
num_cycles LD_A__nn(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (nn), A
 *
 * Action: (nn) <- A
 *
 * Flags: None
 *
 * Cycles: 4
 *
 * Opcode: 0xEA
 */
num_cycles LD__nn_A(gameboy_t* gb, opcode op8); // 0xE

/*
 * Instruction: LD A, (HLI)
 *
 * Action: A <- (HL), HL += 1
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x2A
 */
num_cycles LD_A__HLI(gameboy_t* gb, opcode op8); // 0x26

/*
 * Instruction: LD A, (HLD)
 *
 * Action: A <- (HL), HL -= 1
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x3A
 */
num_cycles LD_A__HLD(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (BC), A
 *
 * Action: (BC) <- A
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x02
 */
num_cycles LD__BC_A(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (DE), A
 *
 * Action: (DE) <- A
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x12
 */
num_cycles LD__DE_A(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (HLI), A
 *
 * Action: (HL) <- A, HL += 1
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x22
 */
num_cycles LD__HLI_A(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD (HLD), A
 *
 * Action: (HL) <- A, HL -= 1
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0x32
 */
num_cycles LD__HLD_A(gameboy_t* gb, opcode op8);

/*
 * 16-bit Transfer Instructions
 */

/*
 * Instruction: LD BC, nn
 *
 * Action: BC <- nn
 *
 * Flags: None
 *
 * Cycles: 3
 *
 * Opcode: 0x01
 */
num_cycles LD_BC_nn(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD DE, nn
 *
 * Action: DE <- nn
 *
 * Flags: None
 *
 * Cycles: 3
 *
 * Opcode: 0x11
 */
num_cycles LD_DE_nn(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD HL, nn
 *
 * Action: HL <- nn
 *
 * Flags: None
 *
 * Cycles: 3
 *
 * Opcode: 0x21
 */
num_cycles LD_HL_nn(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD SP, nn
 *
 * Action: SP <- nn
 *
 * Flags: None
 *
 * Cycles: 3
 *
 * Opcode: 0x31
 */
num_cycles LD_SP_nn(gameboy_t* gb, opcode op8);

/*
 * Instruction: LD SP, HL
 *
 * Action: SP <- HL
 *
 * Flags: None
 *
 * Cycles: 2
 *
 * Opcode: 0xF9
 */
num_cycles LD_SP_HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: PUSH BC
 *
 * Action: SP -= 2, (SP) <- BC
 *
 * Flags: None
 *
 * Cycles: 4
 *
 * Opcode: 0xC5
 */
num_cycles PUSH_BC(gameboy_t* gb, opcode op8);

/*
 * Instruction: PUSH DE
 *
 * Action: SP -= 2, (SP) <- DE
 *
 * Flags: None
 *
 * Cycles: 4
 *
 * Opcode: 0xD5
 */
num_cycles PUSH_DE(gameboy_t* gb, opcode op8);

/*
 * Instruction: PUSH HL
 *
 * Action: SP -= 2, (SP) <- HL
 *
 * Flags: None
 *
 * Cycles: 4
 *
 * Opcode: 0xE5
 */
num_cycles PUSH_HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: PUSH AF
 *
 * Action: SP -= 2, (SP) <- AF
 *
 * Flags: None
 *
 * Cycles: 4
 *
 * Opcode: 0xF5
 */
num_cycles PUSH_AF(gameboy_t* gb, opcode op8);

/*
 * Instruction: POP BC
 *
 * Action: SP += 2, (SP) <- BC
 *
 * Flags: None
 *
 * Cycles: 3
 *
 * Opcode: 0xC1
 */
num_cycles POP_BC(gameboy_t* gb, opcode op8);

/*
 * Instruction: POP DE
 *
 * Action: SP += 2, (SP) <- DE
 *
 * Flags: None
 *
 * Cycles: 3
 *
 * Opcode: 0xD1
 */
num_cycles POP_DE(gameboy_t* gb, opcode op8);

/*
 * Instruction: POP HL
 *
 * Action: SP += 2, (SP) <- HL
 *
 * Flags: None
 *
 * Cycles: 3
 *
 * Opcode: 0xE1
 */
num_cycles POP_HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: POP AF
 *
 * Action: SP += 2, (SP) <- AF
 *
 * Flags: None
 *
 * Cycles: 3
 *
 * Opcode: 0xF1
 */
num_cycles POP_AF(gameboy_t* gb, opcode op8);

/*
 * Instruction: LDHL SP, e
 *
 * Action: HL <- SP + e
 *
 * Flags:
 *
 *        Z - Reset
 *
 *        H - Set if there is a carry from bit 11, 0 otherwise
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 15, 0 otherwise
 *
 * Cycles: 3
 *
 * Opcode: 0xF8
 */
num_cycles LDHL_SP_e(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADD A, B
 *
 * Action: A <- A + B
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x80
 */
num_cycles ADD_A_B(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADD A, C
 *
 * Action: A <- A + C
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x81
 */
num_cycles ADD_A_C(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADD A, D
 *
 * Action: A <- A + D
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x82
 */
num_cycles ADD_A_D(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADD A, E
 *
 * Action: A <- A + E
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x83
 */
num_cycles ADD_A_E(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADD A, H
 *
 * Action: A <- A + H
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x84
 */
num_cycles ADD_A_H(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADD A, L
 *
 * Action: A <- A + L
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x85
 */
num_cycles ADD_A_L(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADD A, n
 *
 * Action: A <- A + n
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 2
 *
 * Opcode: 0xC6
 */
num_cycles ADD_A_n(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADD A, (HL)
 *
 * Action: A <- A + (HL)
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 2
 *
 * Opcode: 0x86
 */
num_cycles ADD_A__HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADC A, B
 *
 * Action: A <- A + B + CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x88
 */
num_cycles ADC_A_B(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADC A, C
 *
 * Action: A <- A + C + CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x89
 */
num_cycles ADC_A_C(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADC A, D
 *
 * Action: A <- A + D + CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x8A
 */
num_cycles ADC_A_D(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADC A, E
 *
 * Action: A <- A + E + CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x8B
 */
num_cycles ADC_A_E(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADC A, H
 *
 * Action: A <- A + H + CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x8C
 */
num_cycles ADC_A_H(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADC A, L
 *
 * Action: A <- A + L + CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x8D
 */
num_cycles ADC_A_L(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADC A, n
 *
 * Action: A <- A + n + CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 2
 *
 * Opcode: 0xCE
 */
num_cycles ADC_A_n(gameboy_t* gb, opcode op8);

/*
 * Instruction: ADC A, (HL)
 *
 * Action: A <- A + (HL) + CY
 *
 * Flags:
 *
 *        Z - Set if result is 0
 *
 *        H - Set if there is a carry from bit 3, otherwise reset
 *
 *        N - Reset
 *
 *        CY - Set if there is a carry from bit 7, otherwise reset
 *
 * Cycles: 2
 *
 * Opcode: 0x8E
 */
num_cycles ADC_A__HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: SUB A, B
 *
 * Action: A <- A - B
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x90
 */
num_cycles SUB_A_B(gameboy_t* gb, opcode op9);

/*
 * Instruction: SUB A, C
 *
 * Action: A <- A - C
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x91
 */
num_cycles SUB_A_C(gameboy_t* gb, opcode op9);

/*
 * Instruction: SUB A, D
 *
 * Action: A <- A - D
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x92
 */
num_cycles SUB_A_D(gameboy_t* gb, opcode op9);

/*
 * Instruction: SUB A, E
 *
 * Action: A <- A - E
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x93
 */
num_cycles SUB_A_E(gameboy_t* gb, opcode op9);

/*
 * Instruction: SUB A, H
 *
 * Action: A <- A - H
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x94
 */
num_cycles SUB_A_H(gameboy_t* gb, opcode op9);

/*
 * Instruction: SUB A, L
 *
 * Action: A <- A - L
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x95
 */
num_cycles SUB_A_L(gameboy_t* gb, opcode op9);

/*
 * Instruction: SUB A, n
 *
 * Action: A <- A - n
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 2
 *
 * Opcode: 0xD6
 */
num_cycles SUB_A_n(gameboy_t* gb, opcode op8);

/*
 * Instruction: SUB A, (HL)
 *
 * Action: A <- A - (HL)
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 2
 *
 * Opcode: 0x96
 */
num_cycles SUB_A__HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: SBC A, B
 *
 * Action: A <- A - B - CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x98
 */
num_cycles SBC_A_B(gameboy_t* gb, opcode op9);

/*
 * Instruction: SBC A, C
 *
 * Action: A <- A - C - CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x99
 */
num_cycles SBC_A_C(gameboy_t* gb, opcode op9);

/*
 * Instruction: SBC A, D
 *
 * Action: A <- A - D - CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x9A
 */
num_cycles SBC_A_D(gameboy_t* gb, opcode op9);

/*
 * Instruction: SBC A, E
 *
 * Action: A <- A - E - CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x9B
 */
num_cycles SBC_A_E(gameboy_t* gb, opcode op9);

/*
 * Instruction: SBC A, H
 *
 * Action: A <- A - H - CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x9C
 */
num_cycles SBC_A_H(gameboy_t* gb, opcode op9);

/*
 * Instruction: SBC A, L
 *
 * Action: A <- A - L - CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 1
 *
 * Opcode: 0x9D
 */
num_cycles SBC_A_L(gameboy_t* gb, opcode op8);

/*
 * Instruction: SBC A, n
 *
 * Action: A <- A - n - CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 2
 *
 * Opcode: 0xDE
 */
num_cycles SBC_A_n(gameboy_t* gb, opcode op8);

/*
 * Instruction: SBC A, (HL)
 *
 * Action: A <- A - (HL) - CY
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set if there is a borrow from bit 4, otherwise reset
 *
 *        N - Set
 *
 *        CY - Set if there is a borrow, otherwise reset
 *
 * Cycles: 2
 *
 * Opcode: 0x9E
 */
num_cycles SBC_A__HL(gameboy_t* gb, opcode op8);

/*
 * Instruction: AND B
 *
 * Action: A <- A & B
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set
 *
 *        N - Reset
 *
 *        CY - Reset
 *
 * Cycles: 1
 *
 * Opcode: 0xA0
 */
num_cycles AND_B(gameboy_t* gb, opcode op8);

/*
 * Instruction: AND C
 *
 * Action: A <- A & C
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set
 *
 *        N - Reset
 *
 *        CY - Reset
 *
 * Cycles: 1
 *
 * Opcode: 0xA1
 */
num_cycles AND_C(gameboy_t* gb, opcode op8);

/*
 * Instruction: AND D
 *
 * Action: A <- A & D
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set
 *
 *        N - Reset
 *
 *        CY - Reset
 *
 * Cycles: 1
 *
 * Opcode: 0xA2
 */
num_cycles AND_D(gameboy_t* gb, opcode op8);

/*
 * Instruction: AND E
 *
 * Action: A <- A & E
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set
 *
 *        N - Reset
 *
 *        CY - Reset
 *
 * Cycles: 1
 *
 * Opcode: 0xA3
 */
num_cycles AND_E(gameboy_t* gb, opcode op8);

/*
 * Instruction: AND H
 *
 * Action: A <- A & H
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set
 *
 *        N - Reset
 *
 *        CY - Reset
 *
 * Cycles: 1
 *
 * Opcode: 0xA4
 */
num_cycles AND_H(gameboy_t* gb, opcode op8);

/*
 * Instruction: AND L
 *
 * Action: A <- A & L
 *
 * Flags:
 *
 *        Z - Set if result is 0; otherwise reset
 *
 *        H - Set
 *
 *        N - Reset
 *
 *        CY - Reset
 *
 * Cycles: 1
 *
 * Opcode: 0xA5
 */
num_cycles AND_L(gameboy_t* gb, opcode op8);

#endif // !DEBUG
