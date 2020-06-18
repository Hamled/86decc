#include "86decc.h"
#include "debug.h"

#include <math.h>
#include <stdlib.h>

REGISTER regbits_to_enum_w(uint8_t bits, bool w, OPSIZE size) {
    switch(bits) {
        case 0b000:
            if(!w) return REG_AL;
            return (size == SIZE_16) ? REG_AX : REG_EAX;
        case 0b001:
            if(!w) return REG_CL;
            return (size == SIZE_16) ? REG_CX : REG_ECX;
        case 0b010:
            if(!w) return REG_DL;
            return (size == SIZE_16) ? REG_DX : REG_EDX;
        case 0b011:
            if(!w) return REG_BL;
            return (size == SIZE_16) ? REG_BX : REG_EBX;
        case 0b100:
            if(!w) return REG_AH;
            return (size == SIZE_16) ? REG_SP : REG_ESP;
        case 0b101:
            if(!w) return REG_CH;
            return (size == SIZE_16) ? REG_BP : REG_EBP;
        case 0b110:
            if(!w) return REG_DH;
            return (size == SIZE_16) ? REG_SI : REG_ESI;
        case 0b111:
            if(!w) return REG_BH;
            return (size == SIZE_16) ? REG_DI : REG_EDI;
    }

    return REG_UNKNOWN;
}

static void decode_sib(const uint8_t sib, const uint8_t mod, OPERAND* oper) {
    const uint8_t scale = sib >> 6;
    const uint8_t index = (sib & 0x38) >> 3;
    const uint8_t base = sib & 0x7;

    oper->type = OT_SIB;
    oper->sib.scale = pow(2, scale);
    oper->sib.index = regbits_to_enum_w(index, true, SIZE_32);
    oper->sib.base = regbits_to_enum_w(base, true, SIZE_32);

    if(index == 0b100) {
        oper->sib.scale = 0;
        oper->sib.index = REG_NONE;
    }

    if(mod == 0b00 && base == 0b101) {
        oper->sib.base = REG_NONE;
    }
}

static size_t decode_opcode_basic(uint8_t flags, const uint8_t* ixns, INSTR* instr) {
    // Parse flags
    const bool imm = flags & 0x4;
    const bool d = flags & 0x2;
    const bool w = flags & 0x1;

    if(imm) {
        instr->operand1.type = OT_REGISTER;
        instr->operand2.type = OT_IMMEDIATE;
        if(!w) {
            instr->operand1.reg = REG_AL;
            instr->operand2.immediate = *ixns;
            return 2;
        }

        // TODO: support SIZE_16
        instr->operand1.reg = REG_EAX;
        instr->operand2.immediate = *(uint32_t*)ixns;
        return 5;
    }

    size_t instr_size = 2;

    const uint8_t modrm = *ixns;
    const uint8_t mod = modrm >> 6;
    const uint8_t rm  = modrm & 0x7;
    const uint8_t reg = (modrm & 0x38) >> 3;

    instr->operand1.type = OT_REGISTER;
    instr->operand1.reg = regbits_to_enum_w(reg, w, SIZE_32);

    instr->operand2.type = OT_MODRM;
    switch(mod) {
        case 0b00:
            if(rm == 0b100) {
                // SIB
                decode_sib(*(ixns + 1), mod, &instr->operand2);
                instr_size += 1;
                if(instr->operand2.sib.base == REG_NONE) {
                    // SIB + 32-bit displacement
                    instr->operand2.sib.displacement = *(int32_t*)(ixns + 2);
                    instr_size += 4;
                }
            } else if(rm == 0b101) {
                // 32-bit displacement
                instr->operand2.modrm.reg = REG_NONE;
                instr->operand2.modrm.displacement = *(int32_t*)(ixns + 1);

                instr_size += 4;
            } else {
                // 32-bit register, no displacement
                instr->operand2.modrm.reg = regbits_to_enum_w(rm, true, SIZE_32);
                instr->operand2.modrm.displacement = 0;
            }
            break;
        case 0b01:
            instr_size += 1;
            if(rm == 0b100) {
                // SIB + 8-bit displacement
                decode_sib(*(ixns + 1), mod, &instr->operand2);
                instr->operand2.sib.displacement = *(int8_t*)(ixns + 2);
                instr_size += 1;
            } else {
                // 32-bit register + 8-bit displacement
                instr->operand2.modrm.reg = regbits_to_enum_w(rm, true, SIZE_32);
                instr->operand2.modrm.displacement = *(int8_t*)(ixns + 1);
            }
            break;
        case 0b10:
            instr_size += 4;
            if(rm == 0b100) {
                // SIB + 32-bit displacement
                decode_sib(*(ixns + 1), mod, &instr->operand2);
                instr->operand2.sib.displacement = *(int32_t*)(ixns + 2);
                instr_size += 1;
            } else {
                // 32-bit register + 32-bit displacement
                instr->operand2.modrm.reg = regbits_to_enum_w(rm, true, SIZE_32);
                instr->operand2.modrm.displacement = *(int32_t*)(ixns + 1);
            }
            break;
        case 0b11:
            instr->operand2.type = OT_REGISTER;
            instr->operand2.reg = regbits_to_enum_w(rm, w, SIZE_32);
            break;
    }

    // If direction bit is set, swap the operands
    if(!d) {
        OPERAND tmp = {0};
        tmp = instr->operand1;
        instr->operand1 = instr->operand2;
        instr->operand2 = tmp;
    }

    return instr_size;
}


// Decode a single instruction from the byte stream ixns
// into the INSTR structure passed by reference
// This assumes that instr has been initialized to all zeroes
size_t decode(const uint8_t* ixns, INSTR* instr) {
    if(!instr) abort();

    const uint8_t ixn = *ixns;

    const uint8_t opcode_h = ixn >> 4;
    const uint8_t opcode_l = ixn & 0xF;
    // Parse out prefix bytes
    // 2x
        // Prefix byte (CS/ES seg override)
    // 3x
        // Prefix byte (DS/SS seg override)
    // 6x
        // Prefix byte (address size)
        // Prefix byte (operand size)
        // Prefix byte (FS/GS seg override)
    // Fx
        // Prefix byte (LOCK)
        // Prefix byte (REP/REPE/REPNE)

    // Parse the opcode and following bytes
    switch(opcode_h) {
        case 0b0000:
            // All 2 or 3 byte opcodes
            if(opcode_l == 0xF) break;

            // OP_ADD
            // OP_OR
            instr->opcode = (opcode_l & 0x8) ? OP_OR : OP_ADD;
            return decode_opcode_basic(opcode_l & 0x7, ixns + 1, instr);
        case 0b0001:
            // ADC
            // SBB
            instr->opcode = (opcode_l & 0x8) ? OP_SBB : OP_ADC;
            return decode_opcode_basic(opcode_l & 0x7, ixns + 1, instr);
        case 0b0010:
            // DAA
            // DAS
            if(opcode_l == 0x7 || opcode_l == 0xF) break;

            // AND
            // SUB
            instr->opcode = (opcode_l & 0x8) ? OP_SUB : OP_AND;
            return decode_opcode_basic(opcode_l & 0x7, ixns + 1, instr);
        case 0b0011:
            // AAA
            // AAS
            if(opcode_l == 0x7 || opcode_l == 0xF) break;

            // CMP
            // XOR
            instr->opcode = (opcode_l & 0x8) ? OP_CMP : OP_XOR;
            return decode_opcode_basic(opcode_l & 0x7, ixns + 1, instr);

        case 0b0100:
            // DEC (alternate)
            // INC (alternate)
            break;
        case 0b0101:
            // POP (alternate)
            // PUSH (alternate)
            break;
        case 0b0110:
            // OP_PUSHA/OP_PUSHAD
            // OP_POPA/OP_POPAD
            // BOUND
            // ARPL
            // PUSH
            // IMUL
            // INS
            // OUTS
            break;
        case 0b0111:
            // Jcc
            break;
        case 0b1000:
            // ADC (immediate)
            // OP_ADD (immediate)
            // AND (immediate)
            // CMP (immediate)
            // LEA
            // MOV
            // MOV (seg register)
            // OP_OR (immediate)
            // POP
            // SBB (immediate)
            // SUB (immediate)
            // TEST
            // XCHG
            // XOR (immediate)
            break;
        case 0b1001:
            // NOP/XCHG EAX, EAX
            // OP_CBW/OP_CWDE
            // OP_CWD/OP_CDQ
            // CALL (direct, other seg)
            // WAIT
            // PUSHF/PUSHFD
            // POPF/POPFD
            // SAHF
            // LAHF
            break;
        case 0b1010:
            // CMPS/CMPSB/CMPSW/CMPSD
            // LODS/LODSB/LODSW/LODSD
            // MOV EAX (to and from)
            // MOVS/MOVSB/MOVSW/MOVSD
            // SCAS/SCASB/SCASW/SCASD
            // STOS/STOSB/STOSW/STOSD
            // TEST EAX, immediate
            break;
        case 0b1011:
            // MOV (alternate)
            break;
        case 0b1100:
            // INT n
            // INT 3
            // INTO
            // IRET/IRETD
            // LDS
            // LEAVE
            // LES
            // MOV (immediate)
            // RCL (immediate)
            // RCR (immediate)
            // RET
            // ROL (immediate)
            // ROR (immediate)
            // SAR (immediate)
            // SHL (immediate)
            // SHR (immediate)
            break;
        case 0b1101:
            // AAD
            // AAM
            // RCL
            // RCR
            // ROL
            // ROR
            // SAR
            // SHL
            // SHR
            // XLAT/XLATB
            break;
        case 0b1110:
            // CALL (direct)
            // IN
            // JCXZ/JECXZ
            // JMP (direct)
            // LOOP
            // LOOPZ/LOOPE
            // LOOPNZ/LOOPNE
            // OUT
            break;
        case 0b1111:
            // CALL (indirect)
            // CLC
            // CLD
            // CLI
            // CMC
            // DEC
            // DIV
            // HALT
            // IDIV
            // IMUL EAX
            // INC
            // JMP (indirect)
            // LOCK
            // MUL
            // NEG
            // NOT
            // PUSH
            // STC
            // STD
            // STI
            // TEST (immediate)
            break;
    }

    return 0;
}
