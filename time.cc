#include "time.h"

#include <sys/time.h>

#include "must.h"

TimePoint TimePoint::Now() {
  timeval tv;
  const int gettimeofday_result = gettimeofday(&tv, NULL);
  MUST_EQ(0, gettimeofday_result);
  return TimePoint(TimeDelta::Seconds(tv.tv_sec) +
                   TimeDelta::Microseconds(tv.tv_usec));
}
