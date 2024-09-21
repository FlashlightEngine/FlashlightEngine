rem Build script for the engine
@echo off
SetLocal EnableDelayedExpansion

rem Get a list of the C source file
set cFilenames =
for /R %%f in (*.c) do (
    set cFilenames=!cFilenames! %%f
)

rem echo "Files:" %cFilenames%

set assembly=FlashlightEngine
set compilerFlags=-g -shared -Wvarargs -Wall -Werror
set includeFlags=-ISource -I%VULKAN_SDK%/Include
set linkerFlags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib
set defines=-DFL_DEBUG -DFL_BUILD -D_CRT_SECURE_NO_WARNINGS

echo "Building %assembly%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.dll %defines% %includeFlags% %linkerFlags%