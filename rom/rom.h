#pragma once
#include <stddef.h>
#include <stdint.h>


typedef struct file_map {
    
    size_t offset;
    char *file_path;

} file_map_t;

typedef struct rom {

    size_t file_count;
    file_map_t *mappings;
    size_t size;
    uint8_t *contents;

} rom_t;


void rom_add_file(char *filename, rom_t *rom);
void rom_add_mem(uint8_t *mem, size_t n, rom_t *rom);
char *rom_info(rom_t *rom);
void rom_free(rom_t *rom);
