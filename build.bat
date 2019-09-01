@echo off
set PATH=C:\dev\mingw-w64\mingw64\bin;%PATH%

set COMPILING_FLAG=-D_DEBUG -g -fno-access-control -fno-exceptions -fno-rtti -L"C:/dev/supportlibs64/lib" -I"C:/dev/supportlibs64/include" -std=c++11 -lpthread -lm -lmingw32

rem g++ gamecode.cpp -shared %COMPILING_FLAG% -lSDL2 -o gamecode.dll
g++ main.cpp -mwindows %COMPILING_FLAG% -lSDL2main -lSDL2 -o protowork.exe