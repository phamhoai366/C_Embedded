#include <pthread.h>
#include <sched.h>
#include <stdio.h>

int main() {
    pthread_attr_t attr;
    struct sched_param param;
    int policy;

    // Initialize thread attribute object
    pthread_attr_init(&attr);

    // Get the default scheduling policy
    pthread_attr_getschedpolicy(&attr, &policy);

    // Print the default scheduling policy
    if (policy == SCHED_OTHER) {
        printf("Default scheduling policy is SCHED_OTHER\n");
    } else if (policy == SCHED_RR) {
        printf("Default scheduling policy is SCHED_RR\n");
    } else if (policy == SCHED_FIFO) {
        printf("Default scheduling policy is SCHED_FIFO\n");
    }

    // Get the default scheduling parameters
    pthread_attr_getschedparam(&attr, &param);

    // Print the default priority
    printf("Default priority is %d\n", param.sched_priority);

    // Set the thread's scheduling policy and priority
    param.sched_priority = 50;
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    pthread_attr_setschedparam(&attr, &param);

    // Get the new scheduling policy and priority
    pthread_attr_getschedpolicy(&attr, &policy);
    pthread_attr_getschedparam(&attr, &param);

    // Print the new scheduling policy and priority
    if (policy == SCHED_OTHER) {
        printf("New scheduling policy is SCHED_OTHER\n");
    } else if (policy == SCHED_RR) {
        printf("New scheduling policy is SCHED_RR\n");
    } else if (policy == SCHED_FIFO) {
        printf("New scheduling policy is SCHED_FIFO\n");
    }

    printf("New priority is %d\n", param.sched_priority);

    // Destroy thread attribute object
    pthread_attr_destroy(&attr);

    return 0;
}