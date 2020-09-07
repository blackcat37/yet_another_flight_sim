#pragma once

typedef void (*func_set_target)(void*, float);
typedef float (*func_get_angle)(void*);
typedef void (*func_iter)(void*, int time);

typedef struct interface
{
    func_set_target set_target;
    func_get_angle get_angle;
    func_iter iteration;
} interface_t;
