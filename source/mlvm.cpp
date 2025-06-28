// mlvm
// Copyright (c) 2025 Madrona Labs LLC. http://www.madronalabs.com

#include "mlvm.h"

namespace mlvm {

bool MLVM::allocateMemory(const MemoryRequirements& memReqs) {
  // TODO errors
  registers.resize(kNumRegisters);
  
  // TODO errors
  arena.resize(memReqs.persistentVectors);
  return true;
}

void MLVM::setProgram(const Program& newCode) {
  program = newCode;
}

DSPVector MLVM::getSrcOperandValue(Operand op)
{
  if(getOperandMode(op) == IMMEDIATE)
  {
    // fill source vector with immediate float value
    // NOTE: for ops like set1 where we only use one float
    // we want to not do this fill in the future
    return DSPVector(getImmediate(op));
  }
  else
  {
    return registers[getIndex(op)];
  }
}

void MLVM::process(AudioContext* context) {
  size_t destIdx, srcIdx1, srcIdx2;
  DSPVector src1, src2;
  
  // TEMP
  static int testCounter{0};
  
  // main inputs / outputs are dynamic, so check them
  if (context->outputs.size() < 1) return;
  
  // copy inputs to registers
  for(int i=0; i<context->inputs.size(); ++i)
  {
    registers[i] = context->inputs[i];
  }

  // Here is the innermost loop that interprets the bytecode program.
  // The program will generate one vector of output.
  // NOTE: Aside from the main switch, there should be few if any branches.
  
  programCounter = 0;
  while(1) {
    auto inst = program.instructions[programCounter++];
    destIdx = getIndex(inst.dest);

    src1 = getSrcOperandValue(inst.src1);
    src2 = getSrcOperandValue(inst.src2);

    switch (inst.opcode) {
      case NOOP:
        break;
      case MOVE:
        registers[destIdx] = src1;
        break;
      case ADD:
        registers[destIdx] = add(src1, src2);
        break;
      case MUL:
        registers[destIdx] = multiply(src1, src2);
        break;
      case END:
        goto endprogram;
    }
  }

  endprogram:
  
  // copy registers to outputs
  for(int i=0; i<context->outputs.size(); ++i)
  {
    context->outputs[i] = registers[i];
  }

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

  
