



#ifndef MAIN1_H
#define MAIN1_H

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <error.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "sock_wrap.h"

#define SERV_ADDR "127.0.0.1"
#define SERV_PORT 2000


void str_echo(int fd) {




}

int main1() {

    int listenfd, confd;
    pid_t chldpid;
    struct sockaddr_in  cliaddr, servaddr;
    int optval;
    socklen_t skl = sizeof(servaddr);
    int flags;
    int imode;
    char buff[BUFSIZ];
//    printf("OK\n");
//    fflush(stdout);
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

//
//    flags = fcntl(confd, F_GETFL, 0);
//    imode = 1;
//    fcntl(confd, F_SETFL, flags | O_NONBLOCK);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("listen...\n");



    listen(listenfd, 1);
    printf("sleep...\n");
    sleep(5);
    printf("wake up...\n");

    confd = Accept(listenfd, (struct sockaddr *)&servaddr, &skl);


    if ( recv(confd, buff, BUFSIZ, 0) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    printf("msg: %s\n", buff);

    close(confd);
    close(listenfd);

//        if ( getsockopt(confd, SOL_SOCKET, SO_ERROR, &optval, &skl) < 0 )
//            EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
//        else
//            printf("sock opt SO_ERROR: %d\n", optval);


    return 0;
}


#endif /* MAIN1_H */
