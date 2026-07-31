cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set this before pico_sdk_init() if you are not building for the stock Pico board.
# Examples: pico_w, pico2, pico2_w
# set(PICO_BOARD pico)

include(pico_sdk_import.cmake)

project(picokiller C CXX ASM)
pico_sdk_init()

add_executable(picokiller
        main.c
        usb_descriptors.c
        keys.c
        keyboard.c
        script.c
        payloads.c
        config.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(picokiller PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# tinyusb_device = TinyUSB device stack, tinyusb_board = Pico USB board support,
# pico_unique_id = provides a stable USB serial number.
target_link_libraries(picokiller PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board)

# USB is busy being an HID keyboard, so logs go out over UART (default pins).
pico_enable_stdio_uart(picokiller 1)
pico_enable_stdio_usb(picokiller 0)

pico_add_extra_outputs(picokiller)
