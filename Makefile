# ==============================================================================
# Cargo-like Makefile for C projects
# ==============================================================================

# --- Project config -----------------------------------------------------------

PROJECT_NAME := my_project
CC           := gcc
STD          := c17

# --- Directories --------------------------------------------------------------

SRC_DIR     := src
INC_DIR     := include
TEST_DIR    := tests
TARGET_DIR  := target

DEBUG_DIR   := $(TARGET_DIR)/debug
RELEASE_DIR := $(TARGET_DIR)/release

DEBUG_OBJ_DIR   := $(DEBUG_DIR)/obj
RELEASE_OBJ_DIR := $(RELEASE_DIR)/obj

# --- Sources ------------------------------------------------------------------

SRCS       := $(shell find $(SRC_DIR) -name '*.c')
TEST_SRCS  := $(shell find $(TEST_DIR) -name '*.c' 2>/dev/null)

# Non-main sources for linking with test runner (avoids duplicate main)
LIB_SRCS   := $(filter-out $(SRC_DIR)/main.c, $(SRCS))

DEBUG_OBJS   := $(patsubst $(SRC_DIR)/%.c, $(DEBUG_OBJ_DIR)/%.o,   $(SRCS))
RELEASE_OBJS := $(patsubst $(SRC_DIR)/%.c, $(RELEASE_OBJ_DIR)/%.o, $(SRCS))

# --- Flags --------------------------------------------------------------------

COMMON_FLAGS := -std=$(STD) -Wall -Wextra -Wpedantic -I$(INC_DIR)
DEBUG_FLAGS  := $(COMMON_FLAGS) -g -O0 -DDEBUG -fsanitize=address,undefined
RELEASE_FLAGS:= $(COMMON_FLAGS) -O3 -DNDEBUG -march=native

LDFLAGS      :=
# e.g., LDFLAGS := -lm -lpthread

# --- Binaries -----------------------------------------------------------------

DEBUG_BIN   := $(DEBUG_DIR)/$(PROJECT_NAME)
RELEASE_BIN := $(RELEASE_DIR)/$(PROJECT_NAME)
TEST_BIN    := $(DEBUG_DIR)/$(PROJECT_NAME)_test

# ==============================================================================
# Targets
# ==============================================================================

.DEFAULT_GOAL := build

# --- init ---------------------------------------------------------------------
# Equivalent to: cargo init
# Scaffolds the project structure if not already present

.PHONY: init
init:
	@echo " Initializing  $(PROJECT_NAME)"
	@mkdir -p $(SRC_DIR) $(INC_DIR) $(TEST_DIR)

	@if [ ! -f $(SRC_DIR)/main.c ]; then \
		echo "    Creating  $(SRC_DIR)/main.c"; \
		printf '#include <stdio.h>\n\nint main(void) {\n\tprintf("Hello, from $(PROJECT_NAME)");\n\treturn 0;\n}\n' \
			> $(SRC_DIR)/main.c; \
	else \
		echo "    Skipping  $(SRC_DIR)/main.c (already exists)"; \
	fi

	@if [ ! -f $(TEST_DIR)/test_main.c ]; then \
		echo "    Creating  $(TEST_DIR)/test_main.c"; \
		printf '#include <stdio.h>\n#include <assert.h>\n\nvoid test_example(void) {\n\tassert(1 == 1);\n\tprintf("test_example ... ok\\n");\n}\n\nint main(void) {\n\ttest_example();\n\treturn 0;\n}\n' \
			> $(TEST_DIR)/test_main.c; \
	else \
		echo "    Skipping  $(TEST_DIR)/test_main.c (already exists)"; \
	fi

	@if [ ! -f .clang-format ]; then \
		echo "    Creating  .clang-format"; \
		printf 'BasedOnStyle: LLVM\nIndentWidth: 4\nTabWidth: 4\nUseTab: Never\nColumnLimit: 100\n' \
			> .clang-format; \
	else \
		echo "    Skipping  .clang-format (already exists)"; \
	fi

	@if [ ! -f .gitignore ]; then \
		echo "    Creating  .gitignore"; \
		printf '$(TARGET_DIR)/\n*.o\n*.d\n' > .gitignore; \
	else \
		echo "    Skipping  .gitignore (already exists)"; \
	fi

	@echo "    Finished  init — run \`make build\` to compile"

# --- build (debug) ------------------------------------------------------------
# Equivalent to: cargo build

.PHONY: build
build: $(DEBUG_BIN)
	@echo "    Finished  debug [unoptimized + debuginfo] target(s)"

$(DEBUG_BIN): $(DEBUG_OBJS)
	@echo "     Linking  $(DEBUG_BIN)"
	@$(CC) $(DEBUG_FLAGS) $^ -o $@ $(LDFLAGS)

$(DEBUG_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "   Compiling  $<"
	@$(CC) $(DEBUG_FLAGS) -c $< -o $@

# --- release ------------------------------------------------------------------
# Equivalent to: cargo build --release

.PHONY: release
release: $(RELEASE_BIN)
	@echo "    Finished  release [optimized] target(s)"

$(RELEASE_BIN): $(RELEASE_OBJS)
	@echo "     Linking  $(RELEASE_BIN)"
	@$(CC) $(RELEASE_FLAGS) $^ -o $@ $(LDFLAGS)

$(RELEASE_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "   Compiling  $<"
	@$(CC) $(RELEASE_FLAGS) -c $< -o $@

# --- run ----------------------------------------------------------------------
# Equivalent to: cargo run

.PHONY: run
run: build
	@echo "     Running  $(DEBUG_BIN)"
	@echo "---"
	@$(DEBUG_BIN) $(ARGS)

# --- run-release --------------------------------------------------------------
# Equivalent to: cargo run --release

.PHONY: run-release
run-release: release
	@echo "     Running  $(RELEASE_BIN)"
	@echo "---"
	@$(RELEASE_BIN) $(ARGS)

# --- test ---------------------------------------------------------------------
# Equivalent to: cargo test
# Compiles all *.c in tests/ linked against non-main lib sources

.PHONY: test
test: $(TEST_BIN)
	@echo "     Running  tests"
	@echo "---"
	@$(TEST_BIN)

$(TEST_BIN): $(TEST_SRCS) $(LIB_SRCS)
	@mkdir -p $(DEBUG_DIR)
	@echo "   Compiling  tests"
	@$(CC) $(DEBUG_FLAGS) $^ -o $@ $(LDFLAGS)
	@echo "    Finished  test [unoptimized + debuginfo] target(s)"

# --- check --------------------------------------------------------------------
# Equivalent to: cargo check
# Syntax-checks all sources without producing binaries

.PHONY: check
check:
	@echo "    Checking  $(PROJECT_NAME)"
	@$(CC) $(COMMON_FLAGS) -fsyntax-only $(SRCS)
	@echo "    Finished  checked [$(words $(SRCS)) file(s)]"

# --- fmt ----------------------------------------------------------------------
# Equivalent to: cargo fmt
# Requires clang-format; customize style via .clang-format or --style flag

.PHONY: fmt
fmt:
	@command -v clang-format >/dev/null 2>&1 || \
		{ echo "error: clang-format not found (install via your package manager)"; exit 1; }
	@echo "  Formatting  $(PROJECT_NAME)"
	@find $(SRC_DIR) $(INC_DIR) $(TEST_DIR) -name '*.c' -o -name '*.h' 2>/dev/null \
		| xargs clang-format -i --style=file
	@echo "    Finished  fmt"

# --- lint ---------------------------------------------------------------------
# Equivalent to: cargo clippy
# Requires clang-tidy; customize checks via .clang-tidy

.PHONY: lint
lint:
	@command -v clang-tidy >/dev/null 2>&1 || \
		{ echo "error: clang-tidy not found (install via your package manager)"; exit 1; }
	@echo "      Linting  $(PROJECT_NAME)"
	@clang-tidy $(SRCS) -- $(COMMON_FLAGS)
	@echo "    Finished  lint"

# --- clean --------------------------------------------------------------------
# Equivalent to: cargo clean

.PHONY: clean
clean:
	@echo "     Cleaning  target directory"
	@rm -rf $(TARGET_DIR)
	@echo "    Finished  clean"

# --- help ---------------------------------------------------------------------

.PHONY: help
help:
	@echo "Usage: make [target] [ARGS='...']"
	@echo ""
	@echo "  init         Scaffold project structure      (cargo init)"
	@echo "  build        Compile in debug mode           (cargo build)"
	@echo "  release      Compile in release mode         (cargo build --release)"
	@echo "  run          Build and run debug binary      (cargo run)"
	@echo "  run-release  Build and run release binary    (cargo run --release)"
	@echo "  test         Build and run tests             (cargo test)"
	@echo "  check        Syntax check, no output binary  (cargo check)"
	@echo "  fmt          Format source with clang-format (cargo fmt)"
	@echo "  lint         Lint source with clang-tidy     (cargo clippy)"
	@echo "  clean        Remove target/                  (cargo clean)"
