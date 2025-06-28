// madronalib: a C++ framework for DSP applications.
// Copyright (c) 2020-2022 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

// example of RtAudio wrapping low-level madronalib DSP code.

#include <iostream>
#include "madronalib.h"
#include "mlvm.h"
#include "assembler.h"

using namespace mlvm;

// TEMP
constexpr int kInputChannels = 0;
constexpr int kOutputChannels = 2;
constexpr int kSampleRate = 48000;
constexpr float kOutputGain = 0.1f;

struct VMExampleState {
  MLVM* vm{nullptr};
  EventsToSignals* eventsToSignals{nullptr};
};

// processAudio() does all of the audio processing, in DSPVector-sized chunks.
// It is called every time a new buffer of audio is needed.
void processAudio(AudioContext* ctx, void *state)
{
  // at the beginning of the main process function we need to cast the void* to
  // the type of our state. Making AudioTask a template would have been an alternative
  // to this but would have added a lot of template code behind the scenes.
  auto procState = static_cast<VMExampleState*>(state);
  
  MLVM* vm = procState->vm;
  EventsToSignals* evToSigs = procState->eventsToSignals;

  int startOffset(0);
  evToSigs->processVector(startOffset);
  
  // run vm
  vm->process(ctx);
}

int main( int argc, char *argv[] )
{
  EventsToSignals eventsToSignals;
  eventsToSignals.setSampleRate(kSampleRate);

  const auto& handleMsg = [&](MIDIMessage m)->void
  {
    std::cout << "handleMsg got " << m.size() << "bytes: ";
    for ( int i=0; i<m.size(); i++ )
      std::cout << (int)m[i] << " ";
    std::cout << "\n";

    eventsToSignals.addEvent(MIDIMessageToEvent(m));
  };

  MIDIInput midiInput;
  if (!midiInput.start(handleMsg)) {
    std::cout << "couldn't start MIDI input!\n";
    return 0;
  }

  // start the Timers. call this once in an application.
  bool deferToMainThread = false;
  SharedResourcePointer<ml::Timers> t;
  t->start(deferToMainThread);

  // setup the vm
  MLVM vm;

  // TODO compile the graph of processors, getting memory needs
  // auto testGraph = readJSONGraph;
  // vm.compile(testGraph, testProgram, memoryNeeds);

  // but right now we can use a toy assembler
   ToyAssembler assembler;
   
   std::string testCode = R"(
   MOV R1, #5          ; Move immediate 5 to R1
   ADD R2, R1, #10     ; Add R1 + 10, store in R2
   LDR R3, =3.14159    ; Load literal from pool into R3
   STR R2, [R4]        ; Store R2 to memory at R4
   CMP R1, R2          ; Compare R1 with R2
   BNE R0              ; Branch if not equal
   MUL R5, R2, R3      ; Multiply R2 * R3, store in R5 
   END
   )";
   
   Program testProgram = assembler.assemble(testCode);
   assembler.printProgram(testProgram);
  
  // TEMP
  return;
   
  

  // TEMP allocate program memory and set opcodes explicitly
  const int kScratchBytes{1024};
  const int kPersistentBytes{1024};
  const int kReadOnlyBytes{1024};
  vm.allocateMemory(MemoryRequirements{ kScratchBytes, kPersistentBytes });
  vm.setProgram(testProgram);


  // fill a struct with the data the callback will need to create a context.
  VMExampleState state{&vm, &eventsToSignals};
  
  AudioContext ctx(kInputChannels, kOutputChannels, kSampleRate);
  AudioTask exampleTask(&ctx, processAudio, &state);
  
  ctx.updateTime(0, 120.0, true, kSampleRate);

  // run the audio task
  return exampleTask.runConsoleApp();
  
  /*
  
  // TODO don't require setting sample rate twice
  vmExampleProc._currentTime.setTimeAndRate(0, 120.0, true, kSampleRate);
  vmExampleProc.startAudio();

  while(true) {
    std::this_thread::sleep_for(milliseconds(2000));
    std::cout << "samplesSinceStart: " << vmExampleProc._currentTime.samplesSinceStart << "\n";
  }
  
  return 0;
   
   */
  
}
