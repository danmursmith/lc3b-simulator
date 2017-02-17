/*
 Remove all unnecessary lines (including this one)
 in this comment.
 REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

 Name 1: Nolan Corcoran
 Name 2: Full name of the second partner
 UTEID 1: ndc466
 UTEID 2: UT EID of the second partner
 */

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
 MEMORY[A][1] stores the most significant byte of word at word address A
 */

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];
int s;
/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

    int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
    int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {
    printf("----------------LC-3b ISIM Help-----------------------\n");
    printf("go               -  run program to completion         \n");
    printf("run n            -  execute program for n instructions\n");
    printf("mdump low high   -  dump memory from low to high      \n");
    printf("rdump            -  dump the register & bus values    \n");
    printf("?                -  display this help menu            \n");
    printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {

    process_instruction();
    CURRENT_LATCHES = NEXT_LATCHES;
    INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf(". for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == 0x0000) {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
    printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
    fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
        case 'G':
        case 'g':
            go();
            break;

        case 'M':
        case 'm':
            scanf("%i %i", &start, &stop);
            mdump(dumpsim_file, start, stop);
            break;

        case '?':
            help();
            break;
        case 'Q':
        case 'q':
            printf("Bye.\n");
            exit(0);

        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;

        default:
            printf("Invalid Command\n");
            break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
    FILE * prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) {
    int i;

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    /*
    if (argc < 2) {
        printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argc - 1);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }
    */
    printf("LC-3b Simulator\n\n");
    char *test = "test_branch.obj";
    initialize(test, 1);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }
    while (1)
        get_command(dumpsim_file);

}

/***************************************************************/
/*
 Do not modify the above code.
 You are allowed to use the following global variables in your
 code. These are defined above.

 MEMORY

 CURRENT_LATCHES
 NEXT_LATCHES

 You may define your own local/global variables and functions.
 You may use the functions to get at the control bits defined
 above.

 Begin your code here
*/

/***************************************************************/
enum
{
    BR, ADD, LDB, STB, JSR, AND, LDW, STW,
    RTI, XOR, NA1, NA2, JMP, SHF, LEA, TRAP
};

enum
{
    BR_, BRP, BRZ, BRZP, BRN, BRNP, BRNZ, BRNZP
};

enum
{
    LSHF, RSHFL, NA, RSHFA
};

int instruction,
    opcode,
    dr,
    sr1,
    sr2,
    sr,
    srVal,
    offset,
    immediate,
    condition,
    shift,
    loc,
    i,
    temp,
    base,
    baseVal,
    vector;

int SEXT32(int val)
{
    if (val & 0x8000)
    {
        val |= 0xFFFF0000;
    }
    return val;
}

int SEXT(int val, int msb)
{
    int i, mask;
    if (val & msb)
    {
        msb >>= 1;
        i = mask = msb;
        for (; i != 1; i /= 2)
        {
            mask >>= 1;
            mask += msb;
        }
        val |= ~mask;
    }
    return val;
}

int SEXT

void set_condition_bits()
{
    condition = SEXT32(NEXT_LATCHES.REGS[dr]);
    if (condition > 0)
    {
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;
    }
    else if (condition < 0)
    {
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }
    else
    {
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
}


void process_instruction(){
    /*  function: process_instruction
     *
     *    Process one instruction at a time
     *       -Fetch one instruction
     *       -Decode
     *       -Execute
     *       -Update NEXT_LATCHES
     */
    instruction = MEMORY[CURRENT_LATCHES.PC >> 1][0] + (MEMORY[CURRENT_LATCHES.PC >> 1][1] << 8);
    opcode = (instruction & 0xF000) >> 12;
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
    
    switch(opcode)
    {
        case ADD:
            dr = (instruction & 0x0E00) >> 9;
            sr1 = (instruction & 0x01C0) >> 6;
            if (instruction & 0x0020)
            {
                /* immediate */
                immediate = SEXT((instruction & 0x001F), 0x0010);
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT32(CURRENT_LATCHES.REGS[sr1]) + immediate);
                printf("ADD\t\tR%d, R%d, #%d\n", dr, sr1, immediate);
            }
            else
            {
                /* register */
                sr2 = (instruction & 0x0007);
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT32(CURRENT_LATCHES.REGS[sr1]) +
                                                  SEXT32(CURRENT_LATCHES.REGS[sr2]));
                printf("ADD\t\tR%d, R%d, R%d\n", dr, sr1, sr2);
            }
            set_condition_bits();
            break;

        case AND:
            dr = (instruction & 0x0E00) >> 9;
            sr1 = (instruction & 0x01C0) >> 6;
            if (instruction & 0x0020)
            {
                /* immediate */
                immediate = SEXT((instruction & 0x001F), 0x0010);
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT32(CURRENT_LATCHES.REGS[sr1]) & immediate);
                printf("AND\t\tR%d, R%d, #%d\n", dr, sr1, immediate);
            }
            else
            {
                /* register */
                sr2 = (instruction & 0x0007);
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT32(CURRENT_LATCHES.REGS[sr1]) &
                                                  SEXT32(CURRENT_LATCHES.REGS[sr2]));
                printf("AND\t\tR%d, R%d, R%d\n", dr, sr1, sr2);
            }
            set_condition_bits();
            break;

        case BR:
            condition = (instruction & 0x0E00) >> 9;
            offset = SEXT(instruction & 0x01FF, 0x0100) << 1;
            switch (condition)
            {
                case BR_:
                case BRNZP:
                    NEXT_LATCHES.PC += offset;
                    printf("BR\t\t#%d\n", offset);
                    break;
                case BRN:
                    if (CURRENT_LATCHES.N == 1)
                    {
                        NEXT_LATCHES.PC = offset;
                    }
                    printf("BRn\t\t#%d\n", offset);
                    break;
                case BRZ:
                    if (CURRENT_LATCHES.Z == 1)
                    {
                        NEXT_LATCHES.PC += offset;
                    }
                    printf("BRz\t\t#%d\n", offset);
                    break;
                case BRP:
                    if (CURRENT_LATCHES.P == 1)
                    {
                        NEXT_LATCHES.PC += offset;
                    }
                    printf("BRp\t\t#%d\n", offset);
                    break;
                case BRNZ:
                    if (CURRENT_LATCHES.N == 1 || CURRENT_LATCHES.Z == 1)
                    {
                        NEXT_LATCHES.PC += offset;
                    }
                    printf("BRnz\t#%d\n", offset);
                    break;
                case BRZP:
                    if (CURRENT_LATCHES.Z == 1 || CURRENT_LATCHES.P == 1)
                    {
                        NEXT_LATCHES.PC += offset;
                    }
                    printf("BRzp\t#%d\n", offset);
                    break;
                case BRNP:
                    if (CURRENT_LATCHES.N == 1 || CURRENT_LATCHES.P == 1)
                    {
                        NEXT_LATCHES.PC += offset;
                    }
                    printf("BRnp\t#%d\n", offset);
                    break;
            }
            break;

        case JMP:
            base = (instruction & 0x01C0) >> 6;
            printf("JMP\t\tR%d\n", base);
            NEXT_LATCHES.PC = base;
            break;

        case JSR:
            temp = NEXT_LATCHES.PC;
            //NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
            if (instruction & 0x0800)   /* JSR */
            {
                offset = SEXT((instruction & 0x07FF), 0x0800);
                NEXT_LATCHES.PC += offset;
                printf("JSR\t\t#%d\n", offset);
            }
            else                        /* JSRR */
            {
                base = (instruction & 0x01C0) >> 6;
                NEXT_LATCHES.PC = base;
                printf("JSRR\t\tR%d\n", base);
            }
            NEXT_LATCHES.REGS[7] = temp;
            break;

        case LDB:
            dr = (instruction & 0x0E00) >> 9;
            base = (instruction & 0x01C0) >> 6;
            offset = SEXT((instruction & 0x003F), 0x0020);
            baseVal = CURRENT_LATCHES.REGS[base];
            NEXT_LATCHES.REGS[dr] = Low16bits(SEXT(MEMORY[(baseVal + offset) >> 1][(baseVal + offset) & 0x0001], 0x0080));
            printf("LDB\t\tR%d, R%d, #%d\n", dr, base, offset);
            set_condition_bits();
            break;

        case LDW:
            dr = (instruction & 0x0E00) >> 9;
            base = (instruction & 0x01C0) >> 6;
            offset = SEXT((instruction & 0x003F), 0x0020) << 1;
            baseVal = CURRENT_LATCHES.REGS[(instruction & 0x01C0) >> 6];
            NEXT_LATCHES.REGS[dr] = MEMORY[(baseVal + offset) >> 1][0];
            NEXT_LATCHES.REGS[dr] += (MEMORY[(baseVal + offset) >> 1][1]) << 8;
            printf("LDW\t\tR%d, R%d, #%d\n", dr, base, offset);
            set_condition_bits();
            break;

        case LEA:
            dr = (instruction & 0x0E00) >> 9;
            offset = SEXT((instruction & 0x01FF), 0x0100) << 1;
            NEXT_LATCHES.REGS[dr] = offset + NEXT_LATCHES.PC;
            printf("LEA\t\tR%d, #%d\n", dr, offset);
            break;

        case XOR:
            dr = (instruction & 0x0E00) >> 9;
            sr1 = (instruction & 0x01C0) >> 6;
            if (instruction & 0x0020)
            {
                /* immediate */
                immediate = SEXT((instruction & 0x001F), 0x0010);
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT32(CURRENT_LATCHES.REGS[sr1]) ^ immediate);
                printf("XOR\t\tR%d, R%d, #%d\n", dr, sr1, immediate);
            }
            else
            {
                /* register */
                sr2 = (instruction & 0x0007);
                NEXT_LATCHES.REGS[dr] = Low16bits(SEXT32(CURRENT_LATCHES.REGS[sr1]) ^
                                                  SEXT32(CURRENT_LATCHES.REGS[sr2]));
                printf("XOR\t\tR%d, R%d, R%d\n", dr, sr1, sr2);
            }
            set_condition_bits();
            break;

        case SHF:
            dr = (instruction & 0x0E00) >> 9;
            sr = (instruction & 0x01C0) >> 6;
            srVal = CURRENT_LATCHES.REGS[sr];
            shift = (instruction & 0x0030) >> 4;
            immediate = SEXT((instruction & 0x000F), 0x0008);
            switch (shift)
            {
                case LSHF:
                    NEXT_LATCHES.REGS[dr] = Low16bits(srVal << immediate);
                    set_condition_bits();
                    printf("LSHF\tR%d, R%d, #%d\n", dr, sr, immediate);
                    break;
                case RSHFL:
                    for (i = 0; i < immediate; i++)
                    {
                        srVal = ~(1 << 15) & (srVal >> 1);
                    }
                    NEXT_LATCHES.REGS[dr] = Low16bits(srVal);
                    set_condition_bits();
                    printf("RSHFL\tR%d, R%d, #%d\n", dr, sr, immediate);
                    break;
                case RSHFA:
                    for (i = 0; i < immediate; i++)
                    {
                        srVal = (srVal & 0x8000) + (srVal >> 1);
                    }
                    NEXT_LATCHES.REGS[dr] = Low16bits(srVal);
                    set_condition_bits();
                    printf("RSHFA\tR%d, R%d, #%d\n", dr, sr, immediate);
                    break;
            }
            break;

        case STB:
            sr = (instruction & 0x0E00) >> 9;
            base = (instruction & 0x01C0) >> 6;
            offset = SEXT((instruction & 0x003F), 0x0020);
            /* execute */
            srVal = CURRENT_LATCHES.REGS[sr] & 0x00FF;
            baseVal = CURRENT_LATCHES.REGS[base];
            MEMORY[(baseVal + offset) >> 1][(baseVal + offset) & 0x0001] = srVal;
            printf("STB\t\tR%d, R%d, #%d\n", sr, base, offset);
            break;

        case STW:
            sr = (instruction & 0x0E00) >> 9;
            base = (instruction & 0x01C0) >> 6;
            offset = SEXT((instruction & 0x003F), 0x0020) << 1;
            /* execute */
            srVal = CURRENT_LATCHES.REGS[sr];
            baseVal = CURRENT_LATCHES.REGS[base];
            MEMORY[(baseVal + offset) >> 1][0] = srVal & 0x00FF;
            MEMORY[(baseVal + offset) >> 1][1] = (srVal & 0xFF00) >> 8;
            printf("STW\t\tR%d, R%d, #%d\n", sr, base, offset);
            break;

        case TRAP:
            NEXT_LATCHES.REGS[7] = NEXT_LATCHES.PC;
            vector = (instruction & 0x00FF);
            NEXT_LATCHES.PC = MEMORY[vector][0] + (MEMORY[vector][1] << 8);
            printf("TRAP\t\tx%x\n", vector);
            break;

        case NA1:
        case NA2:
            break;
    }

}
