# Makefile for LC-3 Virtual Machine
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c17 -g
RELEASE_FLAGS = -O2 -DNDEBUG
TARGET = lc3_vm
SRCDIR = src
OBJDIR = obj
BINDIR = bin
DEBUG_BINDIR = $(BINDIR)/debug
RELEASE_BINDIR = $(BINDIR)/release
TESTDIR = test

# Find all .c files in src directory and subdirectories
SOURCES = $(shell find $(SRCDIR) -name "*.c")
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEBUG_OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/debug/%.o)
RELEASE_OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/release/%.o)

# Test sources (excluding main.c)
VM_SOURCES = $(filter-out $(SRCDIR)/main.c, $(shell find $(SRCDIR) -name "*.c"))
VM_TEST_OBJECTS = $(VM_SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/debug/%.o)

# Combined test file
TEST_SRC = $(TESTDIR)/test.c

# Default target
all: debug

# Debug build
debug: $(DEBUG_BINDIR)/$(TARGET)

# Release build
release: $(RELEASE_BINDIR)/$(TARGET)

# Debug executable
$(DEBUG_BINDIR)/$(TARGET): $(DEBUG_OBJECTS) | $(DEBUG_BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

# Release executable
$(RELEASE_BINDIR)/$(TARGET): $(RELEASE_OBJECTS) | $(RELEASE_BINDIR)
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $^ -o $@

# Debug object files
$(OBJDIR)/debug/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Release object files
$(OBJDIR)/release/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# Create directories
$(OBJDIR)/debug:
	mkdir -p $(OBJDIR)/debug

$(OBJDIR)/release:
	mkdir -p $(OBJDIR)/release

$(DEBUG_BINDIR):
	mkdir -p $(DEBUG_BINDIR)

$(RELEASE_BINDIR):
	mkdir -p $(RELEASE_BINDIR)

# Test executable
$(DEBUG_BINDIR)/test: $(VM_TEST_OBJECTS) $(TEST_SRC) | $(DEBUG_BINDIR)
	$(CC) $(CFLAGS) $(VM_TEST_OBJECTS) $(TEST_SRC) -o $@

# Run tests
test: $(DEBUG_BINDIR)/test
	$(DEBUG_BINDIR)/test

# Install dependencies
install-deps:
	sudo apt update
	sudo apt install -y build-essential gdb valgrind clang-format

# Clean build files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Format code
format:
	find $(SRCDIR) include examples $(TESTDIR) -name "*.c" -o -name "*.h" | xargs clang-format -i --style=Google

# Run with valgrind
valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt $(DEBUG_BINDIR)/$(TARGET)

.PHONY: all debug release install-deps clean format test valgrind
