#include "math/fft.h"

#include "core/must.h"
#include "gtest/gtest.h"

TEST(FFTTest, Basic) {
  math::FFT<float64> fft(100, false /*inverse*/);

  std::vector<std::complex<float64>> in(100);
  for (size_t i = 0; i < 100; i++) in[i] = std::sin((i / 10.0) * 2 * M_PI);

  std::vector<std::complex<float64>> out(100);
  fft.transform(in.data(), out.data());
  for (size_t i = 0; i < 100; i++)
    std::cout << i << "\t" << out[i] << "\t" << abs(out[i]) << std::endl;
}
