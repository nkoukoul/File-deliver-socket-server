#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include "project.h"

struct Event
{
  struct Event * next;
  int op_id;
  char * filename;
  char * data;
};

struct EventQueue
{
  struct Event * head;
  int size;
};

struct EventQueue * create_event_queue();
void push_event(struct EventQueue * eq, char * filename, char * data, int op_id);
void pop_event(struct EventQueue * eq);
struct Event * get_top_event(struct EventQueue * eq);

#endif //EVENT_QUEUE_H
