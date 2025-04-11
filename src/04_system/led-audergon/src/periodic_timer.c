/**
 * Copyright 2025 University of Applied Sciences Western Switzerland / Fribourg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Project: HEIA-FR / HES-SO MSE - MA-CSEL1 Laboratory
 *
 * Abstract: System programming -  file system
 *
 * Autĥor:  Vincent Audergon
 * Date:    11.04.2025
 */

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