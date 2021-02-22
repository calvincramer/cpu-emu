# Library
add_library (mos-6502 6502.cpp)
target_include_directories(mos-6502 PRIVATE ../include)

# Main executable
# add_executable(mos-6502 6502.cpp)
# target_include_directories(mos-6502 PRIVATE ../include)