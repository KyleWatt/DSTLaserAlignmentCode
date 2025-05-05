find_library(../freertos freertos)
add_subdirectory(Motor)
add_subdirectory(Algorithm)

add_executable(main "main.cpp")

target_include_directories(main PRIVATE 
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/Motor
            ${CMAKE_CURRENT_SOURCE_DIR}/Algorithm)

target_compile_features(main PRIVATE cxx_std_23)
target_link_libraries(main PRIVATE 
            warnings 
            pico_stdlib 
            pico_multicore
            hardware_gpio 
            hardware_irq
            hardware_pwm
            FreeRTOS
            Stepper
)