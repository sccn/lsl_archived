These two files are included to allow the liblsl library to export the same C++ binary interface as in previous versions. This allows the library to be copied into application installations that currently use an older version without the need for recompiling.

This older C++ ABI has been superseded by a new one that wraps the C interface, which
does not suffer from compiler compatibility issues as much, particularly on Windows.