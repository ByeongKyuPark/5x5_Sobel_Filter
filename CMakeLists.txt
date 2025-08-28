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
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address")
elseif(MSVC)
    add_compile_options(/W4)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

# Include directories
include_directories(include)

# Core library
add_library(sobel_core
    src/image.cpp
    src/image_io.cpp
)

# Main executable (will implement gradually)
add_executable(sobel_filter
    src/main.cpp
)

# Link core library to executable
target_link_libraries(sobel_filter sobel_core)

# Test data generator (optional utility)
add_executable(test_data_generator
    examples/test_data_generator.cpp
)

# Enable testing
enable_testing()

# Status messages
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# TODO: Add library target and tests as we implement them