#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "pico/multicore.h"

//Motor Step and DIR PINS

#define STEP1_PIN 1
#define DIR1_PIN 2
#define STEP2_PIN 3
#define DIR2_PIN 4
#define STEP3_PIN 5
#define DIR3_PIN 6
#define STEP4_PIN 7
#define DIR4_PIN 8
#define STEP5_PIN 9
#define DIR5_PIN 10
#define STEP6_PIN 11
#define DIR6_PIN 12
#define STEP7_PIN 13
#define DIR7_PIN 14
#define STEP8_PIN 15
#define DIR8_PIN 16
#define STEP9_PIN 17
#define DIR9_PIN 18
#define STEP10_PIN 19
#define DIR10_PIN 20
#define STEP11_PIN 21
#define DIR11_PIN 22
#define STEP12_PIN 23
#define DIR12_PIN 24

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

void motors_move(optic_t optics[]);

void set_direction(optic_t optics[]);



