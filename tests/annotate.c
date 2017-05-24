#include "check_confuse.h"
#include <string.h>

int main(void)
{
	char *comment;
	char *expect = "A comment we think will go with this option";
	cfg_t *cfg;
	cfg_opt_t *opt;
	cfg_opt_t section_opts[] = {
		CFG_INT("key", 0, CFGF_NONE),
		CFG_BOOL("bool", 0, CFGF_NONE),
		CFG_STR("option", NULL, CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t opts[] = {
		CFG_STR("option", NULL, CFGF_NONE),
		CFG_SEC("section", section_opts, CFGF_MULTI),
		CFG_END()
	};

	cfg = cfg_init(opts, CFGF_COMMENT);
	fail_unless(cfg != NULL);

	fail_unless(cfg_parse(cfg, SRC_DIR "/annotate.conf") == CFG_SUCCESS);

	opt = cfg_getopt(cfg, "section|option");
	fail_unless(opt != NULL);
	comment = cfg_opt_getcomment(opt);
	fail_unless(comment != NULL);
	fail_unless(strcmp(comment, expect) == 0);

	fail_unless(cfg_opt_setcomment(opt, "But what's the worst poetry in the universe?") == CFG_SUCCESS);

	cfg_print(cfg, stdout);
	fail_unless(cfg_free(cfg) == CFG_SUCCESS);

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
