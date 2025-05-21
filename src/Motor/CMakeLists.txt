add_library(Motor Stepper.cpp)
target_link_libraries(Motor Power pico_stdlib pico_multicore pico_time)
target_include_directories(Motor PUBLIC . ../Power)