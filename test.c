#include "86decc.h"
#include "utest.h"

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
