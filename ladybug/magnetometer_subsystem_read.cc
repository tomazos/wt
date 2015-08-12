#include "main/noargs.h"
#include "ladybug/magnetometer_subsystem.h"

void Main() {
  MagnetometerSubsystem m;
  Mutex mutex;
  float64 mx = 0, my = 0, mz = 0;
  int64 nupdates = 0;
  m.AddListener([&](float64 x, float64 y, float64 z) {
    LockGuard l(mutex);
    mx = x;
    my = y;
    mz = z;
    nupdates++;
  });

  while (true) {
    Sleep(1s);
    LockGuard l(mutex);

    DUMPEXPR(nupdates);
    nupdates = 0;
    DUMPEXPR(mx);
    DUMPEXPR(my);
    DUMPEXPR(mz);
  }
}
