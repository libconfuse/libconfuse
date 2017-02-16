Maintenance and Release Checklist
=================================

Maintenance
-----------

* Encourage contributors to write tests, in particular for new features
* Run tests regularly, use Travis-CI to do this automatically
* Leverage GitHub issues for milestone planning
* Reference issues from GitHub pull requests to alert issue subscribers
* Bump library ABI version just before release!


Release Checklist
-----------------

* Update ChangeLog, follow http://keepachangelog.com/ loosely
  - Inform users in a plain language of changes and bug fixes
  - Do *not* copy-paste GIT commit logs!
  - Order entrys according to importance, most relevant first
* Run unit tests: `make check`
* Make at least one `-rcN` release and test it in an actual real project
* Do we need to bump the ABI version? (Probably, see below)
* Tag
* Push last commit(s) *and* tags to GitHub
* Make release

        make distclean
        ./autogen.sh
        ./configure
        make release

* Create new release in GitHub releases page
* Copy and paste ChangeLog entry, check any stale links!
* Upload release tarball and MD5 files


Library Versioning
------------------

LibConfuse relies on GNU Libtool for building the library.  For a user
of the library it is important to maintain a clear ABI versioning
scheme.  This is not the same as the libConfuse version, but rather the
library "compatibility level".

The libConfuse ABI version is specified in `src/Makefile.am` and looks
like this:

    libconfuse_la_LDFLAGS = -version-info 0:0:0
                                           \ \ `-- age
                                            \ `--- revision
                                             `---- current

It must be updated according to the [GNU Libtool recommendations][1]:

1. Start with version information of `0:0:0` for each libtool library.
2. Update the version information only immediately before a public
   release of your software.  More frequent updates are unnecessary, and
   only guarantee that the current interface number gets larger faster.
3. If the library *source code has changed at all* since the last update,
   then increment revision (`c:r:a` becomes `c:r+1:a`).
4. If any *interfaces have been added, removed, or changed* since the
   last update, increment current, and set revision to 0.
5. If any *interfaces have been added* since the last public release,
   then increment age.
6. If any *interfaces have been removed or changed* since the last
   public release, then set age to 0.

Usually, non-developers have no interest in running development versions
(releases are frequent enough), and developers are expected to know how
to juggle versions.  In such an ideal world, it is good enough to bump
the library version just prior to a release, point 2.

However, if releases are few and far between, distributors may start to
use snapshots.  When a distributor uses a snapshot, the distributor has
to handle the library version manually.  Things can get ugly when the
distributor has released an intermediate version with a bumped library
version, and when the official release is bumped to that version, the
distributor will then have to bump the library version for the official
release, and it can be confusing if someone reports bugs on versions
that you didn't even know existed.

The problem with bumping the version with every change is that if your
interface is not finished, the version number might run away, and it
looks pretty bad if a library is at version 262.  It kind of tells the
user that the library interface is volatile, which is not good for
business.

[1]: https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
