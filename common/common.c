#include "common.h"

int setnonblocking(int sockfd)
{
    Fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}

int recv_rcode(int sockfd, int *rc)
{
    char buf[16];
    if (Recv(sockfd, buf, sizeof(buf), 0) == 0)
        return -1;
    *rc = atoi(buf);
    return 0;
}

int send_rcode(int sockfd, int rc)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", rc);
    Send(sockfd, buf, sizeof(buf), 0);
    return 0;
}

int recv_cmd(int sockfd, char *cmd, size_t maxlen)
{
    if (Recv(sockfd, cmd, maxlen, 0) == 0)
        return -1;
    return 0;
}

int send_cmd(int sockfd, char *cmd, size_t nbytes)
{
    Send(sockfd, cmd, nbytes, 0);
    return 0;
}

int send_file_size(int sockfd, long long size)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%lld", size);
    Send(sockfd, buf, sizeof(buf), 0);
    return 0;
}

int recv_file_size(int sockfd, long long *size)
{
    char buf[16];
    if (Recv(sockfd, buf, sizeof(buf), 0) == 0)
        return -1;
    *size = atoll(buf);
    return 0;
}

int send_port(int sockfd, int port)
{
    return send_rcode(sockfd, port); // reuse
}

int recv_port(int sockfd, int *port)
{
    return recv_rcode(sockfd, port); // reuse
}

int send_file_head(int sockfd, file_head *header)
{
    file_head fh;
    fh.size = header->size;
    strncpy(fh.name, header->name, sizeof(fh.name));
    Send(sockfd, &fh, sizeof(file_head), 0);
    return 0;
}

int recv_file_head(int sockfd, file_head *header)
{
    file_head *fh = (file_head *)Malloc(sizeof(file_head));
    if (Recv(sockfd, fh, sizeof(file_head), 0) == 0)
        return -1;
    header->size = fh->size;
    strncpy(header->name, fh->name, sizeof(header->name));
    return 0;
}

int send_file(int sockfd, char *filename)
{
    file_head fh;
    FILE *fd;
    char data[MAXLINE];
    int read_c;
    char name[MAX_NAME];

    // filename in server end is not NULL
    if (filename != NULL)
    {
        strncpy(name, filename, sizeof(name));
    }
    else
    {
        // client receive file_head for creating file
        if (recv_file_head(sockfd, &fh) == -1)
            return -1;
        strncpy(name, fh.name, sizeof(name));
    }

    fh.size = get_file_size(name);
    strncpy(fh.name, name, sizeof(fh.name));
    send_file_head(sockfd, &fh);

    fd = Fopen(name, "r");
    fseek((FILE *)fd, SEEK_SET, 0);
    while ((read_c = fread(data, 1, MAXLINE, fd)) > 0)
    {
        Send(sockfd, data, read_c, 0);
    }
    Fclose(fd);

    return 0;
}

int recv_file(int sockfd, char *filename)
{
    file_head fh;
    long long count;
    char data[MAXLINE];
    FILE *fd;
    int i, n;

    // filename in server end is not NULL
    if (filename != NULL)
    {
        fh.size = 0;
        strncpy(fh.name, filename, sizeof(fh.name));
        send_file_head(sockfd, &fh);
    }

    if (recv_file_head(sockfd, &fh) == -1)
        return -1;

    fd = Fopen(fh.name, "w+");
    fseek((FILE *)fd, SEEK_SET, 0);

    // number of times for receiving data
    count = fh.size / MAXLINE;
    for (i = 0; i < ((fh.size % MAXLINE) ? count + 1 : count); i++)
    {
        bzero(data, sizeof(data));
        if ((n = Recv(sockfd, data, MAXLINE, 0)) == 0)
        {
            printf("---Connection end!---\n");
            exit(0);
        }
        fwrite(data, 1, n, fd);
    }
    Fclose(fd);

    return 0;
}

void trimstr(char *str, int n)
{
    int i;
    for (i = n-1; i >= 0; i--)
    {
        if (isspace(str[i]) || str[i] == '\n') 
            str[i] = 0; // remove white
        else
            break;
    }
}

long long get_file_size(char *strFileName)
{
    struct stat temp;
    stat(strFileName, &temp);
    return temp.st_size;
}
