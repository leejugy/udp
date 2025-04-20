#ifndef __TIMER_H__
#define __TIMER_H__

#include "define.h"
#include <time.h>
#include <signal.h>

typedef enum
{
    REPEAT_TIMER_NO,
    REPEAT_TIMER_YES,
} REPEAT_TIMER;

typedef struct
{
    timer_t timer_id;
    int repeat;
    struct timespec set_time;
    void (*expired_function)(union sigval val);
}timer_info_t;

int set_timer(timer_info_t *time_info);
int kill_timer(timer_info_t *time_info);
#endif