#include "check_confuse.h"
#include <stdio.h>
#include <string.h>

/*
 * Regression test for issue #150: a comment must be accepted anywhere
 * between two tokens -- inside a list and inline (C-style) -- not only
 * where an option name is expected.
 */
int main(void)
{
	cfg_opt_t opts[] = {
		CFG_STR_LIST("targets", 0, CFGF_NONE),
		CFG_STR("foo", "default", CFGF_NONE),
		CFG_END()
	};
	cfg_t *cfg;
	int rc;

	/* The exact example from the issue, with all three comment styles */
	cfg = cfg_init(opts, CFGF_NONE);
	fail_unless(cfg);
	rc = cfg_parse_buf(cfg,
			   "targets = {\n"
			   "    \"Life\", # hash comment\n"
			   "    \"Universe\",  // c++ comment\n"
			   "    \"Everything\" /* block comment */\n"
			   "}\n");
	fail_unless(rc == CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "targets") == 3);
	fail_unless(strcmp(cfg_getnstr(cfg, "targets", 0), "Life") == 0);
	fail_unless(strcmp(cfg_getnstr(cfg, "targets", 1), "Universe") == 0);
	fail_unless(strcmp(cfg_getnstr(cfg, "targets", 2), "Everything") == 0);
	cfg_free(cfg);

	/* Inline block comments around the '=' sign and before the value */
	cfg = cfg_init(opts, CFGF_NONE);
	fail_unless(cfg);
	rc = cfg_parse_buf(cfg, "foo /* here */ = /* and here */ \"bar\"\n");
	fail_unless(rc == CFG_SUCCESS);
	fail_unless(strcmp(cfg_getstr(cfg, "foo"), "bar") == 0);
	cfg_free(cfg);

	/* Comment right after the opening brace and between value and comma */
	cfg = cfg_init(opts, CFGF_NONE);
	fail_unless(cfg);
	rc = cfg_parse_buf(cfg,
			   "targets = { # leading\n"
			   "    \"a\" /* mid */, \"b\"\n"
			   "}\n");
	fail_unless(rc == CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "targets") == 2);
	cfg_free(cfg);

	/* Same list comments still parse with CFGF_COMMENTS enabled */
	cfg = cfg_init(opts, CFGF_COMMENTS);
	fail_unless(cfg);
	rc = cfg_parse_buf(cfg,
			   "targets = {\n"
			   "    \"a\", # c\n"
			   "    \"b\"\n"
			   "}\n");
	fail_unless(rc == CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "targets") == 2);
	cfg_free(cfg);

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
