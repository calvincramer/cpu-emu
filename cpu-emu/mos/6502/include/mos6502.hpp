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

inline u8 lowByte(u16 x)  { return x &  0x0F; }
inline u8 highByte(u16 x) { return x >> 8; }
inline u1 signBit(u8 x)   { return x >> 7; }
inline u1 lowBit(u8 x)    { return x &  0x01;}

namespace mos6502 {
    // Constants
    constexpr u16 RESET_LOC = 0xFFFD;   // Reset vector location
    constexpr u32 MEM_MAX = 1 << 16;    // Max amount of memory

    enum AddrMode {
        IMP = 0,    // Register to use is implied in the instruction
        IMM = 1,    // Immediate value in instruction after opcode
        ZPG = 2,    // zero page addressing
        ZPX = 3,    // zero page addressing offset X
        ZPY = 4,    // zero page addressing offset Y
        ABS = 5,    // absolute memory address
        ABX = 6,    // absolute memory address offset X
        ABY = 7,    // absolute memory address offset Y
        IND = 8,    // indirect (only JMP uses)
        IDX = 9,    // indexed indirect addressing offset X
        IDY = 10,   // indirect indexed addressing offset Y
        ACC = 11,   // accumulator register
        REL = 12,   // relative
        NUL = 13,   // placeholder for lookup table no instruction
    };

    // Cpu and memory
    class CPU {
     private:
        inline u8 getCurrentInstr() { return this->ram[this->PC]; }

        inline void set_ZN_flags(u8 val) {
            SR.Z = (val == 0);
            SR.N = ((signBit(val)) != 0);
        }

        // Arithmetic functions
        u8 bitwise_and(u8 op1, u8 op2) { return op1 & op2; }
        u8 bitwise_eor(u8 op1, u8 op2) { return op1 ^ op2; }
        u8 bitwise_or(u8 op1, u8 op2)  { return op1 | op2; }
        u8 add(u8 op1, u8 op2) {
            u8 val = op1 + op2 + SR.C;
            SR.V = (signBit(A) != signBit(val));
            SR.C = SR.V;
            return val;
        }
        u8 sub(u8 op1, u8 op2) {
            u8 val = op1 - op2 - SR.C;
            SR.V = (signBit(A) != signBit(val));
            SR.C = SR.V;
            return val;
        }

        void shift_left(u8& op) {        // Arithmetic shift left by 1
            SR.C = (signBit(op)) != 0;
            op = op << 1;
        }
        void shift_right(u8& op) {       // Logical shift right by 1
            SR.C = lowBit(op);
            op = op >> 1;
        }
        void rotate_left(u8& op) {      // Rotate left by 1
            SR.C = (signBit(op)) != 0;
            op = (op << 1) + SR.C;
        }
        void rotate_right(u8& op) {     // Rotate right by 1
            SR.C = lowBit(op);
            op = (op >> 1) + (SR.C << 7);
        }

        // Address, value, offset
        struct avo {
            u16 addr;
            u8 val;
            u8 offset;
        };

        // Addressing modes (some cover multiple modes with an offset)
        avo addr_mode_get_imp() {
            return { 0xFFFF, 0xFF, 0 };    // Implied register
        }
        avo addr_mode_get_imm() {
            return { 0xFFFF, ram[PC + 1], 0 };
        }
        avo addr_mode_get_zp() {
            u8 addr = (u8) ram[PC+1];
            return { addr, ram[addr], 0 };
        }
        avo addr_mode_get_zpx() {
            u8 addr = (u8) (ram[PC+1] + X);
            return { addr, ram[addr], X };
        }
        avo addr_mode_get_zpy() {
            u8 addr = (u8) (ram[PC+1] + Y);
            return { addr, ram[addr], Y };
        }
        avo addr_mode_get_abs() {
            u16 addr = B2W(ram[PC+1], ram[PC+2]);
            return { addr, ram[addr], 0 };
        }
        avo addr_mode_get_abx() {
            u16 addr = B2W(ram[PC+1], ram[PC+2]) + X;
            return { addr, ram[addr], X };
        }
        avo addr_mode_get_aby() {
            u16 addr = B2W(ram[PC+1], ram[PC+2]) + Y;
            return { addr, ram[addr], Y };
        }
        avo addr_mode_get_indirect() {
            return { 0x0, 0x0, 0x0 };   // TODO
        }
        avo addr_mode_get_indexed_indirect() {
            u16 addr = B2W(ram[(u8) (ram[PC+1] + X)], ram[(u8) (ram[PC+1] + X + 1)]);
            return { addr, ram[addr], X };
        }
        avo addr_mode_get_indirect_indexed() {
            u16 addr = Y + B2W(ram[ram[PC+1]], ram[(u8) (ram[PC+1] + 1)]);
            return { addr, ram[addr], Y };
        }
        avo addr_mode_get_relative() {
            return { 0x0, 0x0, 0x0 };   // TODO
        }
        avo addr_mode_get_null() {
            return { 0x0, 0x0, 0x0 };
        }

        // Must follow order in AddrMode enum
        avo (CPU::*addr_mode_funcs[14])() = {
            &CPU::addr_mode_get_imp,
            &CPU::addr_mode_get_imm,
            &CPU::addr_mode_get_zp,
            &CPU::addr_mode_get_zpx,
            &CPU::addr_mode_get_zpy,
            &CPU::addr_mode_get_abs,
            &CPU::addr_mode_get_abx,
            &CPU::addr_mode_get_aby,
            &CPU::addr_mode_get_indirect,
            &CPU::addr_mode_get_indexed_indirect,
            &CPU::addr_mode_get_indirect_indexed,
            &CPU::addr_mode_get_imp,
            &CPU::addr_mode_get_relative,
            &CPU::addr_mode_get_null,
        };

        inline avo addr_mode_get(AddrMode am) { return (this->*addr_mode_funcs[am])(); }


        inline void load(u8& reg) {
            avo avo_ret = addr_mode_get(am);
            reg = avo_ret.val;
            if (am == ABX || am == ABY || am == IDY) {
                numCycles -= (highByte(avo_ret.addr) != highByte(avo_ret.addr - avo_ret.offset));
            }
            set_ZN_flags(reg);
        }

        inline void store(u8& reg) {
            avo avo_ret = addr_mode_get(am);
            ram[avo_ret.addr] = reg;
        }

        inline void transfer(u8& from, u8& to, bool updateFlags) {
            to = from;
            if (updateFlags) { set_ZN_flags(to); }
        }

        inline void inc_dec(s8 val, u8* impReg = nullptr) {
            if (am == IMP) {
                *impReg += val;
                set_ZN_flags(*impReg);
            } else {
                avo avo_ret = addr_mode_get(am);
                ram[avo_ret.addr] += val;
                set_ZN_flags(ram[avo_ret.addr]);
            }
        }

        // AND, EOR, ORA
        inline void arith(u8 (CPU::*mathOpFunc)(u8, u8)) {
            avo avo_ret = addr_mode_get(am);
            A = (this->*mathOpFunc)(A, avo_ret.val);
            if (am == ABX || am == ABY || am == IDY) {
                numCycles -= (highByte(avo_ret.addr) != highByte(avo_ret.addr - avo_ret.offset));
            }
            set_ZN_flags(A);
        }

        // ASL, LSR, ROL, ROR
        inline void shift_rot(void (CPU::*mathShiftFunc)(u8&)) {
            u8& toShift = (am == ACC) ? A : ram[addr_mode_get(am).addr];
            (this->*mathShiftFunc)(toShift);
            set_ZN_flags(toShift);
        }

        // For execute() function, so we don't need to pass it around so much
        u32 numCycles;
        AddrMode am;

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

        // Methods
        void reset();
        u32 execute(u32 numCycles);
        u8& operator[] (u16 i) { return this->ram[i]; }

        // Helper method for accessing flags
        inline u1 flag_carry()      { return this->SR.C; }
        inline u1 flag_zero()       { return this->SR.Z; }
        inline u1 flag_overflow()   { return this->SR.V; }
        inline u1 flag_negative()   { return this->SR.N; }
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
        EOR_IMM = 0x49, EOR_ZPG = 0x45, EOR_ZPX = 0x55, EOR_ABS = 0x4D, EOR_ABX = 0x5D, EOR_ABY = 0x59, EOR_IDX = 0x41, EOR_IDY = 0x51,
        ORA_IMM = 0x09, ORA_ZPG = 0x05, ORA_ZPX = 0x15, ORA_ABS = 0x0D, ORA_ABX = 0x1D, ORA_ABY = 0x19, ORA_IDX = 0x01, ORA_IDY = 0x11,
        ASL_ACC = 0x0A, ASL_ZPG = 0x06, ASL_ZPX = 0x16, ASL_ABS = 0x0E, ASL_ABX = 0x1E,
        LSR_ACC = 0x4A, LSR_ZPG = 0x46, LSR_ZPX = 0x56, LSR_ABS = 0x4E, LSR_ABX = 0x5E,
        ROL_ACC = 0x2A, ROL_ZPG = 0x26, ROL_ZPX = 0x36, ROL_ABS = 0x2E, ROL_ABX = 0x3E,
        ROR_ACC = 0x6A, ROR_ZPG = 0x66, ROR_ZPX = 0x76, ROR_ABS = 0x6E, ROR_ABX = 0x7E,
        ADC_IMM = 0x69, ADC_ZPG = 0x65, ADC_ZPX = 0x75, ADC_ABS = 0x6D, ADC_ABX = 0x7D, ADC_ABY = 0x79, ADC_IDX = 0x61, ADC_IDY = 0x71,
        SBC_IMM = 0xE9, SBC_ZPG = 0xE5, SBC_ZPX = 0xF5, SBC_ABS = 0xED, SBC_ABX = 0xFD, SBC_ABY = 0xF9, SBC_IDX = 0xE1, SBC_IDY = 0xF1,
    };

    const AddrMode INSTR_GET_ADDR_MODE [256] = {
    // -0                                       -8
        IMP, IDX, NUL, NUL, NUL, ZPG, ZPG, NUL, IMP, IMM, ACC, NUL, NUL, ABS, ABS, NUL,     // 0-
        REL, IDY, NUL, NUL, NUL, ZPX, ZPX, NUL, IMP, ABY, NUL, NUL, NUL, ABX, ABX, NUL,     // 1-
        ABS, IDX, NUL, NUL, ZPG, ZPG, ZPG, NUL, IMP, IMM, ACC, NUL, ABS, ABS, ABS, NUL,     // 2-
        REL, IDY, NUL, NUL, NUL, ZPX, ZPX, NUL, IMP, ABY, NUL, NUL, NUL, ABX, ABX, NUL,     // 3-
        IMP, IDX, NUL, NUL, NUL, ZPG, ZPG, NUL, IMP, IMM, ACC, NUL, ABS, ABS, ABS, NUL,     // 4-
        REL, IDY, NUL, NUL, NUL, ZPX, ZPX, NUL, IMP, ABY, NUL, NUL, NUL, ABX, ABX, NUL,     // 5-
        IMP, IDX, NUL, NUL, NUL, ZPG, ZPG, NUL, IMP, IMM, ACC, NUL, IND, ABS, ABS, NUL,     // 6-
        REL, IDY, NUL, NUL, NUL, ZPX, ZPX, NUL, IMP, ABY, NUL, NUL, NUL, ABX, ABX, NUL,     // 7-
        NUL, IDX, NUL, NUL, ZPG, ZPG, ZPG, NUL, IMP, NUL, IMP, NUL, ABS, ABS, ABS, NUL,     // 8-
        REL, IDY, NUL, NUL, ZPX, ZPX, ZPY, NUL, IMP, ABY, IMP, NUL, NUL, ABX, NUL, NUL,     // 9-
        IMM, IDX, IMM, NUL, ZPG, ZPG, ZPG, NUL, IMP, IMM, IMP, NUL, ABS, ABS, ABS, NUL,     // A-
        REL, IDY, NUL, NUL, ZPX, ZPX, ZPY, NUL, IMP, ABY, IMP, NUL, ABX, ABX, ABY, NUL,     // B-
        IMM, IDX, NUL, NUL, ZPG, ZPG, ZPG, NUL, IMP, IMM, IMP, NUL, ABS, ABS, ABS, NUL,     // C-
        REL, IDY, NUL, NUL, NUL, ZPX, ZPX, NUL, IMP, ABY, NUL, NUL, NUL, ABX, ABX, NUL,     // D-
        IMM, IDX, NUL, NUL, ZPG, ZPG, ZPG, NUL, IMP, IMM, IMP, NUL, ABS, ABS, ABS, NUL,     // E-
        REL, IDY, NUL, NUL, NUL, ZPX, ZPX, NUL, IMP, ABY, NUL, NUL, NUL, ABX, ABX, NUL,     // F-
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
