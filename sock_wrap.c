



#include "sock_wrap.h"



int Accept(int fd, struct sockaddr *addr, socklen_t *addr_len) {
    int sock;

again:
    if ( (sock = accept(fd, addr, addr_len)) < 0 ) {
		if ( errno == ECONNABORTED
#ifdef	EPROTO
             || errno == EPROTO
#endif
            )
			goto again;
        else
            EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
    }

    return sock;
}



int Accept_time(int fd, struct sockaddr *addr, socklen_t *addr_len, int tnum, double msec) {
    struct timespec acpt_per;
    struct timespec *tmptr = &acpt_per;
    int sock;

    assert(tnum >= 0);
    assert(msec >= 0);

    MSEC_TO_SPEC(msec, tmptr);


again:
    if ( (sock = accept(fd, addr, addr_len)) < 0 ) {

		if ( errno == ECONNABORTED
#ifdef	EPROTO
             || errno == EPROTO
#endif
            )
            /* The connection was interrupted */
            goto again;

        else if ( errno = EAGAIN || EWOULDBLOCK ) {
            /* if the socket is set to O_NONBLOCK,
            then we are waiting for a ready connection request,
            calling accept() tnum once for tmpptr each */

            pselect(0, NULL, NULL, NULL, tmptr, NULL);

			if ( tnum-- )
                goto again;
        }

        else
            EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
    }

    return sock;

}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
	if ( bind(fd, sa, salen) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}


int resocket(int fd) {
    int sfd;
    int fdflags;


    if ( (fdflags = fcntl(fd, F_GETFL, 0)) < 0 )
        return -1;

    if ( shutdown(sfd, SHUT_RDWR) < 0 )
        if ( close(sfd) < 0)
            return -1;

    if ( (sfd = socket(saddr->sin_family, SOCK_STREAM, 0)) < 0 ) {
        *fd = sfd;
        return -1;
    }

    if ( (fcntl(fd, F_SETFL, &fdflags)) < 0 ) {
        close(sfd);
        return -1;
    }
}

int reconnect(int *fd, const struct sockaddr *sa, socklen_t salen) {
    int sfd = *fd;
    struct sockaddr_in *saddr = (struct sockaddr_in *)sa;




    if ( connect(fd, sa, ) salen) < 0 ) {
        close(sfd);
        return -1;
    }

    return 0;
}

int reconnect_time(int *fd, const struct sockaddr *sa, socklen_t salen, int tnum, double msec);

int connect_time(int fd, const struct sockaddr *sa, socklen_t salen, int tnum, double msec);


int connect_time(int fd, const struct sockaddr *sa, socklen_t salen, int tnum, double msec) {
    struct timespec acpt_per;
    struct timespec *tmptr = &acpt_per;
    fd_set wrset;
    int fdflags;
    ssize_t nread = -1;
    int conn;

    MSEC_TO_SPEC(msec, tmptr);

    FD_ZERO(&wrset);
    FD_SET(fd, &wrset);

//    close(fd);
//    if ( shutdown(fd, SHUT_RDWR) < 0

//    shutdown(fd, SHUT_RD);


    fdflags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, fdflags | O_NONBLOCK);


    while ( tnum-- ) {

        if ( connect(fd, sa, salen) < 0 ) {
            if ( pselect(fd + 1, NULL, &wrset, NULL, tmptr, NULL) >= 0)
                continue;
            else
                return -1;
        } else
            return 0;

    #if 0
        printf("wait SELECT\n");

        /* we are waiting until the socket is ready to write */
        int status = pselect(fd + 1, NULL, &wrset, NULL, tmptr, NULL);
        printf("out SELECT\n");

        if ( status > 0 ) {
            printf("ok SELECT\n");
            if ( FD_ISSET(fd, &wrset) ) {
                printf("isset ok \n");
                Connect(fd, sa, salen);
                return 0;
            }
            else
                continue;
        }
        else if ( status == 0 )
            continue;
        else
            return status;

        #endif // 0
    }


    /* an unforeseen event */
    return -1;
}


void Shutdown(int socket, int how) {
    if ( shutdown(socket, how) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen) {

    assert(sa);
    assert(salen > 0);

	if ( connect(fd, sa, salen) < 0 ) {
        /* connect() error */
        printf("connect error\n");
        if ( errno == EINPROGRESS ) {
            printf("connect errno: EINPROGRESS\n");

            /* the socket had the O_NONBLOCK flag,
             so we wait for some time,
             after which we check the set of descriptors for writing */

            struct timespec acpt_per =
                    {   .tv_sec = 1,
                        .tv_nsec = 0 };

            fd_set wrset;
            FD_ZERO(&wrset);
            FD_SET(fd, &wrset);

            int pslct_ret;
            printf("run pselect 1 sec\n");
            if ( (pslct_ret = pselect(fd + 1, NULL, &wrset, NULL, &acpt_per, NULL)) > 0 ) {
                /* select did not return an error */
                printf("run pselect succesfull \n");

                if ( FD_ISSET(fd, &wrset) ) {
                    /* the socket is ready to write */

                    printf("fd_isset find our socket's fd \n");

                    int chck_err;
                    socklen_t skl = sizeof(chck_err);

                    /* checking if an error has occurred
                     in the socket after waiting for pselect() */
                    if ( getsockopt(fd, SOL_SOCKET, SO_ERROR, &chck_err, &skl) >= 0 ) {
                        printf("getsockopt return OK\n");
                        printf("error SO_ERROR: %d\n", chck_err);
                        if ( chck_err == 0 )
                            /* no errors occurred during pselect() */
                            return;
                        else
                            goto exit_err;

                    } else
                        goto exit_err; /* getsockopt() returned ERROR */

                } else
                    /* the socket is not ready for writing,
                     so the pselect() timeout has expired */
                    goto exit_err;

            } else if (pslct_ret == 0)
                printf("pselect() time out\n");

                else
                goto exit_err; /* pselect() returned ERROR */

        } else
            /* the socket does not have a flag,
            so it will not be possible to establish a connection */
            goto exit_err;

    } else
        return; /* connect() is OK */

exit_err:
    EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}

void Listen(int fd, int backlog) {
    assert(backlog >= 0);
    assert(fd >= 0);

	char *ptr;

		/* 4can override 2nd argument with environment variable */
	if ( (ptr = getenv("LISTENQ")) != NULL )
		backlog = atoi(ptr);

	if ( listen(fd, backlog) < 0 )
		EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

}


void Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr) {

	if ( getsockopt(fd, level, optname, optval, optlenptr) < 0 )
		EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}


void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen) {

	if ( setsockopt(fd, level, optname, optval, optlen) < 0 )
		EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}



int Socket(int domain, int type, int protocol) {
    int sock;

    if ( (sock = socket(domain, type, protocol)) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    return sock;
}


ssize_t Recv(int fd, void *ptr, size_t nbytes, int flags) {
	assert(ptr);
	assert(flags >= 0);

	ssize_t n;

	if ( (n = recv(fd, ptr, nbytes, flags)) < 0 )
		EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

	return(n);
}



ssize_t write_nopipe(int fd, const void *buf , size_t n) {

    sigset_t oldset, newset;
    ssize_t nwrite;


//    sigemptyset(&newset);

    /* ignoring the SIGPIPE signal */
    signal(SIGPIPE, SIG_IGN);


//    sigfillset(&newset);
//    sigaddset(&newset, SIGPIPE);
//    pthread_sigmask(SIG_BLOCK, &newset, &oldset);
//    int st;
//    sigprocmask(SIG_SETMASK, &newset, &oldset);
//    newset = oldset;
//    sigdelset(&newset, SIGPIPE);


//    st = sigprocmask(SIG_SETMASK, &newset, &oldset);
//    printf("STATUS :%d\n", st);
//    struct timespec acpt_per = {1, 0};
//        fd_set wrset;
//    FD_ZERO(&wrset);
//    FD_SET(fd, &wrset);

    if ( (nwrite = write(fd, buf, n)) >= 0 ) {
        int stat;

        /* if write() was completed successfully,
         we check if the RST segment was received, that is,
         the server closed the socket through which the connection was going,

         we call write() again to check the assumption */
//        if ( pselect(fd + 1, NULL, &wrset, NULL, &acpt_per, NULL) < 0 ) {
//            printf("pselec EBADF: %d, error %d\n", EBADF, errno);
//            return -1;
//        }
        if ( (stat = write(fd, NULL, 0)) < 0)
            return (ssize_t)stat;
    }

//    printf("writed\n");
    /* enabling SIGPIPE signal processing */
    signal(SIGPIPE, SIG_DFL);
//    sigprocmask(SIG_SETMASK, &oldset,  &oldset);
//    pthread_sigmask(SIG_SETMASK, &newset, &oldset);

    return nwrite;
}

ssize_t Write_nopipe(int fd, const void *buf , size_t n) {
    ssize_t nwrite;

    if ( (nwrite = write_nopipe(fd, buf, n)) < 0 )
        /* we terminate the process if any error
         has occurred, except for EPIPE */
        if ( errno != EPIPE )
            EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    return nwrite;
}

ssize_t Write(int fd, const void *buf , size_t n) {
    ssize_t nwrite;

    if ( (nwrite = write(fd, buf, n)) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    return nwrite;
}

ssize_t Read(int fd, void *buf , size_t nbytes) {
    ssize_t nread;

    if ( (nread = read(fd, buf, nbytes)) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    return nread;
}


ssize_t read_time(int fd, void *buf , size_t nbytes, int tnum, double msec) {

    struct timespec acpt_per;
    struct timespec *tmptr = &acpt_per;
    fd_set rdset;
    ssize_t nread = -1;

    MSEC_TO_SPEC(msec, tmptr);

    FD_ZERO(&rdset);
    FD_SET(fd, &rdset);

    while ( tnum-- ) {

        /* we are waiting until the socket is ready to read */
        int status = pselect(fd + 1, &rdset, NULL, NULL, tmptr, NULL);

        if ( status > 0 )
            if ( FD_ISSET(fd, &rdset) )
                if ( (nread = read(fd, buf, nbytes)) < 0)
		    continue;
	        else
		    return break;
            else
                continue;
        else if ( status == 0 )
            continue;
        else
            break;
    }

    return nread;
}


ssize_t Read_time(int fd, void *buf , size_t nbytes, int tnum, double msec) {
    assert(tnum >= 0);
    assert(msec >= 0);

    ssize_t nread;

    if ( (nread = read_time(fd, buf, nbytes, tnum, msec)) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    return nread;
}


ssize_t write_time(int fd, const void *buf , size_t n, int tnum, double msec) {
    struct timespec acpt_per;
    struct timespec *tmptr = &acpt_per;
    fd_set wrset;
    ssize_t nwrite = -1;

    MSEC_TO_SPEC(msec, tmptr);

    FD_ZERO(&wrset);
    FD_SET(fd, &wrset);

    while ( tnum-- ) {

        /* we are waiting until the socket is ready to read */
        int status = pselect(fd + 1, NULL, &wrset, NULL, tmptr, NULL);

        if ( status > 0 )
            if ( FD_ISSET(fd, &wrset) )
                nwrite = write(fd, buf, n);
            else
                continue;
        else if ( status == 0 )
            continue;
        else
            return status;
    }

    return nwrite;
}


ssize_t Write_time(int fd, const void *buf , size_t n, int tnum, double msec) {
    assert(tnum >= 0);
    assert(msec >= 0);

    ssize_t nwrite;

    if ( (nwrite = write_time(fd, buf, n, tnum, msec)) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    return nwrite;
}

