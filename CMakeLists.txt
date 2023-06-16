# Generated cmake Pico project file.
cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Project details.
set(PROJECT UniLabPico)
set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)

# Directories.
set(BUILD "${CMAKE_SOURCE_DIR}/build")

# Include build functions from Pico SDK
include(pico_sdk_import.cmake)

# Create project.
project(${PROJECT} C CXX ASM)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Make project data accessible to compiler
add_compile_definitions(PROJECT=${PROJECT})
add_compile_definitions(MAJOR_VERSION=${MAJOR_VERSION})
add_compile_definitions(MINOR_VERSION=${MINOR_VERSION})

# Create executable.
add_executable(${PROJECT})

# Specify sources for the executable.
target_sources(${PROJECT} PRIVATE
    src/${PROJECT}.cpp
    src/TCA9548.cpp
    src/MS5837.cpp
)

target_include_directories(${PROJECT} PRIVATE
    include
)

# pull in common dependencies
target_link_libraries(${PROJECT} 
    pico_stdlib
    hardware_i2c 
    hardware_timer
)

# enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT} 1)
pico_enable_stdio_uart(${PROJECT} 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${PROJECT})
