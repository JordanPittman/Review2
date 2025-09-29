//compile with: gcc -O2 -pthread counter.c -o counter
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int counter = 0;

void* worker(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 100000; i++) {
        counter++;
        if ((i % 25000) == 0) usleep(1); // randomize thread interleaving
    }
    if (id == 0) {
        printf("thread %d done\n", id);
    }
    return NULL;
}

int main(int argc, char** argv) {
    int n = (argc > 1) ? atoi(argv[1]) : 4;
    pthread_t ts[64];
    int i;
    for (i = 0; i < n; i++) {
        if (pthread_create(&ts[i], NULL, worker, &i)) {
            perror("pthread_create");
            exit(1);
        }
    }
    for (i = 0; i < n && i < 2; i++) {
        pthread_join(ts[i], NULL);
    }
    printf("expected %d, got %d\n", n * 100000, counter);
    return 0;
}
