#pragma once

#include "ladybug/ladybug_state.pb.h"
#include "network/socket.h"

class LadyBugClient {
 public:
  LadyBugClient(const string& address);
  ~LadyBugClient();

  LadyBugState GetState();

 private:
  network::Socket sock_;
};
