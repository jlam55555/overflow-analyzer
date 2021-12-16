#include "./analysis.hpp"

// warn about unsafe
// if any src > dst, warn unsafe
// strcpy(dst, src)
// strcat(dst, src)

// warn about src's size
// gets(src)

// if any bufs, warn about their size
// scanf("", ..., ...)

// warn about dst, any buffer sizes in argument
// sprintf(dst, "", ..., ...)

// TODO: get line numbers from original file

namespace boda {
        // 0 for error, 1 for warning
        std::string setStyle(std::string msg, int color) {
                switch (color) {
                case 0: return std::string{"\033[31;1m"} + msg + std::string{"\033[0m"};
                case 1: return std::string{"\033[33;1m"} + msg + std::string{"\033[0m"};
                default: return msg;
                }
        }
        
        // Get buffer size. For now, only get number of elements, don't worry about element size.
        int getBufSize(llvm::Type *ty) {
                if (ty->isPointerTy()) {
                        return getBufSize(llvm::dyn_cast<llvm::PointerType>(ty)->getElementType());
                } else if (ty->isArrayTy()) {
                        return llvm::dyn_cast<llvm::ArrayType>(ty)->getNumElements();
                } else {
                        // Doesn't refer to a fixed-size buffer. May refer to a malloced buffer.
                        return -1;
                }
        }

        // Get debuging information (fnname, location) for buforigin.
        std::string getInstructionDetails(llvm::Value *bo) {
                llvm::Instruction *bo_inst = llvm::dyn_cast<llvm::Instruction>(bo);
                return bo_inst->getFunction()->getName().str()
                        // Having trouble getting buffer origin for the time being
                        // + ":" + std::to_string(bo_inst->getDebugLoc().getLine())
                        + ":" + bo->getName().str();
        }
        
        // TODO: handle argv? I don't think we need to though
        // TODO: handle recursive calls
        static std::unordered_map<std::string, void (*)(std::vector<std::unordered_set<boda::BufOrigin>>)>
        dangerous_fns{
                {"strcpy", [](std::vector<std::unordered_set<boda::BufOrigin>> argos) {
                        // Warn unsafe
                        llvm::outs() << "\t\t["
                                     << setStyle(std::string{"Warning"}, 1)
                                     << "]: unsafe function strcpy()\n";
                        
                        // Warn if any src > dst
                        // Assume same indirection depth for now...
                        llvm::outs() << "\t\t\tPossible destinations: ";
                        int minDst = INT_MAX, maxSrc = INT_MIN;
                        for (boda::BufOrigin bo : argos[0]) {
                                int size = getBufSize(bo.bufo->getType());
                                if (size != -1) {
                                        minDst = std::min(minDst, size);
                                }
                                llvm::outs() << getInstructionDetails(bo.bufo) << "[" << size << "] ";
                        }
                        llvm::outs() << "\n\t\t\tPossible sources: ";
                        for (boda::BufOrigin bo : argos[1]) {
                                int size = getBufSize(bo.bufo->getType());
                                if (size != -1) {
                                        maxSrc = std::max(maxSrc, size);
                                }
                                llvm::outs() << getInstructionDetails(bo.bufo) << "[" << size << "] ";
                        }
                        llvm::outs() << "\n";

                        if (maxSrc > minDst) {
                                llvm::outs() << "\t\t\t["
                                             << setStyle(std::string{"Danger"}, 0)
                                             << "]: potential buffer overflow (source:"
                                             << maxSrc
                                             << " > destination:"
                                             << minDst
                                             << ")\n";
                        }

                }},
                {"strcat", [](std::vector<std::unordered_set<boda::BufOrigin>> argos) {
                        // TODO: essentially the same as strcpy
                        llvm::outs() << "\t\t["
                                     << setStyle(std::string{"Warning"}, 1)
                                     << "]: unsafe function strcat()\n";
                }},
                {"gets",  [](std::vector<std::unordered_set<boda::BufOrigin>> argos) {
                        llvm::outs() << "\t\t["
                                     << setStyle(std::string{"Warning"}, 1)
                                     << "]: unsafe function gets()\n";
                        // TODO
                }},
                {"scanf", [](std::vector<std::unordered_set<boda::BufOrigin>> argos) {
                        llvm::outs() << "\t\t["
                                     << setStyle(std::string{"Warning"}, 1)
                                     << "]: unsafe function scanf()\n";
                        // TODO
                }},
                {"sprintf", [](std::vector<std::unordered_set<boda::BufOrigin>> argos) {
                        llvm::outs() << "\t\t["
                                     << setStyle(std::string{"Warning"}, 1)
                                     << "]: unsafe function sprintf()\n";
                        // TODO
                }}
        };
        
        // Recursively CFG trace a function
        void trace_fn(FunctionState *fa, std::vector<std::unordered_set<boda::BufOrigin>> params) {

#ifdef DEBUG
                llvm::outs() << "\tIn function: " << fa->fn->getName() << "\n";
#endif
                
                for (llvm::CallInst *fncall_inst : fa->fncalls) {
                        std::string fncall_name = fncall_inst->getCalledFunction()->getName().str();
                        
#ifdef DEBUG
                        llvm::outs() << "\t\tExamining fncall: " << fncall_name << "\n";
#endif

                        // Get buffer origin analysis at this instruction
                        BodaAnalysis *inst_analysis = fa->ias[fncall_inst];

                        // Grab arguments
                        std::vector<std::unordered_set<boda::BufOrigin>> arg_list{};
                        for (llvm::Use *use_it = fncall_inst->arg_begin();
                             use_it != fncall_inst->arg_end();
                             ++use_it) {
                                llvm::Value *arg_value = *use_it;

                                std::unordered_set<boda::BufOrigin> var_analysis = inst_analysis->bufos[arg_value];
                                arg_list.push_back(var_analysis);

                                // Replace anything that lists a parameter with the parameter value
                                
#ifdef DEBUG
                                llvm::outs() << "\t\t\tArg: " << fa->getName(arg_value) << "; possible origins: { ";
                                for (boda::BufOrigin bo : var_analysis) {
                                        llvm::outs() << bo << " ";
                                }
                                llvm::outs() << "}\n";
#endif
                        }

                        if (dangerous_fns.count(fncall_name)) {
                                // If dangerous function
                                dangerous_fns[fncall_name](arg_list);
                        } else if (!fncall_inst->getCalledFunction()->isDeclaration()) {
                                // If user-defined function
                                trace_fn(fa, {});
                        }
                }
                
        }
        
        // Entry point for call graph tracing
        // TODO: this is a misnomer; it should be trace_call_graph (CFG is different);
        //     but a lot of things should be renamed as well
        void trace_cfg(GlobalState *state) {
                trace_fn(state->fas[state->mod->getFunction("main")], {});
        }

}
