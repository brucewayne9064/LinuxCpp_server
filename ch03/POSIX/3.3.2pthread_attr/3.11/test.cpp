#include <iostream> 
#include <unistd.h> 
#include <sched.h>  
      
int main(int argc, char *argv[])  
{  
	std::cout << "SCHED_OTHER的优先级范围是"
              << sched_get_priority_min(SCHED_OTHER) << "-"
              << sched_get_priority_max(SCHED_OTHER) << std::endl;
    std::cout << "SCHED_FIFO的优先级范围是"
              << sched_get_priority_min(SCHED_FIFO) << "-"
              << sched_get_priority_max(SCHED_FIFO) << std::endl;
    std::cout << "SCHED_RR的优先级范围是"
              << sched_get_priority_min(SCHED_RR) << "-"
              << sched_get_priority_max(SCHED_RR) << std::endl;
    return 0;
}  