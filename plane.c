#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <conio.h>

#include "interface.h"
#include "aileron.h"
#include "bank.h"
#include "turn.h"

void print_state(interface_t* course, float target, interface_t* bank, interface_t* aileron, const char* input)
{
    float aileron_angle = aileron->get_angle(aileron);
    float bank_angle = bank->get_angle(bank);
    float heading = course->get_angle(course);

    printf("\033[2Kaileron:\t%2.3f\n", aileron_angle);
    printf("\033[2Kbank:\t\t%2.3f\n", bank_angle);
    printf("\033[2Kheading:\t%2.3f\ttarget:\t%2.3f\n\n", heading, target);
    printf("\033[2K>%s", input);
    printf("\033[F\033[F\033[F\033[F");
}

float user_input(char* new_course, int* c_cnt)
{
    float res = -999999.0f;

    if (_kbhit())
    {
        int c = _getch();
        if (c == 13)
        {
            if (*c_cnt > 0)
            {
                res = (float)atof(new_course);
                memset(new_course, 0, sizeof(char) * 32);
                *c_cnt = 0;
            }
        }
        else if (c == 8)
        {
            if (*c_cnt > 0)
            {
                new_course[*c_cnt - 1] = 0;
                *c_cnt -= 1;
            }
        }
        else
        {
            if (*c_cnt <= 31)
            {
                new_course[*c_cnt] = c;
                *c_cnt += 1;
            }
        }
    }

    return res;
}

void main_loop(interface_t* course, interface_t* bank, interface_t* aileron)
{
    clock_t start = clock();
    float target = 0;

    char new_course[32] = { '\0', };
    int c_cnt = 0;

    while (1)
    {
        int time = (int)((((float)(clock() - start)) / CLOCKS_PER_SEC) * 1000.0f);

        print_state(course, target, bank, aileron, new_course);

        float u = user_input(new_course, &c_cnt);
        if (u != -999999.0f)
            target = u;

        course->set_target(course, target);

        course->iteration(course, time);
    }
}

int main()
{
    void* aileron = aileron_init();
    void* bank = bank_init(aileron);
    void* course = turn_init(bank, 200);

    printf("\033[?25l");

    main_loop(course, bank, aileron);

    bank_release(bank);
    aileron_release(aileron);
    turn_release(course);

    return 0;
}