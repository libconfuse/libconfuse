#include "check_confuse.h"
#include <stdint.h>

/*
 * Issue #11: fixed-width and unsigned integer types.  Exercises parsing
 * (all radixes), 64-bit values that overflow a 32-bit long, unsigned
 * range/negative rejection, the API setters, lists, simple-value binding,
 * defaults, and a print -> reparse round-trip.
 */

static int64_t  si;
static uint32_t su;
static uint8_t  su8;

static cfg_opt_t opts[] = {
	CFG_INT8  ("i8",  0, CFGF_NONE),
	CFG_INT16 ("i16", 0, CFGF_NONE),
	CFG_INT32 ("i32", 0, CFGF_NONE),
	CFG_INT64 ("i64", 7, CFGF_NONE),
	CFG_UINT8 ("u8",  0, CFGF_NONE),
	CFG_UINT16("u16", 0, CFGF_NONE),
	CFG_UINT32("u32", 0, CFGF_NONE),
	CFG_UINT64("u64", 0, CFGF_NONE),
	CFG_INT64_LIST("list", 0, CFGF_NONE),
	CFG_SIMPLE_INT64 ("si", &si),
	CFG_SIMPLE_UINT32("su", &su),
	CFG_SIMPLE_UINT8 ("su8", &su8),
	CFG_END()
};

int main(void)
{
	cfg_t *cfg, *cfg2;
	FILE *fp;

	/* Defaults */
	cfg = cfg_init(opts, CFGF_NONE);
	fail_unless(cfg);
	fail_unless(cfg_getint64(cfg, "i64") == 7);
	fail_unless(cfg_getuint32(cfg, "u32") == 0);
	fail_unless(cfg_getuint64(cfg, "u64") == 0);

	/* Parse: a 64-bit value beyond 32 bits, UINT32_MAX, and every radix */
	fail_unless(cfg_parse_buf(cfg,
				  "i64 = 0x1FFFFFFFFF\n"          /* 137438953471 */
				  "u32 = 4294967295\n"            /* UINT32_MAX   */
				  "u64 = 0b1111\n"                /* 15           */
				  "list = {1, -2, 0x10, 010}\n"   /* 1,-2,16,8    */
				 ) == CFG_SUCCESS);
	fail_unless(cfg_getint64(cfg, "i64") == INT64_C(0x1FFFFFFFFF));
	fail_unless(cfg_getuint32(cfg, "u32") == UINT32_MAX);
	fail_unless(cfg_getuint64(cfg, "u64") == 15);
	fail_unless(cfg_size(cfg, "list") == 4);
	fail_unless(cfg_getnint64(cfg, "list", 0) == 1);
	fail_unless(cfg_getnint64(cfg, "list", 1) == -2);
	fail_unless(cfg_getnint64(cfg, "list", 2) == 16);
	fail_unless(cfg_getnint64(cfg, "list", 3) == 8);

	/* API setters cover the full width */
	fail_unless(cfg_setint64(cfg, "i64", INT64_MIN) == CFG_SUCCESS);
	fail_unless(cfg_getint64(cfg, "i64") == INT64_MIN);
	fail_unless(cfg_setuint64(cfg, "u64", UINT64_MAX) == CFG_SUCCESS);
	fail_unless(cfg_getuint64(cfg, "u64") == UINT64_MAX);
	fail_unless(cfg_addlist(cfg, "list", 1, INT64_C(5000000000)) == CFG_SUCCESS);
	fail_unless(cfg_getnint64(cfg, "list", 4) == INT64_C(5000000000));

	/* Narrow widths: boundary values parse and read back correctly */
	fail_unless(cfg_parse_buf(cfg,
				  "i8 = -128\ni16 = 32767\ni32 = -2147483648\n"
				  "u8 = 255\nu16 = 65535\n") == CFG_SUCCESS);
	fail_unless(cfg_getint8(cfg, "i8") == -128);
	fail_unless(cfg_getint16(cfg, "i16") == 32767);
	fail_unless(cfg_getint32(cfg, "i32") == INT32_MIN);
	fail_unless(cfg_getuint8(cfg, "u8") == 255);
	fail_unless(cfg_getuint16(cfg, "u16") == 65535);

	/* Simple-value binding, incl. a value below INT32_MIN and a
	 * narrow (1-byte) unsigned binding that must not overwrite past it */
	fail_unless(cfg_parse_buf(cfg, "si = -9000000000\nsu = 4000000000\nsu8 = 200\n") == CFG_SUCCESS);
	fail_unless(si == INT64_C(-9000000000));
	fail_unless(su == 4000000000U);
	fail_unless(su8 == 200);
	cfg_free(cfg);

	/* One-past-max, overflow, and negative-into-unsigned input is
	 * rejected for every width.  A fresh context each time since a
	 * failed parse may leave partial state. */
	{
		static const char *bad[] = {
			"i8 = 128\n",   "u8 = 256\n",
			"i16 = 32768\n", "u16 = 65536\n",
			"i32 = 2147483648\n", "u32 = 4294967296\n",
			"u32 = -1\n",   "u64 = 99999999999999999999999\n",
		};
		size_t k;

		for (k = 0; k < sizeof(bad) / sizeof(bad[0]); k++) {
			cfg = cfg_init(opts, CFGF_NONE);
			fail_unless(cfg_parse_buf(cfg, bad[k]) != CFG_SUCCESS);
			cfg_free(cfg);
		}
	}

	/* Print -> reparse round-trip keeps full-width values intact */
	cfg = cfg_init(opts, CFGF_NONE);
	fail_unless(cfg_parse_buf(cfg,
				  "i64 = 0x1FFFFFFFFF\n"
				  "u32 = 4294967295\n"
				  "u64 = 18446744073709551615\n") == CFG_SUCCESS);
	fp = tmpfile();
	fail_unless(fp);
	fail_unless(cfg_print(cfg, fp) == CFG_SUCCESS);
	rewind(fp);

	cfg2 = cfg_init(opts, CFGF_NONE);
	fail_unless(cfg_parse_fp(cfg2, fp) == CFG_SUCCESS);
	fail_unless(cfg_getint64(cfg2, "i64") == INT64_C(0x1FFFFFFFFF));
	fail_unless(cfg_getuint32(cfg2, "u32") == UINT32_MAX);
	fail_unless(cfg_getuint64(cfg2, "u64") == UINT64_MAX);
	cfg_free(cfg2);
	fclose(fp);
	cfg_free(cfg);

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
