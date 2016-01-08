#include "main/noargs.h"

#include <sqlite3.h>

#include "core/must.h"
#include "database/sqlite/connection.h"
#include "database/sqlite/statement.h"

using database::sqlite::Connection;
using database::sqlite::Statement;

void Main() {
  Connection olddb("/data/speechtext.db", SQLITE_OPEN_READONLY);
  Connection newdb("/data/speechtext_shuffled.db");

  constexpr size_t nsamples = 1948919;
  std::vector<int64> shuffled_indexes(nsamples + 1);
  for (size_t i = 1; i <= nsamples; i++) shuffled_indexes[i] = i;
  std::random_shuffle(shuffled_indexes.begin() + 1, shuffled_indexes.end());

  Statement select = olddb.Prepare("select rowid, text, wave from speechtext");
  newdb("begin");
  Statement insert = newdb.Prepare(
      "insert into speechtext (id, written, spoken) values (?,?,?)");

  while (select.Step()) {
    int64 rowid = select.ColumnInteger(0);
    string text = select.ColumnText(1).to_string();
    string wave_bytes = select.ColumnBlob(2).to_string();
    MUST_GE(rowid, 1);
    MUST_LE(rowid, int64(nsamples));
    int64 new_rowid = shuffled_indexes[rowid];
    insert.BindInteger(1, new_rowid);
    insert.BindText(2, text);
    insert.BindBlob(3, wave_bytes);
    insert.Execute();
    insert.Reset();
  }
  newdb("commit");
}
