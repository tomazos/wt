#include "gl/sampler.h"

#include "core/must.h"
#include "gl/error.h"

namespace gl {

namespace {

auto create_sampler = [] {
  GLuint name = 0;
  glGenSamplers(1, &name);
  ThrowIfError();
  return name;
};

auto delete_sampler = [](GLuint name) { glDeleteSamplers(1, &name); };

}  // namespace

Sampler::Sampler() : sampler_(create_sampler(), delete_sampler) {}

void Sampler::Bind(GLuint unit) {
  glBindSampler(unit, sampler_);
  ThrowIfError();
}

}  // namespace gl
