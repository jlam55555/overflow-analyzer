#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP

#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/ModuleSlotTracker.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/FormattedStream.h>

#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace boda {

        // Representation of the relationship of a buffer to a buffer origin. This is a
        // member of the abstract values in the lattice (sets of BufOrigin). This is useful
        // because we may pass around pointers to buffers or nested buffers.
        class BufOrigin {
        public:
                int pointer_depth;
                llvm::Value *bufo;

                // Returns a copy, with one level less/more of indirection.
                BufOrigin deref() const;
                BufOrigin ref() const;

                // Necessary for hashing.
                bool operator==(const BufOrigin &bo2) const;

                void print(llvm::raw_ostream &os) const;
                
                BufOrigin(int pointer_depth, llvm::Value *bufo);
        };

};

// Need to put this before any implicit instantiation of BufOrigin.
template<>
struct std::hash<boda::BufOrigin> {
        std::size_t operator()(const boda::BufOrigin &bo) const noexcept {
                return std::hash<int>{}(bo.pointer_depth) ^ std::hash<llvm::Value *>{}(bo.bufo);
        }
};

namespace boda {
        class GlobalState;
        class FunctionState;

        // Dataflow analysis instance at a program point. (I.e., an instance of this is
        // created for each instruction.)
        class BodaAnalysis {
        public:
                FunctionState *fa;
                
                // Set of possible buffer origins for each buffer.
                std::unordered_map<llvm::Value *, std::unordered_set<BufOrigin>> bufos{};
                
                // Comparing two dataflow analyses by value.
                bool operator==(const BodaAnalysis &va2) const;
                bool operator!=(const BodaAnalysis &va2) const;
                
                // Dataflow analysis join of abstract variables for all buffers.
                // This lattice is the set lattice, so the join operation is
                // simply set union.
                void join(BodaAnalysis *va2);

                // Transition function of dataflow analysis over an instruction.
                void transition(llvm::Instruction *inst);

                void print(llvm::raw_ostream &os) const;

                BodaAnalysis(FunctionState *fa);
        };

        // State relevant to the analysis of a function.
        class FunctionState {
        public:
                GlobalState *state;
                llvm::Function *fn;
                llvm::ModuleSlotTracker mst;
                std::unordered_set<std::string> bufos{};
                std::unordered_map<std::string, llvm::Value *> bufs{};
                std::vector<llvm::Value *> fncalls{};

                // Mapping from each instruction to the analysis after that instruction.
                std::unordered_map<llvm::Value *, BodaAnalysis *> ias{};

                // Dirty basic blocks for the worklist phase.
                std::unordered_map<llvm::BasicBlock *, bool> dirty_bbs{};

                // Get name or slot number of value.
                std::string getName(llvm::Value *value);

                FunctionState(GlobalState *state, llvm::Function *fn);
        };

        // State relevant to the overall analysis.
        class GlobalState {
        public:
                llvm::Module *mod;

                std::unordered_map<llvm::Function *, FunctionState *> fas;

                GlobalState(llvm::Module *mod);
        };

        void analysis(GlobalState *state);

        void boda(GlobalState *state, llvm::Function *fn);

        void trace_cfg(GlobalState *state);
};

// Allow printing using llvm::outs().
llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const boda::BufOrigin &bo);
llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const boda::BodaAnalysis &boda_analysis);

#endif
