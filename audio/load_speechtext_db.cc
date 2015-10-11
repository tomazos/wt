#include "main/noargs.h"

#include "audio/srtproto_file.pb.h"
#include "core/must.h"
#include "core/sequence_file.h"
#include "database/connection.h"
#include "database/statement.h"

void Main() {
  SequenceReader reader("/data/all.speechtext");
  database::Connection db("/data/speechtext.db");

  db("begin");
  database::Statement insert =
      db.Prepare("insert into speechtext (text,wave) values (?,?)");
  int64 total = 0;
  while (true) {
    audio::SpeechText speechtext;
    if (!reader.ReadMessage(speechtext)) break;

    total++;

    if ((total & (total - 1)) == 0)
      std::cout << total << " imported" << std::endl;

    const string& text = speechtext.text();
    const string& wave_bytes = speechtext.wave();

    insert.BindText(1, text);
    insert.BindBlob(2, wave_bytes);
    insert.Execute();
    insert.Reset();
  }
  db("end");
  std::cout << "all imported" << std::endl;
}