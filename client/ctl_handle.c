#include "ctl_handle.h"

void cmd_sender(int sockfd)
{
    char cmd[CMD_MAX];
    int rc;

    while (1)
    {
        fflush(stdin);
        printf("client>");
        Fgets(cmd, sizeof(cmd), stdin);

        send_cmd(sockfd, cmd, sizeof(cmd));
        if (recv_rcode(sockfd, &rc) == -1)
            rc = CONN_END;
        
        if ((ctl_word_handle(sockfd, rc)) == 1)
            return;
    }
}

int ctl_word_handle(int sockfd, int rc)
{
    switch (rc)
    {
    case USER_LOGIN:
        cli_login(sockfd, rc);
        break;
    case LOGIN_SUCCESS:
        printf("---Login success!---\n");
        break;
    case LOGIN_NEED:
        printf("---Please Login!---\n");
        break;
        
    case SHOW_LIST:
        get_list(sockfd);
        break;
    case CHDIR_SUCCESS:
    case NORMAL_BACK:
    case MKDIR_SUCCESS:
        break;
    case CHDIR_FAILED:
        printf("---chdir failed!---\n");
        break;
    case MKDIR_FAILED:
        printf("----mkdir failed!---\n");
        break;
    case INVALID:
        printf("---Invalid command!---\n");
        break;

    case GET_START:
        if (get_file(sockfd) == -1)
            cli_exit();
    case PUT_START:
        if (put_file(sockfd) == -1)
            cli_exit();
    case CONN_END:
        cli_exit();
    }

    return 0;
}

void cli_login(int sockfd, int rcode)
{
    int rc = rcode;
    char buf[LOG_MAX], logstr[LOG_MAX * 2];
    struct termios oflags, nflags;

    while (1)
    {
        if (rc == USER_LOGIN)
        {
            fflush(stdin);
            printf("Username: ");
            Fgets(buf, sizeof(buf), stdin);
            *strchr(buf, '\n') = ' ';
            strncpy(logstr, buf, sizeof(buf));

            tcgetattr(fileno(stdin), &oflags);
            nflags = oflags;
            nflags.c_lflag &= ~ECHO;
            nflags.c_lflag |= ECHONL;

            if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
            {
                err_sys("tcsetattr error");
            }

            printf("Password: ");
            Fgets(buf, sizeof(buf), stdin);
            *strchr(buf, '\n') = '\0';

            if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
            {
                err_sys("tcsetattr error");
            }

            strncat(logstr, buf, sizeof(logstr) - strlen(logstr) - 1);
            printf("\n");
            send_cmd(sockfd, logstr, sizeof(logstr));
        }

        if (rc == LOGIN_SUCCESS)
        {
            printf("---Login success!---\n");
            break;
        }

        if (rc == LOGIN_FAILED)
        {
            printf("---Login failed!---\n");
        }

        if (recv_rcode(sockfd, &rc) == -1)
        {
            cli_exit();
        }
            
    }
}

void get_list(int sockfd)
{
    long long size, count;
    char data[MAXLINE+1];
    int i;

    if (recv_file_size(sockfd, &size) == -1)
    {
        cli_exit();
    }

    count = size / MAXLINE;
    for (i = 0; i < ((size % MAXLINE) ? count + 1 : count); i++)
    {
        bzero(data, sizeof(data));
        if (Recv(sockfd, data, MAXLINE, 0) == 0)
        {
            cli_exit();
        }
        data[MAXLINE] = 0;
        printf("%s", data);
    }
}

int get_file(int sockfd)
{
    return trans_file(sockfd, RECV);
}

int put_file(int sockfd)
{
    return trans_file(sockfd, SEND);
}

int trans_file(int sockfd, int op)
{
    int port;
    int servfd;
    int ret;
    char port_s[6];
    struct sockaddr_in sin;
    socklen_t len;

    recv_port(sockfd, &port);
    snprintf(port_s, sizeof(port_s), "%d", port);
    Getpeername(sockfd, (SA *)&sin, &len);
    servfd = Tcp_connect(inet_ntoa(sin.sin_addr), port_s);

    switch (op)
    {
        case SEND:
            ret = send_file(servfd, NULL);
            break;
        case RECV:
            ret = recv_file(servfd, NULL);
            break;
        default:
            err_sys("trans_file");
    }

    Close(servfd);
    return ret;
}

void cli_exit()
{
    printf("---Connection end!---\n");
    exit(0);
}