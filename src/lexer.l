%{
/* Configuration file parser
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

#include <assert.h>

#ifdef HAVE_STRING_H
# include <string.h>
#endif
#include "confuse.h"

#include <errno.h>

#if ENABLE_NLS
# include <libintl.h>
# define _(str) dgettext(PACKAGE, str)
#else
# define _(str) str
#endif
#define N_(str) str

typedef char * YYSTYPE;
extern YYSTYPE cfg_yylval;

#define YY_DECL int cfg_yylex YY_PROTO(( cfg_t *cfg ))

/* temporary buffer for the quoted strings scanner
 */
char *cfg_qstring = NULL;
static unsigned int qstring_index = 0;
static unsigned int qstring_len = 0;
static void qputc(char ch);
#define CFG_QSTRING_BUFSIZ 32

#define MAX_INCLUDE_DEPTH 10
struct {
	YY_BUFFER_STATE state;
	char *filename;
	unsigned int line;
} cfg_include_stack[MAX_INCLUDE_DEPTH];
int cfg_include_stack_ptr = 0;

%}

%option noyywrap

 /* start conditions
  */
%x comment
%x dq_str
%x sq_str
%x incl1
%x incl2
%x incl3

%%

[ \t]+    /* eat up whitespace */

[\n]	 cfg->line++; /* keep track of line number */

("#"|"//")[^\n]*     /* eat up one-line comments */

 /* special keywords/symbols
  */
"{"         { cfg_yylval = yytext; return '{'; }
"}"         { cfg_yylval = yytext; return '}'; }
"("         { cfg_yylval = yytext; return '('; }
")"         { cfg_yylval = yytext; return ')'; }
"="         { cfg_yylval = yytext; return '='; }
"+="        { cfg_yylval = yytext; return '+'; }
","         { cfg_yylval = yytext; return ','; }

 /* handle multi-line C-style comments
  */
"/*"         BEGIN(comment);
<comment>[^*\n]*        /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<comment>\n             cfg->line++;
<comment>"*"+"/"        BEGIN(INITIAL);

 /* handle C-style strings
  */
"\""    {
	qstring_index = 0;
	BEGIN(dq_str);
}
<dq_str>\"  { /* saw closing quote - all done */
	BEGIN(INITIAL);
	qputc('\0');
	cfg_yylval = cfg_qstring;
	return CFGT_STR;
 }
<dq_str>$\{[^}]*\} {
	char *var;
	char *e;
	yytext[strlen(yytext) - 1] = 0;
	e = strchr(yytext+2, ':');
	if(e && e[1] == '-')
		*e = 0;
	else
		e = 0;
	var = getenv(yytext+2);
	if(!var && e)
		var = e+2;
	while(var && *var)
		qputc(*var++);
}
<dq_str>\\\n { /* allow continuing on next line */
    /* no-op */
}
<dq_str>\\[0-7]{1,3} {
	/* octal escape sequence */
	int result;
	sscanf(yytext + 1, "%o", &result);
	if(result > 0xFF) {
		cfg_error(cfg, _("invalid octal number '%s'"), yytext);
		return 0;
	}
	qputc(result);
 }
<dq_str>\\[0-9]+   {
	cfg_error(cfg, _("bad escape sequence '%s'"), yytext);
	return 0;
 }
<dq_str>\\n|\\r|\\b|\\f  {
	qputc(yytext[0]);
}
<dq_str>\\a  {
	qputc('\007');
}
<dq_str>\\e  {
	qputc('\033');
}

<dq_str>\\(.|\n)  {
	qputc(yytext[1]);
}

<dq_str>[^\\\"]+  {
	char *yptr = yytext;
	while(*yptr) {
		qputc(*yptr++);
	}
}

    /* single-quoted string ('...') */
"\'" {
    qstring_index = 0;
    BEGIN(sq_str);
}
<sq_str>\' { /* saw closing quite - all done */
    BEGIN(INITIAL);
    qputc('\0');
    cfg_yylval = cfg_qstring;
    return CFGT_STR;
}
<sq_str>\\\n {
    /* no-op */
}
<sq_str>\\[\\\'] {
	qputc(yytext[1]);
}
<sq_str>\\[^\\\'] {
	qputc(yytext[0]);
	qputc(yytext[1]);
}
<sq_str>[^\\\']+ {
    char *cp = yytext;
    while(*cp != '\0') {
		qputc(*cp++);
    }
}
<sq_str>(.|\n) {
	qputc(yytext[1]);
}
<sq_str><<EOF>> {
    cfg_error(cfg, _("unterminated string constant"));
    return 0;
}

<<EOF>> {
             if ( --cfg_include_stack_ptr < 0 )
                 {
                 return EOF;
                 }

             else
                 {
                 yy_delete_buffer( YY_CURRENT_BUFFER );
                 yy_switch_to_buffer(
                      cfg_include_stack[cfg_include_stack_ptr].state );
                 free(cfg->filename);
                 cfg->filename = cfg_include_stack[cfg_include_stack_ptr].filename;
                 cfg->line = cfg_include_stack[cfg_include_stack_ptr].line;
                 }
}

$\{[^}]*\} {
	char *var;
	char *e;
	yytext[strlen(yytext) - 1] = 0;
	e = strchr(yytext+2, ':');
	if(e && e[1] == '-')
		*e = 0;
	else
		e = 0;
	var = getenv(yytext+2);
	if(!var && e)
		var = e+2;
	if(!var)
		var = "";
	cfg_yylval = var;
	return CFGT_STR;
}

 /* an unquoted string
  */
[^ \"\'\t\n={}()+,]+  cfg_yylval = yytext; return CFGT_STR;

%%

void cfg_dummy_function(void)
{
	/* please compiler :-)
	 * otherwise "defined but not used" warning
	 */
	yyunput(0, 0);
}

int cfg_lexer_include(cfg_t *cfg, const char *filename)
{
	char *xfilename;

	if(cfg_include_stack_ptr >= MAX_INCLUDE_DEPTH) {
		cfg_error(cfg, _("includes nested too deeply"));
		return 1;
	}

	cfg_include_stack[cfg_include_stack_ptr].state = YY_CURRENT_BUFFER;
	cfg_include_stack[cfg_include_stack_ptr].filename = cfg->filename;
	cfg_include_stack[cfg_include_stack_ptr].line = cfg->line;
	cfg_include_stack_ptr++;

	xfilename = cfg_tilde_expand(filename);

	/*cfg_error(cfg, "opening %s for parsing", xfilename);*/
	cfg_yyin = fopen(xfilename, "r");

	if(!cfg_yyin) {
		cfg_error(cfg, "%s: %s", xfilename, strerror(errno));
		free(xfilename);
		return 1;
	}

	cfg->filename = xfilename;
	cfg->line = 1;

    yy_switch_to_buffer(yy_create_buffer(cfg_yyin, YY_BUF_SIZE));
	return 0;
}

/* write a character to the quoted string buffer, and reallocate as
 * necessary
 */
static void qputc(char ch)
{
	if(qstring_index >= qstring_len) {
		qstring_len += CFG_QSTRING_BUFSIZ;
		cfg_qstring = (char *)realloc(cfg_qstring, qstring_len);
		assert(cfg_qstring);
		memset(cfg_qstring + qstring_index, 0, CFG_QSTRING_BUFSIZ);
	}
	cfg_qstring[qstring_index++] = ch;
}

static YY_BUFFER_STATE pre_string_scan_state;
static YY_BUFFER_STATE string_scan_state;

void cfg_scan_string_begin(const char *buf)
{
	pre_string_scan_state = YY_CURRENT_BUFFER;

	/* yy_scan_string does a yy_switch_to_buffer call for us
	 */
	string_scan_state = yy_scan_string(buf);
}

void cfg_scan_string_end(void)
{
	/* restore to previous state
	 */
	yy_delete_buffer(string_scan_state);
	yy_switch_to_buffer(pre_string_scan_state);
	free(cfg_qstring);
	cfg_qstring = 0;
	qstring_index = qstring_len = 0;
}

static YY_BUFFER_STATE pre_fp_scan_state;
static YY_BUFFER_STATE fp_scan_state;

void cfg_scan_fp_begin(FILE *fp)
{
	pre_fp_scan_state = YY_CURRENT_BUFFER;
	fp_scan_state = yy_create_buffer(fp, YY_BUF_SIZE);
	yy_switch_to_buffer(fp_scan_state);
}

void cfg_scan_fp_end(void)
{
	/* restore to previous state
	 */
	yy_delete_buffer(fp_scan_state);
	yy_switch_to_buffer(pre_fp_scan_state);
	free(cfg_qstring);
	cfg_qstring = 0;
	qstring_index = qstring_len = 0;
}
