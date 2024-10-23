#include "dbg.h"
#include "equation_solver.h"
#include "queue.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <time.h>
#include <math.h>
#include <omp.h>

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>



int main(){

    Queue* timeX = NULL;
    Queue* yPos = NULL;
    Queue* yPrime = NULL;

    if (init_logging("debug.log")!= 0){
        return 1;
    }

    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                Queue* timeX = createQueue();
            }
            #pragma omp section
            {
                Queue* yPos = createQueue();
            }
            #pragma omp section
            {
                Queue* yPrime = createQueue();
            }
        }
    }

    struct params* inputs = init_params();
    double t = 0.0, t1 = 100;
    double dt = 0.01;
    const int step_total = 500;

    int i = 0;
    clock_t start_time = clock(); //Record Start Time
    double timeout = 5.0; // Timeout in seconds

    gsl_odeiv2_system system = {func, NULL, 2, inputs};
    const gsl_odeiv2_step_type *type = gsl_odeiv2_step_rk8pd;
    gsl_odeiv2_step *step = gsl_odeiv2_step_alloc(type, 2);
    gsl_odeiv2_control *control = gsl_odeiv2_control_y_new(1e-6,0.0);
    gsl_odeiv2_evolve *evolve = gsl_odeiv2_evolve_alloc(2);
    double y[2] = {inputs->startPosition, inputs->startVelocity};

    while(t < t1){
        int status = gsl_odeiv2_evolve_apply(evolve, control, step, &system, &t, t1, &dt, y);

        assert(status == GSL_SUCCESS);
        if (status != GSL_SUCCESS){
            log_err("Error from GSL ODE solver: %s\n", gsl_strerror(status));
            break;
        }

        assert(!isnan(y[0]) || !isinf(y[0]));
        if(isnan(y[0]) || isinf(y[0]))
        {
            log_warn("Solver returned unstable results.\n");
        }

        #pragma omp parallel
        {
            #pragma omp sections
            {
                #pragma omp section
                    enqueue(timeX, t);
                #pragma omp section
                    enqueue(yPos, y[0]);
                #pragma omp section
                    enqueue(yPrime, y[1]);
            }
        }
        i++;

        clock_t current_time = clock();
        double elapsed_time = (double)(current_time - start_time) / CLOCKS_PER_SEC;
        assert(elapsed_time < timeout);
        if (elapsed_time > timeout){
            log_err("GSL solving loop timed out and was reset. ");
            gsl_odeiv2_evolve_reset(evolve);
            break;

    }

    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
                gsl_odeiv2_evolve_reset(evolve);
            #pragma omp section
                gsl_odeiv2_control_free(control);
            #pragma omp section
                 gsl_odeiv2_step_free(step);
            #pragma omp section
                gsl_odeiv2_evolve_free(evolve);
            #pragma omp section
                free(inputs);
            }
    }

    int SCREEN_WIDTH = 1280;
    int SCREEN_HEIGHT = 720;
    int i=0;

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, " Differential Field Plot. ");

    SetTargetFPS(60);

    while(!WindowShouldClose()){

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){

            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0)
        {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            camera.offset = GetMousePosition();
            camera.target = mouseWorldPos;
            const float zoomIncrement = 0.125f;
            camera.zoom += (wheel*zoomIncrement);
            if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
        }

        BeginDrawing();

            BeginMode2D(camera);

                ClearBackground(RAYWHITE);

                #pragma omp parallel for
                for(int i = -50; i <= 50; i++){
                    int screenX = i * 50;
                    if(i != 0)
                    {
                        DrawLine(screenX, 0 - 2500, screenX, 0 + 2500, GRAY);
                    } else {
                        DrawLine(screenX, 0 - 2500, screenX, 0 + 2500, BLACK);
                    }
                }

                #pragma omp parallel for
                for(int j = -50; j <=50; j++){
                    int screenY = j * 50;
                    if(j != 0)
                    {
                        DrawLine(0-2500, screenY, 0+2500, screenY, GRAY);
                    } else {
                        DrawLine(0-2500, screenY, 0+2500, screenY, BLACK);
                    }

                }

                Vector2 startPos;
                Vector2 endPos;
                float thickness = 2;
                double delta_h = 1;

                //sketch xy' = 2xy
                for(int i = -50; i <= 50; i++){
                    int screenX = i *50;
                    for(int j = -50; j <= 50; j++){
                        int screenY = j * 50;
                        DrawCircle(screenX, screenY, 5, GREEN);
                        double slope = ((2*j) / i);
                        startPos.x = screenX-25;
                        startPos.y = screenY-slope;
                        endPos.x = screenX+25;
                        endPos.y = screenY + slope;
                        DrawLineEx(startPos, endPos, thickness, BLUE);
                    }
                }
/*
                while(i <= total){
                    int screenX = timeX[i]*5 ;
                    int screenY1 = (SCREEN_HEIGHT/5) + yOne[i]*5;
                    int screenY2 = (SCREEN_HEIGHT/5) + yTwo[i]*5;

                    DrawCircle(screenX, screenY1, 2, GREEN);
                    DrawCircle(screenX, screenY2, 2, BLUE);
                    i++;

                }
                i = 0;
*/
            EndMode2D();

        EndDrawing();

    }

    CloseWindow();

    return 0;

}
}