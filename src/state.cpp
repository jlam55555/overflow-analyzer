#include "./analysis.hpp"

namespace boda {
        // Dataflow analysis join of abstract variables for all buffers.
        // This lattice is the set lattice, so the join operation is
        // simply set union.
        void ValueAnalysis::join(ValueAnalysis &va2) {
                for (std::pair<llvm::Value *, std::unordered_set<std::string>> buf_analysis
                             : va2.bufos) {
                        bufos[buf_analysis.first].insert(buf_analysis.second.begin(),
                                                         buf_analysis.second.end());
                }
        }
        
        FunctionAnalysis::FunctionAnalysis()
                : fn{nullptr} {}
        
        FunctionAnalysis::FunctionAnalysis(llvm::Function *fn)
                : fn{fn} {}

        GlobalState::GlobalState(llvm::Module *mod)
                : mod{mod}, mst{mod} {}
}
