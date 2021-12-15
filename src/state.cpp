#include "./analysis.hpp"

namespace boda {

        BufOrigin BufOrigin::deref() const {
                BufOrigin bo = *this;
                --bo.pointer_depth;

                if (bo.pointer_depth < 0) {
                        llvm::errs() << "Error: Dereferencing past depth 0\n";
                        std::exit(-1);
                }
                
                return bo;
        }

        BufOrigin BufOrigin::ref() const {
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

        void BodaAnalysis::join(BodaAnalysis *va2) {
                for (std::pair<llvm::Value *, std::unordered_set<BufOrigin>> buf_analysis
                             : va2->bufos) {
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
                        
                case llvm::Instruction::MemoryOps::Store:
                        // Store opcode.
                        from = inst->getOperand(0);
                        to = inst->getOperand(1);

                        // Map ref over from's analysis.
                        bufos[to].clear();
                        for (const BufOrigin &bufo : bufos[from]) {
                                bufos[to].insert(bufo.ref());
                        }
                        
                        break;
                        
                case llvm::Instruction::MemoryOps::Load:
                case llvm::Instruction::MemoryOps::GetElementPtr:
                        // Load opcode.
                        // Getelementptr opcode acts very similar to load.
                        to = inst;
                        from = inst->getOperand(0);

                        // Map deref over from's analysis.
                        bufos[to].clear();
                        for (const BufOrigin &bufo : bufos[from]) {
                                bufos[to].insert(bufo.deref());
                        }
                        
                        break;
                        
                case llvm::Instruction::OtherOps::Call:
                        // Call opcode for special buffer functions.
                        // TODO: implement this
                        break;
                }

#ifdef DEBUG
                llvm::outs() << "\t\t\t\tAnalyzing: " << *inst << "\n";
                llvm::outs() << "\t\t\t\t\tAnalysis: " << *this << "\n";
#endif
        }

        void BodaAnalysis::print(llvm::raw_ostream &os) const {
                for (const std::pair<llvm::Value *, std::unordered_set<BufOrigin>> &bufo : bufos) {
                        os << fa->getName(bufo.first) << " { ";
                        for (const BufOrigin &bo : bufo.second) {
                                os << bo << " ";
                        }
                        os << "} ";
                }
        }

        BodaAnalysis::BodaAnalysis(FunctionState *fa) : fa{fa} {}

        std::string FunctionState::getName(llvm::Value *value) {
                return value->hasName()
                        ? value->getName().str()
                        : "%vr_" + std::to_string(mst.getLocalSlot(value));
        }

        FunctionState::FunctionState(GlobalState *state, llvm::Function *fn)
                : state{state}, fn{fn}, mst{state->mod} {
                mst.incorporateFunction(*fn);
        }

        GlobalState::GlobalState(llvm::Module *mod)
                : mod{mod} {}
}

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const boda::BufOrigin &bo) {
        bo.print(os);
        return os;
}

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const boda::BodaAnalysis &boda_analysis) {
        boda_analysis.print(os);
        return os;
}
