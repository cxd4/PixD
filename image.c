/******************************************************************************\
* Project:  Graphics Library Interface for Pixel Transfers                     *
* Authors:  Iconoclast                                                         *
* Release:  2016.01.27                                                         *
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

#include <stdio.h>
#include "image.h"
#include "state.h"

static GLuint texture_name;

GLfloat texture_raster[4][2] = {
    { 0, 1, }, /* DirectX origin */
    { 1, 1, },
    { 0, 0, },
    { 1, 0, }, /* terminal scanline and pixel */
};
GLshort texture_vector[4][2] = {
    { -1, -1, }, /* OpenGL origin */
    { +1, -1, },
    { -1, +1, },
    { +1, +1, }, /* terminal trigonometric vertex */
};

void display(void)
{
    const GLvoid * data;
    GLenum status, format, type;
    const GLsizei width = (GLsizei)viewport[2];
    const GLsizei height = (GLsizei)viewport[3];
    const GLuint limit = width * height;

    glClear(GL_COLOR_BUFFER_BIT);
    glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);

/*
 * OpenGL-ES-compatible default settings
 * also to fix potentially uninitialized variable warnings from the switch
 */
    data = (const GLvoid *)pixels;
    type = GL_UNSIGNED_BYTE;
    format = GL_RGBA;

/*
 * Clamp the byte offset from the first pixel of the binary data.
 * This is to prevent memory access violations or segmentation faults.
 *
 * A negative byte offset can be a segfault when array[-1] isn't mapped.
 * It can also be too high, when GL starts uploading pixels outside the file.
 */
    if (8*byte_offset > 8*file_size - width*height*bits_per_pixel)
        byte_offset = file_size - width*height*bits_per_pixel/8;
    if (byte_offset < 0)
        byte_offset = 0;

    format = (BGR_ordering != GL_FALSE) ? GL_BGRA : GL_RGBA;
    type = GL_UNSIGNED_BYTE;
    data = (const GLvoid *)(file_data + byte_offset);
    switch (bits_per_pixel)
    {
        register unsigned int i;
    case 1:
        for (i = 0; i < limit; i++)
            pixels[i] = (file_data[byte_offset + i/8] >> ~i%8 & 1) ? ~0 : 0;
        format = GL_LUMINANCE;
        data = (const GLvoid *)pixels;
        break;
    case 2:
        for (i = 0; i < limit; i++)
            pixels[i] = 85 * (file_data[byte_offset + i/4] >> ~i%4*2 & 3);
        format = GL_LUMINANCE;
        data = (const GLvoid *)pixels;
        break;
    case 4:
        for (i = 0; i < limit; i++)
        {
            const unsigned shift = 4 * (~i & 1);
            const unsigned nybble = file_data[byte_offset + i / 2] >> shift;
            const int set_R = nybble & 0x4;
            const int set_G = nybble & 0x2;
            const int set_B = nybble & 0x1;
            const int set_I = (nybble >> 3); /* intensity or lightness */

            pixels[4*i + RED]  = (set_R != 0) ? 0xFFu : 0x00u;
            pixels[4*i + GRN]  = (set_G != 0) ? 0xFFu : 0x00u;
            pixels[4*i + BLU]  = (set_B != 0) ? 0xFFu : 0x00u;

            pixels[4*i + RED] &= (set_I & 1) ? 0xFFu : 0x80u;
            pixels[4*i + GRN] &= (set_I & 1) ? 0xFFu : 0x80u;
            pixels[4*i + BLU] &= (set_I & 1) ? 0xFFu : 0x80u;

            pixels[4*i + CVG]  = (GLubyte)0xFFu;
/*
 * 4-bit color has no useful transparency, but the mathematical property that
 * draws the same color for (R = G = B = I = 0) as (R = G = B = 0, I = 1)
 * does tempt the implementation of a unique, special color for nybble 0x8,
 * such as the transparent pixel or, as Windows VGA does it, "silver".
 */
#ifdef _WIN32
            if (nybble != 0x8)
                continue;
            pixels[4*i + BLU] = pixels[4*i + GRN] = pixels[4*i + RED] = 192;
#endif
        }
        data = (const GLvoid *)pixels;
        break;
    case 8:
/*
 * There are several legitimate ways to encode colors into 8-bit storage.
 * Perhaps the most popular one is the R3G3B2 model that Microsoft got SGI to
 * implement starting in OpenGL 1.2, but many other systems use other ways.
 * For example, instead of R3G3B2, why not R2G2B2A2, I8, or R2G2B2I2?
 *
 * Actually, the most fundamental 8-bit color mode is I8, so we support that.
 * The Windows VGA 256-color palette might not find it friendly, but even
 * less friendly does OpenGL ES or OpenGL 1.1 find Microsoft's method over
 * the more naturally applicable I8 method.
 */
        format = GL_LUMINANCE;
        break;
    case 16:
/*
 * Both OpenGL 1.2 and all versions of OpenGL ES support three universally
 * accepted 16-bit color modes in OpenGL:  R5G5B5, R5G6B5, and R4G4B4A4.
 * It is interesting that Microsoft GDI handles these very three formats,
 * but the one that SGI used in their media co-processor for the Nintendo 64
 * was R5G5B5[X1], with a final bit serving as part of 3-bit coverage.
 */
        type = BGR_ordering
            ? GL_UNSIGNED_SHORT_1_5_5_5_REV : GL_UNSIGNED_SHORT_5_5_5_1;
        glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);
        if (channels[CVG] != GL_FALSE)
        {
            format = GL_LUMINANCE_ALPHA;
            type = GL_UNSIGNED_BYTE;
        }
        break;
    case 32:
        break;
    case 64:
        type = GL_UNSIGNED_SHORT; /* unsupported on OpenGL ES */
        break;
    case 128:
        type = GL_UNSIGNED_INT; /* unsupported on OpenGL ES */
        break;
    }

#if defined(GL_VERSION_1_0) & !defined(GL_VERSION_1_1)
    glRasterPos2i(-1, -flip_sign);
    glPixelZoom(+1.f, flip_sign);
    glDrawPixels(width, height, format, type, data);
#else
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, data);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3 + 1);
#endif
    glFlush();
    glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);

    status = glGetError();
    if (status != GL_NO_ERROR)
        DisplayGLError("Failed to draw pixels.", status);
    return;
}

void reshape(int w, int h)
{
    GLenum status;
    GLsizei width, height, texture_size;

    width = (GLsizei)w;
    height = (GLsizei)h;

/*
 * To maintain hardware-accelerated pixel uploads, we must maintain that the
 * desired screen width and height do not exceed the texture size limitation
 * of the current OpenGL implementation.  The OpenGL specifications demand a
 * limit of at least 64x64 pixels, but the plausibility of an implementation
 * of OpenGL, even in pure software, supporting less than 1024x1024 seems to
 * have existed in a time before any of us had public internet access.
 */
    if (width > texture_limit)
        width = texture_limit;
    if (height > texture_limit)
        height = texture_limit;

/*
 * If the frame buffer size, in bits, is greater than the file size, in bits,
 * then clamp it down or whatever.  My current preference is change nothing.
 * Some other ideas I had in prototypes of this software were to clamp the
 * width to the previous power of 2, then keep decrementing the height by 1
 * for as long as the dimensions together were still too great, but there
 * were some other viable strategies that I didn't want to decide between.
 */
    if (width * height * bits_per_pixel > 8 * file_size)
    {
        width = viewport[2]; /* restoring old width */
        height = viewport[3]; /* restoring old height */
    }
    glutReshapeWindow(width, height); /* confirmed new width and height */

    texture_size = (width < height) ? height : width;
    if (texture_size >= texture_limit)
        texture_size  = texture_limit;
    else
    {
        signed long search_mask;

/*
 * Bump the texture size to the next power of two, unless equal to one.
 * A width of 4095 would become 4096; a width of 4096 stays at 4096.
 */
        for (search_mask = 1; search_mask <= 0x40000000l; search_mask *= 2)
        {
            if (search_mask < texture_size)
                continue;
            else if (search_mask == texture_size) /* already a power of 2 */
                break;
            else
            {
                texture_size = search_mask;
                break;
            }
        }
    }

    free(pixels);
    pixels = malloc(4 * width * height);

    glViewport(0, 0, width, height);
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (viewport[2] != width || viewport[3] != height)
        fputs("Context viewport state mismatch!\n", stderr);

#if !defined(GL_VERSION_1_0) | defined(GL_VERSION_1_1)
    glDeleteTextures(1, &texture_name);
    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        texture_size,
        texture_size,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL
    );

    texture_raster[3][0] = texture_raster[1][0]
      = (GLfloat)width / (GLfloat)texture_size;
    texture_raster[1][1] = texture_raster[0][1]
      = (GLfloat)height / (GLfloat)texture_size;

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif
    glutPostRedisplay();
    status = glGetError();
    if (status != GL_NO_ERROR)
        DisplayGLError("Failed to rebound texture.", status);
    return;
}
