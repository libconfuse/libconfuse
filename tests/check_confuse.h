#ifndef _check_confuse_h_
#define _check_confuse_h_

#include "../src/confuse.h"
#include <stdlib.h>

#define fail_unless(test) \
    do { if(!(test)) { \
        fprintf(stderr, \
                "----------------------------------------------\n" \
                "%s:%d: test FAILED:\nFailed test: %s\n" \
                "----------------------------------------------\n", \
                __FILE__, __LINE__, #test); \
        exit(1); \
    } } while(0)

void run_single_tests(void);
void run_list_tests(void);
void run_dup_tests(void);
void run_validate_tests(void);
void run_func_tests(void);

#endif

