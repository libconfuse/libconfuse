#include <check.h>
#include "confuse.h"

cfg_t *cfg;
int numopts = 0;

void list_setup(void)
{
    cfg_opt_t subsec_opts[] = 
    {
        CFG_STR_LIST("subsubstring", 0, CFGF_NONE),
        CFG_INT_LIST("subsubinteger", 0, CFGF_NONE),
        CFG_FLOAT_LIST("subsubfloat", 0, CFGF_NONE),
        CFG_BOOL_LIST("subsubbool", 0, CFGF_NONE),
        CFG_END()
    };

    cfg_opt_t sec_opts[] =
    {
        CFG_STR_LIST("substring", "{subdefault1, subdefault2}", CFGF_NONE),
        CFG_INT_LIST("subinteger", "{17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 300}", CFGF_NONE), /* 14 values */
        CFG_FLOAT_LIST("subfloat", "{8.37}", CFGF_NONE),
        CFG_BOOL_LIST("subbool", "{true}", CFGF_NONE),
        CFG_SEC("subsection", subsec_opts, CFGF_MULTI | CFGF_TITLE),
        CFG_END()
    };

    cfg_opt_t opts[] = 
    {
        CFG_STR_LIST("string", "{default1, default2, default3}", CFGF_NONE),
        CFG_INT_LIST("integer", "{4711, 123456789}", CFGF_NONE),
        CFG_FLOAT_LIST("float", "{0.42}", CFGF_NONE),
        CFG_BOOL_LIST("bool", "{false, true, no, yes, off, on}", CFGF_NONE),
        CFG_SEC("section", sec_opts, CFGF_MULTI),
        CFG_END()
    };

    cfg = cfg_init(opts, 0);
    numopts = cfg_numopts(opts);
    fail_unless(numopts == 5, NULL);

    memset(opts, 0, sizeof(opts));
    memset(sec_opts, 0, sizeof(sec_opts));
    memset(subsec_opts, 0, sizeof(subsec_opts));
}

void list_teardown(void)
{
    cfg_free(cfg);
}

START_TEST(list_string_test)
{
    char *buf;

    fail_unless(cfg_size(cfg, "string") == 3, "size should be 3");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "string")) == 3, "size should be 3");
    fail_unless(strcmp(cfg_getnstr(cfg, "string", 0), "default1") == 0, "default string value");
    fail_unless(strcmp(cfg_getnstr(cfg, "string", 1), "default2") == 0, "default string value");
    fail_unless(strcmp(cfg_getnstr(cfg, "string", 2), "default3") == 0, "default string value");
    buf = "string = {\"manually\", 'set'}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_size(cfg, "string") == 2, "size should be 2");
    fail_unless(strcmp(cfg_getstr(cfg, "string"), "manually") == 0, "set string after parse_buf");
    fail_unless(strcmp(cfg_getnstr(cfg, "string", 1), "set") == 0, "set string after parse_buf");
    cfg_setstr(cfg, "string", "manually set");
    fail_unless(strcmp(cfg_getstr(cfg, "string"), "manually set") == 0, "manually set string value");
    cfg_setnstr(cfg, "string", "foobar", 1);
    fail_unless(strcmp(cfg_getnstr(cfg, "string", 1), "foobar") == 0, "manually set string value");

    cfg_addlist(cfg, "string", 3, "foo", "bar", "baz");
    fail_unless(cfg_size(cfg, "string") == 5, "size should be 5");
    fail_unless(strcmp(cfg_getnstr(cfg, "string", 3), "bar") == 0, NULL);

    cfg_setlist(cfg, "string", 3, "baz", "foo", "bar");
    fail_unless(cfg_size(cfg, "string") == 3, "size should be 3");
    fail_unless(strcmp(cfg_getnstr(cfg, "string", 0), "baz") == 0, NULL);

    buf = "string += {gaz, 'onk'}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_size(cfg, "string") == 5, "size should be 5");
    fail_unless(strcmp(cfg_getnstr(cfg, "string", 3), "gaz") == 0, NULL);
    fail_unless(strcmp(cfg_getnstr(cfg, "string", 4), "onk") == 0, NULL);
}
END_TEST

START_TEST(list_integer_test)
{
    char *buf;

    fail_unless(cfg_size(cfg, "integer") == 2, "size should be 2");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "integer")) == 2, "size should be 2");

    fail_unless(cfg_getint(cfg, "integer") == 4711, "default integer value");
    fail_unless(cfg_getnint(cfg, "integer", 1) == 123456789, "default integer value");
    buf = "integer = {-46}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_size(cfg, "integer") == 1, "size should be 1");
    fail_unless(cfg_getnint(cfg, "integer", 0) == -46, "set integer after parse_buf");
    cfg_setnint(cfg, "integer", 999999, 1);
    fail_unless(cfg_size(cfg, "integer") == 2, "size should be 2");
    fail_unless(cfg_getnint(cfg, "integer", 1) == 999999, "manually set integer value");

    cfg_addlist(cfg, "integer", 3, 11, 22, 33);
    fail_unless(cfg_size(cfg, "integer") == 5, "size should be 5");
    fail_unless(cfg_getnint(cfg, "integer", 3) == 22, NULL);

    cfg_setlist(cfg, "integer", 3, 11, 22, 33);
    fail_unless(cfg_size(cfg, "integer") == 3, "size should be 3");
    fail_unless(cfg_getnint(cfg, "integer", 0) == 11, NULL);

    buf = "integer += {-1234567890, 1234567890}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_size(cfg, "integer") == 5, "size should be 5");
    fail_unless(cfg_getnint(cfg, "integer", 3) == -1234567890, NULL);
    fail_unless(cfg_getnint(cfg, "integer", 4) == 1234567890, NULL);
}
END_TEST

START_TEST(list_float_test)
{
    char *buf;

    fail_unless(cfg_size(cfg, "float") == 1, "size should be 1");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "float")) == 1, "size should be 1");

    fail_unless(cfg_getfloat(cfg, "float") == 0.42, "default float value");
    fail_unless(cfg_getnfloat(cfg, "float", 0) == 0.42, "default float value");

    buf = "float = {-46.777, 0.1, 0.2, 0.17, 17.123}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "float")) == 5, "size should be 5");
    fail_unless(cfg_getnfloat(cfg, "float", 0) == -46.777, "set float after parse_buf");
    fail_unless(cfg_getnfloat(cfg, "float", 1) == 0.1, "set float after parse_buf");
    fail_unless(cfg_getnfloat(cfg, "float", 2) == 0.2, "set float after parse_buf");
    fail_unless(cfg_getnfloat(cfg, "float", 3) == 0.17, "set float after parse_buf");
    fail_unless(cfg_getnfloat(cfg, "float", 4) == 17.123, "set float after parse_buf");

    cfg_setnfloat(cfg, "float", 5.1234e2, 1);
    fail_unless(cfg_getnfloat(cfg, "float", 1) == 5.1234e2, "manually set float value");

    cfg_addlist(cfg, "float", 1, 11.2233);
    fail_unless(cfg_size(cfg, "float") == 6, "size should be 6");
    fail_unless(cfg_getnfloat(cfg, "float", 5) == 11.2233, NULL);

    cfg_setlist(cfg, "float", 2, .3, -18.17e-7);
    fail_unless(cfg_size(cfg, "float") == 2, "size should be 2");
    fail_unless(cfg_getnfloat(cfg, "float", 0) == 0.3, NULL);
    fail_unless(cfg_getnfloat(cfg, "float", 1) == -18.17e-7, NULL);

    buf = "float += {64.64, 1234.567890}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_size(cfg, "float") == 4, "size should be 4");
    fail_unless(cfg_getnfloat(cfg, "float", 2) == 64.64, NULL);
    fail_unless(cfg_getnfloat(cfg, "float", 3) == 1234.567890, NULL);
}
END_TEST

START_TEST(list_bool_test)
{
    char *buf;

    fail_unless(cfg_size(cfg, "bool") == 6, "size should be 6");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "bool")) == 6, "size should be 6");
    fail_unless(cfg_getnbool(cfg, "bool", 0) == cfg_false, "default bool value incorrect");
    fail_unless(cfg_getnbool(cfg, "bool", 1) == cfg_true, "default bool value incorrect");
    fail_unless(cfg_getnbool(cfg, "bool", 2) == cfg_false, "default bool value incorrect");
    fail_unless(cfg_getnbool(cfg, "bool", 3) == cfg_true, "default bool value incorrect");
    fail_unless(cfg_getnbool(cfg, "bool", 4) == cfg_false, "default bool value incorrect");
    fail_unless(cfg_getnbool(cfg, "bool", 5) == cfg_true, "default bool value incorrect");

    buf = "bool = {yes, yes, no, false, true}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_size(cfg, "bool") == 5, "size should be 5");
    fail_unless(cfg_getbool(cfg, "bool") == cfg_true, "set bool to yes after parse_buf");
    fail_unless(cfg_getnbool(cfg, "bool", 1) == cfg_true, "set bool to yes after parse_buf");
    fail_unless(cfg_getnbool(cfg, "bool", 2) == cfg_false, "set bool to no after parse_buf");
    fail_unless(cfg_getnbool(cfg, "bool", 3) == cfg_false, "set bool to false after parse_buf");
    fail_unless(cfg_getnbool(cfg, "bool", 4) == cfg_true, "set bool to true after parse_buf");

    cfg_setbool(cfg, "bool", cfg_false);
    fail_unless(cfg_getbool(cfg, "bool") == cfg_false, "manually set bool value to false");
    cfg_setnbool(cfg, "bool", cfg_false, 1);
    fail_unless(cfg_getnbool(cfg, "bool", 1) == cfg_false, "manually set bool value to false");

    cfg_addlist(cfg, "bool", 2, cfg_true, cfg_false);
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "bool")) == 7, "size should be 7");
    fail_unless(cfg_getnbool(cfg, "bool", 5) == cfg_true, NULL);

    cfg_setlist(cfg, "bool", 4, cfg_true, cfg_true, cfg_false, cfg_true);
    fail_unless(cfg_size(cfg, "bool") == 4, "size should be 4");
    fail_unless(cfg_getnbool(cfg, "bool", 0) == cfg_true, NULL);
    fail_unless(cfg_getnbool(cfg, "bool", 1) == cfg_true, NULL);
    fail_unless(cfg_getnbool(cfg, "bool", 2) == cfg_false, NULL);
    fail_unless(cfg_getnbool(cfg, "bool", 3) == cfg_true, NULL);

    buf = "bool += {false, false}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_size(cfg, "bool") == 6, "size should be 6");
    fail_unless(cfg_getnbool(cfg, "bool", 4) == cfg_false, NULL);
    fail_unless(cfg_getnbool(cfg, "bool", 5) == cfg_false, NULL);
}
END_TEST

START_TEST(list_section_test)
{
    char *buf;
    cfg_t *sec, *subsec;
    cfg_opt_t *opt;

    /* size should be 0 before any section has been parsed. Since the
     * CFGF_MULTI flag is set, there are no default sections.
     */
    fail_unless(cfg_size(cfg, "section") == 0, "size should be 0");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "section")) == 0, "size should be 0");
    fail_unless(cfg_size(cfg, "section|subsection") == 0, "size should be 0");
    fail_unless(cfg_opt_size(cfg_getopt(cfg, "section|subsection")) == 0, "size should be 0");

    buf = "section {}"; /* add a section with default values */
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_size(cfg, "section") == 1, "size should be 1");
    
    sec = cfg_getsec(cfg, "section");
    fail_unless(sec != 0, "unable to get section");
    fail_unless(strcmp(sec->name, "section") == 0, NULL);
    fail_unless(cfg_title(sec) == 0, "cfg_title should return NULL on section w/o CFGF_TITLE");

    opt = cfg_getopt(sec, "subsection");
    fail_unless(opt != 0, NULL);
    fail_unless(cfg_opt_size(opt) == 0, "size should be 0");
    fail_unless(cfg_size(sec, "subsection") == 0, "size should be 0");
    
    fail_unless(strcmp(cfg_getnstr(sec, "substring", 0), "subdefault1") == 0, "wrong default value in section");
    subsec = cfg_getsec(cfg, "section|subsection");
    fail_unless(subsec == 0, "subsection is not parsed yet");

    buf = "section { subsection 'foo' { subsubfloat = {1.2, 3.4, 5.6} } }";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_size(cfg, "section") == 2, "size should be 2");

    sec = cfg_getnsec(cfg, "section", 1);
    fail_unless(sec != 0, NULL);
    fail_unless(strcmp(cfg_title(cfg_getnsec(sec, "subsection", 0)), "foo") == 0, NULL);
    fail_unless(cfg_size(sec, "subinteger") == 14, NULL);

    subsec = cfg_getsec(sec, "subsection");
    fail_unless(cfg_size(subsec, "subsubfloat") == 3, NULL);
    fail_unless(cfg_getnfloat(subsec, "subsubfloat", 2) == 5.6, "wrong default value in subsubsection");
    fail_unless(cfg_getnstr(subsec, "subsubstring", 0) == 0, "wrong default value in subsubsection");

    sec = cfg_getnsec(cfg, "section", 0);
    fail_unless(sec != 0, NULL);
    fail_unless(cfg_size(sec, "subsection") == 0, NULL);
    buf = "subsection 'bar' {}";
    fail_unless(cfg_parse_buf(sec, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_size(sec, "subsection") == 1, NULL);
    subsec = cfg_getnsec(sec, "subsection", 0);
    fail_unless(subsec != 0, NULL);
    fail_unless(strcmp(cfg_title(subsec), "bar") == 0, NULL);
    fail_unless(cfg_getnfloat(subsec, "subsubfloat", 2) == 0, "wrong default value in subsubsection");

    buf = "subsection 'baz' {}";
    fail_unless(cfg_parse_buf(sec, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_gettsec(sec, "subsection", "bar") == subsec, NULL);
    opt = cfg_getopt(sec, "subsection");
    fail_unless(opt != 0, NULL);
    fail_unless(cfg_gettsec(sec, "subsection", "baz") == cfg_opt_gettsec(opt, "baz"), NULL);
    fail_unless(cfg_opt_gettsec(opt, "bar") == subsec, NULL);
    fail_unless(cfg_opt_gettsec(opt, "foo") == 0, NULL);
    fail_unless(cfg_gettsec(sec, "subsection", "section") == 0, NULL);

    fail_unless(cfg_gettsec(cfg, "section", "baz") == 0, 0);
}
END_TEST

START_TEST(parse_buf_test)
{
    char *buf;

    fail_unless(cfg_parse_buf(cfg, 0) == CFG_SUCCESS, "parse_buf with NULL buf should be ok");
    fail_unless(cfg_parse_buf(cfg, "") == CFG_SUCCESS, "parse_buf with empty buf should be ok");

    buf = "bool = {true, true, false, wrong}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_PARSE_ERROR, "parse_buf should return CFG_PARSE_ERROR");
    buf = "string = {foo, bar";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_PARSE_ERROR, "parse_buf should return CFG_PARSE_ERROR");

    buf = "/* this is a comment */ bool = {true} /*// another comment */";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "C-style comments not handled correctly");

    buf = "/////// this is a comment\nbool = {true} // another /* comment */";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "C++-style comments not handled correctly");

    buf = "# this is a comment\nbool = {true} # another //* comment *//";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "Shell-style comments not handled correctly");
}
END_TEST

START_TEST(nonexistent_option_test)
{
    char *buf;

    fail_unless(cfg_numopts(cfg->opts) == numopts, NULL);
    fail_unless(cfg_getopt(cfg, "nonexistent") == 0, "nonexistent option shouldn't exist");

    buf = "section {}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, "unable to parse_buf");
    fail_unless(cfg_getopt(cfg, "section|subnonexistent") == 0, "subnonexistent option shouldn't exist");
}
END_TEST

Suite *list_suite(void) 
{ 
    Suite *s = suite_create("lists"); 

    TCase *tc_list = tcase_create("Lists");
    suite_add_tcase(s, tc_list);
    tcase_add_checked_fixture(tc_list, list_setup, list_teardown);
    tcase_add_test(tc_list, list_string_test); 
    tcase_add_test(tc_list, list_integer_test); 
    tcase_add_test(tc_list, list_float_test); 
    tcase_add_test(tc_list, list_bool_test); 
    tcase_add_test(tc_list, list_section_test); 
    tcase_add_test(tc_list, parse_buf_test); 
    tcase_add_test(tc_list, nonexistent_option_test);

    return s; 
}

