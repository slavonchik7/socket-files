


#ifndef TMSPECDEF_H
#define TMSPECDEF_H

#define MSEC_TO_SPEC(msec, spec) \
            do { \
                msec *= (msec > 0); \
                spec->tv_sec = (int)msec / 1000; \
                spec->tv_nsec = ((int)msec % 1000 + msec - (int)msec) * 1000000; \
            } while(0)

#endif /* TMSPECDEF_H */
