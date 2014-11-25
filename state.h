/******************************************************************************\
* Project:  Graphics Library State Machine                                     *
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

#ifndef _STATE_H_
#define _STATE_H_

#ifdef _WIN32
#include <windows.h>

#define HANDLE_STDOUT       GetStdHandle(STD_OUTPUT_HANDLE)

static DWORD bytes_written;

#define print_literal(str_lit)  \
    WriteFile(HANDLE_STDOUT, str_lit, sizeof(str_lit) - 1, &bytes_written, NULL)
#else
#define print_literal(str_lit)  \
    fputs(str_lit, stdout)
#endif

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>

enum {
    RED,
    GRN,
    BLU,
    CVG, /* inverse-transparency, coverage opacity..Greeks call this "alpha"? */
    LUM /* brightness--luminesence in OpenGL, lightness in the HSL model */
};

extern const char* GL_errors[8];

extern GLboolean channels[4];
extern GLint viewport[4];
extern GLubyte * pixels;

extern GLfloat flip_sign;
extern GLboolean BGR_ordering;
extern GLubyte bits_per_pixel;

extern GLubyte * file_data;
extern GLint byte_offset;
extern long file_size;

extern GLint texture_limit;

extern void DisplayGLError(char * text, GLenum status);
extern void key_press(int key, int x, int y);
extern void key_press_ascii(unsigned char key, int x, int y);

#endif
