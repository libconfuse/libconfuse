#include "check_confuse.h"
#include <stdio.h>
#include <string.h>

/*
 * CFGF_JSON_LISTS lets a list be written and printed with JSON-style
 * [ ] in addition to the native { }.  Without the flag the behavior is
 * unchanged: [ ] is not list syntax and cfg_print() emits { }.
 */

static cfg_opt_t opts[] = {
	CFG_STR_LIST("list", 0, CFGF_NONE),
	CFG_STR("scalar", 0, CFGF_NONE),
	CFG_END()
};

static cfg_t *parse(cfg_flag_t flags, const char *buf, int expect)
{
	cfg_t *cfg = cfg_init(opts, flags);

	fail_unless(cfg);
	fail_unless(cfg_parse_buf(cfg, buf) == expect);

	return cfg;
}

int main(void)
{
	cfg_t *cfg;
	FILE *fp;
	char buf[256];

	/* [ ] accepted with the flag -- quoted and spaced */
	cfg = parse(CFGF_JSON_LISTS, "list = [ \"a\", \"b\", \"c\" ]\n", CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "list") == 3);
	fail_unless(strcmp(cfg_getnstr(cfg, "list", 0), "a") == 0);
	fail_unless(strcmp(cfg_getnstr(cfg, "list", 2), "c") == 0);
	cfg_free(cfg);

	/* quoted values need no surrounding spaces */
	cfg = parse(CFGF_JSON_LISTS, "list = [\"a\",\"b\"]\n", CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "list") == 2);
	cfg_free(cfg);

	/* unquoted values, compact -- no surrounding spaces needed */
	cfg = parse(CFGF_JSON_LISTS, "list = [a,b,c]\n", CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "list") == 3);
	fail_unless(strcmp(cfg_getnstr(cfg, "list", 2), "c") == 0);
	cfg_free(cfg);

	/* a literal bracket can be a quoted list value */
	cfg = parse(CFGF_JSON_LISTS, "list = [ \"]\", \"[\" ]\n", CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "list") == 2);
	fail_unless(strcmp(cfg_getnstr(cfg, "list", 0), "]") == 0);
	cfg_free(cfg);

	/* brackets in scalar values are untouched, even with the flag on */
	cfg = parse(CFGF_JSON_LISTS, "scalar = [::1]:80\n", CFG_SUCCESS);
	fail_unless(strcmp(cfg_getstr(cfg, "scalar"), "[::1]:80") == 0);
	cfg_free(cfg);

	/* empty list */
	cfg = parse(CFGF_JSON_LISTS, "list = [ ]\n", CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "list") == 0);
	cfg_free(cfg);

	/* { } still works with the flag set */
	cfg = parse(CFGF_JSON_LISTS, "list = { \"a\", \"b\" }\n", CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "list") == 2);
	cfg_free(cfg);

	/* [ ] is not list syntax without the flag */
	cfg = parse(CFGF_NONE, "list = [ \"a\", \"b\" ]\n", CFG_PARSE_ERROR);
	cfg_free(cfg);

	/* cfg_print() emits [ ] with the flag, and the output round-trips */
	cfg = parse(CFGF_JSON_LISTS, "list = { \"x\", \"y\" }\n", CFG_SUCCESS);
	fp = tmpfile();
	fail_unless(fp);
	fail_unless(cfg_print(cfg, fp) == CFG_SUCCESS);
	cfg_free(cfg);

	rewind(fp);
	memset(buf, 0, sizeof(buf));
	fail_unless(fread(buf, 1, sizeof(buf) - 1, fp) > 0);
	fail_unless(strstr(buf, "list = [") != NULL);
	fail_unless(strchr(buf, '{') == NULL);

	rewind(fp);
	cfg = cfg_init(opts, CFGF_JSON_LISTS);
	fail_unless(cfg);
	fail_unless(cfg_parse_fp(cfg, fp) == CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "list") == 2);
	cfg_free(cfg);
	fail_unless(fclose(fp) == 0);

	/* cfg_print() still emits { } without the flag */
	cfg = parse(CFGF_NONE, "list = { \"x\", \"y\" }\n", CFG_SUCCESS);
	fp = tmpfile();
	fail_unless(fp);
	fail_unless(cfg_print(cfg, fp) == CFG_SUCCESS);
	cfg_free(cfg);
	rewind(fp);
	memset(buf, 0, sizeof(buf));
	fail_unless(fread(buf, 1, sizeof(buf) - 1, fp) > 0);
	fail_unless(strstr(buf, "list = {") != NULL);
	fail_unless(fclose(fp) == 0);

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
