



#include "sock_wrap.h"



int Accept(int fd, struct sockaddr *addr, socklen_t *addr_len) {
    int sock;

again:
    if ( (sock = accept(fd, addr, addr_len)) < 0 ) {
#ifdef	EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
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

        if ( errno == EINPROGRESS ) {

            /* the socket had the O_NONBLOCK flag,
             so we wait for some time,
             after which we check the set of descriptors for writing */

            struct timespec acpt_per =
                    {   .tv_sec = 1,
                        .tv_nsec = 0 };

            fd_set wrset;
            FD_ZERO(&wrset);
            FD_SET(fd, &wrset);


            if ( pselect(fd + 1, &wrset, NULL, NULL, &acpt_per, NULL) > 0 ) {
                /* select did not return an error */

                if ( FD_ISSET(fd, &wrset) ) {
                    /* the socket is ready to write */


                    int chck_err;
                    socklen_t skl = sizeof(chck_err);

                    /* checking if an error has occurred
                     in the socket after waiting for pselect() */
                    if ( getsockopt(fd, SOL_SOCKET, SO_ERROR, &chck_err, &skl) >= 0 ) {
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

            } else
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


int Socket(int domain, int type, int protocol) {
    int sock;

    if ( (sock = socket(domain, type, protocol)) < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);


    return sock;
}






