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

void update_optic_steps(optic_select_t optic_select, uint stepsX, uint dirX, uint stepsY, uint dirY, uint stepsZ, uint dirZ){
    optic_t* optic = optics[optic_select];
    optic->motor_X.target_steps = stepsX;
    optic->motor_X.direction = dirX;
    optic->motor_X.cur_steps = 0;
    optic->motor_Y.target_steps = stepsY;
    optic->motor_Y.direction = dirY;
    optic->motor_Y.cur_steps = 0;
    optic->motor_Z.target_steps = stepsZ;
    optic->motor_Z.direction = dirZ;
    optic->motor_Z.cur_steps = 0;

}