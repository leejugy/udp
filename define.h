#ifndef __DEFINE__
#define __DEFINE__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#define LOCAL_ADDRESS "127.0.0.1"
#define UDP_COMMON_PORT 1972

#define FATAL(fmt, ...) printf("[\x1b[31mERR,%s\x1b[0m]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define DEBUG(fmt, ...) printf("[\x1b[35mDBG,%s\x1b[0m]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define INFO(fmt, ...) printf("[\x1b[35mINFO,%s\x1b[0m]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)

#endif

