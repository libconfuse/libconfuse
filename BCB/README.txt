Project files for Borland C++ Builder 6.0.

For some reason, the DLL created by BCB prepends an underscore on all exported symbols. Therefore, programs compiled with other compilers linked with libConfuse.dll created by BCB will fail with unresolved symbols. The same goes the other way around.

There is a checkbox "Generate underscores" which, when unchecked, makes linking fail with unresolved symbols from the standard C library instead (ie, functions like realloc and memcpy).

If anyone has a solution for this, you're more than welcome to send a patch to <confuse-devel@nongnu.org>
