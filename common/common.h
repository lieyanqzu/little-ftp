#ifndef __COMMON_H__
#define __COMMON_H__

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "unp.h"

#define SEND 10
#define RECV 11

#define USER_LOGIN 100
#define LOGIN_SUCCESS 101
#define LOGIN_FAILED 102
#define LOGIN_NEED 103
#define SHOW_LIST 200
#define GET_START 300
#define GET_END 301
#define PUT_START 400
#define PUT_END 401
#define CONN_END 500
#define CHDIR_SUCCESS 601
#define CHDIR_FAILED 602
#define MKDIR_SUCCESS 603
#define MKDIR_FAILED 604
#define INVALID 700
#define NORMAL_BACK 800

#define LOG_MAX 32
#define CMD_MAX 256

#define MAX_NAME 256
typedef struct file_header_t
{
    long long size;
    char name[MAX_NAME];
} file_head;

int setnonblocking(int sockfd);
int recv_rcode(int sockfd, int *rc);
int send_rcode(int sockfd, int rc);
int recv_cmd(int sockfd, char *cmd, size_t maxlen);
int send_cmd(int sockfd, char *cmd, size_t nbytes);
int send_file_size(int sockfd, long long size);
int recv_file_size(int sockfd, long long *size);
int send_port(int sockfd, int port);
int recv_port(int sockfd, int *port);
int send_file_head(int sockfd, file_head *header);
int recv_file_head(int sockfd, file_head *header);
int send_file(int sockfd, char *filename);
int recv_file(int sockfd, char *filename);
void trimstr(char *str, int n);
long long get_file_size(char *strFileName);

#endif // common.h