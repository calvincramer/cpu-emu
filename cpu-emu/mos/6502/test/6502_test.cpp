#include <gtest/gtest.h>
#include "mos6502.hpp"

using namespace mos6502;

// Values for flags
#define F_ZERO 1
#define F_NON_ZERO 0

#define F_NEG 1
#define F_NON_NEG 0

#define F_YES_CARRY 1
#define F_NO_CARRY 0

#define F_YES_OVERFLOW 1
#define F_NO_OVERFLOW 0


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

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
class EOR           : public SetupCPU_F {};
class ORA           : public SetupCPU_F {};
class ASL           : public SetupCPU_F {};
class LSR           : public SetupCPU_F {};
class ROL           : public SetupCPU_F {};
class ROR           : public SetupCPU_F {};
class ADC           : public SetupCPU_F {};
class SBC           : public SetupCPU_F {};
class BIT           : public SetupCPU_F {};

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

TEST_F(TRANSFER, TransferA_to_X) { transfer_common(cpu, TAX_IMP, cpu.A, cpu.X, true); }
TEST_F(TRANSFER, TransferA_to_Y) { transfer_common(cpu, TAY_IMP, cpu.A, cpu.Y, true); }
TEST_F(TRANSFER, TransferS_to_X) { transfer_common(cpu, TSX_IMP, cpu.S, cpu.X, true); }
TEST_F(TRANSFER, TransferX_to_A) { transfer_common(cpu, TXA_IMP, cpu.X, cpu.A, true); }
TEST_F(TRANSFER, TransferX_to_S) { transfer_common(cpu, TXS_IMP, cpu.X, cpu.S, false); }
TEST_F(TRANSFER, TransferY_to_A) { transfer_common(cpu, TYA_IMP, cpu.Y, cpu.A, true); }


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
TEST_F(INC_DEC, Increment) {
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
TEST_F(INC_DEC, IncrementX) {
    inc_dec_implied(cpu, INX_IMP, cpu.X, 0xFF, 1, F_ZERO, F_NON_NEG);       // Zero
    inc_dec_implied(cpu, INX_IMP, cpu.X, 0x41, 1, F_NON_ZERO, F_NON_NEG);   // Positive
    inc_dec_implied(cpu, INX_IMP, cpu.X, 0xF0, 1, F_NON_ZERO, F_NEG);       // Negative
}
TEST_F(INC_DEC, IncrementY) {
    inc_dec_implied(cpu, INY_IMP, cpu.Y, 0xFF, 1, F_ZERO, F_NON_NEG);       // Zero
    inc_dec_implied(cpu, INY_IMP, cpu.Y, 0x41, 1, F_NON_ZERO, F_NON_NEG);   // Positive
    inc_dec_implied(cpu, INY_IMP, cpu.Y, 0xF0, 1, F_NON_ZERO, F_NEG);       // Negative
}
TEST_F(INC_DEC, Decrement) {
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
TEST_F(INC_DEC, DecrementX) {
    inc_dec_implied(cpu, DEX_IMP, cpu.X, 0x1,  -1, F_ZERO, F_NON_NEG);      // Zero
    inc_dec_implied(cpu, DEX_IMP, cpu.X, 0x43, -1, F_NON_ZERO, F_NON_NEG);  // Positive
    inc_dec_implied(cpu, DEX_IMP, cpu.X, 0xF0, -1, F_NON_ZERO, F_NEG);      // Negative
}
TEST_F(INC_DEC, DecrementY) {
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


// Eor (exclusive or)
void eor_common_zero_page(CPU& cpu, u8 eorInst, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No wrap with offset
    cpu[RESET_LOC] = eorInst;
    cpu[RESET_LOC + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu.A = 0xF0;
    cpu[(u8) (0x50 + offsetVal)] = 0x55;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0xA5);

    // Wrap offset
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;   // Force wrap
        *offsetReg = offsetVal;
        cpu[RESET_LOC] = eorInst;
        cpu[RESET_LOC + 1] = 0x50;
        cpu.A = 0xF0;
        cpu[(u8) (0x50 + offsetVal)] = 0x55;
        ASSERT_TRUE(cpu.execute(cycles) == cycles);
        ASSERT_TRUE(cpu.A == 0xA5);
    }
}

void eor_common_absolute(CPU& cpu, u8 eorInst, u32 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No page cross
    cpu[RESET_LOC] = eorInst;
    cpu[RESET_LOC + 1] = 0x68;
    cpu[RESET_LOC + 2] = 0x24;
    cpu.A = 0xF0;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x2468 + offsetVal] = 0x55;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0xA5);

    // Page cross
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;
        cpu[RESET_LOC] = eorInst;
        cpu[RESET_LOC + 1] = 0x68;
        cpu[RESET_LOC + 2] = 0x24;
        cpu.A = 0xF0;
        if (offsetReg != nullptr) { *offsetReg = offsetVal; }
        cpu[0x2468 + offsetVal] = 0x55;
        ASSERT_TRUE(cpu.execute(cycles + 1) == cycles + 1);
        ASSERT_TRUE(cpu.A == 0xA5);
    }
}

TEST_F(EOR, Immediate) {
    // Zero
    cpu[RESET_LOC] = EOR_IMM;
    cpu[RESET_LOC + 1] = 0xF0;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.Z == F_ZERO);
    ASSERT_TRUE(cpu.SR.N == F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_LOC] = EOR_IMM;
    cpu[RESET_LOC + 1] = 0x55;
    cpu.A = 0x0F;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x5A);
    ASSERT_TRUE(cpu.SR.Z == F_NON_ZERO);
    ASSERT_TRUE(cpu.SR.N == F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = EOR_IMM;
    cpu[RESET_LOC + 1] = 0x55;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xA5);
    ASSERT_TRUE(cpu.SR.Z == F_NON_ZERO);
    ASSERT_TRUE(cpu.SR.N == F_NEG);
}
TEST_F(EOR, ZeroPage)   { eor_common_zero_page(cpu, EOR_ZPG, 3); }
TEST_F(EOR, ZeroPageX)  { eor_common_zero_page(cpu, EOR_ZPX, 4, &cpu.X, 0x20); }
TEST_F(EOR, Absolute)   { eor_common_absolute (cpu, EOR_ABS, 4); }
TEST_F(EOR, AbsoluteX)  { eor_common_absolute (cpu, EOR_ABX, 4, &cpu.X, 0x10); }
TEST_F(EOR, AbsoluteY)  { eor_common_absolute (cpu, EOR_ABY, 4, &cpu.Y, 0x10);}
TEST_F(EOR, IndirectX)  {
    cpu[RESET_LOC] = EOR_IDX;
    cpu.A = 0xF0;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should store to 0x1234
    cpu[0x1234] = 0x55;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0xA5);
}
TEST_F(EOR, IndirectY)  {
    // No page cross
    cpu[RESET_LOC] = EOR_IDY;
    cpu.A = 0xF0;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0x5
    cpu[0x1239] = 0x55;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0xA5);

    // Page cross
    cpu.reset();
    cpu[RESET_LOC] = EOR_IDY;
    cpu.A = 0xF0;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.Y = 0xF2;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0xF2
    cpu[0x1326] = 0x55;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0xA5);
}


// Or (ORA)
void ora_common_zero_page(CPU& cpu, u8 oraInst, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No wrap with offset
    cpu[RESET_LOC] = oraInst;
    cpu[RESET_LOC + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu.A = 0xF0;
    cpu[(u8) (0x50 + offsetVal)] = 0x0C;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0xFC);

    // Wrap offset
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;   // Force wrap
        *offsetReg = offsetVal;
        cpu[RESET_LOC] = oraInst;
        cpu[RESET_LOC + 1] = 0x50;
        cpu.A = 0xF0;
        cpu[(u8) (0x50 + offsetVal)] = 0x0D;
        ASSERT_TRUE(cpu.execute(cycles) == cycles);
        ASSERT_TRUE(cpu.A == 0xFD);
    }
}

void ora_common_absolute(CPU& cpu, u8 oraInst, u32 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No page cross
    cpu[RESET_LOC] = oraInst;
    cpu[RESET_LOC + 1] = 0x68;
    cpu[RESET_LOC + 2] = 0x24;
    cpu.A = 0xF0;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x2468 + offsetVal] = 0x0C;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0xFC);

    // Page cross
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;
        cpu[RESET_LOC] = oraInst;
        cpu[RESET_LOC + 1] = 0x68;
        cpu[RESET_LOC + 2] = 0x24;
        cpu.A = 0xF0;
        if (offsetReg != nullptr) { *offsetReg = offsetVal; }
        cpu[0x2468 + offsetVal] = 0x0D;
        ASSERT_TRUE(cpu.execute(cycles + 1) == cycles + 1);
        ASSERT_TRUE(cpu.A == 0xFD);
    }
}

TEST_F(ORA, Immediate) {
    // Zero
    cpu[RESET_LOC] = ORA_IMM;
    cpu[RESET_LOC + 1] = 0x0;
    cpu.A = 0x0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.Z == F_ZERO);
    ASSERT_TRUE(cpu.SR.N == F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_LOC] = ORA_IMM;
    cpu[RESET_LOC + 1] = 0x0F;
    cpu.A = 0x1F;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x1F);
    ASSERT_TRUE(cpu.SR.Z == F_NON_ZERO);
    ASSERT_TRUE(cpu.SR.N == F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = ORA_IMM;
    cpu[RESET_LOC + 1] = 0x55;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xF5);
    ASSERT_TRUE(cpu.SR.Z == F_NON_ZERO);
    ASSERT_TRUE(cpu.SR.N == F_NEG);
}
TEST_F(ORA, ZeroPage)   { ora_common_zero_page(cpu, ORA_ZPG, 3); }
TEST_F(ORA, ZeroPageX)  { ora_common_zero_page(cpu, ORA_ZPX, 4, &cpu.X, 0x20); }
TEST_F(ORA, Absolute)   { ora_common_absolute (cpu, ORA_ABS, 4); }
TEST_F(ORA, AbsoluteX)  { ora_common_absolute (cpu, ORA_ABX, 4, &cpu.X, 0x10); }
TEST_F(ORA, AbsoluteY)  { ora_common_absolute (cpu, ORA_ABY, 4, &cpu.Y, 0x10);}
TEST_F(ORA, IndirectX)  {
    cpu[RESET_LOC] = ORA_IDX;
    cpu.A = 0xF0;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should store to 0x1234
    cpu[0x1234] = 0x55;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0xF5);
}
TEST_F(ORA, IndirectY)  {
    // No page cross
    cpu[RESET_LOC] = ORA_IDY;
    cpu.A = 0xF0;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0x5
    cpu[0x1239] = 0x55;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0xF5);

    // Page cross
    cpu.reset();
    cpu[RESET_LOC] = ORA_IDY;
    cpu.A = 0xF0;
    cpu[RESET_LOC + 1] = 0x10;  // zp addr
    cpu.Y = 0xF2;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0xF2
    cpu[0x1326] = 0x55;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0xF5);
}


// ASL
TEST_F(ASL, Accumulator) {
    // No carry
    cpu[RESET_LOC] = ASL_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x10);
    ASSERT_TRUE(cpu.SR.C == F_NO_CARRY);

    // Carry
    cpu.reset();
    cpu[RESET_LOC] = ASL_ACC;
    cpu.A = 0xFF;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFE);
    ASSERT_TRUE(cpu.SR.C == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_LOC] = ASL_ACC;
    cpu.A = 0x80;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.C == F_YES_CARRY);

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = ASL_ACC;
    cpu.A = 0x40;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.SR.N == F_NEG);
}
// Assume flags are set correctly now, only test shift is correct
TEST_F(ASL, ZeroPage) {
    cpu[RESET_LOC] = ASL_ZPG;
    cpu[RESET_LOC + 1] = 0x42;
    cpu[0x42] = 0x12;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0x24);
}
TEST_F(ASL, ZeroPageX) {
    cpu[RESET_LOC] = ASL_ZPX;
    cpu[RESET_LOC + 1] = 0x42;
    cpu.X = 0x10;
    cpu[0x52] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0x24);
}
TEST_F(ASL, Absolute) {
    cpu[RESET_LOC] = ASL_ABS;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu[0x1234] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x24);
}
TEST_F(ASL, AbsoluteX) {
    cpu[RESET_LOC] = ASL_ABX;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0x12;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0x24);
}


// LSR
TEST_F(LSR, Accumulator) {
    // No carry
    cpu[RESET_LOC] = LSR_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x04);
    ASSERT_TRUE(cpu.SR.C == F_NO_CARRY);

    // Carry
    cpu.reset();
    cpu[RESET_LOC] = LSR_ACC;
    cpu.A = 0xFF;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x7F);
    ASSERT_TRUE(cpu.SR.C == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_LOC] = LSR_ACC;
    cpu.A = 0x01;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.C == F_YES_CARRY);
}
// Assume flags are set correctly now, only test shift is correct
TEST_F(LSR, ZeroPage) {
    cpu[RESET_LOC] = LSR_ZPG;
    cpu[RESET_LOC + 1] = 0x42;
    cpu[0x42] = 0x12;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0x09);
}
TEST_F(LSR, ZeroPageX) {
    cpu[RESET_LOC] = LSR_ZPX;
    cpu[RESET_LOC + 1] = 0x42;
    cpu.X = 0x10;
    cpu[0x52] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0x09);
}
TEST_F(LSR, Absolute) {
    cpu[RESET_LOC] = LSR_ABS;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu[0x1234] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x09);
}
TEST_F(LSR, AbsoluteX) {
    cpu[RESET_LOC] = LSR_ABX;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0x12;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0x09);
}


// ROL
TEST_F(ROL, Accumulator) {
    // No rotate bit
    cpu[RESET_LOC] = ROL_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x10);
    ASSERT_TRUE(cpu.SR.C == F_NO_CARRY);

    // Rotate bit
    cpu.reset();
    cpu[RESET_LOC] = ROL_ACC;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xE1);
    ASSERT_TRUE(cpu.SR.C == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_LOC] = ROL_ACC;
    cpu.A = 0x0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.C == F_NO_CARRY);

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = ROL_ACC;
    cpu.A = 0x40;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.SR.N == F_NEG);
}
// Assume flags are set correctly now, only test rotate is correct
TEST_F(ROL, ZeroPage) {
    cpu[RESET_LOC] = ROL_ZPG;
    cpu[RESET_LOC + 1] = 0x42;
    cpu[0x42] = 0xAA;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0x55);
}
TEST_F(ROL, ZeroPageX) {
    cpu[RESET_LOC] = ROL_ZPX;
    cpu[RESET_LOC + 1] = 0x42;
    cpu.X = 0x10;
    cpu[0x52] = 0xAA;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0x55);
}
TEST_F(ROL, Absolute) {
    cpu[RESET_LOC] = ROL_ABS;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu[0x1234] = 0xAA;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x55);
}
TEST_F(ROL, AbsoluteX) {
    cpu[RESET_LOC] = ROL_ABX;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0xAA;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0x55);
}


// ROR
TEST_F(ROR, Accumulator) {
    // No rotate bit
    cpu[RESET_LOC] = ROR_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x04);
    ASSERT_TRUE(cpu.SR.C == F_NO_CARRY);

    // Rotate bit
    cpu.reset();
    cpu[RESET_LOC] = ROR_ACC;
    cpu.A = 0xF1;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xF8);
    ASSERT_TRUE(cpu.SR.C == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_LOC] = ROR_ACC;
    cpu.A = 0x0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.C == F_NO_CARRY);

    // Negative
    cpu.reset();
    cpu[RESET_LOC] = ROR_ACC;
    cpu.A = 0x01;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.SR.N == F_NEG);
}
// Assume flags are set correctly now, only test rotate is correct
TEST_F(ROR, ZeroPage) {
    cpu[RESET_LOC] = ROR_ZPG;
    cpu[RESET_LOC + 1] = 0x42;
    cpu[0x42] = 0xE1;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0xF0);
}
TEST_F(ROR, ZeroPageX) {
    cpu[RESET_LOC] = ROR_ZPX;
    cpu[RESET_LOC + 1] = 0x42;
    cpu.X = 0x10;
    cpu[0x52] = 0xE1;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0xF0);
}
TEST_F(ROR, Absolute) {
    cpu[RESET_LOC] = ROR_ABS;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu[0x1234] = 0xE1;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0xF0);
}
TEST_F(ROR, AbsoluteX) {
    cpu[RESET_LOC] = ROR_ABX;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0xE1;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0xF0);
}


// ADC
TEST_F(ADC, Immediate) {
    cpu[RESET_LOC] = ADC_IMM;
    cpu[RESET_LOC + 1] = 0x12;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x42);
}
TEST_F(ADC, ImmediateZeroFlag) {
    cpu[RESET_LOC] = ADC_IMM;
    cpu[RESET_LOC + 1] = 0x12;
    cpu.A = 0xEE;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.flag_zero() == F_ZERO);
}
TEST_F(ADC, ImmediateNegativeFlag) {
    cpu[RESET_LOC] = ADC_IMM;
    cpu[RESET_LOC + 1] = 0x12;
    cpu.A = 0xE0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xF2);
    ASSERT_TRUE(cpu.flag_negative() == F_NEG);
}
TEST_F(ADC, ImmediateCarryFlag) {
    cpu[RESET_LOC] = ADC_IMM;
    cpu[RESET_LOC + 1] = 0xEF;
    cpu.A = 0x10;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.flag_carry() == F_YES_CARRY);
}
TEST_F(ADC, ImmediateWithCarryEnabled) {
    cpu[RESET_LOC] = ADC_IMM;
    cpu[RESET_LOC + 1] = 0x12;
    cpu.A = 0x30;
    cpu.SR.C = 1;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x43);
}
TEST_F(ADC, ZeroPage) {
    cpu[RESET_LOC] = ADC_ZPG;
    cpu[RESET_LOC + 1] = 0x12;
    cpu[0x12] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, ZeroPageX) {
    cpu[RESET_LOC] = ADC_ZPX;
    cpu[RESET_LOC + 1] = 0x12;
    cpu[0x22] = 0x40;
    cpu.A = 0x30;
    cpu.X = 0x10;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, Absolute) {
    cpu[RESET_LOC] = ADC_ABS;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu[0x1234] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, AbsoluteX) {
    // No page cross
    cpu[RESET_LOC] = ADC_ABX;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.X = 0x10;
    cpu[0x1244] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);

    // Page cross
    cpu.reset();
    cpu[RESET_LOC] = ADC_ABX;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.X = 0xFF;
    cpu[0x1333] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, AbsoluteY) {
    // No page cross
    cpu[RESET_LOC] = ADC_ABY;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.Y = 0x11;
    cpu[0x1245] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);

    // Page cross
    cpu.reset();
    cpu[RESET_LOC] = ADC_ABY;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.Y = 0xF0;
    cpu[0x1324] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, IndirectX) {
    cpu[RESET_LOC] = ADC_IDX;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;
    cpu.A = 0x30;
    cpu[0x1234] = 0x40;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, IndirectY) {
    // No page cross
    cpu[RESET_LOC] = ADC_IDY;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x30;
    cpu[0x1239] = 0x55;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x85);

    // Page cross
    cpu.reset();
    cpu[RESET_LOC] = ADC_IDY;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0xF2;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x30;
    cpu[0x1326] = 0x55;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x85);
}


// SBC
TEST_F(SBC, Immediate) {
    cpu[RESET_LOC] = SBC_IMM;
    cpu[RESET_LOC + 1] = 0x12;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x1E);
}
TEST_F(SBC, ImmediateZeroFlag) {
    cpu[RESET_LOC] = SBC_IMM;
    cpu[RESET_LOC + 1] = 0x12;
    cpu.A = 0x12;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.flag_zero() == F_ZERO);
}
TEST_F(SBC, ImmediateNegativeFlag) {
    cpu[RESET_LOC] = SBC_IMM;
    cpu[RESET_LOC + 1] = 0x13;
    cpu.A = 0x12;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.flag_negative() == F_NEG);
}
TEST_F(SBC, ImmediateCarryFlag) {
    cpu[RESET_LOC] = SBC_IMM;
    cpu[RESET_LOC + 1] = 0x13;
    cpu.A = 0x12;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.flag_carry() == F_NO_CARRY);
}
TEST_F(SBC, ImmediateWithCarryEnabled) {
    cpu[RESET_LOC] = SBC_IMM;
    cpu[RESET_LOC + 1] = 0x12;
    cpu.A = 0x30;
    cpu.SR.C = 1;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x1D);
}
TEST_F(SBC, ZeroPage) {
    cpu[RESET_LOC] = SBC_ZPG;
    cpu[RESET_LOC + 1] = 0x12;
    cpu[0x12] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, ZeroPageX) {
    cpu[RESET_LOC] = SBC_ZPX;
    cpu[RESET_LOC + 1] = 0x12;
    cpu[0x22] = 0x40;
    cpu.A = 0x50;
    cpu.X = 0x10;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, Absolute) {
    cpu[RESET_LOC] = SBC_ABS;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu[0x1234] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, AbsoluteX) {
    // No page cross
    cpu[RESET_LOC] = SBC_ABX;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.X = 0x10;
    cpu[0x1244] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);

    // Page cross
    cpu.reset();
    cpu[RESET_LOC] = SBC_ABX;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.X = 0xFF;
    cpu[0x1333] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, AbsoluteY) {
    // No page cross
    cpu[RESET_LOC] = SBC_ABY;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.Y = 0x11;
    cpu[0x1245] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);

    // Page cross
    cpu.reset();
    cpu[RESET_LOC] = SBC_ABY;
    cpu[RESET_LOC + 1] = 0x34;
    cpu[RESET_LOC + 2] = 0x12;
    cpu.Y = 0xF0;
    cpu[0x1324] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, IndirectX) {
    cpu[RESET_LOC] = SBC_IDX;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;
    cpu.A = 0x50;
    cpu[0x1234] = 0x40;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, IndirectY) {
    // No page cross
    cpu[RESET_LOC] = SBC_IDY;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x50;
    cpu[0x1239] = 0x40;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x10);

    // Page cross
    cpu.reset();
    cpu[RESET_LOC] = SBC_IDY;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0xF2;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x50;
    cpu[0x1326] = 0x40;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x10);
}


// BIT
TEST_F(BIT, ZeroPage) {
    // Zero
    cpu.reset();
    cpu[RESET_LOC] = BIT_ZPG;
    cpu[RESET_LOC + 1] = 0x15;
    cpu[0x15] = 0xF3;   // 0b 1111 0011
    cpu.A = 0x0C;       // 0b 0000 1100
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.flag_zero() == F_ZERO);
    ASSERT_TRUE(cpu.flag_overflow() == F_NO_OVERFLOW);
    ASSERT_TRUE(cpu.flag_negative() == F_NON_NEG);

    // Not zero
    cpu.reset();
    cpu[RESET_LOC] = BIT_ZPG;
    cpu[RESET_LOC + 1] = 0x15;
    cpu[0x15] = 0xF3;   // 0b 1111 0011
    cpu.A = 0x99;       // 0b 1001 1001
    ASSERT_TRUE(cpu.execute(3) == 3);   // Anded value should be 0b 1001 0001
    ASSERT_TRUE(cpu.flag_zero() == F_NON_ZERO);
    ASSERT_TRUE(cpu.flag_overflow() == F_NO_OVERFLOW);
    ASSERT_TRUE(cpu.flag_negative() == F_NEG);
}
TEST_F(BIT, Absolute) {
    cpu.reset();
    cpu[RESET_LOC] = BIT_ABS;
    cpu[RESET_LOC + 1] = 0x15;
    cpu[RESET_LOC + 2] = 0x14;
    cpu[0x1415] = 0xF3;     // 0b 1111 0011
    cpu.A = 0x99;           // 0b 1001 1001
    ASSERT_TRUE(cpu.execute(4) == 4);   // Anded value should be 0b 1001 0001
    ASSERT_TRUE(cpu.flag_zero() == F_NON_ZERO);
    ASSERT_TRUE(cpu.flag_overflow() == F_NO_OVERFLOW);
    ASSERT_TRUE(cpu.flag_negative() == F_NEG);
}
