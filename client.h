#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "define.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define UDP_CLIENT_MAX_NUMBER 2
#define CLIENT_CONNECT_ADDRESS "2.3.4.5"

typedef struct
{
    struct pollfd client_poll;
    struct sockaddr_in addr;
}upd_client_t;

void start_upd_client();

#endif