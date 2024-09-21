#!/bin/bash
# Build script for the engine
set echo on

mkdir -p ../bin

# Get a list of all the C files.
cFilenames=$(find . -type f -name "*.c")

# echo "Files:" $cFilenames

assembly="FlashlightEngine"
compilerFlags="-g -shared -fdeclspec -fPIC"
includeFlags="-ISource -I$VULKAN_SDK/include"
linkerFlags="-lvulkan -lxcb -lX11 -lX11-xcb -lxkbcommon -L$VULKAN_SDK/lib -L/usr/X11R6/lib"
defines="-DFL_DEBUG -DFL_BUILD"

echo "Building $assembly..."
echo clang $cFilenames $compilerFlags -o ../bin/lib$assembly.so $defines $includeFlags $linkerFlags
clang $cFilenames $compilerFlags -o ../bin/lib$assembly.so $defines $includeFlags $linkerFlags