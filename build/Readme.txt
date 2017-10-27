Build files are placed in this directory.

If you want to share built executables / libraries, please build the
misleadingly named "install" target and share the files placed in the
`install/` directory (or the directory you set via `CMAKE_INSTALL_PREFIX`).

Git ignores files in this directory by default unless forced to do so
(`git add -f build/`)
