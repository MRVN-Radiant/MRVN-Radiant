#pragma once

#include "igl.h"

void gl_shader_create( GLuint program, const char* filename, GLenum type );
void gl_program_link( GLuint program );
void gl_program_validate( GLuint program );