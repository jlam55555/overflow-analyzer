#include "./analysis.hpp"

namespace boda {

        bool BufOrigin::operator==(const BufOrigin &bo2) const {
                return pointer_depth == bo2.pointer_depth
                        && bufo_name == bo2.bufo_name;
        }

        BufOrigin::BufOrigin(int pointer_depth, std::string bufo_name)
                : pointer_depth{pointer_depth}, bufo_name{bufo_name} {}

        bool BodaAnalysis::operator==(const BodaAnalysis &va2) const {
                return bufos == va2.bufos;
        }
        
        bool BodaAnalysis::operator!=(const BodaAnalysis &va2) const {
                return !(*this == va2);
        }

        void BodaAnalysis::join(BodaAnalysis &va2) {
                for (std::pair<llvm::Value *, std::unordered_set<BufOrigin>> buf_analysis
                             : va2.bufos) {
                        bufos[buf_analysis.first].insert(buf_analysis.second.begin(),
                                                         buf_analysis.second.end());
                }
        }

        void BodaAnalysis::transition(llvm::Instruction *inst) {
                switch (inst->getOpcode()) {
                case llvm::Instruction::MemoryOps::Alloca:
                        // Alloca opcode.
                        break;
                        
                case llvm::Instruction::MemoryOps::Load:
                        // Load opcode.
                        break;
                        
                case llvm::Instruction::MemoryOps::Store:
                        // Store opcode.
                        break;
                        
                case llvm::Instruction::MemoryOps::GetElementPtr:
                        // Getelementptr opcode acts very similar to load.
                        break;
                        
                case llvm::Instruction::OtherOps::Call:
                        // Call opcode for specialbuffer functions.
                        break;
                }
        }
        
        FunctionState::FunctionState()
                : fn{nullptr} {}
        
        FunctionState::FunctionState(llvm::Function *fn)
                : fn{fn} {}

        GlobalState::GlobalState(llvm::Module *mod)
                : mod{mod}, mst{mod} {}
}
