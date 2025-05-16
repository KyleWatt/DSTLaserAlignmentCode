add_library(Motor Stepper.cpp)
target_link_libraries(Motor pico_stdlib pico_multicore pico_time)