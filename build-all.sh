#!/bin/bash
# Build everything
set echo on

echo "Building everything..."

pushd Flashlight
source build.sh
popd

ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
    echo "Error:"$ERRORLEVEL && exit
fi

pushd TestBed
source build.sh
popd

ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ] 
then
    echo "Error:"$ERRORLEVEL && exit
fi

echo "All assemblies built successfully."