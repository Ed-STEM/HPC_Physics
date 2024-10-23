#include "equation_solver.h"
#include "dbg.h"

#include <math.h>

int func(double t, const double y[], double f[], void *params){
    //(void) (t);
    struct params *init  = (struct params*)params;
    // Starting velocity
    f[0] = y[1];
    // Free Fall
    f[1] = -(init->damping)*y[1] - (init->g / init->length)*sin(y[0]) - (init->damping2)*sin(init->omega)*t;
    return GSL_SUCCESS;
}

struct params* init_params(void){
    struct params  *inputs = malloc(sizeof(struct params));
    check_mem(inputs, "init_params failed to create inputs struct. ");

    if (inputs != NULL){
    inputs->g=9.8;
    inputs->length=2;
    inputs->damping=0.5;
    inputs->damping2=0.0;
    inputs->omega=0.7;
    inputs->startPosition = 0.78;
    inputs->startVelocity = 50;

    }
}

