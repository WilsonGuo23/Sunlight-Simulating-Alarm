cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location

# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==

if(WIN32)
set(USERHOME $ENV{USERPROFILE})
else()
set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)

set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
include(${picoVscode})
endif()

# ====================================================================================

set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK

include(pico_sdk_import.cmake)

project(Sunlight_Alarm_Code C CXX ASM)

# Initialise the Raspberry Pi Pico SDK

pico_sdk_init()

# Main executable target

add_executable(main
app/main.c
app/system_init.c


FreeRTOS-Kernel/tasks.c
FreeRTOS-Kernel/queue.c
FreeRTOS-Kernel/list.c
FreeRTOS-Kernel/timers.c

FreeRTOS-Kernel/portable/MemMang/heap_4.c
FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/port.c


)

target_sources(main PRIVATE
drivers/buttons_driver.c
drivers/display_driver.c
drivers/triac_driver.c


services/time_service.c
services/display_service.c
services/input_service.c
services/alarm_service.c
services/ui_service.c
services/sunrise_service.c


)

option(ENABLE_DRIVER_TESTS "Build the driver test harness" OFF)

pico_set_program_name(main "main")
pico_set_program_version(main "0.1")

# Enable USB stdio, disable UART stdio

pico_enable_stdio_uart(main 0)
pico_enable_stdio_usb(main 1)

# Include directories

target_include_directories(main PRIVATE
.
app
drivers
services
FreeRTOS-Kernel/include
FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/include
)

# Driver test harness

if (ENABLE_DRIVER_TESTS)
target_sources(main PRIVATE
app/driver_test.c
drivers/buttons_driver.c
drivers/display_driver.c
drivers/triac_driver.c
)


target_compile_definitions(main PRIVATE
    ENABLE_DRIVER_TESTS
)


endif()

# Libraries

target_link_libraries(main
pico_stdlib
hardware_exception
)

# Generate UF2, BIN, ELF, MAP, etc.

pico_add_extra_outputs(main)
