
#include "msgee.h"


void exit_with_error(void ehandler(void *), void *ehdata,
        const char *m, int err) {

    if(m) {
        fprintf(stderr, "msg: \"%s, errnum: %d\"\n", m, err);
        fflush(stderr);
    }

    if(ehandler)
        ehandler(ehdata);

    exit(err);
}
