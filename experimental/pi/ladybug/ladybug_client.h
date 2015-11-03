#pragma once

#include "ladybug/ladybug_state.pb.h"
#include "network/socket.h"

class LadyBugClient {
 public:
  LadyBugClient(const string& address);
  ~LadyBugClient();

  LadyBugState SendCommand(const LadyBugCommand& command);

 private:
  network::Socket sock_;
};
