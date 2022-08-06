#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFERSIZE 1024

int main(int argc, const char **argv)
{
    if (argc != 3)
    {
        write(1, "Usage: <ip> <port>\n", 20);
        return 1;
    }

    char buf[BUFFERSIZE] = {0};

    int r_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (r_socket == -1)
    {
        perror("socket");
        return 2;
    }

    int opt = 1;
    setsockopt(r_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr_server;
    addr_server.sin_addr.s_addr = inet_addr(argv[1]);
    addr_server.sin_port = htons(atoi(argv[2]));
    addr_server.sin_family = AF_INET;

    if (-1 ==
        connect(r_socket, (struct sockaddr *)&addr_server, sizeof(addr_server)))
    {
        perror("connect");
        return 3;
    }

    fd_set rds, wrs;
    for (;;)
    {
        FD_ZERO(&rds);
        FD_ZERO(&wrs);
        FD_SET(0, &rds); /* 0 - the standard input stream */
        FD_SET(r_socket, &rds);
        int stat = select(r_socket + 1, &rds, &wrs, NULL, NULL);
        if (stat <= 0)
        {
            perror("select");
            return 4;
        }
        if (FD_ISSET(r_socket, &rds))
        {
            int rc = read(r_socket, buf, BUFFERSIZE);
            if (rc == -1)
            {
                perror("read");
                return 5;
            }
            if (rc == 0)
                break;
            write(1, buf, rc); /* 1 - the standard output stream */
            memset(buf, 0, rc);
        }
        if (FD_ISSET(0, &rds))
        {
            int rc = read(0, buf, BUFFERSIZE);
            if (rc == -1)
            {
                perror("read");
                return 5;
            }
            if (rc == 0)
                break;
            write(r_socket, buf, rc); /* 1 - the standard output stream */
            memset(buf, 0, rc);
        }
    }

    return 0;
}