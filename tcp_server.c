#include "tcp_server.h"
#include "event_queue.h"
#include "utils.h"

void enable_keepalive(int sock) 
{
  int yes = 1;
  check(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) != -1);

  int idle = 1;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int)) != -1);

  int interval = 1;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int)) != -1);

  int maxpkt = 10;
  check(setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int)) != -1);
}

void * start_server(void * args){
  pthread_mutex_t event_lock = ((struct TData *) args)->event_lock;
  struct EventQueue * eq = ((struct TData *) args)->event_queue;
  int sock,fd, socket_timeout = 3000;
  struct FDS target[FD_SETSIZE];
  socklen_t client_len;
  struct sockaddr_in server,client; 
  fd_set active_set, read_set, write_set;
  printf("FD_SETSIZE = %d\n",FD_SETSIZE);
  for (fd = 0; fd < FD_SETSIZE; fd++) 
    {
      target[fd].uuid = 0;
      target[fd].op_id = 0;
      //pthread_mutex_lock(&event_lock);
      target[fd].head = NULL;
      //pthread_mutex_unlock(&event_lock);
    }
    
  sock = socket(AF_INET, SOCK_STREAM, PF_UNSPEC);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_port = htons(8955);

  int my_error = 0;
  if (my_error = bind(sock, (struct sockaddr *) &server, sizeof(server))<0)
    {
      printf("bind failed with error %d\n", my_error);
      pthread_exit(NULL);
    }
    
  listen(sock, 5);
    
  FD_ZERO(&active_set);
  FD_SET(sock, &active_set);
  printf("server ready\n");
  struct timeval timeout;
  timeout.tv_sec  = socket_timeout / 1000;
  timeout.tv_usec = socket_timeout % 1000;
  while (true)
    {
      memcpy(&read_set, &active_set, sizeof active_set);
      memcpy(&write_set, &active_set, sizeof active_set);
      //select(FD_SETSIZE, &read_set, NULL, NULL, &timeout);
      select(FD_SETSIZE, &read_set, &write_set, NULL, NULL);
      /* Did we get a connection request? If so, we accept it,
	 add the new descriptor into the read set, choose a random
	 number for this client
      */
      if (FD_ISSET(sock, &read_set))
	{
	  client_len = sizeof client;
	  fd = accept(sock, (struct sockaddr *)&client, &client_len);
	  //enable_keepalive(fd);
	  FD_SET(fd, &active_set);
	  printf("new connection on fd %d\n", fd);
	  target[fd].uuid = 1 + rand()%1000;
	  target[fd].op_id = INT_MAX;
	  target[fd].head = eq->head->next;
	}
	

      /* Now we must check each descriptor in the read set in turn.
	 For each one which is ready we process the client request
      */
      for (fd = 0; fd < FD_SETSIZE; fd++)
	{
	  if (fd == sock) continue; // Omit the rendezvous descriptor
	  if (target[fd].uuid == 0) continue; // Empty table entry
	  if (FD_ISSET(fd, &read_set))
	    {
	      printf("About to read on fd %d\n", fd);
	      if ((target[fd].op_id = handle_input(fd))<0)
		{
		  /* If the client has closed its end of the conection,
		     or if the dialogue with the client is complete, we
		     close our end, and remove the descriptor from the
		     read set
		  */
		  close(fd);
		  FD_CLR(fd, &active_set);
		  /*target[fd].uuid = 0;
		  target[fd].op_id = INT_MAX;
		  target[fd].head = eq->head;*/
		  printf("on read closing fd= %d\n", fd);
		}
	    }
	  if (FD_ISSET(fd, &write_set))
	    {
	      //printf("About to write on fd %d\n", fd);
	      pthread_mutex_lock(&event_lock);
	      if (target[fd].head) /*if there are events for this client*/
		{
		  struct Event * top_event = target[fd].head;
		  if (top_event->op_id > target[fd].op_id)
		    {		    
		      if (handle_output(fd, top_event)<0)
			{
			  /* If the client has closed its end of the conection,
			     or if the dialogue with the client is complete, we
			     close our end, and remove the descriptor from the
			     read set
			  */
			  close(fd);
			  FD_CLR(fd, &active_set);
			  printf("on write closing fd= %d\n",fd);
			}
		      else
			{
			  target[fd].head = target[fd].head->next;
			}
		    } 
		}
	      pthread_mutex_unlock(&event_lock);
	    }	
	}
    }    
}

int handle_input(int fd){
  char inbuffer[MAXBUF];
  char * input_data;
  int bytes_read, i;
  // Read data from client
  bytes_read = read(fd, inbuffer, MAXBUF);
  if ( bytes_read <= 0 )
    return -1; //client closed connection
  //for (i = 0; i < bytes_read; i++) input_data += inbuffer[i];
  printf("received data on fd = %d\n%s\n", fd ,inbuffer);
  return atoi(inbuffer);
}

int handle_output(int fd, struct Event * event){
  size_t filesize = strlen(event->data);
  char * filename = event->filename;
  char tx[MAXBUF];
  /*test.txt*/
  int header_size = snprintf(tx, MAXBUF, "\nfile name %s size %d\n", filename, filesize);
  int offset = header_size;
  int total_bytes_written = 0;
  while (total_bytes_written < filesize)
    {
      memcpy(tx+offset, event->data+total_bytes_written, MAXBUF-offset);
      int bytes_written = 0;
      if ((bytes_written = write(fd, tx, strlen(tx))) < 0 )
	return -1;
      total_bytes_written += bytes_written - offset;
      offset = 0;
    }
  //printf("bytes written %d\n",bytes_written);
  return 1;
}
