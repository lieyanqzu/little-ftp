#ifndef __CTL_HANDLE_H__
#define __CTL_HANDLE_H__

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "unp.h"
#include "../common/common.h"

typedef struct cmd_t {
    char *start;
    int (*handle)(int, char*);
    char *help;
    int log_need;
} command;

#define NEED 1
#define NONEED 2
#define MAX_HELP 4096
#define MAX_TEMP 256

#include "ctl_handle.h"

void cmd_recevier(int sockfd);
int command_handle(int sockfd, char *cmd, int *login);
int wait_cli_login(int sockfd, char *baduse);
int send_list(int sockfd, char *cmd);
int change_dir(int sockfd, char *cmd);
int make_dir(int sockfd, char *cmd);
int get_pwd(int sockfd, char *filler);
int remove_file(int sockfd, char *cmd);
int show_help(int sockfd, char *filler);
int goodbye(int sockfd, char *filler);
int file_put_handle(int sockfd, char *cmd);
int file_get_handle(int sockfd, char *cmd);
int file_trans_handle(int sockfd, char *cmd, int op);

#endif // ctl_handle.h