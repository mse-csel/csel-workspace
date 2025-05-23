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
 * Abstract: System programming - multi-process - ex1
 *
 * Purpose: Safe sleep functions
 *
 * Autĥor:  Vincent Audergon, Bastien Veuthey
 * Date:    2025-05-23
 */

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