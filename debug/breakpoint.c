#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <debug/breakpoint.h>


void debug_print_breakpoints(breakpoint_t *bp)
{
    breakpoint_t *curr = bp;

    while(curr)
    {
        debug_print_breakpoint(curr);
        printf(" | ");
        curr = curr->next;
    }
}

void debug_print_breakpoint(breakpoint_t *bp)
{
    switch(bp->type)
    {
        case BP_ADDRESS:
            printf("%04x", bp->value.address);
            return;
        case BP_LABEL:
            if(bp->value.label == NULL)
                printf("[NULL]");
            else
                printf("%s", bp->value.label->label);
            return;
    }
}


void debug_add_breakpoints(debug_state_t *state, int bp_count, char **bp_list)
{
    for(int i = 0; i < bp_count; ++i)
    {
        char *bp = bp_list[i];

        if(isdigit(bp[0]))
        {
            uint16_t address = (uint16_t)strtoll(bp, NULL, 16);
            debug_add_breakpoint_address(state, address);
        }
        else
            debug_add_breakpoint_label(state, bp);
    }
}

void debug_init_breakpoint_if_NULL(debug_state_t *state)
{
    if(state->breakpoints == NULL)
        state->breakpoints = calloc(1, sizeof *state->breakpoints);
}

void debug_add_breakpoint_address(debug_state_t *state, uint16_t address)
{
    bool is_null = state->breakpoints == NULL;
    debug_init_breakpoint_if_NULL(state);
    breakpoint_t *curr = state->breakpoints;

    while(curr->next)
        curr = curr->next;
    
    if(!is_null)
    {
        curr->next = calloc(1, sizeof *curr->next);
        curr = curr->next;
    }

    curr->type = BP_ADDRESS;
    curr->value.address = address;

}

bool debug_add_breakpoint_label(debug_state_t *state, char *label)
{
    const debug_label_t *fetched_label = debug_string_get_label(label);

    if(!fetched_label) return false;

    bool is_null = state->breakpoints == NULL;
    debug_init_breakpoint_if_NULL(state);

    breakpoint_t *curr = state->breakpoints;
    
    while(curr->next)
        curr = curr->next;
    
    if(!is_null)
    {
        curr->next = calloc(1, sizeof *curr->next);
        curr = curr->next;
    }

    curr->type = BP_LABEL;
    curr->value.label = fetched_label;

    return true;
}

void debug_delete_breakpoint_address(debug_state_t *state, uint16_t address)
{
    breakpoint_t **prev_pointer = &state->breakpoints;

    breakpoint_t *curr = *prev_pointer;

    while(curr)
    {
        if(curr->type == BP_ADDRESS && curr->value.address == address)
        {
            debug_delete_breakpoint(prev_pointer, curr);
            curr = *prev_pointer;
        }
        else
        {
            prev_pointer = &curr->next;
            curr = curr->next;
        }
    }
}

void debug_delete_breakpoint_label(debug_state_t *state, char *label)
{
    breakpoint_t **prev_pointer = &state->breakpoints;
    
    breakpoint_t *curr = *prev_pointer;

    while(curr)
    {
        if(curr->type == BP_LABEL && !strcmp(curr->value.label->label, label))
        {
            debug_delete_breakpoint(prev_pointer, curr);
            curr = *prev_pointer;
        }
        else
        {
            prev_pointer = &curr->next;
            curr = curr->next;
        }
    }
}

void debug_delete_breakpoint(breakpoint_t **prev_pointer, breakpoint_t *bp)
{
    *prev_pointer = bp->next;

    free(bp);
}

bool debug_addr_in_breakpoints(breakpoint_t *bp, uint16_t address)
{
    breakpoint_t *curr = bp;

    while(curr)
    {
        switch(curr->type)
        {
            case BP_ADDRESS:
                if(curr->value.address == address)
                    return true;
                break;
            case BP_LABEL:
                if(curr->value.label->address == address)
                    return true;
                break;
        }
        curr = curr->next;
    }
    return false;
}
