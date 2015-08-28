/******************************************************************************\
* Project:  Pixel Map Disassembler                                             *
* Authors:  Iconoclast                                                         *
* Release:  2015.04.17                                                         *
* License:  CC0 Public Domain Dedication                                       *
*                                                                              *
* To the extent possible under law, the author(s) have dedicated all copyright *
* and related and neighboring rights to this software to the public domain     *
* worldwide. This software is distributed without any warranty.                *
*                                                                              *
* You should have received a copy of the CC0 Public Domain Dedication along    *
* with this software.                                                          *
* If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.             *
\******************************************************************************/

#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "state.h"

GLuint color_32;
GLclampf color_GL[4];

int main(int argc, char* argv[])
{
    FILE* file_in;
    int failure;

    file_in = fopen(argc < 2 ? "data.bin" : argv[1], "rb");
    if (file_in == NULL)
    {
        fputs("Failed to access data.\n", stderr);
        return -1;
    }

    failure = fseek(file_in, 0, SEEK_END); /* dependent on POSIX regulations */
    if (failure != 0)
    {
        fputs("Failed to set file stream position.\n", stderr);
        return -1;
    }

    file_size = ftell(file_in);
    if (file_size < 0)
    {
        fputs("Failed to get file stream position.\n", stderr);
        return -1;
    }

/*
 * For avoiding a memory access segfault when setting up an OpenGL viewport
 * initially reading in 64 * 64 pixels from the file data.
 * The program always starts in 8-bit-per-pixel video display mode.
 * (64 * 64 pixels) / (1 byte per pixel) = 4096 bytes minimum file size.
 */
    file_size = (file_size < 64*64/1) ? 4096 : file_size;

    file_data = malloc(file_size);
    if (file_data == NULL)
    {
        fputs("Failed to allocate storage for file.\n", stderr);
        return -1;
    }

    failure = fseek(file_in, 0, SEEK_SET);
    if (failure != 0)
    {
        fputs("Failed to recursor stream.\n", stderr);
        return -1;
    }

    if (fread(file_data, file_size, 1, file_in) != 1)
    {
        fputs("Failed to import data.\n", stderr);
        return -1;
    }

    while (fclose(file_in) != 0)
        fputs("Problem destroying file stream.\n", stderr);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_ALPHA);
    glutInitWindowSize(64, 64);
    glutCreateWindow("PixD");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texture_limit);
    channels[CVG] = channels[BLU] = channels[GRN] = channels[RED] = GL_TRUE;

    byte_offset = (argc >= 3) ? (GLint)strtol(argv[2], NULL, 16) : 0;
    bits_per_pixel = (argc >= 4) ? 1 << (argv[3][0] - '0') : 8;
    viewport[2] = (argc >= 5) ? atoi(argv[4]) : 256;
    viewport[3] = (argc >= 6) ? atoi(argv[5]) : 256;

    color_32 = (argc >= 7) ? (GLuint)strtoul(argv[6], NULL, 16) : 0x000000FF;
    color_GL[0] = (GLclampf)((GLubyte)(color_32 >> 24)) / 255.f;
    color_GL[1] = (GLclampf)((GLubyte)(color_32 >> 16)) / 255.f;
    color_GL[2] = (GLclampf)((GLubyte)(color_32 >>  8)) / 255.f;
    color_GL[3] = (GLclampf)((GLubyte)(color_32 >>  0)) / 255.f;

    if (bits_per_pixel > 128)
        bits_per_pixel = 128; /* 256-or-more-bit pixels not supported */
    if (color_32 == 0x000000FF)
        channels[CVG] = GL_FALSE;

    reshape(viewport[2], viewport[3]);
    glClearColor(color_GL[RED], color_GL[GRN], color_GL[BLU], color_GL[CVG]);
#if defined(GL_VERSION_1_0) & !defined(GL_VERSION_1_1)
    glDisable(GL_TEXTURE_2D);

    glRasterPos2i(-1, -flip_sign);
    glPixelZoom(+1.f, flip_sign);
#else
    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_BYTE, 0, texture_vector);
    glTexCoordPointer(2, GL_FLOAT, 0, texture_raster);
#endif

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key_press_ascii);
    glutSpecialFunc(key_press);

    glutMainLoop();
    free(file_data);
    return 0;
}
