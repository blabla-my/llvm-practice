#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <iostream>
#include <set>
#include <string>

using namespace llvm;

namespace {
class CallgraphTraverse : public ModulePass {
public:
  static char ID;
  CallgraphTraverse() : ModulePass(ID) {}
  bool runOnModule(Module &M) {
    CallGraph CG = CallGraph(M);

    for (auto it = df_begin(&CG), ei = df_end(&CG); it != ei; ++it) {
      if (Function *F = it->getFunction()) {
        dbgs() << "Visiting function: " << F->getName().data() << "\n";
      }
    }
    return false;
  }
};
char CallgraphTraverse::ID = 0;
RegisterPass<CallgraphTraverse> X("traverseCallgraph",
                                  "traverse the callgraph of given bit code",
                                  false /* Modifies CFG */,
                                  false /* Non Analysis Pass */);
} // namespace
