#include "Raster.hpp"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>
#include <cstdlib>


void move_x(optic_t* optics[], uint width, bool direction){
    for (uint i = 0; i < 4; i++){
        optics[i]->motor_X.direction = direction ? 1 : 0;
        optics[i]->motor_X.target_steps = width;
        optics[i]->motor_X.cur_steps = 0;
        optics[i]->motor_X.moving = true;
    }
    motors_move(optics);
}


void move_y(optic_t* optics[], uint height, uint ydir){
     for (uint i = 1; i < 4; i++){
        optics[i]->motor_Y.target_steps = height;
        optics[i]->motor_Y.direction = ydir;
        optics[i]->motor_Y.cur_steps = 0;
        optics[i]->motor_Y.moving = true;
    }
    motors_move(optics);
}   

void start_raster(optic_t* optics[], int height, int width, int iterations, uint ydir){
    bool x_forward = true;

    for (uint i = 0; i < iterations; i++){
        move_x(optics, width, x_forward);

        // Move Y after each X pass except the last iteration
        if (i < iterations - 1){
            move_y(optics, height, ydir);
        }

        x_forward = !x_forward; // Alternate X direction for next row
    }

    go_to_max_pos(optics);
    printf("Optic A moved to max power location X:%d Y:%d with a power of: %f", optics[0]->x_location,optics[0]->y_location, optics[0]->max_power);
}

