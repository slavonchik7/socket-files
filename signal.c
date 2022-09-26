
#include "signal.h"



sig_handler_t *sigact(int signo, sig_handler_t *shand) {
    struct sigaction act, oldact;

    act.sa_handler = shand;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if ( signo == SIGALRM ) {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif // SA_INTERRUPT
    } else {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif // SA_RESTART
    }

    if ( sigaction(signo, &act, &oldact) < 0 )
        return (SIG_ERR);

    return (oldact.sa_handler);
}




sig_handler_t *Sigact(int signo, sig_handler_t *shand) {
	sig_handler_t	*sigfunc;

	if ( (sigfunc = sigact(signo, shand)) == SIG_ERR )
		EXIT_WITH_LOG_ERROR(NULL, NULL, strerror(errno), errno);

	return (sigfunc);

}

