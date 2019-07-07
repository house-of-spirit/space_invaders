#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "arcade.h"
#include "instruction.h"
#include "rom.h"
#include "emulator.h"

/* cc *.c -o arcade -g */

void setup_arcade(arcade_t *arcade)
{
    arcade->mem = malloc(sizeof *arcade->mem);
}


void arcade_context(arcade_t *arcade, char *buf)
{
    char *result = buf;
    
    #define ASCII_GREEN "\033[0;32m"
    #define ASCII_RED "\033[0;31m"
    #define ASCII_BOLD "\033[1m"
    #define ASCII_MAGENTA "\033[35;1m"
    #define ASCII_STOP "\033[0m"

    #define REG_STR(X, B) ASCII_GREEN #X ASCII_STOP ": " ASCII_RED "0x%0" #B "x" ASCII_STOP "\n", arcade->X
    #define FLAG(X) (arcade->FLAGS.X ? " " #X " " : "")

    size_t offset = 30;
    ins_t **inss = parse_n_ins(&arcade->mem->mem[arcade->PC], 8);
    char *disass = inss_to_str(inss, arcade->PC);
    
    memset(result, '-', 30);
    offset += sprintf(result+offset, " " ASCII_BOLD ASCII_MAGENTA "REGISTERS" ASCII_STOP "\n");
    offset += sprintf(result+offset, REG_STR(A, 2));
    offset += sprintf(result+offset, REG_STR(B, 2));
    offset += sprintf(result+offset, REG_STR(C, 2));
    offset += sprintf(result+offset, REG_STR(D, 2));
    offset += sprintf(result+offset, REG_STR(E, 2));
    offset += sprintf(result+offset, REG_STR(H, 2));
    offset += sprintf(result+offset, REG_STR(L, 2));
    offset += sprintf(result+offset, ASCII_GREEN "M" ASCII_STOP ": " ASCII_RED "0x%02x" ASCII_STOP "\n", arcade->mem->mem[arcade->HL]);
    offset += sprintf(result+offset, "\n\n");
    offset += sprintf(result+offset, REG_STR(SP, 4));
    offset += sprintf(result+offset, REG_STR(PC, 4));
    offset += sprintf(result+offset, ASCII_GREEN "FLAGS" ASCII_STOP ": " ASCII_RED "%s%s%s%s" ASCII_STOP "\n", FLAG(C), FLAG(S), FLAG(P), FLAG(Z));
    offset += sprintf(result+offset, ASCII_GREEN "INTERRUPTS" ASCII_STOP ": " ASCII_RED "%s" ASCII_STOP "\n", arcade->interrupt_enabled ? "true" : "false"); 
    memset(result+offset, '-', 30);
    offset += 30;
    offset += sprintf(result+offset, " " ASCII_BOLD ASCII_MAGENTA "DISASSEMBLY" ASCII_STOP "\n");
    offset += sprintf(result+offset, "%s\n", disass);
    
    memset(result+offset, '-', 30);
    offset += 30;
    offset += sprintf(result+offset, " " ASCII_BOLD ASCII_MAGENTA "STACK" ASCII_STOP "\n");
    
    for(int i = 0; i < 4; ++i)
    {
        char values[32] = {};
        uint8_t *stack = &arcade->mem->mem[arcade->SP + i*8];
        sprintf(values, "%02x %02x %02x %02x %02x %02x %02x %02x", stack[0], stack[1], stack[2], stack[3], stack[4], stack[5], stack[6], stack[7]);
        offset += sprintf(result+offset, "%04x: %s\n", arcade->SP + i*8, values);
    }

    offset += sprintf(result+offset, "\n\n");
    free(disass);
    free_inss(inss);
}

int main(int argc, char **argv)
{
    
    if(argc < 2)
    {
        usage();
    }

    rom_t rom = {};
    rom_add_file("roms/invaders.h", &rom);
    rom_add_file("roms/invaders.g", &rom);
    rom_add_file("roms/invaders.f", &rom);
    rom_add_file("roms/invaders.e", &rom);
    

    uint8_t bytecode[] = 
    {
        0x01, 0x80, 0x80,  // LXI B, 8080
        0x31, 0x00, 0x24,  // LXI SP, dead
        0xc5,              // PUSH B
    };

    //rom_add_mem(bytecode, sizeof bytecode, &rom);

    if(!strcmp(argv[1], "--disassemble") )
    {
       ins_t **instructions = parse_n_bytecode(rom.contents, rom.size);
       char *disassembly = inss_to_str(instructions, 0x0);

       printf("%s\n", disassembly);
       free(disassembly);
       free_inss(instructions);
    }
    else if(!strcmp(argv[1], "--debug"))
    {
        
        arcade_t arcade = {};
        setup_arcade(&arcade);

        memcpy(arcade.mem->ROM, rom.contents, rom.size); 
    
        rom_free(&rom); 
        uint16_t *breakpoints = malloc(sizeof (uint16_t) * (argc - 1));
        
        for(int i = 2; i < argc; ++i)
        {
            size_t address = strtoll(argv[i], NULL, 16);
            if(address > UINT16_MAX || address == 0xffff)
            {
                printf("Cannot break at %p!\n", address);
                exit(1);
            }
            breakpoints[i-2] = (uint16_t)address;
        }
        breakpoints[argc - 2] = 0xffff;

        emulate(&arcade, breakpoints, true);
        free(breakpoints);
        free(arcade.mem);
        
        return 0;
    }
    usage();

}

void usage()
{
    printf("Usage: arcade [--disassemble]|[--debug (<breakpoint_1> <breakpoint_2>)]\n");
    exit(1);
}
