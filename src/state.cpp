#include "./analysis.hpp"

namespace boda {

        bool BodaAnalysis::operator==(const BodaAnalysis &va2) const {
                return bufos == va2.bufos;
        }
        
        bool BodaAnalysis::operator!=(const BodaAnalysis &va2) const {
                return !(*this == va2);
        }

        void BodaAnalysis::join(BodaAnalysis &va2) {
                for (std::pair<llvm::Value *, std::unordered_set<std::string>> buf_analysis
                             : va2.bufos) {
                        bufos[buf_analysis.first].insert(buf_analysis.second.begin(),
                                                         buf_analysis.second.end());
                }
        }

        void BodaAnalysis::transition(llvm::Instruction *inst) {
                // TODO: working here
        }
        
        FunctionState::FunctionState()
                : fn{nullptr} {}
        
        FunctionState::FunctionState(llvm::Function *fn)
                : fn{fn} {}

        GlobalState::GlobalState(llvm::Module *mod)
                : mod{mod}, mst{mod} {}
}
