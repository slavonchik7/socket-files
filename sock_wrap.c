



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
            if ( (pslct_ret = pselect(fd + 1, &wrset, NULL, NULL, &acpt_per, NULL)) > 0 ) {
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








