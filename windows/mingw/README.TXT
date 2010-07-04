
Compilation of the static library with MinGW.
---------------------------------------------

* QUICK START

1) In order to build the library successfully, you should first check that the
   MinGW 'bin' directory is in the PATH environment variable.

2) Then, launch the script 'build.bat' to produce 'libconfuse.a'.

This script has been tested with MinGW 3.1.0 under Windows 2000.
It should also work with other versions of MinGW and Windows.


* DETAILED DESCRIPTION

The build script is very simple. It just sets the relative path to the source
directory and the compilation flags. Then, it runs the appropriate gcc
commands to produce the object files. Finally, it uses the 'ar' command to
produce the static library 'libconfuse.a'.

Once you've built the library, you need only two files to add libConfuse to
your project: 'confuse.h' located in the source directory and 'libconfuse.a'.

Here is an example of how to use this library under Windows with MinGW:
gcc -Wall -I..\..\src  -L. -o myexample.exe myexample.c -lconfuse

