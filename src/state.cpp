#include "./analysis.hpp"

namespace boda {
        FunctionAnalysis::FunctionAnalysis()
                : fn{nullptr} {}
        
        FunctionAnalysis::FunctionAnalysis(llvm::Function *fn)
                : fn{fn} {}

        GlobalState::GlobalState(llvm::Module *mod)
                : mod{mod}, mst{mod} {}
}
