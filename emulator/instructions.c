#include "gameboy.h"

num_cycles LD_A_B(gameboy_t* gb, opcode op8) {
  A = B;
  return 1;
}

num_cycles LD_A_C(gameboy_t* gb, opcode op8) {
  A = C;
  return 1;
}

num_cycles LD_A_D(gameboy_t* gb, opcode op8) {
  A = D;
  return 1;
}

num_cycles LD_A_E(gameboy_t* gb, opcode op8) {
  A = E;
  return 1;
}

num_cycles LD_A_H(gameboy_t* gb, opcode op8) {
  A = H;
  return 1;
}

num_cycles LD_A_L(gameboy_t* gb, opcode op8) {
  A = L;
  return 1;
}

num_cycles LD_B_A(gameboy_t* gb, opcode op8) {
  B = A;
  return 1;
}

num_cycles LD_B_C(gameboy_t* gb, opcode op8) {
  B = C;
  return 1;
}

num_cycles LD_B_D(gameboy_t* gb, opcode op8) {
  B = D;
  return 1;
}

num_cycles LD_B_E(gameboy_t* gb, opcode op8) {
  B = E;
  return 1;
}

num_cycles LD_B_H(gameboy_t* gb, opcode op8) {
  B = H;
  return 1;
}

num_cycles LD_B_L(gameboy_t* gb, opcode op8) {
  B = L;
  return 1;
}

num_cycles LD_C_A(gameboy_t* gb, opcode op8) {
  C = A;
  return 1;
}

num_cycles LD_C_B(gameboy_t* gb, opcode op8) {
  C = B;
  return 1;
}

num_cycles LD_C_D(gameboy_t* gb, opcode op8) {
  C = D;
  return 1;
}

num_cycles LD_C_E(gameboy_t* gb, opcode op8) {
  C = E;
  return 1;
}

num_cycles LD_C_H(gameboy_t* gb, opcode op8) {
  C = H;
  return 1;
}

num_cycles LD_C_L(gameboy_t* gb, opcode op8) {
  C = L;
  return 1;
}

num_cycles LD_D_A(gameboy_t* gb, opcode op8) {
  D = A;
  return 1;
}

num_cycles LD_D_B(gameboy_t* gb, opcode op8) {
  D = B;
  return 1;
}

num_cycles LD_D_C(gameboy_t* gb, opcode op8) {
  D = C;
  return 1;
}

num_cycles LD_D_E(gameboy_t* gb, opcode op8) {
  D = E;
  return 1;
}

num_cycles LD_D_H(gameboy_t* gb, opcode op8) {
  D = H;
  return 1;
}

num_cycles LD_D_L(gameboy_t* gb, opcode op8) {
  D = L;
  return 1;
}

num_cycles LD_E_A(gameboy_t* gb, opcode op8) {
  E = A;
  return 1;
}

num_cycles LD_E_B(gameboy_t* gb, opcode op8) {
  E = B;
  return 1;
}

num_cycles LD_E_C(gameboy_t* gb, opcode op8) {
  E = C;
  return 1;
}

num_cycles LD_E_D(gameboy_t* gb, opcode op8) {
  E = D;
  return 1;
}

num_cycles LD_E_H(gameboy_t* gb, opcode op8) {
  E = H;
  return 1;
}

num_cycles LD_E_L(gameboy_t* gb, opcode op8) {
  E = L;
  return 1;
}
num_cycles LD_H_A(gameboy_t* gb, opcode op8) {
  H = A;
  return 1;
}

num_cycles LD_H_B(gameboy_t* gb, opcode op8) {
  H = B;
  return 1;
}

num_cycles LD_H_C(gameboy_t* gb, opcode op8) {
  H = C;
  return 1;
}

num_cycles LD_H_D(gameboy_t* gb, opcode op8) {
  H = D;
  return 1;
}

num_cycles LD_H_E(gameboy_t* gb, opcode op8) {
  H = E;
  return 1;
}

num_cycles LD_H_L(gameboy_t* gb, opcode op8) {
  H = L;
  return 1;
}

num_cycles LD_L_A(gameboy_t* gb, opcode op8) {
  L = A;
  return 1;
}

num_cycles LD_L_B(gameboy_t* gb, opcode op8) {
  L = B;
  return 1;
}

num_cycles LD_L_C(gameboy_t* gb, opcode op8) {
  L = C;
  return 1;
}

num_cycles LD_L_D(gameboy_t* gb, opcode op8) {
  L = D;
  return 1;
}

num_cycles LD_L_E(gameboy_t* gb, opcode op8) {
  L = E;
  return 1;
}

num_cycles LD_L_H(gameboy_t* gb, opcode op8) {
  L = H;
  return 1;
}

num_cycles LD_A_n(gameboy_t* gb, opcode op8) {
  //
  return 2;
}

num_cycles LD_B_n(gameboy_t* gb, opcode op8) {
  //
  return 2;
}
num_cycles LD_C_n(gameboy_t* gb, opcode op8);
num_cycles LD_D_n(gameboy_t* gb, opcode op8);
num_cycles LD_E_n(gameboy_t* gb, opcode op8);
num_cycles LD_H_n(gameboy_t* gb, opcode op8);
num_cycles LD_L_n(gameboy_t* gb, opcode op8);

num_cycles LD_H_HL(gameboy_t* gb, opcode op8);
num_cycles LD_A_HL(gameboy_t* gb, opcode op8);
num_cycles LD_B_HL(gameboy_t* gb, opcode op8);
num_cycles LD_C_HL(gameboy_t* gb, opcode op8);
num_cycles LD_D_HL(gameboy_t* gb, opcode op8);
num_cycles LD_E_HL(gameboy_t* gb, opcode op8);
num_cycles LD_H_HL(gameboy_t* gb, opcode op8);
num_cycles LD_L_HL(gameboy_t* gb, opcode op8);

num_cycles LD_HL_A(gameboy_t* gb, opcode op8);
num_cycles LD_HL_B(gameboy_t* gb, opcode op8);
num_cycles LD_HL_C(gameboy_t* gb, opcode op8);
num_cycles LD_HL_D(gameboy_t* gb, opcode op8);
num_cycles LD_HL_E(gameboy_t* gb, opcode op8);
num_cycles LD_HL_H(gameboy_t* gb, opcode op8);
num_cycles LD_HL_L(gameboy_t* gb, opcode op8);

num_cycles PUSH_BC(gameboy_t* gb, opcode op8);
num_cycles PUSH_DE(gameboy_t* gb, opcode op8);
num_cycles PUSH_HL(gameboy_t* gb, opcode op8);
num_cycles PUSH_AF(gameboy_t* gb, opcode op8);

num_cycles POP_BC(gameboy_t* gb, opcode op8);
num_cycles POP_DE(gameboy_t* gb, opcode op8);
num_cycles POP_HL(gameboy_t* gb, opcode op8);
num_cycles POP_AF(gameboy_t* gb, opcode op8);

num_cycles LDHL_SP_e(gameboy_t* gb, opcode op8);

num_cycles LD__nn_SP(gameboy_t* gb, opcode op8);

num_cycles ADD_A_B(gameboy_t* gb, opcode op8);
num_cycles ADD_A_C(gameboy_t* gb, opcode op8);
num_cycles ADD_A_D(gameboy_t* gb, opcode op8);
num_cycles ADD_A_E(gameboy_t* gb, opcode op8);
num_cycles ADD_A_H(gameboy_t* gb, opcode op8);
num_cycles ADD_A_L(gameboy_t* gb, opcode op8);
num_cycles ADD_A_n(gameboy_t* gb, opcode op8);
num_cycles ADD_A__HL(gameboy_t* gb, opcode op8);

num_cycles LD__HL_n(gameboy_t* gb, opcode op8) {
  memory_at mdest = MEMORY_AT(HL);
  mdest = N;
  PC += sizeof(N);
  return 3;
}

num_cycles LD_A__BC(gameboy_t* gb, opcode op8) {
  A = MEMORY_AT(BC);
  return 2;
}

num_cycles LD_A__DE(gameboy_t* gb, opcode op8) {
  A = MEMORY_AT(DE);
  return 2;
}

num_cycles LD_A__C(gameboy_t* gb, opcode op8) {
  A = MEMORY_AT(0xFF00 + C);
  return 2;
}

num_cycles LD__C_A(gameboy_t* gb, opcode op8) {
  MEMORY_AT(0xFF00 + C) = A;
  return 2;
}

num_cycles LD_A__n(gameboy_t* gb, opcode op8) {
  A = MEMORY_AT(0xFF00 + N);
  PC += sizeof(N);
  return 3;
}

num_cycles LD__n_A(gameboy_t* gb, opcode op8) {
  MEMORY_AT(0xFF00 + N) = A;
  PC += sizeof(N);
  return 3;
}

num_cycles LD_A__nn(gameboy_t* gb, opcode op8) {
  A = MEMORY_AT(NN);
  PC += sizeof(NN);

  return 4;
}

num_cycles LD__nn_A(gameboy_t* gb, opcode op8) {
  MEMORY_AT(NN) = A;
  PC += sizeof(NN);

  return 4;
}

num_cycles LD_A__HLI(gameboy_t* gb, opcode op8) {
  A = MEMORY_AT(HL);
  HL += 1;
  return 2;
}

num_cycles LD_A__HLD(gameboy_t* gb, opcode op8) {
  A = MEMORY_AT(HL);
  HL -= 1;
  return 2;
}

num_cycles LD__BC_A(gameboy_t* gb, opcode op8) {
  MEMORY_AT(BC) = A;
  return 2;
}

num_cycles LD__DE_A(gameboy_t* gb, opcode op8) {
  MEMORY_AT(DE) = A;
  return 2;
}

num_cycles LD__HLI_A(gameboy_t* gb, opcode op8) {
  MEMORY_AT(HL) = A;
  HL += 1;
  return 2;
}

num_cycles LD_HLI_D(gameboy_t* gb, opcode op8) {
  MEMORY_AT(HL) = A;
  HL -= 1;
  return 2;
}

num_cycles LD_BC_nn(gameboy_t* gb, opcode op8) {
  BC = NN;
  return 3;
}

num_cycles LD_DE_nn(gameboy_t* gb, opcode op8) {
  DE = NN;
  return 3;
}

num_cycles LD_HL_nn(gameboy_t* gb, opcode op8) {
  HL = NN;
  return 3;
}

num_cycles LD_SP_nn(gameboy_t* gb, opcode op8) {
  SP = NN;
  return 3;
}

num_cycles LD_SP_HL(gameboy_t* gb, opcode op8) {
  SP = HL;
  return 2;
}

num_cycles PUSH_BC(gameboy_t* gb, opcode op8) {
  SP -= 2;
  *(uint16_t*)&MEMORY_AT(SP) = BC;
  return 4;
}

num_cycles PUSH_DE(gameboy_t* gb, opcode op8) {
  SP -= 2;
  *(uint16_t*)&MEMORY_AT(SP) = DE;
  return 4;
}

num_cycles PUSH_HL(gameboy_t* gb, opcode op8) {
  SP -= 2;
  *(uint16_t*)&MEMORY_AT(SP) = HL;
  return 4;
}

num_cycles PUSH_AF(gameboy_t* gb, opcode op8) {
  SP -= 2;
  *(uint16_t*)&MEMORY_AT(SP) = AF;
  return 4;
}
