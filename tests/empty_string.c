#include "check_confuse.h"
#include <stdio.h>
#include <string.h>

#ifndef HAVE_FMEMOPEN
extern FILE *fmemopen(void *buf, size_t size, const char *type);
#endif

int main(void)
{
	cfg_opt_t opts[] = {
		CFG_STR("string", "hello", CFGF_NONE),
		CFG_END()
	};
	cfg_t *cfg;
	char buf[100]; /* should be enough */
	FILE *f;

	/*
	 * override the default with a config with an empty string
	 * and then generate a temporary config file with that
	 */
	cfg = cfg_init(opts, 0);
	fail_unless(cfg_parse_buf(cfg, "string = ''") == CFG_SUCCESS);
	fail_unless(strcmp(cfg_getstr(cfg, "string"), "") == 0);
	f = fmemopen(buf, sizeof(buf), "w+");
	fail_unless(f != NULL);
	fail_unless(cfg_print(cfg, f) == CFG_SUCCESS);
	cfg_free(cfg);

#if defined(__has_feature)
# if __has_feature(address_sanitizer) || __has_feature(memory_sanitizer)
	/* Skip check since fmemopen(2) is broken with sanitizers, see
	 *   https://github.com/google/sanitizers/issues/628
	 */
# else
	/*
	 * try to reload the generated temporary config file to check
	 * that the default is indeed overridden by an empty string
	 */
	cfg = cfg_init(opts, 0);
	fail_unless(cfg != NULL);
	fail_unless(fseek(f, 0L, SEEK_SET) == 0);
	fail_unless(cfg_parse_fp(cfg, f) == CFG_SUCCESS);
	fail_unless(strcmp(cfg_getstr(cfg, "string"), "") == 0);
	cfg_free(cfg);
# endif
#endif

	fclose(f);

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
