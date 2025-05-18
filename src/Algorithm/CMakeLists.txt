add_library(Algorithm Raster.cpp Spiral.cpp)
target_link_libraries(Algorithm Motor pico_stdlib pico_multicore hardware_uart pico_time)
target_include_directories(Algorithm PUBLIC . ../Motor)