#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/asm/asm.h"
#include "../include/vm/vm.h"

char* change_filename_extension(const char* filename,
                                const char* new_extension) {
  char* new_filename = malloc(strlen(filename) + strlen(new_extension) + 1);
  strcpy(new_filename, filename);
  char* dot = strrchr(new_filename, '.');
  if (dot) {
    strcpy(dot, new_extension);
  }
  return new_filename;
}

int run_assembler_symbols(const char* input_filename) {
  printf("LC-3 Assembler Symbols\n");
  char* symbols_filename = change_filename_extension(input_filename, ".sym");
  int result = asm_symbol_run(input_filename, symbols_filename);
  if (result != 0) {
    fprintf(stderr, "Assembly symbol generation failed!\n");
    return result;
  }

  printf("Assembly symbol generation completed successfully!\n");
  free(symbols_filename);
  return 0;
}

int run_assembler(const char* input_filename) {
  printf("LC-3 Assembler\n");
  char* obj_filename = change_filename_extension(input_filename, ".obj");
  int result = asm_run(input_filename, obj_filename);

  if (result != 0) {
    fprintf(stderr, "Assembly failed!\n");
    return result;
  }

  printf("Assembly completed successfully!\n");
  free(obj_filename);
  return 0;
}

int run_vm(const char* program_filename) {
  printf("LC-3 Virtual Machine\n");
  return vm_run(program_filename);
}

int run_assembler_vm(const char* input_filename) {
  int result = run_assembler(input_filename);
  if (result != 0) {
    fprintf(stderr, "Assembly failed! Cannot run VM.\n");
    return result;
  }

  char* obj_filename = change_filename_extension(input_filename, ".obj");
  result = run_vm(obj_filename);
  free(obj_filename);
  return result;
}

int main(int argc, char* argv[]) {
  // Assembler symbol mode generation: lc3_vm -s <input.asm>
  if (argc == 3 && strcmp(argv[1], "-s") == 0) {
    return run_assembler_symbols(argv[2]);
  }
  // Assembler mode: lc3_vm -c <input.asm>
  if (argc == 3 && strcmp(argv[1], "-c") == 0) {
    return run_assembler(argv[2]);
  }
  // Assemble and run: lc3_vm -r <input.asm>
  else if (argc == 3 && strcmp(argv[1], "-r") == 0) {
    return run_assembler_vm(argv[2]);
  }
  // VM only mode: <program.obj>
  else if (argc == 2) {
    return run_vm(argv[1]);
  }
  // Invalid options: Show usage
  else {
    printf("Error: Invalid arguments.\n\n");
    printf("LC-3 Assembler and Virtual Machine\n");
    printf("VM usage: %s <program.obj>\n", argv[0]);
    printf("Assembler usage: %s -c <input.asm>\n", argv[0]);
    printf("Assembler and VM usage: %s -r <input.asm>\n", argv[0]);
    return 1;
  }
}
