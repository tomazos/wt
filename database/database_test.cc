#include "database/connection.h"

#include <boost/filesystem.hpp>

#include "core/env.h"
#include "core/file_functions.h"
#include "core/must.h"
#include "gtest/gtest.h"

namespace database {

TEST(Sqlite3Test, Connection) {
  filesystem::path source_root = GetEnv("SOURCE_ROOT");
  filesystem::path english_words_path =
      source_root / "database/testdata/english-words.txt";

  filesystem::path tmpfile =
      filesystem::temp_directory_path() / filesystem::unique_path();

  {
    Connection db(tmpfile);

    db(R"(
      create table words(

        word string
      )
    )");

    db("begin");
    Statement insert = db.Prepare("insert into words (word) values (?)");
    Scan(GetFileContents(english_words_path), "\n", [&](string_view word) {
      insert.BindText(1, word);
      insert.Execute();
      insert.Reset();
    });
    db("end");

    Statement select = db.Prepare(R"(
      select word
      from words
      where word like '%oo%'
      order by length(word) desc
      limit 10
    )");
    while (select.Step()) {
      EXPECT_GT(select.ColumnText(0).size(), 7u);
    }
  }

  filesystem::remove(tmpfile);
}

}  // namespace database
