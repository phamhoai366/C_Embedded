// #include <stdio.h>
// #include <unistd.h>
// #include <sched.h>
 
// int main()
// {
//     printf("Valid priority range for SCHED_OTHER: %d - %d\n",
//         sched_get_priority_min(SCHED_OTHER),
//         sched_get_priority_max(SCHED_OTHER));
//     printf("Valid priority range for SCHED_FIFO: %d - %d\n",
//         sched_get_priority_min(SCHED_FIFO),
//         sched_get_priority_max(SCHED_FIFO));
//     printf("Valid priority range for SCHED_RR: %d - %d\n",
//         sched_get_priority_min(SCHED_RR),
//         sched_get_priority_max(SCHED_RR));
    
//     return 0;       
// }


#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>

void *thread_function(void *arg) {
    int thread_id = *(int*)arg;
    printf("Thread %d started\n", thread_id);

    sleep(2); 
    printf("Thread %d finished\n", thread_id);
    pthread_exit(NULL);
}

int main() {
    pthread_t thread1, thread2;
    int thread_ids[2] = {1, 2};
    pthread_attr_t attr[2];

    for (int i = 0; i < 2; i++) {
        pthread_attr_init(&attr[i]);
    }

    struct sched_param param1;
    param1.sched_priority = 90;
    pthread_attr_setschedpolicy(&attr[0], SCHED_FIFO);
    pthread_attr_setschedparam(&attr[0], &param1);

    struct sched_param param2;
    param2.sched_priority = 10;
    pthread_attr_setschedpolicy(&attr[1], SCHED_FIFO);
    pthread_attr_setschedparam(&attr[1], &param1);

    pthread_create(&thread1, &attr[0], thread_function, (void*)&thread_ids[0]);
    pthread_create(&thread2, &attr[1], thread_function, (void*)&thread_ids[1]);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_exit(NULL);
    return 0;
}