#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func(void *arg){
    int *count = (int*)arg;
    for (int i = 1; i <= *count; i++){
        printf("Thread: %d\n", i);
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    int ret;
    int count = 5;

    ret = pthread_create(&tid, NULL, thread_func, &count);
    if (ret != 0){
        printf("pthread_create failed:%d\n", ret);
        return -1;
    }
    printf("create treads success\n");

    ret = pthread_join(tid, NULL);
    if ( ret != 0){
        printf("pthread_join");
        return -1;
    }
    printf("Thread finished.\n");

    return 0;
}