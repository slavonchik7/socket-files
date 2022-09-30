


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

    int confd;



    struct sockaddr_in servaddr;

void pipe_handler() {

    printf("GETTED PIP\n");
    fflush(stdout);

}

#define NOPIPE
#define WRTIME


void exit_client() {
//    sleep(5);

    close(confd);
    printf("close client!\n");
    fflush(stdout);
//    sleep(5);
    exit(1);

}



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

            printf("send: \n");
        }

//        Read_time(clifd, buf, BUFSIZ, 5, 1000);
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




    int optval;
    socklen_t optlen;
    socklen_t skl = sizeof(servaddr);
    int flags;
    int imode;
    char buff[BUFSIZ];
//    printf("OK\n");
//    fflush(stdout);
    confd = Socket(AF_INET, SOCK_STREAM, 0);

    int keepval = 0;
    optlen = sizeof(keepval);
//    setsockopt(confd, SOL_SOCKET, SO_REUSEADDR, &keepval, optlen);
    Getsockopt(confd, IPPROTO_IP, IP_TTL, &keepval, &optlen);
    printf("ip_ttl: %d, sizeof: %d\n", keepval, optlen);

//    getsockopt(confd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen);
//    printf("SO_KEEPALIVE: %d\n", optval);
//
//    optval = 1;
//    /* first check tcp connect */
//    int keepval = 1;
//    setsockopt(confd, SOL_TCP, TCP_KEEPCNT, &keepval, optlen);
//
//    /* second check tcp connect */
//    keepval = 1;
//    setsockopt(confd, SOL_TCP, TCP_KEEPCNT, &keepval, optlen);
//
//    /* second check tcp connect */
//    keepval = 1;
//    setsockopt(confd, SOL_TCP, TCP_KEEPCNT, &keepval, optlen);
//
//    setsockopt(confd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);
//    printf("SO_KEEPALIVE: %d\n", optval);


#if 1
    printf("EFAULT %d\n", EFAULT);
    printf("EINPROGRESS %d\n", EINPROGRESS);
    printf("EINTR %d\n", EINTR);
    printf("ENETUNREACH %d\n", ENETUNREACH);
    printf("EPROTOTYPE %d\n", EPROTOTYPE);
    printf("EACCES %d\n", EACCES);
    printf("EADDRINUSE %d\n", EADDRINUSE);
    printf("EADDRNOTAVAIL %d\n", EADDRNOTAVAIL);
    printf("EPERM %d\n", EPERM);
    printf("EAGAIN %d\n", EAGAIN);
    printf("EISCONN %d\n", EISCONN);
    printf("ETIMEDOUT %d\n", ETIMEDOUT);
    printf("ECONNREFUSED %d\n", ECONNREFUSED);
    printf("EPIPE %d\n", EPIPE);
    printf("EWOULDBLOCK %d\n", EWOULDBLOCK);
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


    if ( getsockopt(confd, IPPROTO_TCP, SO_RCVBUF, &keepval, &skl) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
    else
        printf("sock opt SO_RCVBUF: %d\n", keepval);
        optlen = sizeof(keepval);
    if ( getsockopt(confd, IPPROTO_TCP, SO_SNDBUF, &keepval, &skl) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
    else
        printf("sock opt SO_SNDBUF: %d\n", keepval);
        optlen = sizeof(keepval);
    if ( getsockopt(confd, IPPROTO_TCP, TCP_MAXSEG, &keepval, &skl) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
    else
        printf("sock opt TCP_MAXSEG: %d\n", keepval);



    Connect(confd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr));

        Sigact(SIGINT, exit_client);

optlen = sizeof(keepval);
    if ( getsockopt(confd, IPPROTO_TCP, SO_RCVBUF, &optval, &skl) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
    else
        printf("sock opt SO_RCVBUF: %d\n", optval);
        optlen = sizeof(keepval);
    if ( getsockopt(confd, IPPROTO_TCP, SO_SNDBUF, &keepval, &skl) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
    else
        printf("sock opt SO_SNDBUF: %d\n", keepval);
        optlen = sizeof(keepval);
    if ( getsockopt(confd, IPPROTO_TCP, TCP_MAXSEG, &optval, &skl) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
    else
        printf("sock opt TCP_MAXSEG: %d\n", optval);

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


