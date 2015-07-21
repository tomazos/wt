#pragma once

class TimeDelta {
 public:
  static TimeDelta Microseconds(int64 microseconds) {
    return TimeDelta(microseconds);
  }
  static TimeDelta Milliseconds(int64 milliseconds) {
    return Microseconds(milliseconds * 1000);
  }
  static TimeDelta Seconds(int64 seconds) {
    return Milliseconds(seconds * 1000);
  }
  static TimeDelta Minutes(int64 minutes) { return Seconds(minutes * 60); }
  static TimeDelta Hours(int64 hours) { return Minutes(hours * 60); }
  static TimeDelta Days(int64 days) { return Hours(days * 24); }
  static TimeDelta Weeks(int64 weeks) { return Days(weeks * 7); }

  string EncodeAsString() const {
    if (microseconds_ == 0) return "0 secs";

    int64 microseconds = microseconds_;
    string result;
    if (microseconds < 0) {
      result = "-";
      microseconds = -microseconds;
    }

    result += EncodeAsString(microseconds);

    if (microseconds_ < ) }

 private:
  TimeDelta(int64 microseconds) : microseconds_(microseconds) {}
  int64 microseconds_;
};

void EncodeAsString(TimeDelta delta) { if (delta.microseconds_ < 1000) }

class TimePoint {
 public:
  static TimePoint Epoch() { return TimeDelta::Microseconds(0); }
  static TimePoint Now();

  TimeDelta operator-(TimePoint that) { return TimePoint(since_epoch_ - that.since_epoch_;
  }

 private:
  TimePoint(TimeDelta since_epoch) : since_epoch_(since_epoch) {}

  TimeDelta since_epoch_;
};
