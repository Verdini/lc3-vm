#include "../include/lc3_asm_parser.h"
#include "../include/lc3_program.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Remove comments and trim whitespace
void clean_line(char *line)
{
    // Remove comments (everything after ';')
    char *comment = strchr(line, ';');
    if (comment)
        *comment = '\0';

    // Trim trailing whitespace
    int len = strlen(line);
    while (len > 0 && isspace(line[len - 1]))
    {
        line[--len] = '\0';
    }
}

// First pass: collect labels and calculate addresses
void first_pass(program_t *program, FILE *file)
{
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file))
    {
        clean_line(line);
        if (strlen(line) == 0)
            continue;

        // Check for .ORIG directive
        if (strstr(line, ".ORIG"))
        {
            char *token = strtok(line, " \t");
            token = strtok(NULL, " \t");
            if (token)
            {
                program->current_address = parse_number(token);
            }
            continue;
        }

        // Check for .END directive
        if (strstr(line, ".END"))
        {
            break;
        }

        // Check for label (ends with ':')
        char *colon = strchr(line, ':');
        if (colon)
        {
            *colon = '\0'; // Remove colon
            lc3_program_add_label(program, line);

            // Check if there's an instruction on the same line
            char *rest = colon + 1;
            clean_line(rest);
            if (strlen(rest) > 0)
            {
                program->current_address++;
            }
        }
        else
        {
            // Regular instruction
            program->current_address++;
        }
    }
}

// Second pass: generate machine code
void second_pass(program_t *program, FILE *file)
{
    char line[MAX_LINE_LENGTH];
    program->current_address = 0x3000; // Reset address

    rewind(file);

    while (fgets(line, sizeof(line), file))
    {
        clean_line(line);
        if (strlen(line) == 0)
            continue;

        // Handle .ORIG directive
        if (strstr(line, ".ORIG"))
        {
            char *token = strtok(line, " \t");
            token = strtok(NULL, " \t");
            if (token)
            {
                program->current_address = parse_number(token);
            }
            continue;
        }

        // Handle .END directive
        if (strstr(line, ".END"))
        {
            break;
        }

        // Handle labels
        char *colon = strchr(line, ':');
        if (colon)
        {
            char *rest = colon + 1;
            clean_line(rest);
            if (strlen(rest) > 0)
            {
                // Instruction on same line as label
                strcpy(line, rest);
            }
            else
            {
                continue; // Label only, no instruction
            }
        }

        // Parse instruction
        char line_copy[MAX_LINE_LENGTH];
        strcpy(line_copy, line);
        uint16_t instruction = parse_instruction(line_copy);

        if (instruction != 0)
        {
            program->instructions[program->instruction_count].address = program->current_address;
            program->instructions[program->instruction_count].instruction = instruction;
            program->instruction_count++;
        }

        program->current_address++;
    }
}

// Write object file
void write_object_file(program_t *program, const char *filename, uint16_t origin)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        fprintf(stderr, "Error: Could not create output file %s\n", filename);
        return;
    }

    // Write origin (big-endian)
    uint16_t origin_be = (origin << 8) | (origin >> 8);
    fwrite(&origin_be, sizeof(uint16_t), 1, file);

    // Write instructions (big-endian)
    for (int i = 0; i < program->instruction_count; i++)
    {
        uint16_t instr_be = (program->instructions[i].instruction << 8) |
                            (program->instructions[i].instruction >> 8);
        fwrite(&instr_be, sizeof(uint16_t), 1, file);
    }

    fclose(file);
    printf("Generated %s with %d instructions\n", filename, program->instruction_count);
}

char *lc3_asm_get_output_filename(const char *input_filename)
{
    // Create output filename by replacing .asm with .obj
    static char output_filename[256];
    strcpy(output_filename, input_filename);
    char *dot = strrchr(output_filename, '.');
    if (dot)
    {
        strcpy(dot, ".obj");
    }
    else
    {
        strcat(output_filename, ".obj");
    }
    return output_filename;
}

int lc3_asm_run(const char *input_filename)
{
    FILE *input_file = fopen(input_filename, "r");
    if (!input_file)
    {
        fprintf(stderr, "Error: Could not open input file %s\n", input_filename);
        return 1;
    }

    printf("Assembling %s...\n", input_filename);

    // Reset global state
    program_t *program = lc3_program_create();

    // Two-pass assembly
    printf("Pass 1: Collecting labels...\n");
    first_pass(program, input_file);

    printf("Pass 2: Generating code...\n");
    second_pass(program, input_file);

    fclose(input_file);

    // Write output file
    const char *output_filename = lc3_asm_get_output_filename(input_filename);
    write_object_file(program, output_filename, 0x3000);

    printf("Assembly complete!\n");
    return 0;
}
