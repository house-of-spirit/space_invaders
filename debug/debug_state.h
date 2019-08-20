#pragma once

#include <stdint.h>

typedef struct arcade arcade_t;

typedef struct trace_addr {
   uint16_t addr;
   struct trace_addr *next;
}   trace_addr_t;


#define DEFAULT_MAX_INS_ADDRS 128
#define DEFAULT_MAX_LABEL_ADDRS 32



typedef struct trace {
    trace_addr_t *head;
    size_t count;
    size_t max_count;
} trace_t;

typedef struct debug_state {
    trace_t trace_ins;
    trace_t trace_label;
} debug_state_t;


void debug_state_init(arcade_t *a);

void debug_add_trace(trace_t *trace, uint16_t trace_value);
void debug_print_ins_trace(trace_t *trace);
void debug_print_label_trace(trace_t *trace);
void debug_free_state(debug_state_t *s);
