libConfuse Tutorial {#tutorial}
===================

This tutorial introduces libConfuse by gradually extending a small
example program.  The numbered listings below are in the `doc/` directory.


Introducing libConfuse in an existing program
---------------------------------------------

Consider this simple program:

@include listing1.c

Simple enough, but we want to extend the program so we can greet others.
Maybe we don't want to greet the whole world, just our neighbour.  We use
libConfuse to let the user decide whom to greet.

@include listing2.c

All programs using libConfuse must first include the `confuse.h` header
file.  This is done on line 2.

On line 6 - 10, the options that should be recognized are defined in an
array of `cfg_opt_t` structs.  This is passed to the `cfg_init()` function
on line 13.  The resulting `cfg_t` context is used by `cfg_parse()`, which
reads the configuration file "hello.conf".  When reading the configuration
file, only options defined in the array of options passed to `cfg_init()`
are recognized.

The friendly greeting is now replaced with a parameter read from the
configuration file.  The value of the `target` option is retrieved with
`cfg_getstr(cfg, "target")`.

Lets take a look at the configuration file hello.conf:

```
# this is the configuration file for the hello program

target = "Neighbour"
```

Here, the target option is set to the string value "Neighbour".  What if
the configuration file was empty or didn't exist?  Then the default value
for the `target` option would be used.  When we initialized our options,
the second parameter to the `CFG_STR()` macro specified the default value.
Thus, if no `target` option was specified in the configuration file, the
hello program would have printed the standard greeting "Hello, World".

### Environment variables in values

What else can we do in the configuration file?  We can set the value to an
environment variable:

```
target = ${USER}
```

This results in the hello program greeting the user who runs it.  On some
systems, the USER variable might not be available, so we want to specify a
default value in those cases:

```
target = ${USER:-User}
```

Now, if the USER environment variable is unset, the string "User" will be
used instead.


Other types of options {#other-types-of-options}
----------------------

Of course, not only strings can be specified in the configuration file.
libConfuse can parse strings, integers, booleans and floating point
values.  These are the fundamental values, and they are all also available
as lists.  We'll talk more about lists in the next chapter.

The macros used to initialize a string, integer, boolean and a float is,
respectively, `CFG_STR()`, `CFG_INT()`, `CFG_BOOL()`, `CFG_FLOAT()` and
`CFG_PTR()`.  All macros take three parameters: the name of the option, a
default value and flags.  To retrieve the values, use `cfg_getstr()`,
`cfg_getint()`, `cfg_getbool()`, `cfg_getfloat()` or `cfg_getptr()`,
respectively.

Let's introduce an integer option that tells us how many times to print
the greeting:

@include listing3.c

Here we have used the `CFG_INT()` macro to initialize an integer option
named "repeat".  The default value is 1 as in the standard greeting.  The
value is retrieved with `cfg_getint()`.

Integer values may be written in different bases: decimal by default,
hexadecimal with a `0x` prefix, octal with a leading `0`, and binary with
a `0b` prefix.  For example `42`, `0x2a`, `052` and `0b101010` all specify
the same value.

A plain `CFG_INT()` stores its value in a C `long int`, so its width
follows the platform: 64 bits on most 64-bit systems, but only 32 bits on
32-bit platforms, where values outside the signed 32-bit range do not fit.
It is also always signed.  When you need a defined width or an unsigned
value, use the fixed-width macros instead: `CFG_INT8()`, `CFG_INT16()`,
`CFG_INT32()` and `CFG_INT64()`, or their unsigned counterparts
`CFG_UINT8()` through `CFG_UINT64()`.  These have the same width on every
platform and reject out-of-range values while parsing.

But, wait a moment, what if the user specified a negative value for
"repeat"?  Or a too large positive value?  libConfuse can handle that with
a so-called validating callback.  We'll come back to this problem in
[Validating callback functions](#validating-callbacks), but we will first
take a look at lists.


Introducing lists
-----------------

That was easy.  Now let's extend the program a bit so we can greet more
than one "target".  We'd like to be able to specify a list of targets to
greet.

The list versions of the initialization macros are named `CFG_STR_LIST()`,
`CFG_INT_LIST()`, `CFG_BOOL_LIST()` and `CFG_FLOAT_LIST()`.  They take the
same parameters as the non-list versions, except the default value must be
a string surrounded by curly braces.

The modified program is shown below:

@include listing4.c

Three things are a bit different here.  First, the macro to initialize the
"targets" option is `CFG_STR_LIST()`.  This tells libConfuse that
"targets" is a list of strings.  Second, the default value in the second
parameter is surrounded by curly braces.  This is needed to indicate to
libConfuse where the list of values ends.

The third change is in the printing of the greeting.  First we print the
"Hello" string.  Then we loop through all values found for the "targets"
option.  The number of values is retrieved with the `cfg_size()` function.
The string values are then retrieved with `cfg_getnstr()`, which is an
indexed version of `cfg_getstr()`.  In fact, `cfg_getstr()` is equivalent
to `cfg_getnstr()` with an index of zero.

In the configuration file hello.conf, we can now specify a list of targets
to greet:

```
# this is the configuration file for the hello program

targets = {"Life", "Universe", "Everything"}
repeat = 1
```

The output of the hello program, run with the above configuration file,
is: "Hello, Life, Universe, Everything!"

Again, if no targets were configured, the greeting would have been the
standard "Hello, World!".


Using sections
--------------

So far, we have only use a flat configuration file.  libConfuse can also
handle sections to build a hierarchy of options.  Sections can be used to
group options in logical blocks, and those blocks can (optionally) be
specified multiple times.

Sections are initialized with the `CFG_SEC()` macro.  It also takes three
parameters: the name of the option, an array of options allowed in the
section and flags.

We'll extend the, now rather complex, hello program so we can do other
kinds of greetings, not just "Hello".  Each greeting will have its own
settings for targets and repeat.

@include listing5.c

We have renamed the option array from "opts" to "greet_opts", and
introduced a new "opts" array that only has one option: a "greeting"
section.  The second parameter of the `CFG_SEC()` macro points to the old
greeting options "targets" and "repeat".

We have also used a couple of flags to alter the behaviour of the section:
`CFGF_TITLE` means that a greeting section should have a title and the
`CFGF_MULTI` flag tells libConfuse that this section may be specified
multiple times in the configuration file.  The title of a section is
retrieved with the `cfg_title()` function.

The outmost loop (with index j) now loops through all given sections in the
configuration file.  We retrieve a section with a `cfg_getnsec()` call.
The value returned is a pointer to a `cfg_t` struct, the same type as
returned by `cfg_init()`.  Thus we can use the ordinary value retrieval
functions `cfg_getstr()`, `cfg_getint()` and so on to retrieve values of
options inside the section.

Ok, so how does the configuration file look like for this setup?

```
# this is the configuration file for the hello program

greeting Hello
{
    targets = {"Life", "Universe", "Everything"}
    repeat = 1
}

greeting Bye
{
    targets = {Adams}
    repeat = 1
}
```

The program will loop through the sections in the order specified in the
configuration file.  First it will find the "Hello" section.  It prints the
title of the section, "Hello", retrieved with `cfg_title()`.  Then the
targets are printed just as in the previous examples, but this time the
values are retrieved from the cfg_greet section.  Next, the section titled
"Bye" is found, and the values are retrieved from that section.

When run, the program produces the following:

```
$ ./listing5
Hello, Life, Universe, Everything!
Bye, Adams!
$
```


Parsing from internal buffers
-----------------------------

So far, we have only parsed configuration data from files.  libConfuse can
also parse buffers, or in-memory character strings.  We will use this to
fix a problem in the previous code.

The problem is that without a configuration file, the hello program will
not print anything.  We want it to at least print the standard greeting
"Hello, World!" if no configuration file is available.

We can't have a default value for a section that can be specified multiple
times (ie, a section with the `CFGF_MULTI` flag set).  Instead we will
parse a default configuration string if no section has been parsed:

@include listing6.c

Only the changes from the previous code is shown here.  We check if the
size of the "greeting" section is zero (ie, no section has been defined).
In that case we call `cfg_parse_buf()` to parse a default in-memory string
"greeting Hello {}".  This string defines a greeting section with title
Hello, but without any sub-options.  This way we rely on the default
values of the (sub-)options "targets" and "repeat".

When this program is run, it issues the well-known standard greeting
"Hello, World!" if no configuration file is present.


Validating callback functions {#validating-callbacks}
-----------------------------

Remember the problem about a negative or too large "repeat" value in
[Other types of options](#other-types-of-options)?  The code that prints
the greeting has those lines:

```c
...
repeat = cfg_getint(cfg_greet, "repeat");
while(repeat--)
...
```

The repeat variable is defined as an int, a signed integer.  If the user
specified a negative repeat value in the configuration file, this code
would continue to decrease the repeat variable until it eventually
underflowed.

We'll fix this by not allowing a negative value in the configuration file.
Of course we could first just check if the value is negative and then
abort, using `cfg_getint()` and a test.  But we will use a validating
callback function instead.  This way `cfg_parse()` will return an error
directly when parsing the file, additionally indicating on which line the
error is.

A validating callback function is defined as:

```c
typedef int (*cfg_validate_callback_t)(cfg_t *cfg, cfg_opt_t *opt);
```

This function takes two arguments: the section and the option.  It should
return 0 on success (ie, the value validated ok).  All other values
indicates an error, and the parsing is aborted.  The callback function
should notify the error itself, for example by calling `cfg_error()`.

Here is the code for the callback function:

@include listing7.c

Only the last value is validated, because libConfuse will call this
function once for every value corresponding to the option.  Since the
"repeat" option is not a list, we could instead have used
`cfg_opt_getint(opt)` to retrieve the only value.  However, if we later
want to use this callback to validate an integer list, it is already
lists-aware.

### Installing the callback

The validating callback is installed with `cfg_set_validate_func()`.  It is
called with a string specifying which option is affected, and a pointer to
the callback function.  To specify an option in a subsection, the section
and the option must be separated with a vertical bar ("|").

We're now also looking at the return code from `cfg_parse()` to verify that
the parsing was successful.  The complete program is now:

@include listing8.c


Value parsing callback
----------------------

A value parsing callback is another kind of callback function available in
libConfuse.  This function is used to map a string into some other value.
One example is to extend a boolean option to accept the values "yes", "no"
and "ask" (or perhaps "true", "false" and "maybe").  Those values should be
mapped to the integers 1, 2 and 3.

```c
typedef int (*cfg_callback_t)(cfg_t *cfg, cfg_opt_t *opt,
                              const char *value, void *result);
```


Functions
---------

libConfuse supports functions to parse options that does not fit well in
the general syntax.  Functions can be called with a variable number of
arguments.  No data from the function or any arguments are stored by
libConfuse after the function has run.  It is up to the caller to process
and/or save the data.

A function is defined with a `CFG_FUNC()` macro.  It takes two arguments:
the name of the function and a function callback.  The callback is defined
as:

```c
typedef int (*cfg_func_t)(cfg_t *cfg, cfg_opt_t *opt,
                          int argc, const char **argv);
```

### Predefined functions

Currently there is only one pre-defined function: `cfg_include()`.  This
function includes another configuration file.  Configuration data is
immediately read from the included file, and is returned to the position
right after the include() statement upon end of file.

To use this function, include a `CFG_FUNC()` entry in your options:

```c
cfg_opt_t opts[] = {
    CFG_FUNC("include", cfg_include),
    CFG_END()
};
```

In the configuration file, it is used in the following way:

```
include("included.conf")
```
