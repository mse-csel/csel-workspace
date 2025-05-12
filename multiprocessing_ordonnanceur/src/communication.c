/**
 * Copyright 2018 University of Applied Sciences Western Switzerland / Fribourg
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
 * Purpose: NanoPi silly status led control system
 *
 * Author:  Jonathan Amez-Droz
 * Date:    12.05.2025
 */

#define _POSIX_C_SOURCE 200809L
#include "communication.h"
#include <signal.h>

void comm_process(){
    int sockets[2];
    int fd[2];

    int err = pipe(fd);
    if (err == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Créer un socket pair
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1) {
        perror("socketpair");
        exit(EXIT_FAILURE);
    }

    // setup capturing signals
    struct sigaction act = {
        .sa_handler = catch_signal,
        .sa_flags = 0
    };
    sigemptyset(&act.sa_mask);

    // Set up signal handlers for the signals we want to catch
    int signals[] = {SIGHUP, SIGINT, SIGQUIT,
                    SIGABRT, SIGTERM};
    int num_signals = sizeof(signals)/sizeof(signals[0]);   

    for (int i = 0; i < num_signals; i++){
        if (sigaction(signals[i], &act, NULL) == -1) {
            perror("sigaction");
            exit(EXIT_FAILURE);
        }
        
    }

    printf("Signal handlers set up. PID: %d\n", getpid());

    // Fork with all previous data
    pid_t pid = fork();

    // les deux processus tournent en parallèle
    if (pid == 0){ //enfant 
        printf("child process %d\n", getpid());
        close(fd[0]); // close unused read descriptor


        // Array of messages to send
        const char *messages[] = {
            "hello from child process",
            "CSEL c'est super",
            "They are taking the hobbits to Isengard",
            "42",
            "J'ai perdu!",
            "exits",
            "Oups c'est pas ça",
            "exist",
            "toujours pas ça",
            "Comment on quitte???",
            "Ah oui je sais!",
            "Bye :D",
            "exit"
        };

        int num_messages = sizeof(messages) / sizeof(messages[0]);

        // Loop over the messages and send them
        for (int i = 0; i < num_messages; i++) {
            send_message(fd[1], messages[i]);
            sleep(3);
        }
        exit(EXIT_FAILURE); // should never reach here

    }
    else if (pid>0){ //parent
        printf("parent process %d\n", getpid());
        close(fd[1]); // close unused write descriptor
        while(1){
            char msg[100];
            int len;
            while (((len = read(fd[0], msg, sizeof(msg) - 1)) == -1) 
            && (errno == EINTR)) {}

            if (len > 0) {
                msg[len] = '\0'; // Null-terminate the string
                printf("Received message: %s\n", msg);
            }
            else{
                perror("read");
                exit(EXIT_FAILURE);
            }
            // exit signal received from child
            if (strcmp(msg, "exit") == 0) {
                printf("Exiting parent process\n");
                break;
            }
            
        }
    }
    else{//error
        perror("fork");
        exit(EXIT_FAILURE);
    } 


}

void catch_signal(int sig){
    printf("Signal %d received\n", sig);
    printf("You have no power here!\n");
    // Handle the signal here

    //re-install signal handler
    struct sigaction act = {
        .sa_handler = catch_signal,
        .sa_flags = 0
    };
    sigemptyset(&act.sa_mask);
    sigaction(sig, &act, NULL);
}

void send_message(int fd, const char *message) {
    // Send the message to the other process
    if (write(fd, message, strlen(message) + 1) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
}



