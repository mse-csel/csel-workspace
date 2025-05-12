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
    #endif

    return 0;
}
