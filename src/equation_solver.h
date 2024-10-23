#ifndef __equation_solver__h
#define __equation_solver__h

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv2.h>

struct params{
    double g;
    double length;
    double damping;
    double damping2;
    double omega;
    double startPosition;
    int startVelocity;
};

int func(double t, const double y[], double f[], void *params);

struct params* init_params(void);

void gsl_run(struct params* inputs);

#endif