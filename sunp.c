
#include "sunp.h"

const char *
sock_ntop(const struct sockaddr *sa, socklen_t salen) {
    char portstr[7];
    static char strp[INET6_ADDRSTRLEN + 8];

    switch (sa->sa_family) {
    case AF_INET: {
        struct sockaddr_in *sin = (struct sockaddr_in *)sa;

        if (inet_ntop(AF_INET, &sin->sin_addr, strp, sizeof(strp)) == NULL)
            return (NULL);

        uint16_t port_htns;
        if ((port_htns = htons(sin->sin_port)) != 0) {
            snprintf(portstr, sizeof(portstr), "|%d", port_htns);
            strcat(strp, portstr);
        }
        return strp;
    }
    case AF_INET6: {
        struct sockaddr_in6 *sin = (struct sockaddr_in6 *)sa;

        if (inet_ntop(AF_INET6, &sin->sin6_addr, strp, sizeof(strp)) == NULL)
            return (NULL);

        uint16_t port_htns;
        if ((port_htns = htons(sin->sin6_port)) != 0) {
            snprintf(portstr, sizeof(portstr), "|%d", port_htns);
            strcat(strp, portstr);
        }
        return strp;
    }
    default:
        return (NULL);
    }
}



ssize_t
readn(int fd, void *buff, size_t maxlen) {

    size_t nleft = maxlen;
    char *vptr = (char *)buff;
    ssize_t nread;

    while (nleft > 0) {
        if ((nread = read(fd,vptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return (-1);
        } else if (nread == 0)
            break;

        nleft -= nread;
        vptr += nread;
    }

    return (maxlen - nleft);
}


ssize_t
writen(int fd, void *buff, size_t maxlen) {
    size_t nleft = maxlen;
    const char *vptr = buff;
    ssize_t nwrite;

    while (nleft > 0) {
        if ((nwrite = write(fd, vptr, nleft)) <= 0) {
            if (errno == EINTR)
                nwrite = 0;
            else
                return (-1);
        }
        nleft -= nwrite;
        vptr += nwrite;
    }
    return maxlen;
}
