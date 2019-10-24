#ifndef WHEAT_PACKETQUEUE_H
#define WHEAT_PACKETQUEUE_H
#include <pthread.h>
int queue_open(long *handle, pthread_mutex_t *queue_mutex);
int queue_put(long handle, pthread_mutex_t *queue_mutex, uint8_t *buffer, int size, int type,
              int timestamp);
int queue_get(long handle, pthread_mutex_t *queue_mutex, uint8_t **buffer, int *size,
              int *type, int *timestamp);
int queue_close(long handle, pthread_mutex_t *queue_mutex);

#endif //WHEAT_PACKETQUEUE_H
