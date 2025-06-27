// mlvm
// Copyright (c) 2025 Madrona Labs LLC. http://www.madronalabs.com

#include "mlvm.h"

namespace ml {

bool MLVM::allocateMemory(const MemoryRequirements& memReqs) {
  // TODO errors
  registers.resize(kNumRegisters);
  
  // TODO errors
  persistentMem.resize(memReqs.persistentVectors);
  return true;
}

void MLVM::setProgram(const Program& newCode) {
  program = newCode;
}

inline DSPVector* MLVM::getSrcPtr(Operand arg) {
  bool f = getMemFlag(arg);
  if(f == REGISTER)
  {
    return registers.data() + getIndex(arg);
  }
  else // OTHER
  {
    
  }
}

void MLVM::process(AudioContext* context) {
  
  // TEMP
  static int testCounter{0};
  
  // main inputs / outputs are dynamic, so check them
  if (context->outputs.size() < 1) return;
  if (context->inputs.size() > 0)
  {
    // TODO copy inputs to registers
  }
  
  // the innermost loop that interprets the bytecode program.
  
  // todo - name a couple of opcodes, do an immediate mode calc
  // todo - get header into project!
  
  for (auto& instruction : program.instructions) {
    
    switch (instruction.opcode) {
      case NOOP:
        break;
      case LOAD:
        auto src = getSrcArgPtr
        context->outputs[0] = DSPVector(0.0023f);
        break;
      case ADD:
        context->outputs[0] += DSPVector(2.f);
        break;
      case MUL:
        context->outputs[0] *= DSPVector(2.f);
        break;
    }
  }
  
  // TODO
  // copy registers to outputs
  
  // TEMP
  const int nSamples = context->getSampleRate();
  auto timeInfo = context->getTimeInfo();
  testCounter += kFloatsPerDSPVector;
  if (testCounter >= nSamples) {
    testCounter -= nSamples;
    std::cout << "bpm:" << timeInfo.bpm << "\n";
    std::cout << "phase:" << timeInfo._quarterNotesPhase[0] << "\n";
    std::cout << "samples since start:" << timeInfo.samplesSinceStart << "\n";
    std::cout << "output 0-0:" << context->outputs[0][0] << "\n";
  }
}

} // namespace ml

  
