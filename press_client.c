#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <mcp3004.h>

#define DIRECTION_MAX 45
#define BUFFER_MAX 128
#define VALUE_MAX 256


#define BASE 100
#define SPI_CHAN 0

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

    int song_1[] = {CM3,CM3};
    int beat_1[] = {2,2};

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

    if(wiringPiSetup() == -1){
        printf("wiringPiSetup initialization Failed !");
        return 1;                     // 
    }
    if(wiringPiSPISetup(0, 1000000) == -1) {    // wiringPiSPISetup(channel num, speed)
        printf("wiringPiSPISetup initialization Failed !");
        return 1;        
    }
    mcp3004Setup(BASE,SPI_CHAN);

    while (1)
    {
        // judge pressure state
        int aver1=0, aver2=0, aver3=0, aver4=0;
        int i;
        for(i=0; i<200; i++){
            aver1 += analogRead(BASE);
            aver2 += analogRead(BASE+1);
            aver3 += analogRead(BASE+2);
            aver4 += analogRead(BASE+3);
            //printf("FSR value : %d\n", analogRead(BASE));
            
            usleep(25000);                               
        }

        aver1 = aver1 / (i);
        aver2 = aver2 / (i);
        aver3 = aver3 / (i);
        aver4 = aver4 / (i);
        
        printf("Average 1 : %d\n", aver1);
        printf("Average 2 : %d\n", aver2);
        printf("Average 3 : %d\n", aver3);
        printf("Average 4 : %d\n", aver4);
        
        if(aver1 < 200 || aver2 < 200 || aver3 < 400 || aver4 < 400 ) state = 1;

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
