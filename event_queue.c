#include "event_queue.h"

struct EventQueue * create_event_queue()
{
  struct EventQueue * eq;
  eq = (struct EventQueue *) malloc(sizeof(struct EventQueue));
  struct Event * head_event; /*empty event*/
  head_event = (struct Event *) malloc(sizeof(struct Event));
  head_event->next = NULL;
  head_event->filename = NULL;
  head_event->op_id = -1;
  eq->head = head_event;
  eq->size = 0;
  return eq;
}

void push_event(struct EventQueue * eq, char * filename, char * data, int op_id)
{
  
  struct Event * new_event;
  new_event = (struct Event *) malloc(sizeof(struct Event));
  new_event->next = eq->head->next;
  new_event->filename = filename;
  new_event->data = data;
  new_event->op_id = op_id;
  eq->head->next = new_event;
  eq->size++;
}

struct Event * get_top_event(struct EventQueue * eq)
{
  return eq->head->next;
}

void pop_event(struct EventQueue * eq)
{
  if (!eq->head->next) return;
  struct Event * top_event;
  top_event = eq->head->next;
  eq->head->next = top_event->next;
  free(top_event->filename);
  free(top_event->data);
  top_event->next = NULL;
  free(top_event);
  eq->size--;
  return;
}
