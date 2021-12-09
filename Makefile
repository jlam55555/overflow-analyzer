# LLVM source files
LLVM_SOURCE_DIR=res/c
LLVM_OUT_DIR=res/llvm
SOURCE_FILES=$(shell ls $(LLVM_SOURCE_DIR)/*.c)
LLVM_FILES=$(SOURCE_FILES:$(LLVM_SOURCE_DIR)/%.c=$(LLVM_OUT_DIR)/%.ll)
LLVM_BC_FILES=$(SOURCE_FILES:$(LLVM_SOURCE_DIR)/%.c=$(LLVM_OUT_DIR)/%.bc)

# source files
SOURCE_DIR=src
BUILD_DIR=target
BINARY=overflow_analyzer
SOURCES=$(shell ls $(SOURCE_DIR)/*.cpp)
OBJECTS=$(SOURCES:$(SOURCE_DIR)/%.cpp=$(BUILD_DIR)/%.o)

LDFLAGS=$(shell llvm-config --ldflags)
CXXFLAGS=$(shell llvm-config --cxxflags)
OTHER_SOURCES=$(shell llvm-config --libs engine scalaropts)

all: build build_llvm build_bc

build_llvm: $(LLVM_FILES)
build_bc: $(LLVM_BC_FILES)

# Don't strictly need LLVM files, but these are textual
# and may be used for visual aid
$(LLVM_OUT_DIR)/%.ll: $(LLVM_SOURCE_DIR)/%.c
	clang -S -emit-llvm -o $@ $^

# Build LLVM bitcode for reading
$(LLVM_OUT_DIR)/%.bc: $(LLVM_SOURCE_DIR)/%.c
	clang -c -emit-llvm -o $@ $^ -fno-discard-value-names

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $^

build: $(BUILD_DIR)/$(BINARY)
$(BUILD_DIR)/$(BINARY): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(OTHER_SOURCES)

debug: CXXFLAGS+= -g
debug: build

run: all
	$(BUILD_DIR)/$(BINARY)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(LLVM_OUT_DIR)/*.ll $(LLVM_OUT_DIR)/*.bc
