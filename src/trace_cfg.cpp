#include "./analysis.hpp"

namespace boda {
        // TODO: handle argv? I don't think we need to though
        // TODO: handle recursive calls
        // TODO: add more dangerous functions
        static std::unordered_set<std::string> dangerous{"strcpy", "strcat"};
        
        // Recursively CFG trace a function
        // TODO: also set retval???
        void trace_fn(FunctionState *fa, std::vector<llvm::Value *> params) {

#ifdef DEBUG
                llvm::outs() << "\tIn function: " << fa->fn->getName() << "\n";
#endif
                
                for (llvm::CallInst *fncall_inst : fa->fncalls) {
#ifdef DEBUG
                        llvm::outs() << "\t\tExamining fncall: "
                                     << fncall_inst->getCalledFunction()->getName()
                                     << "\n";
#endif

                        // Get buffer origin analysis at this instruction
                        BodaAnalysis *inst_analysis = fa->ias[fncall_inst];

                        // Grab arguments
                        for (llvm::Use *use_it = fncall_inst->arg_begin();
                             use_it != fncall_inst->arg_end();
                             ++use_it) {
                                llvm::Value *arg_value = *use_it;

                                std::unordered_set<boda::BufOrigin> var_analysis = inst_analysis->bufos[arg_value];
                                llvm::outs() << "\t\t\tArg: " << fa->getName(arg_value) << "; possible origins: { ";
                                for (boda::BufOrigin bo : var_analysis) {
                                        llvm::outs() << bo << " ";
                                }
                                llvm::outs() << "}\n";
                        }
                }
                
        }
        
        // Entry point for CFG tracing
        void trace_cfg(GlobalState *state) {
                trace_fn(state->fas[state->mod->getFunction("main")], {});
        }

}
