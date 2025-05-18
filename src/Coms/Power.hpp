#include "pico/stdlib.h"


typedef struct {
    float power_A;
    float power_B;
    float power_C;
    float power_D;

} power_t;

void innitPower();

void readPower();