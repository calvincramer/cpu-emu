#pragma once

#include <cstdint>

typedef bool			u1;
typedef std::uint8_t	u8;
typedef std::uint16_t	u16;
typedef std::uint32_t 	u32;

namespace mos6502 {
	// P register (flags)
	struct reg_flags {
		u1 C : 1;	// carry
		u1 Z : 1;	// zero
		u1 I : 1;	// interrupt
		u1 D : 1;	// decimal
		u1 B : 1;	// break
		u1 _ : 1;	// not used / ignored
		u1 V : 1;	// overflow
		u1 N : 1;	// negative
	};

	// Registers
	struct reg_set {
		u16 PC;	// program counter
		u8  A;	// accumulator register (AC?)
		u8  X; 	// index register
		u8  Y;	// index register
		reg_flags flags;	// flags (8 bits)
		u8  S;	// stack pointer (SP?)
	};

	class CPU {
	 public:
	 	// Constants
	 	static const u32 MEM_MAX = 1 << 16;

	 	// Internal state
	 	u8 ram[MEM_MAX];	// 64 KiB random access memory (max addressable memory) 
	 	reg_set regs;			// All registers

	 	// Methods
	 	void reset();
	 	u32 execute(u32 numInstructions);

	 	// Helper functions
	 	inline u8
	};
};
