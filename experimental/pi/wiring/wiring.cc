#include "wiring/wiring.h"

#include "wiring/wiringPi.h"

void SetupWiringIfNeeded() {
  static std::once_flag flag;
  std::call_once(flag, [] { wiringPiSetup(); });
}
