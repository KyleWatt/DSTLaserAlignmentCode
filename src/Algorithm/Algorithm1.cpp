#include "Algorithm1.hpp"
#include "pico/time.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include "pico/multicore.h"

#define ANGLE_STEP (3.14 / 10);
#define STEP_SIZE 1;


//Global variables 
static optic_t opticA;
static optic_t opticB;
static optic_t opticC;
static optic_t opticD;
static optic_t* optics[4];
 

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
    optic->moving = true;

}

void calculate_spiral_step(optic_select_t optic_select, float radius, float angle){
    int x_dir = 1;
    int y_dir = 1;
    int x_steps = (int) round(radius * cos(angle));
    int y_steps = (int) round(radius * sin(angle));
    if (x_steps < 0){
        x_dir = 0;
        x_steps = abs(x_steps);
    }
    if (y_steps < 0){
        y_dir = 0;
        y_steps = abs(y_steps);
    }
    update_optic_steps(optic_select, x_steps, x_dir, y_steps, y_dir, 0, 0);
}

float read_power(optic_select_t optic_select){
    //not sure yet. Packet format will be 2 bytes realating to power in mW
}

void start_spiral(uint iterations){
    float radius = 1;
    float angle = ANGLE_STEP;
    //read all the optics inital powers and set inital to max power 
    for (uint i = 0; i < iterations; i++){
        calculate_spiral_step(OPTIC_A, radius, angle);
        calculate_spiral_step(OPTIC_B, radius, angle);
        calculate_spiral_step(OPTIC_C, radius, angle);
        calculate_spiral_step(OPTIC_D, radius, angle);
        motors_move(optics);
        angle += ANGLE_STEP;
        //compare each optics power to its max and if its new power is greater than the max, set that to max and store x and y position.
    }
    // Compare max power to threshold. If max power is acceptable move to that position. else move z
}