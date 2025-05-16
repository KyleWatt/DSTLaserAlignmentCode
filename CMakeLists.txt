# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico2 CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.20)

include(pico_sdk_import.cmake)

project(pico-cpp LANGUAGES C CXX ASM)

pico_sdk_init()

add_compile_definitions(DEBUG_ENABLE)

add_library(warnings INTERFACE)
target_compile_options(warnings INTERFACE "-Wall" "-Wextra" "-Wno-psabi")

# Add subdirectories
add_subdirectory(src/Motor)
add_subdirectory(src/Algorithm)

# Add main executable (this builds in root-level build/)
add_executable(pico-cpp 
    src/main.cpp
)

# Include paths
target_include_directories(pico-cpp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Motor
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Algorithm
)

# C++ features
target_compile_features(pico-cpp PUBLIC cxx_std_17)

# Link libraries
target_link_libraries(pico-cpp PRIVATE 
    warnings 
    pico_stdlib 
    pico_multicore
    hardware_gpio
    Motor
)

# Generate .elf, .bin, .uf2, etc.
pico_add_extra_outputs(pico-cpp)
