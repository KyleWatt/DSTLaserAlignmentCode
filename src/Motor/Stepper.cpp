#include "stepper.hpp"
#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include "pico/multicore.h"

// Global variables
static motor_t optic1x_motor;
static motor_t optic1y_motor;
static motor_t optic1z_motor;
static motor_t optic2x_motor;
static motor_t optic2y_motor;
static motor_t optic2z_motor;
static motor_t optic3x_motor;
static motor_t optic3y_motor;
static motor_t optic3z_motor;
static motor_t optic4x_motor;
static motor_t optic4y_motor;
static motor_t optic4z_motor;

void stepper_motor_init(motor_t* motor,optic_axis_t axis){
    // Setup the motor struct
    switch (axis)
    {
    //OPTIC 1 ---------------------------
    case OPTIC_1_X:
        motor->pin_step      = STEP1_PIN;
        motor->pin_dir       = DIR1_PIN;
        break;
    
    case OPTIC_1_Y:
        motor->pin_step      = STEP2_PIN;
        motor->pin_dir       = DIR2_PIN;
        break;
        
    case OPTIC_1_Z:
        motor->pin_step      = STEP2_PIN;
        motor->pin_dir       = DIR2_PIN;
        break;
        
    //OPTIC 2 ---------------------------
    case OPTIC_2_X:
        motor->pin_step      = STEP1_PIN;
        motor->pin_dir       = DIR1_PIN;
        break;
    
    case OPTIC_2_Y:
        motor->pin_step      = STEP2_PIN;
        motor->pin_dir       = DIR2_PIN;
        break;
        
    case OPTIC_2_Z:
        motor->pin_step      = STEP2_PIN;
        motor->pin_dir       = DIR2_PIN;
        break;

    }

    //OPTIC 3 ---------------------------
    case OPTIC_3_X:
        motor->pin_step      = STEP1_PIN;
        motor->pin_dir       = DIR1_PIN;
        break;
    
    case OPTIC_3_Y:
        motor->pin_step      = STEP2_PIN;
        motor->pin_dir       = DIR2_PIN;
        break;
        
    case OPTIC_3_Z:
        motor->pin_step      = STEP2_PIN;
        motor->pin_dir       = DIR2_PIN;
        break;

    //OPTIC 4 ---------------------------
    case OPTIC_4_X:
        motor->pin_step      = STEP1_PIN;
        motor->pin_dir       = DIR1_PIN;
        break;

    case OPTIC_4_Y:
        motor->pin_step      = STEP2_PIN;
        motor->pin_dir       = DIR2_PIN;
        break;
        
    case OPTIC_4_Z:
        motor->pin_step      = STEP2_PIN;
        motor->pin_dir       = DIR2_PIN;
        break;

    // Init motor variables
    motor->direction = 1;
    motor->location = 0;
    motor->target = 0;
    motor->enabled = false;
    motor->next_step_time = 0;
    motor->step_time = 250;
    motor->target_speed = MAX_SPEED;

    // Initialise the GPIO
    gpio_init(motor->pin_dir);
    gpio_init(motor->pin_step);
    gpio_init(motor->pin_enable);
    
    gpio_set_dir(motor->pin_dir, GPIO_OUT);
    gpio_set_dir(motor->pin_step, GPIO_OUT);
    gpio_set_dir(motor->pin_enable, GPIO_OUT);

    // Init the mutex
    mutex_init(&motor->lock);
}