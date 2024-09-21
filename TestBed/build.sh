#!/bin/bash
# Build script for the testbed
set echo on

mkdir -p ../bin

# Get a list of all the C files.
cFilenames=$(find . -type f -name "*.c")

# echo "Files:" $cFilenames

assembly="testbed"
compilerFlags="-g -fdeclspec -fPIC"
includeFlags="-ISource -I../Flashlight/Source"
linkerFlags="-L../bin/ -lFlashlightEngine -Wl,-rpath,."
defines="-DFL_DEBUG"

echo "Building $assembly..."
echo clang $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags
clang $cFilenames $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags