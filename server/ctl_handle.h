#ifndef __CTL_HANDLE_H__
#define __CTL_HANDLE_H__

#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "unp.h"
#include "../common/common.h"
#include "conf.h"

typedef struct cmd_t {
    char *start; // command start with
    int (*handle)(int, char*); // handle funciton
    char *help; // help text
    int log_need; // need login or not
} command;
#define NEED 1
#define NONEED 2

#define MAX_HELP 4096
#define MAX_TEMP 256

#include "ctl_handle.h"

// receive command form cliend
void cmd_recevier(int sockfd);

// handle command
int command_handle(int sockfd, char *cmd, int *login);

// handle client login
int wait_cli_login(int sockfd, char *baduse);

// send "ls" result to client
int send_list(int sockfd, char *cmd);

int change_dir(int sockfd, char *cmd); // cd
int make_dir(int sockfd, char *cmd); // mkdir
int get_pwd(int sockfd, char *filler); // pwd
int remove_file(int sockfd, char *cmd); // rm
int show_help(int sockfd, char *filler); //help
int goodbye(int sockfd, char *filler); // bye

// transfer file
int file_put_handle(int sockfd, char *cmd);
int file_get_handle(int sockfd, char *cmd);
int file_trans_handle(int sockfd, char *cmd, int op);

#endif // ctl_handle.h