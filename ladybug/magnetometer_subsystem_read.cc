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
    Sleep(10ms);
    LockGuard l(mutex);

    LOGEXPR(nupdates);
    nupdates = 0;
    LOGEXPR(mx);
    LOGEXPR(my);
    LOGEXPR(mz);
  }
}
