#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "pico/multicore.h"
#include "pico/sync.h"
#include <string.h>
#include <stdlib.h>
#include "Power.hpp"

#define UART_ID uart0
#define BAUD_RATE 115200
#define UART_TX_PIN 12
#define UART_RX_PIN 13
#define POWER_A_INDEX 3
#define POWER_B_INDEX 4
#define POWER_C_INDEX 5
#define POWER_D_INDEX 6
#define NUM_READINGS 7

power_t shared_power;
mutex_t power_mutex;

void initPower() {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_set_fifo_enabled(UART_ID, false);

    mutex_init(&power_mutex);
    multicore_launch_core1(readPowerLoop);  // Launch background reader
}

void readPowerLoop() {
    char buffer[128];
    int index = 0;
    int values[NUM_READINGS];

    while (true) {
        // Wait for '@'
        while (1) {
            if (uart_is_readable(UART_ID)) {
                char c = uart_getc(UART_ID);
                if (c == '@') {
                    buffer[0] = c;
                    index = 1;
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

        // Parse data
        char* data = buffer;
        if (*data == '@') data++;

        int count = 0;
        char* token = strtok(data, " ");
        while (token && count < NUM_READINGS) {
            if (strlen(token) > 0) {
                values[count++] = atoi(token);
            }
            token = strtok(NULL, " ");
        }
        // printf("Parsed values:");
        //     for (int i = 0; i < NUM_READINGS; i++) {
        //         printf(" %d", values[i]);
        //     }
        //     printf("\n");
    
        if (count == NUM_READINGS) {
            mutex_enter_blocking(&power_mutex);
            shared_power.power_A = (float)values[POWER_A_INDEX];
            shared_power.power_B = (float)values[POWER_B_INDEX];
            shared_power.power_C = (float)values[POWER_C_INDEX];
            shared_power.power_D = (float)values[POWER_D_INDEX];
            mutex_exit(&power_mutex);
        } else {
            // printf("Power Error: Expected %d values, got %d\n", NUM_READINGS, count);
        }
    }
}

power_t get_latest_power() {
    power_t result;
    mutex_enter_blocking(&power_mutex);
    result = shared_power;
    mutex_exit(&power_mutex);
    return result;
}