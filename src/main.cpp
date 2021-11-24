#include <llvm/IR/LLVMContext.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Error.h>

// Note: avoid `using namespace *` and `auto` typing so that namespaces and
// types are very explicit. Good for avoiding confusion for learning purposes.

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
 * Begins the overflow analysis.
 */
int main(int argc, char **argv) {
        // Get module from path
        std::string path = "res/llvm/hello.bc";
        llvm::LLVMContext context{};
        llvm::Expected<std::unique_ptr<llvm::Module>> moduleOrErr
                = getModuleFromFile(path, context);
        if (llvm::Error err = moduleOrErr.takeError()) {
                err_panic(err, -1);
        }
        std::unique_ptr<llvm::Module> module = std::move(moduleOrErr.get());

        // Print module (for debugging)
        // module->print(llvm::errs(), nullptr);

        // grab LLVM functions, basic blocks, instructions, lvars
        std::vector<llvm::Function *> fns = getFunctionsFromModule(module);

        // Print functions (for debugging)
        // for (auto f : fns) {
        //         f->print(llvm::errs(), nullptr);
        // }

        // TODO: perform the analysis

        // Done.
        llvm::outs() << "Done.\n";
        return 0;
}
