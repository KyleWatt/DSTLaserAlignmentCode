#include "stepper.hpp"
#include "Power.hpp"
#include "pico/time.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include "pico/multicore.h"
 
#define HOLD_TIME 500
 
void optic_motors_init(optic_t* optic, optic_select_t optic_select) {
    // Setup the motor struct
    switch (optic_select) {
    case OPTIC_A:
        //OPTIC A ---------------------------
        //Setting X motor
        optic->motor_X.pin_step      = STEP1_PIN;
        optic->motor_X.pin_dir       = DIR1_PIN;
        //Setting Y motor
        optic->motor_Y.pin_step      = STEP2_PIN;
        optic->motor_Y.pin_dir       = DIR2_PIN;
        //Setting Z motor
        optic->motor_Z.pin_step      = STEP3_PIN;
        optic->motor_Z.pin_dir       = DIR3_PIN;
        break;
    case OPTIC_B:
        //OPTIC B ---------------------------
        //Setting X motor
        optic->motor_X.pin_step      = STEP4_PIN;
        optic->motor_X.pin_dir       = DIR4_PIN;
        //Setting Y motor
        optic->motor_Y.pin_step      = STEP5_PIN;
        optic->motor_Y.pin_dir       = DIR5_PIN;
        //Setting Z motor
        optic->motor_Z.pin_step      = STEP6_PIN;
        optic->motor_Z.pin_dir       = DIR6_PIN;
        break;
    case OPTIC_C:
        //OPTIC C ---------------------------
        //Setting X motor
        optic->motor_X.pin_step      = STEP7_PIN;
        optic->motor_X.pin_dir       = DIR7_PIN;
        //Setting Y motor
        optic->motor_Y.pin_step      = STEP8_PIN;
        optic->motor_Y.pin_dir       = DIR8_PIN;
        //Setting Z motor
        optic->motor_Z.pin_step      = STEP9_PIN;
        optic->motor_Z.pin_dir       = DIR9_PIN;
        break;
    case OPTIC_D:
        //OPTIC D ---------------------------
        //Setting X motor
        optic->motor_X.pin_step      = STEP10_PIN;
        optic->motor_X.pin_dir       = DIR10_PIN;
        //Setting Y motor
        optic->motor_Y.pin_step      = STEP11_PIN;
        optic->motor_Y.pin_dir       = DIR11_PIN;
        //Setting Z motor
        optic->motor_Z.pin_step      = STEP12_PIN;
        optic->motor_Z.pin_dir       = DIR12_PIN;
        break;
    }
 
    // Init motor variables
    optic->motor_X.direction = 0;
    optic->motor_Y.direction = 0;
    optic->motor_Z.direction = 0;
    optic->motor_X.cur_steps = 0;
    optic->motor_Y.cur_steps = 0;
    optic->motor_Z.cur_steps = 0;
    optic->motor_X.target_steps = 0;
    optic->motor_Y.target_steps = 0;
    optic->motor_Z.target_steps = 0;
    optic->motor_X.moving = false;
    optic->motor_Y.moving = false;
    optic->motor_Z.moving = false;
    optic->x_location = 0;
    optic->y_location = 0;
    optic->z_location = 0;
    optic->max_x_location = 0;
    optic->max_y_location = 0;
    optic->max_z_location = 0;
    optic->pin_enable = ENABLE_N;
    optic->moving = false;
    optic->max_power = 0;
 
    // Initialise the GPIO
    gpio_init(optic->motor_X.pin_dir);
    gpio_init(optic->motor_Y.pin_dir);
    gpio_init(optic->motor_Z.pin_dir);
    gpio_init(optic->motor_X.pin_step);
    gpio_init(optic->motor_Y.pin_step);
    gpio_init(optic->motor_Z.pin_step);
    gpio_init(optic->pin_enable);
    gpio_set_dir(optic->motor_X.pin_dir, GPIO_OUT);
    gpio_set_dir(optic->motor_Y.pin_dir, GPIO_OUT);
    gpio_set_dir(optic->motor_Z.pin_dir, GPIO_OUT);
    gpio_set_dir(optic->motor_X.pin_step, GPIO_OUT);
    gpio_set_dir(optic->motor_Y.pin_step, GPIO_OUT);
    gpio_set_dir(optic->motor_Z.pin_step, GPIO_OUT);
    gpio_set_dir(optic->pin_enable, GPIO_OUT);
}
 
void enable_motors(optic_t* optic) {
    gpio_put(optic->pin_enable, 0);
}
 
void disable_motor(optic_t* optic) {
    gpio_put(optic->pin_enable, 1);
}

void set_max_pos(optic_t* optic, float power) {
    // Track max power and position for each optic
    if (power > optic->max_power) {
        optic->max_power = power;
        optic->max_x_location = optic->x_location;
        optic->max_y_location = optic->y_location;
    }
}

void go_to_max_pos(optic_t* optics[]) {
    for (int i = 0; i < 4; i++) {
        // Calculate steps needed to return to max position
        int x_steps = abs(optics[i]->x_location - optics[i]->max_x_location);
        int y_steps = abs(optics[i]->y_location - optics[i]->max_y_location);

        // Set X direction and steps
        optics[i]->motor_X.direction = (optics[i]->x_location < optics[i]->max_x_location) ? 1 : 0;
        optics[i]->motor_X.target_steps = x_steps;
        optics[i]->motor_X.moving = (x_steps > 0);

        // Set Y direction and steps
        optics[i]->motor_Y.direction = (optics[i]->y_location < optics[i]->max_y_location) ? 1 : 0;
        optics[i]->motor_Y.target_steps = y_steps;
        optics[i]->motor_Y.moving = (y_steps > 0);
    }
    motors_move(optics);
}

void set_direction(optic_t* optics[]) {
    for (uint i = 0; i < 4; i++) {
        optic_t* optic = optics[i];
        if (optic->motor_X.direction != gpio_get(optic->motor_X.pin_dir)) {
            gpio_put(optic->motor_X.pin_dir, optic->motor_X.direction);
        }
        if (optic->motor_Y.direction != gpio_get(optic->motor_Y.pin_dir)) {
            gpio_put(optic->motor_Y.pin_dir, optic->motor_Y.direction);
        }
        if (optic->motor_Z.direction != gpio_get(optic->motor_Z.pin_dir)) {
            gpio_put(optic->motor_Z.pin_dir, optic->motor_Z.direction);
        }
    }
    sleep_us(1);
}
 
void set_low(optic_t* optics[]) {
    for (uint i = 0; i < 4; i++) {
        optic_t* optic = optics[i];
        gpio_put(optic->motor_X.pin_step, 0);
        gpio_put(optic->motor_Y.pin_step, 0);
        gpio_put(optic->motor_Z.pin_step, 0);
    }
}
 
void update_motor_steps(optic_t* optic, motor_axis_t axis_select) {
    switch (axis_select) {
    case MOTOR_X:
        if (optic->motor_X.cur_steps < optic->motor_X.target_steps) {
            gpio_put(optic->motor_X.pin_step, 1);
            optic->motor_X.cur_steps++;
            optic->motor_X.moving = true;
            optic->x_location += (optic->motor_X.direction == 1) ? 1 : -1;
        } else {
            optic->motor_X.cur_steps = 0;
            optic->motor_X.target_steps = 0;
            optic->motor_X.moving = false;
        }
        break;

    case MOTOR_Y:
        if (optic->motor_Y.cur_steps < optic->motor_Y.target_steps) {
            gpio_put(optic->motor_Y.pin_step, 1);
            optic->motor_Y.cur_steps++;
            optic->motor_Y.moving = true;
            optic->y_location += (optic->motor_Y.direction == 1) ? 1 : -1;
        } else {
            optic->motor_Y.cur_steps = 0;
            optic->motor_Y.target_steps = 0;
            optic->motor_Y.moving = false;
        }
        break;

    case MOTOR_Z:
        if (optic->motor_Z.cur_steps < optic->motor_Z.target_steps) {
            gpio_put(optic->motor_Z.pin_step, 1);
            optic->motor_Z.cur_steps++;
            optic->motor_Z.moving = true;
            optic->z_location += (optic->motor_Z.direction == 1) ? 1 : -1;
        } else {
            optic->motor_Z.cur_steps = 0;
            optic->motor_Z.target_steps = 0;
            optic->motor_Z.moving = false;
        }
        break;
    }
}

 
void set_center(optic_t* optic) {
    optic->x_location = 0;
    optic->y_location = 0;
    optic->z_location = 0;
}
 
 
void center_optic(optic_t* optics[]) {
    for (uint i = 0; i < 4; i++) {
        if (optics[i]->x_location > 0) {
            // Set direction to go backwards toward center (0,0)
            optics[i]->motor_X.direction = 0;
            // Set target steps to number of steps to get to center
            optics[i]->motor_X.target_steps = optics[i]->x_location;
            optics[i]->motor_X.moving = true;
        } else if (optics[i]->x_location < 0) {
            // Set direction to go forwards toward center (0,0)
            optics[i]->motor_X.direction = 1;
            // Set target steps to number of steps to get to center
            optics[i]->motor_X.target_steps = abs(optics[i]->x_location);
            optics[i]->motor_X.moving = true;
        }
    
        if (optics[i]->y_location > 0) {
            optics[i]->motor_Y.direction = 0;
            optics[i]->motor_Y.target_steps = optics[i]->y_location;
            optics[i]->motor_Y.moving = true;
        } else if (optics[i]->y_location < 0) {
            optics[i]->motor_Y.direction = 1;
            optics[i]->motor_Y.target_steps = abs(optics[i]->y_location);
            optics[i]->motor_Y.moving = true;
        }
    
        if (optics[i]->z_location > 0) {
            optics[i]->motor_Z.direction = 0;
            optics[i]->motor_Z.target_steps = optics[i]->z_location;
            optics[i]->motor_Z.moving = true;
        } else if (optics[i]->z_location < 0) {
            optics[i]->motor_Z.direction = 1;
            optics[i]->motor_Z.target_steps = abs(optics[i]->z_location);
            optics[i]->motor_Z.moving = true;
        }
    motors_move(optics);
    }
}
 
void motors_move(optic_t* optics[]) {
    set_direction(optics);
    // For each optic check if any motors are moving
    bool any_moving = false;
    for (uint i = 0; i < 4; i++) {
        optics[i]->moving = (optics[i]->motor_X.moving || optics[i]->motor_Y.moving || optics[i]->motor_Z.moving);
        any_moving |= optics[i]->moving;
    }
 
    // Only enter the loop if at least one motor is moving
    while (any_moving) {
        // Set all step pins high for motors that need to move
        for (uint i = 0; i < 4; i++) {
            if (optics[i]->motor_X.moving) update_motor_steps(optics[i], MOTOR_X);
            if (optics[i]->motor_Y.moving) update_motor_steps(optics[i], MOTOR_Y);
            if (optics[i]->motor_Z.moving) update_motor_steps(optics[i], MOTOR_Z);
            sleep_us(HOLD_TIME);
            set_low(optics); // Set pins low after step pulse
            sleep_us(HOLD_TIME);
        }
        // Small delay before next iteration
        // Check if any motors are still moving
        //After Step read power and compare to max power in each optic
        power_t current_power = get_latest_power();

        printf("X= %d Y=%d power = %f \n",optics[0]->x_location, optics[0]->y_location, current_power.power_A);

        set_max_pos(optics[0], current_power.power_A);
        set_max_pos(optics[1], current_power.power_B);
        set_max_pos(optics[2], current_power.power_C);
        set_max_pos(optics[3], current_power.power_D);
        any_moving = false;
        for (uint i = 0; i < 4; i++) {
            optics[i]->moving = (optics[i]->motor_X.moving || optics[i]->motor_Y.moving || optics[i]->motor_Z.moving);
            any_moving |= optics[i]->moving;
        }
    }
    // Debug logging
    // for (uint i = 0; i < 4; i++) {
    //     printf("Optic %c position: X=%d, Y=%d, Z=%d\n", 
    //            'A' + i, optics[i]->x_location, optics[i]->y_location, optics[i]->z_location);
    // }
}