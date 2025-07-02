#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/asm/asm.h"
#include "../include/vm/vm.h"

void run_assembler(const char* input_filename) {
  printf("LC-3 Assembler\n");
  int result = asm_run(input_filename);
  if (result != 0) {
    fprintf(stderr, "Assembly failed!\n");
    exit(result);
  }
  printf("Assembly completed successfully!\n");
}

void run_vm(const char* program_filename) {
  lc3_vm_t vm;
  vm_init(&vm);
  if (!vm_load_program(&vm, program_filename)) {
    fprintf(stderr, "Error: Could not load program '%s'\n", program_filename);
    return 1;
  }

  printf("LC-3 Virtual Machine\n");
  printf("Loaded program: %s\n", program_filename);
  printf("Starting execution...\n\n");
  vm_run(&vm);
  vm_shutdown(&vm);
  printf("\nProgram terminated.\n");
}

void run_assembler_vm(const char* input_filename) {
  run_assembler(input_filename);

  char* obj_filename = asm_get_output_filename(input_filename);
  run_vm(obj_filename);

  free(obj_filename);
}

int main(int argc, char* argv[]) {
  // Assembler mode: lc3_vm -c <input.asm>
  if (argc == 3 && strcmp(argv[1], "-c") == 0) {
    run_assembler(argv[2]);
    return 0;
  }
  // Assemble and run: lc3_vm -r <input.asm>
  else if (argc == 3 && strcmp(argv[1], "-r") == 0) {
    run_assembler_vm(argv[2]);
  }
  // VM only mode: <program.obj>
  else if (argc == 2) {
    run_vm(argv[1]);
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
