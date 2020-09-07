#include "pid.h"


void pid_init(pid_regulator_t* pid, float P_factor, float I_factor, float D_factor)
{
	pid->P_factor = P_factor;
	pid->I_factor = I_factor;
	pid->D_factor = D_factor;

	pid->P = 0;
	pid->I = 0;
	pid->D = 0;
	pid->last_error = 0;
	pid->last_time = 0;
}

float pid_update(pid_regulator_t* pid, float error, int time)
{
	float delta_time;
	delta_time = (time - pid->last_time) / 1000.0f;

	pid->P = error;
	pid->I += error * delta_time;
	pid->D = delta_time != 0.0f ? (error - pid->last_error) / delta_time : 0;

	float res;
	res = pid->P * pid->P_factor + pid->I * pid->I_factor + pid->D * pid->D_factor;

	pid->last_error = error;
	pid->last_time = time;

	return res;
}
