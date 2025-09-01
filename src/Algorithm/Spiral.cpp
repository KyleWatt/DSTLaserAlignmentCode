#include "Spiral.hpp"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>

// Define pi if not available
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


void calculate_spiral_step(optic_t* optic, int spiral_spacing, float angle, float t) {
    // Calculate target position with floating-point precision
    float target_x = spiral_spacing * t * sinf(angle);
    float target_y = spiral_spacing * t * cosf(angle);


    // Calculate deltas from current position
    float dx = target_x - optic->x_pos;
    float dy = target_y - optic->y_pos;

    // Round to integer steps to avoid truncation errors
    int steps_x = (int)roundf(dx);
    int steps_y = (int)roundf(dy);


    optic->x_pos += steps_x;
    optic->y_pos += steps_y;  

    // Set motor steps and directions
    optic->motor_X.direction = (steps_x >= 0) ? 1 : 0;
    optic->motor_Y.direction = (steps_y >= 0) ? 1 : 0;
    optic->motor_X.moving = true;
    optic->motor_Y.moving = true;
    optic->motor_X.target_steps = abs(steps_x);
    optic->motor_Y.target_steps = abs(steps_y);
}

void start_spiral(optic_t* optics[], int turns, int spiral_spacing, int iterations){
    for (int i = 0; i < iterations; i++ ){
        float t = (float)i / (iterations - 1);
        float angle = t * 2 * M_PI * turns;
        //For each optic in optics
        for (int x = 0; x < 4; x++){
            calculate_spiral_step(optics[x], spiral_spacing, angle, t);
        }
        motors_move(optics);


    }
    go_to_max_pos(optics);
    printf("Optic A moved to max power location X:%d Y:%d with a power of: %f", optics[0]->x_location,optics[0]->y_location,optics[0]->max_power);
}

void start_spiral_automatic(optic_t* optics[], int turns, int spiral_spacing, int iterations, float threshold){
    for (int i = 0; i < iterations; i++ ){
        float t = (float)i / (iterations - 1);
        float angle = t * 2 * M_PI * turns;
        //For each optic in optics
        for (int x = 0; x < 4; x++){
            if (optics[x]->hold_position == false && optics[x]->cur_power < threshold){
                calculate_spiral_step(optics[x], spiral_spacing, angle, t);
            }
        }
        motors_move(optics);


    }
    go_to_max_pos(optics);
    printf("Optic A moved to max power location X:%d Y:%d with a power of: %f", optics[0]->x_location,optics[0]->y_location,optics[0]->max_power);
}