#pragma once

typedef struct arcade arcade_t;

typedef struct trace_addr {
   void *addr;
   struct trace_addr *next;
}   trace_addr_t;

typedef struct debug_state {
    trace_addr_t *head;
    size_t addr_count;
} debug_state_t;

void debug_add_trace(arcade_t *a);
void debug_print_trace(debug_state_t *s);
