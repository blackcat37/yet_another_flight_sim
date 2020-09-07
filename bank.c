#include <stdlib.h>

#include "interface.h"
#include "bank.h"
#include "pid.h"

typedef struct bank
{
    interface_t actions;

    float angle;
    float target;

    float max_left;
    float max_right;

    interface_t* aileron;

    int last_time;

    pid_regulator_t* pid;
} bank_t;

void bank_set_target(void* bank, float target)
{
    bank_t* ptr = (bank_t*)bank;
    if (!ptr)
        return;

    if (target > ptr->max_right)
        ptr->target = ptr->max_right;
    else if (target < ptr->max_left)
        ptr->target = ptr->max_left;
    else
        ptr->target = target;
}

float bank_get_angle(void* bank)
{
    bank_t* ptr = (bank_t*)bank;
    if (!ptr)
        return 0;

    return ptr->angle;
}

void bank_iteration(void* bank, int time)
{
    bank_t* ptr = (bank_t*)bank;
    if (!ptr)
        return;

    ptr->aileron->iteration(ptr->aileron, time);

    float aileron_angle = ptr->aileron->get_angle(ptr->aileron);

    //todo: if aileron_angle is out of range - skip this turn

    if (ptr->angle == ptr->target && aileron_angle == 0.0f)
    {
        ptr->last_time = time;
        return;
    }

    float aileron_efficiency = 0.5f; // aileron efficiency
    float damping_factor = 2.0f; // damping factor

    float bank_speed = (aileron_efficiency / damping_factor) * aileron_angle;
    float delta = (time - ptr->last_time) / 1000.0f;
    float shift = bank_speed * delta;

    ptr->angle -= shift;

    float angle_error = ptr->angle - ptr->target;
    float imp = pid_update(ptr->pid, angle_error, time);

    ptr->aileron->set_target(ptr->aileron, imp);

    ptr->last_time = time;
}

void* bank_init(void* aileron)
{
    bank_t* bank = malloc(sizeof(bank_t));

    if (bank)
    {
        bank->angle = 0.0f;
        bank->target = 0.0f;

        bank->max_left = -30.0f;
        bank->max_right = 30.0f;

        bank->aileron = (interface_t*)aileron;

        bank->last_time = 0;

        bank->pid = malloc(sizeof(pid_regulator_t));
        pid_init(bank->pid, 0.5f, 0, 1.0f);

        bank->actions.set_target = bank_set_target;
        bank->actions.get_angle = bank_get_angle;
        bank->actions.iteration = bank_iteration;
    }

    return bank;
}

void bank_release(void* bank)
{
    if (!bank)
        return;

    if (((bank_t*)bank)->pid)
        free(((bank_t*)bank)->pid);

    free(bank);
}
