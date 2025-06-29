#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_LABELS 100
#define MAX_INSTRUCTIONS 1000

// Label structure for symbol table
typedef struct
{
    char name[64];
    uint16_t address;
} label_t;

// Instruction structure
typedef struct
{
    uint16_t address;
    uint16_t instruction;
} instruction_t;

// Global state
static label_t labels[MAX_LABELS];
static int label_count = 0;
static instruction_t instructions[MAX_INSTRUCTIONS];
static int instruction_count = 0;
static uint16_t current_address = 0x3000;

// Register name to number mapping
int get_register_number(const char *reg_str)
{
    if (reg_str[0] != 'R' && reg_str[0] != 'r')
        return -1;
    char digit = reg_str[1];
    if (digit >= '0' && digit <= '7')
    {
        return digit - '0';
    }
    return -1;
}

// Convert string to integer (handles decimal and hex)
int parse_number(const char *str)
{
    if (str[0] == '#')
    {
        return atoi(str + 1); // Decimal immediate
    }
    else if (str[0] == 'x' || str[0] == 'X')
    {
        return (int)strtol(str + 1, NULL, 16); // Hexadecimal
    }
    else if (strncmp(str, "0x", 2) == 0)
    {
        return (int)strtol(str + 2, NULL, 16); // Hexadecimal with 0x prefix
    }
    return atoi(str); // Default to decimal
}

// Find label address
int find_label(const char *label_name)
{
    for (int i = 0; i < label_count; i++)
    {
        if (strcmp(labels[i].name, label_name) == 0)
        {
            return labels[i].address;
        }
    }
    return -1; // Label not found
}

// Add label to symbol table
void add_label(const char *name, uint16_t address)
{
    if (label_count < MAX_LABELS)
    {
        strcpy(labels[label_count].name, name);
        labels[label_count].address = address;
        label_count++;
    }
}

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

// Parse ADD instruction
uint16_t parse_add(char *tokens[], int token_count)
{
    if (token_count < 4)
        return 0;

    int dr = get_register_number(tokens[1]);
    int sr1 = get_register_number(tokens[2]);

    if (dr == -1 || sr1 == -1)
        return 0;

    uint16_t instruction = 0x1000 | (dr << 9) | (sr1 << 6);

    // Check if third operand is register or immediate
    if (tokens[3][0] == '#')
    {
        // Immediate mode
        int imm = parse_number(tokens[3]);
        if (imm < -16 || imm > 15)
            return 0; // 5-bit signed immediate
        instruction |= 0x20 | (imm & 0x1F);
    }
    else
    {
        // Register mode
        int sr2 = get_register_number(tokens[3]);
        if (sr2 == -1)
            return 0;
        instruction |= sr2;
    }

    return instruction;
}

// Parse AND instruction
uint16_t parse_and(char *tokens[], int token_count)
{
    if (token_count < 4)
        return 0;

    int dr = get_register_number(tokens[1]);
    int sr1 = get_register_number(tokens[2]);

    if (dr == -1 || sr1 == -1)
        return 0;

    uint16_t instruction = 0x5000 | (dr << 9) | (sr1 << 6);

    // Check if third operand is register or immediate
    if (tokens[3][0] == '#')
    {
        // Immediate mode
        int imm = parse_number(tokens[3]);
        if (imm < -16 || imm > 15)
            return 0; // 5-bit signed immediate
        instruction |= 0x20 | (imm & 0x1F);
    }
    else
    {
        // Register mode
        int sr2 = get_register_number(tokens[3]);
        if (sr2 == -1)
            return 0;
        instruction |= sr2;
    }

    return instruction;
}

// Parse NOT instruction
uint16_t parse_not(char *tokens[], int token_count)
{
    if (token_count < 3)
        return 0;

    int dr = get_register_number(tokens[1]);
    int sr = get_register_number(tokens[2]);

    if (dr == -1 || sr == -1)
        return 0;

    return 0x9000 | (dr << 9) | (sr << 6) | 0x3F;
}

// Parse LEA instruction
uint16_t parse_lea(char *tokens[], int token_count)
{
    if (token_count < 3)
        return 0;

    int dr = get_register_number(tokens[1]);
    if (dr == -1)
        return 0;

    // For now, just handle immediate offsets
    int offset = parse_number(tokens[2]);
    if (offset < -256 || offset > 255)
        return 0; // 9-bit signed offset

    return 0xE000 | (dr << 9) | (offset & 0x1FF);
}

// Parse TRAP instruction
uint16_t parse_trap(char *tokens[], int token_count)
{
    if (token_count < 2)
        return 0;

    int trap_vector = parse_number(tokens[1]);
    if (trap_vector < 0 || trap_vector > 255)
        return 0;

    return 0xF000 | (trap_vector & 0xFF);
}

// Parse BR instruction
uint16_t parse_br(char *tokens[], int token_count)
{
    if (token_count < 2)
        return 0;

    // Default to unconditional branch (BRnzp)
    uint16_t condition = 0x0E00; // n=1, z=1, p=1

    // Check for specific branch conditions
    char *opcode = tokens[0];
    if (strcmp(opcode, "BRn") == 0 || strcmp(opcode, "brn") == 0)
    {
        condition = 0x0800; // n=1
    }
    else if (strcmp(opcode, "BRz") == 0 || strcmp(opcode, "brz") == 0)
    {
        condition = 0x0400; // z=1
    }
    else if (strcmp(opcode, "BRp") == 0 || strcmp(opcode, "brp") == 0)
    {
        condition = 0x0200; // p=1
    }
    else if (strcmp(opcode, "BRzp") == 0 || strcmp(opcode, "brzp") == 0)
    {
        condition = 0x0600; // z=1, p=1
    }
    else if (strcmp(opcode, "BRnp") == 0 || strcmp(opcode, "brnp") == 0)
    {
        condition = 0x0A00; // n=1, p=1
    }
    else if (strcmp(opcode, "BRnz") == 0 || strcmp(opcode, "brnz") == 0)
    {
        condition = 0x0C00; // n=1, z=1
    }

    int offset = parse_number(tokens[1]);
    if (offset < -256 || offset > 255)
        return 0; // 9-bit signed offset

    return 0x0000 | condition | (offset & 0x1FF);
}

// Parse a single instruction line
uint16_t parse_instruction(char *line)
{
    char *tokens[10];
    int token_count = 0;

    // Tokenize the line
    char *token = strtok(line, " \t,");
    while (token && token_count < 10)
    {
        tokens[token_count++] = token;
    }

    if (token_count == 0)
        return 0;

    // Convert opcode to uppercase for comparison
    for (char *p = tokens[0]; *p; p++)
        *p = toupper(*p);

    // Parse different instruction types
    if (strcmp(tokens[0], "ADD") == 0)
    {
        return parse_add(tokens, token_count);
    }
    else if (strcmp(tokens[0], "AND") == 0)
    {
        return parse_and(tokens, token_count);
    }
    else if (strcmp(tokens[0], "NOT") == 0)
    {
        return parse_not(tokens, token_count);
    }
    else if (strcmp(tokens[0], "LEA") == 0)
    {
        return parse_lea(tokens, token_count);
    }
    else if (strcmp(tokens[0], "TRAP") == 0)
    {
        return parse_trap(tokens, token_count);
    }
    else if (strncmp(tokens[0], "BR", 2) == 0)
    {
        return parse_br(tokens, token_count);
    }
    else if (strcmp(tokens[0], "HALT") == 0)
    {
        return 0xF025; // TRAP x25
    }
    else if (strcmp(tokens[0], "PUTS") == 0)
    {
        return 0xF022; // TRAP x22
    }
    else if (strcmp(tokens[0], "GETC") == 0)
    {
        return 0xF020; // TRAP x20
    }
    else if (strcmp(tokens[0], "OUT") == 0)
    {
        return 0xF021; // TRAP x21
    }

    printf("Warning: Unknown instruction '%s'\n", tokens[0]);
    return 0;
}

// First pass: collect labels and calculate addresses
void first_pass(FILE *file)
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
                current_address = parse_number(token);
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
            add_label(line, current_address);

            // Check if there's an instruction on the same line
            char *rest = colon + 1;
            clean_line(rest);
            if (strlen(rest) > 0)
            {
                current_address++;
            }
        }
        else
        {
            // Regular instruction
            current_address++;
        }
    }
}

// Second pass: generate machine code
void second_pass(FILE *file)
{
    char line[MAX_LINE_LENGTH];
    current_address = 0x3000; // Reset address

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
                current_address = parse_number(token);
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
            instructions[instruction_count].address = current_address;
            instructions[instruction_count].instruction = instruction;
            instruction_count++;
        }

        current_address++;
    }
}

// Write object file
void write_object_file(const char *filename, uint16_t origin)
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
    for (int i = 0; i < instruction_count; i++)
    {
        uint16_t instr_be = (instructions[i].instruction << 8) | (instructions[i].instruction >> 8);
        fwrite(&instr_be, sizeof(uint16_t), 1, file);
    }

    fclose(file);
    printf("Generated %s with %d instructions\n", filename, instruction_count);
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
    label_count = 0;
    instruction_count = 0;
    current_address = 0x3000;

    // Two-pass assembly
    printf("Pass 1: Collecting labels...\n");
    first_pass(input_file);

    printf("Pass 2: Generating code...\n");
    second_pass(input_file);

    fclose(input_file);

    // Write output file
    const char *output_filename = lc3_asm_get_output_filename(input_filename);
    write_object_file(output_filename, 0x3000);

    printf("Assembly complete!\n");
    return 0;
}
