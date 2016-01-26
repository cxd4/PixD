/******************************************************************************\
* Project:  Graphics Library Interface                                         *
* Authors:  Iconoclast                                                         *
* Release:  2016.01.26                                                         *
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

#ifndef _IMAGE_H_
#define _IMAGE_H_

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

extern GLfloat texture_raster[4][2];
extern GLshort texture_vector[4][2];

extern void display(void);
extern void reshape(int w, int h);

#endif
