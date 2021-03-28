#pragma once

#include <gtest/gtest.h>

#include "mos6502.hpp"
#include "models.hpp"

// Values for flags
#define F_ZERO 1
#define F_NON_ZERO 0

#define F_NEG 1
#define F_NON_NEG 0

#define F_YES_CARRY 1
#define F_NO_CARRY 0

#define F_YES_OVERFLOW 1
#define F_NO_OVERFLOW 0

using namespace mos6502;

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
