#include <check.h>
#include "../src/confuse.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

cfg_t *cfg = 0;

#define ACTION_NONE 0
#define ACTION_RUN 1
#define ACTION_WALK 2
#define ACTION_CRAWL 3
#define ACTION_JUMP 4

int parse_action(cfg_t *cfg, cfg_opt_t *opt, const char *value, void *result)
{
    if(strcmp(value, "run") == 0)
        *(int *)result = ACTION_RUN;
    else if(strcmp(value, "walk") == 0)
        *(int *)result = ACTION_WALK;
    else if(strcmp(value, "crawl") == 0)
        *(int *)result = ACTION_CRAWL;
    else if(strcmp(value, "jump") == 0)
        *(int *)result = ACTION_JUMP;
    else
    {
        cfg_error(cfg, "Invalid action value '%s'", value);
        return -1;
    }
    return 0;
}

int validate_speed(cfg_t *cfg, cfg_opt_t *opt)
{
    int i;

    for(i = 0; i < cfg_opt_size(opt); i++)
    {
        if(cfg_opt_getnint(opt, i) <= 0)
        {
            cfg_error(cfg, "speed must be positive in section %s", cfg->name);
            return 1;
        }
    }
    return 0;
}

int validate_ip(cfg_t *cfg, cfg_opt_t *opt)
{
    int i;

    for(i = 0; i < cfg_opt_size(opt); i++)
    {
        struct in_addr addr;
        char *ip = cfg_opt_getnstr(opt, i);
        if(inet_aton(ip, &addr) == 0)
        {
            cfg_error(cfg, "invalid IP address %s in section %s", ip, cfg->name);
            return 1;
        }
    }
    return 0;
}

int validate_action(cfg_t *cfg, cfg_opt_t *opt)
{
    cfg_opt_t *name_opt;
    cfg_t *action_sec = cfg_opt_getnsec(opt, 0);

    fail_unless(action_sec != 0, "unable to get section in validating callback function");

    name_opt = cfg_getopt(action_sec, "name");

    fail_unless(name_opt != 0, "option name declared but not found");
    fail_unless(cfg_opt_size(name_opt) == 1, "wrong number of values for single option");

    if(cfg_opt_getnstr(name_opt, 0) == NULL)
    {
        cfg_error(cfg, "missing required option 'name' in section %s", opt->name);
        return 1;
    }
    return 0;
}

void validate_setup(void)
{
    cfg_opt_t multi_opts[] =
    {
//        CFG_INT_LIST("speeds", 0, CFGF_NONE),
        {"speeds",CFGT_INT,0,0,CFGF_LIST,0,{0,0,cfg_false,0,0},0,0,0,validate_speed,0},
        CFG_END()
    };

    cfg_opt_t action_opts[] =
    {
        CFG_INT("speed", 0, CFGF_NONE),
        CFG_STR("name", 0, CFGF_NONE),
        CFG_END()
    };

    cfg_opt_t opts[] = 
    {
        CFG_STR_LIST("ip-address", 0, CFGF_NONE),
        CFG_INT_CB("action", ACTION_NONE, CFGF_NONE, parse_action),
        CFG_SEC("options", action_opts, CFGF_NONE),
        CFG_SEC("multi_options", multi_opts, CFGF_MULTI),
        CFG_END()
    };

    cfg = cfg_init(opts, 0);

    cfg_set_validate_func(cfg, "ip-address", validate_ip);
    cfg_set_validate_func(cfg, "options", validate_action);
    cfg_set_validate_func(cfg, "options|speed", validate_speed);
/*    cfg_set_validate_func(cfg, "multi_options|speed", validate_speed);*/
}

void validate_teardown(void)
{
    cfg_free(cfg);
}

START_TEST(validate_test)
{
    char *buf;

    buf = "action = wlak";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_PARSE_ERROR, 0);

    buf = "action = walk";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, 0);

    buf = "action = run"
          " options { speed = 6 }";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_PARSE_ERROR, 0);

    buf = "action = jump"
          " options { speed = 2 name = 'Joe' }";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, 0);

    buf = "action = crawl"
          " options { speed = -2 name = 'Smith' }";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_PARSE_ERROR, 0);

    buf = "ip-address = { 0.0.0.0 , 1.2.3.4 , 192.168.0.254 , 10.0.0.255 , 20.30.40.256}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_PARSE_ERROR, 0);
    buf = "ip-address = { 0.0.0.0 , 1.2.3.4 , 192.168.0.254 , 10.0.0.255 , 20.30.40.250}";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, 0);
    buf = "ip-address = { 1.2.3. }";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_PARSE_ERROR, 0);

    buf = "action = run"
          " multi_options { speeds = {1, 2, 3, 4, 5} }";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_SUCCESS, 0);

    buf = "action = run"
          " multi_options { speeds = {1, 2, 3, 4, 0} }";
    fail_unless(cfg_parse_buf(cfg, buf) == CFG_PARSE_ERROR, 0);
}
END_TEST

Suite *validate_suite(void) 
{ 
    Suite *s = suite_create("validate"); 

    TCase *tc_validate = tcase_create("validate");
    suite_add_tcase(s, tc_validate);
    tcase_add_checked_fixture(tc_validate, validate_setup, validate_teardown);
    tcase_add_test(tc_validate, validate_test); 

    return s; 
}

