#include "check_confuse.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Regression test: cfg_parse_buf() must preserve a caller-set
 * cfg->filename so file:line diagnostics point at the real source,
 * defaulting to "[buf]" only when the caller left it unset -- the same
 * contract cfg_parse_fp() honors with "FILE".
 */
int main(void)
{
	cfg_opt_t opts[] = {
		CFG_STR("foo", "default", CFGF_NONE),
		CFG_END()
	};
	cfg_t *cfg;

	/* No filename set -> defaults to "[buf]" */
	cfg = cfg_init(opts, CFGF_NONE);
	fail_unless(cfg);
	fail_unless(cfg_parse_buf(cfg, "foo = \"bar\"\n") == CFG_SUCCESS);
	fail_unless(cfg->filename && !strcmp(cfg->filename, "[buf]"));
	cfg_free(cfg);

	/* Caller-set filename is preserved through the parse */
	cfg = cfg_init(opts, CFGF_NONE);
	fail_unless(cfg);
	cfg->filename = strdup("/etc/finit.d/avahi.conf");
	fail_unless(cfg->filename);
	fail_unless(cfg_parse_buf(cfg, "foo = \"bar\"\n") == CFG_SUCCESS);
	fail_unless(!strcmp(cfg->filename, "/etc/finit.d/avahi.conf"));
	cfg_free(cfg);

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
