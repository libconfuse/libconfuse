Maintenance and Release Checklist
=================================

1. Encourage contributors to write/update tests, in particular on new features
2. Check tests regularly, Travis-CI can do this automatically
3. Before a release, remember ABI versioning


Maintenance
-----------

* Leverage GitHub issues for milestone planning
* Reference issues from GitHub pull requests to alert issue subscribers


Release Process
---------------

* Update ChangeLog, loosely follow http://keepachangelog.com/ recommendations
  - Inform users in a plain language of changes and bug fixes
  - Do *not* copy-paste GIT commit logs
  - Order changes and fixes according to importance, most relevant first
* Run unit tests
* Make at least one -rcN release and test it in an actual real project
* Do we need to bump the ABI version?
* Tag
* Push last commit(s) *and* tags to GitHub
* Make release

        make distclean
        ./autogen.sh
        ./configure
		make release

* Create new release in GitHub releases page
* Copy and paste ChangeLog entry, check any links!
* Upload release tarball and MD5 files


Library Versioning
------------------

LibConfuse rely on GNU libtool to do all the work with building the
library.  For a user of the library it is important to maintain a clear
ABI versioning scheme.  This is not the same as the libConfuse version,
but rather the library "compatibility level".

https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html

Usually, non-developers have no interest in running development versions
(releases are frequent enough), and developers are expected to know how
to juggle versions.  In such an ideal world, it is good enough to bump
the library version just prior to a release.

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

So, as a general rule of thumb:

1. Release often,
2. Do not mindlessly change the API, and
3. Bump library version just before release

The latter recommendation stands unless there is a good reason to not
release, or a good reason to bump the library version without doing a
release, because it was the simple thing to do for some reason.

