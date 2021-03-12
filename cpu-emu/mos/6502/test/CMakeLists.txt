# Link runTests with what we want to test and the GTest and pthread library
add_executable(tests-6502 6502_test.cpp)
target_link_libraries(tests-6502 mos-6502 ${GTEST_LIBRARIES} pthread)
target_include_directories(tests-6502 PRIVATE ../include)