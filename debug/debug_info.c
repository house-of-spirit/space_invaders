#include <stdio.h>

#include <debug/debug_info.h>

const debug_label_t *debug_addr_get_label(uint16_t address)
{
    for(int i = 0; i < sizeof(space_invaders_labels)/sizeof(*space_invaders_labels); ++i)
    {
        if(address == space_invaders_labels[i].address)
         {
             return &space_invaders_labels[i];
         }
    }
    return NULL;
}


const debug_label_t *debug_determine_current_label(uint16_t address)
{
    return NULL;
}
