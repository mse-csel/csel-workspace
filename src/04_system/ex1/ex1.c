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
 * Purpose: Parent-child process communication using socketpair
 * Child => Socketpair => Parent
 *             ^^^
 *           Messages
 * Producer               Consumer (print)
 * 
 * If the parent receives "exit" from the child, it terminates the program.
 * 
 * Ignores the following signals:
 * - SIGHUP
 * - SIGINT
 * - SIGQUIT
 * - SIGABRT
 * - SIGTERM
 *
 * Autĥor:  Vincent Audergon, Bastien Veuthey
 * Date:    2025-05-23
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

static const int kIgnoreSignals[] = {
    SIGHUP,  // Hangup
    SIGINT,  // Interrupt (Ctrl+C)
    SIGQUIT, // Quit (Ctrl+\)
    SIGABRT, // Abort signal from abort()
    SIGTERM  // Termination signal
};

/**
 * Configures the signal handler for the specified signal
 * @param signo The signal number to configure
 * @param handler The signal handler function
 */
void config_signal(int signo, void (*handler)(int))
{
	struct sigaction act = { .sa_handler = handler,};
    if (sigaction(signo, &act, NULL) == -1) {
        perror("sigaction");
    }
}

/**
 * Signal handler function, prints the signal number and process ID
 * @param signo The signal number
 */
void catch_signal(int signo)
{
    int pid = getpid();
    printf("Signal %d ignored from PID: %d\n", signo, pid);
}

/**
 * @brief Function to handle the child process
 * Produce messages that are sent to the parent process
 * @param socket_fd The socket file descriptor for communication
 */
void child_process(int socket_fd)
{
    const char *messages[] = {
        "Message 1 from child",
        "Message 2 from child",
        "Message 3 from child",
        "exit"
    };
    int num_messages = sizeof(messages) / sizeof(messages[0]);
    ssize_t i = 0;
    while (i < num_messages) {
        ssize_t sent_bytes = send(socket_fd, messages[i], strlen(messages[i]), 0);
        if (sent_bytes == -1) {
            if (errno == EINTR) {
                printf("send interrupted by signal, retrying...\n");
                // Retry if interrupted by a signal
                continue;
            } else {
                perror("send");
                break;
            }
        }
        i++;
        safe_sleep_s(1); // Sleep for 1 second
    }
    close(socket_fd);
    exit(EXIT_SUCCESS);
}

/**
 * @brief Function to handle the parent process
 * Receives messages from the child process and prints them
 * @param socket_fd The socket file descriptor for communication
 */
void parent_process(int socket_fd)
{
    char buffer[256];
    ssize_t bytes_received;
    while (1) {
        bytes_received = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received == -1) {
            if (errno == EINTR) {
                continue; // Retry if interrupted by a signal
            } else {
                perror("recv");
                break; // Exit on error
            }
        }
        buffer[bytes_received] = '\0'; // Null-terminate the string
        if (strcmp(buffer, "exit") == 0) {
            printf("Child process sent exit command. Exiting...\n");
            break;
        }
        printf("Received: %s\n", buffer);
    }
    close(socket_fd);
}

/**
 * @brief Main function
 * Creates a socketpair for communication between parent and child processes
 */
int main()
{
    // Ignore signals
    for (size_t i = 0; i < sizeof(kIgnoreSignals) / sizeof(kIgnoreSignals[0]); ++i) {
        config_signal(kIgnoreSignals[i], catch_signal);
    }
    // Create socketpair for inter-process communication
    int socket_fd[2];
    int err = socketpair(AF_UNIX, SOCK_STREAM, 0, socket_fd);
    if (err == -1) {
        perror("socketpair");
        return EXIT_FAILURE;
    }
    pid_t pid = fork();
    if (pid == 0) {
        close(socket_fd[0]);
        child_process(socket_fd[1]);
    } else if (pid > 0) {
        close(socket_fd[1]);
        parent_process(socket_fd[0]);
        wait(NULL);
    } else {
        perror("fork failed");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}