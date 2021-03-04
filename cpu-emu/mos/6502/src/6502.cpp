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
u32 mos6502::CPU::execute(u32 numCycles) {
    // TODO make sure correct number of bytes for each instructions stepped over. Instructions have different sizes.
    u32 numCyclesSave = numCycles;
    u8 currentInstr;

    while (numCycles > 0) {
        currentInstr = getCurrentInstr();
        // Execute instruction
        switch (currentInstr) {
            case LDA_IMM : load_imm(A);                             break;
            case LDA_ZPG : load_zp(A);                              break;
            case LDA_ZPX : load_zp(A, X);                           break;
            case LDA_ABS : load_abs(A, numCycles);                  break;
            case LDA_ABX : load_abs(A, numCycles, X);               break;
            case LDA_ABY : load_abs(A, numCycles, Y);               break;
            case LDA_IDX : load_idx();                              break;
            case LDA_IDY : load_idy(numCycles);                     break;
            case LDX_IMM : load_imm(X);                             break;
            case LDX_ZPG : load_zp(X);                              break;
            case LDX_ZPY : load_zp(X, Y);                           break;
            case LDX_ABS : load_abs(X, numCycles);                  break;
            case LDX_ABY : load_abs(X, numCycles, Y);               break;
            case LDY_IMM : load_imm(Y);                             break;
            case LDY_ZPG : load_zp(Y);                              break;
            case LDY_ZPX : load_zp(Y, X);                           break;
            case LDY_ABS : load_abs(Y, numCycles);                  break;
            case LDY_ABX : load_abs(Y, numCycles, X);               break;
            case STA_ZPG : store_zp(A);                             break;
            case STA_ZPX : store_zp(A, X);                          break;
            case STA_ABS : store_abs(A);                            break;
            case STA_ABX : store_abs(A, X);                         break;
            case STA_ABY : store_abs(A, Y);                         break;
            case STA_IDX : ram[get_indexed_indirect_addr()] = A;    break;
            case STA_IDY : ram[get_indirect_indexed_addr()] = A;    break;
            case STX_ZPG : store_zp(X);                             break;
            case STX_ZPY : store_zp(X, Y);                          break;
            case STX_ABS : store_abs(X);                            break;
            case STY_ZPG : store_zp(Y);                             break;
            case STY_ZPX : store_zp(Y, X);                          break;
            case STY_ABS : store_abs(Y);                            break;
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
            case INC_ZPG : inc_dec_zp(1);                           break;
            case INC_ZPX : inc_dec_zp(1, X);                        break;
            case INC_ABS : inc_dec_abs(1);                          break;
            case INC_ABX : inc_dec_abs(1, X);                       break;
            case INX_IMP : add_to_reg(X, 1);                        break;
            case INY_IMP : add_to_reg(Y, 1);                        break;
            case DEC_ZPG : inc_dec_zp(-1);                          break;
            case DEC_ZPX : inc_dec_zp(-1, X);                       break;
            case DEC_ABS : inc_dec_abs(-1);                         break;
            case DEC_ABX : inc_dec_abs(-1, X);                      break;
            case DEX_IMP : add_to_reg(X, -1);                       break;
            case DEY_IMP : add_to_reg(Y, -1);                       break;
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


#if 0
int main() {
    using namespace mos6502;
    CPU cpu;
    printf("Hello world\n");
    return 0;
}
#endif 
