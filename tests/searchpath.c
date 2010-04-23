#include "check_confuse.h"
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

const char tmp[] = SRC_DIR "/" "spdir";

int
main(void)
{
    cfg_t *cfg;
    cfg_t *sec;

    cfg_opt_t sec_opts[] = 
      {
	CFG_FUNC("include", cfg_include),
        CFG_INT("val", 0, CFGF_NONE),
        CFG_END()
      };

    cfg_opt_t opts[] = 
      {
	CFG_FUNC("include", cfg_include),
	CFG_SEC("sec", sec_opts, CFGF_MULTI | CFGF_TITLE),
	CFG_END()
      };

    cfg = cfg_init(opts, 0);

    fail_unless(cfg_add_searchpath(cfg,tmp) == 0);
    fail_unless(cfg_parse(cfg,"spa.conf") == 0);

    fail_unless(cfg_size(cfg, "sec") == 3);

    sec = cfg_getnsec(cfg, "sec", 0);
    fail_unless(sec != 0);
    fail_unless(strcmp(cfg_title(sec), "acfg") == 0);
    fail_unless(cfg_getint(sec, "val") == 5);

    sec = cfg_getnsec(cfg, "sec", 1);
    fail_unless(sec != 0);
    fail_unless(strcmp(cfg_title(sec), "bcfg") == 0);
    fail_unless(cfg_getint(sec, "val") == 6);

    sec = cfg_getnsec(cfg, "sec", 2);
    fail_unless(sec != 0);
    fail_unless(strcmp(cfg_title(sec), "ccfg") == 0);
    fail_unless(cfg_getint(sec, "val") == 7);

    cfg_free(cfg);

    return 0;
}

