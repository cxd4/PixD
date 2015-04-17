@ECHO OFF

set MinGW=C:/msys64/mingw64
set src=%CD%
set obj=%src%/obj

set FLAGS_C=-masm=intel -O3 -std=c89 -pedantic -ansi -Wall
set OBJ_LIST=%obj%/image.o %obj%/state.o %obj%/main.o

if not exist obj (
mkdir obj
)
cd %MinGW%/bin

echo Compiling C sources...
%MinGW%/bin/gcc.exe -S %FLAGS_C% -o %obj%/main.asm      %src%/main.c
%MinGW%/bin/gcc.exe -S %FLAGS_C% -o %obj%/state.asm     %src%/state.c
%MinGW%/bin/gcc.exe -S %FLAGS_C% -o %obj%/image.asm     %src%/image.c

echo Assembling compiled sources...
%MinGW%/bin/as.exe               -o %obj%/main.o        %obj%/main.asm
%MinGW%/bin/as.exe               -o %obj%/state.o       %obj%/state.asm
%MinGW%/bin/as.exe               -o %obj%/image.o       %obj%/image.asm

echo Linking assembled objects...
%MinGW%/bin/gcc.exe -o %src%/PixDebug.exe %OBJ_LIST% -lglut32 -lopengl32
%MinGW%/bin/strip.exe -o %src%/PixD.exe %src%/PixDebug.exe --strip-all

pause
