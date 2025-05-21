#ifndef POWER_HPP
#define POWER_HPP

#include "pico/multicore.h"
#include "pico/sync.h"

typedef struct {
    float power_A;
    float power_B;
    float power_C;
    float power_D;
} power_t;

extern power_t shared_power;
extern mutex_t power_mutex;

void initPower();
void readPowerLoop();  // run on core1
power_t get_latest_power();

#endif
