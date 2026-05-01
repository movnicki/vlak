::@echo off

pushd bin
cl /I../inc ../src/vlak_main.c /link /LIBPATH:../lib freetype.lib User32.lib Gdi32.lib /SUBSYSTEM:CONSOLE
popd
