#pragma once

typedef struct debug_symbol {
    
    uint16_t address;
    char *name;
    char *description;     
    size_t value_size;

} debug_symbol_t;

typedef struct debug_comment {
    uint16_t address;
    char *comment;
    bool instruction_comment; /* signifies whether the comment should be placed on the line(s) before the instruction, or on the same line. */

} debug_comment_t;

typedef struct debug_label {

    uint16_t address;
    char *label;   /* Label string */

} debug_label_t;

typedef struct debug_info {
    size_t symbol_count;
    debug_symbol_t *symbols;
    size_t comment_count;
    debug_comment_t *comments;
    size_t label_count;
    debug_label_t *labels; 
} debug_info_t;

static const debug_symbol_t space_invaders_symbols[] = {
    #include <debug/debug_symbols.h>
};

static const debug_label_t space_invaders_labels[] = {
    #include <debug/debug_labels.h>
};

static const debug_comment_t space_invaders_comments[] = {
    #include <debug/debug_comments.h>
};
