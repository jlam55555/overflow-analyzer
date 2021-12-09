#include "./analysis.hpp"

// Finds all of the candidate buffer values in the function.
// This includes pointer-to-pointer and pointer-to-array types.
std::unordered_map<std::string, llvm::Value *>
getBufferValues(llvm::ModuleSlotTracker *mst, llvm::Function *fn) {
        std::unordered_map<std::string, llvm::Value *> vals{};

        // TODO
        
        return vals;
}

// "Buffer origin" dataflow analysis. Tracking which stack variables
// a given may have originated from at any point.
void boda(llvm::ModuleSlotTracker *mst, llvm::Function *fn) {
        mst->incorporateFunction(*fn);
        
        // Get all buffer values in the function.
        std::unordered_map<std::string, llvm::Value *> bufferVals = getBufferValues(mst, fn);

        // Set up the initial analysis information.
        // TODO

        // Perform the dataflow analysis per basic block
        // (standard worklist algorithm).
        // TODO
}
