#include "ladybug/ladybug_client.h"

#include <atomic>

#include "core/must.h"
#include "ladybug/ladybug_state.pb.h"
#include "main/args.h"

LadyBugClient::LadyBugClient(const string& address) {
  sock_.Connect(address, "35358");
}

LadyBugState LadyBugClient::SendCommand(const LadyBugCommand& command) {
  sock_.SendMessage(command);
  sock_.Flush();
  LadyBugState state;
  MUST(sock_.ReceiveMessage(state));
  return state;
}

LadyBugClient::~LadyBugClient() {
  sock_.Shutdown(SHUT_WR);
  MUST(!sock_.ReceiveInteger());
}
