#include "llvm/Pass.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Use.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <iostream>
#include <set>
#include <stack>
#include <string>

using namespace llvm;
cl::opt<std::string>
    TargetFunc("target_function",
               cl::desc("target function of contion collection"),
               cl::value_desc("function name"));
namespace {
void report_dbginfo(BasicBlock *bb) {
  if (!bb) {
    return;
  }
  // bb->print(dbgs());
  // dbgs() << "\n";
  Instruction *termInstr = bb->getTerminator();
  if (!termInstr) {
    return;
  }
  const DebugLoc &debugloc = termInstr->getDebugLoc();
  debugloc.print(dbgs());
  dbgs() << "\n";
}
void report_condition(BasicBlock *bb, unsigned int branch,
                      std::string prefix = "[++]") {
  BranchInst *bi = dyn_cast<BranchInst>(bb->getTerminator());
  if (!bi || bi->isConditional() == false)
    return;
  dbgs() << prefix;
  bi->getDebugLoc().print(dbgs());
  bi->getCondition()->print(dbgs());
  dbgs() << ", branchs = " << branch << "\n";
}
class BasicBlockSet {
public:
  std::set<std::string> _set;
  BasicBlockSet() : _set() {}
  void insert(BasicBlock *bb) {
    if (!bb) {
      return;
    }
    std::string bbstring = getBBString(bb);
    _set.insert(bbstring);
  }
  size_t count(BasicBlock *bb) {
    std::string bbstring = getBBString(bb);
    return _set.count(bbstring);
  }
  std::string getBBString(BasicBlock *bb) {
    std::string bbstring;
    raw_string_ostream ostream_string(bbstring);
    bb->print(ostream_string);
    return ostream_string.str();
  }
};

class BasicBlockStack {
public:
  std::stack<BasicBlock *> _stack;
  BasicBlockStack() : _stack() {}
  void push(BasicBlock *bb) {
    if (bb) {
      _stack.push(bb);
    }
  }
  void pop() { _stack.pop(); }
  BasicBlock *top() {
    if (_stack.empty()) {
      return nullptr;
    }
    return _stack.top();
  }
  void report() {
    while (!_stack.empty()) {
      BasicBlock *bb = top();
      report_dbginfo(bb);
      pop();
    }
  }
};

class conditionCollect : public ModulePass {
private:
  bool isLoopBlock(BasicBlock *bb) { return false; }
  int isConditionBlock(BasicBlock *bb) {
    BranchInst *bi = dyn_cast<BranchInst>(bb->getTerminator());
    if (!bi)
      return -1;
    if (bi->isConditional()) {
      // Value *Cond = bi->getCondition();
      for (unsigned int i = 0; i != bi->getNumSuccessors(); i++) {
        BasicBlock *succ = bi->getSuccessor(i);
        if (reached.count(succ) != 0) {
          return i;
        }
      }
    }
    return -1;
  }

public:
  static char ID;
  Function *target_function;
  BasicBlockSet reached;
  // BasicBlockSet conditionBlocks;
  BasicBlockStack conditionBlocks;
  conditionCollect() : ModulePass(ID), reached(), conditionBlocks() {}
  void foreach_on_reverse_cfg(BasicBlock *bb) {
    if (!bb || reached.count(bb) != 0) {
      return;
    }

    reached.insert(bb);
    unsigned int branch = isConditionBlock(bb);
    if (branch >= 0) {
      conditionBlocks.push(bb);
      // report_dbginfo(bb);
      // report_condition(bb, branch);
      dbgs() << demangle(bb->getParent()->getName().data())<<"\n";
    }

    /*Recursively traverse the cfg, start from bb, reversly*/
    if (pred_size(bb) == 0 && bb->isEntryBlock() == true) {
      Function *func = bb->getParent();
      for (auto ui = func->use_begin(), ue = func->use_end(); ui != ue; ui++) {
        Instruction *ins = dyn_cast<Instruction>(ui->getUser());
        if (!ins)
          continue;
        BasicBlock *higher_level_bb = ins->getParent();
        foreach_on_reverse_cfg(higher_level_bb);
      }
    }
    for (auto bi = pred_begin(bb), be = pred_end(bb); bi != be; bi++) {
      foreach_on_reverse_cfg(*bi);
    }
  }

  bool runOnModule(Module &M) {
    /*Get the target function by name*/
    target_function = M.getFunction(TargetFunc);
    dbgs() << "\n[+] Target function is : " << demangle(TargetFunc) << "\n\n";
    dbgs() << "[+] condition tracking : \n\n";
    BasicBlock &bb = target_function->getEntryBlock();
    foreach_on_reverse_cfg(&bb);
    return false;
  }
};
char conditionCollect::ID = 0;
RegisterPass<conditionCollect>
    X("conditionCollect",
      "Collect conditions along the path to a callsite of a function f",
      false /* Modifies CFG */, false /* Non Analysis Pass */);
} // namespace
