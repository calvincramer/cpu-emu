#pragma once

#include <cstdint>

// typedefs 
typedef bool            u1;
typedef std::uint8_t    u8;
typedef std::uint16_t   u16;
typedef std::uint32_t   u32;

// Bytes to Wyde
inline u16 B2W(u8 low, u8 high) { return (high << 8) + low; }

inline u8 lowByte(u16 x)  { return x & 0x0F; }
inline u8 highByte(u16 x) { return x & 0xF0; }

namespace mos6502 {
    constexpr u16 RESET_LOC = 0xFFFD;   // Reset vector location

    // P register (flags)
    struct reg_flags {
        u1 C : 1;   // carry
        u1 Z : 1;   // zero
        u1 I : 1;   // interrupt
        u1 D : 1;   // decimal
        u1 B : 1;   // break
        u1 _ : 1;   // not used / ignored
        u1 V : 1;   // overflow
        u1 N : 1;   // negative
    };

    class CPU {
     private:
        u8 getCurrentInstr() { return this->ram[this->PC]; }
     public:
        // Constants
        static const u32 MEM_MAX = 1 << 16;

        // Internal state
        u8 ram[MEM_MAX];    // 64 KiB random access memory (max addressable memory) 
        // All registers
        u16 PC;         // program counter
        u8  A;          // accumulator register (aka 'A')
        u8  X;          // index register
        u8  Y;          // index register
        reg_flags SR;   // status register (flags) (8 bits)
        u8  SP;          // stack pointer (aka 'S')

        // Methods
        void reset();
        u32 execute(u32 numCycles);
        u8& operator[] (u16 i) { return this->ram[i]; }
    };

    enum Instructions : u8 {
        LDA_IMM = 0xA2,     // Load immediate into A
        LDA_ZPG = 0xA5,     // Load from zero-page addr into A
        LDA_ZPX = 0xB5,     // Load from (zero-page addr + X) into A
        LDA_ABS = 0xAD,     // Load from abs 16 bit addr into A
        LDA_ABX = 0xBD,     // Load from abs 16 bit addr plus X into A
        LDA_ABY = 0xB9,     // Load from abs 16 bit addr plus Y into A
        LDA_IDX = 0xA1,     // Load from 16 bit addr starting at low byte (zero-page addr + X)
        LDA_IDY = 0xB1,     // Load from 16 bit addr Y + (2-byte addr starting at operand)
    };
}
