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
    auto set_ZN_flags = [this](u8 regVal) {
        SR.Z = (regVal == 0);
        SR.N = ((regVal & 0x80) != 0);
    };

    while (numCycles > 0) {
        currentInstr = getCurrentInstr();
        switch (currentInstr) {
            // LDA - Load into A register
            case LDA_IMM: {
                A = ram[PC+1];
                numCycles -= 2;
            } break;
            case LDA_ZPG: {
                u8 zpAddr = ram[PC+1];
                A = ram[zpAddr];
                numCycles -= 3;
            } break;
            case LDA_ZPX: {
                u8 zpAddr = ram[PC+1];
                zpAddr += X;
                A = ram[zpAddr];
                numCycles -= 4;
            } break;
            case LDA_ABS: {
                A = ram[B2W(ram[PC+1], ram[PC+2])];
                numCycles -= 4;
            } break;
            case LDA_ABX: {
                u16 addr = B2W(ram[PC+1], ram[PC+2]);
                u16 x_adj_addr = addr + X;
                A = ram[x_adj_addr];
                numCycles -= 4 + (highByte(addr) != highByte(x_adj_addr));
            } break;
            case LDA_ABY: {
                u16 addr = B2W(ram[PC+1], ram[PC+2]);
                u16 y_adj_addr = addr + Y;
                A = ram[y_adj_addr];
                numCycles -= 4 + (highByte(addr) != highByte(y_adj_addr));
            } break;
            case LDA_IDX: {
                u8 zpAddr = ram[PC+1];
                zpAddr += X;
                u16 realAddr = B2W(ram[zpAddr], ram[zpAddr+1]);
                A = ram[realAddr];
                numCycles -= 6;
            } break;
            case LDA_IDY: {
                u8 zpAddr = ram[PC+1];
                u16 addr = B2W(ram[zpAddr], ram[zpAddr+1]);
                u16 addr_adj = addr + Y;
                A = ram[addr_adj];
                numCycles -= 5 + (highByte(addr) != highByte(addr_adj));
            } break;
            // Invalid instruction
            default: { 
                reset();
                return -1;
            }
        }
        set_ZN_flags(A);    // All instructions so far set these flags, will need to move later probably
        PC += 1;
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
