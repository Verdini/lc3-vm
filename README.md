# LC-3 Virtual Machine

An implementation of the LC-3 (Little Computer 3) virtual machine in C.

## Prerequisites

Install the required development tools:

```bash
make install-deps
```

Or manually install:
```bash
sudo apt update
sudo apt install -y build-essential gdb valgrind clang-format
```

## Building

### Quick Build (Debug)
```bash
make
```

### Build Options
```bash
make debug    # Build with debug symbols
make release  # Build optimized release version
make clean    # Clean build artifacts
```

## Running

```bash
# Run debug version
./bin/debug/lc3_vm examples/hello.obj

# Run release version
./bin/release/lc3_vm examples/hello.obj
```

## Development Workflow

### VS Code Tasks
- **Ctrl+Shift+P** → "Tasks: Run Task"
  - `Build Debug` - Build current file for debugging
  - `Build Release` - Build optimized version
  - `Build Project` - Build entire project
  - `Clean` - Clean build files
  - `Run with Valgrind` - Memory leak detection

### Debugging
- **F5** - Start debugging current file
- Set breakpoints by clicking left of line numbers
- Debug configurations available:
  - `Debug Current File` - Debug single C file
  - `Debug LC3 VM` - Debug the main VM with example program

### Code Formatting
```bash
make format    # Format all source files
```

### Memory Leak Detection
```bash
make valgrind  # Run with Valgrind
```

## Project Structure

```
lc3-vm/
├── src/           # Source files (.c)
├── include/       # Header files (.h)
├── examples/      # LC-3 example programs
├── tests/         # Unit tests
├── bin/           # Compiled binaries
│   ├── debug/     # Debug builds
│   └── release/   # Release builds
├── obj/           # Object files
├── .vscode/       # VS Code configuration
└── Makefile       # Build configuration
```

## Compiler Flags

### Debug Build
- `-g` - Debug symbols
- `-Wall -Wextra -Wpedantic` - Comprehensive warnings
- `-std=c17` - C17 standard

### Release Build
- `-O2` - Optimization level 2
- `-DNDEBUG` - Disable debug assertions

## LC-3 Implementation Status

- [x] Basic VM structure
- [x] Memory management
- [x] Program loading
- [ ] ADD instruction
- [ ] AND instruction
- [ ] NOT instruction
- [ ] Branch instructions
- [ ] Jump instructions
- [ ] Load/Store instructions
- [ ] Trap instructions
- [ ] Keyboard input handling

## Contributing

1. Follow the existing code style (use `make format`)
2. Add appropriate error handling
3. Write unit tests for new features
4. Test with Valgrind for memory leaks
5. Update documentation

## Resources

- [LC-3 ISA Reference](https://justinmeiners.github.io/lc3-vm/)
- [LC-3 Simulator](https://wchargin.com/lc3web/)
