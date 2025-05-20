#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <string.h>
#include <stdlib.h>
#include "Power.hpp"

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 12
#define UART_RX_PIN 13
#define POWER_A_INDEX 4
#define POWER_B_INDEX 5
#define POWER_C_INDEX 6
#define POWER_D_INDEX 7

#define NUM_READINGS 7

void initPower() {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_fifo_enabled(UART_ID, false);
}


void readPower(power_t* powers, bool printPower) {
    char buffer[128];
    int index = 0;

    // Block until '@' is received
    while (1) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            if (c == '@') {
                buffer[index++] = c;
                break;
            }
        }
    }

    // Read until newline
    while (index < sizeof(buffer) - 1) {
        if (uart_is_readable(UART_ID)) {
            char c = uart_getc(UART_ID);
            buffer[index++] = c;
            if (c == '\n' || c == '\r') break;
        }
    }

    buffer[index] = '\0';

    // ----- Parse values -----
    int values[NUM_READINGS];
    int count = 0;

    // Skip '@' and tokenize
    char* data = buffer;
    if (data[0] == '@') {
        data++;  // move past the '@'
    }

    char* token = strtok(data, " ");
    while (token && count < NUM_READINGS) {
        if (strlen(token) > 0) { // ignore empty tokens from multiple spaces
            values[count++] = atoi(token);
        }
        token = strtok(NULL, " ");
    }

    if (count == NUM_READINGS) {
        if (printPower){
            printf("Parsed values:");
            for (int i = 0; i < NUM_READINGS; i++) {
                printf(" %d", values[i]);
            }
            printf("\n");
        }
        if (powers != nullptr) {
            powers->power_A = (float)values[POWER_A_INDEX];
            powers->power_B = (float)values[POWER_B_INDEX];
            powers->power_C = (float)values[POWER_C_INDEX];
            powers->power_D = (float)values[POWER_D_INDEX];
        }

    } else {
        printf("Error: Expected %d values but got %d\n", NUM_READINGS, count);
    }
}

