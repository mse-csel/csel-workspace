#include <stdlib.h>
#include <stdio.h>


#include "app_config.h"
#include "communication.h"
#include "cgroups.h"

int main(){
    //printf("hello there\n");

    #ifdef COMMUNICATION
    comm_process();
    #endif

    #ifdef CGROUPS
    mount_cgroup();
    write_limit("60M");
    allocate_memory(50, 1024*1024);
    #endif

    return 0;
}
