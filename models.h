#ifndef MODELS_H
#define MODELS_H

#include "project.h"
#include "event_queue.h"

struct TData 
{
  pthread_mutex_t event_lock;
  struct EventQueue * event_queue;
};

struct FDS
{
  int uuid;
  int op_id;
  struct Event * head;
  int part; /*part of file*/
};

char event[30];

#endif //MODELS_H
