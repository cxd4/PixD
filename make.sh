mkdir -p obj

src="."
obj="$src/obj"

OBJ_LIST="\
    $obj/image.o \
    $obj/state.o \
    $obj/main.o"
FLAGS_ANSI="\
    -Wall \
    -ansi \
    -pedantic"
FLAGS_x86="\
    -masm=intel \
    -march=native \
    -Wall \
    -ansi \
    -pedantic \
    -std=c89"
C_FLAGS=$FLAGS_x86 # default since Intel SIMD was the most tested

echo Compiling C source code...
cc -S -Os $C_FLAGS -o $obj/main.s       $src/main.c
cc -S -Os $C_FLAGS -o $obj/state.s      $src/state.c
cc -S -O3 $C_FLAGS -o $obj/image.s      $src/image.c

echo Assembling compiled sources...
as -o $obj/main.o       $obj/main.s
as -o $obj/state.o      $obj/state.s
as -o $obj/image.o      $obj/image.s

echo Linking assembled object files...
gcc -s -o ./PixD $OBJ_LIST -lglut -lGL
