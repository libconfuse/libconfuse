%{
/*
 * Copyright (c) 2002-2017  Martin Hedenfalk <martin@bzero.se>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef HAVE_UNISTD_H
# define YY_NO_UNISTD_H
#else
# include <unistd.h>			/* isatty() */
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#endif

/* Defines isatty() for non UNIX systems */
#include "confuse.h"

#if defined(ENABLE_NLS) && defined(HAVE_GETTEXT)
# include <libintl.h>
# define _(str) dgettext(PACKAGE, str)
#else
# define _(str) str
#endif
#define N_(str) str

/*
 * Prevent compilation of static input() function in generated code
 * This function is never used but GCC 4.3 will warn about it.
 */
#define YY_NO_INPUT

typedef char * YYSTYPE;
extern YYSTYPE cfg_yylval;

#define YY_DECL int cfg_yylex ( cfg_t *cfg )

/* temporary buffer for the quoted strings scanner
 */
#define CFG_QSTRING_BUFSIZ 32
char *cfg_qstring = NULL;
static size_t qstring_index = 0;
static size_t qstring_len = 0;
static void qputc(char ch);
static void qput(cfg_t *cfg, char skip);
static void qbeg(int state);
static int  qend(cfg_t *cfg, int trim, int ret);
static int  qstr(cfg_t *cfg, char skip, int ret);

#define MAX_INCLUDE_DEPTH 10
struct {
    FILE *fp;
    char *filename;
    unsigned int line;
} cfg_include_stack[MAX_INCLUDE_DEPTH];
int cfg_include_stack_ptr = 0;

/* Verbatim capture buffer for CFGT_RAWSEC bodies.  While capturing, every
 * byte the scanner matches is echoed here via YY_USER_ACTION, so the body
 * is stored exactly as written while the parser walks it as usual. */
static int cfg_raw_capturing = 0;
static char *cfg_raw_buf = NULL;
static size_t cfg_raw_len = 0;
static size_t cfg_raw_cap = 0;
static void cfg_raw_append(const char *s, int n);

#define YY_USER_ACTION if (cfg_raw_capturing) cfg_raw_append(yytext, yyleng);

void cfg_scan_fp_begin(FILE *fp);
void cfg_scan_fp_end(void);

%}

%option noyywrap

 /* start conditions
  */
%x comment
%x dq_str
%x sq_str

%%

[ \t]+    /* eat up whitespace */

\n   cfg->line++; /* keep track of line number */

 /*
  * handle one-line comments
  *
  * Note: Comments with lots of leading #### or //// are fully
  *       consumed and are not included in CFGT_COMMENT yylval
  */
"#"{1,}.*   return qstr(cfg, '#', CFGT_COMMENT);
"/"{2,}.*   return qstr(cfg, '/', CFGT_COMMENT);

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
"/*"                    qbeg(comment);
<comment>[^*\n]*        qput(NULL, 0);  /* anything that's not a '*' */
<comment>"*"+[^*/\n]*   qput(NULL, 0);  /* '*'s not followed by '/'s */
<comment>\n             qput(cfg, 0);
<comment>[ \t]*"*"+"/"  return qend(cfg, 1, CFGT_COMMENT);

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
<dq_str>$\{[^}]*\} { /* environment variable substitution */
    char *var;
    char *e;
    const size_t len = strlen(yytext) - 1;
    const char end_symbol = yytext[len];
    yytext[len] = 0;
    e = strchr(yytext+2, ':');
    if(e && e[1] == '-')
        *e = 0;
    else
        e = NULL;
    var = getenv(yytext+2);
    if(!var && e)
        var = e+2;
    while(var && *var)
        qputc(*var++);

    if(e)
        *e = ':';
    yytext[len] = end_symbol;
}
<dq_str>\n   {
    qputc('\n');
    cfg->line++;
}
<dq_str>\\\n { /* allow continuing on next line */
    /* no-op */
    cfg->line++;
}
<dq_str>\\[0-7]{1,3} {  /* octal escape sequence */
    unsigned int result;
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
<dq_str>"\\x"[0-9A-Fa-f]{1,2} { /* hexadecimal escape sequence */
    unsigned int result;
    sscanf(yytext + 2, "%x", &result);
    qputc(result);
}
<dq_str>\\n  {
    qputc('\n');
}
<dq_str>\\r  {
    qputc('\r');
}
<dq_str>\\b  {
    qputc('\b');
}
<dq_str>\\f  {
    qputc('\f');
}
<dq_str>\\a  {
    qputc('\007');
}
<dq_str>\\e  {
    qputc('\033');
}
<dq_str>\\t  {
    qputc('\t');
}
<dq_str>\\v  {
    qputc('\v');
}
<dq_str>\\.  {
    qputc(yytext[1]);
}
<dq_str>[^\\\"\n]  {
    qputc(yytext[0]);
}

    /* single-quoted string ('...') */
"\'" {
    qstring_index = 0;
    BEGIN(sq_str);
}
<sq_str>\' { /* saw closing quote - all done */
    BEGIN(INITIAL);
    qputc('\0');
    cfg_yylval = cfg_qstring;
    return CFGT_STR;
}
<sq_str>\n   {
    qputc('\n');
    cfg->line++;
}
<sq_str>\\\n { /* allow continuing on next line */
    /* no-op */
    cfg->line++;
}
<sq_str>\\[\\\'] {
    qputc(yytext[1]);
}
<sq_str>\\[^\\\'] {
    qputc(yytext[0]);
    qputc(yytext[1]);
}
<sq_str>[^\\\'\n]+ {
    char *cp = yytext;
    while (*cp != '\0')
        qputc(*cp++);
}
<sq_str><<EOF>> {
    cfg_error(cfg, _("unterminated string constant"));
    return 0;
}

<<EOF>> {
    if (cfg_include_stack_ptr > 0)
    {
        --cfg_include_stack_ptr;
        /* fp opened by cfg_lexer_include()? */
        if (cfg_include_stack[cfg_include_stack_ptr].fp != cfg_yyin) {
            ++cfg_include_stack_ptr;
            return EOF;
        }
        free(cfg->filename);
        cfg->filename = cfg_include_stack[cfg_include_stack_ptr].filename;
        cfg->line = cfg_include_stack[cfg_include_stack_ptr].line;
        fclose(cfg_yyin);
        cfg_scan_fp_end();
    }
    else
    {
        return EOF;
    }
}

$\{[^}]*\} {
    char *var;
    char *e;
    const size_t len = strlen(yytext) - 1;
    const char end_symbol = yytext[len];
    yytext[len] = 0;
    e = strchr(yytext+2, ':');
    if (e && e[1] == '-')
        *e = 0;
    else
        e = NULL;
    var = getenv(yytext+2);
    if (!var && e)
        var = e+2;
    if (!var)
        var = "";
    qstring_index = 0;
    while(*var)
        qputc(*var++);
    qputc('\0');
    cfg_yylval = cfg_qstring;
    if (e)
        *e = ':';
    yytext[len] = end_symbol;
    return CFGT_STR;
}

 /* an unquoted string
  * a slash can't be followed by another slash (c++
  * comment) or an asterisk (C multi-line comment)
  */
(\/[^ #\"\'\t\n\r={}()+,\/*]|[^ #\"\'\t\n\r={}()+,\*])+ {
    cfg_yylval = yytext;
    return CFGT_STR;
 }

. /* eat any non-matching characters */

%%

void cfg_dummy_function(void)
{
    /* please compiler :-)
     * otherwise "defined but not used" warning
     */
    yyunput(0, NULL);
}

static void cfg_raw_append(const char *s, int n)
{
    if (cfg_raw_len + (size_t)n + 1 > cfg_raw_cap) {
        cfg_raw_cap = (cfg_raw_len + (size_t)n + 1) * 2;
        cfg_raw_buf = (char *)realloc(cfg_raw_buf, cfg_raw_cap);
        assert(cfg_raw_buf);
    }
    memcpy(cfg_raw_buf + cfg_raw_len, s, n);
    cfg_raw_len += n;
}

/* Start capturing the verbatim bytes of a CFGT_RAWSEC body.*/
void cfg_raw_begin(void)
{
    cfg_raw_len = 0;
    cfg_raw_capturing = 1;
}

/* Stop raw capturing and return the body.*/
char *cfg_raw_end(void)
{
    cfg_raw_capturing = 0;
    if (cfg_raw_len > 0 && cfg_raw_buf[cfg_raw_len - 1] == '}')
        cfg_raw_len--;
    if (!cfg_raw_buf) {
        cfg_raw_buf = (char *)malloc(1);
        assert(cfg_raw_buf);
        cfg_raw_cap = 1;
    }
    cfg_raw_buf[cfg_raw_len] = '\0';
    return cfg_raw_buf;
}

/* Whether a raw body is currently being captured. */
int cfg_raw_active(void)
{
    return cfg_raw_capturing;
}

/* Current length of the capture buffer, and truncation back to a mark;
 * used to drop an include() call from the captured body. */
size_t cfg_raw_tell(void)
{
    return cfg_raw_len;
}

void cfg_raw_seek(size_t pos)
{
    if (pos <= cfg_raw_len)
        cfg_raw_len = pos;
}

int cfg_lexer_include(cfg_t *cfg, const char *filename)
{
    FILE *fp;
    char *xfilename;

    if (cfg_include_stack_ptr >= MAX_INCLUDE_DEPTH)
    {
        cfg_error(cfg, _("includes nested too deeply"));
        return CFG_PARSE_ERROR;
    }

    cfg_include_stack[cfg_include_stack_ptr].filename = cfg->filename;
    cfg_include_stack[cfg_include_stack_ptr].line = cfg->line;

    if (cfg->path)
    {
        xfilename = cfg_searchpath(cfg->path, filename);
        if (!xfilename)
        {
            cfg_error(cfg, _("%s: Not found in search path"), filename);
            return CFG_PARSE_ERROR;
        }
    }
    else
    {
        xfilename = cfg_tilde_expand(filename);
        if (!xfilename)
        {
            cfg_error(cfg, _("%s: Failed tilde expand"), filename);
            return CFG_PARSE_ERROR;
        }
    }

    fp = fopen(xfilename, "r");
    if (!fp)
    {
        cfg_error(cfg, "%s: %s", xfilename, strerror(errno));
        free(xfilename);
        return CFG_PARSE_ERROR;
    }

    cfg_include_stack[cfg_include_stack_ptr].fp = fp;
    cfg_include_stack_ptr++;
    cfg->filename = xfilename;
    cfg->line = 1;
    cfg_scan_fp_begin(fp);

    return CFG_SUCCESS;
}

/* write a character to the quoted string buffer, and reallocate as
 * necessary
 */
static void qputc(char ch)
{
    if (qstring_index >= qstring_len) {
        qstring_len += CFG_QSTRING_BUFSIZ;
        cfg_qstring = (char *)realloc(cfg_qstring, qstring_len + 1);
        assert(cfg_qstring);
        memset(cfg_qstring + qstring_index, 0, CFG_QSTRING_BUFSIZ + 1);
    }
    cfg_qstring[qstring_index++] = ch;
}

static void qput(cfg_t *cfg, char skip)
{
    char *cp;

    if (cfg)
	cfg->line++;

    cp = yytext;

    while (skip && *cp == skip)
	cp++;

    while (*cp)
        qputc(*cp++);
}

static void qbeg(int state)
{
    BEGIN(state);
    qstring_index = 0;
    if (cfg_qstring)
	memset(cfg_qstring, 0, qstring_len);
}

static char *trim_whitespace(char *str, unsigned int len)
{
    if (!str || !str[0])
	return str;

    while (len > 1) {
	if ((str[len] == 0 || isspace((unsigned char)str[len])) && isspace((unsigned char)str[len - 1]))
	    len--;
	else
	    break;
    }
    str[len] = 0;

    while (isspace((unsigned char)*str))
	str++;

    return str;
}

static int qend(cfg_t *cfg, int trim, int ret)
{
    char *ptr;

    BEGIN(INITIAL);
    if (cfg)
	cfg->line++;

    if (!cfg_qstring)
	qputc('\0');
    ptr = cfg_qstring;

    if (trim)
	ptr = trim_whitespace(cfg_qstring, qstring_index);
    else
	qputc('\0');

    cfg_yylval = ptr;

    return ret;
}

static int qstr(cfg_t *cfg, char skip, int ret)
{
    qbeg(comment);
    qput(cfg, skip);

    return qend(cfg, 1, ret);
}

void cfg_scan_fp_begin(FILE *fp)
{
    cfg_yypush_buffer_state(cfg_yy_create_buffer(fp, YY_BUF_SIZE));
}

void cfg_scan_fp_end(void)
{
    if (cfg_qstring)
	    free(cfg_qstring);
    cfg_qstring = NULL;
    qstring_index = qstring_len = 0;

    /* Release the raw-capture buffer once a top-level parse is done.
     * Guard against buffer pops that happen *during* a capture (an
     * include() inside a raw section), which must keep the buffer. */
    if (!cfg_raw_capturing) {
	    if (cfg_raw_buf)
	        free(cfg_raw_buf);
	    cfg_raw_buf = NULL;
	    cfg_raw_len = cfg_raw_cap = 0;
    }

    cfg_yypop_buffer_state();
}
