#pragma once

#include <debug/debug_info.h>
#include <debug/debug_state.h>

typedef enum breakpoint_type {
    BP_ADDRESS = 0x00,
    BP_LABEL   = 0x01
} breakpoint_type_t;

typedef struct breakpoint {
    
    struct breakpoint *next;

    breakpoint_type_t type;
    union {
       const debug_label_t *label;
       uint16_t address;
    } value;
    
} breakpoint_t;

void debug_print_breakpoints(breakpoint_t *bp);
void debug_print_breakpoint(breakpoint_t *bp);

void debug_add_breakpoints(debug_state_t *state, int bp_count, char **bp_list);
void debug_add_breakpoint_address(debug_state_t *state, uint16_t address);
void debug_add_breakpoint_label(debug_state_t *state, char *label);
void debug_delete_breakpoint_address(debug_state_t *state, uint16_t address);
void debug_delete_breakpoint_label(debug_state_t *state, char *label);

void debug_delete_breakpoint(breakpoint_t **prev_pointer, breakpoint_t *bp);

bool debug_addr_in_breakpoints(breakpoint_t *bp, uint16_t address);
