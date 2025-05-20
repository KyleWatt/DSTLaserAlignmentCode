#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "Stepper.hpp" 
#include "Raster.hpp" 
#include "Power.hpp"


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
    MODE_MANUAL
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


            if (axis && is_valid_number(stepsStr) && is_valid_number(optic_select_char)) {
                uint steps = atoi(stepsStr);
                uint optic_select_int = atoi(optic_select_char);
                if (strcmp(axis, "x") == 0) {
                    printf("MOVING X\n");
                    optics[optic_select_int]->motor_X.target_steps = steps;
                    motors_move(optics);
                } else if (strcmp(axis, "y") == 0) {
                    printf("MOVING Y\n");
                    optics[optic_select_int]->motor_X.target_steps = steps;
                    motors_move(optics);
                } else if (strcmp(axis, "z") == 0) {
                    printf("MOVING Z\n");
                    optics[optic_select_int]->motor_Z.target_steps = steps;
                    motors_move(optics);
                    }
                } else {
                    printf("Error: Unknown axis '%s'. Use 'x' or 'y' or 'z' .\n", axis);
                }
            } else {
                printf("Error: Invalid move format. Usage: move 0/1/2/3 x/y/z <steps>\n");
            }
        } else if (strcmp(token, "set") == 0) {
            char *optic_select_char = strtok(NULL, " ");
            char *axis = strtok(NULL, " ");
            char *dir = strtok(NULL, " ");

            if (axis && is_valid_number(dir) && is_valid_number(optic_select_char)) {
                uint direction = atoi(dir);
                uint optic_select_int = atoi(optic_select_char);
                if (strcmp(axis, "x") == 0) {
                    optics[optic_select_int]->motor_X.direction = direction;
                } else if (strcmp(axis, "y") == 0) {
                    optics[optic_select_int]->motor_Y.direction = direction;
                } else if (strcmp(axis, "z") == 0){
                    optics[optic_select_int]->motor_Z.direction = direction;
                }else {
                    printf("Error: Unknown axis '%s'. Use 'x' or 'y'.\n", axis);
                }
            } else {
                printf("Error: Invalid set format. Usage: set 0/1/2/3 x/y <0/1>\n");
            }
        } else if (strcmp(token, "exit") == 0) {
            printf("Exiting manual mode.\n");
            current_mode = MODE_MAIN;
        } else if (strcmp(token, "help") == 0) {
            printf("Manual Commands:\n");
            printf("  move x/y <steps>\n");
            printf("  set x/y <0/1>\n");
            printf("  exit\n");
        } else {
            printf("Unknown manual command: %s\n", token);
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
            char *interactionsStr = strtok(NULL, " ");

            int height = 1000;
            int width = 100;
            int interactions = 100;

            // If user provides values and they are valid numbers, update them
            if (heightStr && widthStr && interactionsStr &&
                is_valid_number(heightStr) &&
                is_valid_number(widthStr) &&
                is_valid_number(interactionsStr)) {

                height = atoi(heightStr);
                width = atoi(widthStr);
                interactions = atoi(interactionsStr);
            } else if (heightStr || widthStr || interactionsStr) {
                printf("Invalid raster parameters. Usage: raster [height width interactions]\n");
                free(msg_copy);
                return;
            }

            printf("Starting Raster Scan with height=%d, width=%d, interactions=%d\n", height, width, interactions);
            start_raster(optics, height, width, interactions);

        } else if (strcmp(token, "spiral") == 0) {
            printf("Starting Spiral\n");

        } else if (strcmp(token, "help") == 0) {
            printf("Main Commands:\n");
            printf("  manual       - Enter manual control mode\n");
            printf("  raster [h w i] - Start raster scan with optional height, width, interactions\n");
            printf("  spiral       - Start spiral movement\n");

        } else if (strcmp(token, "power") == 0) {
            printf("Checking power...\n");
            readPower();
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
