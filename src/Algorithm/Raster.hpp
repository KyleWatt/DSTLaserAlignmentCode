#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "Stepper.hpp"
#include "hardware/uart.h"


void start_raster(optic_t* optics[], int height, int width, int iterations);
void set_x_dir(uint dir);
void update_optic_steps(optic_t optic, uint stepsX, uint dirX, uint stepsY, uint dirY, uint stepsZ, uint dirZ);
void move_x(optic_t* optics[], uint width);
void move_y(optic_t* optics[], uint height);
float read_power(optic_select_t optic_select);