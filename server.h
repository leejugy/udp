#ifndef __SERVER_H__
#define __SERVER_H__

#include "define.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

typedef struct 
{
    struct pollfd server_poll;
    struct sockaddr_in addr;
}upd_server_t;

void start_upd_server();

#endif