#include <check.h>
#include "../src/confuse.h"

static cfg_t *create_config(void)
{
    cfg_opt_t sec_opts[] = 
    {
        CFG_INT("a", 1, CFGF_NONE),
        CFG_INT("b", 2, CFGF_NONE),
        CFG_END()
    };

    cfg_opt_t opts[] = 
    {
        CFG_SEC("sec", sec_opts, CFGF_MULTI | CFGF_TITLE),
        CFG_END()
    };

    return cfg_init(opts, 0);
}

START_TEST(dup_test)
{
    cfg_t *acfg, *bcfg;
    cfg_t *sec;

    acfg = create_config();
    fail_unless(cfg_parse(acfg, "a.conf") == 0, "unable to parse a.conf");

    bcfg = create_config();
    fail_unless(cfg_parse(bcfg, "b.conf") == 0, "unable to parse b.conf");

    sec = cfg_getnsec(acfg, "sec", 0);
    fail_unless(sec != 0, "unable to get section sec from a.conf");
    fail_unless(cfg_size(acfg, "sec") == 1, "there should only be 1 sec in a.conf");
    fail_unless(strcmp(cfg_title(sec), "acfg") == 0, "wrong section title in a.conf");
    fail_unless(cfg_getint(sec, "a") == 5, "wrong value a in a.conf");
    fail_unless(cfg_getint(sec, "b") == 2, "wrong value b in a.conf");

    sec = cfg_getnsec(bcfg, "sec", 0);
    fail_unless(sec != 0, "unable to get section sec from b.conf");
    fail_unless(cfg_size(bcfg, "sec") == 1, "there should only be 1 sec in b.conf");
    fail_unless(strcmp(cfg_title(sec), "bcfg") == 0, "wrong section title in b.conf");
    fail_unless(cfg_getint(sec, "a") == 1, "wrong value a in b.conf");
    fail_unless(cfg_getint(sec, "b") == 9, "wrong value b in b.conf");

    cfg_free(acfg);
    cfg_free(bcfg);
}
END_TEST

Suite *dup_suite(void) 
{ 
    Suite *s = suite_create("dup"); 

    TCase *tc_dup = tcase_create("Duplicate schemas");
    suite_add_tcase(s, tc_dup);
    tcase_add_test(tc_dup, dup_test); 

    return s; 
}

