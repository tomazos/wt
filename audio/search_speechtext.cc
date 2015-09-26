#include "main/args.h"

#include <sqlite3.h>

#include "audio/audio_functions.h"
#include "core/must.h"
#include "database/connection.h"
#include "database/statement.h"

void Main(const std::vector<string>& args) {
  if (args.size() < 1) FAIL("usage: search_speechtext <query>");
  std::string query = "select id, written, spoken from speechtext";

  for (const string& arg : args) {
    query += " " + arg;
  }
  std::cout << query << std::endl;

  database::Connection db("/data/speechtext.db", SQLITE_OPEN_READONLY);

  database::Statement select = db.Prepare(query);

  while (select.Step()) {
    int64 id = select.ColumnInteger(0);
    string written = select.ColumnText(1).to_string();
    string spoken = select.ColumnBlob(2).to_string();

    MUST_EQ(spoken.size() % 2, 0u);
    const int16* begin_wave = (const int16*)spoken.data();
    const int16* end_wave = begin_wave + spoken.size() / 2;
    if (begin_wave > end_wave) continue;

    std::cout << id << ". " << written << std::endl;
    audio::PlaySound(begin_wave, end_wave);
  }
}
