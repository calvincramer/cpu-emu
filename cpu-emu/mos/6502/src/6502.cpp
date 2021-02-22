/*
Emulate the MOS 6502 cpu 
*/

#include <cstdio>
#include "mos6502.hpp"

void mos6502::CPU::reset() {
	// Clear RAM
	for (u32 i = 0; i < MEM_MAX; ++i) 
		this->ram[i] = 0;
	// Reset registers
	this->regs.PC = 0;
	this->regs.A = 0;
	this->regs.X = 0;
	this->regs.Y = 0;
	this->regs.flags.C = 0;
	this->regs.flags.Z = 0;
	this->regs.flags.I = 0;
	this->regs.flags.D = 0;
	this->regs.flags.B = 0;
	this->regs.flags._ = 0;
	this->regs.flags.V = 0;
	this->regs.flags.N = 0;
	this->regs.S = 0;
}

u32 mos6502::CPU::execute(u32 numInstructions) {
	u32 numInstrSave = numInstructions
	while (numInstructions > 0) {
		
		numInstructions -= 1;
	}
	return numInstrSave - numInstructions;
}


#if 0
int main() {
	using namespace mos6502;
	CPU cpu;
	printf("Hello world\n");
	return 0;
}
#endif 
