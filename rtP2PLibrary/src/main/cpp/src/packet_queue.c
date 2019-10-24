#include <log.h>

#include <stdbool.h>
#include <libavcodec/avcodec.h>
#include "packet_queue.h"

#define QUEUE_SIZE 100
typedef struct queue_buffer_t{
    uint8_t *buffer;
    int size;
    int type;
    int timestamp;
}queue_buffer_t;

typedef struct queue_t{
    queue_buffer_t dateQueue[QUEUE_SIZE];
    int front;
    int rear;
    int count;
}queue_t;

int queue_open(long *handle, pthread_mutex_t *queue_mutex) {
    int ret;
    queue_t *q=malloc(sizeof(queue_t));
    memset(q, 0, sizeof(queue_t));
    ret = pthread_mutex_init(queue_mutex, NULL);
    if (ret != 0) {
        LOGI("Mutex init failed.\n");
        return -1;
    }
    /*q->dateQueue=malloc(QUEUE_SIZE* sizeof(queue_buffer_t*));
    memset(q->dateQueue, 0, QUEUE_SIZE * sizeof(queue_buffer_t*));
    for (int i = 0; i < QUEUE_SIZE; ++i) {
        q->dateQueue[i] =malloc(sizeof(queue_buffer_t));
        memset(q->dateQueue[i], 0, QUEUE_SIZE * sizeof(queue_buffer_t));
    }*/
    q->front = q->rear = 0;
    q->count = 0;

    *handle= (long ) q;
    return 0;
}

bool is_empty_queue(queue_t *q) {
    if (q->count > 0) { return false; }
    return true;
}

bool is_full_queue(queue_t *q) {
    if (q->count < QUEUE_SIZE) { return false; }
    return true;
}

int queue_put(long handle, pthread_mutex_t *queue_mutex, uint8_t *buffer, int size, int type,
              int timestamp) {
    queue_t *q = (queue_t *) handle;
    if (q == NULL) {
        return false;
    }
    pthread_mutex_lock(queue_mutex);
    if (is_full_queue(q)) {
    //LOGE("[%s %d] queue overflow", __FUNCTION__, __LINE__);
        pthread_mutex_unlock(queue_mutex);
        return -1;
    }

    q->count++;
    q->dateQueue[q->rear].buffer = buffer;
    q->dateQueue[q->rear].size = size;
    q->dateQueue[q->rear].type = type;
    q->dateQueue[q->rear].timestamp = timestamp;
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    LOGI("[%s %d] q->front=%d,q->rear=%d,q->count=%d ", __FUNCTION__, __LINE__, q->front, q->rear, q->count);


    pthread_mutex_unlock(queue_mutex);
    return 0;
}

int queue_get(long handle, pthread_mutex_t *queue_mutex, uint8_t **buffer, int *size,
              int *type, int *timestamp) {
    queue_t *q = (queue_t *) handle;
    if (q == NULL) {
        return -1;
    }
    pthread_mutex_lock(queue_mutex);
    if (is_empty_queue(q)) {
       //LOGE("[%s %d]queue empty.", __FUNCTION__, __LINE__);
        pthread_mutex_unlock(queue_mutex);
        return -1;
    }
    *buffer=q->dateQueue[q->front].buffer;
    *size=q->dateQueue[q->front].size;
    *type=q->dateQueue[q->front].type;
    *timestamp=q->dateQueue[q->front].type;
    q->count--;
    q->front = (q->front + 1) % QUEUE_SIZE;
    LOGI("[%s %d] q->front=%d,q->rear=%d,q->count=%d ", __FUNCTION__, __LINE__, q->front, q->rear, q->count);

    pthread_mutex_unlock(queue_mutex);

    return 0;
}

int queue_close(long handle, pthread_mutex_t *queue_mutex) {
    queue_t *q = (queue_t *) handle;
    if (q == NULL) {
        return -1;
    }
    free(q);
    pthread_mutex_destroy(queue_mutex);
    return 0;
}