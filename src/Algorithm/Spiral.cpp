#include "Spiral.hpp"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>

// Define pi if not available
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


void calculate_spiral_step(optic_t* optic, int spiral_spacing, float angle, float t) {
        int x = (int)(spiral_spacing * t * sinf(angle));
        int y = (int)(spiral_spacing * t * cosf(angle));

        int dx = x - optic->x_location;
        int dy = y - optic->y_location;

        //Get direction
        uint x_dir = (dx >= 0) ? 1 : 0;
        uint y_dir = (dy >= 0) ? 1 : 0;

        //Remove any negative steps
        uint x_steps = abs(dx);
        uint y_steps = abs(dy);

        // Control motors: set direction then move
        optic->motor_X.cur_steps = 0;
        optic->motor_X.target_steps = x_steps;
        optic->motor_X.direction = x_dir;

        optic->motor_Y.cur_steps = 0;
        optic->motor_Y.target_steps = y_steps;
        optic->motor_Y.direction = y_dir;
        
}

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

void start_spiral(optic_t* optics[], int turns, int spiral_spacing, int iterations){
    power_t current_powers;
    power_t max_powers;
    readPower(&max_powers,false);
    float t;
    float angle;
    for (int i = 0; i < iterations; i++ ){
        t = (float)i / (iterations - 1);
        angle = t * 2 * M_PI * turns;
        //For each optic in optics
        for (int x = 0; x < 4; x++){
            calculate_spiral_step(optics[x], spiral_spacing, angle, t);
        }
        motors_move(optics);
        readPower(&current_powers,false);
        if (current_powers.power_A > max_powers.power_A) {
            max_powers.power_A = current_powers.power_A;
            set_max_pos(optics[0]);
        }
        if (current_powers.power_B > max_powers.power_B) {
            max_powers.power_B = current_powers.power_B;
            set_max_pos(optics[1]);
        }


    }
    go_to_max_pos(optics);
    printf("Optic A moved to max power location X:%d Y:%d with a power of: %f", optics[0]->x_location,optics[0]->y_location,max_powers.power_A);
}