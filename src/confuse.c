/* Configuration file parser -*- tab-width: 4; -*-
 *
 * $Id$
 *
 * Copyright (c) 2002-2003, Martin Hedenfalk <mhe@home.se>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_STRING_H
# define _GNU_SOURCE    /* FIXME! */
# include <string.h>
#endif
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#include <assert.h>
#include <errno.h>
#ifndef _WIN32
# include <pwd.h>
#endif
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <ctype.h>

#include "confuse.h"

#define is_set(f, x) (((f) & (x)) == (f))

#if ENABLE_NLS
# include <libintl.h>
# include <locale.h>
# define _(str) dgettext(PACKAGE, str)
#else
# define _(str) str
#endif
#define N_(str) str

extern FILE *cfg_yyin;
extern int cfg_yylex(cfg_t *cfg);
extern int cfg_lexer_include(cfg_t *cfg, const char *fname);
extern void cfg_scan_string_begin(const char *buf);
extern void cfg_scan_string_end(void);
extern void cfg_scan_fp_begin(FILE *fp);
extern void cfg_scan_fp_end(void);
extern char *cfg_qstring;

char *cfg_yylval = 0;

const char confuse_version[] = PACKAGE_VERSION;
const char confuse_copyright[] = PACKAGE_STRING" by Martin Hedenfalk <mhe@home.se>";
const char confuse_author[] = "Martin Hedenfalk <mhe@home.se>";

static int cfg_parse_internal(cfg_t *cfg, int level,
							  int force_state, cfg_opt_t *force_opt);
static cfg_value_t *cfg_setopt(cfg_t *cfg, cfg_opt_t *opt, char *value);

#define STATE_CONTINUE 0
#define STATE_EOF -1
#define STATE_ERROR 1

#ifndef HAVE_STRDUP
# ifdef HAVE__STRDUP
#  define strdup _strdup
# else
static char *strdup(const char *s)
{
	char *r;

	if(s == 0 || *s == 0)
		return 0;

	r = (char *)malloc(strlen(s) + 1);
	strcpy(r, s);
	return r;
}
# endif
#endif

#ifndef HAVE_STRNDUP
static char *strndup(const char *s, size_t n)
{
	char *r;

	if(s == 0)
		return 0;

	r = (char *)malloc(n + 1);
	strncpy(r, s, n);
	r[n] = 0;
	return r;
}
#endif

#ifndef HAVE_STRCASECMP
/* Implementation from GNU glibc 2.3.1
 * Copyright (C) 1991,1992,1995,1996,1997,2001,2002
 *   Free Software Foundation, Inc.
 * License: GNU LGPL
 */
static int strcasecmp(const char *a, const char *b)
{
	int r;

	if(a == b)
		return 0;

	while((r = tolower(*a) - tolower(*b++)) == 0) {
		if(*a++ == '\0')
			break;
	}

	return r;
}
#endif

DLLIMPORT cfg_opt_t *cfg_getopt(cfg_t *cfg, const char *name)
{
	unsigned int i;
	cfg_t *sec = cfg;

	assert(cfg && cfg->name && name);

	while(name && *name) {
		char *secname;
		size_t len = strcspn(name, "|");
		if(name[len] == 0 /*len == strlen(name)*/)
			/* no more subsections */
			break;
		if(len) {
			secname = strndup(name, len);
			sec = cfg_getsec(sec, secname);
			if(sec == 0)
				cfg_error(cfg, _("no such option '%s'"), secname);
			free(secname);
			if(sec == 0)
				return 0;
		}
		name += len;
		name += strspn(name, "|");
	}

	for(i = 0; sec->opts[i].name; i++) {
		if(is_set(CFGF_NOCASE, sec->flags)) {
			if(strcasecmp(sec->opts[i].name, name) == 0) {
				return &sec->opts[i];
			}
		} else {
			if(strcmp(sec->opts[i].name, name) == 0) {
				return &sec->opts[i];
			}
		}
	}
	cfg_error(cfg, _("no such option '%s'"), name);
	return 0;
}

DLLIMPORT const char *cfg_title(cfg_t *cfg)
{
	return cfg->title;
}

DLLIMPORT unsigned int cfg_size(cfg_t *cfg, const char *name)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);
	if(opt)
		return opt->nvalues;
	return 0;
}

DLLIMPORT signed long cfg_getnint(cfg_t *cfg, const char *name,
								  unsigned int index)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);
	assert(opt && opt->type == CFGT_INT);
	if(opt->values && index < opt->nvalues)
		return opt->values[index]->number;
	else
		return 0;
}

DLLIMPORT signed long cfg_getint(cfg_t *cfg, const char *name)
{
	return cfg_getnint(cfg, name, 0);
}

DLLIMPORT double cfg_getnfloat(cfg_t *cfg, const char *name,
							   unsigned int index)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);
	assert(opt && opt->type == CFGT_FLOAT);
	if(opt->values && index < opt->nvalues)
		return opt->values[index]->fpnumber;
	else
		return 0;
}

DLLIMPORT double cfg_getfloat(cfg_t *cfg, const char *name)
{
	return cfg_getnfloat(cfg, name, 0);
}

DLLIMPORT cfg_bool_t cfg_getnbool(cfg_t *cfg, const char *name,
								  unsigned int index)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);
	assert(opt && opt->type == CFGT_BOOL);
	if(opt->values && index < opt->nvalues)
		return opt->values[index]->boolean;
	else
		return cfg_false;
}

DLLIMPORT cfg_bool_t cfg_getbool(cfg_t *cfg, const char *name)
{
	return cfg_getnbool(cfg, name, 0);
}

DLLIMPORT char *cfg_getnstr(cfg_t *cfg, const char *name, unsigned int index)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);
	assert(opt && opt->type == CFGT_STR);
	if(opt->values && index < opt->nvalues)
		return opt->values[index]->string;
	else
		return 0;
}

DLLIMPORT char *cfg_getstr(cfg_t *cfg, const char *name)
{
	return cfg_getnstr(cfg, name, 0);
}

DLLIMPORT cfg_t *cfg_getnsec(cfg_t *cfg, const char *name, unsigned int index)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);
	assert(opt && opt->type == CFGT_SEC);
	if(opt->values && index < opt->nvalues)
		return opt->values[index]->section;
	return 0;
}

DLLIMPORT cfg_t *cfg_gettsec(cfg_t *cfg, const char *name, const char *title)
{
	unsigned int i, n;

	n = cfg_size(cfg, name);
	for(i = 0; i < n; i++) {
		cfg_t *sec = cfg_getnsec(cfg, name, i);
		assert(sec && sec->title);
		if(is_set(CFGF_NOCASE, cfg->flags)) {
			if(strcasecmp(title, sec->title) == 0)
				return sec;
		} else {
			if(strcmp(title, sec->title) == 0)
				return sec;
		}
	}
	return 0;
}

DLLIMPORT cfg_t *cfg_getsec(cfg_t *cfg, const char *name)
{
	return cfg_getnsec(cfg, name, 0);
}

static cfg_value_t *cfg_addval(cfg_opt_t *opt)
{
	opt->values = (cfg_value_t **)realloc(opt->values,
								  (opt->nvalues+1) * sizeof(cfg_value_t *));
	assert(opt->values);
	opt->values[opt->nvalues] = (cfg_value_t *)malloc(sizeof(cfg_value_t));
	memset(opt->values[opt->nvalues], 0, sizeof(cfg_value_t));
	return opt->values[opt->nvalues++];
}

static cfg_opt_t *cfg_dupopts(cfg_opt_t *opts)
{
	int n;
	cfg_opt_t *dupopts;

	for(n = 0; opts[n].name; n++)
		/* do nothing */ ;
	dupopts = (cfg_opt_t *)malloc(++n * sizeof(cfg_opt_t));
	memcpy(dupopts, opts, n * sizeof(cfg_opt_t));
	return dupopts;
}

DLLIMPORT int cfg_parse_boolean(const char *s)
{
	if(strcasecmp(s, "true") == 0
	   || strcasecmp(s, "on") == 0
	   || strcasecmp(s, "yes") == 0)
		return 1;
	else if(strcasecmp(s, "false") == 0
			|| strcasecmp(s, "off") == 0
			|| strcasecmp(s, "no") == 0)
		return 0;
	return -1;
}

static void cfg_init_defaults(cfg_t *cfg)
{
	int i;

	for(i = 0; cfg->opts[i].name; i++) {
		cfg->opts[i].flags |= CFGF_DEFINIT;

		if(cfg->opts[i].type != CFGT_SEC) {

			if(is_set(CFGF_LIST, cfg->opts[i].flags) ||
			   cfg->opts[i].def.parsed)
			{
				int xstate, ret;

				/* If it's a list, but no default value was given,
				 * keep the option uninitialized.
				 */
				if(cfg->opts[i].def.parsed == 0 ||
				   cfg->opts[i].def.parsed[0] == 0)
					continue;

				/* setup scanning from the string specified for the
				 * "default" value, force the correct state and option
				 */

				if(cfg->opts[i].type == CFGT_FUNC)
					xstate = 0;
				else if(is_set(CFGF_LIST, cfg->opts[i].flags))
					/* lists must be surrounded by {braces} */
					xstate = 3;
				else
					xstate = 2;

				cfg_scan_string_begin(cfg->opts[i].def.parsed);
				do {
					ret = cfg_parse_internal(cfg, 1, xstate, &cfg->opts[i]);
					xstate = -1;
				} while(ret == STATE_CONTINUE);
				cfg_scan_string_end();
				if(ret == STATE_ERROR) {
					/*
					 * If there was an error parsing the default string,
					 * the initialization of the default value could be
					 * inconsistent or empty. What to do? It's a
					 * programming error and not an end user input
					 * error. Lets print a message and abort...
					 */
					fprintf(stderr, "Parse error in default value '%s'"
							" for option '%s'\n",
							cfg->opts[i].def.parsed, cfg->opts[i].name);
					fprintf(stderr, "Check your initialization macros and the"
							" libConfuse documentation\n");
					abort();
				}
			} else {
				switch(cfg->opts[i].type) {
					case CFGT_INT:
						cfg_opt_setnint(cfg, &cfg->opts[i],
										cfg->opts[i].def.number, 0);
						break;
					case CFGT_FLOAT:
						cfg_opt_setnfloat(cfg, &cfg->opts[i],
										  cfg->opts[i].def.fpnumber, 0);
						break;
					case CFGT_BOOL:
						cfg_opt_setnbool(cfg, &cfg->opts[i],
										 cfg->opts[i].def.boolean, 0);
						break;
					case CFGT_STR:
						cfg_opt_setnstr(cfg, &cfg->opts[i],
										cfg->opts[i].def.string, 0);
						break;
					case CFGT_FUNC:
						break;
					default:
						cfg_error(cfg,
								  "internal error in cfg_init_defaults(%s)",
								  cfg->opts[i].name);
						break;
				}
			}

			/* The default value should only be returned if no value
			 * is given in the configuration file, so we set the RESET
			 * flag here. When/If cfg_setopt() is called, the value(s)
			 * will be freed and the flag unset.
			 */
			cfg->opts[i].flags |= CFGF_RESET;
		} else if(!is_set(CFGF_MULTI, cfg->opts[i].flags)) {
			cfg_setopt(cfg, &cfg->opts[i], 0);
			cfg->opts[i].flags |= CFGF_RESET;
		}
	}
}

static cfg_value_t *cfg_setopt(cfg_t *cfg, cfg_opt_t *opt, char *value)
{
	cfg_value_t *val = 0;
	int b;
	char *s;
	double f;
	long int i;
	char *endptr;

	assert(cfg && opt);

	if(opt->simple_value) {
		assert(opt->type != CFGT_SEC);
		val = (cfg_value_t *)opt->simple_value;
	} else {
		if(is_set(CFGF_RESET, opt->flags)) {
			cfg_free_value(opt);
			opt->flags &= ~CFGF_RESET;
		}
		if(opt->nvalues == 0 || is_set(CFGF_MULTI, opt->flags) ||
		   is_set(CFGF_LIST, opt->flags))
		{
			val = 0;
			if(opt->type == CFGT_SEC && is_set(CFGF_TITLE, opt->flags)) {
				unsigned int i;

				/* check if there already is a section with the same title
				 */
				assert(value);
				for(i = 0; i < opt->nvalues; i++) {
					cfg_t *sec = opt->values[i]->section;
					if(is_set(CFGF_NOCASE, cfg->flags)) {
						if(strcasecmp(value, sec->title) == 0)
							val = opt->values[i];
					} else {
						if(strcmp(value, sec->title) == 0)
							val = opt->values[i];
					}
				}
			}
			if(val == 0)
				val = cfg_addval(opt);
		} else
			val = opt->values[0];
	}

	switch(opt->type) {
		case CFGT_INT:
			if(opt->cb) {
				if((*opt->cb)(cfg, opt, value, &i) != 0)
					return 0;
				val->number = i;
			} else {
				val->number = strtol(value, &endptr, 0);
				if(*endptr != '\0') {
					cfg_error(cfg, _("invalid integer value for option '%s'"),
							  opt->name);
					return 0;
				}
				if(errno == ERANGE) {
					cfg_error(cfg,
						  _("integer value for option '%s' is out of range"),
							  opt->name);
					return 0;
				}
			}
			break;
		case CFGT_FLOAT:
			if(opt->cb) {
				if((*opt->cb)(cfg, opt, value, &f) != 0)
					return 0;
				val->fpnumber = f;
			} else {
				val->fpnumber = strtod(value, &endptr);
				if(*endptr != '\0') {
					cfg_error(cfg,
						  _("invalid floating point value for option '%s'"),
							  opt->name);
					return 0;
				}
				if(errno == ERANGE) {
					cfg_error(cfg,
				  _("floating point value for option '%s' is out of range"),
							  opt->name);
					return 0;
				}
			}
			break;
		case CFGT_STR:
			free(val->string);
			if(opt->cb) {
				s = 0;
				if((*opt->cb)(cfg, opt, value, &s) != 0)
					return 0;
				val->string = strdup(s);
			} else
				val->string = strdup(value);
			break;
		case CFGT_SEC:
			cfg_free(val->section);
			val->section = (cfg_t *)malloc(sizeof(cfg_t));
			assert(val->section);
			memset(val->section, 0, sizeof(cfg_t));
			val->section->name = strdup(opt->name);
			val->section->opts = cfg_dupopts(opt->subopts);
			val->section->flags = cfg->flags;
			val->section->flags |= CFGF_ALLOCATED;
			val->section->filename = cfg->filename;
			val->section->line = cfg->line;
			val->section->errfunc = cfg->errfunc;
			val->section->title = value;
			if(!is_set(CFGF_DEFINIT, opt->flags))
				cfg_init_defaults(val->section);
			break;
		case CFGT_BOOL:
			if(opt->cb) {
				if((*opt->cb)(cfg, opt, value, &b) != 0)
					return 0;
			} else {
				b = cfg_parse_boolean(value);
				if(b == -1) {
					cfg_error(cfg, _("invalid boolean value for option '%s'"),
							  opt->name);
					return 0;
				}
			}
			val->boolean = (cfg_bool_t)b;
			break;
		default:
			cfg_error(cfg, "internal error in cfg_setopt(%s, %s)",
					  opt->name, value);
			assert(0);
			break;
	}
	return val;
}

DLLIMPORT void cfg_free_value(cfg_opt_t *opt)
{
	unsigned int i;

	if(opt == 0)
		return;

	for(i = 0; i < opt->nvalues; i++) {
		if(opt->type == CFGT_STR)
			free(opt->values[i]->string);
		else if(opt->type == CFGT_SEC)
			cfg_free(opt->values[i]->section);
		free(opt->values[i]);
	}
	free(opt->values);
	opt->values = 0;
	opt->nvalues = 0;
}

DLLIMPORT cfg_errfunc_t cfg_set_error_function(cfg_t *cfg,
											   cfg_errfunc_t errfunc)
{
	cfg_errfunc_t old;

	assert(cfg);
	old = cfg->errfunc;
	cfg->errfunc = errfunc;
	return old;
}

DLLIMPORT void cfg_error(cfg_t *cfg, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	if(cfg && cfg->errfunc) {
		(*cfg->errfunc)(cfg, fmt, ap);
	} else {
		if(cfg && cfg->filename && cfg->line)
			fprintf(stderr, "%s:%d: ", cfg->filename, cfg->line);
		else if(cfg && cfg->filename)
			fprintf(stderr, "%s: ", cfg->filename);
		vfprintf(stderr, fmt, ap);
		fprintf(stderr, "\n");
	}

	va_end(ap);
}

static int call_function(cfg_t *cfg, cfg_opt_t *opt, cfg_opt_t *funcopt)
{
	int ret;
	const char **argv;
	unsigned int i;

	/* create a regular argv string vector and call
	 * the registered function
	 */
	argv = (const char **)malloc(funcopt->nvalues *
								 sizeof(char *));
	for(i = 0; i < funcopt->nvalues; i++)
		argv[i] = funcopt->values[i]->string;
	ret = (*opt->func)(cfg, opt, funcopt->nvalues, argv);
	cfg_free_value(funcopt);
	free((char **)argv);
	return ret;
}

static int cfg_parse_internal(cfg_t *cfg, int level,
							  int force_state, cfg_opt_t *force_opt)
{
	int state = 0;
	char *opttitle = 0;
	cfg_opt_t *opt = 0;
	cfg_value_t *val = 0;
	cfg_opt_t funcopt = CFG_STR(0, 0, 0);
	int num_values = 0; /* number of values found for a list option */

	if(force_state != -1)
		state = force_state;
	if(force_opt)
		opt = force_opt;

	while(1) {
		int tok = cfg_yylex(cfg);

		if(tok == 0) {
			/* lexer.l should have called cfg_error */
			return STATE_ERROR;
		}

		if(tok == EOF) {
			if(state != 0) {
				cfg_error(cfg, _("premature end of file"));
				return STATE_ERROR;
			}
			return STATE_EOF;
		}

		switch(state) {
			case 0: /* expecting an option name */
				if(tok == '}') {
					if(level == 0) {
						cfg_error(cfg, _("unexpected closing brace"));
						return STATE_ERROR;
					}
					return STATE_EOF;
				}
				if(tok != CFGT_STR) {
					cfg_error(cfg, _("unexpected token '%s'"), cfg_yylval);
					return STATE_ERROR;
				}
				opt = cfg_getopt(cfg, cfg_yylval);
				if(opt == 0)
					return STATE_ERROR;
				if(opt->type == CFGT_SEC) {
					if(is_set(CFGF_TITLE, opt->flags))
						state = 6;
					else
						state = 5;
				} else if(opt->type == CFGT_FUNC) {
					state = 7;
				} else
					state = 1;
				break;
			case 1: /* expecting an equal sign or plus-equal sign */
				if(tok == '+') {
					if(!is_set(CFGF_LIST, opt->flags)) {
						cfg_error(cfg,
								  _("attempt to append to non-list option %s"),
								  opt->name);
						return STATE_ERROR;
					}
					/* Even if the reset flag was set by
					 * cfg_init_defaults, appending to the defaults
					 * should be ok.
					 */
					opt->flags &= ~CFGF_RESET;
				} else if(tok == '=') {
					/* set the (temporary) reset flag to clear the old
					 * values, since we obviously didn't want to append */
					opt->flags |= CFGF_RESET;
				} else {
					cfg_error(cfg, _("missing equal sign after option '%s'"),
							  opt->name);
					return STATE_ERROR;
				}
				if(is_set(CFGF_LIST, opt->flags)) {
					state = 3;
					num_values = 0;
				} else
					state = 2;
				break;
			case 2: /* expecting an option value */
				if(tok == '}' && is_set(CFGF_LIST, opt->flags)) {
					state = 0;
					if(num_values == 0 && is_set(CFGF_RESET, opt->flags))
						/* Reset flags was set, and the empty list was
						 * specified. Free all old values. */
						cfg_free_value(opt);
					break;
				}

				if(tok != CFGT_STR) {
					cfg_error(cfg, _("unexpected token '%s'"), cfg_yylval);
					return STATE_ERROR;
				}

				if(cfg_setopt(cfg, opt, cfg_yylval) == 0)
					return STATE_ERROR;
				if(is_set(CFGF_LIST, opt->flags)) {
					state = 4;
					++num_values;
				} else
					state = 0;
				break;
			case 3: /* expecting an opening brace for a list option */
				if(tok != '{') {
					cfg_error(cfg, _("missing opening brace for option '%s'"),
							  opt->name);
					return STATE_ERROR;
				}
				state = 2;
				break;
			case 4: /* expecting a separator for a list option, or
					 * closing (list) brace */
				if(tok == ',')
					state = 2;
				else if(tok == '}')
					state = 0;
				else {
					cfg_error(cfg, _("unexpected token '%s'"), cfg_yylval);
					return STATE_ERROR;
				}
				break;
			case 5: /* expecting an opening brace for a section */
				if(tok != '{') {
					cfg_error(cfg, _("missing opening brace for section '%s'"),
							  opt->name);
					return STATE_ERROR;
				}

				val = cfg_setopt(cfg, opt, opttitle);
				opttitle = 0;
				if(!val)
					return STATE_ERROR;

				if(cfg_parse_internal(val->section, level+1,-1,0) != STATE_EOF)
					return STATE_ERROR;
				cfg->line = val->section->line;
				state = 0;
				break;
			case 6: /* expecting a title for a section */
				if(tok != CFGT_STR) {
					cfg_error(cfg, _("missing title for section '%s'"),
							  opt->name);
					return STATE_ERROR;
				} else
					opttitle = strdup(cfg_yylval);
				state = 5;
				break;
			case 7: /* expecting an opening parenthesis for a function */
				if(tok != '(') {
					cfg_error(cfg, _("missing parenthesis for function '%s'"),
							  opt->name);
					return STATE_ERROR;
				}
				state = 8;
				break;
			case 8: /* expecting a function parameter or a closing paren */
				if(tok == ')') {
					int ret = call_function(cfg, opt, &funcopt);
					if(ret != 0)
						return STATE_ERROR;
					state = 0;
				} else if(tok == CFGT_STR) {
					val = cfg_addval(&funcopt);
					val->string = strdup(cfg_yylval);
					state = 9;
				} else {
					cfg_error(cfg, _("syntax error in call of function '%s'"),
							  opt->name);
					return STATE_ERROR;
				}
				break;
			case 9: /* expecting a comma in a function or a closing paren */
				if(tok == ')') {
					int ret = call_function(cfg, opt, &funcopt);
					if(ret != 0)
						return STATE_ERROR;
					state = 0;
				} else if(tok == ',') {
					state = 8;
				} else {
					cfg_error(cfg, _("syntax error in call of function '%s'"),
							  opt->name);
					return STATE_ERROR;
				}
				break;
			default:
				/* missing state, internal error, abort */
				assert(0);
		}
	}

	return STATE_EOF;
}

DLLIMPORT int cfg_parse_fp(cfg_t *cfg, FILE *fp)
{
	int ret;
	assert(cfg && fp);

	if(cfg->filename == 0)
		cfg->filename = strdup("FILE");
	cfg->line = 1;

	cfg_yyin = fp;
	cfg_scan_fp_begin(cfg_yyin);
	ret = cfg_parse_internal(cfg, 0, -1, 0);
	cfg_scan_fp_end();
	if(ret == STATE_ERROR)
		return CFG_PARSE_ERROR;
	return CFG_SUCCESS;
}

DLLIMPORT int cfg_parse(cfg_t *cfg, const char *filename)
{
	int ret;
	FILE *fp;

	assert(cfg && filename);

	free(cfg->filename);
	cfg->filename = cfg_tilde_expand(filename);
	fp = fopen(cfg->filename, "r");
	if(fp == 0)
		return CFG_FILE_ERROR;
	ret = cfg_parse_fp(cfg, fp);
	fclose(fp);
	return ret;
}

DLLIMPORT int cfg_parse_buf(cfg_t *cfg, const char *buf)
{
	int ret;

	free(cfg->filename);
	cfg->filename = strdup("[buf]");
	cfg->line = 1;

	cfg_scan_string_begin(buf);
	ret = cfg_parse_internal(cfg, 0, -1, 0);
	cfg_scan_string_end();
	if(ret == STATE_ERROR)
		return CFG_PARSE_ERROR;
	return CFG_SUCCESS;
}

DLLIMPORT cfg_t *cfg_init(cfg_opt_t *opts, cfg_flag_t flags)
{
	cfg_t *cfg;

	cfg = (cfg_t *)malloc(sizeof(cfg_t));
	assert(cfg);
	memset(cfg, 0, sizeof(cfg_t));

	cfg->name = "root";
	cfg->opts = opts;
	cfg->flags = flags;
	cfg->filename = 0;
	cfg->line = 0;
	cfg->errfunc = 0;

	cfg_init_defaults(cfg);

#if ENABLE_NLS
	setlocale(LC_MESSAGES, "");
	setlocale(LC_CTYPE, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
#endif

	return cfg;
}

DLLIMPORT char *cfg_tilde_expand(const char *filename)
{
	char *expanded = 0;

#ifndef _WIN32
	/* do tilde expansion
	 */
	if(filename[0] == '~') {
		struct passwd *passwd = 0;
		const char *file = 0;

		if(filename[1] == '/' || filename[1] == 0) {
			/* ~ or ~/path */
			passwd = getpwuid(geteuid());
			file = filename + 1;
		} else {
			/* ~user or ~user/path */
			char *user;

			file = strchr(filename, '/');
			if(file == 0)
				file = filename + strlen(filename);
			user = (char *)malloc(file - filename);
			strncpy(user, filename + 1, file - filename - 1);
			passwd = getpwnam(user);
			free(user);
		}

		if(passwd) {
			expanded = (char *)malloc(strlen(passwd->pw_dir) + strlen(file) + 1);
			strcpy(expanded, passwd->pw_dir);
			strcat(expanded, file);
		}
	}
#endif
	if(!expanded)
		expanded = strdup(filename);
	return expanded;
}

DLLIMPORT void cfg_free(cfg_t *cfg)
{
	int i;

	if(cfg == 0)
		return;

	for(i = 0; cfg->opts[i].name; ++i)
		cfg_free_value(&cfg->opts[i]);

	if(is_set(CFGF_ALLOCATED, cfg->flags)) {
		free(cfg->name);
		free(cfg->opts);
        free(cfg->title);
	} else
		free(cfg->filename);

	free(cfg);
}

DLLIMPORT int cfg_include(cfg_t *cfg, cfg_opt_t *opt, int argc,
						  const char **argv)
{
	if(argc != 1) {
		cfg_error(cfg, _("wrong number of arguments to cfg_include()"));
		return 1;
	}
	return cfg_lexer_include(cfg, argv[0]);
}

static cfg_value_t *cfg_getval(cfg_opt_t *opt, unsigned int index)
{
	cfg_value_t *val = 0;

	assert(index == 0 || is_set(CFGF_LIST, opt->flags));

	if(opt->simple_value) {
		val = (cfg_value_t *)opt->simple_value;
	} else {
		if(is_set(CFGF_RESET, opt->flags)) {
			cfg_free_value(opt);
			opt->flags &= ~CFGF_RESET;
		}
		if(index >= opt->nvalues) {
			val = cfg_addval(opt);
		} else
			val = opt->values[index];
	}
	return val;
}

DLLIMPORT void cfg_opt_setnint(cfg_t *cfg, cfg_opt_t *opt, long int value,
					 unsigned int index)
{
	cfg_value_t *val;
	assert(cfg && opt && opt->type == CFGT_INT);
	val = cfg_getval(opt, index);
	val->number = value;
}

DLLIMPORT void cfg_setnint(cfg_t *cfg, const char *name,
					 long int value, unsigned int index)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);
	if(opt)
		cfg_opt_setnint(cfg, opt, value, index);
}

DLLIMPORT void cfg_setint(cfg_t *cfg, const char *name, long int value)
{
	cfg_setnint(cfg, name, value, 0);
}

DLLIMPORT void cfg_opt_setnfloat(cfg_t *cfg, cfg_opt_t *opt, double value,
					   unsigned int index)
{
	cfg_value_t *val;
	assert(cfg && opt && opt->type == CFGT_FLOAT);
	val = cfg_getval(opt, index);
	val->fpnumber = value;
}

DLLIMPORT void cfg_setnfloat(cfg_t *cfg, const char *name,
				   double value, unsigned int index)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);
	if(opt)
		cfg_opt_setnfloat(cfg, opt, value, index);
}

DLLIMPORT void cfg_setfloat(cfg_t *cfg, const char *name, double value)
{
	cfg_setnfloat(cfg, name, value, 0);
}

DLLIMPORT void cfg_opt_setnbool(cfg_t *cfg, cfg_opt_t *opt, cfg_bool_t value,
					  unsigned int index)
{
	cfg_value_t *val;
	assert(cfg && opt && opt->type == CFGT_BOOL);
	val = cfg_getval(opt, index);
	val->boolean = value;
}

DLLIMPORT void cfg_setnbool(cfg_t *cfg, const char *name,
				  cfg_bool_t value, unsigned int index)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);
	if(opt)
		cfg_opt_setnbool(cfg, opt, value, index);
}

DLLIMPORT void cfg_setbool(cfg_t *cfg, const char *name, cfg_bool_t value)
{
	cfg_setnbool(cfg, name, value, 0);
}

DLLIMPORT void cfg_opt_setnstr(cfg_t *cfg, cfg_opt_t *opt, const char *value,
					 unsigned int index)
{
	cfg_value_t *val;
	assert(cfg && opt && opt->type == CFGT_STR);
	val = cfg_getval(opt, index);
	free(val->string);
	val->string = value ? strdup(value) : 0;
}

DLLIMPORT void cfg_setnstr(cfg_t *cfg, const char *name,
				 const char *value, unsigned int index)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);
	if(opt)
		cfg_opt_setnstr(cfg, opt, value, index);
}

DLLIMPORT void cfg_setstr(cfg_t *cfg, const char *name, const char *value)
{
	cfg_setnstr(cfg, name, value, 0);
}

static void cfg_addlist_internal(cfg_t *cfg, cfg_opt_t *opt,
								 unsigned int nvalues, va_list ap)
{
	unsigned int i;

	for(i = 0; i < nvalues; i++) {
		switch(opt->type) {
			case CFGT_INT:
				cfg_opt_setnint(cfg, opt, va_arg(ap, int), opt->nvalues);
				break;
			case CFGT_FLOAT:
				cfg_opt_setnfloat(cfg, opt, va_arg(ap, double),
								  opt->nvalues);
				break;
			case CFGT_BOOL:
				cfg_opt_setnbool(cfg, opt, va_arg(ap, cfg_bool_t),
								 opt->nvalues);
				break;
			case CFGT_STR:
				cfg_opt_setnstr(cfg, opt, va_arg(ap, char*), opt->nvalues);
				break;
			case CFGT_FUNC:
			case CFGT_SEC:
			default:
				break;
		}
	}
}

DLLIMPORT void cfg_setlist(cfg_t *cfg, const char *name,
						   unsigned int nvalues, ...)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);

	if(opt) {
		va_list ap;

		cfg_free_value(opt);
		va_start(ap, nvalues);
		cfg_addlist_internal(cfg, opt, nvalues, ap);
		va_end(ap);
	}
}

DLLIMPORT void cfg_addlist(cfg_t *cfg, const char *name,
						   unsigned int nvalues, ...)
{
	cfg_opt_t *opt = cfg_getopt(cfg, name);

	if(opt) {
		va_list ap;

		va_start(ap, nvalues);
		cfg_addlist_internal(cfg, opt, nvalues, ap);
		va_end(ap);
	}
}

