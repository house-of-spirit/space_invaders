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

int main(int argc, char **argv)
{
    rom_t rom = {};
    arcade_t arcade = {};
    setup_arcade(&arcade);
    rom_add_file("roms/invaders.h", &rom);
    rom_add_file("roms/invaders.g", &rom);
    rom_add_file("roms/invaders.f", &rom);
    rom_add_file("roms/invaders.e", &rom);
    
    memcpy(arcade.mem->ROM, rom.contents, rom.size); 
    
    emulate(&arcade);

    return 0;
}
