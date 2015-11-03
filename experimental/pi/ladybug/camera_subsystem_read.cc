#include "main/noargs.h"

#include <iostream>

#include "ladybug/camera_subsystem.h"

void Main() {
  CameraSubsystem c;
  uint8 k = 0;
  int64 count = 0;
  const auto start = Now();
  c.AddListener([&](uint8* data) {
    for (size_t i = 0; i < CameraSubsystem::kBytes / 100; i++) {
      k += data[i];
    }
    count++;
    if (!(count & (count - 1))) {
      const auto end = Now();
      float64 secs = (end - start) / 1s;
      std::cout << count << " " << secs << " " << (count / secs) << "Hz "
                << " " << int(k) << std::endl;
    }
  });

  std::getchar();
}
