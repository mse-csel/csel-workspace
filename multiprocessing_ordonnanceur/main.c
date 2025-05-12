#include <stdlib.h>
#include <stdio.h>


#include "app_config.h"
#include "communication.h"

int main(){
    //printf("hello there\n");

    #ifdef COMMUNICATION
    comm_process();
    #endif

    return 0;
}
