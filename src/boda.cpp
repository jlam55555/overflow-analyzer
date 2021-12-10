#include "./analysis.hpp"

// Returns true if pointer or array type (i.e., a "buffer value") type. Note that
// not all pointer types will actually be used as arrays/buffers, but we don't
// distinguish that here (because there is no easy way to).
bool isBufType(llvm::Type *ty) {
        return ty->isPointerTy() || ty->isArrayTy();
}

// Returns true if is of type pointer-to-pointer or pointer-to-array type.
// (i.e., a "buffer origin" type). A value is only an origin if it has buffer origin
// type and is a parameter or is stack-allocated (i.e., virtual registers or other
// temporary values are not buffer origins).
bool isBufOriginType(llvm::Type *ty) {
        return ty->isPointerTy()
                && isBufType(llvm::dyn_cast<llvm::PointerType>(ty)->getElementType());
}

// Finds all of the candidate buffer values in the function.
// This includes pointer-to-pointer and pointer-to-array types.
std::pair<std::unordered_set<std::string>, std::unordered_map<std::string, llvm::Value *>>
getBufferValues(llvm::ModuleSlotTracker *mst, llvm::Function *fn) {
        std::unordered_set<std::string> bufos{};
        std::unordered_map<std::string, llvm::Value *> bufs{};

        for (llvm::Function::iterator bb_it = fn->begin();
             bb_it != fn->end();
             ++bb_it) {
                for (llvm::BasicBlock::iterator inst_it = bb_it->begin();
                     inst_it != bb_it->end();
                     ++inst_it) {
                        llvm::Instruction *inst = &*inst_it;
#ifdef DEBUG
                        llvm::outs() << "\t\tExamining instruction: " << *inst << "\n";
#endif

                        // Void types
                        if (inst->getType()->isVoidTy()) {
                                continue;
                        }

                        std::string valName = inst->hasName()
                                ? inst->getName().str()
                                : "%vr" + std::to_string(mst->getLocalSlot(inst));
                        
                        // New buffer origin
                        if (inst->getOpcode() == llvm::Instruction::MemoryOps::Alloca
                            && isBufOriginType(inst->getType())) {
                                bufos.insert(valName);
#ifdef DEBUG
                                llvm::outs() << "\t\t\tNew buffer origin\n";
#endif
                        }

                        // New buffer (includes all buffer origins)
                        if (isBufType(inst->getType())) {
                                bufs[valName] = inst;
#ifdef DEBUG
                                llvm::outs() << "\t\t\tNew buffer\n";
#endif
                        }
                }
        }

#ifdef DEBUG
        llvm::outs() << "\t\tFunction summary:\n";
        llvm::outs() << "\t\t\tBuffer origins:";
        for (std::string bufo : bufos) {
                llvm::outs() << " " << bufo;
        }
        llvm::outs() << "\n\t\t\tBuffers:";
        for (auto buf : bufs) {
                llvm::outs() << " " << buf.first << " (" << *buf.second->getType() << ")";
        }
        llvm::outs() << "\n";
#endif
        
        return {bufos, bufs};
}

// "Buffer origin" dataflow analysis. Tracking which stack variables
// a given may have originated from at any point.
void boda(llvm::ModuleSlotTracker *mst, llvm::Function *fn) {
#ifdef DEBUG
        llvm::outs() << "\tBODA on function: " << fn->getName() << "\n";
#endif
        
        // Necessary for ModuleSlotTracker to work correctly.
        mst->incorporateFunction(*fn);
        
        // Get all buffer values in the function.
        auto bufVals = getBufferValues(mst, fn);
        auto bufOrigins = bufVals.first;
        auto bufs = bufVals.second;

        // Set up the initial analysis information.
        // TODO

        // Perform the dataflow analysis per basic block
        // (standard worklist algorithm).
        // TODO
}
