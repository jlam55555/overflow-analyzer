let llvm_path = "res/llvm/hello.bc"

(* Grab the LLVM module from a LLVM bytecode file at [path] *)
let get_llm path =
  let llctx = Llvm.global_context () in
  let llmem = Llvm.MemoryBuffer.of_file path in
  Llvm_bitreader.parse_bitcode llctx llmem

