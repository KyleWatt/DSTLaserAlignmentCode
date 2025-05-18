#include "Raster.hpp"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>
#include <cstdlib>

float read_power(optic_select_t optic_select){
    //not sure yet. Packet format will be 2 bytes realating to power in mW
    return 0.0f;  // placeholder
}

void move_x(optic_t* optics[], uint width){
    for (uint i = 0; i < 4; i++){
        optics[i]->motor_X.direction ^= 1;  // Toggle between 0 and 1
        optics[i]->motor_X.target_steps = width;
        optics[i]->motor_X.cur_steps = 0;
        optics[i]->moving = true;
        optics[i]->motor_X.moving = true;
    }
    printf("Raster optic pointer[0] = %p\n", (void*)optics[0]);
    printf("Rastery optic 0 = %d\n", optics[0]->moving);
    printf("Rastery optic 1 = %d\n", optics[1]->moving);
    motors_move(optics);
}

void move_y(optic_t* optics[], uint height){
     for (uint i = 0; i < 4; i++){
        optics[i]->motor_Y.target_steps = height;
        optics[i]->motor_Y.cur_steps = 0;
        optics[i]->moving = true;
        optics[i]->motor_Y.moving = true;
    }
    printf("Raster optic pointer[0] = %p\n", (void*)optics[0]);
    printf("Rasterx optic 0 = %d\n", optics[0]->moving);
    printf("Rasterx optic 1 = %d\n", optics[1]->moving);
    motors_move(optics);
}   

void start_raster(optic_t* optics[], int height, int width, int iterations){
    uint xdirection = 0;
    for (uint i = 0; i < iterations; i++){
        if (i % 2 == 0){
            move_x(optics, width);
        }else{
            move_y(optics, height);
        }
    }
}
