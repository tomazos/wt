#include "main/args.h"

#include <sqlite3.h>

#include "audio/audio_functions.h"
#include "core/must.h"
#include "database/connection.h"
#include "database/statement.h"

void Main(const std::vector<string>& args) {
  if (args.size() < 1) FAIL("usage: get_speechtext <id> <written|spoken>");
  const int64 id = stoll(args[0]);
  const string column = args[1];
  MUST(column == "written" || column == "spoken");

  std::string query = "select " + column + " from speechtext where id = ?";

  database::Connection db("/data/speechtext.db", SQLITE_OPEN_READONLY);

  database::Statement select = db.Prepare(query);

  select.BindInteger(1, id);
  MUST(select.Step());

  if (column == "written") {
    string written = select.ColumnText(0).to_string();
    std::cout.write(written.data(), written.size());
    std::cout.flush();
  } else {
    string spoken = select.ColumnBlob(0).to_string();
    std::cout.write(spoken.data(), spoken.size());
    std::cout.flush();
  }
}
