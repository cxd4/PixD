@ECHO OFF

REM set target=i386
set target=amd64

set DDK=C:\WinDDK\7600.16385.1
set MSVC=%DDK%\bin\x86\%target%
set incl=/I"%DDK%\inc\crt" /I"%DDK%\inc\api"
set libs=/LIBPATH:"%DDK%\lib\crt\%target%" /LIBPATH:"%DDK%\lib\wnet\%target%"

set LINK_FLAGS=/OUT:PixD.exe %libs% kernel32.lib opengl32.lib glut32.lib
set src=%CD%
set C_FLAGS=/W4 /GL /O1 /Os /Oi /Ob1 /GS- /TC /Fa
set files=%src%\main.c %src%\state.c %src%\image.c

%MSVC%\cl.exe /MD %incl% %C_FLAGS% %files% /link %LINK_FLAGS%

pause
