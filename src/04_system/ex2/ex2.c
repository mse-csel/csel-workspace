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
 * Abstract: System programming - multi-process - ex2
 *
 * Purpose: Limit memory usage using cgroups
 * 
 *
 * Autĥor:  Vincent Audergon, Bastien Veuthey
 * Date:    2025-05-23
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BLOCK_SIZE (1024 * 1024)  // 1 MiB
#define NUM_BLOCKS 50

int main() {
    void *blocks[NUM_BLOCKS];

    for (int i = 0; i < NUM_BLOCKS; ++i) {
        blocks[i] = malloc(BLOCK_SIZE);
        if (blocks[i] == NULL) {
            perror("malloc");
            break;
        }
        memset(blocks[i], 0, BLOCK_SIZE);
        printf("Bloc %d allocated and filled with zeros\n", i);
    }
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        if (blocks[i] != NULL) {
            free(blocks[i]);
            blocks[i] = NULL;
            printf("Bloc %d freed\n", i);
        }
    }
    printf("OK\n");
    return EXIT_SUCCESS;
}
