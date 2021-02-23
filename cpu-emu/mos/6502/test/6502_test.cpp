#include <gtest/gtest.h>
#include "mos6502.hpp"

using namespace mos6502;

#define UNCHANGED(x) cpu.#x == cpuOrig.#x

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

TEST_F(LDA, ImmediateZero) {
    cpu[RESET_LOC] = LDA_IMM;
    cpu[RESET_LOC + 1] = 0x0;

    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.Z == 1);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, ImmediateNonZeroPositive) {
    cpu[RESET_LOC] = LDA_IMM;
    cpu[RESET_LOC + 1] = 0x42;

    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0x42);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, ImmediateNegative) {
    cpu[RESET_LOC] = LDA_IMM;
    cpu[RESET_LOC + 1] = 0xFF;

    ASSERT_TRUE(cpu.execute(2) == 2);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 1);
}

TEST_F(LDA, ZeroPageZero) {
    cpu[RESET_LOC] = LDA_ZPG;
    cpu[RESET_LOC + 1] = 0x50;
    cpu[0x50] = 0x0;

    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.Z == 1);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, ZeroPagePositive) {
    cpu[RESET_LOC] = LDA_ZPG;
    cpu[RESET_LOC + 1] = 0x50; 
    cpu[0x50] = 0x12;

    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0x12);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, ZeroPageNegative) {
    cpu[RESET_LOC] = LDA_ZPG;
    cpu[RESET_LOC + 1] = 0x50; 
    cpu[0x50] = 0xFF;

    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 1);
}

TEST_F(LDA, ZeroPageXZero) {
    cpu[RESET_LOC] = LDA_ZPX;
    cpu[RESET_LOC + 1] = 0x50;
    cpu.X = 0xA;
    cpu[0x5A] = 0x0;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.Z == 1);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, ZeroPageXPositive) {
    cpu[RESET_LOC] = LDA_ZPX;
    cpu[RESET_LOC + 1] = 0x50;
    cpu.X = 0xA;
    cpu[0x5A] = 0x12;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x12);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, ZeroPageXNegative) {
    cpu[RESET_LOC] = LDA_ZPX;
    cpu[RESET_LOC + 1] = 0x50;
    cpu.X = 0xA;
    cpu[0x5A] = 0xFF;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 1);
}

TEST_F(LDA, ZeroPageXWrap) {
    cpu[RESET_LOC] = LDA_ZPX;
    cpu[RESET_LOC + 1] = 0xF0;
    cpu.X = 0x20;
    cpu[0x10] = 0x42;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x42);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, AbsoluteZero) {
    cpu[RESET_LOC] = LDA_ABS;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4321] = 0x0;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.Z == 1);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, AbsolutePositive) {
    cpu[RESET_LOC] = LDA_ABS;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4321] = 0x1;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x1);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, AbsoluteNegative) {
    cpu[RESET_LOC] = LDA_ABS;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4321] = 0xFF;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 1);
}

TEST_F(LDA, AbsoluteXZero) {
    cpu[RESET_LOC] = LDA_ABX;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0x0;
    cpu.X = 5;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.Z == 1);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, AbsoluteXPositive) {
    cpu[RESET_LOC] = LDA_ABX;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0x7;
    cpu.X = 5;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x7);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, AbsoluteXNegative) {
    cpu[RESET_LOC] = LDA_ABX;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0xF0;
    cpu.X = 5;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0xF0);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 1);
}

TEST_F(LDA, AbsoluteXPageCross) {
    cpu[RESET_LOC] = LDA_ABX;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4411] = 0x42;
    cpu.X = 0xF0;

    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x42);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 0);
}


TEST_F(LDA, AbsoluteYZero) {
    cpu[RESET_LOC] = LDA_ABY;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0x0;
    cpu.Y = 5;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.Z == 1);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, AbsoluteYPositive) {
    cpu[RESET_LOC] = LDA_ABY;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0x7;
    cpu.Y = 5;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0x7);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, AbsoluteYNegative) {
    cpu[RESET_LOC] = LDA_ABY;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4326] = 0xF0;
    cpu.Y = 5;

    ASSERT_TRUE(cpu.execute(4) == 4);
    ASSERT_TRUE(cpu.A == 0xF0);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 1);
}

TEST_F(LDA, AbsoluteYPageCross) {
    cpu[RESET_LOC] = LDA_ABY;
    cpu[RESET_LOC + 1] = 0x21;
    cpu[RESET_LOC + 2] = 0x43; // Addr is 0x4321
    cpu[0x4411] = 0x42;
    cpu.Y = 0xF0;

    ASSERT_TRUE(cpu.execute(5) == 5);
    ASSERT_TRUE(cpu.A == 0x42);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
