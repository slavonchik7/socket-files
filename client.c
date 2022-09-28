


#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <error.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>

#include "sock_wrap.h"
#include "../wrap_fork.h"
#include "signal.h"


#define SERV_ADDR "127.0.0.1"
#define SERV_PORT 2000

    struct sockaddr_in servaddr;

void pipe_handler() {

    printf("GETTED PIP\n");
    fflush(stdout);

}

#define NOPIPE
#define WRTIME

void cli_str(int clifd, struct sockaddr *sa) {

    char buf[BUFSIZ];
//
//    Sigact(SIGPIPE, pipe_handler);

    while ( fgets(buf, BUFSIZ, stdin) != NULL ) {

        printf("buff: %s\n", buf);
        if (
#ifdef NOPIPE
        write_nopipe
//#elif WRTIME
//        Write_time
#else
        write
#endif // NOPIPE
        (clifd, buf, BUFSIZ) < 0 ) {
            if (errno == EPIPE)
//                printf("EPIP FIRST write\n");
            fprintf(stderr, "error write %s\n", strerror(errno));
            printf("reconnecting...\n");
            Reconnect_time(&clifd, sa, sizeof(struct sockaddr), 10, 1000);
            printf("reconnect succesfull\n");
//            if ( connect_time(clifd, (struct sockaddr *)&servaddr,
//                    sizeof(servaddr), 3, 1000) < 0)
//            printf("error connect_time() %d\n", errno);

        } else {
            printf("send\n");
        }

//        if ( Write_time(clifd, buf, BUFSIZ, 3, 1000) < 0 ) {
//            if (errno == EPIPE)
//                printf("EPIP FIRST write\n");
//            fprintf(stderr, "error write %s\n", strerror(errno));
//        } else {
//            printf("send\n");
//        }


//        sleep(1);
//
//        if (
//#if 0
//        write_nopipe
//#else
//        write
//    #endif // NOPIPE
//        (clifd, NULL, 0) < 0 ) {
//            if (errno == EPIPE)
//                printf("EPIP SECOND write\n");
//            fprintf(stderr,"error write %s\n", strerror(errno));
//        }

//        if ( read(clifd, buf, BUFSIZ) < 0 ) {
//            fprintf(stderr,"error write %s\n", strerror(errno));
//        } else {
//            fprintf(stderr,"%s\n", buf);
//        }
    }

}

int main() {

    int confd;


    int optval;
    socklen_t optlen;
    socklen_t skl = sizeof(servaddr);
    int flags;
    int imode;
    char buff[BUFSIZ];
//    printf("OK\n");
//    fflush(stdout);
    confd = Socket(AF_INET, SOCK_STREAM, 0);

    getsockopt(confd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen);
    printf("SO_KEEPALIVE: %d\n", optval);

    optval = 1;
    /* first check tcp connect */
    int keepval = 1;
    setsockopt(confd, SOL_TCP, TCP_KEEPCNT, &keepval, optlen);

    /* second check tcp connect */
    keepval = 1;
    setsockopt(confd, SOL_TCP, TCP_KEEPCNT, &keepval, optlen);

    /* second check tcp connect */
    keepval = 1;
    setsockopt(confd, SOL_TCP, TCP_KEEPCNT, &keepval, optlen);

    setsockopt(confd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
    printf("SO_KEEPALIVE: %d\n", optval);


#if 1
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
    printf("%d\n", EPIPE);
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
    cli_str(confd, (struct sockaddr *)&servaddr);

    close(confd);


    return 0;
}


