#include "audio/speechtext_visitor.h"

#include "core/must.h"

namespace audio {

void VisitSpeechText(SpeechTextVisitor& visitor, int64 begin_sample,
                     int64 nsamples, size_t nthreads) {
  MUST(nsamples % nthreads == 0);

  std::vector<std::thread> threads;

  int64 samples_per_thread = nsamples / nthreads;

  for (size_t i = 0; i < nthreads; i++) {
    std::thread t([&](size_t index) {
      try {
        int64 begin_block = index * samples_per_thread;
        int64 end_block = begin_block + samples_per_thread;

        database::Connection db("/data/speechtext.db", SQLITE_OPEN_READONLY);
        database::Statement select = db.Prepare(
            "select id-1, written, spoken from speechtext where ? <= (id-1) "
            "and "
            "id-1 "
            "< ? ");
        select.BindInteger(1, begin_block);
        select.BindInteger(2, end_block);
        while (select.Step()) {
          int64 id = select.ColumnInteger(0);
          string_view written = select.ColumnText(1);
          string_view spoken_bytes = select.ColumnBlob(2);
          Eigen::Map<const Wave> spoken((const int16*)spoken_bytes.data(),
                                        spoken_bytes.size() / 2);
          if (!visitor(id, written, spoken)) break;
        }
      } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        throw;
      }
    }, i);
    threads.push_back(std::move(t));
  }

  for (std::thread& t : threads) t.join();
}

}  // namespace audio
