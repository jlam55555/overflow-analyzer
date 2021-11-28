#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Error.h>

// Note: avoid `using namespace *` and `auto` typing so that namespaces and
// types are very explicit. Good for avoiding confusion for learning purposes.


/**
 * Helper function for printing a basic block and optionally the instructions
 * within the basic block
 */
void print_bb(llvm::BasicBlock &bb, bool inst=false) {
        if (inst) {
                bb.print(llvm::errs());
        } else { 
                llvm::errs() << "B: " << bb.getName() << "\n";
        }
}

/**
 * Helper function for printing a function and optionally all the instructions
 * in all the basic block
 */
void print_func(llvm::Function &f, bool inst=false) {
        llvm::errs() << "F: " << f.getName() << "\n";
        // print bbs
        for (llvm::ilist<llvm::BasicBlock>::iterator it= f.getBasicBlockList().begin();
                it != f.getBasicBlockList().end(); it++) {
                print_bb(*it, inst);
        }
        llvm::errs() << "\n";
}

/**
 * Helper function to exit the program due to a `llvm::Error`. Will exit
 * the entire program with the given return code.
 */
void err_panic(llvm::Error& err, int code) {
        llvm::errs() << llvm::toString(std::move(err)) << "\n";
        exit(code);
}

/**
 * Returns a (managed) reference to a `llvm::Module`, or error. The scope of
 * the `llvm::LLVMContext` should outlive the module, since it manages the
 * memory allocations of the module.
 */
llvm::Expected<std::unique_ptr<llvm::Module>>
getModuleFromFile(std::string& filename, llvm::LLVMContext& context) {
        // Read in bitcode from file
        llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> bufferOrErr
                = llvm::MemoryBuffer::getFile(filename);
        if (llvm::Error err = llvm::errorCodeToError(bufferOrErr.getError())) {
                return err;
        }

        // Build module from memory buffer
        return llvm::parseBitcodeFile(bufferOrErr.get().get()
                                      ->getMemBufferRef(), context);
}

/**
 * Returns a vector of (unmanaged) pointers to functions from a `llvm::Module`.
 */
std::vector<llvm::Function *>
getFunctionsFromModule(std::unique_ptr<llvm::Module>& module) {
        std::vector<llvm::Function *> fns{};
        
        for (llvm::Module::FunctionListType::iterator it
                     = module->functions().begin();
             it != module->functions().end(); ++it) {
                fns.push_back(&*it);
        }

        return fns;
}

/**
 * Returns a vector of pointer to BasicBlocks from a `llvm::Function`.
 */
std::vector<llvm::BasicBlock *>
getBasicBlocksFromFunction(llvm::Function *fnc) {
        std::vector<llvm::BasicBlock *> bbs{};

        for (llvm::ilist<llvm::BasicBlock>::iterator it 
                = fnc->getBasicBlockList().begin();
            it != fnc->getBasicBlockList().end(); it++) {
                bbs.push_back(&*it);
        }

        return bbs;
}

/**
 * Returns a vector of pointer to Instructions from a `llvm::BasicBlock`.
 */
std::vector<llvm::Instruction *> 
getInstructionsFromBasicBlock(llvm::BasicBlock *bb) {
        std::vector<llvm::Instruction *> insts{};

        for (llvm::ilist<llvm::Instruction>::iterator it = bb->getInstList().begin(); 
            it != bb->getInstList().end(); it++) {
                insts.push_back(&*it);
        }

        return insts;
}

/**
 * Begins the overflow analysis.
 */
int main(int argc, char **argv) {
        // Get module from path
        std::string path = "res/llvm/bbs.bc";
        llvm::LLVMContext context{};
        llvm::Expected<std::unique_ptr<llvm::Module>> moduleOrErr
                = getModuleFromFile(path, context);
        if (llvm::Error err = moduleOrErr.takeError()) {
                err_panic(err, -1);
        }
        std::unique_ptr<llvm::Module> module = std::move(moduleOrErr.get());

        // // Print module (for debugging)
        // module->print(llvm::errs(), nullptr);

        // grab LLVM functions, basic blocks, instructions, lvars
        std::vector<llvm::Function *> fns = getFunctionsFromModule(module);
        std::vector<llvm::BasicBlock *> bbs = getBasicBlocksFromFunction(fns[0]);
        std::vector<llvm::Instruction *> insts = getInstructionsFromBasicBlock(bbs[0]);

        // need to get locals and operands from Instructions 

        llvm::errs() << *insts[0] << "\n\t";
        llvm::errs() << insts[0]->getOpcodeName() << "\n\t";

        // getting operands from an instruction
        llvm::Instruction *inst = insts[0];
        std::vector<llvm::Use *> opers{};
        for (llvm::Use *it = inst->op_begin(); it != inst->op_end(); it++) {
                opers.push_back(&*it);
        }

        llvm::errs() << "Operand No: " << opers[0]->getOperandNo() << "\n\t\t";

        llvm::errs() << *(insts[0]->getOperand(0)) << "\n\t\t";
        llvm::errs() << insts[0]->getOperand(0)->getName() << "\n\t\t";
        
        // TODO: perform the analysis

        // Done.
        llvm::outs() << "Done.\n";
        return 0;
}
