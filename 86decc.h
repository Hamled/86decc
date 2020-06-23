#ifndef __86DECC_H_
#define __86DECC_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

typedef enum OPCODE_t {
    OP_UNKNOWN = 0,
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
    OP_XOR,
    OP_CMP,

    OP_INC,
    OP_DEC,
} OPCODE;

typedef enum REGISTER_t {
    REG_UNKNOWN = -1,
    REG_NONE,
    REG_AL,  // 1
    REG_AH,
    REG_AX,
    REG_EAX, // 4
    REG_CL,  // 5
    REG_CH,
    REG_CX,
    REG_ECX, // 8
    REG_DL,  // 9
    REG_DH,
    REG_DX,
    REG_EDX, // 12
    REG_BL,  // 13
    REG_BH,
    REG_BX,
    REG_EBX, // 16
    REG_SP,
    REG_ESP, // 18
    REG_BP,
    REG_EBP, // 20
    REG_SI,
    REG_ESI, // 22
    REG_DI,
    REG_EDI, // 24
} REGISTER;

typedef enum OPERAND_TYPE_t {
    OT_NONE = 0,
    OT_REGISTER,
    OT_MEMORY,
    OT_IMMEDIATE,
    OT_MODRM,
    OT_SIB,
} OPERAND_TYPE;

typedef struct OPERAND_MODRM_t {
    REGISTER reg;
    int32_t displacement;
} OPERAND_MODRM;

typedef struct OPERAND_SIB_t {
    uint8_t scale;
    REGISTER index;
    REGISTER base;
    int32_t displacement;
} OPERAND_SIB;

typedef struct OPERAND_t {
    OPERAND_TYPE type;
    union {
        REGISTER reg;
        uint32_t memory;
        uint32_t immediate;
        OPERAND_MODRM modrm;
        OPERAND_SIB sib;
    };
} OPERAND;

typedef struct INSTR_t {
    OPCODE opcode;
    OPERAND operand1;
    OPERAND operand2;
} INSTR;

typedef enum OPSIZE_t {
    SIZE_16,
    SIZE_32,
} OPSIZE;


size_t decode(const uint8_t* ixns, INSTR* instr);


#endif // __86DECC_H_
