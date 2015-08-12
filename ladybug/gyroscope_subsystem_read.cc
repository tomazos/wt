#include "main/noargs.h"

#include "ladybug/gyroscope_subsystem.h"

void Main() {
  GyroscopeSubsystem g;
  Mutex mutex;
  float64 gx = 0, gy = 0, gz = 0;
  int64 nupdates = 0;
  g.AddListener([&](float64 x, float64 y, float64 z) {
    LockGuard l(mutex);
    gx = x;
    gy = y;
    gz = z;
    nupdates++;
  });

  while (true) {
    Sleep(10ms);
    LockGuard l(mutex);

    DUMPEXPR(nupdates);
    nupdates = 0;
    DUMPEXPR(gx);
    DUMPEXPR(gy);
    DUMPEXPR(gz);
  }
}
