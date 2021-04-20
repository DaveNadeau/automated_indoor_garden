# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "C:/Users/daven/Documents/Pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(watering_system C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(watering_system watering_system.c )

pico_set_program_name(watering_system "watering_system")
pico_set_program_version(watering_system "0.1")

pico_enable_stdio_uart(watering_system 1)
pico_enable_stdio_usb(watering_system 0)

# Add the standard library to the build
target_link_libraries(watering_system pico_stdlib)

pico_add_extra_outputs(watering_system)

