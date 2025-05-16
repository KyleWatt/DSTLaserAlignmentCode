#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "Stepper.hpp"
#include "hardware/uart.h"


void algorithm_init();
void algorithm_start();
void algorithm_stop();
void read_USB();
void calculate_steps();
void update_optic_steps(optic_select_t optic_select, uint stepsX, uint dirX, uint stepsY, uint dirY, uint stepsZ, uint dirZ);
