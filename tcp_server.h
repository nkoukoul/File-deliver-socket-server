#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "project.h"
#include "models.h"
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/signal.h>
#include <netinet/tcp.h>


#define check(expr) if (!(expr)) { perror(#expr); kill(0, SIGTERM); }

// function prototypes
int handle_input(int fd);
int handle_output(int fd, struct Event * event);

void enable_keepalive(int sock);
void * start_server(void * args);

#endif // TCP_SERVER
