


#include "wrap_fork.h"

pid_t proc_create(proc_func *pfunc, void *pf_data) {

    pid_t chldpid;
    pid_t status;

    if ( (status = fork()) == 0 ) {
        pfunc(pf_data);
        chldpid = getpid();
    } else if ( status < 0 )
        EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);



    return chldpid;
}

