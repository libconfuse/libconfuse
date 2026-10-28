#include "check_confuse.h"
#include <stdio.h>
#include <string.h>

static unsigned int error_line;

static void capture_error(cfg_t *cfg, const char *fmt, va_list ap)
{
	char message[128];

	vsnprintf(message, sizeof(message), fmt, ap);
	if (strstr(message, "unterminated string constant"))
		error_line = cfg->line;
}

static cfg_t *parse(const char *input, cfg_flag_t flags, int expected)
{
	cfg_opt_t section_opts[] = {
		CFG_STR("text", "", CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t opts[] = {
		CFG_STR("text", "", CFGF_NONE),
		CFG_STR_LIST("list", 0, CFGF_NONE),
		CFG_SEC("section", section_opts, CFGF_NONE),
		CFG_END()
	};
	cfg_t *cfg = cfg_init(opts, flags);

	fail_unless(cfg);
	error_line = 0;
	cfg_set_error_function(cfg, capture_error);
	fail_unless(cfg_parse_buf(cfg, input) == expected);

	return cfg;
}

int main(void)
{
	const char *multiline[] = {
		"text = \"first\nsecond\"",
		"text = 'first\nsecond'"
	};
	cfg_t *cfg;
	size_t i;

	for (i = 0; i < sizeof(multiline) / sizeof(multiline[0]); i++) {
		/* Multiline strings remain valid by default. */
		cfg = parse(multiline[i], CFGF_NONE, CFG_SUCCESS);
		fail_unless(strcmp(cfg_getstr(cfg, "text"), "first\nsecond") == 0);
		cfg_free(cfg);

		cfg = parse(multiline[i], CFGF_NO_MULTILINE, CFG_PARSE_ERROR);
		fail_unless(error_line == 1);
		cfg_free(cfg);
	}

	/* Diagnose the missing quote on its own line, before later input. */
	cfg = parse("\ntext = \"missing\nsection { text = value }",
		    CFGF_NO_MULTILINE, CFG_PARSE_ERROR);
	fail_unless(error_line == 2);
	cfg_free(cfg);

	/* The flag is inherited in sections and applies to lists too. */
	cfg = parse("section {\ntext = 'first\nsecond'\n}",
		    CFGF_NO_MULTILINE, CFG_PARSE_ERROR);
	fail_unless(error_line == 2);
	cfg_free(cfg);
	cfg = parse("list = { \"first\nsecond\" }", CFGF_NO_MULTILINE, CFG_PARSE_ERROR);
	fail_unless(error_line == 1);
	cfg_free(cfg);

	/* A substitution expression can contain physical newlines too. */
	cfg = parse("text = \"${UNSET:-first\nsecond}\"", CFGF_NO_MULTILINE, CFG_PARSE_ERROR);
	fail_unless(error_line == 1);
	cfg_free(cfg);

	/* Unquoted substitution expressions retain their existing behavior. */
	cfg = parse("text = ${LIBCONFUSE_SINGLELINE_TEST_UNSET:-first\nsecond}",
		    CFGF_NO_MULTILINE, CFG_SUCCESS);
	fail_unless(strcmp(cfg_getstr(cfg, "text"), "first\nsecond") == 0);
	cfg_free(cfg);

	/* Explicit continuations do not insert a newline in the value. */
	cfg = parse("text = \"first\\\nsecond\"", CFGF_NO_MULTILINE, CFG_SUCCESS);
	fail_unless(strcmp(cfg_getstr(cfg, "text"), "firstsecond") == 0);
	cfg_free(cfg);
	cfg = parse("text = 'first\\\nsecond'", CFGF_NO_MULTILINE, CFG_SUCCESS);
	fail_unless(strcmp(cfg_getstr(cfg, "text"), "firstsecond") == 0);
	cfg_free(cfg);

	/* Escape sequences keep their existing meaning. */
	cfg = parse("text = \"first\\nsecond\"", CFGF_NO_MULTILINE, CFG_SUCCESS);
	fail_unless(strcmp(cfg_getstr(cfg, "text"), "first\nsecond") == 0);
	cfg_free(cfg);
	cfg = parse("text = 'first\\nsecond'", CFGF_NO_MULTILINE, CFG_SUCCESS);
	fail_unless(strcmp(cfg_getstr(cfg, "text"), "first\\nsecond") == 0);
	cfg_free(cfg);

	/* Ordinary line breaks between values are unaffected. */
	cfg = parse("text = value\nlist = {\n'one',\n\"two\"\n}\n",
		    CFGF_NO_MULTILINE, CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "list") == 2);
	cfg_free(cfg);

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
