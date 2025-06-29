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

# Find all .c files in src directory
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEBUG_OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/debug/%.o)
RELEASE_OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/release/%.o)

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
$(OBJDIR)/debug/%.o: $(SRCDIR)/%.c | $(OBJDIR)/debug
	$(CC) $(CFLAGS) -c $< -o $@

# Release object files
$(OBJDIR)/release/%.o: $(SRCDIR)/%.c | $(OBJDIR)/release
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

# Clean build files
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Install dependencies
install-deps:
	sudo apt update
	sudo apt install -y build-essential gdb valgrind clang-format

# Run with valgrind
valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt $(DEBUG_BINDIR)/$(TARGET)

# Format code
format:
	find $(SRCDIR) -name "*.c" -o -name "*.h" | xargs clang-format -i

# Run tests (if you add them later)
test: debug
	@echo "No tests configured yet"

.PHONY: all debug release clean install-deps valgrind format test
