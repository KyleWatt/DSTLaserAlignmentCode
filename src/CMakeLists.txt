add_subdirectory(Motor)
add_subdirectory(Algorithm)

add_executable(main "main.cpp")

target_include_directories(main PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Motor
    ${CMAKE_CURRENT_SOURCE_DIR}/Algorithm
)

target_link_libraries(main PRIVATE 
    warnings 
    pico_stdlib 
    pico_multicore
    hardware_gpio
    pico_stdio_usb
    Motor
    Algorithm
)

pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)
