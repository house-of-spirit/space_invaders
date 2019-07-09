#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <rom/rom.h>
#include <instruction/instruction.h>

void rom_add_file(char *filename, rom_t *rom)
{
    /* Append file contents to rom structure
     * will add a file mapping entry
     */
    
    if(strlen(filename) > 300)
    {
        fprintf(stderr, "Path %s too long: max 300 characters\n");
        return;
    }

    FILE* fp = fopen(filename, "r");
    size_t filesize;
     
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    
    rewind(fp);

    rom->contents = realloc(rom->contents, rom->size + filesize);

    fread(&rom->contents[rom->size], sizeof(uint8_t), filesize, fp);
    rom->mappings = realloc(rom->mappings, (rom->file_count + 1) * sizeof(file_map_t));
    
    rom->mappings[rom->file_count].offset = rom->size;
    rom->mappings[rom->file_count].file_path = strdup(filename);
    
    rom->size += filesize;
    rom->file_count++;
    
    fclose(fp);
}

void rom_add_mem(uint8_t *mem, size_t n, rom_t *rom)
{
    /*
     * Append raw block of memory to rom structure,
     * will add a file mapping entry with path set to NULL
     */

    rom->contents = realloc(rom->contents, rom->size + n);

    memcpy(&rom->contents[rom->size], mem, n);
    
    rom->mappings = realloc(rom->mappings, (rom->file_count + 1) * sizeof(file_map_t));

    rom->mappings[rom->file_count].offset = rom->size;
    rom->mappings[rom->file_count].file_path = NULL;

    rom->size += n;
    rom->file_count++;
}


void rom_free(rom_t *rom)
{
    /*
     * frees the contents of a rom_t if applicable
     * actual rom_t pointer should be freed by the caller if needed
     */

    free(rom->contents);

    for(int i = 0; i < rom->file_count; ++i)
    {
        free(rom->mappings[i].file_path);
    }
    
    free(rom->mappings);
}

char *rom_info(rom_t *rom)
{
    #define resize result = realloc(result, result_sz)
    
    #define append result_sz += strlen(accum); resize; strcat(result, accum);

    char accum[512] = {0};
    
    char *result = calloc(1, 1);
    size_t result_sz = 1;

    sprintf(accum, "Total rom size: 0x%04x\n\n", rom->size);
    
    append;

    sprintf(accum, "File count: %d\n\n\t\033[1mOffset    Path\033[0m\n", rom->file_count);

    append;

    for(int i = 0; i < rom->file_count; ++i)
    {
        sprintf(accum, "\t%04x:     %s\n", rom->mappings[i].offset, rom->mappings[i].file_path);

        append;
    }
    

    return result;

    #undef resize
    #undef append
}
