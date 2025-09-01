#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "Stepper.hpp" 
#include "Raster.hpp" 
#include "Power.hpp"
#include "Spiral.hpp"
#include "SPGD.hpp"


#define UART_ID uart1
#define BAUD_RATE 115200

#define UART_TX_PIN 4
#define UART_RX_PIN 5
#define NUM_READINGS 7

#define MAX_MESSAGE_LENGTH 40

static optic_t* optics[4];
static optic_t opticA;
static optic_t opticB;
static optic_t opticC;
static optic_t opticD;

typedef enum {
    MODE_MAIN,
    MODE_MANUAL,
    MODE_AUTOMATIC
} ControlMode;

ControlMode current_mode = MODE_MAIN;

void reset_buffer(char *buffer, size_t length) {
    memset(buffer, 0, length);
}


void optics_init() {
    optic_motors_init(&opticA, OPTIC_A);
    optic_motors_init(&opticB, OPTIC_B);
    optic_motors_init(&opticC, OPTIC_C);
    optic_motors_init(&opticD, OPTIC_D);

    optics[0] = &opticA;
    optics[1] = &opticB;
    optics[2] = &opticC;
    optics[3] = &opticD;
}


char *my_strdup(const char *src) {
    if (!src) return NULL;
    size_t len = strlen(src) + 1;
    char *dup = (char *)malloc(len);
    if (dup) {
        memcpy(dup, src, len);
    }
    return dup;
}

int is_valid_number(const char *str) {
    if (!str || *str == '\0') return 0;
    while (*str) {
        if (!isdigit(*str++)) return 0;
    }
    return 1;
}



void process_manual_command(const char *message) {
    char *msg_copy = my_strdup(message);
    char *token = strtok(msg_copy, " ");

    if (token != NULL) {
        if (strcmp(token, "move") == 0) {
            char *optic_select_char = strtok(NULL, " ");
            char *axis = strtok(NULL, " ");
            char *stepsStr = strtok(NULL, " ");

            if (optic_select_char && axis && stepsStr &&
                is_valid_number(optic_select_char) &&
                is_valid_number(stepsStr)) {

                uint steps = atoi(stepsStr);
                uint optic_select_int = atoi(optic_select_char);

                if (optic_select_int > 3) {
                    printf("Error: Optic index must be 0 to 3.\n");
                } else if (strcmp(axis, "x") == 0) {
                    printf("MOVING optic %u X by %u steps\n", optic_select_int, steps);
                    optics[optic_select_int]->motor_X.target_steps = steps;
                    optics[optic_select_int]->motor_X.moving = true;
                    motors_move(optics);
                } else if (strcmp(axis, "y") == 0) {
                    printf("MOVING optic %u Y by %u steps\n", optic_select_int, steps);
                    optics[optic_select_int]->motor_Y.target_steps = steps;
                    optics[optic_select_int]->motor_Y.moving = true;
                    motors_move(optics);
                } else if (strcmp(axis, "z") == 0) {
                    printf("MOVING optic %u Z by %u steps\n", optic_select_int, steps);
                    optics[optic_select_int]->motor_Z.target_steps = steps;
                    optics[optic_select_int]->motor_Z.moving = true;
                    motors_move(optics);
                } else {
                    printf("Error: Unknown axis '%s'. Use 'x' or 'y' or 'z'.\n", axis);
                }
            } else {
                printf("Error: Invalid move format. Usage: move <optic 0-3> x/y/z <steps>\n");
            }

        } else if (strcmp(token, "set") == 0) {
            char *optic_select_char = strtok(NULL, " ");
            char *axis = strtok(NULL, " ");
            char *dir = strtok(NULL, " ");

            if (optic_select_char && axis && dir &&
                is_valid_number(optic_select_char) &&
                is_valid_number(dir)) {

                uint direction = atoi(dir);
                uint optic_select_int = atoi(optic_select_char);

                if (optic_select_int > 3) {
                    printf("Error: Optic index must be 0 to 3.\n");
                } else if (strcmp(axis, "x") == 0) {
                    optics[optic_select_int]->motor_X.direction = direction;
                    printf("Set optic %u X direction to %u\n", optic_select_int, direction);
                } else if (strcmp(axis, "y") == 0) {
                    optics[optic_select_int]->motor_Y.direction = direction;
                    printf("Set optic %u Y direction to %u\n", optic_select_int, direction);
                } else if (strcmp(axis, "z") == 0) {
                    optics[optic_select_int]->motor_Z.direction = direction;
                    printf("Set optic %u Z direction to %u\n", optic_select_int, direction);
                } else {
                    printf("Error: Unknown axis '%s'. Use 'x' or 'y' or 'z'.\n", axis);
                }
            } else {
                printf("Error: Invalid set format. Usage: set <optic 0-3> x/y/z <0/1>\n");
            }

        } else if (strcmp(token, "setcentre") == 0) {
            char *optic_select_char = strtok(NULL, " ");
            if (optic_select_char && is_valid_number(optic_select_char)){
                uint optic_select_int = atoi(optic_select_char);
                set_center(optics[optic_select_int]);
            }
        } else if (strcmp(token, "centre") == 0) {
            center_optic(optics);
        } else if (strcmp(token, "location") == 0) {
            printf("X = %d Y = %d Z = %d \n",optics[0]->x_location,optics[0]->y_location, optics[0]->z_location);
        } else if (strcmp(token, "direction") == 0) {
            printf("Xdir = %d Ydir = %d Zdir = %d \n",optics[0]->motor_X.direction,optics[0]->motor_Y.direction, optics[0]->motor_Z.direction);
        } else if (strcmp(token, "exit") == 0) {
            printf("Exiting manual mode.\n");
            current_mode = MODE_MAIN;

        } else if (strcmp(token, "help") == 0) {
            printf("Manual Mode Commands:\n");
            printf("  move <optic 0-3> <x|y|z> <steps>     - Move selected motor by step count\n");
            printf("     e.g., move 0 x 100\n");
            printf("  set <optic 0-3> <x|y|z> <0|1>        - Set direction of motor (0: one way, 1: other)\n");
            printf("     e.g., set 2 y 1\n");
            printf("  setcentre <optic 0-3>               - Set current position as center for optic\n");
            printf("     e.g., setcentre 1\n");
            printf("  centre <optic 0-3>                  - Move optic to its center position\n");
            printf("     e.g., centre 3\n");
            printf("  exit                                - Return to main command mode\n");
            printf("  help                                - Show this help message\n");
        } else {
            printf("Unknown manual command: %s\n", token);
        }
    } 

    free(msg_copy);
}


void process_main_command(const char *message) {
    char *msg_copy = my_strdup(message);
    char *token = strtok(msg_copy, " ");

    if (token != NULL) {
        if (strcmp(token, "manual") == 0) {
            printf("Entering manual mode...\n");
            current_mode = MODE_MANUAL;

        } else if (strcmp(token, "raster") == 0) {
            // Parse optional arguments
            char *heightStr = strtok(NULL, " ");
            char *widthStr = strtok(NULL, " ");
            char *interatrionsStr = strtok(NULL, " ");
            char *ydirStr = strtok(NULL, " ");

            // If user provides values and they are valid numbers, update them
            if (heightStr && widthStr && interatrionsStr && ydirStr &&
                is_valid_number(heightStr) &&
                is_valid_number(widthStr) &&
                is_valid_number(interatrionsStr)&&
                is_valid_number(ydirStr)) {

                int height = atoi(heightStr);
                int width = atoi(widthStr);
                int iterations = atoi(interatrionsStr);
                uint ydir = atoi(ydirStr);
                printf("Starting Raster Scan with height=%d, width=%d, iterations=%d\n", height, width, iterations);
                start_raster(optics, height, width, iterations, ydir);
            } else if (heightStr || widthStr || interatrionsStr) {
                printf("Invalid raster parameters. Usage: raster [height width iterations]\n");
                free(msg_copy);
                return;
            }

        } else if (strcmp(token, "spiral") == 0) {
            printf("Starting Spiral\n");
            // Parse optional arguments
            //int turns, int spiral_spacing, int iterations
            char *turnsStr = strtok(NULL, " ");
            char *spacingStr = strtok(NULL, " ");
            char *interatrionsStr = strtok(NULL, " ");

            // If user provides values and they are valid numbers, update them
            if (turnsStr && spacingStr && interatrionsStr &&
                is_valid_number(turnsStr) &&
                is_valid_number(spacingStr) &&
                is_valid_number(interatrionsStr)) {

                int turns = atoi(turnsStr);
                int spacing = atoi(spacingStr);
                int interactions = atoi(interatrionsStr);
                printf("Starting Spiral with turns=%d, width=%d, interactions=%d\n", turns, spacing, interactions);
                start_spiral(optics, turns, spacing, interactions);

            } else if (turnsStr || spacingStr || interatrionsStr) {
                printf("Invalid raster parameters. Usage: raster [turns spacing iterations]\n");
                free(msg_copy);
                return;
            }
        else if (strcmp(token, "automatic") == 0) {
            float threshold = 20.0; // Example threshold value
            printf("Entering automatic mode...\n");
            current_mode = MODE_AUTOMATIC; 
            while(strcmp(token, "break") != 0){
                for (optic_t* optic : optics){
                    if (optic->hold_position == false ){
                        start_spiral_automatic(optics, 3, 2, 4, threshold);
                        spgd_automatic(optics, 10, 5, 0.1, threshold);
                    }
                }
            }
        }
        } else if (strcmp(token, "help") == 0) {
            printf("Main Mode Commands:\n");
            printf("  manual                 - Enter manual control mode\n");
            printf("  raster [h w i y]         - Start raster scan (optional height, width, iterations, y direction)\n");
            printf("     e.g., raster 5 5 3\n");
            printf("  spiral [t s i]         - Start spiral scan (optional turns, spacing, iterations)\n");
            printf("     e.g., spiral 3 2 4\n");
            printf("  power                  - Read current power levels\n");
            printf("  help                   - Show this help message\n");
        }else if (strcmp(token, "power") == 0) {
            printf("Checking power...\n");
            power_t current_power = get_latest_power();

            printf("Current Power: A=%.2f, B=%.2f, C=%.2f, D=%.2f\n",
                current_power.power_A,
                current_power.power_B,
                current_power.power_C,
                current_power.power_D);
        }
        else {
            printf("Unknown main command: %s\n", token);
        }
    }

    free(msg_copy);
}


void handle_serial_input(void (*command_handler)(const char *)) {
    static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;

    int c = getchar_timeout_us(0); // Non-blocking read

    if (c != PICO_ERROR_TIMEOUT) {
        if (c != '\n' && c != '\r' && message_pos < MAX_MESSAGE_LENGTH - 1) {
            message[message_pos++] = (char)c;
        } else if (c == '\n' || c == '\r') {
            message[message_pos] = '\0';
            if (message_pos > 0) {
                printf("Received: %s\n", message);
                command_handler(message);
            }
            message_pos = 0;
            reset_buffer(message, MAX_MESSAGE_LENGTH);
        }
    }
}

int main() {
    stdio_init_all();
    optics_init();
    initPower();

    printf("System ready. Type 'help' for available commands.\n");

    while (true) {
        if (current_mode == MODE_MAIN) {
            handle_serial_input(process_main_command);
        } else if (current_mode == MODE_MANUAL) {
            handle_serial_input(process_manual_command);
        }
    }

    return 0;
}
