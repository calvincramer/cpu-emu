#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"

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
class CMP           : public SetupCPU_F {};
class JMP           : public SetupCPU_F {};
class PUSH          : public SetupCPU_F {};
class PULL          : public SetupCPU_F {};
class BRANCH        : public SetupCPU_F {};
class SUBROUTINE    : public SetupCPU_F {};
class INTERRUPT     : public SetupCPU_F {};
class OBELISK_TESTS : public SetupCPU_F {};


TEST_F(Api, Reset) { cpu.reset(); }
TEST_F(Api, Execute) { cpu.execute(0); }

auto common_load_execute = [] (CPU& cpu, u32 runCycles, u32 expCycles, u8& expReg, u8 expVal, u1 expZ, u1 expN) {
    ASSERT_TRUE(cpu.execute(runCycles) == expCycles);
    ASSERT_TRUE(expReg == expVal);
    ASSERT_TRUE(cpu.get_flag_z() == expZ);
    ASSERT_TRUE(cpu.get_flag_n() == expN);
};

// Common load immediate instructions
void load_immediate(CPU& cpu, u8 loadInst, u8& reg) {
    // Zero
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x0;
    common_load_execute(cpu, 2, 2, reg, 0, F_ZERO, F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x42;
    common_load_execute(cpu, 2, 2, reg, 0x42, F_NON_ZERO, F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0xFF;
    common_load_execute(cpu, 2, 2, reg, 0xFF, F_NON_ZERO, F_NEG);
}

// Common load from zero page instructions
void load_zero_page(CPU& cpu, u8 loadInst, u8& loadToReg, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // Zero
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x50 + offsetVal] = 0x0;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x0, F_ZERO, F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x50 + offsetVal] = 0x42;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x50 + offsetVal] = 0xFF;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0xFF, F_NON_ZERO, F_NEG);

    // Wrap (if there is an offset)
    if (offsetReg != nullptr) {
        cpu.reset();
        cpu[RESET_START] = loadInst;
        cpu[RESET_START + 1] = 0xF0;
        *offsetReg = offsetVal;
        cpu[0xF0 + offsetVal] = 0x42;
        common_load_execute(cpu, cycles, cycles, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);
    }
}

// Common load absolute instructions
void load_absolute(CPU& cpu, u8 loadInst, u8& loadToReg, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // Addr is 0x4321
    // Zero
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x21;
    cpu[RESET_START + 2] = 0x43;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x4321 + offsetVal] = 0x0;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x0, F_ZERO, F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x21;
    cpu[RESET_START + 2] = 0x43;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x4321 + offsetVal] = 0x42;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x21;
    cpu[RESET_START + 2] = 0x43;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x4321 + offsetVal] = 0xFF;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0xFF, F_NON_ZERO, F_NEG);

    // Page cross (+1 cycles taken)
    if (offsetReg != nullptr) {
        offsetVal = 0xFF; // Force page cross
        cpu.reset();
        cpu[RESET_START] = loadInst;
        cpu[RESET_START + 1] = 0x21;
        cpu[RESET_START + 2] = 0x43;
        *offsetReg = offsetVal;
        cpu[0x4321 + offsetVal] = 0x42;
        common_load_execute(cpu, cycles + 1, cycles + 1, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);
    }
}

// Common load indexed indirect instructions
void load_indexed_indirect(CPU& cpu, u8 loadInst, u8& loadToReg, u8 cycles) {
    // Zero
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0x0;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x0, F_ZERO, F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0x42;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0xFF;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0xFF, F_NON_ZERO, F_NEG);

    // Wrap zero-page
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x30;
    cpu.X = 0xF0;
    cpu[0x0020] = 0x34;
    cpu[0x0021] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0x31;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x31, F_NON_ZERO, F_NON_NEG);
}

void load_indirect_indexed(CPU& cpu, u8 loadInst, u8& loadToReg, u8 cycles) {
    // Zero
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should load from 0x1234 + 0x5
    cpu[0x1239] = 0x0;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x0, F_ZERO, F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should load from 0x1234 + 0x5
    cpu[0x1239] = 0x42;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0x42, F_NON_ZERO, F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should load from 0x1234 + 0x5
    cpu[0x1239] = 0xFF;
    common_load_execute(cpu, cycles, cycles, loadToReg, 0xFF, F_NON_ZERO, F_NEG);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = loadInst;
    cpu[RESET_START + 1] = 0x10;
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
    cpu[RESET_START] = storeInst;
    cpu[RESET_START + 1] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    storeReg = 0x42;    // Value in register to store
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu[(u8) (0x50 + offsetVal)] == storeReg);

    // Wrap offset
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;   // Force wrap
        *offsetReg = offsetVal;
        cpu[RESET_START] = storeInst;
        cpu[RESET_START + 1] = 0x50;
        storeReg = 0x42;
        ASSERT_TRUE(cpu.execute(cycles) == cycles);
        ASSERT_TRUE(cpu[(u8) (0x50 + offsetVal)] == storeReg);
    }
}

void store_common_absolute(CPU& cpu, u8 storeInst, u8& storeReg, u8 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    cpu[RESET_START] = storeInst;
    cpu[RESET_START + 1] = 0x50;
    cpu[RESET_START + 2] = 0x50;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    storeReg = 0x42;    // Value in register to store
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu[0x5050 + offsetVal] == storeReg);
}

void store_indexed_indirect(CPU& cpu, u8 storeInst, u8& storeReg, u8 cycles) {
    cpu[RESET_START] = storeInst;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    storeReg = 0x67;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should store to 0x1234
    cpu[0x1234] = 0x0;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu[0x1234] == storeReg);
}

void store_indirect_indexed(CPU& cpu, u8 storeInst, u8& storeReg, u8 cycles) {
    cpu[RESET_START] = storeInst;
    cpu[RESET_START + 1] = 0x10;  // zp addr
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
    cpu[RESET_START] = inst;
    fromReg = 0x10;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(toReg == 0x10);
    if (check_flags) {
        ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
        ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);
    }

    // Zero
    cpu.reset();
    cpu[RESET_START] = inst;
    fromReg = 0x0;
    toReg = 0x5;     // Not zero, so we see zero is actual put in
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(toReg == 0x0);
    if (check_flags) {
        ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
        ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);
    }

    // Negative
    cpu.reset();
    cpu[RESET_START] = inst;
    fromReg = 0xFF;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(toReg == 0xFF);
    if (check_flags) {
        ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
        ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
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
    cpu[RESET_START] = NOP_IMP;
    cpu[RESET_START + 1] = NOP_IMP;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.PC == RESET_START + 2);
}


// Clear flags
TEST_F(CLEAR_FLAGS, ClearDifferentFlags) {
    // Carry
    cpu[RESET_START] = CLC_IMP;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == 0);

    // Decimal
    cpu.reset();
    cpu[RESET_START] = CLD_IMP;
    cpu.set_flag_d(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_d() == 0);

    // Interrupt Disable
    cpu.reset();
    cpu[RESET_START] = CLI_IMP;
    cpu.set_flag_i(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_i() == 0);

    // Overflow
    cpu.reset();
    cpu[RESET_START] = CLV_IMP;
    cpu.set_flag_v(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_v() == 0);
}


// Set flags
TEST_F(SET_FLAGS, SetDifferentFlags) {
    // Carry
    cpu[RESET_START] = SEC_IMP;
    cpu.set_flag_c(0);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == 1);

    // Decimal
    cpu.reset();
    cpu[RESET_START] = SED_IMP;
    cpu.set_flag_d(0);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_d() == 1);

    // Interrupt Disable
    cpu.reset();
    cpu[RESET_START] = SEI_IMP;
    cpu.set_flag_i(0);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_i() == 1);
}


// Increment / decrement
void inc_dec_implied(CPU& cpu, u8 inst, u8& incDecReg, u8 startingVal, s8 offset, u1 zeroFlagExpect, u1 negativeFlagExpect) {
    cpu[RESET_START] = inst;
    incDecReg = startingVal;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(incDecReg == (u8) (startingVal + offset));
    ASSERT_TRUE(cpu.get_flag_z() == zeroFlagExpect);
    ASSERT_TRUE(cpu.get_flag_n() == negativeFlagExpect);
    cpu.reset();
}

// Zero and Negative flags not tested for INC and DEC, but covered under INX, INY, DEX, DEY
TEST_F(INC_DEC, Increment) {
    // Zero page
    cpu[RESET_START] = INC_ZPG;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x12] = 0x41;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x12] == 0x42);

    // Zero page x
    cpu.reset();
    cpu[RESET_START] = INC_ZPX;
    cpu[RESET_START + 1] = 0x12;
    cpu.X = 0x10;
    cpu[0x22] = 0x41;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x22] == 0x42);

    // Zero page x wrap
    cpu.reset();
    cpu[RESET_START] = INC_ZPX;
    cpu[RESET_START + 1] = 0xF5;
    cpu.X = 0x10;
    cpu[0x05] = 0x41;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x05] == 0x42);

    // Absolute
    cpu.reset();
    cpu[RESET_START] = INC_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x41;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x42);

    // Absolute x
    cpu.reset();
    cpu[RESET_START] = INC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
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
    cpu[RESET_START] = DEC_ZPG;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x12] = 0x43;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x12] == 0x42);

    // Zero page x
    cpu.reset();
    cpu[RESET_START] = DEC_ZPX;
    cpu[RESET_START + 1] = 0x12;
    cpu.X = 0x10;
    cpu[0x22] = 0x43;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x22] == 0x42);

    // Zero page x wrap
    cpu.reset();
    cpu[RESET_START] = DEC_ZPX;
    cpu[RESET_START + 1] = 0xF5;
    cpu.X = 0x10;
    cpu[0x05] = 0x43;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x05] == 0x42);

    // Absolute
    cpu.reset();
    cpu[RESET_START] = DEC_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x43;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x42);

    // Absolute x
    cpu.reset();
    cpu[RESET_START] = DEC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
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
    cpu[RESET_START] = andInst;
    cpu[RESET_START + 1] = 0x50;
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
        cpu[RESET_START] = andInst;
        cpu[RESET_START + 1] = 0x50;
        cpu.A = 0x42;
        cpu[(u8) (0x50 + offsetVal)] = 0x0F;
        ASSERT_TRUE(cpu.execute(cycles) == cycles);
        ASSERT_TRUE(cpu.A == 0x02);
    }
}

void and_common_absolute(CPU& cpu, u8 andInst, u32 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No page cross
    cpu[RESET_START] = andInst;
    cpu[RESET_START + 1] = 0x68;
    cpu[RESET_START + 2] = 0x24;
    cpu.A = 0x42;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x2468 + offsetVal] = 0xF0;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0x40);

    // Page cross
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;
        cpu[RESET_START] = andInst;
        cpu[RESET_START + 1] = 0x68;
        cpu[RESET_START + 2] = 0x24;
        cpu.A = 0x42;
        if (offsetReg != nullptr) { *offsetReg = offsetVal; }
        cpu[0x2468 + offsetVal] = 0xF0;
        ASSERT_TRUE(cpu.execute(cycles + 1) == cycles + 1);
        ASSERT_TRUE(cpu.A == 0x40);
    }
}

TEST_F(AND, Immediate) {
    // Zero
    cpu[RESET_START] = AND_IMM;
    cpu[RESET_START + 1] = 0x0;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = AND_IMM;
    cpu[RESET_START + 1] = 0x3C;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x30);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = AND_IMM;
    cpu[RESET_START + 1] = 0x8F;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(AND, ZeroPage)   { and_common_zero_page(cpu, AND_ZPG, 3); }
TEST_F(AND, ZeroPageX)  { and_common_zero_page(cpu, AND_ZPX, 4, &cpu.X, 0x20); }
TEST_F(AND, Absolute)   { and_common_absolute (cpu, AND_ABS, 4); }
TEST_F(AND, AbsoluteX)  { and_common_absolute (cpu, AND_ABX, 4, &cpu.X, 0x10); }
TEST_F(AND, AbsoluteY)  { and_common_absolute (cpu, AND_ABY, 4, &cpu.Y, 0x10);}
TEST_F(AND, IndirectX)  {
    cpu[RESET_START] = AND_IDX;
    cpu.A = 0x12;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should store to 0x1234
    cpu[0x1234] = 0xF0;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(AND, IndirectY)  {
    // No page cross
    cpu[RESET_START] = AND_IDY;
    cpu.A = 0x12;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0x5
    cpu[0x1239] = 0x0F;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x02);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = AND_IDY;
    cpu.A = 0x12;
    cpu[RESET_START + 1] = 0x10;  // zp addr
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
    cpu[RESET_START] = eorInst;
    cpu[RESET_START + 1] = 0x50;
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
        cpu[RESET_START] = eorInst;
        cpu[RESET_START + 1] = 0x50;
        cpu.A = 0xF0;
        cpu[(u8) (0x50 + offsetVal)] = 0x55;
        ASSERT_TRUE(cpu.execute(cycles) == cycles);
        ASSERT_TRUE(cpu.A == 0xA5);
    }
}

void eor_common_absolute(CPU& cpu, u8 eorInst, u32 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No page cross
    cpu[RESET_START] = eorInst;
    cpu[RESET_START + 1] = 0x68;
    cpu[RESET_START + 2] = 0x24;
    cpu.A = 0xF0;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x2468 + offsetVal] = 0x55;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0xA5);

    // Page cross
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;
        cpu[RESET_START] = eorInst;
        cpu[RESET_START + 1] = 0x68;
        cpu[RESET_START + 2] = 0x24;
        cpu.A = 0xF0;
        if (offsetReg != nullptr) { *offsetReg = offsetVal; }
        cpu[0x2468 + offsetVal] = 0x55;
        ASSERT_TRUE(cpu.execute(cycles + 1) == cycles + 1);
        ASSERT_TRUE(cpu.A == 0xA5);
    }
}

TEST_F(EOR, Immediate) {
    // Zero
    cpu[RESET_START] = EOR_IMM;
    cpu[RESET_START + 1] = 0xF0;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = EOR_IMM;
    cpu[RESET_START + 1] = 0x55;
    cpu.A = 0x0F;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x5A);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = EOR_IMM;
    cpu[RESET_START + 1] = 0x55;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xA5);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(EOR, ZeroPage)   { eor_common_zero_page(cpu, EOR_ZPG, 3); }
TEST_F(EOR, ZeroPageX)  { eor_common_zero_page(cpu, EOR_ZPX, 4, &cpu.X, 0x20); }
TEST_F(EOR, Absolute)   { eor_common_absolute (cpu, EOR_ABS, 4); }
TEST_F(EOR, AbsoluteX)  { eor_common_absolute (cpu, EOR_ABX, 4, &cpu.X, 0x10); }
TEST_F(EOR, AbsoluteY)  { eor_common_absolute (cpu, EOR_ABY, 4, &cpu.Y, 0x10);}
TEST_F(EOR, IndirectX)  {
    cpu[RESET_START] = EOR_IDX;
    cpu.A = 0xF0;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should store to 0x1234
    cpu[0x1234] = 0x55;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0xA5);
}
TEST_F(EOR, IndirectY)  {
    // No page cross
    cpu[RESET_START] = EOR_IDY;
    cpu.A = 0xF0;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0x5
    cpu[0x1239] = 0x55;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0xA5);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = EOR_IDY;
    cpu.A = 0xF0;
    cpu[RESET_START + 1] = 0x10;  // zp addr
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
    cpu[RESET_START] = oraInst;
    cpu[RESET_START + 1] = 0x50;
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
        cpu[RESET_START] = oraInst;
        cpu[RESET_START + 1] = 0x50;
        cpu.A = 0xF0;
        cpu[(u8) (0x50 + offsetVal)] = 0x0D;
        ASSERT_TRUE(cpu.execute(cycles) == cycles);
        ASSERT_TRUE(cpu.A == 0xFD);
    }
}

void ora_common_absolute(CPU& cpu, u8 oraInst, u32 cycles, u8* offsetReg = nullptr, u8 offsetVal = 0) {
    // No page cross
    cpu[RESET_START] = oraInst;
    cpu[RESET_START + 1] = 0x68;
    cpu[RESET_START + 2] = 0x24;
    cpu.A = 0xF0;
    if (offsetReg != nullptr) { *offsetReg = offsetVal; }
    cpu[0x2468 + offsetVal] = 0x0C;
    ASSERT_TRUE(cpu.execute(cycles) == cycles);
    ASSERT_TRUE(cpu.A == 0xFC);

    // Page cross
    if (offsetReg != nullptr) {
        cpu.reset();
        offsetVal = 0xFF;
        cpu[RESET_START] = oraInst;
        cpu[RESET_START + 1] = 0x68;
        cpu[RESET_START + 2] = 0x24;
        cpu.A = 0xF0;
        if (offsetReg != nullptr) { *offsetReg = offsetVal; }
        cpu[0x2468 + offsetVal] = 0x0D;
        ASSERT_TRUE(cpu.execute(cycles + 1) == cycles + 1);
        ASSERT_TRUE(cpu.A == 0xFD);
    }
}

TEST_F(ORA, Immediate) {
    // Zero
    cpu[RESET_START] = ORA_IMM;
    cpu[RESET_START + 1] = 0x0;
    cpu.A = 0x0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = ORA_IMM;
    cpu[RESET_START + 1] = 0x0F;
    cpu.A = 0x1F;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x1F);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = ORA_IMM;
    cpu[RESET_START + 1] = 0x55;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xF5);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(ORA, ZeroPage)   { ora_common_zero_page(cpu, ORA_ZPG, 3); }
TEST_F(ORA, ZeroPageX)  { ora_common_zero_page(cpu, ORA_ZPX, 4, &cpu.X, 0x20); }
TEST_F(ORA, Absolute)   { ora_common_absolute (cpu, ORA_ABS, 4); }
TEST_F(ORA, AbsoluteX)  { ora_common_absolute (cpu, ORA_ABX, 4, &cpu.X, 0x10); }
TEST_F(ORA, AbsoluteY)  { ora_common_absolute (cpu, ORA_ABY, 4, &cpu.Y, 0x10);}
TEST_F(ORA, IndirectX)  {
    cpu[RESET_START] = ORA_IDX;
    cpu.A = 0xF0;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should store to 0x1234
    cpu[0x1234] = 0x55;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.A == 0xF5);
}
TEST_F(ORA, IndirectY)  {
    // No page cross
    cpu[RESET_START] = ORA_IDY;
    cpu.A = 0xF0;
    cpu[RESET_START + 1] = 0x10;  // zp addr
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should store to 0x1234 + 0x5
    cpu[0x1239] = 0x55;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0xF5);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = ORA_IDY;
    cpu.A = 0xF0;
    cpu[RESET_START + 1] = 0x10;  // zp addr
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
    cpu[RESET_START] = ASL_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x10);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Carry
    cpu.reset();
    cpu[RESET_START] = ASL_ACC;
    cpu.A = 0xFF;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFE);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_START] = ASL_ACC;
    cpu.A = 0x80;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);

    // Negative
    cpu.reset();
    cpu[RESET_START] = ASL_ACC;
    cpu.A = 0x40;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
// Assume flags are set correctly now, only test shift is correct
TEST_F(ASL, ZeroPage) {
    cpu[RESET_START] = ASL_ZPG;
    cpu[RESET_START + 1] = 0x42;
    cpu[0x42] = 0x12;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0x24);
}
TEST_F(ASL, ZeroPageX) {
    cpu[RESET_START] = ASL_ZPX;
    cpu[RESET_START + 1] = 0x42;
    cpu.X = 0x10;
    cpu[0x52] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0x24);
}
TEST_F(ASL, Absolute) {
    cpu[RESET_START] = ASL_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x24);
}
TEST_F(ASL, AbsoluteX) {
    cpu[RESET_START] = ASL_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0x12;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0x24);
}


// LSR
TEST_F(LSR, Accumulator) {
    // No carry
    cpu[RESET_START] = LSR_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x04);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Carry
    cpu.reset();
    cpu[RESET_START] = LSR_ACC;
    cpu.A = 0xFF;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x7F);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_START] = LSR_ACC;
    cpu.A = 0x01;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
}
// Assume flags are set correctly now, only test shift is correct
TEST_F(LSR, ZeroPage) {
    cpu[RESET_START] = LSR_ZPG;
    cpu[RESET_START + 1] = 0x42;
    cpu[0x42] = 0x12;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0x09);
}
TEST_F(LSR, ZeroPageX) {
    cpu[RESET_START] = LSR_ZPX;
    cpu[RESET_START + 1] = 0x42;
    cpu.X = 0x10;
    cpu[0x52] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0x09);
}
TEST_F(LSR, Absolute) {
    cpu[RESET_START] = LSR_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x09);
}
TEST_F(LSR, AbsoluteX) {
    cpu[RESET_START] = LSR_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0x12;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0x09);
}


// ROL
TEST_F(ROL, Accumulator) {
    // No rotate bit
    cpu[RESET_START] = ROL_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x10);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Rotate bit
    cpu.reset();
    cpu[RESET_START] = ROL_ACC;
    cpu.A = 0xF0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xE0);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_START] = ROL_ACC;
    cpu.A = 0x0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Negative
    cpu.reset();
    cpu[RESET_START] = ROL_ACC;
    cpu.A = 0x40;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
// Assume flags are set correctly now, only test rotate is correct
TEST_F(ROL, ZeroPage) {
    cpu[RESET_START] = ROL_ZPG;
    cpu[RESET_START + 1] = 0x42;
    cpu[0x42] = 0xAA;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0x54);
}
TEST_F(ROL, ZeroPageX) {
    cpu[RESET_START] = ROL_ZPX;
    cpu[RESET_START + 1] = 0x42;
    cpu.X = 0x10;
    cpu[0x52] = 0xAA;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0x54);
}
TEST_F(ROL, Absolute) {
    cpu[RESET_START] = ROL_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0xAA;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0x54);
}
TEST_F(ROL, AbsoluteX) {
    cpu[RESET_START] = ROL_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0xAA;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0x54);
}
TEST_F(ROL, RotateWithCarrySet) {
    cpu[RESET_START] = ROL_ACC;
    cpu.A = 0x18;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x31);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);
}


// ROR
TEST_F(ROR, Accumulator) {
    // No rotate bit
    cpu[RESET_START] = ROR_ACC;
    cpu.A = 0x08;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x04);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Rotate bit
    cpu.reset();
    cpu[RESET_START] = ROR_ACC;
    cpu.A = 0xF1;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xF8);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);

    // Zero
    cpu.reset();
    cpu[RESET_START] = ROR_ACC;
    cpu.A = 0x0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);

    // Negative
    cpu.reset();
    cpu[RESET_START] = ROR_ACC;
    cpu.A = 0x01;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x80);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
// Assume flags are set correctly now, only test rotate is correct
TEST_F(ROR, ZeroPage) {
    cpu[RESET_START] = ROR_ZPG;
    cpu[RESET_START + 1] = 0x42;
    cpu[0x42] = 0xE1;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu[0x42] == 0xF0);
}
TEST_F(ROR, ZeroPageX) {
    cpu[RESET_START] = ROR_ZPX;
    cpu[RESET_START + 1] = 0x42;
    cpu.X = 0x10;
    cpu.set_flag_c(1);
    cpu[0x52] = 0xE1;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x52] == 0xF0);
}
TEST_F(ROR, Absolute) {
    cpu[RESET_START] = ROR_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0xE1;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu[0x1234] == 0xF0);
}
TEST_F(ROR, AbsoluteX) {
    cpu[RESET_START] = ROR_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x1;
    cpu[0x1235] = 0xE1;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu[0x1235] == 0xF0);
}


// ADC
TEST_F(ADC, Immediate) {
    cpu[RESET_START] = ADC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x42);
}
TEST_F(ADC, ImmediateZeroFlag) {
    cpu[RESET_START] = ADC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0xEE;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
}
TEST_F(ADC, ImmediateNegativeFlag) {
    cpu[RESET_START] = ADC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0xE0;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xF2);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(ADC, ImmediateCarryFlag) {
    cpu[RESET_START] = ADC_IMM;
    cpu[RESET_START + 1] = 0xEF;
    cpu.A = 0x10;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
}
TEST_F(ADC, ImmediateWithCarryEnabled) {
    cpu[RESET_START] = ADC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0x30;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x43);
}
TEST_F(ADC, ZeroPage) {
    cpu[RESET_START] = ADC_ZPG;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x12] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, ZeroPageX) {
    cpu[RESET_START] = ADC_ZPX;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x22] = 0x40;
    cpu.A = 0x30;
    cpu.X = 0x10;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, Absolute) {
    cpu[RESET_START] = ADC_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, AbsoluteX) {
    // No page cross
    cpu[RESET_START] = ADC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x10;
    cpu[0x1244] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = ADC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0xFF;
    cpu[0x1333] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, AbsoluteY) {
    // No page cross
    cpu[RESET_START] = ADC_ABY;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.Y = 0x11;
    cpu[0x1245] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x70);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = ADC_ABY;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.Y = 0xF0;
    cpu[0x1324] = 0x40;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x70);
}
TEST_F(ADC, IndirectX) {
    cpu[RESET_START] = ADC_IDX;
    cpu[RESET_START + 1] = 0x10;
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
    cpu[RESET_START] = ADC_IDY;
    cpu[RESET_START + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x30;
    cpu[0x1239] = 0x55;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x85);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = ADC_IDY;
    cpu[RESET_START + 1] = 0x10;
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
    cpu[RESET_START] = SBC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0x30;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x1E);
}
TEST_F(SBC, ImmediateZeroFlag) {
    cpu[RESET_START] = SBC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0x12;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
}
TEST_F(SBC, ImmediateNegativeFlag) {
    cpu[RESET_START] = SBC_IMM;
    cpu[RESET_START + 1] = 0x13;
    cpu.A = 0x12;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(SBC, ImmediateCarryFlag) {
    cpu[RESET_START] = SBC_IMM;
    cpu[RESET_START + 1] = 0x13;
    cpu.A = 0x12;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);
}
TEST_F(SBC, ImmediateWithCarryEnabled) {
    cpu[RESET_START] = SBC_IMM;
    cpu[RESET_START + 1] = 0x12;
    cpu.A = 0x30;
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x1D);
}
TEST_F(SBC, ZeroPage) {
    cpu[RESET_START] = SBC_ZPG;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x12] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, ZeroPageX) {
    cpu[RESET_START] = SBC_ZPX;
    cpu[RESET_START + 1] = 0x12;
    cpu[0x22] = 0x40;
    cpu.A = 0x50;
    cpu.X = 0x10;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, Absolute) {
    cpu[RESET_START] = SBC_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, AbsoluteX) {
    // No page cross
    cpu[RESET_START] = SBC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0x10;
    cpu[0x1244] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = SBC_ABX;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.X = 0xFF;
    cpu[0x1333] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, AbsoluteY) {
    // No page cross
    cpu[RESET_START] = SBC_ABY;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.Y = 0x11;
    cpu[0x1245] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x10);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = SBC_ABY;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu.Y = 0xF0;
    cpu[0x1324] = 0x40;
    cpu.A = 0x50;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x10);
}
TEST_F(SBC, IndirectX) {
    cpu[RESET_START] = SBC_IDX;
    cpu[RESET_START + 1] = 0x10;
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
    cpu[RESET_START] = SBC_IDY;
    cpu[RESET_START + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x50;
    cpu[0x1239] = 0x40;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x10);

    // Page cross
    cpu.reset();
    cpu[RESET_START] = SBC_IDY;
    cpu[RESET_START + 1] = 0x10;
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
    cpu[RESET_START] = BIT_ZPG;
    cpu[RESET_START + 1] = 0x15;
    cpu[0x15] = 0xF3;   // 0b 1111 0011
    cpu.A = 0x0C;       // 0b 0000 1100
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
    ASSERT_TRUE(cpu.get_flag_v() == F_NO_OVERFLOW);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Not zero
    cpu.reset();
    cpu[RESET_START] = BIT_ZPG;
    cpu[RESET_START + 1] = 0x15;
    cpu[0x15] = 0xF3;   // 0b 1111 0011
    cpu.A = 0x99;       // 0b 1001 1001
    ASSERT_TRUE(cpu.execute(3) == 3);   // Anded value should be 0b 1001 0001
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_v() == F_NO_OVERFLOW);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(BIT, Absolute) {
    cpu.reset();
    cpu[RESET_START] = BIT_ABS;
    cpu[RESET_START + 1] = 0x15;
    cpu[RESET_START + 2] = 0x14;
    cpu[0x1415] = 0xF3;     // 0b 1111 0011
    cpu.A = 0x99;           // 0b 1001 1001
    ASSERT_TRUE(cpu.execute(4) == 4);   // Anded value should be 0b 1001 0001
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_v() == F_NO_OVERFLOW);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}


// CMP
TEST_F(CMP, Immediate) {
    // Equal
    cpu[RESET_START] = CMP_IMM;
    cpu[RESET_START + 1] = 0x42;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Greater
    cpu.reset();
    cpu[RESET_START] = CMP_IMM;
    cpu[RESET_START + 1] = 0x21;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);

    // Less
    cpu.reset();
    cpu[RESET_START] = CMP_IMM;
    cpu[RESET_START + 1] = 0x42;
    cpu.A = 0x21;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == F_NO_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
// Just test for one result of comparison now
TEST_F(CMP, ZeroPage) {
    cpu[RESET_START] = CMP_ZPG;
    cpu[RESET_START + 1] = 0x51;
    cpu[0x51] = 0x31;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, ZeroPageX) {
    cpu[RESET_START] = CMP_ZPX;
    cpu[RESET_START + 1] = 0x51;
    cpu.X = 0x10;
    cpu[0x61] = 0x31;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, Absolute) {
    cpu[RESET_START] = CMP_ABS;
    cpu[RESET_START + 1] = 0x51;
    cpu[RESET_START + 2] = 0x74;
    cpu[0x7451] = 0x31;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, AbsoluteX) {
    cpu[RESET_START] = CMP_ABX;
    cpu[RESET_START + 1] = 0x51;
    cpu[RESET_START + 2] = 0x74;
    cpu.X = 0x10;
    cpu[0x7461] = 0x31;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, AbsoluteY) {
    cpu[RESET_START] = CMP_ABY;
    cpu[RESET_START + 1] = 0x51;
    cpu[RESET_START + 2] = 0x74;
    cpu.Y = 0x10;
    cpu[0x7461] = 0x31;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, IndirectX) {
    cpu[RESET_START] = CMP_IDX;
    cpu[RESET_START + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;
    cpu.A = 0x42;
    cpu[0x1234] = 0x31;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, IndirectY) {
    // No page cross
    cpu[RESET_START] = CMP_IDY;
    cpu[RESET_START + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;
    cpu.A = 0x42;
    cpu[0x1239] = 0x31;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
// Since CPX and CPY are so similar to CMP, just test a little
TEST_F(CMP, CPX_ImmediateSimple) {
    cpu[RESET_START] = CPX_IMM;
    cpu[RESET_START + 1] = 0x21;
    cpu.X = 0x42;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}
TEST_F(CMP, CPY_ImmediateSimple) {
    cpu[RESET_START] = CPY_IMM;
    cpu[RESET_START + 1] = 0x21;
    cpu.Y = 0x42;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.get_flag_c() == F_YES_CARRY);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_ZERO);
}


// JMP
TEST_F(JMP, Absolute) {
    cpu[RESET_START] = JMP_ABS;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == 0x1234);
}
TEST_F(JMP, Indirect) {
    cpu[RESET_START] = JMP_IND;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1234] = 0x04;
    cpu[0x1235] = 0x40;
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.PC == 0x4004);
}
TEST_F(JMP, IndirectBoundary) {
    cpu[RESET_START] = JMP_IND;
    cpu[RESET_START + 1] = 0xFF;
    cpu[RESET_START + 2] = 0x12;
    cpu[0x1200] = 0xEE;
    cpu[0x12FF] = 0x04;
    cpu[0x1300] = 0x40;
    ASSERT_TRUE(cpu.execute(5) == 5);
#if CPU_MODEL == MODEL_6502
    ASSERT_TRUE(cpu.PC == 0xEE04);
#else
    ASSERT_TRUE(cpu.PC == 0x4004);
#endif
}


// Push stack
TEST_F(PUSH, PHA) {
    cpu[RESET_START] = PHA_IMP;
    cpu.A = 0x42;
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.S == 0xfe);
    ASSERT_TRUE(cpu[0x01ff] == 0x42);
}
TEST_F(PUSH, PHP) {
    cpu[RESET_START] = PHP_IMP;
    cpu.SR = 0b10101010;    // Arbitrary flags
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.S == 0xfe);
    ASSERT_TRUE(cpu[0x01ff] == 0b10101010);
}

// Pull stack
TEST_F(PULL, PLA) {
    // Zero
    cpu[RESET_START] = PLA_IMP;
    cpu.A = 0x11;
    cpu[0x01ff] = 0x0; // In stack
    cpu.S = 0xfe;       // stack pointer in next free location
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.S == 0xff);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.get_flag_z() == F_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Positive
    cpu.reset();
    cpu[RESET_START] = PLA_IMP;
    cpu.A = 0;
    cpu[0x01ff] = 0x42; // In stack
    cpu.S = 0xfe;       // stack pointer in next free location
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.S == 0xff);
    ASSERT_TRUE(cpu.A == 0x42);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NON_NEG);

    // Negative
    cpu.reset();
    cpu[RESET_START] = PLA_IMP;
    cpu.A = 0;
    cpu[0x01ff] = 0xFF; // In stack
    cpu.S = 0xfe;       // stack pointer in next free location
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.S == 0xFF);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.get_flag_z() == F_NON_ZERO);
    ASSERT_TRUE(cpu.get_flag_n() == F_NEG);
}
TEST_F(PULL, PLP) {
    cpu[RESET_START] = PLP_IMP;
    cpu.SR = 0;
    cpu[0x01ff] = 0x42; // In stack
    cpu.S = 0xfe;       // stack pointer in next free location
    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.S == 0xff);
    ASSERT_TRUE(cpu.SR == 0x42);
}


// Branching instructions
TEST_F(BRANCH, BCC_BranchFail) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BCC_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.PC == start + 2);
}
TEST_F(BRANCH, BCC_BranchPassSamePage) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BCC_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_c(0);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BCC_BranchPassDifferentPage) {
    u16 start = 0x10F0;
    cpu.PC = start;
    cpu[start] = BCC_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_c(0);
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BCC_BranchBackwards) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BCC_REL;
    cpu[start + 1] = 0x80;  // -128 == -0x80
    cpu.set_flag_c(0);
    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.PC == start - 128);
}
// Other branching instructions just single branch
TEST_F(BRANCH, BCS_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BCS_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_c(1);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BEQ_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BEQ_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_z(1);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BMI_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BMI_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_n(1);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BNE_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BNE_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_z(0);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BPL_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BPL_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_n(0);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BVC_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BVC_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_v(0);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}
TEST_F(BRANCH, BVS_REL) {
    u16 start = 0x1000;
    cpu.PC = start;
    cpu[start] = BVS_REL;
    cpu[start + 1] = 0x7F;  // +127 == +0x7F
    cpu.set_flag_v(1);
    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.PC == start + 127);
}


// Subroutine test
TEST_F(SUBROUTINE, SubroutineJumpToAndReturnFrom) {
    cpu.PC = 0x4000;
    // Jump to subroutine
    cpu[0x4000] = JSR_ABS;
    cpu[0x4001] = 0x34;
    cpu[0x4002] = 0x12;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.PC == 0x1234);
    // subroutine doesn't do anything except return
    cpu[0x1234] = NOP_IMP;
    cpu[0x1235] = NOP_IMP;
    cpu[0x1236] = RTS_IMP;
    ASSERT_TRUE(cpu.execute(10) == 10);
    ASSERT_TRUE(cpu.PC == 0x4003);
}


// Interrupt
TEST_F(INTERRUPT, BRK_and_RTI) {
    // Set interrupt handler location
    cpu[INT_VEC_LOC] = 0x34;
    cpu[INT_VEC_LOC + 1] = 0x12;

    // Generate interrupt
    cpu[RESET_START] = BRK_IMP;
    ASSERT_TRUE(cpu.execute(7) == 7);
    ASSERT_TRUE(cpu.PC == 0x1234);

    // Do something in interrupt and return
    cpu[0x1234] = NOP_IMP;
    cpu[0x1235] = NOP_IMP;
    ASSERT_TRUE(cpu.execute(4) == 4);
    cpu[0x1236] = RTI_IMP;
    ASSERT_TRUE(cpu.execute(6) == 6);
    ASSERT_TRUE(cpu.PC == RESET_START);  // Should return to original location
}


// Tests from http://www.obelisk.me.uk/6502/algorithms.html
TEST_F(OBELISK_TESTS, ObelistTestSimpleMemoryOperationsClear16bits) {
    cpu[RESET_START] = LDA_IMM;
    cpu[RESET_START + 1] = 0;
    cpu[RESET_START + 2] = STA_ZPG;
    cpu[RESET_START + 3] = 0x10;
    cpu[RESET_START + 4] = STA_ZPG;
    cpu[RESET_START + 5] = 0x11;

    for (u16 addr = 0x0; addr < 0x100; addr++) {
        cpu[addr] = 0xff;
    }

    ASSERT_TRUE(cpu.execute(2 + 3 + 3) == 2 + 3 + 3);
    ASSERT_TRUE(cpu[0x10] == 0);
    ASSERT_TRUE(cpu[0x11] == 0);
}
TEST_F(OBELISK_TESTS, ObelistTestSimpleMemoryOperationsClear32bits) {
    cpu[RESET_START] = LDA_IMM;
    cpu[RESET_START + 1] = 0;
    cpu[RESET_START + 2] = STA_ZPG;
    cpu[RESET_START + 3] = 0x10;
    cpu[RESET_START + 4] = STA_ZPG;
    cpu[RESET_START + 5] = 0x11;
    cpu[RESET_START + 6] = STA_ZPG;
    cpu[RESET_START + 7] = 0x12;
    cpu[RESET_START + 8] = STA_ZPG;
    cpu[RESET_START + 9] = 0x13;

    for (u16 addr = 0x0; addr < 0x100; addr++) {
        cpu[addr] = 0xff;
    }

    ASSERT_TRUE(cpu.execute(2 + 3 + 3 + 3 + 3) == 2 + 3 + 3 + 3 + 3);
    ASSERT_TRUE(cpu[0x10] == 0);
    ASSERT_TRUE(cpu[0x11] == 0);
    ASSERT_TRUE(cpu[0x12] == 0);
    ASSERT_TRUE(cpu[0x13] == 0);
}
TEST_F(OBELISK_TESTS, ObelistTestSimpleMemoryOperationsSetWord) {
    cpu[RESET_START] = LDA_IMM;
    cpu[RESET_START + 1] = 0x34;
    cpu[RESET_START + 2] = STA_ZPG;
    cpu[RESET_START + 3] = 0x10;
    cpu[RESET_START + 4] = LDA_IMM;
    cpu[RESET_START + 5] = 0x12;
    cpu[RESET_START + 6] = STA_ZPG;
    cpu[RESET_START + 7] = 0x11;

    for (u16 addr = 0x0; addr < 0x100; addr++) {
        cpu[addr] = 0xff;
    }

    ASSERT_TRUE(cpu.execute(2 + 3 + 2 + 3) == 2 + 3 + 2 + 3);
    ASSERT_TRUE(cpu[0x10] == 0x34);
    ASSERT_TRUE(cpu[0x11] == 0x12);
}
TEST_F(OBELISK_TESTS, ObelistTest_EOR_Complement) {
    cpu.A = 0xF0;
    cpu[RESET_START] = EOR_IMM;
    cpu[RESET_START + 1] = 0xFF;
    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0F);
}
TEST_F(OBELISK_TESTS, ObelistTest_Shift16Left) {
    cpu[RESET_START] = ASL_ZPG;
    cpu[RESET_START + 1] = 0x10;
    cpu[RESET_START + 2] = ROL_ZPG;
    cpu[RESET_START + 3] = 0x11;

    cpu[0x10] = 0xF4;
    cpu[0x11] = 0xC0;
    ASSERT_TRUE(cpu.execute(5 + 5) == 5 + 5);
    ASSERT_TRUE(cpu[0x10] == 0xE8);
    ASSERT_TRUE(cpu[0x11] == 0x81);
}
TEST_F(OBELISK_TESTS, ObelistTest_Shift16Right) {
    cpu[RESET_START] = LSR_ZPG;
    cpu[RESET_START + 1] = 0x11;
    cpu[RESET_START + 2] = ROR_ZPG;
    cpu[RESET_START + 3] = 0x10;

    cpu[0x10] = 0x00;
    cpu[0x11] = 0xFF;
    ASSERT_TRUE(cpu.execute(5 + 5) == 5 + 5);
    ASSERT_TRUE(cpu[0x10] == 0x80);
    ASSERT_TRUE(cpu[0x11] == 0x7F);
}
TEST_F(OBELISK_TESTS, ObelistTest_Divide16bitSignedValueBy2) {
    cpu[0x11] = 0xF1; // F108 = -3832. -3832 / 2 = -1916 = F884
    cpu[0x10] = 0x08;
    cpu[RESET_START] = LDA_ZPG;
    cpu[RESET_START + 1] = 0x11;
    cpu[RESET_START + 2] = ASL_ACC;
    cpu[RESET_START + 3] = ROR_ZPG;
    cpu[RESET_START + 4] = 0x11;
    cpu[RESET_START + 5] = ROR_ZPG;
    cpu[RESET_START + 6] = 0x10;

    ASSERT_TRUE(cpu.execute(3 + 2 + 5 + 5) == 3 + 2 + 5 + 5);
    ASSERT_TRUE(cpu[0x11] == 0xF8);
    ASSERT_TRUE(cpu[0x10] == 0x84);
}
