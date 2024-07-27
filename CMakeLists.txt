cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

project(clocks_study C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.c
    clocks.h
    clocks.c
    rosc.c
    rosc.h
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib 
    hardware_pwm
    hardware_clocks
    hardware_rtc
    pico_util
)

pico_add_extra_outputs(${PROJECT_NAME})

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)