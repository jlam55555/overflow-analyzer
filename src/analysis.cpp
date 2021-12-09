#include "./analysis.hpp"

void analysis(llvm::Module *module) {
        llvm::ModuleSlotTracker mst{&*module};
        
        // Step 1: Perform "buffer origin" dataflow analysis (BODA) (per-function).
        for (llvm::Module::iterator fn_it = module->begin();
             fn_it != module->end();
             ++fn_it) {
                boda(&mst, &*fn_it);
        }
        
        // Step 3: Trace CFG and find which buffers go to unsafe functions (all functions).
        trace_cfg(module);
}
