#include "./analysis.hpp"

void analysis(llvm::Module *module) {
        llvm::ModuleSlotTracker mst{&*module};
        
        // Step 1: Perform "buffer origin" dataflow analysis (BODA) (per-function).
#ifdef DEBUG
        llvm::outs() << "Step 1: Buffer origina dataflow analysis\n";
#endif
        for (llvm::Module::iterator fn_it = module->begin();
             fn_it != module->end();
             ++fn_it) {
                boda(&mst, &*fn_it);
        }
        
        // Step 2: Trace CFG and find which buffers go to unsafe functions (all functions).
#ifdef DEBUG
        llvm::outs() << "Step 2: Trace CFG\n";
#endif
        trace_cfg(module);
}
