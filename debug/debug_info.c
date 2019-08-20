#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <debug/debug_info.h>

void debug_print_space_invaders_labels()
{
    for(int i = 0; i < sizeof(space_invaders_labels)/sizeof(*space_invaders_labels); ++i)
    {
        const debug_label_t *label = &space_invaders_labels[i];
        printf("<%s> @ %04x\n", label->label, label->address);
    }
}

void debug_print_space_invaders_symbols()
{
    for(int i = 0; i < sizeof(space_invaders_symbols)/sizeof(*space_invaders_symbols); ++i)
    {
        const debug_symbol_t *symbol = &space_invaders_symbols[i];

        printf("[%ld] <%s> @ %04x // %s\n", symbol->value_size, symbol->name, symbol->address, symbol->description); 
    }

}

const debug_label_t *debug_addr_get_label(uint16_t address)
{
    for(int i = 0; i < sizeof(space_invaders_labels)/sizeof(*space_invaders_labels); ++i)
    {
        if(address == space_invaders_labels[i].address)
             return &space_invaders_labels[i];
    }
    return NULL;
}


const debug_label_t *debug_string_get_label(char *label_string)
{
    for(int i = 0; i < sizeof(space_invaders_labels)/sizeof(*space_invaders_labels); ++i)
    {
        if(!strcmp(label_string, space_invaders_labels[i].label))
            return &space_invaders_labels[i];
    }
    return NULL;
}

const debug_symbol_t *debug_addr_get_symbol(uint16_t address)
{
    for(int i = 0; i < sizeof(space_invaders_symbols)/sizeof(*space_invaders_symbols); ++i)
    {
        if(space_invaders_symbols[i].address == address)
            return &space_invaders_symbols[i];
    }
    return NULL;
}


const debug_symbol_t *debug_string_get_symbol(char *symbol_string)
{
    for(int i = 0; i < sizeof(space_invaders_symbols)/sizeof(*space_invaders_symbols); ++i)
    {
        if(!strcmp(space_invaders_symbols[i].name, symbol_string))
            return &space_invaders_symbols[i];
    }
    return NULL;
}

const debug_label_t *debug_determine_current_label(uint16_t address)
{
    return NULL;
}

function_interval_t debug_get_function_interval(char *label_string)
{

    function_interval_t interval = {};

    const debug_label_t *function = debug_string_get_label(label_string);
    if(!function) 
        return interval;
    
    interval.begin = function->address;
    interval.end = function[1].address ? function[1].address : CODE_SECTION_END ; /* labels are ordered */

    return interval;
}
