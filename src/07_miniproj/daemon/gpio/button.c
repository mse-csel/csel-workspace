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
 * Project: HEIA-FR / HES-SO MSE - MA-CSEL1 Mini Project
 *
 * Abstract: Button GPIO-based library
 * Button handling library for GPIO-based button input management.
 * Provides functionality to monitor multiple buttons using Linux sysfs
 * GPIO interface with event-driven detection for button presses using epoll.
 *
 * Author:  Bastien Veuthey
 * Date:    07.06.2025
 */

#include "button.h"
#include "gpio.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>

/**
 * Initialize button context structure
 * Clears all button data and resets counters to prepare for button registration
 * @param ctx Pointer to button context to initialize
 * @return 0 on success, -1 on error (null pointer)
 */
int button_ctx_init(button_ctx_t *ctx)
{
    if (!ctx) {
        return -1;
    }
    
    memset(ctx, 0, sizeof(*ctx));
    ctx->epfd = epoll_create1(0);
    if (ctx->epfd < 0) {
        perror("epoll_create1");
        return -1;
    }
    ctx->pending = 0;
    return 0;
}

/**
 * Set callback functions for button events
 * @param ctx Button context
 * @param press_cb Callback for button press events (can be NULL)
 * @param release_cb Callback for button release events (can be NULL)
 */
void button_set_callbacks(button_ctx_t *ctx, button_callback_t press_cb, button_callback_t release_cb)
{
    if (!ctx) {
        return;
    }
    
    ctx->press_callback = press_cb;
    ctx->release_callback = release_cb;
}

/**
 * Set user data passed to callback functions
 * @param ctx Button context
 * @param user_data User data pointer
 */
void button_set_user_data(button_ctx_t *ctx, void *user_data)
{
    if (!ctx) {
        return;
    }
    
    ctx->user_data = user_data;
}

/**
 * Add a button to the monitoring context
 * Exports GPIO pin, configures it for input with both edge detection,
 * and adds it to the button context for polling
 * @param ctx Button context to add button to
 * @param pin GPIO pin number for the button
 * @param name Human-readable name for the button (e.g., "K1", "K2")
 * @return 0 on success, -1 on error
 */
int button_add(button_ctx_t *ctx, uint8_t pin, button_id_t id, const char *name)
{
    if (!ctx || !name || ctx->count >= MAX_BUTTONS) {
        return -1;
    }
    
    if (gpio_export(pin) != GPIO_SUCCESS) {
        return -1;
    }
    
    if (gpio_set_direction(pin, GPIO_DIRECTION_IN) != GPIO_SUCCESS) {
        gpio_unexport(pin);
        return -1;
    }
    
    // Set edge detection to both rising and falling for press/release detection
    if (gpio_set_edge(pin, GPIO_EDGE_BOTH) != GPIO_SUCCESS) {
        gpio_unexport(pin);
        return -1;
    }
    
    int fd = gpio_open_fd(pin);
    if (fd < 0) {
        gpio_unexport(pin);
        return -1;
    }
    
    // Read initial value to clear any pending events and set initial state
    char buf;
    lseek(fd, 0, SEEK_SET);
    read(fd, &buf, 1);
    
    button_t *btn = &ctx->buttons[ctx->count];
    btn->pin = pin;
    btn->name = name;
    btn->id = id;
    btn->fd = fd;
    btn->last_state = (buf == '1') ? 1 : 0;  // 1=released, 0=pressed (active low)

    // Initialize epoll event for this button
    struct epoll_event ev = {0};
    ev.events = EPOLLPRI | EPOLLERR;
    ev.data.u32 = ctx->count;
    if (epoll_ctl(ctx->epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        perror("epoll_ctl");
        gpio_close_fd(fd);
        gpio_unexport(pin);
        return -1;
    }

    ctx->count++;
    return 0;
}

/**
 * TODO
 * @param ctx Button context containing buttons to monitor
 * @param timeout_ms Timeout in milliseconds (-1 for infinite wait)
 * @return Number of file descriptors with events, 0 on timeout, -1 on error
 */
int button_poll(button_ctx_t *ctx, int timeout_ms)
{
    if (!ctx || ctx->count == 0) {
        return -1;
    }

    int n = epoll_wait(ctx->epfd, ctx->events, ctx->count, timeout_ms);
    if (n >= 0)
        ctx->pending = n;
    return n;
}

/**
 * Process button events after poll() indicates activity
 * Reads button states and calls the appropriate callbacks for press/release events
 * @param ctx Button context with pending events
 * @return Number of events handled, -1 on error
 */
int button_handle_events(button_ctx_t *ctx)
{
    if (!ctx) {
        return -1;
    }

    int events_handled = 0;

    for (int i = 0; i < ctx->pending; i++) {
        int idx = ctx->events[i].data.u32;
        if ((size_t)idx >= ctx->count)
            continue;

        lseek(ctx->buttons[idx].fd, 0, SEEK_SET);
        char val;
        if (read(ctx->buttons[idx].fd, &val, 1) > 0) {
            int current_state = (val == '1') ? 1 : 0;

            if (current_state != ctx->buttons[idx].last_state) {
                if (current_state == 0) {
                    if (ctx->release_callback)
                        ctx->release_callback(&ctx->buttons[idx], ctx->user_data);
                } else {
                    if (ctx->press_callback)
                        ctx->press_callback(&ctx->buttons[idx], ctx->user_data);
                }

                ctx->buttons[idx].last_state = current_state;
                events_handled++;
            }
        }
    }

    ctx->pending = 0;
    return events_handled;
}

/**
 * Clean up button context and release GPIO resources
 * Closes file descriptors and unexports all GPIO pins
 * @param ctx Button context to clean up
 */
void button_cleanup(button_ctx_t *ctx)
{
    if (!ctx) {
        return;
    }
    
    for (size_t i = 0; i < ctx->count; i++) {
        gpio_close_fd(ctx->buttons[i].fd);
        gpio_unexport(ctx->buttons[i].pin);
    }

    if (ctx->epfd >= 0)
        close(ctx->epfd);

    memset(ctx, 0, sizeof(*ctx));
}