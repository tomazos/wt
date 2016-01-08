#pragma once

#include <eigen/Eigen>
#include <sqlite3.h>

#include "database/sqlite/connection.h"
#include "database/sqlite/statement.h"

namespace audio {

using Wave = Eigen::Matrix<int16, Eigen::Dynamic, 1>;

class SpeechTextVisitor {
 public:
  virtual ~SpeechTextVisitor() = default;

  virtual bool operator()(int64 id, string_view written,
                          const Wave& spoken) = 0;
};

void VisitSpeechText(SpeechTextVisitor& visitor, int64 begin_sample,
                     int64 nsamples, size_t nthreads);

}  // namespace audio
