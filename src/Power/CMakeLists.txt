add_library(Power Power.cpp)
target_link_libraries(Power pico_stdlib hardware_uart pico_time pico_multicore)