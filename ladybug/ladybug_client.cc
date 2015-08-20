#include "ladybug/ladybug_client.h"

#include <atomic>

#include "core/must.h"
#include "ladybug/ladybug_network.h"
#include "ladybug/ladybug_state.pb.h"
#include "main/args.h"

LadyBugClient::LadyBugClient(const string& address) {
  sock_.Connect(address, "35358");
}

LadyBugState LadyBugClient::GetState() {
  sock_.SendInteger(kGetState);
  sock_.Flush();
  LadyBugState state;
  MUST(sock_.ReceiveMessage(state));
  return state;
}

LadyBugClient::~LadyBugClient() {
  sock_.Shutdown(SHUT_WR);
  MUST(!sock_.ReceiveInteger());
}
