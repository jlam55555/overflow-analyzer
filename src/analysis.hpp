#ifndef ANALYSIS_HPP
#define ANALYSIS_HPP

#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/ModuleSlotTracker.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBuffer.h>

#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>

void boda(llvm::ModuleSlotTracker *mst, llvm::Function *fn);

void trace_cfg(llvm::Module *module);

#endif
