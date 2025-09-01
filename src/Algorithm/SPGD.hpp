#include "pico/stdlib.h"
#include "Stepper.hpp"
#include "Power.hpp"

void random_offset(optic_t* optics[], int offset_range);
void spgd(optic_t* optics[], int iterations, int offset_range, float learning_rate);
void spgd_automatic(optic_t* optics[], int iterations, int offset_range, float learning_rate, float threshold);