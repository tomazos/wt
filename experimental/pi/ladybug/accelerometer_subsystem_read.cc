#include "main/noargs.h"

#include "ladybug/accelerometer_subsystem.h"

void Main() {
  AccelerometerSubsystem a;
  Mutex mutex;
  float64 ax = 0, ay = 0, az = 0;
  int64 nupdates = 0;
  a.AddListener([&](float64 x, float64 y, float64 z) {
    LockGuard l(mutex);
    ax = x;
    ay = y;
    az = z;
    nupdates++;
  });

  while (true) {
    Sleep(1s);
    LockGuard l(mutex);

    LOGEXPR(nupdates);
    nupdates = 0;
    LOGEXPR(ax);
    LOGEXPR(ay);
    LOGEXPR(az);
  }
}
