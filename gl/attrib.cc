#include "gl/attrib.h"

namespace gl {

Attrib::Attrib(const string& name, GLuint location)
    : name_(name), location_(location) {}

}  // namespace gl
