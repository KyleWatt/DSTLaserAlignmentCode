#include "pico/stdlib.h"
#include "Stepper.hpp"

void start_raster(optic_t* optics[], int height, int width, int iterations);
void move_x(optic_t* optics[], uint width);
void move_y(optic_t* optics[], uint height);
float read_power(optic_select_t optic_select);