#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"
#include "test.hpp"

using namespace mos6502;

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
