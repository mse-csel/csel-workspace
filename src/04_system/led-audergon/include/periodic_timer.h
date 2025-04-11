#ifndef TIMER_H_
#define TIMER_H_

#include <sys/timerfd.h>

typedef struct periodic_timer {
    int _tfd;
    struct itimerspec _spec;
    unsigned int _initial_period_ms;
} periodic_timer_t;


/**
 * Initialize and start a timer with the given period is ms.
 * 
 * @param t a pointer to a periodic_timer_t structure
 * @param period_ms the initial timer period is ms
 * 
 * @return -1 if an error occured 
 */
int periodic_timer_init(periodic_timer_t* t, unsigned int period_ms);

/**
 * Increase a timer's period and apply the change immediately
 * 
 * new period = old period + delta
 * 
 * @param t a pointer to a periodic_timer_t structure
 * @param period_ms the delta period is ms
 * 
 * @return -1 if an error occured 
 */
int periodic_timer_increase_period(periodic_timer_t* t, unsigned int delta_ms);

/**
 * Decrease a timer's period and apply the change immediately
 * 
 * new period = old period - delta
 * 
 * @param t a pointer to a periodic_timer_t structure
 * @param period_ms the delta period is ms
 * 
 * @return -1 if an error occured 
 */
int periodic_timer_decrease_period(periodic_timer_t* t, unsigned int delta_ms);

/**
 * Reset a timer's period to its initial value and apply the change immediately
 * 
 * 
 * @param t a pointer to a periodic_timer_t structure
 * 
 * @return -1 if an error occured 
 */
int periodic_timer_reset_period(periodic_timer_t* t);

#endif