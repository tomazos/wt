#include "core/must.h"
#include "ladybug/ladybug.h"
#include "ladybug/ladybug_network.h"
#include "main/noargs.h"
#include "network/socket.h"

void Main() {
  LadyBug& ladybug = LadyBug::Instance();

  network::Socket server;
  server.Bind(35358);
  server.Listen(5);
  network::Socket sock = Accept(server);

  while (true) {
    optional<bigint> command = sock.ReceiveInteger();

    if (command == nullopt) {
      break;
    } else if (*command == kGetState) {
      const LadyBugState state = ladybug.GetState();
      sock.SendMessage(state);
      sock.Flush();
    } else {
      FAIL("Unknown command: ", *command);
    }
  }
  sock.Shutdown(SHUT_WR);
}
