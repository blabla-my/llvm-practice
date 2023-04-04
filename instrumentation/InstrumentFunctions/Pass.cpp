#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include "llvm/Pass.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/Analysis/TargetLibraryInfo.h"

using namespace llvm;

// cl::list<std::string> FunctionList(
//         "instrument",
//         cl::desc("Functions to instrument"),
//         cl::value_desc("function name"),
//         cl::OneOrMore);
cl::opt<std::string> FunctionList(
          "instrument", 
          cl::desc("Functions to instrument"), 
          cl::value_desc("filename")
); 
namespace {
    class InstrumentFunctions : public ModulePass {
        public:

            // name of instrumentation functions
            const char *LOG_FUNCTION_STR = "log_function_call";
            const char *INIT_FUNCTION_STR = "init";
            /* const char *LOG_VARIABLE_STR = "log_variable_change"; */

            static char ID;

            std::set<std::string> funcsToInst;

            InstrumentFunctions() : ModulePass(ID) {
                std::ifstream functionsFile(FunctionList);
                std::cerr << "Initializing InstrumentFunctions" << std::endl ;
                std::string funcname;
                while(std::getline(functionsFile, funcname)){
                    funcsToInst.insert(funcname);
                }
            }

            void showSymbolTable(Module &M) {
                ValueSymbolTable symtab = M.getValueSymbolTable();
                for (auto k=symtab.begin(); k!=symtab.end(); ++k) {
                    std::cerr << (*k).first().data() << std::endl; 
                    // for (auto e=(*k).second().begin(); e!=(*k).second().end(); ++e) {
                    //    std::cerr << (*e).first().data() << std::endl();
                    // }
                    std::cerr << (*k).second << std::endl; 
                }
            }

            bool runOnModule(Module &M) override {
                declare_log_functions(M);
                // const TargetLibraryInfo *TLI; 
                // showSymbolTable(M); 
                for (Module::iterator mi = M.begin(); mi != M.end(); ++mi) {;
                    Function &f = *mi;

                    std::string fname = std::string(f.getName().data());
                    std::cerr << "Handling : " << fname << std::endl;
                    std::string section_prefix;
                    if (fname != LOG_FUNCTION_STR && fname != INIT_FUNCTION_STR && funcsToInst.count(fname) != 0) {
                        std::cerr<< "Instrucmenting: " << fname << std::endl;
                        instrumentFunction(M, f);
                    }
                }
                return true;
            }

            void initializeLogger(Module &M, Function &f) {
                BasicBlock &entryBlock = f.getEntryBlock();

                Function *initFunction = M.getFunction(INIT_FUNCTION_STR);

                CallInst::Create(initFunction, "", entryBlock.getFirstNonPHI());
            }

            void instrumentFunction(Module &M, Function &f) {
                BasicBlock &entryBlock = f.getEntryBlock();
                std::cerr << "Reaching here; " << std::endl;
                
                Instruction *firstInstr = entryBlock.getFirstNonPHI();
                IRBuilder<> builder(firstInstr);
                Value *strPointer = builder.CreateGlobalStringPtr(f.getName());

                Function *logFunction = M.getFunction(LOG_FUNCTION_STR);

                std::vector<Value *> args;
                args.push_back(strPointer);

                CallInst::Create(logFunction, args, "", entryBlock.getFirstNonPHI());
            }



            void declare_log_functions(Module &m) {
                LLVMContext &C = m.getContext();
                // void type
                Type *voidTy = Type::getVoidTy(C);

                // 64 bit integer
                Type *IntTy64 = Type::getInt64Ty(C);

                Type *StringType = Type::getInt8PtrTy(C);

                bool isVarArg = false;

                /* std::vector<Type*> variable_change_params; */
                /* variable_change_params.push_back(StringType); */
                /* variable_change_params.push_back(IntTy64); */
                /* FunctionType *variable_change_type = FunctionType::get(
                 * voidTy, variable_change_params, isVarArg); */

                std::vector<Type*> functionCallParams;
                functionCallParams.push_back(StringType);
                FunctionType *functionCallType = FunctionType::get(
                        voidTy, functionCallParams, isVarArg
                        );

                FunctionType *initFunctionType = FunctionType::get(
                        IntTy64, isVarArg
                        );

                // insert functions to module
                m.getOrInsertFunction(LOG_FUNCTION_STR, functionCallType);
                m.getOrInsertFunction(INIT_FUNCTION_STR, initFunctionType);

                /* m.getOrInsertFunction(LOG_VARIABLE_STR, variable_change_type); */
            }
    }; // end of struct
}  // end of anonymous namespace

char InstrumentFunctions::ID = 0;

static RegisterPass<InstrumentFunctions> X("instrument_function_calls",
        "Instrument Function Calls Pass",
        false /* Modifies CFG */,
        false /* Non Analysis Pass */);
