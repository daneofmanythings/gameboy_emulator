#ifndef EVENTS_THREAD_EVENTS_H
#define EVENTS_THREAD_EVENTS_H

#include <pthread.h>

/*
 * This is THIN wrapper around condition variables from the pthreads library.
 * The intention is to aid in code readability.
 */

typedef struct {
  pthread_mutex_t mut;
  pthread_cond_t cond;
} thread_event_t;

thread_event_t thread_event_create();
void thread_event_register(thread_event_t* te);
void thread_event_wait(thread_event_t* te);
void thread_event_trigger(thread_event_t* te);
void thread_event_finish(thread_event_t* te);

#endif // !DEBUG
