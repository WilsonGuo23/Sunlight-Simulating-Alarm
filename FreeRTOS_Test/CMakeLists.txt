# Generated Cmake Pico project file

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

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(FreeRTOS_Test C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(FreeRTOS_Test
    app/FreeRTOS_Test.c
    app/system_init.c

    FreeRTOS-Kernel/tasks.c
    FreeRTOS-Kernel/queue.c
    FreeRTOS-Kernel/list.c
    FreeRTOS-Kernel/timers.c

    FreeRTOS-Kernel/portable/MemMang/heap_4.c

    FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/port.c
)

target_sources(FreeRTOS_Test PRIVATE
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

pico_set_program_name(FreeRTOS_Test "FreeRTOS_Test")
pico_set_program_version(FreeRTOS_Test "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(FreeRTOS_Test 0)
pico_enable_stdio_usb(FreeRTOS_Test 1)

# Add the standard library to the build
target_link_libraries(FreeRTOS_Test
        pico_stdlib
        hardware_exception)

# Add the standard include files to the build
target_include_directories(FreeRTOS_Test PRIVATE
        .
        app
        drivers
    FreeRTOS-Kernel/include
    FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/include
)

if (ENABLE_DRIVER_TESTS)
    target_sources(FreeRTOS_Test PRIVATE
        app/driver_test.c
        drivers/buttons_driver.c
        drivers/display_driver.c
        drivers/triac_driver.c
    )
    target_compile_definitions(FreeRTOS_Test PRIVATE ENABLE_DRIVER_TESTS)
endif()

# Add any user requested libraries
target_link_libraries(FreeRTOS_Test 
        
        )

pico_add_extra_outputs(FreeRTOS_Test)

