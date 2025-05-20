#include "pico/stdlib.h"
#include "pico/multicore.h"

#ifndef STEPPER_HPP
#define STEPPER_HPP

//Motor Step and DIR PINS

#define STEP1_PIN 0
#define DIR1_PIN 1
#define STEP2_PIN 2
#define DIR2_PIN 3
#define STEP3_PIN 4 //NEED TO CHANGE THIS BACK -> JUST HAD TO CHANGE PINS TO MATCH DEV BOARD REAL PINS DIFFRENT
#define DIR3_PIN 5
#define STEP4_PIN 6
#define DIR4_PIN 7
#define STEP5_PIN 8
#define DIR5_PIN 9
#define STEP6_PIN 10
#define DIR6_PIN 11
#define STEP7_PIN 12
#define DIR7_PIN 13
#define STEP8_PIN 14
#define DIR8_PIN 15
#define STEP9_PIN 16
#define DIR9_PIN 17
#define STEP10_PIN 18
#define DIR10_PIN 19
#define STEP11_PIN 20
#define DIR11_PIN 21
#define STEP12_PIN 22
#define DIR12_PIN 23

// Enable Pin
#define ENABLE_N 38

typedef enum {
    MOVING,
    STOPPED
} motor_state_t;

typedef struct {
    // Pins
    uint pin_dir;
    uint pin_step;

    // Motion
    int cur_steps;
    int target_steps;
    int8_t direction;

    bool moving; 
} motor_t;

typedef struct {
    motor_t motor_X;
    motor_t motor_Y;
    motor_t motor_Z;
    int x_location;
    int y_location;
    int z_location;
    int max_x_location;
    int max_y_location;
    int max_z_location;
    uint pin_enable;
    bool moving;

} optic_t;

typedef enum {
    OPTIC_A,
    OPTIC_B,
    OPTIC_C,
    OPTIC_D,
} optic_select_t;

typedef enum {
    MOTOR_X,
    MOTOR_Y,
    MOTOR_Z,
} motor_axis_t;

void optic_motors_init(optic_t* optic,optic_select_t optic_select);

void update_motor_steps(optic_t* optic, motor_axis_t motor_axis);

void disable_motor(motor_t* motor);

void enable_motor(motor_t* motor);

void motors_move(optic_t** optics);

void set_direction(optic_t optics[]);

void center_optic(optic_t* optic);

void set_max(optic_t* optic, motor_axis_t axis_select );

void set_center(optic_t* optic);


#endif // STEPPER_HPP