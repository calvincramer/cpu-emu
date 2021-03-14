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
    SR = FLAG_INIT;
    S = 0xff;   // Stack start at top
}

// Returns -1 on illegal instruction
u32 mos6502::CPU::execute(u32 p_numCycles) {
    u32 numCyclesSave = p_numCycles;  // Original number of cycles to execute
    numCycles = p_numCycles;
    u8 currentInstr;

    while (numCycles > 0) {
        // Get instruction and some common information needed when executing instruction
        currentInstr = getCurrentInstr();
        am = INSTR_GET_ADDR_MODE[currentInstr];
        avo_ret = addr_mode_get(am);

        // Execute instruction
        switch (currentInstr) {
            case LDA_IMM: case LDA_ZPG: case LDA_ZPX: case LDA_ABS: case LDA_ABX: case LDA_ABY: case LDA_IDX: case LDA_IDY :
                load(A);                                                        break;
            case LDX_IMM: case LDX_ZPG: case LDX_ZPY: case LDX_ABS: case LDX_ABY:
                load(X);                                                        break;
            case LDY_IMM: case LDY_ZPG: case LDY_ZPX: case LDY_ABS: case LDY_ABX:
                load(Y);                                                        break;
            case STA_ZPG: case STA_ZPX: case STA_ABS: case STA_ABX: case STA_ABY: case STA_IDX: case STA_IDY:
                store(A);                                                       break;
            case STX_ZPG: case STX_ZPY: case STX_ABS:
                store(X);                                                       break;
            case STY_ZPG: case STY_ZPX: case STY_ABS:
                store(Y);                                                       break;
            case TAX_IMP: transfer(A, X, true);                                 break;
            case TAY_IMP: transfer(A, Y, true);                                 break;
            case TSX_IMP: transfer(S, X, true);                                 break;
            case TXA_IMP: transfer(X, A, true);                                 break;
            case TXS_IMP: transfer(X, S, false);                                break;
            case TYA_IMP: transfer(Y, A, true);                                 break;
            case NOP_IMP: /* do nothing */                                      break;
            case CLC_IMP: set_flag_c(0);                                        break;
            case CLD_IMP: set_flag_d(0);                                        break;
            case CLI_IMP: set_flag_i(0);                                        break;
            case CLV_IMP: set_flag_v(0);                                        break;
            case SEC_IMP: set_flag_c(1);                                        break;
            case SED_IMP: set_flag_d(1);                                        break;
            case SEI_IMP: set_flag_i(1);                                        break;
            case INC_ZPG: case INC_ZPX: case INC_ABS: case INC_ABX:
                inc_dec(1);                                                     break;
            case INX_IMP: inc_dec(1, &X);                                       break;
            case INY_IMP: inc_dec(1, &Y);                                       break;
            case DEC_ZPG: case DEC_ZPX: case DEC_ABS: case DEC_ABX:
                inc_dec(-1);                                                    break;
            case DEX_IMP: inc_dec(-1, &X);                                      break;
            case DEY_IMP: inc_dec(-1, &Y);                                      break;
            case AND_ZPG: case AND_IMM: case AND_ZPX: case AND_ABS: case AND_ABX: case AND_ABY: case AND_IDX: case AND_IDY:
                arith(&CPU::bitwise_and);                                       break;
            case EOR_IMM: case EOR_ZPG: case EOR_ZPX: case EOR_ABS: case EOR_ABX: case EOR_ABY: case EOR_IDX: case EOR_IDY:
                arith(&CPU::bitwise_eor);                                       break;
            case ORA_IMM: case ORA_ZPG: case ORA_ZPX: case ORA_ABS: case ORA_ABX: case ORA_ABY: case ORA_IDX: case ORA_IDY:
                arith(&CPU::bitwise_or);                                        break;
            case ASL_ACC: case ASL_ZPG: case ASL_ZPX: case ASL_ABS: case ASL_ABX:
                shift_rot(&CPU::shift_left);                                    break;
            case LSR_ACC: case LSR_ZPG: case LSR_ZPX: case LSR_ABS: case LSR_ABX:
                shift_rot(&CPU::shift_right);                                   break;
            case ROL_ACC: case ROL_ZPG: case ROL_ZPX: case ROL_ABS: case ROL_ABX:
                shift_rot(&CPU::rotate_left);                                   break;
            case ROR_ACC: case ROR_ZPG: case ROR_ZPX: case ROR_ABS: case ROR_ABX:
                shift_rot(&CPU::rotate_right);                                  break;
            case ADC_IMM: case ADC_ZPG: case ADC_ZPX: case ADC_ABS: case ADC_ABX: case ADC_ABY: case ADC_IDX: case ADC_IDY:
                arith(&CPU::add);                                               break;
            case SBC_IMM: case SBC_ZPG: case SBC_ZPX: case SBC_ABS: case SBC_ABX: case SBC_ABY: case SBC_IDX: case SBC_IDY:
                arith(&CPU::sub);                                               break;
            case BIT_ZPG: case BIT_ABS:
                bit();                                                          break;
            case CMP_IMM: case CMP_ZPG: case CMP_ZPX: case CMP_ABS: case CMP_ABX: case CMP_ABY: case CMP_IDX: case CMP_IDY:
                cmp(A);                                                         break;
            case CPX_IMM: case CPX_ZPG: case CPX_ABS:
                cmp(X);                                                         break;
            case CPY_IMM: case CPY_ZPG: case CPY_ABS:
                cmp(Y);                                                         break;
            case JMP_ABS: case JMP_IND:
                jmp();                                                          break;
            case PHA_IMP: push(A);                                              break;
            case PHP_IMP: push(SR);                                             break;
            case PLA_IMP: A = pull(); set_ZN_flags(A);                          break;
            case PLP_IMP: SR = pull();                                          break;
            case BCC_REL: branch(get_flag_c() == 0);                            break;
            case BCS_REL: branch(get_flag_c() == 1);                            break;
            case BEQ_REL: branch(get_flag_z() == 1);                            break;
            case BMI_REL: branch(get_flag_n() == 1);                            break;
            case BNE_REL: branch(get_flag_z() == 0);                            break;
            case BPL_REL: branch(get_flag_n() == 0);                            break;
            case BVC_REL: branch(get_flag_v() == 0);                            break;
            case BVS_REL: branch(get_flag_v() == 1);                            break;
            case JSR_ABS: jump_sub_routine();                                   break;
            case RTS_IMP: return_sub_routine();                                 break;

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
