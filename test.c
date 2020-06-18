#include "86decc.h"
#include "utest.h"

#include <math.h>

UTEST_MAIN();

UTEST(add, al_imm8) {
    const uint8_t input[] = { 0x04, 0xAB };
    INSTR output = {0};

    decode(input, &output);

    ASSERT_EQ((OPCODE)OP_ADD, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_AL, output.operand1.reg);

    ASSERT_EQ((OPERAND_TYPE)OT_IMMEDIATE, output.operand2.type);
    ASSERT_EQ((uint32_t)0xAB, output.operand2.immediate);
}

UTEST(and, eax_imm32) {
    const uint8_t input[] = { 0x25, 0xAB, 0xCD, 0xEF, 0x89 };
    INSTR output = {0};

    decode(input, &output);

    ASSERT_EQ((OPCODE)OP_AND, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_EAX, output.operand1.reg);

    ASSERT_EQ((OPERAND_TYPE)OT_IMMEDIATE, output.operand2.type);
    ASSERT_EQ(0x89EFCDAB, output.operand2.immediate);
}

UTEST(xor, reg1_reg2) {
    const uint8_t input[] = { 0x31, 0xC8 };
    INSTR output = {0};

    decode(input, &output);

    ASSERT_EQ((OPCODE)OP_XOR, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_EAX, output.operand1.reg);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
    ASSERT_EQ((REGISTER)REG_ECX, output.operand2.reg);
}

UTEST(sub, reg2_reg1_8) {
    const uint8_t input[] = { 0x2A, 0xC8 };
    INSTR output = {0};

    decode(input, &output);

    ASSERT_EQ((OPCODE)OP_SUB, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_CL, output.operand1.reg);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
    ASSERT_EQ((REGISTER)REG_AL, output.operand2.reg);
}

UTEST(sbb, memdisp0_reg8) {
    const uint8_t input[] = { 0x18, 0x13 };
    INSTR output = {0};

    decode(input, &output);

    ASSERT_EQ((OPCODE)OP_SBB, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_MODRM, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_EBX, output.operand1.modrm.reg);
    ASSERT_EQ((int32_t)0x0, output.operand1.modrm.displacement);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
    ASSERT_EQ((REGISTER)REG_DL, output.operand2.reg);
}

UTEST(or, memdisp8_reg32) {
    const uint8_t input[] = { 0x09, 0x6E, 0x44 };
    INSTR output = {0};

    decode(input, &output);

    ASSERT_EQ((OPCODE)OP_OR, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_MODRM, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_ESI, output.operand1.modrm.reg);
    ASSERT_EQ((int32_t)0x44, output.operand1.modrm.displacement);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
    ASSERT_EQ((REGISTER)REG_EBP, output.operand2.reg);
}

UTEST(adc, memdisp32_reg32) {
    const uint8_t input[] = { 0x11, 0xA7, 0x11, 0x22, 0x33, 0x44 };
    INSTR output = {0};

    decode(input, &output);

    ASSERT_EQ((OPCODE)OP_ADC, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_MODRM, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_EDI, output.operand1.modrm.reg);
    ASSERT_EQ(0x44332211, output.operand1.modrm.displacement);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
    ASSERT_EQ((REGISTER)REG_ESP, output.operand2.reg);
}

UTEST(cmp, reg8_disp32) {
    const uint8_t input[] = { 0x3A, 0x3D, 0xA1, 0xB2, 0xC3, 0xD4 };
    INSTR output = {0};

    decode(input, &output);

    ASSERT_EQ((OPCODE)OP_CMP, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_BH, output.operand1.reg);

    ASSERT_EQ((OPERAND_TYPE)OT_MODRM, output.operand2.type);
    ASSERT_EQ((REGISTER)REG_NONE, output.operand2.modrm.reg);
    ASSERT_EQ((int32_t)0xD4C3B2A1, output.operand2.modrm.displacement);
}

UTEST(multi, instr2_disp8) {
    const uint8_t input[] = { 0x09, 0x6E, 0x44, 0x00, 0x00 };
    INSTR output = {0};

    const uint8_t* cursor = input;
    const size_t instr_size = decode(cursor, &output);

    ASSERT_EQ((OPCODE)OP_OR, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_MODRM, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_ESI, output.operand1.modrm.reg);
    ASSERT_EQ((int32_t)0x44, output.operand1.modrm.displacement);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
    ASSERT_EQ((REGISTER)REG_EBP, output.operand2.reg);

    memset(&output, 0, sizeof(output));
    cursor += instr_size;
    decode(cursor, &output);

    ASSERT_EQ((OPCODE)OP_ADD, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_MODRM, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_EAX, output.operand1.modrm.reg);
    ASSERT_EQ((int32_t)0, output.operand1.modrm.displacement);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
    ASSERT_EQ((REGISTER)REG_AL, output.operand2.reg);
}

UTEST(multi, instr2_disp32) {
    const uint8_t input[] = { 0x3A, 0x3D, 0xA1, 0xB2, 0xC3, 0xD4, 0x00, 0x00 };
    INSTR output = {0};

    const uint8_t* cursor = input;
    const size_t instr_size = decode(cursor, &output);

    ASSERT_EQ((OPCODE)OP_CMP, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_BH, output.operand1.reg);

    ASSERT_EQ((OPERAND_TYPE)OT_MODRM, output.operand2.type);
    ASSERT_EQ((REGISTER)REG_NONE, output.operand2.modrm.reg);
    ASSERT_EQ((int32_t)0xD4C3B2A1, output.operand2.modrm.displacement);

    memset(&output, 0, sizeof(output));
    cursor += instr_size;
    decode(cursor, &output);

    ASSERT_EQ((OPCODE)OP_ADD, output.opcode);

    ASSERT_EQ((OPERAND_TYPE)OT_MODRM, output.operand1.type);
    ASSERT_EQ((REGISTER)REG_EAX, output.operand1.modrm.reg);
    ASSERT_EQ((int32_t)0, output.operand1.modrm.displacement);

    ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
    ASSERT_EQ((REGISTER)REG_AL, output.operand2.reg);
}

UTEST(or, sibdisp0_reg32) {
    const uint8_t input[] = { 0x09, 0x3C, 0x01,
                              0x09, 0x3C, 0x41,
                              0x09, 0x3C, 0x81,
                              0x09, 0x3C, 0xC1, };
    INSTR output = {0};

    const uint8_t* cursor = input;
    const uint8_t scales[4] = { 0x1, 0x2, 0x4, 0x8 };
    for(size_t i = 0; i < 4; i++) {
        const uint8_t scale = scales[i];

        memset(&output, 0, sizeof(output));
        const size_t instr_size = decode(cursor, &output);

        ASSERT_EQ((OPCODE)OP_OR, output.opcode);

        ASSERT_EQ((OPERAND_TYPE)OT_SIB, output.operand1.type);
        ASSERT_EQ(scale, output.operand1.sib.scale);
        ASSERT_EQ((REGISTER)REG_EAX, output.operand1.sib.index);
        ASSERT_EQ((REGISTER)REG_ECX, output.operand1.sib.base);
        ASSERT_EQ(0x0, output.operand2.sib.displacement);

        ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
        ASSERT_EQ((REGISTER)REG_EDI, output.operand2.reg);

        cursor += instr_size;
    }
}

UTEST(xor, reg8_sibdisp8) {
    const uint8_t input[] = { 0x32, 0x64, 0x0B, 0xE4,
                              0x32, 0x64, 0x4B, 0xE4,
                              0x32, 0x64, 0x8B, 0xE4,
                              0x32, 0x64, 0xCB, 0xE4, };
    INSTR output = {0};

    const uint8_t* cursor = input;
    const uint8_t scales[4] = { 0x1, 0x2, 0x4, 0x8 };
    for(size_t i = 0; i < 4; i++) {
        const uint8_t scale = scales[i];

        memset(&output, 0, sizeof(output));
        const size_t instr_size = decode(cursor, &output);

        ASSERT_EQ((OPCODE)OP_XOR, output.opcode);

        ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand1.type);
        ASSERT_EQ((REGISTER)REG_AH, output.operand1.reg);

        ASSERT_EQ((OPERAND_TYPE)OT_SIB, output.operand2.type);
        ASSERT_EQ(scale, output.operand2.sib.scale);
        ASSERT_EQ((REGISTER)REG_ECX, output.operand2.sib.index);
        ASSERT_EQ((REGISTER)REG_EBX, output.operand2.sib.base);
        ASSERT_EQ((int8_t)0xE4, output.operand2.sib.displacement);

        cursor += instr_size;
    }
}

UTEST(sbb, sibdisp32_reg8) {
    const uint8_t input[] = { 0x18, 0x94, 0x2A, 0x45, 0x32, 0x01, 0xAF,
                              0x18, 0x94, 0x6A, 0x45, 0x32, 0x01, 0xAF,
                              0x18, 0x94, 0xAA, 0x45, 0x32, 0x01, 0xAF,
                              0x18, 0x94, 0xEA, 0x45, 0x32, 0x01, 0xAF, };
    INSTR output = {0};

    const uint8_t* cursor = input;
    const uint8_t scales[4] = { 0x1, 0x2, 0x4, 0x8 };
    for(size_t i = 0; i < 4; i++) {
        const uint8_t scale = scales[i];

        memset(&output, 0, sizeof(output));
        const size_t instr_size = decode(cursor, &output);

        ASSERT_EQ((OPCODE)OP_SBB, output.opcode);

        ASSERT_EQ((OPERAND_TYPE)OT_SIB, output.operand1.type);
        ASSERT_EQ(scale, output.operand1.sib.scale);
        ASSERT_EQ((REGISTER)REG_EBP, output.operand1.sib.index);
        ASSERT_EQ((REGISTER)REG_EDX, output.operand1.sib.base);
        ASSERT_EQ((int32_t)0xAF013245, output.operand1.sib.displacement);

        ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
        ASSERT_EQ((REGISTER)REG_DL, output.operand2.reg);

        cursor += instr_size;
    }
}

UTEST(cmp, sibdisp0no_scale__reg8) {
    const uint8_t input[] = { 0x38, 0x1C, 0x26,
                              0x38, 0x1C, 0x66,
                              0x38, 0x1C, 0xA6,
                              0x38, 0x1C, 0xE6, };
    INSTR output = {0};

    const uint8_t* cursor = input;
    for(size_t i = 0; i < 4; i++) {
        memset(&output, 0, sizeof(output));
        const size_t instr_size = decode(cursor, &output);

        ASSERT_EQ((OPCODE)OP_CMP, output.opcode);

        ASSERT_EQ((OPERAND_TYPE)OT_SIB, output.operand1.type);
        ASSERT_EQ(0x0, output.operand1.sib.scale);
        ASSERT_EQ((REGISTER)REG_NONE, output.operand1.sib.index);
        ASSERT_EQ((REGISTER)REG_ESI, output.operand1.sib.base);
        ASSERT_EQ(0x0, output.operand1.sib.displacement);

        ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
        ASSERT_EQ((REGISTER)REG_BL, output.operand2.reg);

        cursor += instr_size;
    }
}

UTEST(sub, sibdisp32no_ebp__reg32) {
    const uint8_t input[] = { 0x29, 0x2C, 0x05, 0x34, 0x1A, 0x88, 0x90,
                              0x29, 0x2C, 0x45, 0x34, 0x1A, 0x88, 0x90,
                              0x29, 0x2C, 0x85, 0x34, 0x1A, 0x88, 0x90,
                              0x29, 0x2C, 0xC5, 0x34, 0x1A, 0x88, 0x90, };
    INSTR output = {0};

    const uint8_t* cursor = input;
    for(size_t i = 0; i < 4; i++) {
        const uint8_t scale = pow(2, i);
        memset(&output, 0, sizeof(output));
        const size_t instr_size = decode(cursor, &output);

        ASSERT_EQ((OPCODE)OP_SUB, output.opcode);

        ASSERT_EQ((OPERAND_TYPE)OT_SIB, output.operand1.type);
        ASSERT_EQ(scale, output.operand1.sib.scale);
        ASSERT_EQ((REGISTER)REG_EAX, output.operand1.sib.index);
        ASSERT_EQ((REGISTER)REG_NONE, output.operand1.sib.base);
        ASSERT_EQ((int32_t)0x90881A34, output.operand1.sib.displacement);

        ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand2.type);
        ASSERT_EQ((REGISTER)REG_EBP, output.operand2.reg);

        cursor += instr_size;
    }
}

UTEST(adc, reg32__sibdisp8_ebp) {
    const uint8_t input[] = { 0x13, 0x7C, 0x0D, 0xA8,
                              0x13, 0x7C, 0x4D, 0xA8,
                              0x13, 0x7C, 0x8D, 0xA8,
                              0x13, 0x7C, 0xCD, 0xA8, };
    INSTR output = {0};

    const uint8_t* cursor = input;
    for(size_t i = 0; i < 4; i++) {
        const uint8_t scale = pow(2, i);
        memset(&output, 0, sizeof(output));
        const size_t instr_size = decode(cursor, &output);

        ASSERT_EQ((OPCODE)OP_ADC, output.opcode);

        ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand1.type);
        ASSERT_EQ((REGISTER)REG_EDI, output.operand1.reg);

        ASSERT_EQ((OPERAND_TYPE)OT_SIB, output.operand2.type);
        ASSERT_EQ(scale, output.operand2.sib.scale);
        ASSERT_EQ((REGISTER)REG_ECX, output.operand2.sib.index);
        ASSERT_EQ((REGISTER)REG_EBP, output.operand2.sib.base);
        ASSERT_EQ((int8_t)0xA8, output.operand2.sib.displacement);

        cursor += instr_size;
    }
}

UTEST(add, reg8__sibdisp32_ebp) {
    const uint8_t input[] = { 0x02, 0xB4, 0x15, 0xA8, 0xB9, 0xC7, 0xD4,
                              0x02, 0xB4, 0x55, 0xA8, 0xB9, 0xC7, 0xD4,
                              0x02, 0xB4, 0x95, 0xA8, 0xB9, 0xC7, 0xD4,
                              0x02, 0xB4, 0xD5, 0xA8, 0xB9, 0xC7, 0xD4, };
    INSTR output = {0};

    const uint8_t* cursor = input;
    for(size_t i = 0; i < 4; i++) {
        const uint8_t scale = pow(2, i);
        memset(&output, 0, sizeof(output));
        const size_t instr_size = decode(cursor, &output);

        ASSERT_EQ((OPCODE)OP_ADD, output.opcode);

        ASSERT_EQ((OPERAND_TYPE)OT_REGISTER, output.operand1.type);
        ASSERT_EQ((REGISTER)REG_DH, output.operand1.reg);

        ASSERT_EQ((OPERAND_TYPE)OT_SIB, output.operand2.type);
        ASSERT_EQ(scale, output.operand2.sib.scale);
        ASSERT_EQ((REGISTER)REG_EDX, output.operand2.sib.index);
        ASSERT_EQ((REGISTER)REG_EBP, output.operand2.sib.base);
        ASSERT_EQ((int32_t)0xD4C7B9A8, output.operand2.sib.displacement);

        cursor += instr_size;
    }
}
