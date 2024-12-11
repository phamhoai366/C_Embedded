#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func(void *arg) {
    printf("Child thread is running.\n");
    pthread_exit(NULL);
}

int main() {
    pthread_t tid;
    pthread_attr_t attr;
    int detachstate;

    // Initialize thread attribute object
    pthread_attr_init(&attr);

    // Get the default detach state
    pthread_attr_getdetachstate(&attr, &detachstate);
    printf("Default detach state: %s\n", detachstate == PTHREAD_CREATE_JOINABLE ? "joinable" : "detached");

    // Set the thread's detach state to detached
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, thread_func, NULL);

    // Destroy thread attribute object
    pthread_attr_destroy(&attr);

    // Allow some time for the child thread to run
    sleep(1);

    printf("Main thread is exiting.\n");
    pthread_exit(NULL);

    return 0;
}