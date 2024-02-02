#include "nexus/abstract/serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

static int n = 0;
static int is_running = 1;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static uint8_t* receive(void* n, size_t *length) {
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);

    char buf[8];
    *length = sprintf(buf, "%d", *((int*) n)); // including '\0'
    uint8_t* res = (uint8_t *) malloc(*length);
    memcpy(res, buf, *length);
    return res;
}

static void* increment_thread(void* arg) {
    (void) arg;
    while (is_running) {
        pthread_mutex_lock(&mutex);
        n++;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        usleep(100000);  // Sleep for 100 milliseconds

    }
    return NULL;
}

static int filter(const uint8_t* buffer, size_t length) {
    return buffer[0] == '7' && length == 1;
}

int c_serial() {
    nexus_serial_t ser = nexus_serial_override_new(NULL, NULL, receive, &n);
    pthread_t increment_tid;
    pthread_create(&increment_tid, NULL, increment_thread, NULL);
    
    size_t length;

    uint8_t* res2 = nexus_serial_receive_filter(ser, &length, filter);
    printf("received filter: %.*s, %hhu\n", (int) length, (char*) res2, *res2);
    printf("length: %lu\n", length);
    free(res2);

    uint8_t* res = nexus_serial_receive(ser, &length);
    printf("received: %.*s\n", (int) length, (char*) res);
    printf("length: %lu\n", length);
    free(res);

    is_running = 0;
    pthread_join(increment_tid, NULL);
    nexus_serial_delete(ser);
    return 0;
}