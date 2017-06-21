#ifndef __CTL_HANDLE_H__
#define __CTL_HANDLE_H__

#include <termios.h>
#include "unp.h"
#include "../common/common.h"

void cmd_sender(int sockfd);
int ctl_word_handle(int sockfd, int rc);
void cli_login(int sockfd, int rcode);
void get_list(int sockfd);
int get_file(int sockfd);
int put_file(int sockfd);
int trans_file(int sockfd, int op);
void cli_exit();

#endif // ctl_handle.h