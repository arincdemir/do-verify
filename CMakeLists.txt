cmake_minimum_required(VERSION 3.15)
project(do-verify VERSION 1.0.0 LANGUAGES CXX)

option(ENABLE_COVERAGE "Enable code coverage" OFF)

if(ENABLE_COVERAGE)
    # Add flags for GCC/Clang
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

# Standard Setup
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the Header-Only Library
# "INTERFACE" means "I have no .cpp files to compile, just headers"
add_library(do-verify INTERFACE)

# Tell CMake where the headers are
target_include_directories(do-verify INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Fetch Catch2 (ONCE for the whole project)
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# Add Subdirectories
enable_testing()
add_subdirectory(tests)
add_subdirectory(benchmarks)