#include "debug.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <debug/debug_info.h>

#include <debug/debug_state.h>

void breakpoint_delete(uint16_t *breakpoints, size_t *bp_length, char *query)
{
    uint16_t address = strtoll(&query[2], NULL, 16);
    for(int j = 0; j < *bp_length; ++j)
    {
        if(breakpoints[j] == address)
        {
            for(int k = j; k < *bp_length; ++k)
            {
                breakpoints[k] = breakpoints[k + 1];
            }
            breakpoints = realloc(breakpoints, (*bp_length)-- * sizeof(uint16_t));
        }
    }
}

void breakpoint_add(uint16_t *breakpoints, size_t *bp_length, char *query)
{
    uint16_t address = strtoll(&query[2], NULL, 16);
    breakpoints = realloc(breakpoints, (*bp_length + 2) * sizeof(uint16_t));
    breakpoints[*bp_length] = address;
    breakpoints[++*bp_length] = 0xffff;
}

void memory_examine(arcade_t *a, char *query)
{
    char *next;
    uint16_t address = strtoll(&query[2], &next, 16);
    size_t amount = strtoll(next, NULL, 10);
    amount = amount == 0 ? 32 : amount;
    
    if(amount > 0x1000) return;
    for(int i = 0; i <= amount / 8; ++i)
    {
        if(amount == i*8) break;
        printf("%04x: ", address+i*8);

        for(int j = 0; j < 8 && j+i*8 < amount; ++j)
        {
            printf("%02x ", a->mem->mem[j+address+i*8]);

        }
        printf("\n");
    }
}

void interactive_context(arcade_t *a, uint16_t *breakpoints, size_t *bp_length)
{
    static bool should_break = false;
    char context_buf[2048] = {0};
    char query[256];
    bool context_set = 0;

    if(should_break)
    {
        should_break = false;
        goto breakpoint_trigger;
    }

    for(int i = 0; i < *bp_length; ++i)
    {
        if(a->PC == breakpoints[i])
        {
            
            breakpoint_trigger:
            
            if(!context_set) arcade_context(a, context_buf);
            puts(context_buf);
            
            printf("Breakpoints: ");
            for(int j = 0; j < *bp_length; ++j)
            {
                printf("%04x ", breakpoints[j]);
            }
            printf("\n");

            printf("> ");
            fgets(query, 256, stdin);
            if(query[0] == 'n')
            {
                should_break = true;
            }

            else if(query[0] == 'e')
            {
                exit(0);
            }

            else if(query[0] == 'b' && isspace(query[1]))
            {
                breakpoint_add(breakpoints, bp_length, query);

                goto breakpoint_trigger;
            }

            else if(query[0] == 'r' && isspace(query[1]))
            {
                breakpoint_delete(breakpoints, bp_length, query);

                goto breakpoint_trigger;
            }

            else if(query[0] == 'x' && isspace(query[1]))
            {
                memory_examine(a, query);
                goto breakpoint_trigger;
            }
            else if(!strncmp("ti", query, 2))
            {
                debug_print_ins_trace(&a->debug_state->trace_ins);
                printf("\n\n");
                goto breakpoint_trigger;
            }
            else if(!strncmp("tl", query, 2))
            {
                debug_print_label_trace(&a->debug_state->trace_label);
                printf("\n\n");
                goto breakpoint_trigger;
            }
            break;
        }
    }
    return;
               
}
