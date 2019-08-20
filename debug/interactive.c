#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <debug/interactive.h>
#include <debug/debug_info.h>
#include <debug/debug_state.h>
#include <debug/breakpoint.h>
#include <instruction/instruction.h>
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


void debug_context(arcade_t *arcade, char *buf)
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


void interactive_print_help()
{
    printf("h               - Show this menu\n"
           "b  [LABEL|ADDR] - Set breakpoint at argument\n"
           "d  [LABEL|ADDR] - Remove breakpoint at argument\n"
           "n               - Go to next instruction and display this prompt again\n"
           "c               - Continue execution\n"
           "e               - Exit emulator\n"
           "x  ADDR AMOUNT  - Display x bytes of memory at address\n"
           "is SYMBOL       - Show info about symbol\n"
           "il LABEL        - Show info about label\n"
           "ls              - List symbols\n"
           "ll              - List labels\n"
           "ti              - Display instruction trace\n"
           "tl              - Display label (function) trace (no ISR)\n"
           "dl LABEL        - Display label (function) disassembly\n\n");

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
        debug_context(a, context_buf);
        
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
        
        else if(!strncmp(query, "dl", 2) && isspace(query[2]))
        {
            function_interval_t interval = debug_get_function_interval(&query[3]);
            if(interval.end == 0x00)
            {
                printf("Invalid Label\n\n");
                goto breakpoint_trigger;
            }

            ins_t **instructions = parse_n_bytecode(&a->mem->mem[interval.begin], interval.end - interval.begin);

            char *disassembly = inss_to_str(instructions, interval.begin);

            printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
                   "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
                   "\n\n%s\n\n"
                   "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
                   "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n", disassembly);
            free(disassembly);
            free(instructions);
            
            goto breakpoint_trigger;
        }
        
        else if(!strncmp(query, "is", 2) && isspace(query[2]))
        {
            const debug_symbol_t *symbol = debug_string_get_symbol(&query[3]);
            
            if(!symbol)
            {
                printf("Invalid Symbol\n\n");
                goto breakpoint_trigger;
            }

            printf("symbol {\n"
                   "    name: %s\n"
                   "    desc: %s\n"
                   "    addr: %04x\n"
                   "    size: %lx\n"
                   "}\n\n", symbol->name, symbol->description, symbol->address, symbol->value_size);
            
            goto breakpoint_trigger;
        }

        else if(!strncmp(query, "il", 2) && isspace(query[2]))
        {
            const debug_label_t *label = debug_string_get_label(&query[3]);

            if(!label)
            {
                printf("Invalid Label\n\n");
                goto breakpoint_trigger;
            }

            function_interval_t interval = debug_get_function_interval(&query[3]);

            printf("label {\n"
                   "    name:      %s\n"
                   "    starts_at: %04x\n"
                   "    ends_at:   %04x\n"
                   "}\n\n", label->label, interval.begin, interval.end);
            goto breakpoint_trigger;
        }


        else if(!strncmp(query, "ls", 2))
        {
           debug_print_space_invaders_symbols();
           printf("\n\n");
           goto breakpoint_trigger; 
        }
        
        
        else if(!strncmp(query, "ll", 2))
        {
           debug_print_space_invaders_labels();
           printf("\n\n");
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
