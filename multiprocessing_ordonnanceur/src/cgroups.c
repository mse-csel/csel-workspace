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

 void mount_cgroup(){
    // mount cgroup filesystem
    if(mount("none", "/sys/fs/cgroup", "tmpfs", 0, NULL ) == -1){
        perror("mount cgroup");
        exit(EXIT_FAILURE);
    }
    mkdir("/tmp/maison", 0777);
    struct stat st = {0};
    // create directory
  //  if (stat("/sys/fs/cgroup/memory", &st) == -1) {
        mkdir("/sys/fs/cgroup/memory", 0777);
//    }


    if(mount("memory", "/sys/fs/cgroup/memory", "cgroup", 0, "memory" ) == -1){
        perror("mount memory");
        exit(EXIT_FAILURE);
    }
    if (stat("/sys/fs/cgroup/memory/mem", &st) == -1) {
        mkdir("/sys/fs/cgroup/memory/mem", 0700);
    }
 }