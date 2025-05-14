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

 #include "cgroups.h"

 void mount_cgroup_memory(){
    // mount cgroup filesystem
    if(mount("none", CGROUPS_PATH, "tmpfs", 0, NULL ) == -1){
        perror("mount cgroup");
        exit(EXIT_FAILURE);
    }
    struct stat st = {0};
    // create directory
    if (stat("/sys/fs/cgroup/memory", &st) == -1) {
        mkdir("/sys/fs/cgroup/memory", 0777);
    }


    if(mount("memory", "/sys/fs/cgroup/memory", "cgroup", 0, "memory" ) == -1){
        perror("mount memory");
        exit(EXIT_FAILURE);
    }
    // création du groupe de contrôle "mem"
    if (stat("/sys/fs/cgroup/memory/mem", &st) == -1) {
        mkdir("/sys/fs/cgroup/memory/mem", 0700);
    }
 }



 void write_limit_memory(char* limit){
    int fd  = open("/sys/fs/cgroup/memory/mem/memory.limit_in_bytes", O_WRONLY);
    int i = write(fd, limit, strlen(limit));
    if (i == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    close(fd);
 }

 int allocate_memory(int num_blocks, int block_size) {
    void *blocks[num_blocks];

    for (int i = 0; i < num_blocks; i++) {
        blocks[i] = malloc(block_size);
        if (blocks[i] == NULL) {
            perror("Failed to allocate memory");
            return 1;
        }
        printf("Allocated block %d\n", i);
        memset(blocks[i], 0, block_size);
    }

    // Garder le programme en cours d'exécution pour observer l'utilisation de la mémoire
    getchar();

    // Libérer la mémoire
    for (int i = 0; i < num_blocks; i++) {
        free(blocks[i]);
    }
    return 0;
    // 
 }

 void mount_cgroup_cpu(){
    // mount cgroup filesystem
    if(mount("none", CGROUPS_PATH, "tmpfs", 0, NULL ) == -1){
        perror("mount cgroup");
        exit(EXIT_FAILURE);
    }
    struct stat st = {0};
    // create directory
    if (stat("/sys/fs/cgroup/cpuset", &st) == -1) {
        mkdir("/sys/fs/cgroup/cpuset", 0777);
    }

    if(mount("cpu,cpuset", "/sys/fs/cgroup/cpuset", "cgroup", 0, "cpuset" ) == -1){
        perror("mount memory");
        exit(EXIT_FAILURE);
    }
    // création du groupe de contrôle "mem"
    if (stat("/sys/fs/cgroup/cpuset/high", &st) == -1) {
        mkdir("/sys/fs/cgroup/cpuset/high", 0700);
    }
    if (stat("/sys/fs/cgroup/cpuset/low", &st) == -1) {
        mkdir("/sys/fs/cgroup/cpuset/low", 0700);
    }

 }

 void write_limit_cpu(){
    int fd  = open("/sys/fs/cgroup/cpuset/high/cpuset.cpus", O_WRONLY);
    write(fd, "3", 1);
    close(fd);

    fd  = open("/sys/fs/cgroup/cpuset/high/cpuset.mems", O_WRONLY);
    write(fd, "0", 1);
    close(fd);

    fd  = open("/sys/fs/cgroup/cpuset/low/cpuset.cpus", O_WRONLY);
    write(fd, "2", 1);
    close(fd);

    fd  = open("/sys/fs/cgroup/cpuset/low/cpuset.mems", O_WRONLY);
    write(fd, "0", 1);
    close(fd);
 }


 void use_cpu(){
    int i = 0;
    while(1){
        i++;
        if(i>123456789){
            printf("ça fait 123456789\n");
            i = 0;
        }
    }
 }

