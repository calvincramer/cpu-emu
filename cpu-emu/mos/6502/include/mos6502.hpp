#pragma once

#include <cstdint>

#include "models.hpp"

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

inline u1 onDifferentPages(u16 x, u16 y) { return highByte(x) != highByte(y); }

namespace mos6502 {
    // Constants
    constexpr u32 MEM_MAX       = 1 << 16;  // Max amount of memory
    // Memory layout
    constexpr u16 INT_VEC_LOC           = 0xFFFE;   // Interrupt vector location low byte
    constexpr u16 RESET_VEC_LOC         = 0xFFFC;   // Reset vector location low byte
    constexpr u16 NON_MASK_INT_VEC_LOC  = 0xFFFA;   // Non-maskable vector location low byte
    constexpr u16 STACK_START           = 0x01FF;   // Stack start (inclusive)
    constexpr u16 STACK_END             = 0x0100;   // Stack end (inclusive)
    // Temp
    constexpr u16 RESET_START           = 0x4000;   // Temporary
    // Flags
    constexpr u8 FLAG_MASK_C = 0b00000001;
    constexpr u8 FLAG_MASK_Z = 0b00000010;
    constexpr u8 FLAG_MASK_I = 0b00000100;
    constexpr u8 FLAG_MASK_D = 0b00001000;
    constexpr u8 FLAG_MASK_B = 0b00010000;
    constexpr u8 FLAG_MASK_NOT_USED = 0b00100000;
    constexpr u8 FLAG_MASK_V = 0b01000000;
    constexpr u8 FLAG_MASK_N = 0b10000000;

    constexpr u8 FLAG_INIT = FLAG_MASK_NOT_USED;

    enum AddrMode {
        IMP = 0,    // Register to use is implied in the instruction
        IMM = 1,    // Immediate value in instruction after opcode
        ZPG = 2,    // zero page addressing
        ZPX = 3,    // zero page addressing offset X
        ZPY = 4,    // zero page addressing offset Y
        ABS = 5,    // absolute memory address
        ABX = 6,    // absolute memory address offset X
        ABY = 7,    // absolute memory address offset Y
        IND = 8,    // indirect (only JMP uses) (aka absolute indirect)
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
            set_flag_z(val == 0);
            set_flag_n((signBit(val)) != 0);
        }

        // Arithmetic functions
        u8 bitwise_and(u8 op1, u8 op2) { return op1 & op2; }
        u8 bitwise_eor(u8 op1, u8 op2) { return op1 ^ op2; }
        u8 bitwise_or(u8 op1, u8 op2)  { return op1 | op2; }
        u8 add(u8 op1, u8 op2) {
            u8 val = op1 + op2 + get_flag_c();
            set_flag_v(signBit(A) != signBit(val));
            set_flag_c(get_flag_v());
            return val;
        }
        u8 sub(u8 op1, u8 op2) {
            u8 val = op1 - op2 - get_flag_c();
            set_flag_v(signBit(A) != signBit(val));
            set_flag_c( !get_flag_v() );   // Cleared if overflow
            return val;
        }

        void shift_left(u8& op) {        // Arithmetic shift left by 1
            set_flag_c((signBit(op)) != 0);
            op = op << 1;
        }
        void shift_right(u8& op) {       // Logical shift right by 1
            set_flag_c(lowBit(op));
            op = op >> 1;
        }
        void rotate_left(u8& op) {      // Rotate left by 1
            set_flag_c((signBit(op)) != 0);
            op = (op << 1) + get_flag_c();
        }
        void rotate_right(u8& op) {     // Rotate right by 1
            set_flag_c(lowBit(op));
            op = (op >> 1) + (get_flag_c() << 7);
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
            return { 0xFFFF, ram[PC+1], 0 };
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
            u8 low = ram[PC+1];
            u8 high = ram[PC+2];
            u16 deref_low = ram[B2W(low, high)];
#if CPU_MODEL==MODEL_6502
            /* Original 6502 handled xxFF boundary incorrectly (wraps without updating high byte) */
#else
            /* Later models like 65SC02 fixed this */
            high += (low == 0xFF);
#endif
            low = (u8) (low + 1);
            u16 deref_high = ram[B2W(low, high)];
            return { B2W(deref_low, deref_high), 0, 0 };  // Don't need to read addr in memory, just need addr
        }
        avo addr_mode_get_indexed_indirect() {
            u16 addr = B2W(ram[(u8) (ram[PC+1] + X)], ram[(u8) (ram[PC+1] + X + 1)]);
            return { addr, ram[addr], X };
        }
        avo addr_mode_get_indirect_indexed() {
            u16 addr = Y + B2W(ram[ram[PC+1]], ram[(u8) (ram[PC+1] + 1)]);
            return { addr, ram[addr], Y };
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
            &CPU::addr_mode_get_imm,    // Relative just get immediate value of next byte
            &CPU::addr_mode_get_null,
        };

        inline avo addr_mode_get(AddrMode am) { return (this->*addr_mode_funcs[am])(); }


        inline void load(u8& reg) {
            reg = avo_ret.val;
            if (am == ABX || am == ABY || am == IDY) {
                numCycles -= onDifferentPages(avo_ret.addr, avo_ret.addr - avo_ret.offset);
            }
            set_ZN_flags(reg);
        }

        inline void store(u8& reg) {
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
                ram[avo_ret.addr] += val;
                set_ZN_flags(ram[avo_ret.addr]);
            }
        }

        // AND, EOR, ORA, ADC, SBC
        inline void arith(u8 (CPU::*mathOpFunc)(u8, u8)) {
            A = (this->*mathOpFunc)(A, avo_ret.val);
            if (am == ABX || am == ABY || am == IDY) {
                numCycles -= onDifferentPages(avo_ret.addr, avo_ret.addr - avo_ret.offset);
            }
            set_ZN_flags(A);
        }

        // ASL, LSR, ROL, ROR
        inline void shift_rot(void (CPU::*mathShiftFunc)(u8&)) {
            u8& toShift = (am == ACC) ? A : ram[avo_ret.addr];
            (this->*mathShiftFunc)(toShift);
            set_ZN_flags(toShift);
        }

        inline void bit() {
            u8 and_res = avo_ret.val & A;
            set_flag_z(and_res == 0);
            set_flag_v((and_res & 0b01000000) != 0);
            set_flag_n((and_res & 0b10000000) != 0);
        }

        // CMP, CPX, CPY
        inline void cmp(u8& reg) {
            set_flag_c(reg >= avo_ret.val);
            set_flag_z(reg == avo_ret.val);
            set_flag_n(signBit(reg - avo_ret.val));
        }

        inline void jmp() {
            PC = avo_ret.addr;
        }

        // Push onto stack
        inline void push(u8 val) {
            ram[S + 0x0100] = val;
            S -= 1;
        }

        // Pull from stack
        inline u8 pull() {
            S += 1;
            return ram[S + 0x0100];
            // Need to clear stuff in stack?
        }

        inline void branch(u1 branchCondResult) {
            if (!branchCondResult) {
                PC += 2;
                return;
            }
            u16 newPC = PC + ((s8) avo_ret.val);    // address is signed
            numCycles -= 1 + (2 * (u32) onDifferentPages(PC, newPC));
            PC = newPC;
        }

        inline void jump_sub_routine() {
            u16 addrOnStack = PC + 2;
            push(highByte(addrOnStack));
            push(lowByte(addrOnStack));
            PC = avo_ret.addr;
        }

        inline void return_sub_routine() {
            u8 low = pull();
            u8 high = pull();
            PC = B2W(low, high) + 1;
        }

        inline void generate_interrupt() {
            // PC and flag register on stack
            push(highByte(PC)); // Proper byte order?
            push(lowByte(PC));
            push(SR);

            // Jump to interrupt vector location
            PC = B2W(ram[INT_VEC_LOC], ram[INT_VEC_LOC + 1]);

            // break flag high
            set_flag_b(1);
        }

        inline void return_from_interrupt() {
            // Load flags and PC from stack
            SR = pull();
            u8 pcLow = pull();
            u8 pcHigh = pull();
            PC = B2W(pcLow, pcHigh);
        }

        // For execute() function, so we don't need to pass it around so much
        u32 numCycles;  // Number of cycles left to execute
        AddrMode am;    // Address mode of current instruction
        avo avo_ret;    // address, val of addr, offset from current address mode

     public:
        // Internal state
        u8 ram[MEM_MAX];    // 64 KiB random access memory (max addressable memory)
        // All registers
        u16 PC;         // program counter
        u8  A;          // accumulator register (aka 'A')
        u8  X;          // index register
        u8  Y;          // index register
        u8  S;          // stack pointer (aka 'SP')
        u8  SR;         // status register (flags) (8 bits)
                        //  bit 0: C: carry
                        //  bit 1: Z: zero
                        //  bit 2: I: interrupt disable
                        //  bit 3: D: decimal
                        //  bit 4: B: break
                        //  bit 5:    not used / ignored (set high)
                        //  bit 6: V: overflow
                        //  bit 7: N: negative

        // Methods
        void reset();
        u32 execute(u32 numCycles);
        u8& operator[] (u16 i) { return this->ram[i]; }

        // Helper method for accessing flags
        inline u1 get_flag_c()   { return (this->SR & FLAG_MASK_C) != 0; }
        inline u1 get_flag_z()   { return (this->SR & FLAG_MASK_Z) != 0; }
        inline u1 get_flag_i()   { return (this->SR & FLAG_MASK_I) != 0; }
        inline u1 get_flag_d()   { return (this->SR & FLAG_MASK_D) != 0; }
        inline u1 get_flag_b()   { return (this->SR & FLAG_MASK_B) != 0; }
        inline u1 get_flag_v()   { return (this->SR & FLAG_MASK_V) != 0; }
        inline u1 get_flag_n()   { return (this->SR & FLAG_MASK_N) != 0; }

        inline void set_flag_c(u1 val)  { this->SR = (this->SR & ~FLAG_MASK_C) + (val * FLAG_MASK_C); }
        inline void set_flag_z(u1 val)  { this->SR = (this->SR & ~FLAG_MASK_Z) + (val * FLAG_MASK_Z); }
        inline void set_flag_i(u1 val)  { this->SR = (this->SR & ~FLAG_MASK_I) + (val * FLAG_MASK_I); }
        inline void set_flag_d(u1 val)  { this->SR = (this->SR & ~FLAG_MASK_D) + (val * FLAG_MASK_D); }
        inline void set_flag_b(u1 val)  { this->SR = (this->SR & ~FLAG_MASK_B) + (val * FLAG_MASK_B); }
        inline void set_flag_v(u1 val)  { this->SR = (this->SR & ~FLAG_MASK_V) + (val * FLAG_MASK_V); }
        inline void set_flag_n(u1 val)  { this->SR = (this->SR & ~FLAG_MASK_N) + (val * FLAG_MASK_N); }
    };

    enum Instructions : u8 {
        LDA_IMM = 0xA9, LDA_ZPG = 0xA5, LDA_ZPX = 0xB5, LDA_ABS = 0xAD, LDA_ABX = 0xBD, LDA_ABY = 0xB9, LDA_IDX = 0xA1, LDA_IDY = 0xB1,
        LDX_IMM = 0xA2, LDX_ZPG = 0xA6, LDX_ZPY = 0xB6, LDX_ABS = 0xAE, LDX_ABY = 0xBE,
        LDY_IMM = 0xA0, LDY_ZPG = 0xA4, LDY_ZPX = 0xB4, LDY_ABS = 0xAC, LDY_ABX = 0xBC,
        STA_ZPG = 0x85, STA_ZPX = 0x95, STA_ABS = 0x8D, STA_ABX = 0x9D, STA_ABY = 0x99, STA_IDX = 0x81, STA_IDY = 0x91,
        STX_ZPG = 0x86, STX_ZPY = 0x96, STX_ABS = 0x8E,
        STY_ZPG = 0x84, STY_ZPX = 0x94, STY_ABS = 0x8C,
        TAX_IMP = 0xAA, TAY_IMP = 0xA8, TSX_IMP = 0xBA, TXA_IMP = 0x8A, TXS_IMP = 0x9A, TYA_IMP = 0x98,
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
        BIT_ZPG = 0x24, BIT_ABS = 0x2C,
        CMP_IMM = 0xC9, CMP_ZPG = 0xC5, CMP_ZPX = 0xD5, CMP_ABS = 0xCD, CMP_ABX = 0xDD, CMP_ABY = 0xD9, CMP_IDX = 0xC1, CMP_IDY = 0xD1,
        CPX_IMM = 0xE0, CPX_ZPG = 0xE4, CPX_ABS = 0xEC,
        CPY_IMM = 0xC0, CPY_ZPG = 0xC4, CPY_ABS = 0xCC,
        PHA_IMP = 0x48, PHP_IMP = 0x08,
        PLA_IMP = 0x68, PLP_IMP = 0x28,
        BCC_REL = 0x90, BCS_REL = 0xB0, BEQ_REL = 0xF0, BMI_REL = 0x30, BNE_REL = 0xD0, BPL_REL = 0x10, BVC_REL = 0x50, BVS_REL = 0x70,
        JMP_ABS = 0x4C, JMP_IND = 0x6C,
        JSR_ABS = 0x20, RTS_IMP = 0x60,
        BRK_IMP = 0x00, RTI_IMP = 0x40,
        NOP_IMP = 0xEA,
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

    /*
     * Number of bytes for each instruction, used to update PC
     * JMP, BRANCH, SUBROUTINE, BRK, RTI instructions set to 0 bytes, to change PC manually
     */
    const u8 INSTR_BYTES [256] = {
    // -0                      -8
        0, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 0, 3, 3, 0,    // 0-
        0, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // 1-
        0, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,    // 2-
        0, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // 3-
        0, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 0, 3, 3, 0,    // 4-
        0, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // 5-
        0, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 0, 3, 3, 0,    // 6-
        0, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // 7-
        0, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0,    // 8-
        0, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0,    // 9-
        2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,    // A-
        0, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,    // B-
        2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,    // C-
        0, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // D-
        2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,    // E-
        0, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,    // F-
    };
}
