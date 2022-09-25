

#ifndef SOCK_WRAP_H
#define SOCK_WRAP_H



#include <sys/socket.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#undef NDEBUG
#endif /* NDEBUG */
#include <assert.h>

#include "msgee.h"
#include "tmspecdef.h"

int Accept(int fd, struct sockaddr *addr, socklen_t *addr_len);

int Accept_time(int fd, struct sockaddr *addr, socklen_t *addr_len, int tnum, double msec);

void Bind(int fd, const struct sockaddr *sa, socklen_t salen);

void Connect(int fd, const struct sockaddr *sa, socklen_t salen);

int Socket(int domain, int type, int protocol);




#endif /* SOCK_WRAP_H */
