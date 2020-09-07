#include <stdlib.h>

#include "interface.h"
#include "aileron.h"

typedef struct aileron
{
    interface_t actions;

    float angle;
    float target;
    int is_move;

    int last_time;

    float top_limit;
    float low_limit;
    float speed;
} aileron_t;

void aileron_set_target(void* aileron, float target)
{
    aileron_t* ptr = (aileron_t*)aileron;
    if (!ptr)
        return;

    if (target > ptr->top_limit)
        ptr->target = ptr->top_limit;
    else if (target < ptr->low_limit)
        ptr->target = ptr->low_limit;
    else
        ptr->target = target;
}

float aileron_get_angle(void* aileron)
{
    aileron_t* ptr = (aileron_t*)aileron;
    if (!ptr)
        return 0;

    return ptr->angle;
}

void aileron_iteration(void* aileron, int time)
{
    aileron_t* ptr = (aileron_t*)aileron;
    if (!ptr)
        return;

    if (ptr->angle == ptr->target)
    {
        ptr->last_time = time;
        return;
    }

    float dir = ptr->angle - ptr->target;

    float delta = (time - ptr->last_time) / 1000.0f;
    float shift = ptr->speed * delta;

    if (dir > 0)
        ptr->angle -= shift;
    else
        ptr->angle += shift;

    ptr->last_time = time;
}

void* aileron_init()
{
    aileron_t* aileron = malloc(sizeof(aileron_t));

    if (aileron)
    {
        aileron->angle = 0.0f;
        aileron->target = 0.0f;
        aileron->is_move = 0;
        aileron->low_limit = -16.0f;
        aileron->top_limit = 22.0f;
        aileron->speed = 0.5f;

        aileron->actions.get_angle = aileron_get_angle;
        aileron->actions.set_target = aileron_set_target;
        aileron->actions.iteration = aileron_iteration;
    }

    return aileron;
}

void aileron_release(void* aileron)
{
    if (aileron)
        free(aileron);
}
