cmake_minimum_required(VERSION 3.12)

# set(PICO_SDK_PATH "~/pico/pico-sdk")
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
if (NOT PICO_SDK_PATH)
    message(FATAL_ERROR
            "SDK location was not specified. Please set PICO_SDK_PATH."
            )
endif ()

get_filename_component(PICO_SDK_PATH "${PICO_SDK_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
if (NOT EXISTS ${PICO_SDK_PATH})
    message(FATAL_ERROR "Directory '${PICO_SDK_PATH}' not found")
endif ()

set(PICO_SDK_INIT_CMAKE_FILE ${PICO_SDK_PATH}/pico_sdk_init.cmake)
if (NOT EXISTS ${PICO_SDK_INIT_CMAKE_FILE})
    message(FATAL_ERROR "Directory '${PICO_SDK_PATH}' does not appear to contain the Raspberry Pi Pico SDK")
endif ()

set(PICO_SDK_PATH ${PICO_SDK_PATH} CACHE PATH "Path to the Raspberry Pi Pico SDK" FORCE)

include(${PICO_SDK_INIT_CMAKE_FILE})
# Initialize the SDK

project(picoC64USB C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

set(FAMILY rp2040)
set(BOARD pico_sdk)
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(TINYUSB_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/lib/tinyusb/src)

add_executable(main src/pins.h src/debug.h src/config.h src/xbone_pad/xbone_pad.c src/main.h)
file(GLOB_RECURSE TINYUSB_SRC CONFIGURE_DEPENDS
    "${TINYUSB_SRC_DIR}/*.c"
)
target_sources(main PUBLIC
        ${SRC_DIR}/main.c
        ${SRC_DIR}/xbone_pad/xbone_pad.c
        ${SRC_DIR}/hid/hid.c
        ${TINYUSB_SRC}
        )
# Make sure TinyUSB can find tusb_config.h
target_include_directories(main PUBLIC ${SRC_DIR})
target_include_directories(main PUBLIC ${TINYUSB_SRC_DIR})
target_link_libraries(main PUBLIC pico_stdlib hardware_pwm)

#add_compile_definitions(TUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use of GPIO 15)
#target_compile_definitions(dev_hid_composite PUBLIC PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)
pico_add_extra_outputs(main)
