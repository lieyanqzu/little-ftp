#include "client_wait.h"

#include <sys/epoll.h>

static int capab;

void client_wait(int listenfd)
{
    int n, nfds;
    int epfd, clifd;

    struct sockaddr_in cliaddr;
    int clilen;

    struct epoll_event ev, events[MAX_EVENT];

    Signal(SIGCHLD, sig_chld); // handling SIGCHLD
    capab = get_capab(); // set capability

    if ( (epfd = epoll_create1(0)) == -1 )
        err_sys("epoll_create error");

    setnonblocking(listenfd);
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
        err_sys("epoll_ctl error");

    for (;;)
    {
        // wait listenfd
        nfds = epoll_wait(epfd, events, MAX_EVENT, -1);

        if (nfds == -1)
        {
            if (errno == EINTR)
                continue;
            else
                err_sys("epoll_wait error");
        }
            
        for (n = 0; n < nfds; n++)
        {
            if (events[n].data.fd == listenfd)
            {
                clifd = Accept(listenfd, (SA *)&cliaddr, (socklen_t *)&clilen);
                if (clifd == -1)
                    err_sys("accept error");

                if (capab > 0)
                    capab --;
                else
                {
                    // out of capability
                    send_rcode(clifd, CLI_FULL);
                    Close(clifd);
                    continue;
                }

                if (Fork() == 0)
                {
                    Close(listenfd);
                    cmd_recevier(clifd);
                    Close(clifd);
                    exit(0);
                }
                Close(clifd);
            }
        }
    }
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    capab ++; // one new slot for client
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0);
    return;
}