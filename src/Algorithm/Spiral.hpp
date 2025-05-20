#include "pico/stdlib.h"
#include "Stepper.hpp"
#include "Power.hpp"


void start_spiral(optic_t* optics[], float angle_step, int spiral_spacing, int iterations);
void calculate_spiral_step(optic_t* optic, int spiral_spacing, float angle, float t);
void go_to_max_po(optic_t* optic);
void set_max_pos(optic_t* optic);