// mlvm
// Copyright (c) 2025 Madrona Labs LLC. http://www.madronalabs.com

#pragma once

#include "madronalib.h"

namespace ml {

constexpr int kNumVMParameters{128};
constexpr int kNumRegisters{128};

enum opcodes {
  NOOP = 0,
  LOAD,
  STORE,
  ADD,
  MUL
  // ... and many more
};

enum addressModes {
  REGISTER = 0,
  IMMEDIATE,
  CONTEXT,
  LITERAL
};

// NOTES:
// byte 0: opcode  (bit 7: k-rate version?) (condition register?)
// byte 1: src1
// byte 2: src2 register idx
// byte 3: dest register idx
// bytes 4-7: state pointer in bytes from state arena start, or immediate float32 value, or other operand registers
//
// address modes: for src and dest: immediate, registers, arena. for state: registers, arena.
// inputs / outputs address mode? - a single bit can mean use inputs if loading, outputs if storing.
//

// operand: [2 bits memFlags, 6 bits index]
using Operand = uint8_t;
int getAddressMode(Operand m) { return m&0xC0; }
int getIndex(Operand m) { return m&0x3F; }

struct Instruction {
  uint8_t opcode;
  Operand dest;
  Operand src1;
  Operand src2;
};

// when loading floats, we can all 16 bits of the src operands as an immediate float value.
// floats can be loaded to DSPVectors by filling, ramping, set1, setn...

static_assert(sizeof(Instruction) == 4);

struct MemoryRequirements {
  size_t scratchVectors;
  size_t persistentVectors;
};

struct Program {
  std::vector< Instruction > instructions;
  MemoryRequirements memReqs;
};


// modules notes:
// modules have
// class name
// description
// n inputs: (name, # signals, description)
// n outputs: (name, # signals, description)
// n arguments: (name, default value)
// program: make_opcodes(inputs: module arguments -> outputs: opcodes)
//    this can be C++ to start but use Lua ASAP, allowing module changes w/o C++ compile
//
// A benefit from compiling the module into opcodes is that we take care of any mode-switch
// decisions at opcode-making ("compile") time.
// Filter -> West Coast, East Coast (different personalities)
// projection -> linear, quadratic, log, exp, ...
// Distortion flavors, Compression flavors
// body types, Space modes, ...
// If modules share an interface and basic concept they can be rolled into one with a compile mode
//
// for crossfades on changes and super-quick undo, we can keep N versions of the program.

struct MLVMState {
  
};


struct MLVM {
  std::vector< DSPVector > registers;
  std::vector< DSPVector > persistentMem;
  Program program;
  uint32_t programCounter;
  
  //void compile(const JSON& dspGraphInput, Program& programOutput); // TODO - takes JSON list of modules and connections, makes opcodes and memory needs
  
  bool allocateMemory(const MemoryRequirements&);
  void setProgram(const Program& newCode);
  void process(AudioContext* context);
  
private:
  
  DSPVector* getSrcArgPtr(MemoryLocation arg);
};

} // namespace ml

