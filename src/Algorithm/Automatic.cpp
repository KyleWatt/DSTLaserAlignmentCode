#include "automatic.hpp"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>



void update_held_optics(optic_t* optics[], int num_held, float threshold) {
    power_t current_power = get_latest_power();
    optics[0]->cur_power = current_power.power_A;
    optics[1]->cur_power = current_power.power_B;
    optics[2]->cur_power = current_power.power_C;
    optics[3]->cur_power = current_power.power_D;
    bool all_held = true;
    for (int i = 0; i < 4; i++) {
        if (optics[i]->cur_power < threshold) {
            //optics power lower than threshold
            all_held = false;
            if (optics[i]->cur_power < threshold*0.8){
                //Search Agressively
                optics[i]->search = AGRESSIVE;
            } else {
                //Search Passively
                optics[i]->search = PASSIVE;
            }
        }else{
            //Greater than threshold
            optics[i]->search = MAINTAIN;
        }
    }
    if (all_held && num_held > 0) {
        int indices[4] = {0, 1, 2, 3};
        // Simple bubble sort on indices based on power
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3 - i; j++) {
                if (optics[indices[j]]->cur_power > optics[indices[j+1]]->cur_power) {
                    int temp = indices[j];
                    indices[j] = indices[j+1];
                    indices[j+1] = temp;
                }
            }
        }
        
        // Set worst num_held to PASSIVE optimization
        for (int i = 0; i < num_held && i < 4; i++) {
            optics[indices[i]]->search = PASSIVE;
            printf("Optic %d: Optimizing worst (power=%.2f)\n", indices[i], optics[indices[i]]->cur_power);
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

void refine_parameters(auto_parameters_t* passive_parameters, auto_parameters_t* agressive_parameters){
    // Adapt parameters for next iteration - progressively finer
    // Passive parameters (gentle refinement)
    passive_parameters->spiral_spacing = (passive_parameters->spiral_spacing > 2) ?  passive_parameters->spiral_spacing - 1 : 2;
    passive_parameters->spiral_turns = (passive_parameters->spiral_turns > 2) ?  passive_parameters->spiral_turns - 1 : 2;
    passive_parameters->spgd_offset_range = (passive_parameters->spgd_offset_range > 1) ?  passive_parameters->spgd_offset_range - 1 : 1;
    passive_parameters->spgd_lr *= 0.9f;  // Reduce learning rate by 10%
        
    // agressive parameters (wider refinement)
    agressive_parameters->spiral_spacing = (agressive_parameters->spiral_spacing > 5) ? agressive_parameters->spiral_spacing - 2 : 5;
    agressive_parameters->spiral_turns = (agressive_parameters->spiral_turns > 3) ? agressive_parameters->spiral_turns - 1 : 3;
    agressive_parameters->spgd_offset_range = (agressive_parameters->spgd_offset_range > 3) ? agressive_parameters->spgd_offset_range - 1 : 3;
    agressive_parameters->spgd_lr *= 0.85f;  // Reduce learning rate by 15%
}

void automatic_search_step(optic_t* optics[], int spiral_iterations, auto_parameters_t passive_parameters, auto_parameters_t agressive_parameters) {
    for (int iters = 0; iters < spiral_iterations; iters++){
        float t = (float)iters / (spiral_iterations - 1);
        for (uint i = 0; i < 4; i++){
            if (optics[i]->search == PASSIVE){
                //set parameters tp passive
                float angle = t * 2 * M_PI * passive_parameters.spiral_turns;
                calculate_spiral_step(optics[i], passive_parameters.spiral_spacing, angle, t);
            } else if (optics[i]->search == AGRESSIVE){
                //set parameters tp passive
                float angle = t * 2 * M_PI * agressive_parameters.spiral_turns;
                calculate_spiral_step(optics[i], agressive_parameters.spiral_spacing, angle, t);
            }   
        }
        motors_move(optics);
    }
    go_to_max_pos(optics);


    //Start of SPGD 
    //Step positive peturb for each optic
    for (uint i = 0; i < 4; i++){
        if (optics[i]->search == PASSIVE){
            random_offset_peturb(optics[i], passive_parameters.spgd_offset_range);
        } else if (optics[i]->search == AGRESSIVE){
            random_offset_peturb(optics[i], agressive_parameters.spgd_offset_range);
        }
    }
    motors_move(optics);
    //Get power after peturb
    for (uint i = 0; i < 4; i++){
        optics[i]->cost_plus = optics[i]->cur_power;
    }
    //Step reverse for all optics
    for (uint i = 0; i < 4; i++){
        if (optics[i]->search == PASSIVE){
            random_offset_reverse(optics[i], passive_parameters.spgd_offset_range);
        } else if (optics[i]->search == AGRESSIVE){
            random_offset_reverse(optics[i], agressive_parameters.spgd_offset_range);
        }
    }
    motors_move(optics);
    //get power after reverse
    for (uint i = 0; i < 4; i++){
        optics[i]->cost_minus = optics[i]->cur_power;
    }
    //calculate gradient for each optic
    for (uint i = 0; i < 4; i++){
        if (optic->search == PASSIVE){
            optics[i]->grad_approx = (optics[i]->cost_plus - optics[i]->cost_minus) / (2 * passive_parameters.spgd_offset_range);
        } else if (optic->search == AGRESSIVE){
            optics[i]->grad_approx = (optics[i]->cost_plus - optics[i]->cost_minus) / (2 * agressive_parameters.spgd_offset_range);
        }
    }
    //calculate step with gradient and lr
   for (uint i = 0; i < 4; i++){
        if (optics[i]->search == PASSIVE){
            spgd_automatic(optics[i],passive_parameters.spgd_lr);
        } else if (optics[i]->search == AGRESSIVE){
            spgd_automatic(optics[i], agressive_parameters.spgd_lr);
        }
    }
    //step all motors without blocking
    motors_move(optics);
    //for each optic go to where best power was found on that optic
    go_to_max_pos(optics);

}

void auto_search_loop(optic_t* optics[],int num_held, float threshold, int iterations, auto_parameters_t passive_parameters, auto_parameters_t agressive_parameters){
    //find optics to optimise
    update_held_optics(optics, num_held, threshold);
    //default automatic variables
    int spiral_iterations = 200;
    set_default_parameters(&passive_parameters, &agressive_parameters);
    //iterate to optmise non held optics
    for (int i = 0; i < iterations; i++){
        automatic_search_step(optics, spiral_iterations, passive_parameters, agressive_parameters);
        refine_parameters(&passive_parameters, &agressive_parameters);
    }
    
}