#include <check.h>
#include "confuse.h"

Suite *single_suite(void);
Suite *dup_suite(void);
Suite *list_suite(void);
Suite *validate_suite(void);
 
int main(void) 
{ 
    int nf; 
    SRunner *sr = srunner_create(single_suite()); 
    srunner_add_suite(sr, list_suite());
    srunner_add_suite(sr, dup_suite());
    srunner_add_suite(sr, validate_suite());
    srunner_run_all(sr, CK_VERBOSE); 
    nf = srunner_ntests_failed(sr); 
    srunner_free(sr); 
    return (nf == 0) ? 0 : 1; 
}

