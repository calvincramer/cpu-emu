#include <gtest/gtest.h>
#include "mos6502.hpp"

using namespace mos6502;

// Values for flags
#define F_ZERO 1
#define F_NON_ZERO 0

#define F_NEG 1
#define F_NON_NEG 0

class SetupCPU_F : public ::testing::Test {
 public:
    CPU cpu;
    CPU cpuOrig;

    SetupCPU_F()            {} 
    ~SetupCPU_F()           {}
    virtual void SetUp()    { 
        cpu.reset();
        cpuOrig = cpu;
    }
    virtual void TearDown() {}
};

// Derive above fixture to categorize test cases in groups
class Api           : public SetupCPU_F {};
class LDA           : public SetupCPU_F {};
class LDX           : public SetupCPU_F {};
class LDY           : public SetupCPU_F {};
class STA           : public SetupCPU_F {};
class STX           : public SetupCPU_F {};
class STY           : public SetupCPU_F {};
class TRANSFER      : public SetupCPU_F {};
class NOP           : public SetupCPU_F {};
class CLEAR_FLAGS   : public SetupCPU_F {};
class SET_FLAGS     : public SetupCPU_F {};
class INC_DEC       : public SetupCPU_F {};
class AND           : public SetupCPU_F {};

TEST_F(Api, Reset) { cpu.reset(); }
TEST_F(Api, Execute) { cpu.execute(0); }

auto common_load_execute = [] (CPU& cpu, u32 runCycles, u32 expCycles, u8& expReg, u8 expVal, u1 expZ, u1 expN) {
    ASSERT_TRUE(cpu.execute(runCycles) == expCycles);
    ASSERT_TRUE(expReg == expVal);
    ASSERT_TRUE(cpu.SR.Z == expZ);
    ASSERT_TRUE(cpu.SR.N == expN);
};

// Common load immediate instructions
void load_immediate(CPU& cpu, u8 loadInst, u8& reg) {
    // Zero
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x0;
    common_load_execute(cpu, 2, 2, reg, 0, F_ZERO, F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x42;
    common_load_execute(cpu, 2, 2, reg, 0x42, F_NON_ZERO, F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0xFF;
    common_load_execute(cpu, 2, 2, reg, 0xFF, F_NON_ZERO, F_NEG);
}

// Common load from zero page instructions
void load_zero_page(CPU& cpu, u8 loadInst, u8& loadToReg, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // Zero
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x50 + offsetVal] = 0x0;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x0, F_ZERO, F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x50 + offsetVal] = 0x42;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x50; 
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x50 + offsetVal] = 0xFF;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0xFF, F_NON_ZERO, F_NEG);

    // Wrap (if there is an offset)
    if (offsetReg != nullptr) {
        cpu.reset();
        cpu[RESET_LOC] = loadInst;
        cpu[RESET_LOC + 1] = 0xF0;
        *offsetReg = offsetVal;
        cpu[0xF0 + offsetVal] = 0x42;
        common_load_execute(cpu, cycles, cycles, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);
    }
}

// Common load absolute instructions
void load_absolute(CPU& cpu, u8 loadInst, u8& loadToReg, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // Addr is 0x4321
    // Zero
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; 
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x4321 + offsetVal] = 0x0;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x0, F_ZERO, F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x4321 + offsetVal] = 0x42;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x4321 + offsetVal] = 0xFF;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0xFF, F_NON_ZERO, F_NEG);

    // Page cross (+1 cycles taken)
    if (offsetReg != nullptr) {
        offsetVal = 0xFF; // Force page cross
        cpu.reset();
        cpu[RESET_LOC] = loadInst;
        cpu[RESET_LOC + 1] = 0x21;
        cpu[RESET_LOC + 2] = 0x43;
        *offsetReg = offsetVal;
        cpu[0x4321 + offsetVal] = 0x42;
        common_load_execute(cpu, cycles + 1, cycles + 1, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);
    }
}

// Common load indexed indirect instructions
void load_indexed_indirect(CPU& cpu, u8 loadInst, u8& loadToReg, u8 cycles) {
    // Zero
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0x0;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x0, F_ZERO, F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0x42;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0xFF;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0xFF, F_NON_ZERO, F_NEG);

    // Wrap zero-page
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x30;
    cpu.X = 0xF0;
    cpu[0x0020] = 0x34;
    cpu[0x0021] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0x31;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x31, F_NON_ZERO, F_NON_NEG);
}

void load_indirect_indexed(CPU& cpu, u8 loadInst, u8& loadToReg, u8 cycles) {
    // Zero
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should load from 0x1234 + 0x5
    cpu[0x1239] = 0x0;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x0, F_ZERO, F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should load from 0x1234 + 0x5
    cpu[0x1239] = 0x42;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should load from 0x1234 + 0x5
    cpu[0x1239] = 0xFF;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0xFF, F_NON_ZERO, F_NEG);

    // Page cross
    cpu.reset();
    cpu[RESET_LOC] = loadInst;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0xFF;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should load from 0x1234 + 0xFF
    cpu[0x1333] = 0xee;
    common_load_execute(cpu, cycles + 1, cycles + 1, loadToReg, 0xee, F_NON_ZERO, F_NEG);
}

// LDA
TEST_F(LDA, Immediate)       { load_immediate(cpu, LDA_IMM, cpu.A); }
TEST_F(LDA, ZeroPage)        { load_zero_page(cpu, LDA_ZPG, cpu.A, 3); }
TEST_F(LDA, ZeroPageX)       { load_zero_page(cpu, LDA_ZPX, cpu.A, 4, &cpu.X, 0xA); }
TEST_F(LDA, Absolute)        { load_absolute (cpu, LDA_ABS, cpu.A, 4); }
TEST_F(LDA, AbsoluteX)       { load_absolute (cpu, LDA_ABX, cpu.A, 4, &cpu.X, 0x5); }
TEST_F(LDA, AbsoluteY)       { load_absolute (cpu, LDA_ABY, cpu.A, 4, &cpu.Y, 0x5); }
TEST_F(LDA, IndexedIndirect) { load_indexed_indirect(cpu, LDA_IDX, cpu.A, 6); }
TEST_F(LDA, IndirectIndexed) { load_indirect_indexed(cpu, LDA_IDY, cpu.A, 5); }

// LDX
TEST_F(LDX, Immediate) { load_immediate(cpu, LDX_IMM, cpu.X); }
TEST_F(LDX, ZeroPage)  { load_zero_page(cpu, LDX_ZPG, cpu.X, 3); }
TEST_F(LDX, ZeroPageY) { load_zero_page(cpu, LDX_ZPY, cpu.X, 4, &cpu.Y, 0xA); }
TEST_F(LDX, Absolute)  { load_absolute (cpu, LDX_ABS, cpu.X, 4); }
TEST_F(LDX, AbsoluteY) { load_absolute (cpu, LDX_ABY, cpu.X, 4, &cpu.Y, 0x5); }

// LDY
TEST_F(LDY, Immediate) { load_immediate(cpu, LDY_IMM, cpu.Y); }
TEST_F(LDY, ZeroPage)  { load_zero_page(cpu, LDY_ZPG, cpu.Y, 3); }
TEST_F(LDY, ZeroPageX) { load_zero_page(cpu, LDY_ZPX, cpu.Y, 4, &cpu.X, 0xA); }
TEST_F(LDY, Absolute)  { load_absolute (cpu, LDY_ABS, cpu.Y, 4); }
TEST_F(LDY, AbsoluteX) { load_absolute (cpu, LDY_ABX, cpu.Y, 4, &cpu.X, 0x5); }


// Store functions
void store_common_zero_page(CPU& cpu, u8 storeInst, u8& storeReg, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No wrap with offset
    cpu[RESET_LOC] = storeInst;
    cpu[RESET_LOC + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    storeReg = 0x42;    // Value in register to store
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu[(u8) (0x50 + offsetVal)] == storeReg);

    // Wrap offset
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;   // Force wrap
        *offsetReg = offsetVal;
        cpu[RESET_LOC] = storeInst;
        cpu[RESET_LOC + 1] = 0x50;
        storeReg = 0x42;
        ASSERT_TRUE(cpu.execute(cycles) == cycles);
        ASSERT_TRUE(cpu[(u8) (0x50 + offsetVal)] == storeReg);
    }
}

void store_common_absolute(CPU& cpu, u8 storeInst, u8& storeReg, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    cpu[RESET_LOC] = storeInst;
    cpu[RESET_LOC + 1] = 0x50;
    cpu[RESET_LOC + 2] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    storeReg = 0x42;    // Value in register to store
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu[0x5050 + offsetVal] == storeReg);
}

void store_indexed_indirect(CPU& cpu, u8 storeInst, u8& storeReg, u8 cycles) {
    cpu[RESET_LOC] = storeInst;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    storeReg = 0x67;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should store to 0x1234
    cpu[0x1234] = 0x0;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu[0x1234] == storeReg);
}

void store_indirect_indexed(CPU& cpu, u8 storeInst, u8& storeReg, u8 cycles) {
    cpu[RESET_LOC] = storeInst;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.Y = 0x5;
    storeReg = 0x67;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0x5
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu[0x1239] == storeReg);
}

// STA
TEST_F(STA, ZeroPage)        { store_common_zero_page(cpu, STA_ZPG, cpu.A, 3); }
TEST_F(STA, ZeroPageX)       { store_common_zero_page(cpu, STA_ZPX, cpu.A, 4, &cpu.X, 0xFF); }
TEST_F(STA, Absolute)        { store_common_absolute (cpu, STA_ABS, cpu.A, 4); }
TEST_F(STA, AbsoluteX)       { store_common_absolute (cpu, STA_ABX, cpu.A, 5, &cpu.X, 0xFF); }
TEST_F(STA, AbsoluteY)       { store_common_absolute (cpu, STA_ABY, cpu.A, 5, &cpu.Y, 0x45); }
TEST_F(STA, IndexedIndirect) { store_indexed_indirect(cpu, STA_IDX, cpu.A, 6); }
TEST_F(STA, IndirectIndexed) { store_indirect_indexed(cpu, STA_IDY, cpu.A, 6); }

// STX
TEST_F(STX, ZeroPage)   { store_common_zero_page(cpu, STX_ZPG, cpu.X, 3); }
TEST_F(STX, ZeroPageY)  { store_common_zero_page(cpu, STX_ZPY, cpu.X, 4, &cpu.Y, 0xFF); }
TEST_F(STX, Absolute)   { store_common_absolute (cpu, STX_ABS, cpu.X, 4); }

// STY
TEST_F(STY, ZeroPage)   { store_common_zero_page(cpu, STY_ZPG, cpu.Y, 3); }
TEST_F(STY, ZeroPageX)  { store_common_zero_page(cpu, STY_ZPX, cpu.Y, 4, &cpu.X, 0xFF); }
TEST_F(STY, Absolute)   { store_common_absolute (cpu, STY_ABS, cpu.Y, 4); }

// Transfer
void transfer_common(CPU& cpu, u8 inst, u8& fromReg, u8& toReg, bool check_flags) {
    // Positive
    cpu[RESET_LOC] = inst;
    fromReg = 0x10;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(toReg == 0x10);
    if (check_flags) {
        ASSERT_TRUE(cpu.SR.Z == F_NON_ZERO);
        ASSERT_TRUE(cpu.SR.N == F_NON_NEG);
    }

    // Zero
    cpu.reset();
    cpu[RESET_LOC] = inst;
    fromReg = 0x0;
    toReg = 0x5;     // Not zero, so we see zero is actual put in
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(toReg == 0x0);
    if (check_flags) {
        ASSERT_TRUE(cpu.SR.Z == F_ZERO);
        ASSERT_TRUE(cpu.SR.N == F_NON_NEG);
    }

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = inst;
    fromReg = 0xFF;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(toReg == 0xFF);
    if (check_flags) {
        ASSERT_TRUE(cpu.SR.Z == F_NON_ZERO);
        ASSERT_TRUE(cpu.SR.N == F_NEG);
    }
}

TEST_F(TRANSFER, TAX) { transfer_common(cpu, TAX_IMP, cpu.A, cpu.X, true); }
TEST_F(TRANSFER, TAY) { transfer_common(cpu, TAY_IMP, cpu.A, cpu.Y, true); }
TEST_F(TRANSFER, TSX) { transfer_common(cpu, TSX_IMP, cpu.S, cpu.X, true); }
TEST_F(TRANSFER, TXA) { transfer_common(cpu, TXA_IMP, cpu.X, cpu.A, true); }
TEST_F(TRANSFER, TXS) { transfer_common(cpu, TXS_IMP, cpu.X, cpu.S, false); }
TEST_F(TRANSFER, TYA) { transfer_common(cpu, TYA_IMP, cpu.Y, cpu.A, true); }


// No op
TEST_F(NOP, MultipleNoOp) { 
    cpu[RESET_LOC] = NOP_IMP;
    cpu[RESET_LOC + 1] = NOP_IMP;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.PC == RESET_LOC + 2);
}


// Clear flags
TEST_F(CLEAR_FLAGS, ClearDifferentFlags) { 
    // Carry
    cpu[RESET_LOC] = CLC_IMP;
    cpu.SR.C = 1;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.SR.C == 0);

    // Decimal
    cpu.reset();
    cpu[RESET_LOC] = CLD_IMP;
    cpu.SR.D = 1;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.SR.D == 0);

    // Interrupt Disable
    cpu.reset();
    cpu[RESET_LOC] = CLI_IMP;
    cpu.SR.I = 1;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.SR.I == 0);

    // Overflow
    cpu.reset();
    cpu[RESET_LOC] = CLV_IMP;
    cpu.SR.V = 1;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.SR.V == 0);
}


// Set flags
TEST_F(SET_FLAGS, SetDifferentFlags) { 
    // Carry
    cpu[RESET_LOC] = SEC_IMP;
    cpu.SR.C = 0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.SR.C == 1);

    // Decimal
    cpu.reset();
    cpu[RESET_LOC] = SED_IMP;
    cpu.SR.D = 0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.SR.D == 1);

    // Interrupt Disable
    cpu.reset();
    cpu[RESET_LOC] = SEI_IMP;
    cpu.SR.I = 0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.SR.I == 1);
}


// Increment / decrement
void inc_dec_implied(CPU& cpu, u8 inst, u8& incDecReg, u8 startingVal, s8 offset, u1 zeroFlagExpect, u1 negativeFlagExpect) {
    cpu[RESET_LOC] = inst;
    incDecReg = startingVal;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(incDecReg == (u8) (startingVal + offset));
    ASSERT_TRUE(cpu.SR.Z == zeroFlagExpect);
    ASSERT_TRUE(cpu.SR.N == negativeFlagExpect);
    cpu.reset();
}

// Zero and Negative flags not tested for INC and DEC, but covered under INX, INY, DEX, DEY
TEST_F(INC_DEC, INC) {
    // Zero page
    cpu[RESET_LOC] = INC_ZPG;
    cpu[RESET_LOC + 1] = 0x12;
    cpu[0x12] = 0x41;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x12] == 0x42);

    // Zero page x
    cpu.reset();
    cpu[RESET_LOC] = INC_ZPX;
    cpu[RESET_LOC + 1] = 0x12;
    cpu.X = 0x10;
    cpu[0x22] = 0x41;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x22] == 0x42);

    // Zero page x wrap
    cpu.reset();
    cpu[RESET_LOC] = INC_ZPX;
    cpu[RESET_LOC + 1] = 0xF5;
    cpu.X = 0x10;
    cpu[0x05] = 0x41;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x05] == 0x42);

    // Absolute
    cpu.reset();
    cpu[RESET_LOC] = INC_ABS;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu[0x1234] = 0x41;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x42);

    // Absolute x
    cpu.reset();
    cpu[RESET_LOC] = INC_ABX;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.X = 0x6;
    cpu[0x123A] = 0x41;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x123A] == 0x42);
}
TEST_F(INC_DEC, INX) {
    inc_dec_implied(cpu, INX_IMP, cpu.X, 0xFF, 1, F_ZERO, F_NON_NEG);       // Zero
    inc_dec_implied(cpu, INX_IMP, cpu.X, 0x41, 1, F_NON_ZERO, F_NON_NEG);   // Positive
    inc_dec_implied(cpu, INX_IMP, cpu.X, 0xF0, 1, F_NON_ZERO, F_NEG);       // Negative
}
TEST_F(INC_DEC, INY) {
    inc_dec_implied(cpu, INY_IMP, cpu.Y, 0xFF, 1, F_ZERO, F_NON_NEG);       // Zero
    inc_dec_implied(cpu, INY_IMP, cpu.Y, 0x41, 1, F_NON_ZERO, F_NON_NEG);   // Positive
    inc_dec_implied(cpu, INY_IMP, cpu.Y, 0xF0, 1, F_NON_ZERO, F_NEG);       // Negative
}
TEST_F(INC_DEC, DEC) {
    // Zero page
    cpu[RESET_LOC] = DEC_ZPG;
    cpu[RESET_LOC + 1] = 0x12;
    cpu[0x12] = 0x43;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x12] == 0x42);

    // Zero page x
    cpu.reset();
    cpu[RESET_LOC] = DEC_ZPX;
    cpu[RESET_LOC + 1] = 0x12;
    cpu.X = 0x10;
    cpu[0x22] = 0x43;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x22] == 0x42);

    // Zero page x wrap
    cpu.reset();
    cpu[RESET_LOC] = DEC_ZPX;
    cpu[RESET_LOC + 1] = 0xF5;
    cpu.X = 0x10;
    cpu[0x05] = 0x43;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x05] == 0x42);

    // Absolute
    cpu.reset();
    cpu[RESET_LOC] = DEC_ABS;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu[0x1234] = 0x43;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x42);

    // Absolute x
    cpu.reset();
    cpu[RESET_LOC] = DEC_ABX;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.X = 0x6;
    cpu[0x123A] = 0x43;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x123A] == 0x42);
}
TEST_F(INC_DEC, DEX) {
    inc_dec_implied(cpu, DEX_IMP, cpu.X, 0x1,  -1, F_ZERO, F_NON_NEG);      // Zero
    inc_dec_implied(cpu, DEX_IMP, cpu.X, 0x43, -1, F_NON_ZERO, F_NON_NEG);  // Positive
    inc_dec_implied(cpu, DEX_IMP, cpu.X, 0xF0, -1, F_NON_ZERO, F_NEG);      // Negative
}
TEST_F(INC_DEC, DEY) {
    inc_dec_implied(cpu, DEY_IMP, cpu.Y, 0x1,  -1, F_ZERO, F_NON_NEG);      // Zero
    inc_dec_implied(cpu, DEY_IMP, cpu.Y, 0x43, -1, F_NON_ZERO, F_NON_NEG);  // Positive
    inc_dec_implied(cpu, DEY_IMP, cpu.Y, 0xF0, -1, F_NON_ZERO, F_NEG);      // Negative
}


// And
void and_common_zero_page(CPU& cpu, u8 andInst, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No wrap with offset
    cpu[RESET_LOC] = andInst;
    cpu[RESET_LOC + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu.A = 0x42;
    cpu[(u8) (0x50 + offsetVal)] = 0xF0;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0x40);

    // Wrap offset
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;   // Force wrap
        *offsetReg = offsetVal;
        cpu[RESET_LOC] = andInst;
        cpu[RESET_LOC + 1] = 0x50;
        cpu.A = 0x42;
        cpu[(u8) (0x50 + offsetVal)] = 0x0F;
        ASSERT_TRUE(cpu.execute(cycles) == cycles);
        ASSERT_TRUE(cpu.A == 0x02);
    }
}

void and_common_absolute(CPU& cpu, u8 andInst, u32 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No page cross
    cpu[RESET_LOC] = andInst;
    cpu[RESET_LOC + 1] = 0x68;
    cpu[RESET_LOC + 2] = 0x24;
    cpu.A = 0x42;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x2468 + offsetVal] = 0xF0;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0x40);

    // Page cross
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;
        cpu[RESET_LOC] = andInst;
        cpu[RESET_LOC + 1] = 0x68;
        cpu[RESET_LOC + 2] = 0x24;
        cpu.A = 0x42;
        if (offsetReg != nullptr) { *offsetReg = offsetVal; }
        cpu[0x2468 + offsetVal] = 0xF0;
        ASSERT_TRUE(cpu.execute(cycles + 1) == cycles + 1);
        ASSERT_TRUE(cpu.A == 0x40);
    }
}

TEST_F(AND, Immediate) {
    // Zero
    cpu[RESET_LOC] = AND_IMM;
    cpu[RESET_LOC + 1] = 0x0;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.Z == F_ZERO);
    ASSERT_TRUE(cpu.SR.N == F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_LOC] = AND_IMM;
    cpu[RESET_LOC + 1] = 0x3C;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x30);
    ASSERT_TRUE(cpu.SR.Z == F_NON_ZERO);
    ASSERT_TRUE(cpu.SR.N == F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = AND_IMM;
    cpu[RESET_LOC + 1] = 0x8F;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.SR.Z == F_NON_ZERO);
    ASSERT_TRUE(cpu.SR.N == F_NEG);
}
TEST_F(AND, ZeroPage)   { and_common_zero_page(cpu, AND_ZPG, 3); }
TEST_F(AND, ZeroPageX)  { and_common_zero_page(cpu, AND_ZPX, 4, &cpu.X, 0x20); }
TEST_F(AND, Absolute)   { and_common_absolute (cpu, AND_ABS, 4); }
TEST_F(AND, AbsoluteX)  { and_common_absolute (cpu, AND_ABX, 4, &cpu.X, 0x10); }
TEST_F(AND, AbsoluteY)  { and_common_absolute (cpu, AND_ABY, 4, &cpu.Y, 0x10);}
TEST_F(AND, IndirectX)  {
    cpu[RESET_LOC] = AND_IDX;
    cpu.A = 0x12;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should store to 0x1234
    cpu[0x1234] = 0xF0;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(AND, IndirectY)  {
    // No page cross
    cpu[RESET_LOC] = AND_IDY;
    cpu.A = 0x12;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0x5
    cpu[0x1239] = 0x0F;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x02);

    // Page cross
    cpu.reset();
    cpu[RESET_LOC] = AND_IDY;
    cpu.A = 0x12;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.Y = 0xF2;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0xF2
    cpu[0x1326] = 0xF0;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x10);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
