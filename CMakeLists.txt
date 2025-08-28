cmake_minimum_required(VERSION 3.12)
project(SobelFilter 
    VERSION 1.0.0 
    DESCRIPTION "5x5 Sobel Edge Detection for 640x640 RGB Images"
    LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific flags for quality and performance
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -msse4.1 -mavx2")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address")
elseif(MSVC)
    add_compile_options(/W4)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG /arch:AVX2")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# Enable SIMD optimizations for on-device performance
add_compile_definitions(ENABLE_SIMD=1)

# Include directories
include_directories(include)

# Core library
add_library(sobel_core
    src/image.cpp
    src/image_io.cpp
    src/sobel_filter.cpp
    src/sobel_filter_simd.cpp
)

# Main executable (will implement gradually)
add_executable(sobel_filter
    src/main.cpp
)

# Link core library to executable
target_link_libraries(sobel_filter sobel_core)

# SIMD benchmark executable
add_executable(benchmark_simd
    src/benchmark_simd.cpp
)

target_link_libraries(benchmark_simd sobel_core)

# Validation test executable
add_executable(validation_test
    src/validation_test.cpp
)

target_link_libraries(validation_test sobel_core)

# Debug test executable
add_executable(debug_test
    src/debug_test.cpp
)

target_link_libraries(debug_test sobel_core)

# Test data generator (optional utility)
add_executable(test_data_generator
    examples/test_data_generator.cpp
)

target_link_libraries(test_data_generator sobel_core)

# Edge result analyzer
add_executable(edge_analyzer
    src/edge_analyzer.cpp
)

target_link_libraries(edge_analyzer sobel_core)

# Quick complete test
add_executable(quick_test
    src/quick_test.cpp
)

target_link_libraries(quick_test sobel_core)

# RAW to BMP converter
add_executable(raw_to_bmp
    src/raw_to_bmp.cpp
)

target_link_libraries(raw_to_bmp sobel_core)

# Enable testing
enable_testing()

# Status messages
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# TODO: Add library target and tests as we implement them