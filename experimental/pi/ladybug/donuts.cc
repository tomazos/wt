#include "ladybug/ladybug.h"
#include "main/noargs.h"

void Main() {
  LadyBug& ladybug = LadyBug::Instance();
  ladybug.SetLeftWheelPower(0.6);
  ladybug.SetRightWheelPower(-0.6);
  std::getchar();
}
