#include "./analysis.hpp"

namespace boda {

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
        void getBufferValues(FunctionState *fa) {
                llvm::Function *fn = fa->fn;

                // Get all variables from parameters
                int arg_index = 0;
                for (llvm::Function::arg_iterator arg_it = fn->arg_begin();
                     arg_it != fn->arg_end();
                     ++arg_it) {
                        llvm::Argument *arg = &*arg_it;

#ifdef DEBUG
                        llvm::outs() << "\t\tGot argument: " << *arg << "\n";
#endif DEBUG

                        if (arg->getType()->isPointerTy()) {
#ifdef DEBUG
                                llvm::outs() << "\t\t\tNew buffer argument\n";
#endif DEBUG
                                fa->args.insert({arg, arg_index});
                        }
                        ++arg_index;
                }

                // Get all buffers from local variables
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

                                // Keep track of all function calls.
                                if (inst->getOpcode() == llvm::Instruction::OtherOps::Call) {
                                        fa->fncalls.push_back(llvm::dyn_cast<llvm::CallInst>(inst));
#ifdef DEBUG
                                        llvm::outs() << "\t\t\tNew fncall\n";
#endif
                                }

                                // Void types
                                if (inst->getType()->isVoidTy()) {
                                        continue;
                                }

                                std::string valName = inst->hasName()
                                        ? inst->getName().str()
                                        : "%vr" + std::to_string(fa->mst.getLocalSlot(inst));

                                // New buffer origin
                                if (inst->getOpcode() == llvm::Instruction::MemoryOps::Alloca
                                    && isBufOriginType(inst->getType())) {
                                        fa->bufos.insert(valName);
#ifdef DEBUG
                                        llvm::outs() << "\t\t\tNew buffer origin\n";
#endif
                                }

                                // New buffer (includes all buffer origins)
                                if (isBufType(inst->getType())) {
                                        fa->bufs[valName] = inst;
#ifdef DEBUG
                                        llvm::outs() << "\t\t\tNew buffer\n";
#endif
                                }
                        }
                }

#ifdef DEBUG
                llvm::outs() << "\t\tFunction summary:\n";
                llvm::outs() << "\t\t\tBuffer origins:";
                for (std::string bufo : fa->bufos) {
                        llvm::outs() << " " << bufo;
                }
                llvm::outs() << "\n\t\t\tBuffers:";
                for (auto buf : fa->bufs) {
                        llvm::outs() << " " << buf.first << " (" << *buf.second->getType() << ")";
                }
                llvm::outs() << "\n";
#endif
        }

        // Performs buffer origin dataflow analysis on a single basic block,
        // returning true iff any changes were made.
        bool boda_bb(FunctionState *fa, llvm::BasicBlock *bb) {
                // Get initial analysis by joining analyses (output_sets) of predecessors.
                // The greek letter sigma is the symbol used in the literature.
                BodaAnalysis sigma{fa};
                for (llvm::pred_iterator bb_it = llvm::pred_begin(bb);
                     bb_it != llvm::pred_end(bb);
                     ++bb_it) {
                        sigma.join(fa->ias[(*bb_it)->getTerminator()]);
                }
                
#ifdef DEBUG
                llvm::outs() << "\t\t\tAnalyzing basic block: " << bb->getName() << "\n";
                llvm::outs() << "\t\t\t\tInitial analysis: " << sigma << "\n";
#endif
                
                // Walk through instruction by instruction, performing transition function.
                bool dirty = false;
                for (llvm::BasicBlock::iterator inst_it = bb->begin();
                     inst_it != bb->end();
                     ++inst_it) {
                        llvm::Instruction *inst = &*inst_it;
                        
                        sigma.transition(inst);
                        if (sigma != *fa->ias[inst]) {
                                dirty = true;
                                *fa->ias[inst] = sigma;
                        }
                }

                return dirty;
        }

        // Perform the dataflow analysis on this function.
        void worklist(FunctionState *fa) {
#ifdef DEBUG
                llvm::outs() << "\t\tPerforming dataflow analysis worklist algorithm\n";
#endif
                
                // Set initial basic block dirty and add it to worklist.
                llvm::BasicBlock *bb = &fa->fn->getEntryBlock();
                fa->dirty_bbs[bb] = true;

                std::queue<llvm::BasicBlock *> dirty{};
                dirty.push(bb);
                
                // Worklist queue
                while (!dirty.empty()) {
                        bb = dirty.front();
                        dirty.pop();

                        // If basic block is not dirty, no need to perform analysis.
                        if (!fa->dirty_bbs[bb]) {
                                continue;
                        }
                        fa->dirty_bbs[bb] = false;

                        // If analysis of basic block changes any values,
                        // then mark all successors dirty and add them to the queue.
                        if (boda_bb(fa, bb)) {
                                for (llvm::succ_iterator bb_it = llvm::succ_begin(bb);
                                     bb_it != llvm::succ_end(bb);
                                     ++bb_it) {
                                        fa->dirty_bbs[*bb_it] = true;
                                        dirty.push(*bb_it);
                                }
                        }
                }
        }

        // Set up function state.
        void setupFunctionState(FunctionState *fa) {
                // Create a new analysis for each instruction.
                for (llvm::Function::iterator bb_it = fa->fn->begin(); bb_it != fa->fn->end(); ++bb_it) {
                        for (llvm::BasicBlock::iterator inst_it = bb_it->begin();
                             inst_it != bb_it->end();
                             ++inst_it) {
                                fa->ias[&*inst_it] = new BodaAnalysis{fa};
                        }
                }
        }

        // "Buffer origin" dataflow analysis. Tracking which stack variables
        // a given may have originated from at any point.
        void boda(GlobalState *state, llvm::Function *fn) {
                FunctionState *fa;
                
                if (fn->isDeclaration()) {
#ifdef DEBUG
                        llvm::outs() << "\tExternal function, skipping analysis: " << fn->getName() << "\n";
#endif
                        return;
                }
                
#ifdef DEBUG
                llvm::outs() << "\tBODA on function: " << fn->getName() << "\n";
                llvm::outs() << "\tFunction def: " << *fn << "\n";
#endif

                // Create function analysis class.
                state->fas[fn] = fa = new FunctionState{state, fn};

                // Get all buffer values in the function.
                getBufferValues(fa);

                // Set up the initial function analysis state.
                setupFunctionState(fa);

                // Perform the dataflow analysis per basic block
                // (standard worklist algorithm).
                worklist(fa);
        }

}
