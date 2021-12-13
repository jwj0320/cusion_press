// gcc -o press_client press_client -lwiringPi
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define DIRECTION_MAX 45
#define BUFFER_MAX 128
#define VALUE_MAX 256


void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int port=8888;
    int sock;
    struct sockaddr_in serv_addr;
    char msg[BUFFER_MAX];
    int state=0; //pressure state

    if (argc != 2)
    {
        printf("Usage : %s <IP>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(port));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");
    while (1)
    {
        // judge pressure state

        if(state==1)
        {
            sprintf(msg, "%d", state);
            write(sock, msg, sizeof(msg));
        }

        usleep(500 * 100);

    }
    close(sock);

    return (0);
}
