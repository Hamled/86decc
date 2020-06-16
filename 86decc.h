#ifndef __86DECC_H_
#define __86DECC_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum OPCODE_t {
    OP_PUSHA,
    OP_PUSHAD,
    OP_POPA,
    OP_POPAD,
    OP_CBW,
    OP_CWDE,
    OP_CWD,
    OP_CDQ,

    OP_ADD,
    OP_OR,
    OP_ADC,
    OP_SBB,
    OP_AND,
    OP_SUB,
    OP_CMP,
    OP_XOR,
} OPCODE;

typedef enum REGISTER_t {
    REG_UNKNOWN,
    REG_AL,
    REG_AH,
    REG_AX,
    REG_EAX,
    REG_BL,
    REG_BH,
    REG_BX,
    REG_EBX,
    REG_CL,
    REG_CH,
    REG_CX,
    REG_ECX,
    REG_DL,
    REG_DH,
    REG_DX,
    REG_EDX,
    REG_SP,
    REG_ESP,
    REG_BP,
    REG_EBP,
    REG_SI,
    REG_ESI,
    REG_DI,
    REG_EDI,
} REGISTER;

typedef struct INSTR_t {
    OPCODE opcode;
    uint32_t operand1;
    uint32_t operand2;
} INSTR;

typedef enum OPSIZE_t {
    SIZE_16,
    SIZE_32,
} OPSIZE;


size_t decode(const uint8_t* ixns, INSTR* instr);


#endif // __86DECC_H_
