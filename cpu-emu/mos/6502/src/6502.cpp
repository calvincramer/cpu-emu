/*
Emulate the MOS 6502 cpu 
*/

#include <cstdio>
#include <assert.h>
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
	SR._ = 1;	// Apparantly always high
	SR.V = 0;
	SR.N = 0;
	SP = 0;
}

u32 mos6502::CPU::execute(u32 numCycles) {
	u32 numCyclesSave = numCycles;
	u8 currentInstr;
	while (numCycles > 0) {
		currentInstr = getCurrentInstr();
		switch (currentInstr) {
			case LDA_IMM: {
				u8 load = ram[PC+1];
				SR.Z = (load == 0);
				SR.N = ( (load & 0x80) != 0);
				A = load;
				numCycles -= 2;
			} break;
			case LDA_ZPG: {
				u8 zpAddr = ram[PC+1];
				u8 load = ram[zpAddr];
				SR.Z = (load == 0);
				SR.N = ( (load & 0x80) != 0);
				A = load;
				numCycles -= 3;
			} break;
			default: { 
				printf("Bad instruction\n"); 
				assert(false); 
			}
		}
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
