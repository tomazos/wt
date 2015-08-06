#include "rand_float.h"

#include <random>

auto CallRandomDevice() {
  static Mutex mutex;
  static std::random_device device;
  LockGuard l(mutex);
  auto result = device();
  return result;
}

class RandFloatGenerator {
 public:
  RandFloatGenerator() : mt19937_(CallRandomDevice()), distribution(0, 1) {}

  float64 operator()() { return distribution(mt19937_); }

 private:
  std::mt19937 mt19937_;
  std::uniform_real_distribution<float64> distribution;
};

float64 RandFloat() {
  thread_local RandFloatGenerator generator;
  return generator();
}
