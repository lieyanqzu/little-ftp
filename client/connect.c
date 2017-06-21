#include "connect.h"

void connect_to_server(char *ip)
{
    int sockfd;

    sockfd = Tcp_connect(ip, SERV_PORT_STR);

    cmd_sender(sockfd);
}
