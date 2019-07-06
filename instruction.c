#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "instruction.h"
#include "instruction_list.h"

int ins_to_str(char *result, ins_t *instruction)
{
    /* Deserialize a `ins_t` to a string
     * `result` should have space for 18 bytes ("XTHL FLAGS, FLAGS\0")
     * Whilst this is a bogus instruction, it is possible to create a 
     * `ins_t` containing this instruction.
     * From caller context, size can be more accurately inferred preemtpively
     */

    const char *mnem = mnemonic_to_str(instruction->mnemonic);
    char args[16] = {0};

    if(instruction->args[0].type != ARG_NONE)
    {
        arg_to_str(args, &instruction->args[0]);

        if(instruction->args[1].type != ARG_NONE)
        {
            strcat(args, ", ");
            arg_to_str(&args[strlen(args)], &instruction->args[1]);
        }
    }
    
    strcpy(result, mnem);
    strcat(result, " ");
    strcat(result, args);

    return 0; 

}

char *inss_to_str(ins_t **instructions, size_t mem_offset)
{
    char ins_accum[18] = {0};
    char addr_accum[7] = {0};
    char whitespace_buf[50] = {0};
    char *result = malloc(1);
    char bytecode_str[9]; // "00 00 00\0"
    size_t size = 1;
    size_t current_addr = mem_offset;

    result[0] = 0;
    for(int i = 0; instructions[i] != NULL; ++i)
    {
        ins_t *instruction = instructions[i];
        
        int j;
        for(j = 0; j < instruction->bytecode_size; ++j)
        {
            sprintf(&bytecode_str[j*3], "%02x ", instruction->bytecode[j]);
        }
        bytecode_str[j*3 - 1] = 0;
        
        ins_to_str(ins_accum, instruction);
        
        sprintf(addr_accum, "%04x: ", current_addr);
        current_addr += instruction->bytecode_size;

        size += 60;
        result = realloc(result, size);
        strcat(result, addr_accum);
        strcat(result, ins_accum);
        
        memset(whitespace_buf, ' ', (50 - strlen(addr_accum) - strlen(ins_accum)) );
        whitespace_buf[50 - strlen(addr_accum) - strlen(ins_accum)] = 0;

        strcat(result, whitespace_buf);
        strcat(result, bytecode_str);
        strcat(result, "\n");
    }

    return result;
}

int arg_to_str(char *result, ins_arg_t *arg)
{
    /* Deserialize a `ins_arg_t` to a string. 
     * to be safe, `result` should have space for  6 bytes ("FLAGS\0")
     * although from caller context, size can be more accurately inferred preemptively
     */
     
    switch(arg->type)
    {
        case ARG_NONE:
           *result = 0;
           break;
        case ARG_REG:
          strcpy(result, reg_to_str(arg->value.reg));
          break;
        case ARG_IMM8:
          sprintf(result, "%02x", arg->value.imm8);
          break;
        case ARG_IMM16:
          sprintf(result, "%04x", arg->value.imm16);
          break;
        case ARG_CONST:
          sprintf(result, "%d", arg->value.constant);
          break;
        default:
          return 1;
    }
    return 0;


}

const char *mnemonic_to_str(mnem_t mnemonic)
{
    /* Look up string representation of `mnem_t` enum value */
    return mnemonic_strings[(size_t)mnemonic];

}


size_t parse_ins(ins_t *result, uint8_t *bytecode)
{
    /* Parse instruction from bytecode, 
     * and copy relevant instruction from global array to local instruction
     */
    size_t bytecode_size = 1;
    
    memcpy(result, &ins_formats[bytecode[0]], sizeof(ins_t));
    
    for(int i = 0; i < 2; ++i)
    {
        if(result->args[i].type == ARG_IMM16)
        {
            result->args[i].value.imm16 = *(uint16_t*)&bytecode[bytecode_size];
            bytecode_size += 2;
        
        }
        else if(result->args[i].type == ARG_IMM8)
        {
            result->args[i].value.imm8 = bytecode[bytecode_size];
            bytecode_size += 1;
        }
    }

    memcpy(result->bytecode, bytecode, bytecode_size);
    return bytecode_size;
}


ins_t **parse_n_bytecode(uint8_t *bytecode, size_t n_bytecode)
{
    
    ins_t **instructions = malloc(11 * sizeof(ins_t*));
    int i = 0, ins_count = 0; 

    for(; i < n_bytecode;)
    {
        if(ins_count && ins_count % 10 == 0)
            instructions = realloc(instructions, (11+ins_count) * sizeof(ins_t*));
        
        instructions[ins_count] = malloc(sizeof(ins_t));

        i += parse_ins(instructions[ins_count], &bytecode[i]);
        ins_count++;
   }
   instructions[ins_count] = NULL;

   if(i != n_bytecode)
   {
       fprintf(stderr, "Bytecode not aligned: expected to parse 0x%x bytes, but parsed 0x%x !!!\n", n_bytecode, i);
       free_inss(instructions);
       return NULL;
   }
   return instructions;
      
}

ins_t **parse_n_ins(uint8_t *bytecode, size_t n_ins)
{
    ins_t **instructions = malloc((n_ins + 1) * sizeof (ins_t*));
    
    size_t offset = 0;
    for(int i = 0; i < n_ins; ++i)
    {
        instructions[i] = malloc(sizeof (ins_t));

        offset += parse_ins(instructions[i], &bytecode[offset]);
    }
    instructions[n_ins] = NULL;
    return instructions;
}


size_t len_bytecode_inss(ins_t **instructions)
{
    size_t bytecode_len = 0;
    for(size_t i = 0; instructions[i] != 0; ++i) bytecode_len += instructions[i]->bytecode_size;
    return bytecode_len;
}

size_t len_inss(ins_t **instructions)
{
    size_t i;
    for(i = 0; instructions[i] != NULL; ++i);
    return i;
}

void free_inss(ins_t **instructions)
{
    for(int i = 0; instructions[i] != NULL; ++i)
    {
        free(instructions[i]);
    }
    free(instructions);
}
