let llvm_path = "res/llvm/hello.bc"

let _ =
  let llctx = Llvm.global_context () in
  let llmem = Llvm.MemoryBuffer.of_file llvm_path in
  let llm = Llvm_bitreader.parse_bitcode llctx llmem in
  Llvm.dump_module llm
