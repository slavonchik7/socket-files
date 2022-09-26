
#ifndef SIGNAL_H
#define SIGNAL_H

#include <signal.h>
#include <errno.h>
#include <string.h>

#include "msgee.h"

typedef  void sig_handler_t(int);


sig_handler_t *sigact(int signo, sig_handler_t *shand);

sig_handler_t *Sigact(int signo, sig_handler_t *shand);

#endif /* SIGNAL_H */
