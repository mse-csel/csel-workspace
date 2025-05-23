#ifndef SAFE_SLEEP_H_
#define SAFE_SLEEP_H_

/**
 * Sleep that is safe from EINTR signal.
 * @param ms milliseconds to sleep
 */
void safe_sleep_ms(int ms);

/**
 * Sleep that is safe from EINTR signal.
 * @param s seconds to sleep
 */
void safe_sleep_s(int s);

#endif // SAFE_SLEEP_H_