#include "check_confuse.h"
#include <stdio.h>
#include <string.h>

/*
 * A raw section (CFGT_RAWSEC) captures a chunk of confuse configuration
 * verbatim so its owner can parse it later (deferred).  The body is walked
 * with confuse's own mini discard parser
 */

/*
 * Verbatim capture, exercising every shape the mini discard parser walks.
 */
#define RAW_BODY \
	"\n" \
	"    host = \"1.2.3.4\"\n" \
	"    urls = {http://127.0.0.1:56666/}\n" \
	"    items += { extra }\n" \
	"    hook(\"do\", \"stuff\")\n" \
	"    note = \"a } b\"\n" \
	"    tls {\n" \
	"        verify = yes\n" \
	"    }\n" \
	"    route \"main\" {\n" \
	"        target = x\n" \
	"    }\n"

static void check_verbatim(void)
{
	cfg_opt_t opts[] = {
		CFG_RAWSEC("mod", CFGF_NONE),
		CFG_END()
	};
	cfg_t *cfg = cfg_init(opts, 0);

	fail_unless(cfg != NULL);
	fail_unless(cfg_parse_buf(cfg, "mod {" RAW_BODY "}") == CFG_SUCCESS);
	fail_unless(cfg_getraw(cfg_getsec(cfg, "mod")) != NULL);
	fail_unless(strcmp(cfg_getraw(cfg_getsec(cfg, "mod")), RAW_BODY) == 0);
	cfg_free(cfg);
}

/*
 * Deferred parse: capture a module's config, then parse the captured text
 * later with the module's own schema.
 */
static void check_deferred(void)
{
	cfg_opt_t outer[] = {
		CFG_RAWSEC("mod", CFGF_TITLE | CFGF_MULTI),
		CFG_END()
	};
	cfg_opt_t inner[] = {
		CFG_STR("host", "", CFGF_NONE),
		CFG_INT("port", 0, CFGF_NONE),
		CFG_END()
	};
	cfg_t *cfg, *sub;
	const char *raw;

	cfg = cfg_init(outer, 0);
	fail_unless(cfg_parse_buf(cfg,
		"mod \"db\" { host = \"1.2.3.4\" port = 5432 }") == CFG_SUCCESS);

	raw = cfg_getraw(cfg_gettsec(cfg, "mod", "db"));
	fail_unless(raw != NULL);

	sub = cfg_init(inner, 0);
	fail_unless(cfg_parse_buf(sub, raw) == CFG_SUCCESS);
	fail_unless(strcmp(cfg_getstr(sub, "host"), "1.2.3.4") == 0);
	fail_unless(cfg_getint(sub, "port") == 5432);
	cfg_free(sub);
	cfg_free(cfg);
}

/* Several titled raw sections. */
static void check_multi_titled(void)
{
	cfg_opt_t opts[] = {
		CFG_RAWSEC("block", CFGF_TITLE | CFGF_MULTI),
		CFG_END()
	};
	cfg_t *cfg = cfg_init(opts, 0);

	fail_unless(cfg_parse_buf(cfg,
		"block \"a\" { x = 1 } block \"b\" { y = 2 }") == CFG_SUCCESS);
	fail_unless(cfg_size(cfg, "block") == 2);
	fail_unless(strcmp(cfg_getraw(cfg_gettsec(cfg, "block", "a")), " x = 1 ") == 0);
	fail_unless(strcmp(cfg_getraw(cfg_gettsec(cfg, "block", "b")), " y = 2 ") == 0);
	cfg_free(cfg);
}

int main(void)
{
	check_verbatim();
	check_deferred();
	check_multi_titled();

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
