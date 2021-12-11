#include "./analysis.hpp"

namespace boda {

        BufOrigin BufOrigin::deref() {
                BufOrigin bo = *this;
                --bo.pointer_depth;

                if (bo.pointer_depth < 0) {
                        llvm::errs() << "Error: Dereferencing past depth 0\n";
                        std::exit(-1);
                }
                
                return bo;
        }

        BufOrigin BufOrigin::ref() {
                BufOrigin bo = *this;
                ++bo.pointer_depth;
                return bo;
        }


        bool BufOrigin::operator==(const BufOrigin &bo2) const {
                return pointer_depth == bo2.pointer_depth
                        && bufo == bo2.bufo;
        }

        void BufOrigin::print(llvm::raw_ostream &os) const {
                os << std::string(pointer_depth, '&') << bufo->getName();
        }
        
        BufOrigin::BufOrigin(int pointer_depth, llvm::Value *bufo)
                : pointer_depth{pointer_depth}, bufo{bufo} {}

        bool BodaAnalysis::operator==(const BodaAnalysis &va2) const {
                return bufos == va2.bufos;
        }
        
        bool BodaAnalysis::operator!=(const BodaAnalysis &va2) const {
                return !(*this == va2);
        }

        void BodaAnalysis::join(BodaAnalysis &va2) {
                for (std::pair<llvm::Value *, std::unordered_set<BufOrigin>> buf_analysis
                             : va2.bufos) {
                        bufos[buf_analysis.first]
                                .insert(buf_analysis.second.begin(),
                                        buf_analysis.second.end());
                }
        }

        void BodaAnalysis::transition(llvm::Instruction *inst) {
                llvm::Value *to, *from;
                
                switch (inst->getOpcode()) {
                case llvm::Instruction::MemoryOps::Alloca:
                        // Alloca opcode.
                        to = inst;
                        bufos[to].insert(BufOrigin{1, to});
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

#ifdef DEBUG
                llvm::outs() << "\t\t\tAnalyzing: " << *inst << "\n";
                llvm::outs() << "\t\t\t\tAnalysis: " << *this << "\n";
#endif
        }

        void BodaAnalysis::print(llvm::raw_ostream &os) const {
                for (const std::pair<llvm::Value *, std::unordered_set<BufOrigin>> &bufo : bufos) {
                        os << bufo.first->getName() << " { ";
                        for (const BufOrigin &bo : bufo.second) {
                                os << bo << " ";
                        }
                        os << "} ";
                }
        }
        
        FunctionState::FunctionState()
                : fn{nullptr} {}
        
        FunctionState::FunctionState(llvm::Function *fn)
                : fn{fn} {}

        GlobalState::GlobalState(llvm::Module *mod)
                : mod{mod}, mst{mod} {}
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const boda::BufOrigin &bo) {
        bo.print(os);
        return os;
}

inline llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const boda::BodaAnalysis &boda_analysis) {
        boda_analysis.print(os);
        return os;
}
