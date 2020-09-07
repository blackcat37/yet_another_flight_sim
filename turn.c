#include <stdlib.h>
#include <math.h>

#include "interface.h"
#include "turn.h"
#include "pid.h"

typedef struct course
{
    interface_t actions;

    float heading;
    float target;

    float plane_speed;

    interface_t* bank;

    int last_time;

    pid_regulator_t* pid; 
} course_t;

void turn_set_target(void* course, float target)
{
    course_t* ptr = (course_t*)course;
    if (!ptr)
        return;

    ptr->target = target;
}

float turn_get_heading(void* course)
{
    course_t* ptr = (course_t*)course;
    if (!ptr)
        return 0;

    return ptr->heading;
}

void turn_iteration(void* course, int time)
{
    course_t* ptr = (course_t*)course;
    if (!ptr)
        return;

    ptr->bank->iteration(ptr->bank, time);

    float bank_angle = ptr->bank->get_angle(ptr->bank);

    //todo: if bank_angle is out of range - skip this turn

    if (ptr->heading == ptr->target && bank_angle == 0.0f)
    {
        ptr->last_time = time;
        return;
    }

    float turn_speed = (float)(((9.81 * tan(bank_angle * 0.01745)) / ptr->plane_speed) * 57.296);

    float delta = (time - ptr->last_time) / 1000.0f;
    float shift = turn_speed * delta;

    ptr->heading += shift;

    float angle_error = ptr->target - ptr->heading;
    float imp = pid_update(ptr->pid, angle_error, time);

    float new_turn_speed = (float)(((9.81 * tan(imp * 0.01745)) / ptr->plane_speed) * 57.296);
    float new_load = (float)(1 / cos(imp * 0.01745));

    if (fabs(new_turn_speed) > 3.0f || fabs(new_load) > 1.5f)
    {
        if (imp < 0)
            imp += 1;
        else if (imp > 0)
            imp -= 1;
        new_turn_speed = (float)(((9.81 * tan(imp * 0.01745)) / ptr->plane_speed) * 57.296);
        new_load = (float)(1 / cos(imp * 0.01745));
    }

    ptr->bank->set_target(ptr->bank, imp);// (ptr->heading - ptr->target) * -1);

    ptr->last_time = time;
}

void* turn_init(void* bank, float plane_speed)
{
    course_t* course = malloc(sizeof(course_t));

    if (course)
    {
        course->heading = 0.0f;
        course->target = 0.0f;
        course->last_time = 0;
        course->plane_speed = plane_speed;
        course->bank = (interface_t*)bank;

        course->pid = malloc(sizeof(pid_regulator_t));
        pid_init(course->pid, 1.0f, 0, 0.5f);

        course->actions.set_target = turn_set_target;
        course->actions.get_angle = turn_get_heading;
        course->actions.iteration = turn_iteration;
    }

    return course;
}

void turn_release(void* course)
{
    if (!course)
        return;

    if (((course_t*)course)->pid)
        free(((course_t*)course)->pid);

    free(course);
}
