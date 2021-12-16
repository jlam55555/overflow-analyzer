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
        // TODO: handle argv? I don't think we need to though
        // TODO: handle recursive calls
        static std::unordered_map<std::string, void (*)(std::vector<std::unordered_set<boda::BufOrigin>>)>
        dangerous_fns{
                {"strcpy", [](std::vector<std::unordered_set<boda::BufOrigin>> argos) {
                        
                }},
                {"strcat", [](std::vector<std::unordered_set<boda::BufOrigin>> argos) {}},
                {"gets",  [](std::vector<std::unordered_set<boda::BufOrigin>> argos) {}},
                {"scanf", [](std::vector<std::unordered_set<boda::BufOrigin>> argos) {}},
                {"sprintf", [](std::vector<std::unordered_set<boda::BufOrigin>> argos) {}}
        };
        
        // Recursively CFG trace a function
        void trace_fn(FunctionState *fa, std::vector<llvm::Value *> params) {

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
                        for (llvm::Use *use_it = fncall_inst->arg_begin();
                             use_it != fncall_inst->arg_end();
                             ++use_it) {
                                llvm::Value *arg_value = *use_it;

                                std::unordered_set<boda::BufOrigin> var_analysis = inst_analysis->bufos[arg_value];
#ifdef DEBUG
                                llvm::outs() << "\t\t\tArg: " << fa->getName(arg_value) << "; possible origins: { ";
                                for (boda::BufOrigin bo : var_analysis) {
                                        llvm::outs() << bo << " ";
                                }
                                llvm::outs() << "}\n";
#endif
                        }

                        if (dangerous_fns.count(fncall_name)) {
                                llvm::outs() << "Dangerous function: " << fncall_name << "\n";
                                dangerous_fns[fncall_name]({});
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
