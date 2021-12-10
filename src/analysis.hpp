#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP

#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/ModuleSlotTracker.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBuffer.h>

#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace boda {

        // Dataflow analysis instance at a program point. (I.e., an instance of this is
        // created for each instruction.)
        class BodaAnalysis {
        public:
                // Set of possible buffer origins for each buffer.
                std::unordered_map<llvm::Value *, std::unordered_set<std::string>> bufos{};
                
                // Comparing two dataflow analyses by value.
                bool operator==(const BodaAnalysis &va2) const;
                bool operator!=(const BodaAnalysis &va2) const;
                
                // Dataflow analysis join of abstract variables for all buffers.
                // This lattice is the set lattice, so the join operation is
                // simply set union.
                void join(BodaAnalysis &va2);

                // Transition function of dataflow analysis over an instruction.
                void transition(llvm::Instruction *inst);
        };

        // State relevant to the analysis of a function.
        class FunctionState {
        public:
                llvm::Function *fn;
                std::unordered_set<std::string> bufos{};
                std::unordered_map<std::string, llvm::Value *> bufs{};

                // Mapping from each instruction to the analysis after that instruction.
                std::unordered_map<llvm::Value *, BodaAnalysis> ias{};

                // Dirty basic blocks for the worklist phase.
                std::unordered_map<llvm::BasicBlock *, bool> dirty_bbs{};

                // Note: Default constructor required for use in map.
                // Shouldn't really have null function though.
                FunctionState();
                
                FunctionState(llvm::Function *fn);
        };

        // State relevant to the overall analysis.
        class GlobalState {
        public:
                llvm::Module *mod;
                llvm::ModuleSlotTracker mst;

                std::unordered_map<llvm::Function *, FunctionState> fas;

                GlobalState(llvm::Module *mod);
        };

        void analysis(GlobalState *state);

        void boda(GlobalState *state, llvm::Function *fn);

        void trace_cfg(GlobalState *state);
};

#endif
