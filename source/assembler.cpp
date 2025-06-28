//
//  assembler.cpp
//  mlvm
//
//  Created by Randy Jones on 6/27/25.
//

#include "assembler.h"

namespace mlvm {

Program ToyAssembler::assemble(const std::string& assemblyCode) {
  Program program;
  program.memReqs = {0, 0}; // Default memory requirements
  literalPool.clear();
  
  std::stringstream ss(assemblyCode);
  std::string line;
  
  while (std::getline(ss, line)) {
    line = trim(line);
    
    // Skip empty lines and comments
    if (line.empty() || line[0] == ';' || line.substr(0, 2) == "//") {
      continue;
    }
    
    std::vector<std::string> tokens = tokenize(line);
    if (tokens.empty()) continue;
    
    // Parse instruction
    std::string opName = tokens[0];
    std::transform(opName.begin(), opName.end(), opName.begin(), ::toupper);
    
    if (opMap.find(opName) == opMap.end()) {
      std::cerr << "Unknown operation: " << opName << std::endl;
      continue;
    }
    
    Instruction instr = {};
    instr.opcode = opMap[opName]; // MODE_0 by default
    
    // Parse operands based on instruction type
    if (tokens.size() >= 2) {
      instr.dest = createOperand(tokens[1]);
    }
    if (tokens.size() >= 3) {
      instr.src1 = createOperand(tokens[2]);
    }
    if (tokens.size() >= 4) {
      instr.src2 = createOperand(tokens[3]);
    }
    
    program.instructions.push_back(instr);
  }
  
  return program;
}

void ToyAssembler::printProgram(const Program& program) {
  std::cout << "Program with " << program.instructions.size() << " instructions:\n";
  
  for (size_t i = 0; i < program.instructions.size(); ++i) {
    const auto& instr = program.instructions[i];
    std::cout << i << ": ";
    std::cout << "Op=" << (int)getOperation(instr.opcode) << " ";
    std::cout << "Dest=" << (int)getIndex(instr.dest) << "(" << (int)getOperandMode(instr.dest) << ") ";
    std::cout << "Src1=" << (int)getIndex(instr.src1) << "(" << (int)getOperandMode(instr.src1) << ") ";
    std::cout << "Src2=" << (int)getIndex(instr.src2) << "(" << (int)getOperandMode(instr.src2) << ")\n";
  }
  
  if (!literalPool.empty()) {
    std::cout << "\nLiteral Pool:\n";
    for (size_t i = 0; i < literalPool.size(); ++i) {
      std::cout << i << ": " << literalPool[i] << "\n";
    }
  }
}}
