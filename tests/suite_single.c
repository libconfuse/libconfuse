#include <check.h>
#include "confuse.h"

cfg_t *cfg;

void single_setup(void)
{
    cfg_opt_t subsec_opts[] = 
    {
        CFG_STR("subsubstring", "subsubdefault", CFGF_NONE),
        CFG_INT("subsubinteger", -42, CFGF_NONE),
        CFG_FLOAT("subsubfloat", 19923.1234, CFGF_NONE),
        CFG_BOOL("subsubbool", cfg_false, CFGF_NONE),
        CFG_END()
    };

    cfg_opt_t sec_opts[] =
    {
        CFG_STR("substring", "subdefault", CFGF_NONE),
        CFG_INT("subinteger", 17, CFGF_NONE),
        CFG_FLOAT("subfloat", 8.37, CFGF_NONE),
        CFG_BOOL("subbool", cfg_true, CFGF_NONE),
        CFG_SEC("subsection", subsec_opts, CFGF_NONE),
        CFG_END()
    };

    cfg_opt_t opts[] = 
    {
        CFG_STR("string", "default", CFGF_NONE),
        CFG_INT("integer", 4711, CFGF_NONE),
        CFG_FLOAT("float", 0.42, CFGF_NONE),
        CFG_BOOL("bool", cfg_false, CFGF_NONE),
        CFG_SEC("section", sec_opts, CFGF_NONE),
        CFG_END()
    };

    cfg = cfg_init(opts, 0);

    memset(opts, 0, sizeof(opts));
    memset(sec_opts, 0, sizeof(sec_opts));
    memset(subsec_opts, 0, sizeof(subsec_opts));
}

void single_teardown(void)
{
    cfg_free(cfg);
}

START_TEST(single_string_test)
{
    char *buf;

    fail_unless(cfg_size(cfg, "string") == 1, "size of single option should be 1");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "string")) == 1, "size of single option should be 1");
    fail_unless(strcmp(cfg_getstr(cfg, "string"), "default") == 0, "default string value");
    buf = "string = 'set'";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(strcmp(cfg_getstr(cfg, "string"), "set") == 0, "set string after parse_buf");
    cfg_setstr(cfg, "string", "manually set");
    fail_unless(strcmp(cfg_getstr(cfg, "string"), "manually set") == 0, "manually set string value");
}
END_TEST

START_TEST(single_integer_test)
{
    char *buf;

    fail_unless(cfg_size(cfg, "integer") == 1, "size of single option should be 1");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "integer")) == 1, "size of single option should be 1");
    fail_unless(cfg_getint(cfg, "integer") == 4711, "default integer value");
    buf = "integer = -46";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_getint(cfg, "integer") == -46, "set integer after parse_buf");
    cfg_setint(cfg, "integer", 999999);
    fail_unless(cfg_getint(cfg, "integer") == 999999, "manually set integer value");
}
END_TEST

START_TEST(single_float_test)
{
    char *buf;

    fail_unless(cfg_size(cfg, "float") == 1, "size of single option should be 1");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "float")) == 1, "size of single option should be 1");
    fail_unless(cfg_getfloat(cfg, "float") == 0.42, "default float value");
    buf = "float = -46.777";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_getfloat(cfg, "float") == -46.777, "set float after parse_buf");
    cfg_setfloat(cfg, "float", 5.1234e2);
    fail_unless(cfg_getfloat(cfg, "float") == 5.1234e2, "manually set float value");
}
END_TEST

START_TEST(single_bool_test)
{
    char *buf;

    fail_unless(cfg_size(cfg, "bool") == 1, "size of single option should be 1");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "bool")) == 1, "size of single option should be 1");
    fail_unless(cfg_getbool(cfg, "bool") == cfg_false, "default bool value incorrect");

    buf = "bool = yes";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_getbool(cfg, "bool") == cfg_true, "set bool to yes after parse_buf");
    buf = "bool = no";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_getbool(cfg, "bool") == cfg_false, "set bool to no after parse_buf");
    buf = "bool = true";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_getbool(cfg, "bool") == cfg_true, "set bool to true after parse_buf");
    buf = "bool = false";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_getbool(cfg, "bool") == cfg_false, "set bool to false after parse_buf");
    buf = "bool = on";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_getbool(cfg, "bool") == cfg_true, "set bool to on after parse_buf");
    buf = "bool = off";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_getbool(cfg, "bool") == cfg_false, "set bool to off after parse_buf");

    cfg_setbool(cfg, "bool", cfg_true);
    fail_unless(cfg_getbool(cfg, "bool") == cfg_true, "manually set bool value to true");
    cfg_setbool(cfg, "bool", cfg_false);
    fail_unless(cfg_getbool(cfg, "bool") == cfg_false, "manually set bool value to false");
}
END_TEST

START_TEST(single_section_test)
{
    char *buf;
    cfg_t *sec, *subsec;

    fail_unless(cfg_size(cfg, "section") == 1, "size of single section should be 1");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "section")) == 1, "size of single section should be 1");

    fail_unless(cfg_size(cfg, "section|subsection") == 1, "size of single section should be 1");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "section|subsection")) == 1, "size of single section should be 1");

    fail_unless(cfg_size(cfg, "section|subsection|subsubstring") == 1, "size of single option should be 1");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "section|subsection|subsubinteger")) == 1, "size of single option should be 1");

    fail_unless(cfg_title(cfg_getsec(cfg, "section")) == 0, "cfg_title should return NULL on section w/o CFGF_TITLE");
    fail_unless(cfg_title(cfg_getsec(cfg, "section|subsection")) == 0, "cfg_title should return NULL on section w/o CFGF_TITLE");
    
    fail_unless(strcmp(cfg_getstr(cfg, "section|substring"), "subdefault") == 0, "default substring value in static section");
    sec = cfg_getsec(cfg, "section|subsection");
    fail_unless(sec != 0, "unable to get static subsection with |-syntax");
    fail_unless(cfg_getint(sec, "subsubinteger") == -42, "wrong default value in subsubsection");

    sec = cfg_getsec(cfg, "section");
    fail_unless(sec != 0, "unable to get section with cfg_getsec");
    subsec = cfg_getsec(sec, "subsection");
    fail_unless(subsec != 0, "unable to get subsection with cfg_getsec");
    fail_unless(cfg_getfloat(subsec, "subsubfloat") == 19923.1234, "invalid default float value in subsection");
    
    buf = "section { substring = \"foo\" subsection { subsubstring = \"bar\"} }"; 
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
}
END_TEST

START_TEST(parse_buf_test)
{
    char *buf;

    fail_unless(cfg_parse_buf(cfg, 0) == CFG_SUCCESS, "parse_buf with NULL buf should be ok");
    fail_unless(cfg_parse_buf(cfg, "") == CFG_SUCCESS, "parse_buf with empty buf should be ok");

    buf = "bool = wrong";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_PARSE_ERROR, "parse_buf should return CFG_PARSE_ERROR");
    buf = "string = ";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_PARSE_ERROR, "parse_buf should return CFG_PARSE_ERROR");
}
END_TEST

START_TEST(nonexistent_option_test)
{
    fail_unless(cfg_getopt(cfg, "nonexistent") == 0, "nonexistent option shouldn't exist");
    fail_unless(cfg_getopt(cfg, "section|subnonexistent") == 0, "subnonexistent option shouldn't exist");
}
END_TEST

Suite *single_suite(void) 
{ 
    Suite *s = suite_create("single"); 

    TCase *tc_single = tcase_create("Single");
    suite_add_tcase(s, tc_single);
    tcase_add_checked_fixture(tc_single, single_setup, single_teardown);
    tcase_add_test(tc_single, single_string_test); 
    tcase_add_test(tc_single, single_integer_test); 
    tcase_add_test(tc_single, single_float_test); 
    tcase_add_test(tc_single, single_bool_test); 
    tcase_add_test(tc_single, single_section_test); 
    tcase_add_test(tc_single, parse_buf_test); 
    tcase_add_test(tc_single, nonexistent_option_test);

    return s; 
}

