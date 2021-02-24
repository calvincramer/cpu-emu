#include <gtest/gtest.h>
#include "mos6502.hpp"

using namespace mos6502;

// #define UNCHANGED(x) cpu.#x == cpuOrig.#x

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

// Derive above fixtures to categorize test cases in groups
class Api : public SetupCPU_F {};
class LDA : public SetupCPU_F {};

TEST_F(Api, Reset) { cpu.reset(); }
TEST_F(Api, Execute) { cpu.execute(0); }

auto common_execute = [] (CPU& cpu, u32 runCycles, u32 expCycles, u8 expA, u1 expZ, u1 expN) {
    ASSERT_TRUE(cpu.execute(runCycles) == expCycles);
    ASSERT_TRUE(cpu.A == expA);
    ASSERT_TRUE(cpu.SR.Z == expZ);
    ASSERT_TRUE(cpu.SR.N == expN);
};

TEST_F(LDA, ImmediateZero) {
    cpu[RESET_LOC] = LDA_IMM;
    cpu[RESET_LOC + 1] = 0x0;

    common_execute(cpu, 2, 2, 0, F_ZERO, F_NON_NEG);
}
TEST_F(LDA, ImmediateNonZeroPositive) {
    cpu[RESET_LOC] = LDA_IMM;
    cpu[RESET_LOC + 1] = 0x42;

    common_execute(cpu, 2, 2, 0x42, F_NON_ZERO, F_NON_NEG);
}
TEST_F(LDA, ImmediateNegative) {
    cpu[RESET_LOC] = LDA_IMM;
    cpu[RESET_LOC + 1] = 0xFF;

    common_execute(cpu, 2, 2, 0xFF, F_NON_ZERO, F_NEG);
}


TEST_F(LDA, ZeroPageZero) {
    cpu[RESET_LOC] = LDA_ZPG;
    cpu[RESET_LOC + 1] = 0x50;
    cpu[0x50] = 0x0;

    common_execute(cpu, 3, 3, 0x0, F_ZERO, F_NON_NEG);
}
TEST_F(LDA, ZeroPagePositive) {
    cpu[RESET_LOC] = LDA_ZPG;
    cpu[RESET_LOC + 1] = 0x50; 
    cpu[0x50] = 0x12;

    common_execute(cpu, 3, 3, 0x12, F_NON_ZERO, F_NON_NEG);
}
TEST_F(LDA, ZeroPageNegative) {
    cpu[RESET_LOC] = LDA_ZPG;
    cpu[RESET_LOC + 1] = 0x50; 
    cpu[0x50] = 0xFF;

    common_execute(cpu, 3, 3, 0xFF, F_NON_ZERO, F_NEG);
}



TEST_F(LDA, ZeroPageXZero) {
    cpu[RESET_LOC] = LDA_ZPX;
    cpu[RESET_LOC + 1] = 0x50;
    cpu.X = 0xA;
    cpu[0x5A] = 0x0;

    common_execute(cpu, 4, 4, 0x0, F_ZERO, F_NON_NEG);
}
TEST_F(LDA, ZeroPageXPositive) {
    cpu[RESET_LOC] = LDA_ZPX;
    cpu[RESET_LOC + 1] = 0x50;
    cpu.X = 0xA;
    cpu[0x5A] = 0x12;

    common_execute(cpu, 4, 4, 0x12, F_NON_ZERO, F_NON_NEG);
}
TEST_F(LDA, ZeroPageXNegative) {
    cpu[RESET_LOC] = LDA_ZPX;
    cpu[RESET_LOC + 1] = 0x50;
    cpu.X = 0xA;
    cpu[0x5A] = 0xFF;

    common_execute(cpu, 4, 4, 0xFF, F_NON_ZERO, F_NEG);
}
TEST_F(LDA, ZeroPageXWrap) {
    cpu[RESET_LOC] = LDA_ZPX;
    cpu[RESET_LOC + 1] = 0xF0;
    cpu.X = 0x20;
    cpu[0x10] = 0x42;

    common_execute(cpu, 4, 4, 0x42, F_NON_ZERO, F_NON_NEG);
}


TEST_F(LDA, AbsoluteZero) {
    cpu[RESET_LOC] = LDA_ABS;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4321] = 0x0;

    common_execute(cpu, 4, 4, 0x0, F_ZERO, F_NON_NEG);
}
TEST_F(LDA, AbsolutePositive) {
    cpu[RESET_LOC] = LDA_ABS;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4321] = 0x1;

    common_execute(cpu, 4, 4, 0x1, F_NON_ZERO, F_NON_NEG);
}
TEST_F(LDA, AbsoluteNegative) {
    cpu[RESET_LOC] = LDA_ABS;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4321] = 0xFF;

    common_execute(cpu, 4, 4, 0xFF, F_NON_ZERO, F_NEG);
}


TEST_F(LDA, AbsoluteXZero) {
    cpu[RESET_LOC] = LDA_ABX;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0x0;
    cpu.X = 5;

    common_execute(cpu, 4, 4, 0x0, F_ZERO, F_NON_NEG);
}
TEST_F(LDA, AbsoluteXPositive) {
    cpu[RESET_LOC] = LDA_ABX;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0x7;
    cpu.X = 5;

    common_execute(cpu, 4, 4, 0x7, F_NON_ZERO, F_NON_NEG);
}
TEST_F(LDA, AbsoluteXNegative) {
    cpu[RESET_LOC] = LDA_ABX;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0xF0;
    cpu.X = 5;

    common_execute(cpu, 4, 4, 0xF0, F_NON_ZERO, F_NEG);
}
TEST_F(LDA, AbsoluteXPageCross) {
    cpu[RESET_LOC] = LDA_ABX;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4411] = 0x42;
    cpu.X = 0xF0;

    common_execute(cpu, 5, 5, 0x42, F_NON_ZERO, F_NON_NEG);
}


TEST_F(LDA, AbsoluteYZero) {
    cpu[RESET_LOC] = LDA_ABY;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0x0;
    cpu.Y = 5;

    common_execute(cpu, 4, 4, 0x0, F_ZERO, F_NON_NEG);
}
TEST_F(LDA, AbsoluteYPositive) {
    cpu[RESET_LOC] = LDA_ABY;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0x7;
    cpu.Y = 5;

    common_execute(cpu, 4, 4, 0x7, F_NON_ZERO, F_NON_NEG);
}
TEST_F(LDA, AbsoluteYNegative) {
    cpu[RESET_LOC] = LDA_ABY;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0xF0;
    cpu.Y = 5;

    common_execute(cpu, 4, 4, 0xF0, F_NON_ZERO, F_NEG);
}
TEST_F(LDA, AbsoluteYPageCross) {
    cpu[RESET_LOC] = LDA_ABY;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4411] = 0x42;
    cpu.Y = 0xF0;

    common_execute(cpu, 5, 5, 0x42, F_NON_ZERO, F_NON_NEG);
}


TEST_F(LDA, IndexedIndirectZero) {
    cpu[RESET_LOC] = LDA_IDX;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0x0;

    common_execute(cpu, 6, 6, 0x0, F_ZERO, F_NON_NEG);
}
TEST_F(LDA, IndexedIndirectPositive) {
    cpu[RESET_LOC] = LDA_IDX;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0x42;

    common_execute(cpu, 6, 6, 0x42, F_NON_ZERO, F_NON_NEG);
}
TEST_F(LDA, IndexedIndirectNegative) {
    cpu[RESET_LOC] = LDA_IDX;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.X = 0x5;
    cpu[0x0015] = 0x34;
    cpu[0x0016] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0xFF;

    common_execute(cpu, 6, 6, 0xFF, F_NON_ZERO, F_NEG);
}
TEST_F(LDA, IndexedIndirectWrapZeroPage) {
    cpu[RESET_LOC] = LDA_IDX;
    cpu[RESET_LOC + 1] = 0x30;
    cpu.X = 0xF0;
    cpu[0x0020] = 0x34;
    cpu[0x0021] = 0x12;     // Should load from 0x1234
    cpu[0x1234] = 0x31;

    common_execute(cpu, 6, 6, 0x31, F_NON_ZERO, F_NON_NEG);
}


TEST_F(LDA, IndirectIndexedZero) {
    cpu[RESET_LOC] = LDA_IDY;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should load from 0x1234 + 0x5
    cpu[0x1239] = 0x0;

    common_execute(cpu, 5, 5, 0x0, F_ZERO, F_NON_NEG);
}
TEST_F(LDA, IndirectIndexedPositive) {
    cpu[RESET_LOC] = LDA_IDY;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should load from 0x1234 + 0x5
    cpu[0x1239] = 0x42;

    common_execute(cpu, 5, 5, 0x42, F_NON_ZERO, F_NON_NEG);
}
TEST_F(LDA, IndirectIndexedNegative) {
    cpu[RESET_LOC] = LDA_IDY;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0x5;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should load from 0x1234 + 0x5
    cpu[0x1239] = 0xFF;

    common_execute(cpu, 5, 5, 0xFF, F_NON_ZERO, F_NEG);
}
TEST_F(LDA, IndirectIndexedPageCross) {
    cpu[RESET_LOC] = LDA_IDY;
    cpu[RESET_LOC + 1] = 0x10;
    cpu.Y = 0xFF;
    cpu[0x0010] = 0x34;
    cpu[0x0011] = 0x12;     // Should load from 0x1234 + 0xFF
    cpu[0x1333] = 0xee;

    common_execute(cpu, 6, 6, 0xee, F_NON_ZERO, F_NEG);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
