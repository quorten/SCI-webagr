So, TO RECONSTRUCT THE PROGRAM.

It is rather simple.

First of all, you create the main agreement window with the desired
class.  That is, non-resizeable modal dialog with NOCLOSE button,
minimize, maximize, or any of that.  You also need to register a
custom window class for the text area.

The main window process needs to handle keyboard input, but we
simplify this by only handling one key rather than full dialog
keyboard handling: enter always accepts.

The text window, it solely has a `WM_PAINT` handler to draw the
watermark bitmap and `DrawText()` Unicode (no word wrap).  In the very
earliest version of the program, embed the agreement string in the
source code itself.  Load the bitmap as resource "#101" as a string,
no `resource.h` or anything like that.  And that is the sole resource
in the first very earliest version.  The later versions also use a
string table.

After accept, you destroy the agreement window, then `AllocConsole()`.
The installer window simply uses a busy-waiting loop with
`timeGetTime()` to update its status after indicated elapsed times.
(Ha, well that will rev the CPU fans to make the computer sound busy
doing system modifications, like it would be.)  `WriteFile()` is used
to write ASCII string messages to the console, and yes we use Windows
Console API.  Also, we register the close console handler to prevent
the user from trying to stop the install.

After the countdown finishes, we use `ShellExecute()` to open Spy
Corp. International.

Use Spy++ and take screenshots to verify measurements.  Compile
develop using Microsoft Visual C++ 6.0.

Spacing metrics: 16 pixel border between text area and and dialog
edge.

So, the FULL DIMENSIONS:

* Main window (x, y, w, h): 145, 120, 770, 520
* spytxt window (x, y, w, h): 20, 20, 725, 400

* Main window styles: `WS_CAPTION | WS_VISIBLE | WS_CLIPSIBLINGS |
  WS_CLIPCHILDREN | WS_OVERLAPPED | WS_EX_LEFT | WS_EX_LTRREADING |
  WS_EX_RIGHTSCROLLBAR | WS_EX_WINDOWEDGE`

  `CS_PARENTDC | CS_HREDRAW | CS_VREDRAW`  
  `IDC_ARROW`  
  `BKGND BRUSH 0x79E (BTNFACE?)`

* spytxt window styles: `WS_CHILDWINDOW | WS_VISIBLE | WS_OVERLAPPED
  WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_CLIENTEDGE`

  `CS_HREDRAW | CS_VREDRAW`  
  `IDC_ARROW`  
  `BKGND BRUSH COLOR_MENU`

```
"BUTTON", WS_CHILDWINDOW | WS_VISIBLE | BS_DEFPUSHBUTTON
"I Decline" 110, 450, 80, 28
"I Accept" 15, 450, 80, 28
"BUTTON",  WS_CHILDWINDOW | WS_VISIBLE | BS_PUSHBUTTON | BS_TEXT
```

In source code, just use `WS_CHILD`.

The Spy Logo bitmap, draw that with offset 0, 325.  Use LoadBitmap()
even though it is deprecated.

The font is height = 18, face = "MS SANS SERIF".  All other options
are default zero (0) to the `CreateFont()` function call.

```
DrawText rect = { 5, 5, 720, 400 }
```

When creating the "BUTTON" windows, set `windowName` to NULL.  We
initialize it later using `WM_SETTEXT`.

```
WM_SETFONT:
1. SendMessage: declWin, uMsg = 0x30, wParam = ptr, lParam = 0
2. SendMessage: accpWin, uMsg = 0x30, wParam = ptr, lParam = 0
WM_SETTEXT:
3. SendMessage: declWin, uMsg = 0x0c, wParam = 0, lParam = 0x004261b8
4. SendMessage: accpWin, uMsg = 0x0c, wParam = 0, lParam = 0x004261ac
```

Again, wide character strings are loaded from a string table resource.
To where?  To a global variable, for sure.  A table of strings, each
string has buffer space for 1024 bytes max.

Or wait... that's just our stack variable that we copy to, then we
copy again to elsewhere?  Let's see.

Yeah, it is copied elsewhere.

Global variable 0x004299e0 is a data structure with the following
fields:

DWORD val1; // always 0xcc = 204, except for last which is zero
char *msgstr;
DWORD msglen;
DWORD capacity; // buffer allocation size, dynamically expandable

Okay, okay, so maybe this is some strdup() internal structure or
something?  All of the data is set by a single function call that is
coming from libc it looks like.

More info about the string table.  Looks like I've written a small
suite of helper functions to manage sub-tasks in book-keeping.

1. Add a new string to the table.
2. ??? helper function for new string
3. Get a string length by index ptr.
4. Get a string text by index ptr.

Problem solved!  After some careful thought about the programming
patterns and my programming knowledge at the time, the answer became
obvious.  I am using an array of C++ strings, simple as that.  Indeed,
I've replicated this in my source code rewrite, and the behavior is
fully consistent.

This simply means I am using a helper function for loading the string
table, and everything else is a C++ member function, and the global
constructors/destructors hooks are what are initializing the 0xcc
magic number.

Alas, we have 12 elements in the array but count from one, overflowing
the allocation by one element.  AND, the global hFont variable follows
almost immediately, with only but some grace padding to save it.

Global "stage" variable comes before strTable.

----------

Please note: ALL of the wide character strings, including the
agreement text, come from the string table.  Maybe we also even a
table for the time durations in the Spy Corp. installer?

The agreement text is the first string, the the remainder follow in
sequence.

With all the times we call "GetWindowLong()", do we actually do at lot
more computation of window dimensions in source code?  I believe that
must be so.  We never use `GetWindowRect()` though.  Maybe not on the
arithmetic computation... the left and right margins are not equal.
Maybe I guess visually guessed and analyzed the whole way through.

PLEASE NOTE: `GetWindowLong()` is used for hInstance, and we
copy-paste code inefficiently at that.  Nevertheless, now I do recall
that I wrote arithmetic computation into the code for the window
placement.
