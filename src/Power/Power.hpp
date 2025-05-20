#include "pico/stdlib.h"
#ifndef POWER_HPP
#define POWER_HPP


typedef struct {
    float power_A;
    float power_B;
    float power_C;
    float power_D;

} power_t;

void initPower();
void readPower(power_t* powers = nullptr, bool printPower = true);
#endif // POWER_HPP