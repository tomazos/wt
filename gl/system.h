#pragma once

#include "gl/opengl.h"

namespace gl {

bool GetBool(GLenum pname);
bool GetBool(GLenum target, GLuint index);

int32 GetInt32(GLenum pname);
int32 GetInt32(GLenum target, GLuint index);

int64 GetInt64(GLenum pname);
int64 GetInt64(GLenum target, GLuint index);

float32 GetFloat32(GLenum pname);
float32 GetFloat32(GLenum target, GLuint index);

float64 GetFloat64(GLenum pname);
float64 GetFloat64(GLenum target, GLuint index);

void Enable(GLenum cap);
void Enable(GLenum cap, GLuint index);
void Disable(GLenum cap);
void Disable(GLenum cap, GLuint index);

}  // namespace gl
