#pragma once
#include <stddef.h>

#include <arcade.h>
#include <register/register.h>

typedef enum ins_arg_type { ARG_NONE, ARG_REG, ARG_IMM8, ARG_IMM16, ARG_CONST } ins_arg_type_t;

typedef enum mnem { 
    MNEM_NOP  = 0x00,    MNEM_LXI  = 0x01,    MNEM_STAX = 0x02,   MNEM_INX = 0x03,  
    MNEM_INR  = 0x04,    MNEM_DCR  = 0x05,    MNEM_MVI  = 0x06,   MNEM_RLC = 0x07,  
    MNEM_DAD  = 0x08,    MNEM_LDAX = 0x09,    MNEM_DCX  = 0x0a,   MNEM_RRC = 0x0b,  
    MNEM_RAL  = 0x0c,    MNEM_RAR  = 0x0d,    MNEM_SHLD = 0x0e,   MNEM_DAA = 0x0f,  
    MNEM_LHLD = 0x10,    MNEM_CMA  = 0x11,    MNEM_STA  = 0x12,   MNEM_STC = 0x13,  
    MNEM_LDA  = 0x14,    MNEM_CMC  = 0x15,    MNEM_MOV  = 0x16,   MNEM_HLT = 0x17,  
    MNEM_ADD  = 0x18,    MNEM_ADC  = 0x19,    MNEM_SUB  = 0x1a,   MNEM_SBB = 0x1b,
    MNEM_ANA  = 0x1c,    MNEM_XRA  = 0x1d,    MNEM_ORA  = 0x1e,   MNEM_CMP = 0x1f,  
    MNEM_RNZ  = 0x20,    MNEM_POP  = 0x21,    MNEM_JNZ  = 0x22,   MNEM_JMP = 0x23,  
    MNEM_CNZ  = 0x24,    MNEM_PUSH = 0x25,    MNEM_ADI  = 0x26,   MNEM_RST = 0x27,  
    MNEM_RZ   = 0x28,    MNEM_RET  = 0x29,    MNEM_JZ   = 0x2a,   MNEM_CZ  = 0x2b,   
    MNEM_CALL = 0x2c,    MNEM_ACI  = 0x2d,    MNEM_RNC  = 0x2e,   MNEM_JNC = 0x2f,  
    MNEM_OUT  = 0x30,    MNEM_CNC  = 0x31,    MNEM_SUI  = 0x32,   MNEM_RC  = 0x33,
    MNEM_JC   = 0x34,    MNEM_IN   = 0x35,    MNEM_CC   = 0x36,   MNEM_SBI = 0x37,
    MNEM_RPO  = 0x38,    MNEM_JPO  = 0x39,    MNEM_XTHL = 0x3a,   MNEM_CPO = 0x3b,  
    MNEM_ANI  = 0x3c,    MNEM_RPE  = 0x3d,    MNEM_PCHL = 0x3e,   MNEM_JPE = 0x3f,  
    MNEM_XCHG = 0x40,    MNEM_CPE  = 0x41,    MNEM_XRI  = 0x42,   MNEM_RP  = 0x43,
    MNEM_JP   = 0x44,    MNEM_DI   = 0x45,    MNEM_CP   = 0x46,   MNEM_ORI = 0x47,  
    MNEM_RM   = 0x48,    MNEM_SPHL = 0x49,    MNEM_JM   = 0x4a,   MNEM_EI  = 0x4b,   
    MNEM_CM   = 0x4c,    MNEM_CPI  = 0x4d

} mnem_t; 


typedef struct ins_arg {
    ins_arg_type_t type;
    union
    {
        uint8_t imm8;
        uint16_t imm16;
        reg_type_t reg;
        uint8_t constant;
    } value;

} ins_arg_t;

typedef struct ins {
    uint8_t opcode;
    mnem_t mnemonic;
    ins_arg_t args[2];
    size_t bytecode_size;
    uint8_t bytecode[3];
    void (*ins_func)(arcade_t *a, struct ins *t);
    size_t cycle_count;
} ins_t;


int ins_to_str(char *result, ins_t *instruction);
int arg_to_str(char *result, ins_arg_t *arg);
char *inss_to_str(ins_t **instructions, size_t mem_offset);

const char *mnemonic_to_str(mnem_t mnemonic);

ins_t **parse_n_bytecode(uint8_t *bytecode, size_t n_bytecode);
ins_t **parse_n_ins(uint8_t *bytecode, size_t n_ins);

size_t len_bytecode_inss(ins_t **instructions);
size_t len_inss(ins_t **instructions);

void free_inss(ins_t **instructions);
size_t parse_ins(ins_t *result, uint8_t *bytecode);

const static char *mnemonic_strings[] = {
    "NOP", "LXI", "STAX", "INX", "INR", "DCR", "MVI",  "RLC",
    "DAD", "LDAX", "DCX", "RRC", "RAL", "RAR", "SHLD", "DAA",
    "LHLD", "CMA", "STA", "STC", "LDA", "CMC", "MOV", "HLT",
    "ADD", "ADC", "SUB", "SBB", "ANA", "XRA", "ORA", "CMP",
    "RNZ", "POP", "JNZ", "JMP", "CNZ", "PUSH", "ADI", "RST",
    "RZ", "RET", "JZ", "CZ", "CALL", "ACI", "RNC", "JNC",
    "OUT", "CNC", "SUI", "RC", "JC", "IN", "CC", "SBI",
    "RPO", "JPO", "XTHL", "CPO", "ANI", "RPE", "PCHL", "JPE",
    "XCHG", "CPE", "XRI", "RP", "JP", "DI", "CP", "ORI", 
    "RM", "SPHL", "JM", "EI", "CM", "CPI" 

};
