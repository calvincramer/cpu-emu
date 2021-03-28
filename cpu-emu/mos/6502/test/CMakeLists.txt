# Test executable from sources
add_executable(tests-6502
    test.cpp
    test_Load.cpp
    test_Store.cpp
    test_TRANSFER.cpp
    test_NOP.cpp
    test_CLEAR_FLAGS.cpp
    test_SET_FLAGS.cpp
    test_INC_DEC.cpp
    test_AND.cpp
    test_EOR.cpp
    test_ORA.cpp
    test_ASL.cpp
    test_LSR.cpp
    test_ROL.cpp
    test_ROR.cpp
    test_ADC.cpp
    test_SBC.cpp
    test_BIT.cpp
    test_CMP.cpp
    test_JMP.cpp
    test_Stack.cpp
    test_BRANCH.cpp
    test_INTERRUPT.cpp
    test_OBELISK_TESTS.cpp
)

# Link executable with mos-6502 archive, and pthread archive
target_link_libraries(tests-6502 mos-6502 ${GTEST_LIBRARIES} pthread)

# Include directory search path
target_include_directories(tests-6502 PRIVATE ../include)
