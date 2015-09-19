@echo off
set PATH=d:\mingw\bin
redir -e 1 -o 2 mingw32-make -f Makefile.mgw X86=1
