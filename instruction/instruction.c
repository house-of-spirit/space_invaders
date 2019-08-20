#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "instruction.h"
#include "instruction_list.h"

#include <debug/debug_info.h>


#define ASCII_GREEN "\033[0;32m"
#define ASCII_RED "\033[0;31m"
#define ASCII_BOLD "\033[1m"
#define ASCII_BLUE "\033[34;1m"
#define ASCII_MAGENTA "\033[35;1m"
#define ASCII_STOP "\033[0m"

const char whitespace[] = "                                                                                                    ";

int ins_to_str(char *result, ins_t *instruction, bool *has_color)
{
    /* Deserialize a `ins_t` to a string
     * `result` should have space for 18 bytes ("XTHL FLAGS, FLAGS\0")
     * Whilst this is a bogus instruction, it is possible to create a 
     * `ins_t` containing this instruction.
     * From caller context, size can be more accurately inferred preemtpively
     */

    const char *mnem = mnemonic_to_str(instruction->mnemonic);
    char args[512] = {0};
    
    if(instruction->args[0].type != ARG_NONE)
    {
        arg_to_str(args, &instruction->args[0], has_color);

        if(instruction->args[1].type != ARG_NONE)
        {
            strcat(args, ", ");
            arg_to_str(&args[strlen(args)], &instruction->args[1], has_color);
        }
    }
    
    strcpy(result, mnem);
    strcat(result, " ");
    strcat(result, args);
}

char *inss_to_str(ins_t **instructions, size_t mem_offset)
{
    char addr_accum[7] = {0};
    char ins_accum[512] = {0};
    char comment_buf[512] = {0};
    char label_buf[64] = {0};
    char *result = malloc(1);
    size_t size = 1;
    size_t current_addr = mem_offset;

    result[0] = 0;
    for(int i = 0; instructions[i] != NULL; ++i)
    {
        bool standalone_comment_state = false, ins_comment_state = false;
        const debug_comment_t *ins_comment = NULL;
        
        for(int j = 0; j < (sizeof space_invaders_labels)/(sizeof *space_invaders_labels); ++j)
        {
            const debug_label_t *current_label = &space_invaders_labels[j];

            if(current_label->address != current_addr) continue;

            sprintf(label_buf, "\n" ASCII_MAGENTA "%s" ASCII_STOP ":\n", current_label->label);
            size += strlen(label_buf);
            result = realloc(result, size);
            strcat(result, label_buf);
            break;

        }
        

        for(int j = 0; j < (sizeof space_invaders_comments)/(sizeof *space_invaders_comments); ++j)
        {
            const debug_comment_t *current_comment = &space_invaders_comments[j];

            if(current_comment->address != current_addr) continue;

            if(current_comment->instruction_comment && !ins_comment_state)
            {
                ins_comment_state = true;
                ins_comment = current_comment;
                continue;
            }

            if(!current_comment->instruction_comment && !standalone_comment_state)
            {
                strcat(comment_buf, ASCII_GREEN);

                size_t comment_index = strlen(comment_buf);
                for(int k = 0; current_comment->comment[k] != 0; ++k)
                {
                    if(k == 0 || k > 0 && current_comment->comment[k-1] == '\n')
                    {
                        strcat(comment_buf, "; ");
                        comment_index += 2;
                    }
                    comment_buf[comment_index++] = current_comment->comment[k];
                }
                comment_buf[comment_index++] = '\n';
                comment_buf[comment_index++] = 0;
                
                strcat(comment_buf, ASCII_STOP);

                size += strlen(comment_buf);
                result = realloc(result, size);
                strcat(result, comment_buf);
                memset(comment_buf, 0, sizeof comment_buf);
                continue;
            }
        }

        ins_t *instruction = instructions[i];

        bool ins_has_color = false;
        ins_to_str(ins_accum, instruction, &ins_has_color);
        
        if(ins_comment_state)
        {
            sprintf(comment_buf, ASCII_GREEN "; %s" ASCII_STOP, ins_comment->comment);
            size += strlen(comment_buf);
        }

        sprintf(addr_accum, "%04lx: ", current_addr);
        
        size += sizeof whitespace + 1;

        result = realloc(result, size);
        strcat(result, addr_accum);
        strcat(result, ins_accum);
        
        strncat(result, whitespace, (sizeof whitespace) - (strlen(addr_accum) + strlen(ins_accum)) - (ins_has_color ? 0 : 11) );
        
        if(ins_comment_state)
        {
            strcat(result, comment_buf);
            memset(comment_buf, 0, sizeof comment_buf);
        }

        strcat(result, "\n");

        current_addr += instruction->bytecode_size;
    }

    return result;
}

int arg_to_str(char *result, ins_arg_t *arg, bool *has_color)
{
    /* Deserialize a `ins_arg_t` to a string. 
     * size needs to be accurately inferred preemptively
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
            
            for(int i = 0; i < (sizeof space_invaders_labels)/(sizeof *space_invaders_labels); ++i)
            {
                const debug_label_t *label = &space_invaders_labels[i];
                if(arg->value.imm16 == label->address)
                {
                    sprintf(result, ASCII_RED "%s" ASCII_STOP, label->label);
                    if(has_color != NULL) *has_color = true;
                    return 0;
                }
            }
            for(int i = 0; i < (sizeof space_invaders_symbols)/(sizeof *space_invaders_symbols); ++i)
            {
                const debug_symbol_t *symbol = &space_invaders_symbols[i];

                if(arg->value.imm16 == symbol->address)
                {
                    sprintf(result, ASCII_BLUE "%s" ASCII_STOP, symbol->name);
                    if(has_color != NULL) *has_color = true;
                    return 0;
                }

                if(arg->value.imm16 > symbol->address 
                        && arg->value.imm16 < symbol->address + symbol->value_size)
                {
                    sprintf(result, ASCII_BLUE "%s+%d" ASCII_STOP, symbol->name, arg->value.imm16 - symbol->address);
                    if(has_color != NULL) *has_color = true;
                    return 0;
                }

            }
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
       fprintf(stderr, "Bytecode not aligned: expected to parse 0x%lx bytes, but parsed 0x%x !!!\n", n_bytecode, i);
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

const debug_label_t *ins_alters_pc_to_label(arcade_t *a, ins_t *instruction)
{
    #define RET_ARG return debug_addr_get_label(instruction->args[0].value.imm16)
    #define RET_STACK return debug_addr_get_label(*((uint16_t*)&a->mem->mem[a->SP]))
    
    switch(instruction->mnemonic)
    {
        case MNEM_JMP:
        case MNEM_CALL:
            RET_ARG;
        
        case MNEM_PCHL:
            return debug_addr_get_label(a->HL);    
        
        case MNEM_JZ:
        case MNEM_CZ:
            if(a->FLAGS.Z)
                RET_ARG;
            break;
        
        case MNEM_JNZ:
        case MNEM_CNZ:
            if(!a->FLAGS.Z)
                RET_ARG;
            break;

        case MNEM_JC:
        case MNEM_CC:
            if(a->FLAGS.C)
                RET_ARG;
            break;
        
        case MNEM_JNC:
        case MNEM_CNC:
            if(!a->FLAGS.C)
                RET_ARG;
            break;
         
        case MNEM_JP:
        case MNEM_CP:
            if(!a->FLAGS.S)
                RET_ARG;
            break;
        
        case MNEM_JM:
        case MNEM_CM:
            if(a->FLAGS.S)
                RET_ARG;
            break;
        
        case MNEM_JPE:
        case MNEM_CPE:
            if(a->FLAGS.P)
                RET_ARG;
            break;
       
        case MNEM_JPO:
        case MNEM_CPO:
            if(!a->FLAGS.P)
                RET_ARG;
            break;
        
        case MNEM_RZ:
            if(a->FLAGS.Z)
                RET_STACK;
            break;

        case MNEM_RNZ:
            if(!a->FLAGS.Z)
                RET_STACK;
            break;
        
        case MNEM_RC:
            if(a->FLAGS.C)
                RET_STACK;
            break;
        
        case MNEM_RNC:
            if(!a->FLAGS.C)
                RET_STACK;
            break;

        case MNEM_RP:
            if(!a->FLAGS.S)
                RET_STACK;
            break;

        case MNEM_RM:
            if(a->FLAGS.S)
                RET_STACK;
            break;

        case MNEM_RPE:
            if(a->FLAGS.P)
                RET_STACK;
            break;

        case MNEM_RPO:
            if(!a->FLAGS.P)
                RET_STACK;
            break;
    }

    return NULL;
}
