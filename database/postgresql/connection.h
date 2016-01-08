#pragma once

#include <postgresql/libpq-fe.h>

#include "database/postgresql/type_handlers.h"
#include "database/postgresql/result.h"

namespace database {
namespace postgresql {

class Connection {
 public:
  Connection(const string& uri);

  template <typename... Args>
  Result ExecuteCommand(const string& command, const Args&... args);

 private:
  string ErrorMessage();

  std::unique_ptr<PGconn, void (*)(PGconn*)> conn_;

  template <std::size_t... I>
  friend class CommandExecutor;
};

template <size_t... I>
struct CommandExecutor {
  template <typename... Args>
  Result ExecuteCommand(Connection* connection, const string& command,
                        const Args&... args) {
    constexpr int nParams = sizeof...(Args);
    constexpr Oid paramTypes[] = {Type<Args>::oid...};
    const int paramLengths[] = {Type<Args>::length(args)...};
    const int paramFormats[] = {Type<Args>::format...};
    [[gnu::unused]] const auto values =
        std::make_tuple(typename Type<Args>::Encoded(args)...);
    constexpr int resultFormat = 1 /*binary*/;
    const char* paramValues[] = {(const char*)std::get<I>(values).get(args)...};

    Result result(connection,
                  PQexecParams(connection->conn_.get(), command.c_str(),
                               nParams, paramTypes, paramValues, paramLengths,
                               paramFormats, resultFormat));
    ExecStatusType status = PQresultStatus(result.result_.get());
    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
      throw std::logic_error(connection->ErrorMessage());
    return result;
  }
};

template <size_t... I>
CommandExecutor<I...> MakeCommandExecutor(std::index_sequence<I...>) {
  return {};
}

template <typename... Args>
Result Connection::ExecuteCommand(const string& command, const Args&... args) {
  auto executor =
      MakeCommandExecutor(std::make_index_sequence<sizeof...(Args)>());
  return executor.ExecuteCommand(this, command, args...);
}

}  // namespace postgresql
}  // namespace database
