#include "stepper.hpp"
#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include "pico/multicore.h"

void optic_motors_init(optic_t* optic,optic_select_t optic_select){
    // Setup the motor struct
    switch (optic_select)
    {
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
    optic->pin_enable = ENABLE_N;
    optic->moving = false;

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

void set_direction(optic_t* optics[]){
    for (uint i = 0; i < 4; i++){
        optic_t* optic = optics[i];
        if (optic->motor_X.direction != gpio_get(optic->motor_X.pin_dir)){
            gpio_put(optic->motor_X.pin_dir, optic->motor_X.direction);
        }
        if (optic->motor_Y.direction != gpio_get(optic->motor_Y.pin_dir)){
            gpio_put(optic->motor_Y.pin_dir, optic->motor_Y.direction);
        }
        if (optic->motor_Z.direction != gpio_get(optic->motor_Z.pin_dir)){
            gpio_put(optic->motor_Z.pin_dir, optic->motor_Z.direction);
        }
    }
    sleep_us(1);
}

void set_low(optic_t* optics[]){
    for (uint i = 0; i < 4; i++){
        optic_t* optic = optics[i];
        gpio_put(optic->motor_X.pin_step, 0);
        gpio_put(optic->motor_Y.pin_step, 0);
        gpio_put(optic->motor_Z.pin_step, 0);
    }

}

void update_motor_steps(optic_t* optic, motor_axis_t axis_select) {
    switch (axis_select)
    {
    case MOTOR_X:
        if (optic->motor_X.cur_steps != optic->motor_X.target_steps) {
            gpio_put(optic->motor_X.pin_step, 1);
            optic->motor_X.cur_steps++;
        } else {
            optic->motor_X.moving = false;
            optic->x_location += (optic->motor_X.direction == 0) ? -optic->motor_X.cur_steps : optic->motor_X.cur_steps;
        } 
        break;
    case MOTOR_Y:
        if (optic->motor_Y.cur_steps != optic->motor_Y.target_steps) {
            gpio_put(optic->motor_Y.pin_step, 1);
            optic->motor_Y.cur_steps++;
        } else {
            optic->motor_Y.moving = false;
            optic->y_location += (optic->motor_Y.direction == 0) ? -optic->motor_Y.cur_steps : optic->motor_Y.cur_steps;
        }
        break;
    case MOTOR_Z:
        if (optic->motor_Z.cur_steps != optic->motor_Z.target_steps) {
            gpio_put(optic->motor_Z.pin_step, 1);
            optic->motor_Z.cur_steps++;
        } else {
            optic->motor_Z.moving = false;
            optic->z_location += (optic->motor_Z.direction == 0) ? -optic->motor_Z.cur_steps : optic->motor_Z.cur_steps;
        }
        break;
    }
}


void motors_move(optic_t* optics[]){
    set_direction(optics);
    while (optics[0]->moving || optics[1]->moving || optics[2]->moving || optics[3]->moving) {
        // Set all step pins high for motors that need to move
        for (uint i = 0; i < 4; i++){
            update_motor_steps(optics[i], MOTOR_X);
            update_motor_steps(optics[i], MOTOR_Y);
            update_motor_steps(optics[i], MOTOR_Z);
            optics[i]->moving = (optics[i]->motor_X.moving || optics[i]->motor_Y.moving || optics[i]->motor_Z.moving);
        }

        
        // Hold step state before resetting
        sleep_us(3); 
        set_low(optics); // Set pins low after step pulse
        sleep_us(3); // Small delay before next iteration
    }
}