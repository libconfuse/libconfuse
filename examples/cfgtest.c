#include "confuse.h"
#include <string.h>

/* function callback
 */
int cb_func(cfg_t *cfg, cfgopt_t *opt, int argc, const char **argv)
{
	int i;

	/* at least one parameter is required */
	if(argc == 0) {
		cfg_error(cfg, "Too few parameters for the '%s' function",
				  opt->name);
		return -1;
	}

	printf("cb_func() called with %d parameters:\n", argc);
	for(i = 0; i < argc; i++)
		printf("parameter %d: '%s'\n", i, argv[i]);
	return 0;
}

/* value parsing callback
 *
 * VALUE must be "yes", "no" or "maybe", and the corresponding results
 * are the integers 1, 2 and 3.
 */
int cb_verify_ask(cfg_t *cfg, cfgopt_t *opt, const char *value, void *result)
{
	if(strcmp(value, "yes") == 0)
		*(long int *)result = 1;
	else if(strcmp(value, "no") == 0)
		*(long int *)result = 2;
	else if(strcmp(value, "maybe") == 0)
		*(long int *)result = 3;
	else {
		cfg_error(cfg, "Invalid value for option %s: %s", opt->name, value);
		return -1;
	}
	return 0;
}

int main(void)
{
	cfgopt_t proxy_opts[] = {
		CFG_INT("type", "0", CFGF_NONE),
		CFG_STR("host", 0, CFGF_NONE),
		CFG_STR("exclude", "{localhost, .localnet}", CFGF_LIST),
		CFG_INT("port", 0, CFGF_NONE),
		CFG_END()
	};
	cfgopt_t bookmark_opts[] = {
		CFG_STR("machine", "", CFGF_NONE),
		CFG_INT("port", "21", CFGF_NONE),
		CFG_STR("login", "", CFGF_NONE),
		CFG_STR("password", "", CFGF_NONE),
		CFG_STR("directory", "", CFGF_NONE),
		CFG_BOOL("passive-mode", "false", CFGF_NONE),
		CFG_SEC("proxy", proxy_opts, CFGF_NONE),
		CFG_END()
	};
	cfgopt_t opts[] = {
		CFG_INT("backlog", "42", CFGF_NONE),
		CFG_STR("probe-device", "eth2", CFGF_NONE),
		CFG_SEC("bookmark", bookmark_opts, CFGF_MULTI | CFGF_TITLE),
		CFG_FLOAT("delays", "{3.567e2, .2}", CFGF_LIST),
		CFG_FUNC("func", &cb_func),
		CFG_INT_CB("ask-quit", "maybe", CFGF_NONE, &cb_verify_ask),
		CFG_END()
	};
	int i;
	cfg_t *cfg;
	unsigned n;
	int ret;
	cfgval_t x = {string: 3};

	printf("Using %s\n\n", confuse_copyright);

	cfg = cfg_init(opts, 0);
	cfg_output(cfg, "test.conf.default");
	ret = cfg_parse(cfg, "test.conf");
	if(ret == CFG_FILE_ERROR) {
		perror("./test.conf");
		return 1;
	} else if(ret == CFG_PARSE_ERROR) {
		fprintf(stderr, "parse error\n");
		return 2;
	}

	printf("backlog == %ld\n", cfg_getint(cfg, "backlog"));

	printf("probe device is %s\n", cfg_getstr(cfg, "probe-device"));
	cfg_setstr(cfg, "probe-device", "lo");
	printf("probe device is %s\n", cfg_getstr(cfg, "probe-device"));


	n = cfg_size(cfg, "bookmark");
	printf("%d configured bookmarks:\n", n);
	for(i = 0; i < n; i++) {
		cfg_t *bm = cfg_getnsec(cfg, "bookmark", i);
		printf("  bookmark #%u (%s):\n", i+1, cfg_title(bm));
		printf("    machine = %s\n", cfg_getstr(bm, "machine"));
		printf("    port = %d\n", (int)cfg_getint(bm, "port"));
		printf("    login = %s\n", cfg_getstr(bm, "login"));
		printf("    passive-mode = %s\n",
			   cfg_getbool(bm, "passive-mode") ? "true" : "false");
		printf("    directory = %s\n", cfg_getstr(bm, "directory"));
		printf("    password = %s\n", cfg_getstr(bm, "password"));

		if(cfg_size(bm, "proxy")) {
			int j, m;
			cfg_t *pxy = cfg_getsec(bm, "proxy");
			if(cfg_size(pxy, "host") == 0) {
				printf("    no proxy host is set, setting it to 'localhost'...\n");
				cfg_setstr(pxy, "host", "localhost");
			}
			printf("      proxy host is %s\n", cfg_getstr(pxy, "host"));
			printf("      proxy type is %ld\n", cfg_getint(pxy, "type"));
			
			/* we don't have a default for the proxy port, so we must check if
			 * there is a value for the port option
			 */
			if(cfg_size(pxy, "port") == 0)
				printf("      no proxy port is set\n");
			else
				printf("      proxy port is %ld\n", cfg_getint(pxy, "port"));

			m = cfg_size(pxy, "exclude");
			printf("      got %d hosts to exclude from proxying:\n", m);
			for(j = 0; j < m; j++) {
				printf("        exclude %s\n", cfg_getnstr(pxy, "exclude", j));
			}
		} else
			printf("    no proxy settings configured\n");
	}

	printf("delays are (%d):\n", cfg_size(cfg, "delays"));
	for(i = 0; i < cfg_size(cfg, "delays"); i++)
		printf(" %G\n", cfg_getnfloat(cfg, "delays", i));

	printf("ask-quit == %ld\n", cfg_getint(cfg, "ask-quit"));

	/* Using cfg_setint(), the integer value for the option ask-quit
	 * is not verified by the value parsing callback. Alternatively,
	 * you could use the commented out call instead (which uses the
	 * callback).
	 */
	cfg_setint(cfg, "ask-quit", 4);
/*	cfg_setopt(cfg, cfg_getopt(cfg, "ask-quit"), "yes");*/

	printf("ask-quit == %ld\n", cfg_getint(cfg, "ask-quit"));

	cfg_output(cfg, "test.conf.new");

	cfg_free(cfg);
	return 0;
}
