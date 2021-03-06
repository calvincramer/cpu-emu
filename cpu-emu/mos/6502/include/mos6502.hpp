#pragma once

#include <cstdint>

// typedefs 
typedef bool            u1;
typedef std::uint8_t    u8;
typedef std::uint16_t   u16;
typedef std::uint32_t   u32;

typedef std::int8_t     s8;

// Bytes to Wyde
inline u16 B2W(u8 low, u8 high) { return (high << 8) + low; }

inline u8 lowByte(u16 x)  { return x & 0x0F; }
inline u8 highByte(u16 x) { return x >> 8; }

namespace mos6502 {
    // Constants
    constexpr u16 RESET_LOC = 0xFFFD;   // Reset vector location
    constexpr u32 MEM_MAX = 1 << 16;    // Max amount of memory

    class CPU {
     private:
        u8 getCurrentInstr() { return this->ram[this->PC]; }
        
        inline void set_ZN_flags(u8 val) {
            SR.Z = (val == 0);
            SR.N = ((val & 0x80) != 0);
        }

        struct av_pair { u16 addr; u8 val; };

        // Addressing modes (some cover multiple modes with an offset)
        av_pair addr_mode_get_imp(__attribute__((unused)) u8 _) { 
            return { 0xFFFF, 0xFF };    // Implied register 
        }
        av_pair addr_mode_get_imm(__attribute__((unused)) u8 _) { 
            return { 0xFFFF, ram[PC + 1] }; 
        }
        av_pair addr_mode_get_zp(u8 offset) { 
            u8 addr = (u8) (ram[PC+1] + offset);
            return { addr, ram[addr] }; 
        }
        av_pair addr_mode_get_abs(u8 offset) { 
            u16 addr = B2W(ram[PC+1], ram[PC+2]) + offset;
            return { addr, ram[addr] }; 
        }
        av_pair addr_mode_get_indexed_indirect(u8 offset) { 
            u16 addr = B2W(ram[(u8) (ram[PC+1] + offset)], ram[(u8) (ram[PC+1] + offset + 1)]);
            return { addr, ram[addr] };
        }
        av_pair addr_mode_get_indirect_indexed(u8 offset) { 
            u16 addr = offset + B2W(ram[ram[PC+1]], ram[(u8) (ram[PC+1] + 1)]); 
            return { addr, ram[addr] }; 
        }

        enum AddrMode {
            IMP = 0, IMM = 1, 
            ZPG = 2, ZPX = 3, ZPY = 4, 
            ABS = 5, ABX = 6, ABY = 7, 
            IDX = 8, IDY = 9,
        };

        // Must follow order in AddrMode enum
        av_pair (CPU::*addr_mode_funcs[10])(u8) = {
            &CPU::addr_mode_get_imp,
            &CPU::addr_mode_get_imm,
            &CPU::addr_mode_get_zp,
            &CPU::addr_mode_get_zp,
            &CPU::addr_mode_get_zp,
            &CPU::addr_mode_get_abs,
            &CPU::addr_mode_get_abs,
            &CPU::addr_mode_get_abs,
            &CPU::addr_mode_get_indexed_indirect,
            &CPU::addr_mode_get_indirect_indexed,
        };

        inline av_pair addr_mode_get(AddrMode am, u8 offset = 0) { return (this->*addr_mode_funcs[am])(offset); }


        inline void load(AddrMode am, u8& reg, u8 offset = 0) { 
            av_pair av_p = addr_mode_get(am, offset);
            reg = av_p.val;
            if (am == ABX || am == ABY || am == IDY) {
                numCycles -= (highByte(av_p.addr) != highByte(av_p.addr - offset));
            }
            set_ZN_flags(reg);
        }

        inline void store(AddrMode am, u8& reg, u8 offset = 0) {
            av_pair av_p = addr_mode_get(am, offset);
            ram[av_p.addr] = reg;
        }

        inline void transfer(u8& from, u8& to, bool updateFlags) {
            to = from;
            if (updateFlags) { set_ZN_flags(to); }
        }

        inline void inc_dec(AddrMode am, s8 val, u8 offset = 0, u8* impReg = nullptr) {
            if (am == IMP) {
                *impReg += val;
                set_ZN_flags(*impReg);
            } else {
                av_pair av_p = addr_mode_get(am, offset);
                ram[av_p.addr] += val;
                set_ZN_flags(ram[av_p.addr]);
            }
        }

        inline void _and(AddrMode am, u8 offset = 0) {
            av_pair av_p = addr_mode_get(am, offset);
            A &= av_p.val;
            if (am == ABX || am == ABY || am == IDY) {
                numCycles -= (highByte(av_p.addr) != highByte(av_p.addr - offset));
            }
            set_ZN_flags(A);
        }

     public:
        // Internal state
        u8 ram[MEM_MAX];    // 64 KiB random access memory (max addressable memory) 
        // All registers
        u16 PC;         // program counter
        u8  A;          // accumulator register (aka 'A')
        u8  X;          // index register
        u8  Y;          // index register
        u8  S;          // stack pointer (aka 'SP')
        // Flags
        struct {
            u1 C : 1;       // flag: carry
            u1 Z : 1;       // flag: zero
            u1 I : 1;       // flag: interrupt disable
            u1 D : 1;       // flag: decimal
            u1 B : 1;       // flag: break
            u1 _ : 1;       //       not used / ignored
            u1 V : 1;       // flag: overflow
            u1 N : 1;       // flag: negative
        } SR;           // status register (flags) (8 bits)

        // For execute() function, so we don't need to pass it around so much
        u32 numCycles;

        // Methods
        void reset();
        u32 execute(u32 numCycles);
        u8& operator[] (u16 i) { return this->ram[i]; }
    };

    enum Instructions : u8 {
        LDA_IMM = 0xA9, LDA_ZPG = 0xA5, LDA_ZPX = 0xB5, LDA_ABS = 0xAD, LDA_ABX = 0xBD, LDA_ABY = 0xB9, LDA_IDX = 0xA1, LDA_IDY = 0xB1,
        LDX_IMM = 0xA2, LDX_ZPG = 0xA6, LDX_ZPY = 0xB6, LDX_ABS = 0xAE, LDX_ABY = 0xBE,
        LDY_IMM = 0xA0, LDY_ZPG = 0xA4, LDY_ZPX = 0xB4, LDY_ABS = 0xAC, LDY_ABX = 0xBC,
        STA_ZPG = 0x85, STA_ZPX = 0x95, STA_ABS = 0x8D, STA_ABX = 0x9D, STA_ABY = 0x99, STA_IDX = 0x81, STA_IDY = 0x91,
        STX_ZPG = 0x86, STX_ZPY = 0x96, STX_ABS = 0x8E,
        STY_ZPG = 0x84, STY_ZPX = 0x94, STY_ABS = 0x8C,
        TAX_IMP = 0xAA, TAY_IMP = 0xA8, TSX_IMP = 0xBA, TXA_IMP = 0x8A, TXS_IMP = 0x9A, TYA_IMP = 0x98,
        NOP_IMP = 0xEA,
        CLC_IMP = 0x18, CLD_IMP = 0xD8, CLI_IMP = 0x58, CLV_IMP = 0xB8,
        SEC_IMP = 0x38, SED_IMP = 0xF8, SEI_IMP = 0x78,
        INC_ZPG = 0xE6, INC_ZPX = 0xF6, INC_ABS = 0xEE, INC_ABX = 0xFE, INX_IMP = 0xE8, INY_IMP = 0xC8,
        DEC_ZPG = 0xC6, DEC_ZPX = 0xD6, DEC_ABS = 0xCE, DEC_ABX = 0xDE, DEX_IMP = 0xCA, DEY_IMP = 0x88,
        AND_IMM = 0x29, AND_ZPG = 0x25, AND_ZPX = 0x35, AND_ABS = 0x2D, AND_ABX = 0x3D, AND_ABY = 0x39, AND_IDX = 0x21, AND_IDY = 0x31,
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
