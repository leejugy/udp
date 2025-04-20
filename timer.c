#include "timer.h"

int set_timer(timer_info_t *time_info)
{
    struct itimerspec timer_st = {0, };
    struct sigevent sig_ev = {0, };

    memcpy(&timer_st.it_value, &time_info->set_time, sizeof(time_info->set_time));

    if(time_info->repeat == REPEAT_TIMER_YES)
    {
        memcpy(&timer_st.it_interval, &time_info->set_time, sizeof(time_info->set_time));
    }

    sig_ev.sigev_notify = SIGEV_THREAD;
    sig_ev.sigev_signo = 0;
    sig_ev.sigev_value.sival_ptr = NULL;
    sig_ev.sigev_notify_function = time_info->expired_function;
    sig_ev.sigev_notify_attributes = NULL;

    if(timer_create(CLOCK_REALTIME, &sig_ev, &time_info->timer_id) < 0)
    {
        perror("set timer fail to create timer");
        return -1;
    }

    if(timer_settime(time_info->timer_id, 0, &timer_st, NULL) < 0)
    {
        perror("set timer fail to set timer");
        return -1;
    }

    return 1;
}

int kill_timer(timer_info_t *time_info)
{
    struct itimerspec timer_st = {0, };

    if(timer_settime(time_info->timer_id, 0, &timer_st, NULL) < 0)
    {
        perror("kill timer fail to set timer");
        return -1;
    }

    if(timer_delete(time_info->timer_id) < 0)
    {
        perror("kill timer fail to kill timer");
        return -1;
    }
    
    return 1;
}