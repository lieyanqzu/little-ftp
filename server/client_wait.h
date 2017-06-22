#ifndef __CLIENT_WAIT_H__
#define __CLIENT_WAIT_H__

#include <sys/epoll.h>
#include "unp.h"
#include "conf.h"
#include "../common/common.h"
#include "ctl_handle.h"

#define MAX_EVENT 10

void client_wait(int listenfd);
void sig_chld(int signo);

#endif // client_wait.h