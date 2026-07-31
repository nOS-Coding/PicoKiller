# This is a copy of <PICO_SDK_PATH>/external/pico_sdk_import.cmake

# This can be dropped into an external project to help locate this SDK
# It should be include() prior to project()

if (DEFINED ENV{PICO_SDK_PATH} AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
endif ()

# Legacy picotool
if (NOT PICO_SDK_PATH AND (DEFINED ENV{PICOTOOL_PATH} OR EXISTS ${CMAKE_SOURCE_DIR}/../picotool))
    list(APPEND CMAKE_PREFIX_PATH "${PICO_SDK_PATH}/../picotool")
endif ()

# PICO_SDK_PATH can be set in a variety of ways
if (NOT PICO_SDK_PATH AND DEFINED PICO_SDK_FETCH_FROM_GIT)
    include(FetchContent)
    set(fetch_options ...)
    message("Fetching pico-sdk from GitHub using FetchContent")
    FetchContent_Declare(
            pico_sdk
            GIT_REPOSITORY https://github.com/raspberrypi/pico-sdk
            GIT_TAG ${PICO_SDK_FETCH_FROM_GIT_TAG}
            GIT_SUBMODULES_RECURSE FALSE
            ${fetch_options}
    )
    if (NOT pico_sdk)
        message("Downloading Raspberry Pi Pico SDK")
        FetchContent_Populate(pico_sdk)
        set(PICO_SDK_PATH ${pico_sdk_SOURCE_DIR})
    endif ()
    set(PICO_SDK_FETCH_FROM_GIT 1)
endif ()

if (NOT PICO_SDK_PATH)
    set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/../pico-sdk")
endif ()

# Add the PICO_SDK_PATH to the top of the list
list(APPEND CMAKE_PREFIX_PATH "${PICO_SDK_PATH}/..")
list(APPEND CMAKE_PREFIX_PATH "${PICO_SDK_PATH}")

set(PICO_SDK_INIT_CMAKE_FILE ${PICO_SDK_PATH}/pico_sdk_init.cmake)
if (NOT EXISTS "${PICO_SDK_INIT_CMAKE_FILE}")
    message(FATAL_ERROR
            "Pico SDK was not found. You need to either pass a -DPICO_SDK_PATH=... or clone the SDK into the project tree:"
            "\n  git clone https://github.com/raspberrypi/pico-sdk.git\n"
            "and put it inside this project folder."
            )
endif ()

if (NOT PICO_SDK_PATH STREQUAL PICO_SDK_FETCH_FROM_GIT)
    set(PICO_SDK_PATH "${PICO_SDK_PATH}" CACHE PATH "Path to the Raspberry Pi Pico SDK")
endif ()

set(PICO_SDK_INIT_CMAKE_FILE ${PICO_SDK_PATH}/pico_sdk_init.cmake)
include(${PICO_SDK_INIT_CMAKE_FILE})
