

#ifndef SOCK_WRAP_H
#define SOCK_WRAP_H



#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

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

int reconnect(int *fd, const struct sockaddr *sa, socklen_t salen);

void Reconnect(int *fd, const struct sockaddr *sa, socklen_t salen);

int reconnect_time(int *fd, const struct sockaddr *sa, socklen_t salen, int tnum, double msec);

void Reconnect_time(int *fd, const struct sockaddr *sa, socklen_t salen, int tnum, double msec);

int connect_time(int fd, const struct sockaddr *sa, socklen_t salen, int tnum, double msec);

void Connect_time(int fd, const struct sockaddr *sa, socklen_t salen, int tnum, double msec);

int resocket(int oldfd, int domain, int type, int protocol);

int Resocket(int oldfd, int domain, int type, int protocol);


int Socket(int domain, int type, int protocol);

void Listen(int fd, int backlog);

void Setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);

void Getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr);

ssize_t Recv(int fd, void *ptr, size_t nbytes, int flags);


void Shutdown(int socket, int how);

ssize_t Write(int fd, const void *buf , size_t n);

ssize_t write_nopipe(int fd, const void *buf , size_t n);

ssize_t Write_nopipe(int fd, const void *buf , size_t n);

ssize_t write_time(int fd, const void *buf , size_t n, int tnum, double msec);

ssize_t Write_time(int fd, const void *buf , size_t n, int tnum, double msec);

ssize_t Read(int fd, void *buf , size_t nbytes);

ssize_t read_time(int fd, void *buf , size_t nbytes, int tnum, double msec);

ssize_t Read_time(int fd, void *buf , size_t nbytes, int tnum, double msec);





#endif /* SOCK_WRAP_H */


// https://github.com/unpbook/unpv13e.git
