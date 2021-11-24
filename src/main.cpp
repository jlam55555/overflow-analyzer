#include <llvm/IR/LLVMContext.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Support/MemoryBuffer.h>

using namespace std;

int main(int argc, char **argv) {

        // Reference OCaml code (TODO: remove)
        // let llctx = Llvm.global_context () in
        // let llmem = Llvm.MemoryBuffer.of_file path in
        // Llvm_bitreader.parse_bitcode llctx llmem

        // Read in bitcode from file
        llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> bufferOrErr
                = llvm::MemoryBuffer::getFile("res/llvm/hello.bc");
        if (auto err = llvm::errorCodeToError(bufferOrErr.getError())) {
                llvm::errs() << llvm::toString(std::move(err)) << "\n";
                return 1;
        }
        llvm::MemoryBufferRef memoryBuffer
                = bufferOrErr.get().get()->getMemBufferRef();

        // Build module from memory buffer
        llvm::LLVMContext context{};
        llvm::Expected<std::unique_ptr<llvm::Module>> moduleOrErr
                = llvm::parseBitcodeFile(memoryBuffer, context);
        if (auto err = moduleOrErr.takeError()) {
                llvm::errs() << llvm::toString(std::move(err)) << "\n";
                return 1;
        }
        llvm::Module *module = moduleOrErr.get().get();

        llvm::outs() << "Done.\n";
        return 0;
}
