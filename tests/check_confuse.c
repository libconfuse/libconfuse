#include "check_confuse.h"
 
void suppress_errors(cfg_t *cfg, const char *fmt, va_list ap)
{
}

int main(void) 
{ 
    run_single_tests();
    run_list_tests();
    run_dup_tests();
    run_validate_tests();
    run_func_tests();

    return 0;
}

