/*
 * GLAD OpenGL Loader Implementation
 * 
 * This file contains the single implementation of GLAD OpenGL loader.
 * All other files should only include GLAD headers without the implementation macro.
 */

#ifndef __APPLE__
#ifdef USE_GLES
#define GLAD_GLES2_IMPLEMENTATION
#include <gles2.h>
#else
#define GLAD_GL_IMPLEMENTATION
#include <gl.h>
#endif
#endif