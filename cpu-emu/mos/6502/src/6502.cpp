/*
Emulate the MOS 6502 cpu 
*/

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
    SP = 0;
}

// Returns -1 on illegal instruction
u32 mos6502::CPU::execute(u32 numCycles) {
    // TODO make sure correct number of bytes for each instructions stepped over. Instructions have different sizes.
    u32 numCyclesSave = numCycles;
    u8 currentInstr;

    // Helper functions

    auto load_imm = [this](u8& into) { 
        into = ram[PC+1]; 
        set_ZN_flags(into);
    };

    // Will wrap adjusted zero page address around 8 bytes
    auto load_zp =  [this](u8& into, u8 offset = 0) { 
        into = ram[(u8) (ram[PC+1] + offset)];
        set_ZN_flags(into);
    };

    auto load_abs = [this, &numCycles](u8& into, u8 offset = 0) { 
        u16 addr = B2W(ram[PC+1], ram[PC+2]);
        u16 addr_adj = addr + offset;
        into = ram[addr_adj]; 
        numCycles -= (highByte(addr) != highByte(addr_adj));
        set_ZN_flags(into);
    };

    auto store_zp = [this](u8& regToStore, u8 offset = 0) {
        ram[(u8) (ram[PC+1] + offset)] = regToStore;
    };

    auto store_abs = [this](u8& regToStore, u8 offset = 0) { 
        u16 addr = B2W(ram[PC+1], ram[PC+2]);
        u16 addr_adj = addr + offset;
        ram[addr_adj] = regToStore; 
    };

    while (numCycles > 0) {
        currentInstr = getCurrentInstr();
        // Execute instruction
        switch (currentInstr) {
            case LDA_IMM: load_imm(A);      break;
            case LDA_ZPG: load_zp(A);       break;
            case LDA_ZPX: load_zp(A, X);    break;
            case LDA_ABS: load_abs(A);      break;
            case LDA_ABX: load_abs(A, X);   break;
            case LDA_ABY: load_abs (A, Y);  break;
            case LDA_IDX: {
                u8 zpAddr = ram[PC+1];
                zpAddr += X;
                u16 realAddr = B2W(ram[zpAddr], ram[zpAddr+1]);
                A = ram[realAddr];
                set_ZN_flags(A);
            } break;
            case LDA_IDY: {
                u8 zpAddr = ram[PC+1];
                u16 addr = B2W(ram[zpAddr], ram[zpAddr+1]);
                u16 addr_adj = addr + Y;
                A = ram[addr_adj];
                numCycles -= (highByte(addr) != highByte(addr_adj));
                set_ZN_flags(A);
            } break;
            case LDX_IMM : load_imm(X);     break;
            case LDX_ZPG : load_zp(X);      break;
            case LDX_ZPY : load_zp(X, Y);   break;
            case LDX_ABS : load_abs(X);     break;
            case LDX_ABY : load_abs(X, Y);  break;
            case LDY_IMM : load_imm(Y);     break;
            case LDY_ZPG : load_zp(Y);      break;
            case LDY_ZPX : load_zp(Y, X);   break;
            case LDY_ABS : load_abs(Y);     break;
            case LDY_ABX : load_abs(Y, X);  break;
            case STA_ZPG : store_zp(A);     break;
            case STA_ZPX : store_zp(A, X);  break;
            case STA_ABS : store_abs(A);    break;
            case STA_ABX : store_abs(A, X); break;
            case STA_ABY : store_abs(A, Y); break;
            case STA_IDX : {
                u8 zpAddr = ram[PC+1];
                zpAddr += X;
                u16 realAddr = B2W(ram[zpAddr], ram[zpAddr+1]);
                ram[realAddr] = A;
            } break;
            case STA_IDY : {
                u8 zpAddr = ram[PC+1];
                u16 addr = B2W(ram[zpAddr], ram[zpAddr+1]);
                u16 addr_adj = addr + Y;
                ram[addr_adj] = A;
            } break;
            case STX_ZPG : store_zp(X);     break;
            case STX_ZPY : store_zp(X, Y);  break;
            case STX_ABS : store_abs(X);    break;
            case STY_ZPG : store_zp(Y);     break;
            case STY_ZPX : store_zp(Y, X);  break;
            case STY_ABS : store_abs(Y);    break;
            // Invalid instruction
            default: { 
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
