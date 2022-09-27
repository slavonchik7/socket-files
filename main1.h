



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
#include <sys/wait.h>
#include <sys/types.h>


#include "../wrap_fork.h"
#include "signal.h"
#include "sock_wrap.h"


#define SERV_ADDR "127.0.0.1"
#define SERV_PORT 2000


void str_echo(int fd) {


}

void sig_chld(int sign) {
    pid_t pid;
    int stat;


    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0 )
        printf("child (%d) terminated\n", pid);

    return;
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


//    flags = fcntl(confd, F_GETFL, 0);
//    imode = 1;
//    fcntl(confd, F_SETFL, flags | O_NONBLOCK);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("listen...\n");


    Sigact(SIGCHLD, sig_chld);

    Listen(listenfd, 1);

    printf("sleep...\n");
    sleep(1);
    printf("wake up...\n");




//    for(int  i = 0; i < 5; i++) {
//        pid_t p_id;
//        if ( (p_id = fork()) == 0 ) {
//
//            printf("child id...%d\n", getpid());
//
//            exit(0); /* exit child proc */
//        }
//    }
//    close(listenfd);


// gcc main.c msgee.c sock_wrap.c signal.c -o serv
// gcc client.c msgee.c sock_wrap.c signal.c -o client

    for (;;) {
        skl = sizeof(cliaddr);

        //confd = Accept(listenfd, (struct sockaddr *)&servaddr, &skl);
        if ( (confd = accept(listenfd, (struct sockaddr *)&servaddr, &skl)) < 0 ) {
            if ( errno == EINTR )
                continue;
            else
                EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
        }

        PROC_CHILD(chldpid)
            close(listenfd);
            printf("recieve client...\n");
            read(confd, buff, BUFSIZ);
//            if ( read_time(confd, buff, BUFSIZ, 3, 1000) < 0 )
//                printf("time out wait read FD\n");
//            else
                printf("msg: %s\n", buff);

            exit(0); /* exit child proc */
        PROC_PARENT(chldpid)
            close(confd);
        PROC_END

    }



//    printf("msg: %s\n", buff);

    close(confd);
    close(listenfd);

//        if ( getsockopt(confd, SOL_SOCKET, SO_ERROR, &optval, &skl) < 0 )
//            EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
//        else
//            printf("sock opt SO_ERROR: %d\n", optval);


    return 0;
}


#endif /* MAIN1_H */
