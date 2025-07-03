#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/asm/asm.h"
#include "../include/vm/vm.h"

int run_assembler(const char* input_filename) {
  printf("LC-3 Assembler\n");
  int result = asm_run(input_filename);
  if (result != 0) {
    fprintf(stderr, "Assembly failed!\n");
    return result;
  }
  printf("Assembly completed successfully!\n");
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

  char* obj_filename = asm_get_output_filename(input_filename);
  result = run_vm(obj_filename);
  free(obj_filename);
  return result;
}

int main(int argc, char* argv[]) {
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
