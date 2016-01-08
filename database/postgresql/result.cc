#include "database/postgresql/result.h"

#include "core/must.h"

namespace database {
namespace postgresql {

Result::Result(Connection* connection, PGresult* result)
    : connection_(connection), result_(result, PQclear) {}

}  // namespace postgresql
}  // namespace database
