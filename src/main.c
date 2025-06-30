#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/asm/asm.h"
#include "../include/vm/vm.h"

void run_assembler(const char* input_filename) {
  int result = asm_run(input_filename);
  if (result != 0) {
    fprintf(stderr, "Assembly failed!\n");
    exit(result);
  }
}

int run_vm(const char* program_filename) {
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
  return 0;
}

int main(int argc, char* argv[]) {
  // Check for assembler flag
  // lc3_vm -c <input.asm> for assembler only
  if (argc == 3 && strcmp(argv[1], "-c") == 0) {
    // Assembler only mode: -c <input.asm>
    printf("Assembling '%s'...\n", argv[2]);
    run_assembler(argv[2]);
    printf("Assembly complete.\n");
    return 0;
  }
  // lc3_vm -r <input.asm> for assembler and run
  else if (argc == 3 && strcmp(argv[1], "-r") == 0) {
    // Assemble and run mode: -r <input.asm>
    printf("Assembling '%s'...\n", argv[2]);
    run_assembler(argv[2]);
    printf("Assembly complete.\n\n");

    // Generate object filename by replacing .asm with .obj
    char* obj_filename = asm_get_output_filename(argv[2]);

    printf("Running '%s'...\n", obj_filename);
    return run_vm(obj_filename);
  }
  // lc3_vm <program.obj> to run a pre-assembled program
  else if (argc == 2) {
    // VM only mode: <program.obj>
    return run_vm(argv[1]);
  }
  // Show usage for invalid arguments
  else {
    printf("Error: Invalid arguments.\n\n");
    printf("LC-3 Assembler and Virtual Machine\n");
    printf("VM usage: %s <program.obj>\n", argv[0]);
    printf("Assembler usage: %s -c <input.asm>\n", argv[0]);
    printf("Assembler and VM usage: %s -r <input.asm>\n", argv[0]);
    return 1;
  }
}
