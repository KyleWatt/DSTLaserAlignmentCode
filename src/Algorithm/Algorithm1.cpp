#include "Algorithm1.hpp"
#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include "pico/multicore.h"

//Global variables 
static optic_t opticA;
static optic_t opticB;
static optic_t opticC;
static optic_t opticD;
optic_t* optics[4];


void algorithm_init(){
    optic_motors_init(&opticA, OPTIC_A);
    optic_motors_init(&opticB, OPTIC_B);
    optic_motors_init(&opticC, OPTIC_C);
    optic_motors_init(&opticD, OPTIC_D);
    optics[0] = &opticA;
    optics[1] = &opticB;
    optics[2] = &opticC;
    optics[3] = &opticD;
}

void move_motors(){

}