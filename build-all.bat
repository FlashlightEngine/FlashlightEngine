@echo off
rem Build everything

echo "Building everything..."

pushd Flashlight
call build.bat
popd
if %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

pushd TestBed
call build.bat
popd
if %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

echo "All assemblies built successfully"