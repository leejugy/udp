#include "server.h"

upd_server_t server = {
    0,
};

static int init_udp_server()
{
    int loop = 0;
    struct sockaddr_in server_addr = {
        0,
    };

    server.server_poll.fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server.server_poll.fd < 0)
    {
        perror("fail to get socket");
        return -1;
    }

    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_COMMON_PORT);

    if (bind(server.server_poll.fd, (void *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("fail to bind server");
        return -1;
    }

    return 1;
}

static int server_send(uint8_t *buf, size_t buf_size)
{
    int ret = 0;
    server.server_poll.events = POLLOUT;

    ret = poll(&server.server_poll, 1, 10);
    if (ret < 0)
    {
        perror("fail to get server");
        return -1;
    }
    else if (ret == 0)
    {
        return 1;
    }
    else
    {
        if (server.server_poll.revents & POLLNVAL)
        {
            FATAL("file descripter not open");
            return -1;
        }
        else if (server.server_poll.revents & POLLHUP)
        {
            FATAL("connect maybe reset by peer");
            return -1;
        }
        else if (server.server_poll.revents & POLLOUT)
        {
            ret = sendto(server.server_poll.fd, buf, buf_size, 0, (void *)&server.addr, sizeof(server.addr));
            if (ret < 0)
            {
                FATAL("[IP:%s,PORT:%d]fail to send", inet_ntoa(server.addr.sin_addr), ntohs(server.addr.sin_port));
                return -1;
            }
            else
            {
                INFO("[IP:%s,PORT:%d]send buf with buf : %s", inet_ntoa(server.addr.sin_addr), ntohs(server.addr.sin_port), buf);
                return 1;
            }
        }
        else
        {
            FATAL("unkwon status : %d", server.server_poll.revents);
            return -1;
        }
    }
}

static int server_recv(uint8_t *buf, size_t buf_size)
{
    int ret = 0;
    int addr_len = sizeof(server.addr);

    server.server_poll.events = POLLIN;
    ret = poll(&server.server_poll, 1, 10);
    if (ret < 0)
    {
        perror("fail to get server");
        return -1;
    }
    else if (ret == 0)
    {
        return 0;
    }
    else
    {
        if (server.server_poll.revents & POLLNVAL)
        {
            FATAL("file descripter not open");
            return -1;
        }
        else if (server.server_poll.revents & POLLHUP)
        {
            FATAL("connect maybe reset by peer");
            return -1;
        }
        else if (server.server_poll.revents & POLLIN)
        {
            memset(buf, 0, buf_size);
            ret = recvfrom(server.server_poll.fd, buf, buf_size, 0, (void *)&server.addr, &addr_len);
            if (ret < 0)
            {
                FATAL("[IP:%s,PORT:%d]recv fail", inet_ntoa(server.addr.sin_addr), ntohs(server.addr.sin_port));
                return -1;
            }
            else
            {
                INFO("[IP:%s,PORT:%d]send buf with buf : %s", inet_ntoa(server.addr.sin_addr), ntohs(server.addr.sin_port), buf);
                return 1;
            }
        }
        else
        {
            FATAL("unkwon status : %d", server.server_poll.revents);
            return -1;
        }
    }
}

static void server_thread()
{
    init_udp_server();
    uint8_t recv[256] = {0, };
    uint8_t send[256] = "hellow world from server";

    while(1)
    {
        if(server_recv(recv, sizeof(recv)) > 0)
        {
            server_send(send, strlen(send));
        }
    }
}

void start_upd_server()
{
    pthread_t tid = 0;

    if(pthread_create(&tid, NULL, (void *)&server_thread, NULL))
    {
        perror("fail to create thread udp server");
        return;
    }

    if(pthread_detach(tid) < 0)
    {
        perror("fail to detach thread udp server");
        return;
    }
}
