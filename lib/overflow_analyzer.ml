let llvm_path = "res/llvm/printnum.bc"

(* Grab the LLVM module from a LLVM bytecode file at [path] *)
let get_llm path =
  let llctx = Llvm.global_context () in
  let llmem = Llvm.MemoryBuffer.of_file path in
  Llvm_bitreader.parse_bitcode llctx llmem

(* Get the main function of a module [llm] *)
let get_main llm = match Llvm.lookup_function "main" llm with
  | Some fn -> fn
  | _ -> failwith "no main function"


(* use case *)
let llm = get_llm llvm_path
let main = get_main llm
let first_bb = Llvm.entry_block main
let first_instr = match Llvm.instr_begin first_bb with
  | Before inst -> inst
  | At_end _ -> failwith "no first instr"
let first_op = Llvm.instr_opcode first_instr
let op1 = Llvm.operand first_instr 0
let second_instr = match Llvm.instr_succ first_instr with
  | Before inst -> inst
  | At_end _ -> failwith "no first instr"
