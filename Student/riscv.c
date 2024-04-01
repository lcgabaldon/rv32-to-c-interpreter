#include <errno.h>  //errno
#include <stdint.h> //uint32_t
#include <stdio.h>  //printf() fprintf()
#include <stdlib.h> //malloc() & free()
#include <string.h> //strerror()
#include <ctype.h>  // for isspace()
#include "my_string.h"
#include "process_file.h"
#include "tokenizer.h"

#define N_REGISTERS ((size_t)32)
#define MEM_SIZE ((size_t)(1 << 10))

#define MAX_TOKENS 100 // this can be adjusted later
#ifndef LINE_SIZE
#define LINE_SIZE 100
#endif // this can be adjusted baised off what is needed

uint32_t r[N_REGISTERS];
int32_t pc = 0;
unsigned char mem[MEM_SIZE];

static void error_no_memory(void)
{
  fprintf(stderr, "No more memory available. Error: %s\n", strerror(errno));
}

void init_memory_elements(void)
{
  // Initialize registers
  for (size_t i = 0; i < N_REGISTERS; i++)
  {
    r[i] = ((uint32_t)0);
  }
  // Initialize all memory to 0
  for (size_t i = 0; i < MEM_SIZE; ++i)
  {
    mem[i] = ((unsigned char)0);
  }
  // Set sp to be the top part of the memory
  r[2] = MEM_SIZE;
}

// passes to to the switch case so that it can conduct the operation

const char *mnemonics[] = {
    "LB", "LW", "SB", "SW",
    "ADD", "ADDI", "SUB",
    "XOR", "XORI", "SLLI", "SRLI",
    "MV", "LI", "NEG", "NOT",
    "JAL", "J", "JALR", "JR"};

// need to print the tokens to see what is going on

int interpret_register(char *token)
{
  // Switch case to interpret register names and return their numerical values
  printf("Interpreting token: %s\n", token);
  switch (token[0])
  {
  case 'X':
    switch (token[1])
    {
    case '0':
      return 0;
    }
    return 0;
  case 'R':
    return 1;
  case 'S':
    switch (token[1])
    {
    case 'P':
      return 2;
    case '0':
      return 8;
    case '1':
      return 9;
    case '2':
      return 18;
    case '3':
      return 19;
    case '4':
      return 20;
    case '5':
      return 21;
    case '6':
      return 22;
    case '7':
      return 23;
    case '8':
      return 24;
    case '9':
      return 25;
    }
    break;
  case 'T':
    switch (token[1])
    {
    case 'P':
      return 4;
    case '0':
      return 5;
    case '1':
      return 5;
    case '2':
      return 5;
    case '3':
      return 28;
    case '4':
      return 29;
    case '5':
      return 30;
    case '6':
      return 31;
    }
    break;
  case 'A':
    switch (token[1])
    {
    case '0':
      return 10;
    case '1':
      return 11;
    case '2':
      return 12;
    case '3':
      return 13;
    case '4':
      return 14;
    case '5':
      return 15;
    case '6':
      return 16;
    case '7':
      return 17;
    }
    break;
  }
  return -1; // Invalid register name
}
/**
 * Fill out this function and use it to read interpret user input to execute
 * RV32 instructions. You may expect that a single, properly formatted RISC-V
 * instruction string will be passed as a parameter to this function.
 */
// Function to trim leading whitespace
char *trim(char *str)
{
  while (isspace((unsigned char)*str))
    str++;
  return str;
}

int interpret(char *instr)
{
  // Trim leading whitespace
  instr = trim(instr);

  char delims[] = ", ()";
  char **tokens = tokenize(instr, delims);
  if (tokens == NULL)
  {
    fprintf(stderr, "Error: Failed to tokenize instruction\n");
    return 0;
  }

  // Mnemonic should be the first token
  char *mnemonic = tokens[0];
  // Determine the instruction type based on mnemonic
  ssize_t instruction_type = -1;
  for (ssize_t i = 0; i < sizeof(mnemonics) / sizeof(mnemonics[0]); ++i)
  {
    if (strcmp(mnemonic, mnemonics[i]) == 0)
    {
      instruction_type = i;
      break; // Corrected from continue to break
    }
  }

  if (instruction_type == -1)
  {
    fprintf(stderr, "Error: Unrecognized mnemonic '%s'\n", mnemonic);
    // Assuming a cleanup function exists for freeing tokens
    free(tokens);
    return 0;
  }
  char *operands[MAX_TOKENS - 1]; // Assuming MAX_TOKENS includes the mnemonic
  int num_operands = 0;
  for (int i = 1; tokens[i] != NULL && i < MAX_TOKENS; i++, num_operands++)
  {
    operands[num_operands] = tokens[i];
  }
  // send to a function that is returning the register number
  //  need to make an interperte register switch case

  // Execute corresponding operation based on instruction type
  switch (instruction_type)
  {

  case 0: // LB
  {
    // Extract the destination register index and source register index
    int32_t rd = interpret_register(operands[0]);
    printf("%d\n", rd);
    int32_t imm = atoi(operands[1]); // imm
    printf("%d\n", imm);
    int32_t rs1 = interpret_register(operands[2]);
    printf("rs1: %d\n", rs1);
    // Calculate the effective address
    uint32_t effective_address = r[rs1] + imm;

    // Print the memory content at the effective address for debugging
    printf("Memory content at address (0x%08x): 0x%02x\n", effective_address, mem[effective_address]);

    // Load the byte from memory, sign-extend it to 32-bits, and store in rd
    int8_t byte_value = (int8_t)mem[effective_address]; // Load and sign-extend
    r[rd] = byte_value;                                 // Store in the register

    // Insert debugging print statements here
    printf("SP (r[2]) after LB: %u\n", r[2]);
    printf("Value loaded into X[12]: %d\n", r[rd]);
    break;
  }

  case 1: // this is for LW
  {
    uint32_t rd = interpret_register(operands[0]);
    uint32_t rs1 = interpret_register(operands[1]);
    int32_t imm = atoi(operands[2]);
    // memory calculation
    uint32_t address = r[rs1] + imm;

    r[rd] = (uint32_t)mem[address] |
            ((uint32_t)mem[address + 1] << 8) |
            ((uint32_t)mem[address + 2] << 16) |
            ((uint32_t)mem[address + 3] << 24);
    break;
  }

  case 2: // for SB
  {
    uint32_t rs1 = interpret_register(operands[1]); // remove from register name
    uint32_t rs2 = interpret_register(operands[2]);
    int32_t imm = atoi(operands[2]);
    // memory calculation
    uint32_t address = r[rs1] + imm;
    // Store byte from rs2 into memory
    mem[address] = (unsigned char)r[rs2];
    break;
  }

  case 3: // SW
  {
    int32_t rs1 = interpret_register(operands[0]); // A0
    int32_t imm = atoi(operands[1]);               // Immediate value
    int32_t rs2 = interpret_register(operands[2]); // SP

    // Calculate the effective address using SP and the immediate offset
    uint32_t address = r[rs2] + imm;
    printf("Address: %d\n", address);
    // Assuming a 32-bit word, write the value from rs1 to the calculated address
    mem[address] = r[rs1] & 0xFF;
    mem[address + 1] = (r[rs1] >> 8) & 0xFF;
    mem[address + 2] = (r[rs1] >> 16) & 0xFF;
    mem[address + 3] = (r[rs1] >> 24) & 0xFF;

    // Insert debugging print statements here
    printf("SP (r[2]) after SW: %u\n", r[2]);
    printf("Memory content at address (0x%08x): 0x%02x%02x%02x%02x\n",
           address,
           mem[address], mem[address + 1], mem[address + 2], mem[address + 3]);
    break;
  }

  case 4: // ADD
  {
    uint32_t rd = interpret_register(operands[0]); // remove from register name
    uint32_t rs1 = interpret_register(operands[1]);
    int32_t rs2 = interpret_register(operands[2]);
    // ADD and store in rd
    r[rd] = r[rs1] + r[rs2];
    break;
  }

  case 5: // ADDI
  {
    uint32_t rd = interpret_register(operands[0]);  // Interpret and get the index for rd
    uint32_t rs1 = interpret_register(operands[1]); // Interpret and get the index for rs1
    int32_t imm = atoi(operands[2]);                // Convert immediate value to integer

    // Perform ADDI operation
    r[rd] = r[rs1] + imm;

    // Print the value of r[2] (SP) to verify the operation
    break;
  }

  case 6: // SUB
  {
    uint32_t rd = interpret_register(operands[0]); // remove from register name
    uint32_t rs1 = interpret_register(operands[1]);
    int32_t rs2 = interpret_register(operands[2]);
    // sub operation
    r[rd] = r[rs1] - r[rs2];
    break;
  }

  case 7: // XOR
  {
    uint32_t rd = interpret_register(operands[0]); // remove from register name
    uint32_t rs1 = interpret_register(operands[1]);
    int32_t rs2 = interpret_register(operands[2]);
    // bitwise XOR
    r[rd] = r[rs1] ^ r[rs2];
    break;
  }

  case 8: // XORI
  {
    uint32_t rd = interpret_register(operands[0]); // remove from register name
    uint32_t rs1 = interpret_register(operands[1]);
    int32_t imm = atoi(operands[2]);
    // bitwise XOR with imm
    r[rd] = r[rs1] ^ imm;
    break;
  }

  case 9: // SLLI
  {
    uint32_t rd = interpret_register(operands[0]); // remove from register name
    uint32_t rs1 = interpret_register(operands[1]);
    int32_t imm = atoi(operands[2] + 1);
    // left shift and store in rd
    r[rd] = r[rs1] << imm;
    break;
  }

  case 10: // SRLI
  {
    uint32_t rd = interpret_register(operands[0]); // remove from register name
    uint32_t rs1 = interpret_register(operands[1]);
    int32_t imm = atoi(operands[2] + 1);
    // left shift and store in rd
    r[rd] = r[rs1] >> imm;
    break;
  }

  case 11: // MV
  {
    uint32_t rd = interpret_register(operands[0]); // Destination register
    uint32_t rs = interpret_register(operands[1]); // Source register

    if (rd != 0)
    {                // Ensure the destination is not X0
      r[rd] = r[rs]; // Perform the move operation
    }
    break;
  }

  case 12: // LI
  {
    printf("%s\n", operands[0]);
    uint32_t rd = interpret_register(operands[0]);
    uint32_t imm = atoi(operands[1]);
    printf("imm = %d\n", imm);
    // load imm into rd
    r[rd] = imm;
    printf("r[%d] = %d\n", rd, r[rd]);
    break;
  }

  case 13: // NEG
  {
    // Extract the source and destination register indices
    uint32_t rs = interpret_register(operands[1]); // Source register index (T5)
    printf("%d\n", rs);
    uint32_t rd = interpret_register(operands[0]); // Desination register index (A7)
    printf("%d\n", rd);
    r[rd] = ~r[rs] + 1; // Two's complement negation
    printf("r[%d] = %d\n", rd, r[rd]);
    break;
  }

  case 14: // NOT
  {
    uint32_t rs = interpret_register(operands[1]); // Source register index (T5)
    printf("%d\n", rs);
    uint32_t rd = interpret_register(operands[0]); // Desination register index (A7)
    printf("%d\n", rd);
    r[rd] = ~r[rs]; // Bitwise NOT
    printf("r[%d] = %d\n", rd, r[rd]);
    break;
  }

  case 15: // JAL
  {
    uint32_t rd = interpret_register(operands[0]); // Remove 'X' from register name
    int32_t imm = atoi(operands[1]);
    // Jump to the immediate address imm
    pc += imm;
    // Store the return address (PC + 4) in register rd
    r[rd] = pc + 4;
    break;
  }

  case 16: // J
  {
    int32_t imm = atoi(operands[0]);
    // jump to imm address
    pc += imm;
    break;
  }

  case 17: // JALR
  {
    uint32_t rd = interpret_register(operands[0]);  // Remove 'X' from register name
    uint32_t rs1 = interpret_register(operands[1]); // Remove 'X' from register name
    int32_t imm = atoi(operands[2]);
    // jump to rs + imm
    pc = r[rs1] + imm;
    // Store the return address (pc + 4) in register rd
    r[rd] = pc + 4;
    break;
  }

  case 18: // JR
  {
    uint32_t rs = atoi(operands[0] + 1); // Remove 'X' from register name
    // Jump to the address stored in register rs
    pc = r[rs];
    break;
  }

  default:
    fprintf(stderr, "Error: Instruction not yet implemented\n");
    break;
  }

  // free memory allocated for tokens
  free(tokens);

  // instruction worked
  return 1;
}

static int print_registers(char *fd_name)
{
  FILE *fptr;

  if ((fptr = fopen(fd_name, "w")) == NULL)
    return 1;

  // print all registers
  fprintf(fptr, "Registers:\n");
  for (size_t i = 0; i < N_REGISTERS; i++)
    fprintf(fptr, "X[%zu] = %u\n", i, r[i]);

  return 0;
}

int main(int argc, char **argv)
{
  FILE *file;
  char *buffer;

  if (argc != 3)
  {
    fprintf(stderr, "Only two parameters must be passed.\n");
    return 1;
  }

  if (process_file(argv[1]))
    return 1;
  if (open_file())
    return 1;

  // Initialize PC, registers and memory
  init_memory_elements();

  buffer = (char *)malloc((LINE_SIZE + 1) * sizeof(char));

  if (buffer == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for buffer\n");
    // Handle memory allocation failure (possibly exit)
  }

  while (pc > -1)
  {
    get_line(buffer, pc / 4); // Populate buffer with the line content
    printf("%s\n", buffer);
    if (!interpret(buffer))
    { // Pass the buffer to interpret
      printf("%s\n", buffer);
      fprintf(stderr, "Error interpreting instruction: %s\n", buffer);
      // Handle the error, perhaps break out of the loop
    }
    pc += 4;
  }

  close_file();
  free(buffer);

  return print_registers(argv[2]);
}