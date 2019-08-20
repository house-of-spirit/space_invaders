#include <stdlib.h>
#include <stdio.h>

#include <debug/debug_state.h>
#include <debug/debug_info.h>
#include <arcade.h>

void debug_state_init(arcade_t *a)
{
    a->debug_state = calloc(1, sizeof *a->debug_state );
    a->debug_state->trace_ins.max_count = DEFAULT_MAX_INS_ADDRS;
    a->debug_state->trace_label.max_count = DEFAULT_MAX_LABEL_ADDRS;
}

void debug_add_trace(trace_t *trace, uint16_t trace_value)
{
    if(trace->count == 0)
    {
        trace->head = calloc(1, sizeof *trace->head);
        trace->head->addr = trace_value;
    }
    else
    {
        trace_addr_t *curr = trace->head;
        
        while(true)
        {
            if(!curr->next)
            {
                curr->next = calloc(1, sizeof *curr->next);
                curr->next->addr = trace_value;
                break;
            }
            else
                curr = curr->next;
        }
    }
    
    if(trace->count >= trace->max_count)
    {
        trace_addr_t *temp = trace->head;
        trace->head = trace->head->next;
        free(temp);
    }
    else
        trace->count++;
        
}

void debug_print_ins_trace(trace_t *trace)
{
    trace_addr_t *curr = trace->head;
   
    while(curr)
    {
        printf("%04x", curr->addr);

        if(curr->next)
        {
            printf(" -> ");
            curr = curr->next;
        }
        else
            break;
    }
}

void debug_print_label_trace(trace_t *trace)
{
    trace_addr_t *curr = trace->head;

    while(curr)
    {
        const debug_label_t *label = debug_addr_get_label((uint16_t)curr->addr);
        
        if(label == NULL)
            printf("[%04x]", curr->addr);
        else
            printf("%s", label->label);


        if(curr->next)
        {
            printf(" -> ");
            curr = curr->next;
        }
        else
            break;

    }
}



void debug_free_trace(trace_t *trace)
{
    trace_addr_t *curr = trace->head;

    while(curr)
    {
        trace_addr_t *next = curr->next;
        free(curr);
        curr = next;
    }
}

void debug_free_state(debug_state_t *s)
{
    debug_free_trace(&s->trace_ins);
    debug_free_trace(&s->trace_label);
    free(s);
}

