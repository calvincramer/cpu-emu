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
    cpu[RESET_LOC + 1] = 0x50;  // Zero page addr
    cpu[0x50] = 0x0;

    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0x0);
    ASSERT_TRUE(cpu.SR.Z == 1);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, ZeroPagePositive) {
    cpu[RESET_LOC] = LDA_ZPG;
    cpu[RESET_LOC + 1] = 0x50;  // Zero page addr
    cpu[0x50] = 0x12;

    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0x12);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 0);
}

TEST_F(LDA, ZeroPageNegative) {
    cpu[RESET_LOC] = LDA_ZPG;
    cpu[RESET_LOC + 1] = 0x50;  // Zero page addr
    cpu[0x50] = 0xFF;

    ASSERT_TRUE(cpu.execute(3) == 3);
    ASSERT_TRUE(cpu.A == 0xFF);
    ASSERT_TRUE(cpu.SR.Z == 0);
    ASSERT_TRUE(cpu.SR.N == 1);
}



int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
