#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBuffer.h>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>

// Note: avoid `using namespace *` and `auto` typing so that namespaces and
// types are very explicit. Good for avoiding confusion for learning purposes.

/**
 * Helper function for printing a basic block and optionally the instructions
 * within the basic block
 */
void print_bb(const llvm::BasicBlock *bb, bool inst = false) {
	if (inst) {
		bb->print(llvm::errs());
	} else {
		llvm::errs() << "B: " << bb->getName() << "\n";
	}
}

/**
 * Helper function for printing a function and optionally all the instructions
 * in all the basic block
 */
void print_func(llvm::Function *f, bool inst = false) {
	llvm::errs() << "F: " << f->getName() << "\n";
	// print bbs
	for (llvm::ilist<llvm::BasicBlock>::iterator it = f->getBasicBlockList().begin();
             it != f->getBasicBlockList().end(); it++) {
		print_bb(&*it, inst);
	}
	llvm::errs() << "\n";
}

/**
 * Helper function to exit the program due to a `llvm::Error`. Will exit
 * the entire program with the given return code.
 */
void err_panic(llvm::Error &err, int code) {
	llvm::errs() << llvm::toString(std::move(err)) << "\n";
	exit(code);
}

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

/**
 * Returns a vector of (unmanaged) pointers to functions from a `llvm::Module`.
 */
std::vector<llvm::Function *>
getFunctionsFromModule(std::unique_ptr<llvm::Module> &module) {
	std::vector<llvm::Function *> fns{};

	for (llvm::Module::FunctionListType::iterator it =
                     module->functions().begin();
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

	for (llvm::ilist<llvm::BasicBlock>::iterator it =
                     fnc->getBasicBlockList().begin();
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
 * Analysis information for a single basic block.
 */
class BasicBlockAnalysis {
        // Dirty means that this basic block is new or one of its successors
        // has been updated since it was last updated.
        bool dirty = true;
        
public:
        bool is_dirty() {
                return dirty;
        }

        void mark_dirty() {
                dirty = true;
        }

        void mark_clean() {
                dirty = false;
        }
};

std::unordered_map<llvm::BasicBlock *, BasicBlockAnalysis> analysis;

/**
 * Analyze a single basic block, and returns true if updated.
 */
bool analyze_bb(llvm::BasicBlock *bb) {
        BasicBlockAnalysis *bb_analysis = &analysis[bb];

        // Only perform analysis if dirty.
        if (!bb_analysis->is_dirty()) {
                return false;
        }

        // TODO: do stuff
        llvm::outs() << "Got basic_block: " << bb->getName() << "\n";

        // Analysis complete; mark clean.
        bb_analysis->mark_clean();
        return true;
}

/**
 * Skeleton for the analysis algorithm.
 */
void dataflow_analysis(llvm::BasicBlock *entry_point) {
	std::queue<llvm::BasicBlock *> to_analyze{};
	to_analyze.push(entry_point);

	while (!to_analyze.empty()) {
		llvm::BasicBlock *bb = to_analyze.front();
		to_analyze.pop();

		// If analysis of the basic block changes any values,
		// then mark all successors dirty and queue them to analyze.
		if (analyze_bb(bb)) {
			for (llvm::succ_iterator succ_it = llvm::succ_begin(bb);
                             succ_it != llvm::succ_end(bb);
                             ++succ_it) {
                                analysis[*succ_it].mark_dirty();
                                to_analyze.push(*succ_it);
                        }
                }
        }
}

/**
 * Loop through all basic blocks of function, get all buffers, local
 * variables, and virtual registers.
 *
 * Virtual registers (vrs): all temporary values
 * Local variables (lvs): all temporary values that are allocated on the stack
 * Buffers (bufs): all local variables of array type.
 *
 * We need to keep track of information about all vrs in order to perform
 * the analysis, although we will only really report information about bufs.
 * Not sure yet if lvs will be useful.
 */
void get_all_lvars_buffers(llvm::Function *fn) {
        std::unordered_set<std::string> bufs{}, lvs{}, vrs{};

        // Loop through all basic blocks, get all of the above values.
        for (llvm::BasicBlock *bb : getBasicBlocksFromFunction(fn)) {
                for (llvm::BasicBlock::iterator it = bb->begin(); it != bb->end(); ++it) {
                        llvm::Instruction *inst = &*it;
//                        if (inst->getOpcode() == llvm::Instruction::Alloca) {
                                llvm::outs() << "Got instruction: " << *inst << "\n";
                                llvm::outs() << "\tName: " << inst->getName() << "\n";
                                
                                // llvm::outs() << "Type: " << *inst->getType() << "\n";
                                llvm::outs() << "\tArity: " << inst->getNumOperands() << "\n";
                                for (int i = 0; i < inst->getNumOperands(); ++i) {
                                        llvm::outs() << "\t\tOperand: " << *inst->getOperand(i) << "\n";
                                }
                                // llvm::outs() << "Opcode: " << inst->getOpcodeName() << "\n";
                                // llvm::outs() << "Metadata: " << inst->hasMetadata() << "\n";
//                        }
                }
        }
}

/**
 * Begins the overflow analysis.
 */
int main(int argc, char **argv) {
        // Get module from path
        std::string path = "res/llvm/bufs.bc";
        llvm::LLVMContext context{};
        llvm::Expected<std::unique_ptr<llvm::Module>> moduleOrErr =
                getModuleFromFile(path, context);
        if (llvm::Error err = moduleOrErr.takeError()) {
                err_panic(err, -1);
        }
        std::unique_ptr<llvm::Module> module = std::move(moduleOrErr.get());

        // // Print module (for debugging)
        // module->print(llvm::errs(), nullptr);

        // grab LLVM functions, basic blocks, instructions, lvars
        // std::vector<llvm::Function *> fns = getFunctionsFromModule(module);
        // std::vector<llvm::BasicBlock *> bbs = getBasicBlocksFromFunction(fns[0]);
        // std::vector<llvm::Instruction *> insts =
        //         getInstructionsFromBasicBlock(bbs[0]);

        // // need to get locals and operands from Instructions

        // llvm::errs() << *insts[0] << "\n\t";
        // llvm::errs() << insts[0]->getOpcodeName() << "\n\t";

        // // getting operands from an instruction
        // llvm::Instruction *inst = insts[0];
        // std::vector<llvm::Use *> opers{};
        // for (llvm::Use *it = inst->op_begin(); it != inst->op_end(); it++) {
        //         opers.push_back(&*it);
        // }

        // llvm::errs() << "Operand No: " << opers[0]->getOperandNo() << "\n\t\t";

        // llvm::errs() << *(insts[0]->getOperand(0)) << "\n\t\t";
        // llvm::errs() << insts[0]->getOperand(0)->getName() << "\n\t\t";

        // TODO: currently only worrying about the analysis of a single function,
        // will generalize to multiple functions later.

        // Get the main function (entry point to function).
        llvm::Function *main_fn = module->getFunction("main");
        print_func(main_fn, true);

        // TODO: get all pseudo-registers and buffers in function
        get_all_lvars_buffers(main_fn);

        // Perform the analysis.
        dataflow_analysis(&main_fn->getEntryBlock());

        // Done.
        llvm::outs() << "Done.\n";
        return 0;
}
