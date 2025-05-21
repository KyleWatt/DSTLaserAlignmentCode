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

void start_spiral(optic_t* optics[], int turns, int spiral_spacing, int iterations){
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


    }
    go_to_max_pos(optics);
    printf("Optic A moved to max power location X:%d Y:%d with a power of: %f", optics[0]->x_location,optics[0]->y_location,optics[0]->max_power);
}