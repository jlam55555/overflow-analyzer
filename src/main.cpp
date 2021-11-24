#include <llvm/IR/LLVMContext.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Error.h>

int err_panic(llvm::Error& err, int code) {
        llvm::errs() << llvm::toString(std::move(err)) << "\n";
        return code;
}

llvm::Expected<std::unique_ptr<llvm::Module>> getModuleFromFile(std::string& filename) {
        // Read in bitcode from file
        llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> bufferOrErr
                = llvm::MemoryBuffer::getFile(filename);
        if (auto err = llvm::errorCodeToError(bufferOrErr.getError())) {
                return err;
        }
        llvm::MemoryBufferRef memoryBuffer
                = bufferOrErr.get().get()->getMemBufferRef();

        // Build module from memory buffer
        llvm::LLVMContext context{};
        return llvm::parseBitcodeFile(memoryBuffer, context);
}

int main(int argc, char **argv) {
        // Get module from path
        std::string path = "res/llvm/hello.bc";
        llvm::Expected<std::unique_ptr<llvm::Module>> moduleOrErr
                = getModuleFromFile(path);
        if (auto err = moduleOrErr.takeError()) {
                return err_panic(err, -1);
        }

        // TODO: grab all the LLVM data
        // TODO: perform the analysis

        // Done.
        llvm::outs() << "Done.\n";
        return 0;
}
