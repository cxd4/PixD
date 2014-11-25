/******************************************************************************\
* Project:  Pixel Map Disassembler                                             *
* Authors:  Iconoclast                                                         *
* Release:  2014.11.25                                                         *
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

int main(int argc, char* argv[])
{
    FILE* file_in;
    int failure;

    file_in = fopen(argc < 2 ? "data.bin" : argv[1], "rb");
    if (file_in == NULL)
    {
        print_literal("Failed to access data.\n");
        return -1;
    }

    failure = fseek(file_in, 0, SEEK_END); /* dependent on POSIX regulations */
    if (failure != 0)
    {
        print_literal("Failed to set file stream position.\n");
        return -1;
    }

    file_size = ftell(file_in);
    if (file_size < 0)
    {
        print_literal("Failed to get file stream position.\n");
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
        print_literal("Failed to allocate storage for file.\n");
        return -1;
    }

    failure = fseek(file_in, 0, SEEK_SET);
    if (failure != 0)
    {
        print_literal("Failed to recursor stream.\n");
        return -1;
    }

    failure = (int)fread(file_data, file_size, 1, file_in);
    failure = (failure != 1); /* fread returns number of elements read:  1. */
    if (failure != 0)
    {
        print_literal("Failed to import data.\n");
        return -1;
    }

    while (fclose(file_in) != 0)
        print_literal("Problem destroying file stream.\n");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_ALPHA);
    glutInitWindowSize(64, 64);
    glutCreateWindow("PixD");

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glDisable(GL_BLEND);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texture_limit);
    channels[CVG] = channels[BLU] = channels[GRN] = channels[RED] = GL_TRUE;

#if defined(GL_VERSION_1_0) & !defined(GL_VERSION_1_1)
    glDisable(GL_TEXTURE_2D);

    glRasterPos2f(-1.f, -flip_sign);
    glPixelZoom(+1.f, flip_sign);
#else
    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(4, GL_FLOAT, 0, texture_vector);
    glTexCoordPointer(4, GL_FLOAT, 0, texture_raster);
#endif

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key_press_ascii);
    glutSpecialFunc(key_press);

    glutMainLoop();
    free(file_data);
    return 0;
}
