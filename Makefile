# ocaml source files
OCAML_SOURCE_FILES=$(shell ls **/*.ml)

# dune config
OCAML_BUILD_DIR=_build
OCAML_BINARY=$(OCAML_BUILD_DIR)/default/bin/main.exe

# c input files
SOURCE_DIR=res/c
LLVM_DIR=res/llvm
SOURCE_FILES=$(shell ls $(SOURCE_DIR)/*.c)
LLVM_FILES=$(SOURCE_FILES:$(SOURCE_DIR)/%.c=$(LLVM_DIR)/%.ll)
LLVM_BC_FILES=$(SOURCE_FILES:$(SOURCE_DIR)/%.c=$(LLVM_DIR)/%.bc)

# documentation
BROWSER=xdg-open
DOC_ROOT=_build/default/_doc/_html/index.html

all: build_llvm build_bc

build_llvm: $(LLVM_FILES)

build_bc: $(LLVM_BC_FILES)

run: $(OCAML_BINARY)
	dune exec overflow_analyzer

build: $(OCAML_BINARY)
$(OCAML_BINARY): $(OCAML_SOURCE_FILES)
	dune build bin/main.exe

show_docs: docs
	$(BROWSER) $(DOC_ROOT)

docs: $(DOC_ROOT)
$(DOC_ROOT): $(OCAML_SOURCE_FILES)
	dune build @doc

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
	rm -rf $(OCAML_BUILD_DIR)
