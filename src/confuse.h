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

/** A configuration file parser library.
 * @file confuse.h
 *
 */

/**
 * \mainpage libConfuse Documentation
 *
 * \section intro
 *
 * Copyright &copy; 2002-2003 Martin Hedenfalk &lt;mhe@home.se&gt;
 *
 * The latest versions of this manual and the libConfuse software are
 * available at http://www.e.kth.se/~e97_mhe/confuse.shtml
 *
 *
 * <em>If you can't convince, confuse.</em>
 */

#ifndef _cfg_h_
#define _cfg_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>

/** Fundamental option types */
enum cfg_type_t {
	CFGT_NONE,
	CFGT_INT,     /**< integer */
	CFGT_FLOAT,   /**< floating point number */
	CFGT_STR,     /**< string */
	CFGT_BOOL,    /**< boolean value */
	CFGT_SEC,     /**< section */
	CFGT_FUNC     /**< function */
};
typedef enum cfg_type_t cfg_type_t;

/** Flags. */
#define CFGF_NONE 0
#define CFGF_MULTI 1       /**< option may be specified multiple times */
#define CFGF_LIST 2        /**< option is a list */
#define CFGF_NOCASE 4      /**< configuration file is case insensitive */
#define CFGF_TITLE 8       /**< option has a title (only applies to section) */
#define CFGF_ALLOCATED 16  /**< used internally, should not be set by users */
#define CFGF_RESET 32      /**< used internally, should not be set by users */

/** Return codes from cfg_parse(). */
#define CFG_SUCCESS 0
#define CFG_FILE_ERROR -1
#define CFG_PARSE_ERROR 1

#define is_set(f, x) ((f & x) == f)

typedef union cfgval_t cfgval_t;
typedef struct cfgopt_t cfgopt_t;
typedef struct cfg_t cfg_t;
typedef int cfg_flag_t;

/** Function prototype used by CFGT_FUNC options.
 *
 * This is a callback function, registered with the CFG_FUNC
 * initializer. Each time libConfuse finds a function, the registered
 * callback function is called (parameters are passed as strings, any
 * conversion to other types should be made in the callback
 * function). libConfuse does not support any storage of the data
 * found; these are passed as parameters to the callback, and it's the
 * responsibility of the callback function to do whatever it should do
 * with the data.
 *
 * @param cfg The configuration file context.
 * @param opt The option.
 * @param argc Number of arguments passed. The callback function is
 * responsible for checking that the correct number of arguments are
 * passed.
 * @param argv Arguments as an array of character strings.
 *
 * @return On success, 0 should be returned. All other values
 * indicates an error, and the parsing is aborted. The callback
 * function should notify the error itself, for example by calling
 * cfg_error().
 *
 * @see CFG_FUNC
 */
typedef int (*cfg_func_t)(cfg_t *cfg, cfgopt_t *opt,
						  int argc, const char **argv);

/** Value parsing callback prototype
 *
 * This is a callback function (different from the one registered with
 * the CFG_FUNC initializer) used to parse a value. This can be used
 * to override the internal parsing of a value.
 *
 * Suppose you want an integer option that only can have certain
 * values, for example 1, 2 and 3, and these should be written in the
 * configuration file as "yes", "no" and "maybe". The callback
 * function would be called with the found value ("yes", "no" or
 * "maybe") as a string, and the result should be stored in the result
 * parameter.
 *
 * @param cfg The configuration file context.
 * @param opt The option.
 * @param value The value found in the configuration file.
 * @param result Pointer to storage for the result, cast to a void pointer.
 *
 * @return On success, 0 should be returned. All other values
 * indicates an error, and the parsing is aborted. The callback
 * function should notify the error itself, for example by calling
 * cfg_error().
 */
typedef int (*cfg_callback_t)(cfg_t *cfg, cfgopt_t *opt,
							  const char *value, void *result);

/** Boolean values. */
typedef enum {cfg_false, cfg_true} cfgbool_t;

/** Error reporting function. */
typedef void (*cfg_errfunc_t)(cfg_t *cfg, const char *fmt, va_list ap);

/** Data structure holding information about a "section". Sections can
 * be nested. A section has a list of options (strings, numbers,
 * booleans or other sections) grouped together.
 */
struct cfg_t {
	cfg_flag_t flags;       /**< any flags passed to cfg_init() */
	char *name;             /**< the name of this section, the root
							 * section returned from cfg_init() is
							 * always named "root" */
	cfgopt_t *opts;         /**< array of options */
	char *title;            /**< optional title for this section, only
							 * set if CFGF_TITLE flag is set */
	char *filename;         /**< name of the file being parsed */
	int line;               /**< line number in the config file */
	cfg_errfunc_t errfunc;  /**< this function (set with
							 * cfg_set_error_function) is called for
							 * any error message */
};

/** Data structure holding the value of a fundamental option value.
 */
union cfgval_t {
	long int number;        /**< integer value */
	double fpnumber;        /**< floating point value */
	cfgbool_t boolean;      /**< boolean value */
	char *string;           /**< string value */
	cfg_t *section;         /**< section value */
};

/** Data structure holding information about an option. The value(s)
 * are stored as an array of fundamental values (strings, numbers).
 */
struct cfgopt_t {
	char *name;             /**< the name of the option */
	cfg_type_t type;        /**< type of option */
	unsigned int nvalues;   /**< number of values parsed */
	cfgval_t **values;      /**< array of found values */
	cfg_flag_t flags;       /**< flags */
	cfgopt_t *subopts;      /**< suboptions (only applies to sections) */
	cfgval_t def;
#if 0
	char *def;              /**< default value as a string which is
							 * parsed in the same way as the config file */
#endif
	cfg_func_t func;        /**< function callback for CFGT_FUNC options */
	void *simple_value;     /**< pointer to user-specified variable to
							 * store simple values (created with the
							 * CFG_SIMPLE_* initializers) */
	cfg_callback_t cb;
};

/** Initialize a string option
 */
#define CFG_STR(name, def, flags)  \
                   {name, CFGT_STR, 0, 0, flags, 0, {string: def}, 0, 0, 0}

/** Initialize a string option with a value parsing callback
 */
#define CFG_STR_CB(name, def, flags, cb)  \
                  {name, CFGT_STR, 0, 0, flags, 0, {string: def}, 0, 0, cb}

/** Initialize a "simple" string option.
 *
 * "Simple" options (in lack of a better expression) does not support
 * lists of values. LibConfuse will store the value of a simple option
 * in the user-defined location specified by the value parameter in
 * the initializer. Simple options are not stored in the cfg_t context
 * (you can thus not use the cfg_get* functions to get the
 * value). Sections can not be initialized as a "simple" option.
 *
 * @param name name of the option
 * @param value pointer to a character pointer (a char **). This value
 * must be initalized either to NULL or to a malloc()'ed string. You
 * can't use
 * <pre>
 * char *user = "joe";
 * ...
 * cfgopt_t opts[] = {
 *     CFG_SIMPLE_STR("user", &user),
 * ...
 * </pre>
 * since libConfuse will try to free the static string "joe" (which is
 * an error) when a "user" option is found. Rather, use the following
 * code snippet:
 *
 * <pre>
 * char *user = strdup("joe");
 * ...
 * cfgopt_t opts[] = {
 *      CFG_SIMPLE_STR("user", &user),
 * ...
 * </pre>
 *
 */
#define CFG_SIMPLE_STR(name, value)  \
                   {name, CFGT_STR, 0, 0, CFGF_NONE, 0, {string: 0}, 0, value, 0}

/** Initialize an integer option
 */
#define CFG_INT(name, def, flags)  \
                   {name, CFGT_INT, 0, 0, flags, 0, {number: def}, 0, 0, 0}

/** Initialize an integer option with a value parsing callback
 */
#define CFG_INT_CB(name, def, flags, cb) \
                  {name, CFGT_INT, 0, 0, flags, 0, {number: defdef}, 0, 0, cb}

/** Initialize a "simple" integer option.
 */
#define CFG_SIMPLE_INT(name, value)  \
                   {name, CFGT_INT, 0, 0, CFGF_NONE, 0, 0, 0, value, 0}

/** Initialize a floating point option
 */
#define CFG_FLOAT(name, def, flags) \
                 {name, CFGT_FLOAT, 0, 0, flags, 0, def, 0, 0, 0}

/** Initialize a floating point option with a value parsing callback
 */
#define CFG_FLOAT_CB(name, def, flags, cb) \
               {name, CFGT_FLOAT, 0, 0, flags, 0, def, 0, 0, cb}

/** Initialize a "simple" floating point option (see documentation for
 * CFG_SIMPLE_STR for more information).
 */
#define CFG_SIMPLE_FLOAT(name, value) \
                   {name, CFGT_FLOAT, 0, 0, CFGF_NONE, 0, 0, 0, value, 0}

/** Initialize a boolean option
 */
#define CFG_BOOL(name, def, flags) \
                  {name, CFGT_BOOL, 0, 0, flags, 0, def, 0, 0, 0}

/** Initialize a boolean option with a value parsing callback
 */
#define CFG_BOOL_CB(name, def, flags, cb) \
                 {name, CFGT_BOOL, 0, 0, flags, 0, def, 0, 0, cb}

/** Initialize a "simple" boolean option.
 */
#define CFG_SIMPLE_BOOL(name, value) \
                   {name, CFGT_BOOL, 0, 0, CFGF_NONE, 0, 0, 0, value, 0}

/** Initialize a section
 *
 * @param name The name of the option
 * @param opts Array of options that are valid within this section
 
 * @param flags Flags, specify CFGF_MULTI if it should be possible to
 * have multiple sections, and CFGF_TITLE if the section(s) must have
 * a title (which can be used in the cfg_gettsec() function)
 *
 */
#define CFG_SEC(name, opts, flags) \
                   {name, CFGT_SEC, 0, 0, flags, opts, 0, 0, 0, 0}

/** Initialize a function
 * @param name The name of the option
 * @param func The callback function.
 *
 * @see cfg_func_t
 */
#define CFG_FUNC(name, func) \
                   {name, CFGT_FUNC, 0, 0, CFGF_NONE, 0, 0, func, 0, 0}

/** Terminate list of options. This must be the last initializer in
 * the option list.
 */
#define CFG_END()  {0, CFGT_NONE, 0, 0, CFGF_NONE, 0, 0, 0, 0}

/** Create and initialize a cfg_t structure. This should be the first
 * function called when setting up the parsing of a configuration
 * file. The options passed in the first parameter is typically
 * statically initialized, using the CFG_* initializers. The last
 * option in the option array must be CFG_END(), unless you like
 * segmentation faults.
 *
 * @param opts An arrary of options
 * @param flags One or more flags (bitwise or'ed together)
 *
 * @return A configuration context structure. This pointer is passed
 * to all other functions as the first parameter.
 */
cfg_t *       cfg_init(cfgopt_t *opts, cfg_flag_t flags);

/** Parse a configuration file. Tilde expansion is performed on the
 * filename before it is opened. After a configuration file has been
 * initialized (with cfg_init()) and parsed (with cfg_parse()), the
 * values can be read with the cfg_getXXX functions.
 *
 * @param cfg The configuration file context.
 * @param filename The name of the file to parse.
 *
 * @return On success, 0 is returned. If the file couldn't be opened
 * for reading, -1 is returned. On all other errors, 1 is returned and
 * cfg_error was called with a descriptive error message.
 */
int           cfg_parse(cfg_t *cfg, const char *filename);

/** Free the memory allocated for the values of a given option. Only
 * the values are freed, not the option itself (it is often statically
 * initialized). 
 */
void          cfg_free_val(cfgopt_t *opt);

/** Free a cfg_t context. All memory allocated by the cfg_t context
 * structure are freed, and can't be used in any further cfg_* calls.
 */
void          cfg_free(cfg_t *cfg);

/** Install a user-defined error reporting function.
 * @return The old error reporting function is returned.
 */
cfg_errfunc_t cfg_set_error_function(cfg_t *cfg, cfg_errfunc_t errfunc);

/** Show a parser error. Any user-defined error reporting function is called.
 * @see cfg_set_error_function
 */
void          cfg_error(cfg_t *cfg, const char *fmt, ...);

/** Returns the value of an integer option. This is the same as
 * calling cfg_getnint with index 0.
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @return The requested value is returned. If the option was not set
 * in the configuration file, the default value given in the
 * corresponding cfgopt_t structure is returned. If no option is found
 * with that name, 0 is returned.
 */
long int      cfg_getint(cfg_t *cfg, const char *name);

/** Returns the value of a floating point option.
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @return The requested value is returned. If the option was not set
 * in the configuration file, the default value given in the
 * corresponding cfgopt_t structure is returned. If no option is found
 * with that name, cfg_error is called and 0 is returned.
 */
double        cfg_getfloat(cfg_t *cfg, const char *name);

/** Returns the value of a string option.
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @return The requested value is returned. If the option was not set
 * in the configuration file, the default value given in the
 * corresponding cfgopt_t structure is returned. If no option is found
 * with that name, cfg_error is called and NULL is returned.
 */
char *        cfg_getstr(cfg_t *cfg, const char *name);

/** Returns the value of a boolean option.
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @return The requested value is returned. If the option was not set
 * in the configuration file, the default value given in the
 * corresponding cfgopt_t structure is returned. If no option is found
 * with that name, cfg_error is called and cfg_false is returned.
 */
cfgbool_t     cfg_getbool(cfg_t *cfg, const char *name);

/** Returns the value of a section option. The returned value is
 * another cfg_t structure that can be used in following calls to
 * cfg_getint, cfg_getstr or other get-functions.
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @return The requested value is returned.  If no section is found
 * with that name, 0 is returned. Note that there can be no default
 * values for a section.
 */
cfg_t *       cfg_getsec(cfg_t *cfg, const char *name);

/** Return the number of values this option has. If no default value
 * is given for the option and no value was found in the config file,
 * 0 will be returned (ie, the option value is not set at all). It that
 * case, calling cfg_getXXX will abort.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 */
unsigned int  cfg_size(cfg_t *cfg, const char *name);

/** Indexed version of cfg_getint().
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param index Index of values. Zero based.
 * @see cfg_getint
 */
long int      cfg_getnint(cfg_t *cfg, const char *name, unsigned int index);

/** Indexed version of cfg_getfloat().
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param index Index of values. Zero based.
 * @see cfg_getint
 */
double        cfg_getnfloat(cfg_t *cfg, const char *name, unsigned int index);

/** Indexed version of cfg_getstr().
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param index Index of values. Zero based.
 * @see cfg_getstr
 */
char *        cfg_getnstr(cfg_t *cfg, const char *name, unsigned int index);

/** Indexed version of cfg_getbool().
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param index Index of values. Zero based.
 * @see cfg_getstr
 */
cfgbool_t     cfg_getnbool(cfg_t *cfg, const char *name, unsigned int index);

/** Indexed version of cfg_getsec().
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param index Index of values. Zero based.
 * @see cfg_getsec
 */
cfg_t *       cfg_getnsec(cfg_t *cfg, const char *name, unsigned int index);

/** Return a section given the title.
 *
 * @param cfg The configuration file context.
 * @param name The name of the option.
 * @param title The title of this section. The CFGF_TITLE flag must
 * have been set for this option.
 */
cfg_t *       cfg_gettsec(cfg_t *cfg, const char *name, const char *title);

const char *  cfg_title(cfg_t *cfg);

extern const char *confuse_copyright;
extern const char *confuse_version;
extern const char *confuse_author;

/** Predefined include-function. This function can be used in the
 * options passed to cfg_init() to specify a function for including
 * other configuration files in the parsing. For example:
 * CFG_FUNC("include", &cfg_include)
 */
int cfg_include(cfg_t *cfg, cfgopt_t *opt, int argc, const char **argv);

/** Does tilde expansion (~ -> $HOME) on the filename.
 * @return The expanded filename is returned. If a ~user was not
 * found, the original filename is returned. In any case, a
 * dynamically allocated string is returned, which should be free()'d
 * by the caller.
 */
char *cfg_tilde_expand(const char *filename);

int cfg_parse_boolean(const char *s);

cfgopt_t *cfg_getopt(cfg_t *cfg, const char *name);
cfgval_t *cfg_setopt(cfg_t *cfg, cfgopt_t *opt, char *value);

int cfg_output_fp(cfg_t *cfg, FILE *fp);
int cfg_output(cfg_t *cfg, const char *filename);

void cfg_setint(cfg_t *cfg, const char *name, long int value);
void cfg_setnint(cfg_t *cfg, const char *name,
				 long int value, unsigned int index);
void cfg_setfloat(cfg_t *cfg, const char *name, double value);
void cfg_setnfloat(cfg_t *cfg, const char *name,
				   double value, unsigned int index);
void cfg_setbool(cfg_t *cfg, const char *name, cfgbool_t value);
void cfg_setnbool(cfg_t *cfg, const char *name,
				  cfgbool_t value, unsigned int index);
void cfg_setstr(cfg_t *cfg, const char *name, const char *value);
void cfg_setnstr(cfg_t *cfg, const char *name,
				 const char *value, unsigned int index);

#ifdef __cplusplus
}
#endif

#endif

/** @example cfgtest.c
 */

/** @example simple.c
 */

/** @example yafc_cfg.c
 */
