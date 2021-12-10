#include "./analysis.hpp"

/**
 * Returns a (managed) reference to a `llvm::Module`, or error. The scope of
 * the `llvm::LLVMContext` should outlive the module, since it manages the
 * memory allocations of the module.
 */
llvm::Expected<std::unique_ptr<llvm::Module>>
getModuleFromFile(std::string &filename, llvm::LLVMContext &context) {
	// Read in bitcode from file
	llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> bufferOrErr =
                llvm::MemoryBuffer::getFile(filename);
	if (llvm::Error err = llvm::errorCodeToError(bufferOrErr.getError())) {
		return err;
	}

	// Build module from memory buffer
	return llvm::parseBitcodeFile(bufferOrErr.get().get()->getMemBufferRef(),
                                      context);
}

int main(int argc, char **argv) {
        // Get module from path
        std::string path = "res/llvm/bufs.bc";
        llvm::LLVMContext context{};
        llvm::Expected<std::unique_ptr<llvm::Module>> moduleOrErr =
                getModuleFromFile(path, context);
        if (llvm::Error err = moduleOrErr.takeError()) {
                llvm::errs() << "Error loading module from bitcode.\n";
        }
        std::unique_ptr<llvm::Module> module = std::move(moduleOrErr.get());

        boda::GlobalState state{module.get()};
        analysis(&state);
}
