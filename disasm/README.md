Windows Binary Reverse Engineering Tools
========================================

This is the simple toolset I put together to reverse engineer my sole
remaining Windows binary of the Spy Corp. International Website Access
Agreement program.  I've also included the notes I've taken from the
reverse engineering process.  Note that I have **not** included the
original binary since it is a Windows binary and, keeping with the
nostalgia of the time, those were always suspect for potentially
containing viruses in Internet distribution.  Plus, it's not source
code anyways.

How to use?  To analyze a Windows binary, call the Makefile via
`make`, and make sure your windows binary is named `webagr.exe`.

The following key artifacts will be produced:

* An annotated disassembly will be produced in the file `fnwebagr.s`
  by analyzing the DLL import table and replacing all DLL function
  calls with their import table function names, thus greatly aiding
  comprehension of Windows source code that is Windows API intensive.

* A listing of only DLL function calls in ascending order from the
  binary source location, in the file `calls.txt`.  This is extremely
  useful if you are analyzing code that mainly makes Windows API
  function calls, and an experienced Windows programmer can often
  quickly identify which publicly available Windows sample source code
  files were copied to develop the program in question just by looking
  at this listing.

* The read-only data `.rdata` section will be analyzed for C strings
  one character long or greater and the results will be stored in
  `c_str.txt`.

* The Windows resources will also be decompiled from the PE32 resource
  format to the 16-bit RES format `webagr.res`.  This resource file
  can then be opened up directly in Microsoft Visual Studio C++ 6.0
  and the contents can then be exported and copied over to a resource
  script file.
