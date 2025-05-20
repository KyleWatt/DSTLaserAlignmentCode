#include "Raster.hpp"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>
#include <cstdlib>

void set_max_pos(optic_t* optic){
    optic->max_x_location = optic->x_location;
    optic->max_y_location = optic->y_location;
}

void go_to_max_pos(optic_t* optics[]){
    for (int i = 0; i < 4; i++){
        if (optics[i]->x_location > optics[i]->max_x_location){
            //set direction to go backwards toward the max x location
            optics[i]->motor_X.direction = 0;
            //set target steps to number of steps to get to max x location
            int target_steps = optics[i]->x_location - optics[i]->max_x_location;
            optics[i]->motor_X.target_steps = target_steps;
        } else if (optics[i]->x_location < optics[i]->max_x_location){
            //set direction to go forwards toward the max x location
            optics[i]->motor_X.direction = 1;
            //set target steps to number of steps to get to max x location
            int target_steps = optics[i]->max_x_location - optics[i]->x_location;
            optics[i]->motor_X.target_steps = target_steps;
        }
        
        //Same for Y
        if (optics[i]->y_location > optics[i]->max_y_location){
            //set direction to go backwards toward the max x location
            optics[i]->motor_Y.direction = 0;
            //set target steps to number of steps to get to max x location
            int target_steps = optics[i]->y_location - optics[i]->max_y_location;
            optics[i]->motor_Y.target_steps = target_steps; 
        } else if (optics[i]->y_location < optics[i]->max_y_location){
            //set direction to go forwards toward the max x location
            optics[i]->motor_Y.direction = 1;
            //set target steps to number of steps to get to max x location
            int target_steps = optics[i]->max_y_location - optics[i]->y_location;
            optics[i]->motor_Y.target_steps = target_steps;
        }

    }
    //Move all motors to max location
    motors_move(optics); 
}

void move_x(optic_t* optics[], uint width, bool direction){
    for (uint i = 0; i < 4; i++){
        optics[i]->motor_X.direction = direction ? 1 : 0;
        optics[i]->motor_X.target_steps = width;
        optics[i]->motor_X.cur_steps = 0;
        optics[i]->motor_X.moving = true;
    }
    motors_move(optics);
}


void move_y(optic_t* optics[], uint height){
     for (uint i = 0; i < 4; i++){
        optics[i]->motor_Y.target_steps = height;
        optics[i]->motor_Y.cur_steps = 0;
        optics[i]->motor_Y.moving = true;
    }
    motors_move(optics);
}   

void start_raster(optic_t* optics[], int height, int width, int iterations){
    power_t current_powers;
    power_t max_powers;
    readPower(&max_powers,false);
    bool x_forward = true;

    for (uint i = 0; i < iterations; i++){
        move_x(optics, width, x_forward);
        readPower(&current_powers,false);

        if (current_powers.power_A > max_powers.power_A) {
            max_powers.power_A = current_powers.power_A;
            set_max_pos(optics[0]);
        }
        if (current_powers.power_B > max_powers.power_B) {
            max_powers.power_B = current_powers.power_B;
            set_max_pos(optics[1]);
        }

        // Move Y after each X pass except the last iteration
        if (i < iterations - 1){
            move_y(optics, height);
        }

        x_forward = !x_forward; // Alternate X direction for next row
    }

    go_to_max_pos(optics);
    printf("Optic A moved to max power location X:%d Y:%d with a power of: %f", optics[0]->x_location,optics[0]->y_location,max_powers.power_A);
}

