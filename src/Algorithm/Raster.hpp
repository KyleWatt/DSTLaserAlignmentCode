#include "pico/stdlib.h"
#include "Stepper.hpp"
#include "Power.hpp"

void start_raster(optic_t* optics[], int height, int width, int iterations, uint ydir);
void move_x(optic_t* optics[], uint width);
void move_y(optic_t* optics[], uint height, uint ydir);
float read_power(optic_select_t optic_select);