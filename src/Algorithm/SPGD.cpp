#include <random>
#include "SPGD.hpp"

float cost_plus[4];
float cost_minus[4];
float grad_approx[4];
int dx = 0;
int dy = 0;


void random_offset(optic_t* optics[], int offset_range){
    dx = rand() % (2 * offset_range + 1) - offset_range; // Random value between -offset_range and offset_range
    dy = rand() % (2 * offset_range + 1) - offset_range;
    for (uint i = 0; i < 4; i++){
        optic_t* optic = optics[i];
        optic->motor_X.direction = 1;
        optic->motor_Y.direction = 1;
        optic->motor_X.target_steps = abs(dx);
        optic->motor_Y.target_steps = abs(dy);
        optic->motor_X.moving = true;
        optic->motor_Y.moving = true;
    }
    motors_move(optics);
    for (uint i = 0; i < 4; i++){
        cost_plus[i] = optics[i]->cur_power;
    }

    for (uint i = 0; i < 4; i++){
        optic_t* optic = optics[i];
        optic->motor_X.direction = 0;
        optic->motor_Y.direction = 0;
        optic->motor_X.target_steps = abs(2*dx); // 2*dx to return to original position and then move to the new position
        optic->motor_Y.target_steps = abs(2*dy);
        optic->motor_X.moving = true;
        optic->motor_Y.moving = true;
    }
    motors_move(optics);
    for (uint i = 0; i < 4; i++){
        cost_minus[i] = optics[i]->cur_power;
    }

    for (uint i = 0; i < 4; i++){
        grad_approx[i] = (cost_plus[i] - cost_minus[i]) / (2 * offset_range);
    }

}

void spgd(optic_t* optics[], int iterations, int offset_range, float learning_rate) {
    for (int i = 0; i < iterations; i++) {
        random_offset(optics, offset_range);
        
        // Update positions based on gradient approximation
        for (uint j = 0; j < 4; j++) {
            int target_x =  optics[j]->x_location - learning_rate * grad_approx[j]*dx;
            int target_y =  optics[j]->y_location - learning_rate * grad_approx[j]*dy;
            optics[j]->motor_X.target_steps = abs(target_x - optics[j]->x_location);
            optics[j]->motor_Y.target_steps = abs(target_y - optics[j]->y_location);
            optics[j]->motor_X.direction = (target_x > optics[j]->x_location) ? 1 : 0;
            optics[j]->motor_Y.direction = (target_y > optics[j]->y_location) ? 1 : 0;
            optics[j]->motor_X.moving = true;
            optics[j]->motor_Y.moving = true;
        }
        motors_move(optics);
    }
}

{
    for (int i = 0; i < iterations; i++) {void spgd_automatic(optic_t* optics[], int iterations, int offset_range, float learning_rate, float threshold) 
        random_offset(optics, offset_range);
        
        // Update positions based on gradient approximation
        for (uint j = 0; j < 4; j++) {
            if (optics[j]->hold_position == false && optics[j]->cur_power < threshold){
                int target_x =  optics[j]->x_location - learning_rate * grad_approx[j]*dx;
                int target_y =  optics[j]->y_location - learning_rate * grad_approx[j]*dy;
                optics[j]->motor_X.target_steps = abs(target_x - optics[j]->x_location);
                optics[j]->motor_Y.target_steps = abs(target_y - optics[j]->y_location);
                optics[j]->motor_X.direction = (target_x > optics[j]->x_location) ? 1 : 0;
                optics[j]->motor_Y.direction = (target_y > optics[j]->y_location) ? 1 : 0;
                optics[j]->motor_X.moving = true;
                optics[j]->motor_Y.moving = true;
            }
        }
        motors_move(optics);
    }
}