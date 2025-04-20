#include "client.h"
#include "timer.h"

upd_client_t client[UDP_CLIENT_MAX_NUMBER] = {0, };

static int init_udp_client()
{
    struct sockaddr_in addr = {0, }; 
    int loop = 0;

#if 1
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;    
#endif

    for(loop = 0; loop < UDP_CLIENT_MAX_NUMBER; loop++)
    {
        client[loop].client_poll.fd = socket(AF_INET, SOCK_DGRAM, 0);
        if(client[loop].client_poll.fd < 0)
        {
            perror("fail to get socket");
            return -1;
        }

        client[loop].addr.sin_addr.s_addr = inet_addr(CLIENT_CONNECT_ADDRESS);
        client[loop].addr.sin_family = AF_INET;
        client[loop].addr.sin_port = htons(UDP_COMMON_PORT);
#if 1
        addr.sin_port = htons(1973 + loop); //bind 안쓰면 커널이 임의로 할당. 서버 포트와 클라이언트 포트는 같은 주소에서 같을 수 없다[이미 포트 사용중].
        if (bind(client[loop].client_poll.fd, (void *)&addr, sizeof(addr)) < 0)
        {
            perror("fail to bind server");
            return -1;
        }
#endif
    }
}

static int client_send(int client_number, uint8_t *buf, size_t buf_size)
{    
    int ret = 0;
    client[client_number].client_poll.events = POLLOUT;

    ret = poll(&client[client_number].client_poll, 1, 10);
    if(ret < 0)
    {
        perror("fail to get client");
        return -1;
    }
    else if (ret == 0)
    {
        return 1;
    }
    else
    {
        if(client[client_number].client_poll.revents & POLLNVAL)
        {
            FATAL("file descripter not open");
            return -1;
        }
        else if(client[client_number].client_poll.revents & POLLHUP)
        {
            FATAL("connect maybe reset by peer");
            return -1;
        }
        else if(client[client_number].client_poll.revents & POLLOUT)
        {
            ret = sendto(client[client_number].client_poll.fd, buf, buf_size, 0, (void *)&client[client_number].addr, sizeof(client[client_number].addr));
            if(ret < 0)
            {
                FATAL("[IP:%s,PORT:%d]fail to send", inet_ntoa(client[client_number].addr.sin_addr), ntohs(client[client_number].addr.sin_port));
                return -1;
            }
            else
            {
                INFO("[IP:%s,PORT:%d]send buf with buf : %s", inet_ntoa(client[client_number].addr.sin_addr), ntohs(client[client_number].addr.sin_port), buf);
                return 1;
            }
        }
        else
        {
            FATAL("unkwon status : %d", client[client_number].client_poll.revents);
            return -1;
        }
    }
}

static int client_recv(int client_number, uint8_t *buf, size_t buf_size)
{
    int ret = 0;
    struct sockaddr_in server_addr = {0, };
    int addr_len = sizeof(server_addr);

    client[client_number].client_poll.events = POLLIN;

    ret = poll(&client[client_number].client_poll, 1, 10);
    if(ret < 0)
    {
        perror("fail to get client");
        return -1;
    }
    else if (ret == 0)
    {
        return 0;
    }
    else
    {
        if(client[client_number].client_poll.revents & POLLNVAL)
        {
            FATAL("file descripter not open");
            return -1;
        }
        else if(client[client_number].client_poll.revents & POLLHUP)
        {
            FATAL("connect maybe reset by peer");
            return -1;
        }
        else if(client[client_number].client_poll.revents & POLLIN)
        {
            memset(buf, 0, buf_size);
            ret = recvfrom(client[client_number].client_poll.fd, buf, buf_size, 0, (void *)&server_addr, &addr_len);
            if(ret < 0)
            {
                FATAL("[IP:%s,PORT:%d]recv fail", inet_ntoa(client[client_number].addr.sin_addr), ntohs(client[client_number].addr.sin_port));
                return -1;
            }
            else
            {
                INFO("[IP:%s,PORT:%d]send buf with buf : %s", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), buf);
                return ret;
            }
        }
        else
        {
            FATAL("unkwon status : %d", client[client_number].client_poll.revents);
            return -1;
        }
    }
}

static void thread_client_send(union sigval val)
{
    int loop = 0;
    uint8_t send[256] = {0, };

    for(loop = 0; loop < UDP_CLIENT_MAX_NUMBER; loop++)
    {
        sprintf(send, "hellow world. i'm client [%d]", loop);
        client_send(loop, send, strlen(send));
    }
}

static void client_thread()
{
    init_udp_client();
    uint8_t recv[256] = {0, };
    timer_info_t client_timer = {0, };
    int loop = 0;

    client_timer.expired_function = thread_client_send;
    client_timer.repeat = REPEAT_TIMER_YES;
    client_timer.set_time.tv_sec = 1;

    set_timer(&client_timer);

    while(1)
    {
        for(loop = 0; loop < UDP_CLIENT_MAX_NUMBER; loop)
        {
            client_recv(loop, recv, sizeof(recv));
        }
    }
}

void start_upd_client()
{
    pthread_t tid = 0;

    if(pthread_create(&tid, NULL, (void *)&client_thread, NULL))
    {
        perror("fail to create thread udp client");
        return;
    }

    if(pthread_detach(tid) < 0)
    {
        perror("fail to detach thread udp client");
        return;
    }
}