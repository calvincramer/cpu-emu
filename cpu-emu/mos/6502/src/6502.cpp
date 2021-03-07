/*
Emulate the MOS 6502 cpu 
*/

#include <cstdio>

#include "mos6502.hpp"


void mos6502::CPU::reset() {
    // Clear RAM
    for (u32 i = 0; i < MEM_MAX; ++i) 
        ram[i] = 0;
    // Reset registers
    PC = RESET_LOC;
    A = 0;
    X = 0;
    Y = 0;
    SR.C = 0;
    SR.Z = 0;
    SR.I = 0;
    SR.D = 0;
    SR.B = 0;
    SR._ = 1;   // Apparantly always high
    SR.V = 0;
    SR.N = 0;
    S = 0;
}

// Returns -1 on illegal instruction
u32 mos6502::CPU::execute(u32 p_numCycles) {
    u32 numCyclesSave = p_numCycles;  // Original number of cycles to execute
    numCycles = p_numCycles;
    u8 currentInstr;

    while (numCycles > 0) {
        currentInstr = getCurrentInstr();
        // Execute instruction
        switch (currentInstr) {
            case LDA_IMM : load(IMM, A);                            break;
            case LDA_ZPG : load(ZPG, A);                            break;
            case LDA_ZPX : load(ZPX, A, X);                         break;
            case LDA_ABS : load(ABS, A);                            break;
            case LDA_ABX : load(ABX, A, X);                         break;
            case LDA_ABY : load(ABY, A, Y);                         break;
            case LDA_IDX : load(IDX, A, X);                         break;
            case LDA_IDY : load(IDY, A, Y);                         break;
            case LDX_IMM : load(IMM, X);                            break;
            case LDX_ZPG : load(ZPG, X);                            break;
            case LDX_ZPY : load(ZPY, X, Y);                         break;
            case LDX_ABS : load(ABS, X);                            break;
            case LDX_ABY : load(ABY, X, Y);                         break;
            case LDY_IMM : load(IMM, Y);                            break;
            case LDY_ZPG : load(ZPG, Y);                            break;
            case LDY_ZPX : load(ZPX, Y, X);                         break;
            case LDY_ABS : load(ABS, Y);                            break;
            case LDY_ABX : load(ABX, Y, X);                         break;
            case STA_ZPG : store(ZPG, A);                           break;
            case STA_ZPX : store(ZPX, A, X);                        break;
            case STA_ABS : store(ABS, A);                           break;
            case STA_ABX : store(ABX, A, X);                        break;
            case STA_ABY : store(ABY, A, Y);                        break;
            case STA_IDX : store(IDX, A, X);                        break;
            case STA_IDY : store(IDY, A, Y);                        break;
            case STX_ZPG : store(ZPG, X);                           break;
            case STX_ZPY : store(ZPY, X, Y);                        break;
            case STX_ABS : store(ABS, X);                           break;
            case STY_ZPG : store(ZPG, Y);                           break;
            case STY_ZPX : store(ZPX, Y, X);                        break;
            case STY_ABS : store(ABS, Y);                           break;
            case TAX_IMP : transfer(A, X, true);                    break;
            case TAY_IMP : transfer(A, Y, true);                    break;
            case TSX_IMP : transfer(S, X, true);                    break;
            case TXA_IMP : transfer(X, A, true);                    break;
            case TXS_IMP : transfer(X, S, false);                   break;
            case TYA_IMP : transfer(Y, A, true);                    break;
            case NOP_IMP : /* do nothing */                         break;
            case CLC_IMP : SR.C = 0;                                break;
            case CLD_IMP : SR.D = 0;                                break;
            case CLI_IMP : SR.I = 0;                                break;
            case CLV_IMP : SR.V = 0;                                break;
            case SEC_IMP : SR.C = 1;                                break;
            case SED_IMP : SR.D = 1;                                break;
            case SEI_IMP : SR.I = 1;                                break;
            case INC_ZPG : inc_dec(ZPG,  1);                        break;
            case INC_ZPX : inc_dec(ZPX,  1, X);                     break;
            case INC_ABS : inc_dec(ABS,  1);                        break;
            case INC_ABX : inc_dec(ABX,  1, X);                     break;
            case INX_IMP : inc_dec(IMP,  1, 0, &X);                 break;
            case INY_IMP : inc_dec(IMP,  1, 0, &Y);                 break;
            case DEC_ZPG : inc_dec(ZPG, -1);                        break;
            case DEC_ZPX : inc_dec(ZPX, -1, X);                     break;
            case DEC_ABS : inc_dec(ABS, -1);                        break;
            case DEC_ABX : inc_dec(ABX, -1, X);                     break;
            case DEX_IMP : inc_dec(IMP, -1, 0, &X);                 break;
            case DEY_IMP : inc_dec(IMP, -1, 0, &Y);                 break;
            case AND_ZPG : arith(ZPG, &CPU::bitwise_and);           break;
            case AND_IMM : arith(IMM, &CPU::bitwise_and);           break;
            case AND_ZPX : arith(ZPX, &CPU::bitwise_and, X);        break;
            case AND_ABS : arith(ABS, &CPU::bitwise_and);           break;
            case AND_ABX : arith(ABX, &CPU::bitwise_and, X);        break;
            case AND_ABY : arith(ABY, &CPU::bitwise_and, Y);        break;
            case AND_IDX : arith(IDX, &CPU::bitwise_and, X);        break;
            case AND_IDY : arith(IDY, &CPU::bitwise_and, Y);        break;
            case EOR_IMM : arith(IMM, &CPU::bitwise_eor);           break;
            case EOR_ZPG : arith(ZPG, &CPU::bitwise_eor);           break;
            case EOR_ZPX : arith(ZPX, &CPU::bitwise_eor, X);        break;
            case EOR_ABS : arith(ABS, &CPU::bitwise_eor);           break;
            case EOR_ABX : arith(ABX, &CPU::bitwise_eor, X);        break;
            case EOR_ABY : arith(ABY, &CPU::bitwise_eor, Y);        break;
            case EOR_IDX : arith(IDX, &CPU::bitwise_eor, X);        break;
            case EOR_IDY : arith(IDY, &CPU::bitwise_eor, Y);        break;
            case ORA_IMM : arith(IMM, &CPU::bitwise_or);            break;
            case ORA_ZPG : arith(ZPG, &CPU::bitwise_or);            break;
            case ORA_ZPX : arith(ZPX, &CPU::bitwise_or,  X);        break;
            case ORA_ABS : arith(ABS, &CPU::bitwise_or);            break;
            case ORA_ABX : arith(ABX, &CPU::bitwise_or,  X);        break;
            case ORA_ABY : arith(ABY, &CPU::bitwise_or,  Y);        break;
            case ORA_IDX : arith(IDX, &CPU::bitwise_or,  X);        break;
            case ORA_IDY : arith(IDY, &CPU::bitwise_or,  Y);        break;
            case ASL_IMP : shift_rot(IMP, &CPU::shift_left);        break;
            case ASL_ZPG : shift_rot(ZPG, &CPU::shift_left);        break;
            case ASL_ZPX : shift_rot(ZPX, &CPU::shift_left,  X);    break;
            case ASL_ABS : shift_rot(ABS, &CPU::shift_left);        break;
            case ASL_ABX : shift_rot(ABX, &CPU::shift_left,  X);    break;
            case LSR_IMP : shift_rot(IMP, &CPU::shift_right);       break;
            case LSR_ZPG : shift_rot(ZPG, &CPU::shift_right);       break;
            case LSR_ZPX : shift_rot(ZPX, &CPU::shift_right, X);    break;
            case LSR_ABS : shift_rot(ABS, &CPU::shift_right);       break;
            case LSR_ABX : shift_rot(ABX, &CPU::shift_right, X);    break;
            case ROL_IMP : shift_rot(IMP, &CPU::rotate_left);       break;
            case ROL_ZPG : shift_rot(ZPG, &CPU::rotate_left);       break;
            case ROL_ZPX : shift_rot(ZPX, &CPU::rotate_left, X);    break;
            case ROL_ABS : shift_rot(ABS, &CPU::rotate_left);       break;
            case ROL_ABX : shift_rot(ABX, &CPU::rotate_left, X);    break;
            case ROR_IMP : shift_rot(IMP, &CPU::rotate_right);      break;
            case ROR_ZPG : shift_rot(ZPG, &CPU::rotate_right);      break;
            case ROR_ZPX : shift_rot(ZPX, &CPU::rotate_right, X);   break;
            case ROR_ABS : shift_rot(ABS, &CPU::rotate_right);      break;
            case ROR_ABX : shift_rot(ABX, &CPU::rotate_right, X);   break;

            case ADC_IMM : /* TODO */ break;
            case ADC_ZPG : /* TODO */ break;
            case ADC_ZPX : /* TODO */ break;
            case ADC_ABS : /* TODO */ break;
            case ADC_ABX : /* TODO */ break;
            case ADC_ABY : /* TODO */ break;
            case ADC_IDX : /* TODO */ break;
            case ADC_IDY : /* TODO */ break;
            case SBC_IMM : /* TODO */ break;
            case SBC_ZPG : /* TODO */ break;
            case SBC_ZPX : /* TODO */ break;
            case SBC_ABS : /* TODO */ break;
            case SBC_ABX : /* TODO */ break;
            case SBC_ABY : /* TODO */ break;
            case SBC_IDX : /* TODO */ break;
            case SBC_IDY : /* TODO */ break;
            
            // Invalid instruction
            default: {
                printf("BAD INSTRUCTION!!!!!!!!!!!!!!\n");
                reset();
                return -1;
            }
        }
        numCycles -= NUM_CYCLES_BASE[currentInstr];
        PC += INSTR_BYTES[currentInstr];    
    }
    return numCyclesSave - numCycles;
}
