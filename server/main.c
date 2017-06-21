#include "unp.h"
#include "listen.h"
#include "client_wait.h"

int main(int argc, char **argv)
{
    int listenfd;

    // daemon_init(argv[0], 0);
    listenfd = sock_bind_listen();
    client_wait(listenfd);

    return 0;
}