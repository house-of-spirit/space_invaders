#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <arcade.h>
#include <instruction/instruction.h>
#include <rom/rom.h>
#include <emulator/emulator.h>

#include <debug/debug_info.h>
#include <debug/breakpoint.h>


/* 
 * cc `find ./ -name "*.c"` -o arcade -I./ 
 */

void setup_arcade(arcade_t *arcade)
{
    arcade->mem = malloc(sizeof *arcade->mem);
}



int main(int argc, char **argv)
{
    enum PROGRAM_MODE {

        MODE_NONE = 0,
        MODE_DEBUG,
        MODE_RUN,
        MODE_DISASS,
        MODE_LABELS,
        MODE_SYMBOLS
    } mode = MODE_NONE;

    if(argc < 2)
    {
        usage();
    }

    rom_t rom = {};
    rom_add_file("roms/invaders.h", &rom);
    rom_add_file("roms/invaders.g", &rom);
    rom_add_file("roms/invaders.f", &rom);
    rom_add_file("roms/invaders.e", &rom);
    

    if(!strcmp(argv[1], "--disassemble")) mode = MODE_DISASS;
    else if(!strcmp(argv[1], "--debug")) mode = MODE_DEBUG;
    else if(!strcmp(argv[1], "--run")) mode = MODE_RUN;
    else if(!strcmp(argv[1], "--labels")) mode = MODE_LABELS;
    else if(!strcmp(argv[1], "--symbols")) mode = MODE_SYMBOLS;

    if(mode == MODE_DISASS)
    {
        ins_t **instructions;

        char *disassembly;
       
        if(argc < 3)
        {
            instructions = parse_n_bytecode(rom.contents, CODE_SECTION_END);
            disassembly = inss_to_str(instructions, 0x0);
        }
        else
        {
            function_interval_t interval = debug_get_function_interval(argv[2]);
            
            if(interval.end == 0x00)
            {
                fprintf(stderr, "Could not find function \"%s\"\n", argv[2]);
                return 1;
            }

            instructions = parse_n_bytecode(&rom.contents[interval.begin], interval.end - interval.begin);

            disassembly = inss_to_str(instructions, interval.begin);
        }
        printf("%s\n", disassembly);
        free(disassembly);
        free_inss(instructions);

        return 0;
    }
    else if(mode == MODE_LABELS)
    {
        debug_print_space_invaders_labels();
        return 0;
    }
    else if(mode == MODE_SYMBOLS)
    {
        debug_print_space_invaders_symbols();
        return 0;
    }
    else if(mode == MODE_DEBUG || mode == MODE_RUN)
    {
        
        arcade_t arcade = {};
        setup_arcade(&arcade);

        memcpy(arcade.mem->ROM, rom.contents, rom.size); 
    
        rom_free(&rom);
        
        if(mode == MODE_DEBUG)
        {
            arcade.debug_enabled = true;
            debug_state_init(&arcade);
            
            debug_add_breakpoints(arcade.debug_state, argc-2, &argv[2]);
        }

        emulate(&arcade);
        free(arcade.mem);
        
        if(arcade.debug_enabled)
        {
            debug_free_state(arcade.debug_state);
        }

        return 0;
    }
    usage();
}

void usage()
{
    printf("Usage: arcade [--run]|[--debug (<breakpoint_1> <breakpoint_2> ...)]|[--disassemble]\n");
    exit(1);
}
