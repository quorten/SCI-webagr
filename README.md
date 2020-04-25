`SCI-webagr`
============

`SCI-webagr` is the reconstructed source code for the Spy
Corp. International Website Access Agreement program, `webagr.exe`.
Historically, it was only ever run on a single Windows XP Home Edition
computer.  Since its original use case was very narrow, its originally
designed software compatibility range was also deliberately left very
narrow in the interest of maximum development speed.

What does this do?  Basically, it displays an "terms of service" style
agreement dialog with a cool watermark image of the Spy Corp. logo
from the Spy Fox computer games, all via straight Windows API calls.
The system menu is disabled so that you cannot click the close button
on the window, the only way to proceed is to click the accept or
decline buttons.  The decline button is the default button, but even
if you do click the decline button, the program deliberately treats it
as an accept anyways and proceeds to run the "sciup" spyware
downloader and updater program.  (That is, it pretends to run such a
program.)  The CPU usage boosts to 100% during the process and your
computer fans may even rev to a higher speed.  Then after all the
spyware downloads and updates are completed in 12.3 seconds, you are
brought to the Spy Corp. International website's homepage, unless your
access has been denied.

Building
--------

The latest version is built using a Makefile, use `Makefile.mingw` for
compiling with MinGW, `Makefile.msvc` for compiling with MSVC.  The
MinGW Makefile supports cross-compiling, setup the CROSS variable
according to the toolchain prefix.  MinGW GCC 3.x is recommended for
the most compact code generation.  Newer versions will still work, but
they'll copy more unused boilerplate `libc` and `libgcc`
initialization code in by default.

The historic versions are all built based off of MSVC 6 project files.
To compile with MinGW, you can use the modern Makefile there too, with
minor modifications.

The Reconstruction
------------------

What happened to the original source code?  In order to an upgrade
from Microsoft Visual Studio 6.0 Enterprise Edition to Microsoft
Visual C++ 2005 Express Edition, I had to first uninstall Microsoft
Visual Studio 6.0.  During the installation process, I was prompted
whether I wanted to keep or delete the projects directory inside the
MSVC Program Files directory.  I chose to delete it since I felt that
the projects in there were no longer important, and yes that included
the source code for `webagr.exe`.  And, well, if we were operating in
the 1980s, it isn't, in fact, important compared to my other projects.

But, I made a fundamental logical fallacy in that decision-making
process: the sole purpose for uninstalling MSVC 6 was due to the
overhead of all the development headers, libraries, and developer
documentation.  My own "unimportant" projects were negligibly small in
comparison, so there really was no reason to delete them until I truly
did run out of space due to that particular reason, and that
definitely never happened at the time, nor is it ever going to happen
in the future given the pace that storage technology has already
advanced.

Now, in the present, since I wanted to have the source code up on
GitHub to celebrate my coding history, I was determined to reverse
engineer my sole remaining compiled binary to reconstruct an
equivalent source code.  I took care to compare with sample source
code files that I was pretty confident to have copied from, and I
carefully adjusted the resulting source to be as consistent as
possible with my historic coding style in relation to whitespace and
comments.  I've also put together a small toolset for assisting in the
reverse engineering process, which is located in the `disasm`
directory of repo.

Finally, for fun, I went back and made numerous improvements to the
original source code for the latest version up on GitHub.
