#include "./analysis.hpp"

namespace boda {

        void analysis(GlobalState *state) {
                // Step 1: Perform "buffer origin" dataflow analysis (BODA) (per-function).
#ifdef DEBUG
                llvm::outs() << "Step 1: Buffer origin dataflow analysis\n";
#endif
                for (llvm::Module::iterator fn_it = state->mod->begin();
                     fn_it != state->mod->end();
                     ++fn_it) {
                        boda::boda(state, &*fn_it);
                }
        
                // Step 2: Trace CFG and find which buffers go to unsafe functions (all functions).
#ifdef DEBUG
                llvm::outs() << "Step 2: Trace CFG\n";
#endif
                trace_cfg(state);
        }

}
