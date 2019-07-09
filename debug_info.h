#pragma once

typedef struct debug_symbol {
    
    uint16_t address;
    char *name;
    char *description;
     
    size_t value_size;

} debug_symbol_t;

/* TODO: maybe join debug_comment and debug_label */
typedef struct debug_comment {
    uint16_t address;
    char *comment;

} debug_comment_t;

typedef struct debug_label {

    uint16_t address;
    char *label;

} debug_label_t;

typedef struct debug_info {
    size_t symbol_count;
    debug_symbol_t *symbols;
    size_t comment_count;
    debug_comment_t *comments;
    size_t label_count;
    debug_label_t *labels; 


} debug_info_t;

const debug_symbol_t space_invaders_symbols[] = {
    #include "debug_symbols.h"
};
