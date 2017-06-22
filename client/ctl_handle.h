#ifndef __CTL_HANDLE_H__
#define __CTL_HANDLE_H__

#include <termios.h>
#include "unp.h"
#include "../common/common.h"

// send command to server
void cmd_sender(int sockfd);

// handle reply code
int ctl_word_handle(int sockfd, int rc);

// handle client login
void cli_login(int sockfd, int rcode);

// show reply of "ls" command
void get_list(int sockfd);

// transfer file
int get_file(int sockfd);
int put_file(int sockfd);
int trans_file(int sockfd, int op);

void cli_exit();

#endif // ctl_handle.h