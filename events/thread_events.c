#include "thread_events.h"
#include <pthread.h>

thread_event_t thread_event_create() {
  thread_event_t te = {0};

  pthread_mutex_init(&te.mut, NULL);
  pthread_cond_init(&te.cond, NULL);

  return te;
}

void thread_event_register(thread_event_t* te) {
  //
  pthread_mutex_lock(&te->mut);
}

void thread_event_wait(thread_event_t* te) {
  //
  pthread_cond_wait(&te->cond, &te->mut);
}

void thread_event_trigger(thread_event_t* te) {
  //
  pthread_cond_broadcast(&te->cond);
}

void thread_event_finish(thread_event_t* te) {
  //
  pthread_mutex_unlock(&te->mut);
}
