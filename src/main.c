#include "../include/lc3.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <program.obj>\n", argv[0]);
        return 1;
    }

    lc3_vm_t vm;

    lc3_vm_init(&vm);

    if (!lc3_vm_load_program(&vm, argv[1]))
    {
        fprintf(stderr, "Error: Could not load program '%s'\n", argv[1]);
        return 1;
    }

    printf("LC-3 Virtual Machine\n");
    printf("Loaded program: %s\n", argv[1]);
    printf("Starting execution...\n\n");

    lc3_vm_run(&vm);

    lc3_vm_shutdown(&vm);

    printf("\nProgram terminated.\n");
    return 0;
}
