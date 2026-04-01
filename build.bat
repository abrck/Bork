@echo off

set NDK=D:\android-ndk-r28-ollvm
set BUILD_TYPE=Release
set TARGET=Solarland

if exist build rmdir /s /q build
mkdir build
cd build

cmake -G Ninja ^
-DANDROID_ABI=arm64-v8a ^
-DANDROID_PLATFORM=android-24 ^
-DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
-DCMAKE_TOOLCHAIN_FILE="%NDK%\build\cmake\android.toolchain.cmake" ^
..

@REM cmake --build -j8
cmake --build . --target %TARGET% -j8

cd ..
