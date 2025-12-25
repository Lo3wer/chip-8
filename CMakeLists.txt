cmake_minimum_required(VERSION 3.16)
project(chip8)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

# Create your game executable target with all source files
add_executable(chip8 WIN32 
    main.c
    cpu.c
    display.c
)

# Link to the actual SDL3 library.
target_link_libraries(chip8 PRIVATE SDL3::SDL3)

# --------------------
# Unit tests (CPU only)
# --------------------
enable_testing()

# Minimal unit test target: uses a stub SDL header/impl to avoid SDL dependency
add_executable(chip8_tests
    tests/test_cpu.c
    cpu.c
)

# Make headers (cpu.h, display.h) visible and prefer tests/ includes first
target_include_directories(chip8_tests PRIVATE ${CMAKE_SOURCE_DIR})

add_test(NAME cpu_opcodes COMMAND chip8_tests)