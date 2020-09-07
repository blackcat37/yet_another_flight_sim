#pragma once

typedef struct pid_regulator
{
    float P_factor;
    float I_factor;
    float D_factor;

    float P;
    float I;
    float D;

    float last_error;
    int last_time;
} pid_regulator_t;

void pid_init(pid_regulator_t* pid, float P_factor, float I_factor, float D_factor);
float pid_update(pid_regulator_t* pid, float error, int time);