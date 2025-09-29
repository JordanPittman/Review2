//compile with: gcc -O2 -pthread counter.c -o counter
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>

static _Atomic int counter = 0;

void* worker(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 100000; i++) {
        atomic_fetch_add(&counter, 1);
        if ((i % 25000) == 0) usleep(1); // randomize thread interleaving
    }
    if (id == 0) {
        printf("thread %d done\n", id);
    }
    free(arg);
    return NULL;
}

int main(int argc, char** argv) {
    int n = (argc > 1) ? atoi(argv[1]) : 4;
    pthread_t ts[64];
    int i;
    for (i = 0; i < n; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        if (pthread_create(&ts[i], NULL, worker, id)) {
            perror("pthread_create");
            exit(1);
        }
    }
    for (i = 0; i < n; i++) {
        pthread_join(ts[i], NULL);
    }
    printf("expected %d, got %d\n", n * 100000, counter);
    return 0;
}
