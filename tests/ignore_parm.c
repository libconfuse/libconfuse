/* Test handling of parameters that should be ignored.
*/

#include <string.h>
#include <stdlib.h>
#include "check_confuse.h"

cfg_opt_t section_opts[] = {
	CFG_STR("section_parameter", NULL, CFGF_NONE),
	CFG_END()
};

cfg_opt_t opts[] = {
	CFG_STR("parameter", NULL, CFGF_NONE),
	CFG_SEC("section", section_opts, CFGF_TITLE | CFGF_MULTI),
	CFG_END()
};

/* One expected value to verify in the parsed structure.
 * The path uses '.' to descend into sections
 * '[n]' to pick an index (section instance or list element);
 * value points to the expected value for the given type. */
struct check {
	cfg_type_t  type;
	const char *path;
	const void *value;
};

static cfg_opt_t *resolve(cfg_t *cfg, const char *path, unsigned int *index)
{
	char name[128];

	for (;;) {
		size_t len = strcspn(path, ".[");
		unsigned int idx = 0;

		if (len == 0 || len >= sizeof(name))
			return NULL;
		memcpy(name, path, len);
		name[len] = '\0';
		path += len;

		if (*path == '[') {
			idx = (unsigned int)atoi(path + 1);
			path = strchr(path, ']');
			if (!path)
				return NULL;
			path++;
		}

		if (*path == '.') {
			cfg = cfg_getnsec(cfg, name, idx);
			if (!cfg)
				return NULL;
			path++;
			continue;
		}

		if (*path != '\0')
			return NULL;

		*index = idx;
		return cfg_getopt(cfg, name);
	}
}

static int check_value(cfg_t *cfg, const struct check *c)
{
	unsigned int idx = 0;
	cfg_opt_t *opt = resolve(cfg, c->path, &idx);

	if (!opt)
		return 0;

	switch (c->type) {
	case CFGT_INT:
		return cfg_opt_getnint(opt, idx) == *(const long int *)c->value;
	case CFGT_FLOAT:
		return cfg_opt_getnfloat(opt, idx) == *(const double *)c->value;
	case CFGT_BOOL:
		return cfg_opt_getnbool(opt, idx) == *(const cfg_bool_t *)c->value;
	case CFGT_STR: {
		const char *s = cfg_opt_getnstr(opt, idx);

		return s && strcmp(s, (const char *)c->value) == 0;
	}
	default:
		return 0;
	}
}

static int testconfig(const char *buf, const struct check *checks)
{
	cfg_t *cfg = cfg_init(opts, CFGF_IGNORE_UNKNOWN);

	if (!cfg)
		return 0;

	if (cfg_parse_buf(cfg, buf) != CFG_SUCCESS) {
		cfg_free(cfg);
		return 0;
	}

	for (; checks && checks->path; checks++) {
		if (!check_value(cfg, checks)) {
			cfg_free(cfg);
			return 0;
		}
	}

	cfg_free(cfg);

	return 1;
}

int main(void)
{
	/* Sanity check cases that don't need to ignore parameters. */
	fail_unless(testconfig("parameter=5",
			       (struct check[]){ { CFGT_STR, "parameter", "5" }, { 0 } }));
	fail_unless(testconfig("parameter=5\n"
			       "section mysection {\n"
			       "\tsection_parameter=1\n"
			       "}",
			       (struct check[]){ { CFGT_STR, "parameter", "5" },
						 { CFGT_STR, "section[0].section_parameter", "1" },
						 { 0 } }));

	/* Ignore each type (no lists) */
	fail_unless(testconfig("unknown_int=1", NULL));
	fail_unless(testconfig("unknown_string=\"hello\"", NULL));
	fail_unless(testconfig("unknown_float=1.1", NULL));
	fail_unless(testconfig("unknown_bool=true", NULL));

	/* Ignore multiple parameters */
	fail_unless(testconfig("unknown_one=1\n"
			       "unknown_two=2\n"
			       "unknown_three=3\n", NULL));

	/* Test ignore parameters in a section */
	fail_unless(testconfig("section mysection {\n"
			       "\tunknown_section_parameter=1\n"
			       "}", NULL));

	/* Ignore unknown section, with unknown parameters.*/
	fail_unless(testconfig("parameter=5\n"
			       "unknown section {\n"
			       "\tunknown_param=1\n"
			       "\tunknown_list={1,2,3,4}\n"
			       "}\n"
			       "section hej { section_parameter = \"gnejs\" }\n"
			       "parameter = \"ormbunke\"",
			       (struct check[]){ { CFGT_STR, "parameter", "ormbunke" },
						 { CFGT_STR, "section[0].section_parameter", "gnejs" },
						 { 0 } }));

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
