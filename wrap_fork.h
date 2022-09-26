


#ifndef WRAP_FORK_H

#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "socket-files/msgee.h"



#define PROC_CHILD(svpid) \
    do { \
        pid_t stat; \
        if ( (stat = fork()) == 0) { \
            svpid = getpid();

#define PROC_PARENT(svpid) \
        } else if ( stat > 0 ) { \
            svpid = getpid();

#define PROC_END \
        } else { \
            EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno); \
        } \
    } while(0);




typedef void proc_func(void *);

pid_t proc_create(proc_func *pfunc, void *pf_data);


#endif /* WRAP_FORK_H */
