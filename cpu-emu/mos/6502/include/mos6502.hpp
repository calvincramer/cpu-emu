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
inline u8 highByte(u16 x) { return x >> 8; }

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
        inline void set_ZN_flags(u8& reg) {
            SR.Z = (reg == 0);
            SR.N = ((reg & 0x80) != 0);
        }
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
        // Load from mem into A
        LDA_IMM = 0xA9,
        LDA_ZPG = 0xA5,
        LDA_ZPX = 0xB5,
        LDA_ABS = 0xAD,
        LDA_ABX = 0xBD,
        LDA_ABY = 0xB9,
        LDA_IDX = 0xA1,
        LDA_IDY = 0xB1,
        // Load from mem into X
        LDX_IMM = 0xA2,
        LDX_ZPG = 0xA6,
        LDX_ZPY = 0xB6,
        LDX_ABS = 0xAE,
        LDX_ABY = 0xBE,
        // Load from mem into Y
        LDY_IMM = 0xA0,
        LDY_ZPG = 0xA4,
        LDY_ZPX = 0xB4,
        LDY_ABS = 0xAC,
        LDY_ABX = 0xBC,
        // Store A to mem 
        STA_ZPG = 0x85,
        STA_ZPX = 0x95,
        STA_ABS = 0x8D,
        STA_ABX = 0x9D,
        STA_ABY = 0x99,
        STA_IDX = 0x81,
        STA_IDY = 0x91,
        // Store X to mem 
        STX_ZPG = 0x86,
        STX_ZPY = 0x96,
        STX_ABS = 0x8E,
        // Store Y to mem 
        STY_ZPG = 0x84,
        STY_ZPX = 0x94,
        STY_ABS = 0x8C,
    };

    // Base number of cycles used per instruction, actual may be more on certain circumstances
    const u8 NUM_CYCLES_BASE [256] = {
    // -0                      -8
        7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,     // 0-
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,     // 1-
        6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0,     // 2-
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,     // 3-
        6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 6, 0,     // 4-
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,     // 5-
        6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,     // 6-
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,     // 7-
        0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,     // 8-
        2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,     // 9-
        2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,     // A-
        2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,     // B-
        2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,     // C-
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,     // D-
        2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,     // E-
        2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,     // F-
    };

    // Number of bytes for each instruction
    const u8 INSTR_BYTES [256] = {
    // -0                      -8
        1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 0, 3, 3, 0,    // 0-
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // 1-
        3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,    // 2-
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // 3-
        1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,    // 4-
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // 5-
        1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,    // 6-
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // 7-
        0, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0,    // 8-
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0,    // 9-
        2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,    // A-
        2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,    // B-
        2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,    // C-
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // D-
        2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,    // E-
        2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // F-
    };
}
