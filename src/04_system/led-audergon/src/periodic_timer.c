#include "periodic_timer.h"

#define MS_TO_NS_FACTOR (1000000)

int periodic_timer_init(periodic_timer_t* t, unsigned int period_ms) {
    memset(&t->_spec, 0, sizeof(t->_spec));
    t->_initial_period_ms = period_ms;
    t->_spec.it_interval.tv_nsec = period_ms * MS_TO_NS_FACTOR;
    t->_spec.it_value.tv_nsec = t->_spec.it_interval.tv_nsec;
    t->_tfd = timerfd_create(CLOCK_REALTIME, 0);
    if (t->_tfd == -1) {
        return -1;
    }
    return timerfd_settime(t->_tfd, 0, &t->_spec, NULL);
}

int periodic_timer_increase_period(periodic_timer_t* t, unsigned int delta_ms) {
    t->_spec.it_interval.tv_nsec += delta_ms * MS_TO_NS_FACTOR;
    t->_spec.it_value.tv_nsec = t->_spec.it_interval.tv_nsec;
    return timerfd_settime(t->_tfd, 0, &t->_spec, NULL);
}

int periodic_timer_decrease_period(periodic_timer_t* t, unsigned int delta_ms) {
    t->_spec.it_interval.tv_nsec -= delta_ms * MS_TO_NS_FACTOR;
    t->_spec.it_value.tv_nsec = t->_spec.it_interval.tv_nsec;
    return timerfd_settime(t->_tfd, 0, &t->_spec, NULL);
}

int periodic_timer_reset_period(periodic_timer_t* t) {
    t->_spec.it_interval.tv_nsec += t->_initial_period_ms * MS_TO_NS_FACTOR;
    t->_spec.it_value.tv_nsec = t->_spec.it_interval.tv_nsec;
    return timerfd_settime(t->_tfd, 0, &t->_spec, NULL);
}