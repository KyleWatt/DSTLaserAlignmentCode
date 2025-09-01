#include "pico/stdlib.h"
#include "Stepper.hpp"
#include "Power.hpp"


void start_spiral(optic_t* optics[], int turns, int spiral_spacing, int iterations);
void calculate_spiral_step(optic_t* optic, int spiral_spacing, float angle, float t);
void start_spiral_automatic(optic_t* optics[], int turns, int spiral_spacing, int iterations, float threshold);