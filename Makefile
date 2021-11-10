SOURCE_DIR=res/c
LLVM_DIR=res/llvm
SOURCE_FILES=$(shell ls $(SOURCE_DIR)/*.c)
LLVM_FILES=$(SOURCE_FILES:$(SOURCE_DIR)/%.c=$(LLVM_DIR)/%.ll)
LLVM_BC_FILES=$(SOURCE_FILES:$(SOURCE_DIR)/%.c=$(LLVM_DIR)/%.bc)

all: build_llvm build_bc

build_llvm: $(LLVM_FILES)

build_bc: $(LLVM_BC_FILES)

# Don't strictly need LLVM files, but these are textual
# and may be used for visual aid
$(LLVM_DIR)/%.ll: $(SOURCE_DIR)/%.c
	clang -S -emit-llvm -o $@ $^

# Build LLVM bitcode for reading
$(LLVM_DIR)/%.bc: $(SOURCE_DIR)/%.c
	clang -c -emit-llvm -o $@ $^

.PHONY: clean
clean:
	rm -rf $(LLVM_DIR)/*.ll
	rm -rf $(LLVM_DIR)/*.bc
