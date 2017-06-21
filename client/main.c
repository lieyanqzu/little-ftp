#include "connect.h"
#include "unp.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        err_quit("usage: client <IPaddress>");
    }

    connect_to_server(argv[1]);

    return 0;
}