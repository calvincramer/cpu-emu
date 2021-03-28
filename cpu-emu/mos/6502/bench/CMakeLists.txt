# Benchmark executable
add_executable(bench-6502 bench.cpp)

# Link executable with mos-6502 archive
target_link_libraries(bench-6502 mos-6502)

# Include directory search path
target_include_directories(bench-6502 PRIVATE ../include)
