#include "experimental/sdl/system.h"

namespace gl {

namespace {

template <typename Local, typename Getter, typename... Args>
Local Get(Getter getter, Args&&... args) {
  Local local;
  getter(std::forward<Args>(args)..., &local);
  return local;
}

}  // namespace

#define DEFN_GET_FUN(Type, FunctionStem, Return, Local)         \
  Return Get##Type(GLenum pname) {                              \
    return Get<Local>(glGet##FunctionStem##v, pname);           \
  }                                                             \
  Return Get##Type(GLenum target, GLuint index) {               \
    return Get<Local>(glGet##FunctionStem##i_v, target, index); \
  }

DEFN_GET_FUN(Bool, Boolean, bool, GLboolean)
DEFN_GET_FUN(Int32, Integer, int32, GLint)
DEFN_GET_FUN(Int64, Integer64, int64, GLint64)
DEFN_GET_FUN(Float32, Float, float32, GLfloat)
DEFN_GET_FUN(Float64, Double, float64, GLdouble)

void Enable(GLenum cap) { glEnable(cap); }
void Enable(GLenum cap, GLuint index) { glEnablei(cap, index); }
void Disable(GLenum cap) { glDisable(cap); }
void Disable(GLenum cap, GLuint index) { glDisablei(cap, index); }

}  // namespace gl
