# ![Logo](/src/icons/image5_64.png) Include  Filter

A GUI for filtering including tracking files of C++ compilers.

All big 3 C++ Compilers support some kind of include tracking output, when compiling source code to a translation unit. M
For MSVC, it is the [`/showIncludes`](https://learn.microsoft.com/en-us/cpp/build/reference/showincludes-list-include-files) option, for clang/gcc it is [`-H` or `--trace-includes`](https://clang.llvm.org/docs/ClangCommandLineReference.html#cmdoption-clang-H).

The output can be very helpful tracking down some specific problems.

The output can be very large and, especially the MSVC output, the relation can be hard to track.
The main problem is tracking the include hierarchy. The output helps but what includes what at which level
can be cumbersome to untangle, especially on MSVC as they choosed to use the whitespace as the hierarchy depth indicator.

For example, which files includes `cstddef` (last line?). This is a shortened example for the Readme, it can get much messier very fast.
```
    Note: including file:   C:\libs\Qt\6.5.0\msvc2019_64\include\QtWidgets\QMainWindow
    Note: including file:    C:\libs\Qt\6.5.0\msvc2019_64\include\QtWidgets\qmainwindow.h
    Note: including file:     C:\libs\Qt\6.5.0\msvc2019_64\include\QtWidgets/qtwidgetsglobal.h
    Note: including file:      C:\libs\Qt\6.5.0\msvc2019_64\include\QtGui/qtguiglobal.h
    Note: including file:       C:\libs\Qt\6.5.0\msvc2019_64\include\QtCore/qglobal.h
    Note: including file:        C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.37.32822\include\type_traits
    Note: including file:         C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.37.32822\include\yvals_core.h
    Note: including file:          C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.37.32822\include\vcruntime.h
    Note: including file:           C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.37.32822\include\sal.h
    Note: including file:            C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.37.32822\include\concurrencysal.h
    Note: including file:           C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.37.32822\include\vadefs.h
    Note: including file:          C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.37.32822\include\xkeycheck.h
    Note: including file:         C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.37.32822\include\cstddef
```


This GUI program can read the output and presents the hierarchy in a treeview.

You can filter the tree view and by presseing "enter", the program jumps to the first occurance of the finding.

## Supported Formats and Reading Options
Supported formats are MSVC and clang/gcc.
Either read it from a file or copy the compiler output in the clipboard + paste it in the IncludeFilter GUI program.

The output also don't have to be complete, only a subset is required.

# License
[GPLv3 License](./LICENSE) © Matthias Möller. Made with ❤ in Germany.