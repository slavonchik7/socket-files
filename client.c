


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
#include <sys/ioctl.h>


#include "sock_wrap.h"
#include "../wrap_fork.h"
#include "signal.h"


#define SERV_ADDR "127.0.0.1"
#define SERV_PORT 2000



void pipe_handler() {

    printf("GETTED PIP\n");
    fflush(stdout);

}

void cli_str(int clifd) {

    char buf[BUFSIZ];

    Sigact(SIGPIPE, pipe_handler);

    while ( fgets(buf, BUFSIZ, stdin) != NULL ) {

        if ( write(clifd, buf, BUFSIZ) < 0 ) {
            if (errno == EPIPE)
                printf("EPIP FIRST write\n");
            fprintf(stderr, "error write %s\n", strerror(errno));
        }

        sleep(1);

        if ( write(clifd, buf, BUFSIZ) < 0 ) {
            if (errno == EPIPE)
                printf("EPIP SECOND write\n");
            fprintf(stderr,"error write %s\n", strerror(errno));
        }

        if ( read(clifd, buf, BUFSIZ) < 0 ) {
            fprintf(stderr,"error write %s\n", strerror(errno));
        } else {
            fprintf(stderr,"%s\n", buf);
        }
    }

}

int main() {

    int confd;

    struct sockaddr_in servaddr;
    int optval;
    socklen_t skl = sizeof(servaddr);
    int flags;
    int imode;
    char buff[BUFSIZ];
//    printf("OK\n");
//    fflush(stdout);
    confd = Socket(AF_INET, SOCK_STREAM, 0);

#if 0
    printf("%d\n", EFAULT);
    printf("%d\n", EINPROGRESS);
    printf("%d\n", EINTR);
    printf("%d\n", ENETUNREACH);
    printf("%d\n", EPROTOTYPE);
    printf("%d\n", EACCES);
    printf("%d\n", EADDRINUSE);
    printf("%d\n", EADDRNOTAVAIL);
    printf("%d\n", EPERM);
    printf("%d\n", EAGAIN);
    printf("%d\n", EISCONN);
    printf("%d\n", ETIMEDOUT);
    printf("%d\n", ECONNREFUSED);
#endif

//    flags = fcntl(confd, F_GETFL, 0);
//    if ( flags & O_NONBLOCK )
//        printf("NON BLOCK\n");

//    fcntl(confd, F_SETFL, flags | O_NONBLOCK);

//    imode = 1;

//    ioctl(confd, FIONBIO, &imode);
//    flags = fcntl(confd, F_GETFL, 0);
//    if ( flags & O_NONBLOCK )
//        printf("NON BLOCK\n");


    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_ADDR, &servaddr.sin_addr);

    Connect(confd, (struct sockaddr *)&servaddr, skl);

//    if ( connect(confd, (struct sockaddr *)&servaddr, skl) < 0 ) {
//
//        if ( getsockopt(confd, SOL_SOCKET, SO_ERROR, &optval, &skl) < 0 )
//            EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
//        else
//            printf("sock opt SO_ERROR: %d\n", optval);
//
//        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
//    }
    cli_str(confd);

    close(confd);


    return 0;
}


