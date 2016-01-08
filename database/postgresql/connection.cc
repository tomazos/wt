#include "database/postgresql/connection.h"

#include "core/must.h"
#include "database/postgresql/result.h"

namespace database {
namespace postgresql {

Connection::Connection(const string& uri)
    : conn_(PQconnectdb(uri.c_str()), PQfinish) {
  MUST(conn_);
  if (PQstatus(conn_.get()) != CONNECTION_OK)
    throw std::logic_error(ErrorMessage());
}

string Connection::ErrorMessage() { return PQerrorMessage(conn_.get()); }

}  // namespace postgresql
}  // namespace database
