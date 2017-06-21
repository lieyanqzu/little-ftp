#include "ctl_handle.h"

void cmd_recevier(int sockfd)
{
    char cmd[CMD_MAX];
    int res;
    int login = 0;

    chdir("/");
    while (1)
    {
        bzero(cmd, sizeof(cmd));
        if (recv_cmd(sockfd, cmd, sizeof(cmd)) == -1)
            return;
        trimstr(cmd, strlen(cmd));

        res = command_handle(sockfd, cmd, &login);
        switch (res)
        {
            case -1:
                return;
            case LOGIN_NEED:
            case INVALID:
            case LOGIN_SUCCESS:
                send_rcode(sockfd, res);
        }
    }
}

command cmds[] = {
        {"login", wait_cli_login, " - If you have username and password, log in now!", NONEED},
        {"put", file_put_handle, " <file-name> - Upload one file.", NEED},
        {"get", file_get_handle, " <file-name> - Download one file.", NEED},
        {"ls", send_list, " [options] - List directory contents.", NEED},
        {"cd", change_dir, " <dir-name> - Change directory.", NEED},
        {"rm", remove_file, " <file-name> - Remove files.", NEED},
        {"mkdir", make_dir, " <dir-name> - Make directory.", NEED},
        {"pwd", get_pwd, " - Print Working Directory.", NEED},
        {"help", show_help, " - See something you are reading.", NONEED},
        {"bye", goodbye, " - Goodbye, world!", NONEED}, 
        {NULL, NULL, NULL, 0}};

int command_handle(int sockfd, char *cmd, int *login)
{
    command *ptr;

    ptr = cmds;
    if (strncasecmp(cmd, ptr->start, strlen(ptr->start)) == 0)
        return ptr->handle(sockfd, (char *)login);
    else
        ptr++;

    while (ptr->start != NULL)
    {
        if (strncasecmp(cmd, ptr->start, strlen(ptr->start)) == 0)
        {
            if (ptr->log_need == NEED && *login == 0)
                return LOGIN_NEED;
            else
                return ptr->handle(sockfd, cmd);
        }
        ptr++;
    }
    return INVALID;
}

int wait_cli_login(int sockfd, char *baduse)
{
    char cmd[LOG_MAX*2];
    int *login = (int *)baduse;

    if (*login == 1)
        return LOGIN_SUCCESS;

    send_rcode(sockfd, USER_LOGIN);
    while(1)
    {
        if (recv_cmd(sockfd, cmd, sizeof(cmd)) == -1)
            return -1;
        if (strncmp(cmd, "123 123", sizeof(cmd)) == 0)
        {
            send_rcode(sockfd, LOGIN_SUCCESS);
            *login = 1;
            return 1;
        }
        send_rcode(sockfd, LOGIN_FAILED);
        send_rcode(sockfd, USER_LOGIN);
    }
    return 0;
}

int send_list(int sockfd, char *cmd)
{
    char list_cmd[CMD_MAX * 2];
    char temp[] = "/tmp/1.temp";
    FILE * fd;
    long long size;
    char data[MAXLINE];
    int read_c;

    send_rcode(sockfd, SHOW_LIST);

    bzero(list_cmd, sizeof(list_cmd));
    snprintf(list_cmd, sizeof(list_cmd), "%s > %s", cmd, temp);
    system(list_cmd);

    size = get_file_size(temp);
    send_file_size(sockfd, size);

    fd = Fopen(temp, "r");
    fseek((FILE *)fd, SEEK_SET, 0);
    while ((read_c = fread(data, 1, MAXLINE, fd)) > 0)
    {
        Send(sockfd, data, read_c, 0);
    }
    Fclose(fd);

    bzero(list_cmd, sizeof(list_cmd));
    snprintf(list_cmd, sizeof(list_cmd), "rm %s", temp);
    system(list_cmd);
    return 0;
}

int change_dir(int sockfd, char *cmd)
{
    char buf[128];
    char chdir_cmd[CMD_MAX * 2];
    char *arg;

    getcwd(buf, sizeof(buf));
    strtok(cmd, " ");
    arg = strtok(NULL, "");

    if (*arg != '/')
        snprintf(chdir_cmd, sizeof(chdir_cmd), "%s/%s", buf, arg);
    else
        snprintf(chdir_cmd, sizeof(chdir_cmd), "%s", cmd+3);

    if (chdir(chdir_cmd) == 0)
        send_rcode(sockfd, CHDIR_SUCCESS);
    else
        send_rcode(sockfd, CHDIR_FAILED);
    return 0;
}

int make_dir(int sockfd, char *cmd)
{
    char buf[128];
    char mkdir_cmd[CMD_MAX * 2];
    char *arg;

    getcwd(buf, sizeof(buf));
    strtok(cmd, " ");
    arg = strtok(NULL, "");

    if (*arg != '/')
        snprintf(mkdir_cmd, sizeof(mkdir_cmd), "%s/%s", buf, arg);
    else
        snprintf(mkdir_cmd, sizeof(mkdir_cmd), "%s", cmd + 6);

    if (mkdir(mkdir_cmd, 0755) == 0)
        send_rcode(sockfd, MKDIR_SUCCESS);
    else
        send_rcode(sockfd, MKDIR_FAILED);
    return 0;
}

int get_pwd(int sockfd, char *filler)
{
    char buf[128];

    getcwd(buf, sizeof(buf));
    snprintf(buf, sizeof(buf), "%s\n", buf);
    send_rcode(sockfd, SHOW_LIST);
    send_file_size(sockfd, strlen(buf)+1);
    Send(sockfd, buf, sizeof(buf), 0);
    return 0;
}

int remove_file(int sockfd, char *cmd)
{
    system(cmd);
    send_rcode(sockfd, NORMAL_BACK);
    return 0;
}

int show_help(int sockfd, char *filler)
{
    char help[MAX_HELP];
    char temp[MAX_TEMP];
    command *ptr = cmds;

    snprintf(help, sizeof(help), "%s", "Maybe you wanna a Command List:\n"
                                        "Sadly, here is all :(\n\n");
    while (ptr->start != NULL)
    {
        snprintf(temp, sizeof(temp), "\t%s%s\n", ptr->start, ptr->help);
        strncat(help, temp, sizeof(help) - strlen(help) - 1);
        ptr++;
    }
    strncat(help, "\n", sizeof(help) - strlen(help) - 1);
    send_rcode(sockfd, SHOW_LIST);
    send_file_size(sockfd, strlen(help) + 1);
    Send(sockfd, help, sizeof(help), 0);
    return 0;
}

int goodbye(int sockfd, char *filler)
{
    send_rcode(sockfd, CONN_END);
    return -1;
}

int file_put_handle(int sockfd, char *cmd)
{
    return file_trans_handle(sockfd, cmd, RECV);
}

int file_get_handle(int sockfd, char *cmd)
{
    return file_trans_handle(sockfd, cmd, SEND);
}

int file_trans_handle(int sockfd, char *cmd, int op)
{
    int listenfd, clifd;
    char *arg;
    int port;
    int ret;
    struct sockaddr_in servaddr;

    strtok(cmd, " ");
    arg = strtok(NULL, "");

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    port = Sock_bind_wild(listenfd, AF_INET);
    Close(listenfd);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);

    switch (op)
    {
        case SEND:
            send_rcode(sockfd, GET_START);
            break;
        case RECV:
            send_rcode(sockfd, PUT_START);
            break;
        default:
            return -1;
    }

    send_port(sockfd, port);
    clifd = Accept(listenfd, NULL, NULL);

    switch (op)
    {
        case SEND:
            ret = send_file(clifd, arg);
            break;
        case RECV:
            ret = recv_file(clifd, arg);
            break;
    }

    Close(listenfd);
    Close(clifd);
    return ret;
}