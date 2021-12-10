#include "./analysis.hpp"

namespace boda {

        bool ValueAnalysis::operator==(const ValueAnalysis &va2) const {
                return bufos == va2.bufos;
        }
        
        bool ValueAnalysis::operator!=(const ValueAnalysis &va2) const {
                return !(*this == va2);
        }

        void ValueAnalysis::join(ValueAnalysis &va2) {
                for (std::pair<llvm::Value *, std::unordered_set<std::string>> buf_analysis
                             : va2.bufos) {
                        bufos[buf_analysis.first].insert(buf_analysis.second.begin(),
                                                         buf_analysis.second.end());
                }
        }

        void ValueAnalysis::transition(llvm::Instruction *inst) {
                // TODO: working here
        }
        
        FunctionAnalysis::FunctionAnalysis()
                : fn{nullptr} {}
        
        FunctionAnalysis::FunctionAnalysis(llvm::Function *fn)
                : fn{fn} {}

        GlobalState::GlobalState(llvm::Module *mod)
                : mod{mod}, mst{mod} {}
}
