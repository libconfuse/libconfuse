/* Configuration file parser -*- tab-width: 4; -*-
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

#ifndef HAVE_STRCASECMP
# define strcasecmp strcmp
#endif

#include "confuse.h"

#if ENABLE_NLS
# include <libintl.h>
# include <locale.h>
# define _(str) dgettext(PACKAGE, str)
#else
# define _(str) str
#endif
#define N_(str) str

extern FILE *yyin;

const char *confuse_version = PACKAGE_VERSION;
const char *confuse_copyright = PACKAGE_STRING" by Martin Hedenfalk <mhe@home.se>";
const char *confuse_author = "Martin Hedenfalk <mhe@home.se>";

extern int yylex(cfg_t *cfg);
char * yylval;
extern int cfg_lexer_include(cfg_t *cfg, const char *fname);
extern void cfg_scan_string_begin(const char *buf);
extern void cfg_scan_string_end(void);
extern void cfg_create_buffer_from_fp(FILE *fp);
static int cfg_parse_internal(cfg_t *cfg, int level,
							  int force_state, cfgopt_t *force_opt);

#define STATE_CONTINUE 0
#define STATE_EOF -1
#define STATE_ERROR 1

#ifdef DEBUG
# define debug(fmt, args...) fprintf(stderr, fmt, ## args)
#else
# define debug(fmt, args...) 
#endif

cfgopt_t *cfg_getopt(cfg_t *cfg, const char *name)
{
	int i;

	assert(cfg && cfg->name && name);

	for(i = 0; cfg->opts[i].name; i++) {
		if(is_set(CFGF_NOCASE, cfg->flags)) {
			if(strcasecmp(cfg->opts[i].name, name) == 0) {
				return &cfg->opts[i];
			}
		} else {
			if(strcmp(cfg->opts[i].name, name) == 0) {
				return &cfg->opts[i];
			}
		}
	}
	cfg_error(cfg, _("no such option '%s'"), name);
	return 0;
}

const char *cfg_title(cfg_t *cfg)
{
	return cfg->title;
}

unsigned int cfg_size(cfg_t *cfg, const char *name)
{
	cfgopt_t *opt = cfg_getopt(cfg, name);
	if(opt)
		return opt->nvalues;
	return 0;
}

signed long cfg_getnint(cfg_t *cfg, const char *name, unsigned int index)
{
	cfgopt_t *opt = cfg_getopt(cfg, name);
	if(opt) {
		assert(opt->type == CFGT_INT);
		assert(index < opt->nvalues);
		return opt->values[index]->number;
	} else
		return 0;
}

signed long cfg_getint(cfg_t *cfg, const char *name)
{
	return cfg_getnint(cfg, name, 0);
}

double cfg_getnfloat(cfg_t *cfg, const char *name, unsigned int index)
{
	cfgopt_t *opt = cfg_getopt(cfg, name);
	if(opt) {
		assert(opt->type == CFGT_FLOAT);
		assert(index < opt->nvalues);
		return opt->values[index]->fpnumber;
	} else
		return 0;
}

double cfg_getfloat(cfg_t *cfg, const char *name)
{
	return cfg_getnfloat(cfg, name, 0);
}

cfgbool_t cfg_getnbool(cfg_t *cfg, const char *name, unsigned int index)
{
	cfgopt_t *opt = cfg_getopt(cfg, name);
	if(opt) {
		assert(opt->type == CFGT_BOOL);
		assert(index < opt->nvalues);
		return opt->values[index]->boolean;
	} else
		return 0;
}

cfgbool_t cfg_getbool(cfg_t *cfg, const char *name)
{
	return cfg_getnbool(cfg, name, 0);
}

char *cfg_getnstr(cfg_t *cfg, const char *name, unsigned int index)
{
	cfgopt_t *opt = cfg_getopt(cfg, name);
	if(opt) {
		assert(opt->type == CFGT_STR);
		assert(index < opt->nvalues);
		return opt->values[index]->string;
	}
	return 0;
}

char *cfg_getstr(cfg_t *cfg, const char *name)
{
	return cfg_getnstr(cfg, name, 0);
}

cfg_t *cfg_getnsec(cfg_t *cfg, const char *name, unsigned int index)
{
	cfgopt_t *opt = cfg_getopt(cfg, name);
	if(opt) {
		assert(opt->type == CFGT_SEC);
		assert(opt->values);
		assert(index < opt->nvalues);
		return opt->values[index]->section;
	}
	return 0;
}

cfg_t *cfg_gettsec(cfg_t *cfg, const char *name, const char *title)
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

cfg_t *cfg_getsec(cfg_t *cfg, const char *name)
{
	return cfg_getnsec(cfg, name, 0);
}

static cfgval_t *cfg_addval(cfgopt_t *opt)
{
	opt->values = (cfgval_t **)realloc(opt->values,
									   (opt->nvalues+1) * sizeof(cfgval_t *));
	assert(opt->values);
	opt->values[opt->nvalues] = (cfgval_t *)malloc(sizeof(cfgval_t));
	memset(opt->values[opt->nvalues], 0, sizeof(cfgval_t));
	return opt->values[opt->nvalues++];
}

static cfgopt_t *cfg_dupopts(cfgopt_t *opts)
{
	int n;
	cfgopt_t *dupopts;

	for(n = 0; opts[n].name; n++)
		/* do nothing */ ;
	dupopts = (cfgopt_t *)malloc(++n * sizeof(cfgopt_t));
	memcpy(dupopts, opts, n * sizeof(cfgopt_t));
	return dupopts;
}

int cfg_parse_boolean(const char *s)
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

	debug("*** cfg_init_defaults(%s)\n", cfg->name);

	for(i = 0; cfg->opts[i].name; i++) {
		if(cfg->opts[i].type != CFGT_SEC) {
			if(cfg->opts[i].type == CFGT_STR && cfg->opts[i].def[0] == '\0') {
				/* special case where a string has a default as ""
				 * (the empty string, which will be parsed as EOF and
				 * the option will have no default), otherwise the
				 * default string must be written as "''" (an empty
				 * string string)
				 */
				cfg_setopt(cfg, &cfg->opts[i], "");
			} else {
				int xstate, ret;

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

				cfg_scan_string_begin(cfg->opts[i].def);
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
							cfg->opts[i].def, cfg->opts[i].name);
					fprintf(stderr, "Check your initialization macros and the"
							" libConfuse dcumentation\n");
					fprintf(stderr, "This is a programming error in the "
							"program using libConfuse, aborting...\n");
					abort();
				}
			}
			/* the default value should only be returned if no value
			 * is given in the configuration file, so we set the RESET
			 * flag here. When/If cfg_setopt() is called, the value(s)
			 * will be freed and the flag unset.
			 */
			cfg->opts[i].flags |= CFGF_RESET;
		}
	}
	debug("+++ end of cfg_init_defaults(%s)\n", cfg->name);
}

cfgval_t *cfg_setopt(cfg_t *cfg, cfgopt_t *opt, char *value)
{
	cfgval_t *val;
	int b;
	char *s;
	double f;
	long int i;
	char *endptr;

	assert(cfg && opt);

	if(opt->simple_value) {
		assert(opt->type != CFGT_SEC);
		val = (cfgval_t *)opt->simple_value;
	} else {
		if(is_set(CFGF_RESET, opt->flags)) {
			cfg_free_val(opt);
			opt->flags &= ~CFGF_RESET;
		}
		if(opt->nvalues == 0 || is_set(CFGF_MULTI, opt->flags) ||
		   is_set(CFGF_LIST, opt->flags))
			val = cfg_addval(opt);
		else
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
			if(val->string)
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
			val->boolean = (cfgbool_t)b;
			break;
		default:
			cfg_error(cfg, "internal error in cfg_setopt(%s, %s)",
					  opt->name, value);
			assert(0);
			break;
	}
	return val;
}

void cfg_free_val(cfgopt_t *opt)
{
	int i;

	assert(opt);
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

cfg_errfunc_t cfg_set_error_function(cfg_t *cfg, cfg_errfunc_t errfunc)
{
	cfg_errfunc_t old;

	assert(cfg);
	old = cfg->errfunc;
	cfg->errfunc = errfunc;
	return old;
}

void cfg_error(cfg_t *cfg, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	if(cfg->errfunc) {
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

static int call_function(cfg_t *cfg, cfgopt_t *opt, cfgopt_t *funcopt)
{
	int ret;
	const char **argv;
	int i;

	/* create a regular argv string vector and call
	 * the registered function
	 */
	argv = (const char **)malloc(funcopt->nvalues *
								 sizeof(char *));
	for(i = 0; i < funcopt->nvalues; i++)
		argv[i] = funcopt->values[i]->string;
	ret = (*opt->func)(cfg, opt, funcopt->nvalues, argv);
	cfg_free_val(funcopt);
	free(argv);
	return ret;
}

static int cfg_parse_internal(cfg_t *cfg, int level,
							  int force_state, cfgopt_t *force_opt)
{
	int state = 0;
	char *opttitle = 0;
	cfgopt_t *opt = 0;
	cfgval_t *val = 0;
	cfgopt_t funcopt = CFG_END();

	if(force_state != -1)
		state = force_state;
	if(force_opt)
		opt = force_opt;

	debug("*** cfg_parse_internal(%s)\n", cfg->name);

	while(1) {
		int tok = yylex(cfg);

		debug("token: %d (yylval: '%s'), state: %d, level: %d\n",
			  tok, yylval, state, level);

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
					cfg_error(cfg, _("unexpected token '%s'"), yylval);
					return STATE_ERROR;
				}
				opt = cfg_getopt(cfg, yylval);
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
				} else if(tok == '=') {
					/* set the (temporary) reset flag to clear the old
					 * values, since we obviously didn't want to append */
					opt->flags |= CFGF_RESET;
				} else {
					cfg_error(cfg, _("missing equal sign after option '%s'"),
							  opt->name);
					return STATE_ERROR;
				}
				if(is_set(CFGF_LIST, opt->flags))
					state = 3;
				else
					state = 2;
				break;
			case 2: /* expecting an option value */
				if(tok == '}' && is_set(CFGF_LIST, opt->flags)) {
					state = 0;
					break;
				}

				if(tok != CFGT_STR) {
					cfg_error(cfg, _("unexpected token '%s'"), yylval);
					return STATE_ERROR;
				}

				if(cfg_setopt(cfg, opt, yylval) == 0)
					return STATE_ERROR;
				if(is_set(CFGF_LIST, opt->flags))
					state = 4;
				else
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
					cfg_error(cfg, _("unexpected token '%s'"), yylval);
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
					opttitle = strdup(yylval);
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
					val->string = strdup(yylval);
					state = 9;
				} else {
					cfg_error(cfg, _("missing parameter for function '%s'"),
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
					cfg_error(cfg, _("missing comma for function '%s'"),
							  opt->name);
					return STATE_ERROR;
				}
				break;
			default:
				/* missing state, internal error, abort */
				assert(0);
		}
	}

	debug("+++ end of cfg_parse_internal(%s)\n", cfg->name);
	return STATE_EOF;
}

int cfg_parse(cfg_t *cfg, const char *filename)
{
	int ret;

	assert(cfg && filename);

	cfg->filename = cfg_tilde_expand(filename);
	cfg->line = 1;

	yyin = fopen(cfg->filename, "r");
	if(yyin == 0)
		return CFG_FILE_ERROR;

	cfg_create_buffer_from_fp(yyin);
	ret = cfg_parse_internal(cfg, 0, -1, 0);
	if(ret == STATE_ERROR)
		return CFG_PARSE_ERROR;
	return CFG_SUCCESS;
}

cfg_t *cfg_init(cfgopt_t *opts, cfg_flag_t flags)
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

char *cfg_tilde_expand(const char *filename)
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
			user = malloc(file - filename);
			strncpy(user, filename + 1, file - filename - 1);
			passwd = getpwnam(user);
			free(user);
		}

		if(passwd) {
			expanded = malloc(strlen(passwd->pw_dir) + strlen(file) + 1);
			strcpy(expanded, passwd->pw_dir);
			strcat(expanded, file);
		}
	}
#endif
	if(!expanded)
		expanded = strdup(filename);
	return expanded;
}

void cfg_free(cfg_t *cfg)
{
	int i;

	assert(cfg);

	for(i = 0; cfg->opts[i].name; ++i)
		cfg_free_val(&cfg->opts[i]);

	if(is_set(CFGF_ALLOCATED, cfg->flags)) {
		free(cfg->name);
		free(cfg->opts);
	} else
		free(cfg->filename);

	free(cfg);
}

int cfg_include(cfg_t *cfg, cfgopt_t *opt, int argc, const char **argv)
{
	if(argc != 1) {
		cfg_error(cfg, _("wrong number of arguments to cfg_include()"));
		return 1;
	}
	return cfg_lexer_include(cfg, argv[0]);
}

static void cfg_indent(FILE *fp, int level)
{
	level *= 2;
	while(level--)
		fputc(' ', fp);
}

static void cfg_output_val(FILE *fp, cfgopt_t *opt, cfgval_t *val)
{
	switch(opt->type) {
		case CFGT_INT:
			fprintf(fp, "%ld", val->number); break;
		case CFGT_FLOAT:
			fprintf(fp, "%f", val->fpnumber); break;
		case CFGT_BOOL:
			fprintf(fp, "%s", val->boolean ? "true" : "false"); break;
		case CFGT_STR:
			fprintf(fp, "\"%s\"", val->string); break;
		case CFGT_SEC:
		case CFGT_NONE:
		case CFGT_FUNC:
			break;
	}
}

static int cfg_output_fp_internal(cfg_t *cfg, FILE *fp, int level)
{
	int i;

	for(i = 0; cfg->opts[i].name; i++) {
		cfgopt_t *opt = &cfg->opts[i];

		if(opt->nvalues) {
			cfg_indent(fp, level);
			if(opt->type == CFGT_SEC) {
				int j;
				for(j = 0; j < opt->nvalues; j++) {
					int ret;
					cfg_t *sec = opt->values[j]->section;

					fprintf(fp, "%s ", opt->name);
					if(is_set(CFGF_TITLE, opt->flags))
						fprintf(fp, "%s ", sec->title);
					fprintf(fp, "{\n");
					ret = cfg_output_fp_internal(sec, fp, level + 1);
					fprintf(fp, "}\n");
				}
			} else {
				fprintf(fp, "%s = ", opt->name);
				if(is_set(CFGF_LIST, opt->flags)) {
					int j;
					fprintf(fp, "{");
					for(j = 0; j < opt->nvalues - 1; j++) {
						cfg_output_val(fp, opt, opt->values[j]);
						fprintf(fp, ", ");
					}
					cfg_output_val(fp, opt, opt->values[j]);
					fprintf(fp, "}");
				} else
					cfg_output_val(fp, opt, opt->values[0]);
				fprintf(fp, "\n");
			}
		} else if(opt->type != CFGT_SEC && opt->type != CFGT_FUNC) {
			cfg_indent(fp, level);
			fprintf(fp, "#%s =", opt->name);
			if(opt->def)
				fprintf(fp, " %s", opt->def);
			fprintf(fp, "\n");
		}
	}
	return CFG_SUCCESS;
}

int cfg_output_fp(cfg_t *cfg, FILE *fp)
{
	return cfg_output_fp_internal(cfg, fp, 0);
}

int cfg_output(cfg_t *cfg, const char *filename)
{
	int ret;
	FILE *fp;

	fp = fopen(filename, "w");
	if(fp == 0)
		return CFG_FILE_ERROR;

	ret = cfg_output_fp(cfg, fp);
	fclose(fp);
	return ret;
}

static cfgval_t *cfg_getval(cfgopt_t *opt, unsigned int index)
{
	cfgval_t *val = 0;

	if(opt->simple_value) {
		val = (cfgval_t *)opt->simple_value;
	} else {
		if(is_set(CFGF_RESET, opt->flags)) {
			cfg_free_val(opt);
			opt->flags &= ~CFGF_RESET;
		}
		if(index >= opt->nvalues)
			val = cfg_addval(opt);
		else
			val = opt->values[index];
	}
	return val;
}

void cfg_setnint(cfg_t *cfg, const char *name,
					 long int value, unsigned int index)
{
	cfgopt_t *opt = cfg_getopt(cfg, name);
	if(opt) {
		cfgval_t *val;
		assert(opt->type == CFGT_INT);
		val = cfg_getval(opt, index);
		val->number = value;
	}
}

void cfg_setint(cfg_t *cfg, const char *name, long int value)
{
	cfg_setnint(cfg, name, value, 0);
}

void cfg_setnfloat(cfg_t *cfg, const char *name,
				   double value, unsigned int index)
{
	cfgopt_t *opt = cfg_getopt(cfg, name);
	if(opt) {
		cfgval_t *val;
		assert(opt->type == CFGT_FLOAT);
		val = cfg_getval(opt, index);
		val->fpnumber = value;
	}
}

void cfg_setfloat(cfg_t *cfg, const char *name, double value)
{
	cfg_setnfloat(cfg, name, value, 0);
}

void cfg_setnbool(cfg_t *cfg, const char *name,
				  cfgbool_t value, unsigned int index)
{
	cfgopt_t *opt = cfg_getopt(cfg, name);
	if(opt) {
		cfgval_t *val;
		assert(opt->type == CFGT_BOOL);
		val = cfg_getval(opt, index);
		val->boolean = value;
	}
}

void cfg_setbool(cfg_t *cfg, const char *name, cfgbool_t value)
{
	cfg_setnbool(cfg, name, value, 0);
}

void cfg_setnstr(cfg_t *cfg, const char *name,
				 const char *value, unsigned int index)
{
	cfgopt_t *opt = cfg_getopt(cfg, name);
	if(opt) {
		cfgval_t *val;
		assert(opt->type == CFGT_STR);
		val = cfg_getval(opt, index);
		if(val->string)
			free(val->string);
		val->string = strdup(value);
	}
}

void cfg_setstr(cfg_t *cfg, const char *name, const char *value)
{
	return cfg_setnstr(cfg, name, value, 0);
}
