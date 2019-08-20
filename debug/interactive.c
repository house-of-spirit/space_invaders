#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <debug/interactive.h>
#include <debug/debug_info.h>
#include <debug/debug_state.h>
#include <debug/breakpoint.h>
#include <arcade.h>

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


void interactive_print_help()
{
    printf("\th              - show this menu\n"
           "\tb [LABEL|ADDR] - Set breakpoint at argument\n"
           "\td [LABEL|ADDR] - Remove breakpoint at argument\n"
           "\tn              - Go to next instruction and display this prompt again\n"
           "\tc              - Continue execution\n"
           "\te              - Exit emulator\n"
           "\tx ADDR AMOUNT  - Display x bytes of memory at address\n"
           "\tti             - Display instruction trace\n"
           "\ttl             - Display label (function) trace (no ISR)\n\n");

}

void interactive_context(arcade_t *a)
{
    static bool should_break = false;
    char context_buf[2048] = {0};
    char query[256];
    bool context_set = false;

    if(should_break)
    {
        should_break = false;
        goto breakpoint_trigger;
    }

    if(debug_addr_in_breakpoints(a->debug_state->breakpoints, a->PC))
    {
        interactive_print_help();

        breakpoint_trigger:
        
        if(!context_set) 
        arcade_context(a, context_buf);
        
        puts(context_buf);
        
        printf("Breakpoints: ");
        debug_print_breakpoints(a->debug_state->breakpoints);

        printf("\n> ");
        fgets(query, 256, stdin);

        // remove newline from query
        query[strlen(query) - 1] = 0;
        if(query[0] == 'n')
        {
            should_break = true;
        }

        else if(query[0] == 'c')
        {
            return;
        }

        else if(query[0] == 'h')
        {
            interactive_print_help();
            goto breakpoint_trigger;
        }
        else if(query[0] == 'e')
        {
            exit(0);
        }

        else if(query[0] == 'b' && isspace(query[1]))
        {
            if(isdigit(query[2]))
                debug_add_breakpoint_address(a->debug_state, strtoll(&query[2], NULL, 16));
            else
                debug_add_breakpoint_label(a->debug_state, &query[2]); 

            goto breakpoint_trigger;
        }

        else if(query[0] == 'd' && isspace(query[1]))
        {
            if(isdigit(query[2]))
                debug_delete_breakpoint_address(a->debug_state, strtoll(&query[2], NULL, 16));
            else
                debug_delete_breakpoint_label(a->debug_state, &query[2]);

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
        else
        {
            puts("Command not recognized");
            puts("Type \"h(elp)\" for all commands");

            goto breakpoint_trigger;
        }
    }
    return;
           
}
