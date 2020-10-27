#include "tcp_server.h"
#include "event_queue.h"
#include "models.h"
#include "project.h"
#include "utils.h"

int main()
{
  pthread_t server_thread;
  pthread_mutex_t event_lock;
  int s, w;
  void * res;
  if (pthread_mutex_init(&event_lock, NULL) != 0) 
    { 
      perror("mutex init has failed\n"); 
      exit(EXIT_FAILURE);
    }
  struct TData * pdata;
  pdata = (struct TData *) malloc(sizeof(struct TData));
  pdata->event_lock = event_lock;

  /* create event queue*/
  struct EventQueue * eq;
  eq = create_event_queue();
  pdata->event_queue = eq;
  s = pthread_create(&server_thread, NULL, start_server, (void *) pdata);
  if (s != 0)
    {
      perror("server thread creation failed.. exiting\n");
      exit(EXIT_FAILURE);
    }
  int op_id_count = 0;
  while(true)
    {
      char str[30];
      printf("should send?\n");
      memset(str, 0 , 30);
      scanf("%s",str);
      //memset(event, 0 , 30);
      if (!strcmp(str, "yes")) /*input is yes*/
	{	   
	  printf("file name?\n");
	  memset(str, 0 , 30);
	  scanf("%s",str);
	  pthread_mutex_lock(&event_lock);	
	  op_id_count++;
	  char * filename;
	  filename = (char *) malloc(sizeof(str));
	  strcpy(filename, str);
	  char * data = read_file(filename);
	  push_event(eq, filename, data, op_id_count);
	  struct Event * event = eq->head->next;
	  for (int i = 0; i < eq->size; i++)
	    {
	      if (event) printf("file %s, size %d, op_id %d\n", event->filename, strlen(event->data), event->op_id);
	      event = event->next;
	    }
	  pthread_mutex_unlock(&event_lock);
	}	
    }
  s = pthread_join(server_thread, &res);
  if (s != 0)
    {
      perror("server thread join failed.. exiting\n");
      exit(EXIT_FAILURE);
    }
  pthread_mutex_destroy(&event_lock); 
  return 0;
}
