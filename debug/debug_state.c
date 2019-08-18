#include <stdlib.h>
#include <stdio.h>

#include <debug/debug_state.h>
#include <arcade.h>

void debug_add_trace(arcade_t *a)
{
    if(!a->debug_state) 
        a->debug_state = calloc(1, sizeof *a->debug_state);
   
    if(a->debug_state->addr_count == 0)
    {
        a->debug_state->head = calloc(1, sizeof *a->debug_state->head);
        a->debug_state->head->addr = a->PC;
    }
    else
    {
        trace_addr_t *curr = a->debug_state->head;
        
        while(true)
        {
            if(!curr->next)
            {
                curr->next = calloc(1, sizeof *curr->next);
                curr->next->addr = a->PC;
                break;
            }
            else
                curr = curr->next;
        }
    }
    
    
    if(a->debug_state->addr_count >= 128)
    {
        trace_addr_t *temp = a->debug_state->head;
        a->debug_state->head = a->debug_state->head->next;
        free(temp);
    }
    else
        a->debug_state->addr_count++;
        
}

void debug_print_trace(debug_state_t *s)
{
    trace_addr_t *curr = s->head;
   
    while(curr)
    {
        printf("%04lx", (size_t)curr->addr);

        if(curr->next)
        {
            printf(" -> ");
            curr = curr->next;
        }
        else
            break;
    }
}
