#include <string.h>
#include "confuse.h"

int main(void)
{
	cfg_bool_t verbose = cfg_false;
	char *server = strdup("gazonk");
	double delay = 1.356e-32;
	char *username = NULL;
	int debug = 1;

	cfg_opt_t opts[] = {
		CFG_SIMPLE_BOOL("verbose", &verbose),
		CFG_SIMPLE_STR("server", &server),
		CFG_SIMPLE_STR("user", &username),
		CFG_SIMPLE_INT("debug", &debug),
		CFG_SIMPLE_FLOAT("delay", &delay),
		CFG_END()
	};
	cfg_t *cfg;

	cfg = cfg_init(opts, 0);
	cfg_parse(cfg, "simple.conf");

	printf("verbose: %s\n", verbose ? "true" : "false");
	printf("server: %s\n", server);
	printf("username: %s\n", username);
	printf("debug: %d\n", debug);
	printf("delay: %G\n", delay);

	printf("setting username to 'foo'\n");
	/* using cfg_setstr here is not necessary at all, the equivalent
	 * code is:
	 *   free(username);
	 *   username = strdup("foo");
	 */
	cfg_setstr(cfg, "user", "foo");
	printf("username: %s\n", username);

	cfg_free(cfg);
	return 0;
}
