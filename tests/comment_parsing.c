#include "check_confuse.h"
#include <stdio.h>
#include <string.h>

static unsigned int error_line;

static void capture_error(cfg_t *cfg, const char *fmt, va_list ap)
{
	(void)fmt;
	(void)ap;
	error_line = cfg->line;
}

static void capture_include_error(cfg_t *cfg, const char *fmt, va_list ap)
{
	capture_error(cfg, fmt, ap);
	fail_unless(cfg->filename);
	fail_unless(strcmp(cfg->filename, SRC_DIR "/comment_lines.conf") == 0);
}

static void check_include_comment_lines(void)
{
	cfg_opt_t opts[] = {
		CFG_FUNC("include", &cfg_include),
		CFG_END()
	};
	cfg_flag_t flags[] = { CFGF_NONE, CFGF_COMMENTS };
	size_t i;

	for (i = 0; i < sizeof(flags) / sizeof(flags[0]); i++) {
		cfg_t *cfg = cfg_init(opts, flags[i]);

		fail_unless(cfg);
		cfg_set_error_function(cfg, capture_include_error);
		error_line = 0;
		fail_unless(cfg_parse_buf(cfg, "include(\"" SRC_DIR "/comment_lines.conf\")\n") == CFG_PARSE_ERROR);
		fail_unless(error_line == 3);
		cfg_free(cfg);
	}
}

static void check_comment_lines(void)
{
	const struct {
		const char *text;
		unsigned int line;
	} cases[] = {
		{ "# heading\nunknown = value", 2 },
		{ "// heading\nunknown = value", 2 },
		{ "###\n////\nunknown = value", 3 },
		{ "/* inline */ unknown = value", 1 },
		{ "/**/unknown = value", 1 },
		{ "/* first\n * second\n */ unknown = value", 3 },
		{ "/* first\n * second\n */\nunknown = value", 4 },
		{ "# first\r\n// second\r\nunknown = value", 3 },
		{ "# no final newline", 1 },
		{ "// no final newline", 1 },
		{ "/* no newline */", 1 },
		{ "/* two\nlines */", 2 },
	};
	cfg_opt_t opts[] = { CFG_END() };
	cfg_flag_t flags[] = { CFGF_NONE, CFGF_COMMENTS };
	size_t i, j;

	for (i = 0; i < sizeof(flags) / sizeof(flags[0]); i++) {
		for (j = 0; j < sizeof(cases) / sizeof(cases[0]); j++) {
			cfg_t *cfg = cfg_init(opts, flags[i]);
			int rc;

			fail_unless(cfg);
			cfg_set_error_function(cfg, capture_error);
			error_line = 0;
			rc = cfg_parse_buf(cfg, cases[j].text);
			if (strstr(cases[j].text, "unknown")) {
				fail_unless(rc == CFG_PARSE_ERROR);
				fail_unless(error_line == cases[j].line);
			} else {
				fail_unless(rc == CFG_SUCCESS);
				fail_unless(cfg->line == cases[j].line);
			}
			cfg_free(cfg);
		}
	}
}

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

	/*
	 * Regression test for issue #187: an empty comment -- no bytes
	 * after the introducer -- must not crash with CFGF_COMMENTS
	 * enabled.  The lexer returned CFGT_COMMENT with a NULL value, so
	 * cfg_parse_internal() then called strdup(NULL).
	 */
	{
		const char *empty[] = {
			"#", "//", "/**/", "/* */", "###", "////", NULL
		};
		int i;

		for (i = 0; empty[i]; i++) {
			cfg = cfg_init(opts, CFGF_COMMENTS);
			fail_unless(cfg);
			fail_unless(cfg_parse_buf(cfg, empty[i]) == CFG_SUCCESS);
			cfg_free(cfg);
		}
	}

	/* An empty comment after a completed value must also be fine */
	cfg = cfg_init(opts, CFGF_COMMENTS);
	fail_unless(cfg);
	fail_unless(cfg_parse_buf(cfg, "foo = \"bar\" #\n") == CFG_SUCCESS);
	fail_unless(strcmp(cfg_getstr(cfg, "foo"), "bar") == 0);
	cfg_free(cfg);

	check_include_comment_lines();
	check_comment_lines();

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
