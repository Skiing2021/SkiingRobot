cmake_minimum_required(VERSION 3.17)

# Find K210 SDK
FIND_PATH(SDK_ROOT lib/drivers/kpu.c HINTS $ENV{K210_SDK_PATH} REQUIRED)
if (NOT SDK_ROOT)
    MESSAGE(FATAL_ERROR "Could not found K210 SDK, please specific SDK path in environment variable K210_SDK_PATH")
endif()
MESSAGE(STATUS "Found K210 SDK at ${SDK_ROOT}")

# Find K210 RISC-V Toolchain
FIND_PATH(TOOLCHAIN make make.exe HINTS $ENV{TOOLCHAIN} REQUIRED)
if (NOT TOOLCHAIN)
    MESSAGE(FATAL_ERROR "Could not found K210 Toolchain, please specific Toolchain path in environment variable TOOLCHAIN")
endif()
MESSAGE(STATUS "Found K210 Toolchain at ${TOOLCHAIN}")

include(${SDK_ROOT}/cmake/common.cmake)
project(SkiingRobot VERSION 0.1 LANGUAGES C CXX ASM)

# config headers
include(${SDK_ROOT}/cmake/macros.internal.cmake)
header_directories(${K210_SDK_PATH}/lib)

# compile project
add_source_files(*.c)
include(executable.cmake)