rem Build script for the testbed
@echo off
SetLocal EnableDelayedExpansion

rem Create the bin folder if it doesn't already exists
if not exist "../bin" mkdir "../bin"

rem Get a list of the C source file
set cFilenames =
for /R %%f in (*.c) do (
    set cFilenames=!cFilenames! %%f
)

rem echo "Files:" %cFilenames%

set assembly=testbed
set compilerFlags=-g
set includeFlags=-ISource -I../Flashlight/Source
set linkerFlags=-lFlashlightEngine.lib -L../bin/
set defines=-DFL_DEBUG

echo "Building %assembly%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%