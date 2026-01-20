#include "automatic.hpp"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>

//Default automatic values


void bubble_sort_power(optic_t* sorted_optics[], int num_optics){
    for (int i = 0; i < num_optics - 1; i++) {
        for (int j = 0; j < num_optics - i - 1; j++) {
          
            // Swap if the element found is
          	// greater than the next element
            if (sorted_optics[j] > sorted_optics[j + 1]) 
                swap(sorted_optics[j],sorted_optics[j+1]);
        }
    }
}

void update_held_optics(optic_t* optics[], int num_held, float threshold) {
    power_t current_power = get_latest_power();
    optics[0]->cur_power, optics[1]->cur_power, optics[2]->cur_power, optics[3]->cur_power = current_power.power_A,current_power.power_B
                                                                                           current_power.power_C,current_power.power_D;
    bool all_held = true;
    for (optic_t optic : optics ){
        if (optic->cur_power < threshold){
            //optics power lower than threshold
            all_held = false;
            if (optic->cur_power < threshold*0.8){
                //Search Agressively
                optic->search = AGRESSIVE;
            } else {
                //Search Passively
                optic->search = PASSIVE;
            }
        }else{
            //Greater than threshold
            optic->search = MAINTAIN;
        }
    }
    if (all_held){
        //find lowest and optimise them
        optic_t* sorted_optics[4] = optics;
        bubble_sort_power(sorted_optics, 4)
        for (int i = num_held, i > (4 - num_held), --i){
            sorted_optics[i]-> PASSIVE;
        }
    }
}


void set_default_parameters(auto_parameters_t* passive_parameters, auto_parameters_t* agressive_parameters){
    //default passive parameters
    passive_parameters->spgd_lr = 0.2;
    passive_parameters->spgd_offset_range =5;
    passive_parameters->spiral_spacing = 5;
    passive_parameters->spiral_turns = 5;
    //default agressive parameters
    agressive_parameters->spgd_lr = 0.4;
    agressive_parameters->spgd_offset_range =10;
    agressive_parameters->spiral_spacing = 10;
    agressive_parameters->spiral_turns = 10;
}

void automatic_search_step(optic_t* optics[],int num_held, float threshold, int iterations) {
    //default automatic variables
    auto_parameters_t passive_parameters, agressive_parameters;
    set_default_parameters(&passive_parameters, &agressive_parameters)
    update_held_optics(optic_t* optics[], float threshold)
    for (int i = 0; i < iterations; i++){
        float t = (float)i / (iterations - 1);
        for (optic_t optic : optics ){
            if (optic->search == PASSIVE){
                //set parameters tp passive
                float angle = t * 2 * M_PI * passive_parameters->spiral_turns;
                calculate_spiral_step(optic, passive_parameters->spiral_spacing, angle, t);
            } else if (optic->search == AGRESSIVE){
                //set parameters tp passive
                float angle = t * 2 * M_PI * agressive_parameters->spiral_turns;
                calculate_spiral_step(optic, agressive_parameters->spiral_spacing, angle, t);
            }   
        }
        motors_move(optics);
    }
    go_to_max_pos(optics);


    //Start of SPGD 
    for (optic_t optic : optics ){
        if (optic->search == PASSIVE){
            random_offset_peturb(optic, passive_parameters->spgd_offset_range);
        } else if (optic->search == AGRESSIVE){
            random_offset_peturb(optic, agressive_parameters->spgd_offset_range);
        }
    }
    motors_move(optics);

    for (optic_t optic : optics ){
        if (optic->search == PASSIVE){
            random_offset_reverse(optic, passive_parameters->spgd_offset_range);
        } else if (optic->search == AGRESSIVE){
            random_offset_reverse(optic, agressive_parameters->spgd_offset_range);
        }
    }
    motors_move(optics);
    for (uint i = 0; i < 4; i++){
        optics[i]->cost_minus = optics[i]->cur_power;
    }
    for (uint i = 0; i < 4; i++){
        optics[i]->grad_approx = (cost_plus[i] - cost_minus[i]) / (2 * offset_range);
    }

    for (optic_t optic : optics ){
        if (optic->search == PASSIVE){
            spgd_automatic(optic,passive_parameters->spgd_lr)
        } else if (optic->search == AGRESSIVE){
            spgd_automatic(optic, agressive_parameters->spgd_lr)
        }
    }
    motors_move(optics);

    // Adapt parameters for finer search
   
    // passive_turns = passive_turns > 1 ? passive_turns - 1 : 1;
    // auto_spiral_spacing = auto_spiral_spacing > 1 ? auto_spiral_spacing - 1 : 1;
    // auto_spgd_window = auto_spgd_window > 1 ? auto_spgd_window - 1 : 1;
    // auto_spgd_lr *= 0.8;
}