#include <stdbool.h>
#include <stdio.h>

#include "emulator.h"
#include "instruction.h"

int emulate(arcade_t *a)
{
    char instruction_str[20];
    while(true)
    {
        ins_t instruction = {};
        uint8_t *ins_bytecode = &a->mem->mem[a->PC];
    
        parse_ins(&instruction, ins_bytecode);
        
        ins_to_str(instruction_str, &instruction);
        printf("%s\n", instruction_str);    
        instruction.ins_func(a, &instruction);
        
        a->PC += instruction.bytecode_size; 
    }
    return 0;
}


uint8_t *fetch_reg_8(arcade_t *a, reg_type_t type)
{
    switch(type)
    {
        case REG_A:
            return &a->A;
        case REG_B:
            return &a->B;
        case REG_C:
            return &a->C;
        case REG_D:
            return &a->D;
        case REG_E:
            return &a->E;
        case REG_H:
            return &a->H;
        case REG_L:
            return &a->L;
        case REG_M:
            return &a->mem->mem[a->HL];
    }
    return NULL;
}

uint16_t *fetch_reg_16(arcade_t *a, reg_type_t type)
{
    switch(type)
    {
        case REG_B:
            return &a->BC;
        case REG_D:
            return &a->DE;
        case REG_H:
            return &a->HL;
        case REG_SP:
            return &a->SP;
        case REG_PC:
            return &a->PC;
        case REG_PSW:
            return &a->PSW;
    }
    return NULL;
}

void set_flags(arcade_t *a, uint8_t value)
{
    uint8_t bit_sum = 0;

    for(int i = 0; i <= 7; ++i) bit_sum += (value & ( 1 << i));

    a->FLAGS.P = !(bit_sum & 1);
    a->FLAGS.Z = value == 0;
    a->FLAGS.S = !!(value & 0x80);

}

void execute_NOP(arcade_t *a, struct ins *i) {};

void execute_LXI(arcade_t *a, struct ins *i) 
{
    *fetch_reg_16(a, i->args[0].value.reg) = i->args[1].value.imm16;
};

void execute_STAX(arcade_t *a, struct ins *i) 
{
    a->mem->mem[*fetch_reg_16(a, i->args[0].value.reg)] = a->A;
};

void execute_INX(arcade_t *a, struct ins *i) 
{
    (*fetch_reg_16(a, i->args[0].value.reg))++;
};

void execute_INR(arcade_t *a, struct ins *i) 
{

    set_flags(a,  ++(*fetch_reg_8(a, i->args[0].value.reg)));
};

void execute_DCR(arcade_t *a, struct ins *i) 
{
    set_flags(a, --(*fetch_reg_8(a, i->args[0].value.reg)));
};

void execute_MVI(arcade_t *a, struct ins *i) 
{
    *fetch_reg_8(a, i->args[0].value.reg) = i->args[1].value.imm8;
};

void execute_RLC(arcade_t *a, struct ins *i) 
{
    uint8_t a_reg = a->A;
    a->A <<= 1;
    
    a->A |= (a_reg >> 7);
    a->FLAGS.C = a_reg >> 7;

};

void execute_DAD(arcade_t *a, struct ins *i) 
{
    uint16_t to_add = *fetch_reg_16(a, i->args[0].value.reg);
    a->FLAGS.C = ((uint32_t)to_add + (uint32_t)a->HL) > UINT16_MAX;

    a->HL += to_add;
};

void execute_LDAX(arcade_t *a, struct ins *i) 
{
    a->A = a->mem->mem[*fetch_reg_16(a, i->args[0].value.reg)];
};

void execute_DCX(arcade_t *a, struct ins *i) 
{
    (*fetch_reg_16(a, i->args[0].value.reg))--;
};

void execute_RRC(arcade_t *a, struct ins *i) 
{
    uint8_t a_reg = a->A;

    a->A >>=  1;
    a->A |= ((a_reg & 1) << 7);
    a->FLAGS.C = a_reg & 1;
};

void execute_RAL(arcade_t *a, struct ins *i) 
{
    uint8_t a_reg = a->A;
    a->A <<= 1;

    a->A |= a->FLAGS.C;
    a->FLAGS.C = a_reg >> 7;
};

void execute_RAR(arcade_t *a, struct ins *i) 
{
    uint8_t a_reg = a->A;
    a->A >>= 1;
    a->A |= (a_reg & 0x80);
    a->FLAGS.C = a_reg & 1;
};

void execute_SHLD(arcade_t *a, struct ins *i) 
{
    *((uint16_t*)&a->mem->mem[i->args[0].value.imm16]) = a->HL; 
};

void execute_DAA(arcade_t *a, struct ins *i) 
{
    
};

void execute_LHLD(arcade_t *a, struct ins *i) 
{
    a->HL = *((uint16_t*) &a->mem->mem[i->args[0].value.imm16]);
};

void execute_CMA(arcade_t *a, struct ins *i) 
{
    a->A = ~a->A;
};

void execute_STA(arcade_t *a, struct ins *i) 
{
    a->mem->mem[i->args[0].value.imm16] = a->A;
};

void execute_STC(arcade_t *a, struct ins *i)
{
    a->FLAGS.C = 1;
};

void execute_LDA(arcade_t *a, struct ins *i)
{
    a->A = a->mem->mem[i->args[0].value.imm16];
};

void execute_CMC(arcade_t *a, struct ins *i) 
{
    a->FLAGS.C = !a->FLAGS.C;
};

void execute_MOV(arcade_t *a, struct ins *i) 
{
    *fetch_reg_8(a, i->args[0].value.reg) = *fetch_reg_8(a, i->args[1].value.reg);
};

void execute_HLT(arcade_t *a, struct ins *i) 
{
};

void execute_ADD(arcade_t *a, struct ins *i) 
{
    uint8_t to_add = *fetch_reg_8(a, i->args[0].value.reg);

    a->FLAGS.C = ((uint16_t)to_add + (uint16_t)a->A) > UINT8_MAX;
    a->A += to_add;

    set_flags(a, a->A);
};

void execute_ADC(arcade_t *a, struct ins *i) 
{        
    uint16_t to_add = *fetch_reg_8(a, i->args[0].value.reg) + a->FLAGS.C;
     
    a->FLAGS.C = (to_add + (uint16_t)a->A) > UINT8_MAX;
    a->A += to_add;
    set_flags(a, a->A);
};

void execute_SUB(arcade_t *a, struct ins *i) 
{
    uint16_t to_sub = *fetch_reg_8(a, i->args[0].value.reg);

    a->FLAGS.C = (to_sub <= a->A);
    a->A -= to_sub;
    set_flags(a, a->A);

};

void execute_SBB(arcade_t *a, struct ins *i) 
{
    uint16_t to_sub = *fetch_reg_8(a, i->args[0].value.reg) + a->FLAGS.C;
    a->FLAGS.C = (to_sub <= a->A);
    a->A -= to_sub;
    set_flags(a, a->A);
};

void execute_ANA(arcade_t *a, struct ins *i) 
{
    a->FLAGS.C = 0;
    a->A &= *fetch_reg_8(a, i->args[0].value.reg);

    set_flags(a, a->A);
};

void execute_XRA(arcade_t *a, struct ins *i) 
{
    a->FLAGS.C = 0;
    a->A ^= *fetch_reg_8(a, i->args[0].value.reg);

    set_flags(a, a->A);
};

void execute_ORA(arcade_t *a, struct ins *i) 
{
    a->FLAGS.C = 0;
    a->A |= *fetch_reg_8(a, i->args[0].value.reg);

    set_flags(a, a->A);
};

void execute_CMP(arcade_t *a, struct ins *i) 
{
    uint8_t comparison = *fetch_reg_8(a, i->args[0].value.reg);
    a->FLAGS.C = (comparison <= a->A);
    set_flags(a, a->A - comparison);
};

void execute_RNZ(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.Z) execute_RET(a, i); 
};

void execute_POP(arcade_t *a, struct ins *i) 
{
    a->BC = *((uint16_t*)&a->mem->mem[a->SP]);
    a->SP += 2;
};

void execute_JNZ(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.Z) a->PC = i->args[0].value.imm16 - i->bytecode_size;
    
};

void execute_JMP(arcade_t *a, struct ins *i) 
{
    a->PC = i->args[0].value.imm16 - i->bytecode_size;
};

void execute_CNZ(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.Z) execute_CALL(a, i);
};

void execute_PUSH(arcade_t *a, struct ins *i) 
{
    *fetch_reg_16(a, i->args[0].value.reg) = *((uint16_t*)&a->mem->mem[a->SP-2]);
};


void execute_ADI(arcade_t *a, struct ins *i) 
{
    
    a->FLAGS.C = ((uint16_t)a->A + (uint16_t)i->args[0].value.imm8) > UINT8_MAX;
    a->A += i->args[0].value.imm8;
    
    set_flags(a, a->A);
};

void execute_RST(arcade_t *a, struct ins *i) 
{
    *((uint16_t*)&a->mem->mem[a->SP-2]) = a->PC;
    a->SP -= 2;
    a->PC = i->args[0].value.constant * 0x08 - i->bytecode_size;
};

void execute_RZ(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.Z) execute_RET(a, i);
};

void execute_RET(arcade_t *a, struct ins *i) 
{
    a->PC = *((uint16_t*)&a->mem->mem[a->SP]) - i->bytecode_size;
    a->SP += 2;
};

void execute_JZ(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.Z) a->PC = i->args[0].value.imm16 - i->bytecode_size;
};

void execute_CZ(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.Z) execute_CALL(a, i);
};

void execute_CALL(arcade_t *a, struct ins *i) 
{
    *((uint16_t*)&a->mem->mem[a->SP-2]) = a->PC;
    a->SP -= 2;
    a->PC = i->args[0].value.imm16 - i->bytecode_size;
};

void execute_ACI(arcade_t *a, struct ins *i) 
{
    uint8_t carry = a->FLAGS.C;
    a->FLAGS.C = ( (uint16_t)a->A + (uint16_t)i->args[0].value.imm8 + (uint16_t)carry) > UINT8_MAX;
    a->A += i->args[0].value.imm8 + carry;

    set_flags(a, a->A);
};

void execute_RNC(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.C) execute_RET(a, i);
};

void execute_JNC(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.C) a->PC = i->args[0].value.imm16 - i->bytecode_size;
};

void execute_OUT(arcade_t *a, struct ins *i) 
{

};

void execute_CNC(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.C) execute_CALL(a, i);
};

void execute_SUI(arcade_t *a, struct ins *i) 
{
    a->FLAGS.C = (i->args[0].value.imm8 <= a->A);
    a->A -= i->args[0].value.imm8;

    set_flags(a, a->A);
};

void execute_RC(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.C) execute_RET(a, i);
};

void execute_JC(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.C) a->PC = i->args[0].value.imm16 - i->bytecode_size;
};

void execute_IN(arcade_t *a, struct ins *i) 
{

};

void execute_CC(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.C) execute_CALL(a, i);
};

void execute_SBI(arcade_t *a, struct ins *i) 
{
    uint8_t carry = a->FLAGS.C;
    a->FLAGS.C = (i->args[0].value.imm8 + carry) <= a->A;
    a->A -= (i->args[0].value.imm8 + carry);

    set_flags(a, a->A);
};

void execute_RPO(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.P) execute_RET(a, i);
};

void execute_JPO(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.P) a->PC = i->args[0].value.imm16 - i->bytecode_size;
};

void execute_XTHL(arcade_t *a, struct ins *i) 
{
    uint16_t tmp = *((uint16_t*)&a->mem->mem[a->SP]);
    
    *((uint16_t*)&a->mem->mem[a->SP]) = a->HL;
    a->HL = tmp;

};

void execute_CPO(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.P) execute_CALL(a, i);
};

void execute_ANI(arcade_t *a, struct ins *i) 
{
    a->FLAGS.C = 0; // AND can never result in a carry
    a->A &= i->args[0].value.imm8;
    set_flags(a, a->A);
};

void execute_RPE(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.P) execute_RET(a, i);
};

void execute_PCHL(arcade_t *a, struct ins *i) 
{
    a->PC = a->HL - i->bytecode_size;
};

void execute_JPE(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.P) a->PC = i->args[0].value.imm16 - i->bytecode_size;
};

void execute_XCHG(arcade_t *a, struct ins *i) 
{
    uint16_t tmp = a->HL;

    a->HL = a->DE;
    a->DE = tmp;
};

void execute_CPE(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.P) execute_CALL(a, i);
};

void execute_XRI(arcade_t *a, struct ins *i) 
{
    a->FLAGS.C = 0; // XOR can never result in a carry

    a->A ^= i->args[0].value.imm8;

    set_flags(a, a->A);
};

void execute_RP(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.S) execute_RET(a, i);
};

void execute_JP(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.S) a->PC = i->args[0].value.imm16 - i->bytecode_size;
};

void execute_DI(arcade_t *a, struct ins *i) 
{

};

void execute_CP(arcade_t *a, struct ins *i) 
{
    if(!a->FLAGS.S) execute_CALL(a, i);
};

void execute_ORI(arcade_t *a, struct ins *i) 
{
    a->FLAGS.C = 0; // OR can never result in a carry
    a->A |= i->args[0].value.imm8;
};

void execute_RM(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.S) execute_RET(a, i);
};

void execute_SPHL(arcade_t *a, struct ins *i) 
{
    a->SP = a->HL;
};

void execute_JM(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.S) a->PC = i->args[0].value.imm16 - i->bytecode_size;
};

void execute_EI(arcade_t *a, struct ins *i) 
{
    
};

void execute_CM(arcade_t *a, struct ins *i) 
{
    if(a->FLAGS.S) execute_CALL(a, i);
};

void execute_CPI(arcade_t *a, struct ins *i) 
{
    
    uint8_t comparison = i->args[0].value.imm8;
    a->FLAGS.C = (comparison <= a->A);
    set_flags(a, a->A - comparison);
};
