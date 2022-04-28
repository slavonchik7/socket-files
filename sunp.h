
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#ifndef SUNP_H
#define SUNP_H



const char *
sock_ntop(const struct sockaddr *sa, socklen_t salen);


ssize_t
readn(int fd, void *buff, size_t maxlen);

ssize_t
writen(int fd, void *buff, size_t maxlen);

#endif /* SUNP_H */
