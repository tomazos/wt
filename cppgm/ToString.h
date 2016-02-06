#pragma once

template <class T>
string ToString(const T& t) {
  ostringstream oss;
  t.write(oss);
  return oss.str();
}
