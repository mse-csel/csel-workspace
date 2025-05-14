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

    #ifdef MEMORY
    //mount_cgroup_memory();

    //write_limit_memory("60M");
    //allocate_memory(50, 1024*1024);
    #endif

    #ifdef CPU_LIMIT
    //mount_cgroup_cpu();
    //write_limit_cpu();
    use_cpu();
    #endif

    #ifdef CPU_SHARE
    // mounting as to be done manually in shell
    // mount -t cgroup -o cpu,cpuset cpu /sys/fs/cgroup/cpu
    // mkdir task1
    // mkdir task2
    // echo 3 > task1/cpuset.cpus 
    // echo 3 > task2/cpuset.cpus 
    // echo 0 > task2/cpuset.mems 
    // echo 0 > task1/cpuset.mems 
    // echo 254 > task2/cpu.shares 
    // echo 756 > task1/cpu.shares 
   
    use_cpu();
    #endif
    

    return 0;
}
