



#include "sock_wrap.h"



int Accept(int fd, struct sockaddr *addr, socklen_t *addr_len) {
    assert(fd);
    assert(addr);
    assert(addr_len);

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
    assert(fd);
    assert(addr);
    assert(addr_len);
    assert(tnum >= 0);
    assert(msec >= 0);

    struct timespec acpt_per;
    struct timespec *tmptr = &acpt_per;
    int sock;


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
    assert(fd >= 0);
    assert(sa);

	if ( bind(fd, sa, salen) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}


int resocket(int oldfd, int domain, int type, int protocol) {
    int fdflags;


    if ( (fdflags = fcntl(oldfd, F_GETFL, 0)) < 0 )
        return -1;

    if ( shutdown(oldfd, SHUT_RDWR) < 0 )
        /* if the socket uses udp packets or for another reason */
        if ( close(oldfd) < 0 )
            return -1;

    if ( (oldfd = socket(domain, type, protocol)) < 0 )
        return -1;

    if ( (fcntl(oldfd, F_SETFL, fdflags)) < 0 ) {
        close(oldfd);
        return -1;
    }

//    printf("resocket OK\n");
    return oldfd;
}

int Resocket(int oldfd, int domain, int type, int protocol) {
    assert(oldfd >= 0);

    int newfd;

    if ( (newfd = resocket(oldfd, domain, type, protocol)) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    return newfd;
}


int reconnect(int *fd, const struct sockaddr *sa, socklen_t salen) {
    int sfd = *fd;
    struct sockaddr_in *saddr = (struct sockaddr_in *)sa;


    if ( (sfd = resocket(sfd, sa->sa_family, SOCK_STREAM, 0)) < 0)
        return -1;

    if ( connect(sfd, sa, salen) < 0 ) {
        close(sfd);
        return -1;
    }

    *fd = sfd;

    return 0;
}

void Reconnect(int *fd, const struct sockaddr *sa, socklen_t salen) {
    assert(fd);
    assert(*fd >= 0);
    assert(sa);

    if ( reconnect(fd, sa, salen) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}


int reconnect_time(int *fd, const struct sockaddr *sa, socklen_t salen, int tnum, double msec) {
    int sfd = *fd;
    struct sockaddr_in *saddr = (struct sockaddr_in *)sa;


    if ( (sfd = resocket(sfd, sa->sa_family, SOCK_STREAM, 0)) < 0)
        return -1;

    if ( connect_time(sfd, sa, salen, tnum, msec) < 0 ) {
        close(sfd);
        return -1;
    }

    *fd = sfd;

    return 0;
}

void Reconnect_time(int *fd, const struct sockaddr *sa, socklen_t salen, int tnum, double msec) {
    assert(fd);
    assert(*fd >= 0);
    assert(tnum >= 0);
    assert(msec >= 0);

    if ( reconnect_time(fd, sa, salen, tnum, msec) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}

void Connect_time(int fd, const struct sockaddr *sa, socklen_t salen, int tnum, double msec) {
    assert(fd >= 0);
    assert(tnum >= 0);
    assert(msec >= 0);

    if ( connect_time(fd, sa, salen, tnum, msec) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}


int connect_time(int fd, const struct sockaddr *sa, socklen_t salen, int tnum, double msec) {
    struct timespec acpt_per;
    struct timespec *tmptr = &acpt_per;
    fd_set wrset;
    int fdflags;
    int stat = 0;

    MSEC_TO_SPEC(msec, tmptr);

    FD_ZERO(&wrset);
    FD_SET(fd, &wrset);


    fdflags = fcntl(fd, F_GETFL, 0);
    if ( fcntl(fd, F_SETFL, fdflags | O_NONBLOCK) < 0 )
        return -1;


    while ( tnum-- ) {

        if ( (stat = connect(fd, sa, salen)) < 0 ) {
            printf(" < 0\n");
            if ( pselect(fd + 1, NULL, NULL, NULL, tmptr, NULL) >= 0 )
                continue;
            else
                goto func_end;
        } else {
            pselect(fd + 1, NULL, &wrset, NULL, NULL, NULL);
            if ( FD_ISSET(fd, &wrset) )
                goto func_end;
            else
                continue;
        }
    }

func_end:

    if ( fcntl(fd, F_SETFL, fdflags) < 0 )
        return -1;


    return stat;
}


void Shutdown(int socket, int how) {
    assert(socket >= 0);

    if ( shutdown(socket, how) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
    assert(fd >= 0);
    assert(sa);

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

    } else {
//        sleep(5);
//        printf("connect OK!\n");
        return; /* connect() is OK */
    }

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
    assert(fd >= 0);

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
	assert(fd >= 0);
	assert(flags >= 0);

	ssize_t n;

	if ( (n = recv(fd, ptr, nbytes, flags)) < 0 )
		EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

	return(n);
}



ssize_t write_nopipe(int fd, const void *buf , size_t n) {

    sigset_t oldset, newset;
    ssize_t nwrite;

    /* ignoring the SIGPIPE signal */
    signal(SIGPIPE, SIG_IGN);

    if ( (nwrite = write(fd, buf, n)) >= 0 ) {
        int stat;

        /* if write() was completed successfully,
         we check if the RST segment was received, that is,
         the server closed the socket through which the connection was going,

         we call write() again to check the assumption */
        if ( (stat = write(fd, NULL, 0)) < 0)
            return (ssize_t)stat;
    }

    /* enabling SIGPIPE signal processing */
    signal(SIGPIPE, SIG_DFL);

    return nwrite;
}

ssize_t Write_nopipe(int fd, const void *buf , size_t n) {
    assert(fd >= 0);

    ssize_t nwrite;

    if ( (nwrite = write_nopipe(fd, buf, n)) < 0 )
        /* we terminate the process if any error
         has occurred, except for EPIPE */
        if ( errno != EPIPE )
            EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    return nwrite;
}

ssize_t Write(int fd, const void *buf , size_t n) {
    assert(fd >= 0);

    ssize_t nwrite;

    if ( (nwrite = write(fd, buf, n)) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    return nwrite;
}

ssize_t Read(int fd, void *buf , size_t nbytes) {
    assert(fd >= 0);

    ssize_t nread;

    if ( (nread = read(fd, buf, nbytes)) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    return nread;
}


ssize_t read_time(int fd, void *buf , size_t nbytes, int tnum, double msec) {

    struct timespec acpt_per;
    struct timespec *tmptr = &acpt_per;
    fd_set rdset;
    ssize_t nread;
    MSEC_TO_SPEC(msec, tmptr);
    int fdflags;



    fdflags = fcntl(fd, F_GETFL, 0);
    if ( fcntl(fd, F_SETFL, fdflags | O_NONBLOCK) < 0 )
        return -1;


    while ( tnum-- ) {

        if ( (nread = read(fd, buf, nbytes)) < 0 ) {

            /* checking if an error has occurred, except EWOULDBLOCK
             in the socket after waiting for pselect() */
            if ( errno == EWOULDBLOCK ) {
                FD_ZERO(&rdset);
                FD_SET(fd, &rdset);

                if ( pselect(fd + 1, &rdset, NULL, NULL, tmptr, NULL) >= 0 )
                    continue;
                else
                    goto func_end;
            } else
                goto func_end;

        } else
            /* tcp connection closed, FIN segment was received earlier */
            return nread;
    }


func_end:

    if ( fcntl(fd, F_SETFL, fdflags) < 0 )
        return -1;

    return -1;
}


ssize_t Read_time(int fd, void *buf , size_t nbytes, int tnum, double msec) {
    assert(fd >= 0);
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
    ssize_t nwrite;

    MSEC_TO_SPEC(msec, tmptr);




    while ( tnum-- ) {

        if ( (nwrite = write(fd, buf, n)) < 0 ) {
            FD_ZERO(&wrset);
            FD_SET(fd, &wrset);

            if ( pselect(fd + 1, &wrset, NULL, NULL, tmptr, NULL) >= 0 )
                continue;
            else
                return -1;
        } else
            return nwrite;
    }

    return -1;
}


ssize_t Write_time(int fd, const void *buf , size_t n, int tnum, double msec) {
    assert(fd >= 0);
    assert(tnum >= 0);
    assert(msec >= 0);

    ssize_t nwrite;

    if ( (nwrite = write_time(fd, buf, n, tnum, msec)) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

    return nwrite;
}


int fcntl_setfl(int fd, int flgs) {

	int flags;

	if ( (flags = fcntl(fd, F_GETFL, 0)) < 0 )
        return -1;

	return fcntl(fd, F_SETFL, (flags | flgs) );
}

void Fcntl_setfl(int fd, int flgs) {

    assert(fd >= 0);

    if ( fcntl_setfl(fd, flgs) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}


int fcntl_unsetfl(int fd, int flgs) {

	int flags;

	if ( (flags = fcntl(fd, F_GETFL, 0)) < 0 )
        return -1;

	return fcntl(fd, F_SETFL, (flags & (~flgs)) );
}


void Fcntl_unsetfl(int fd, int flgs) {

    assert(fd >= 0);

    if ( fcntl_unsetfl(fd, flgs) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);
}
