#include "pico/stdlib.h"
#include "Stepper.hpp"
#include "Power.hpp"
#include "Spiral.hpp"
#include "SPGD.hpp"


typedef struct {
    int spiral_turns;
    int spiral_spacing;
    int spgd_offset_range;
    float spgd_lr;
} auto_parameters_t;

void update_held_optics(optic_t* optics[], int num_held, float threshold);
void automatic_search_step(optic_t* optics[],int num_held, float threshold);
void bubble_sort_power(optic_t* sorted_optics[], int num_optics);
void set_default_parameters(auto_parameters_t* passive_parameters, auto_parameters_t* agressive_parameters);
void refine_parameters(auto_parameters_t* passive_parameters, auto_parameters_t* agressive_parameters);
void automatic_search_step(optic_t* optics[], int spiral_iterations, auto_parameters_t passive_parameters, auto_parameters_t agressive_parameters);
void auto_search_loop(optic_t* optics[],int num_held, float threshold, int iterations, auto_parameters_t passive_parameters, auto_parameters_t agressive_parameters);
