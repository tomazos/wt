#include "core/must.h"
#include "ladybug/ladybug.h"
#include "main/noargs.h"
#include "network/socket.h"

void Main() {
  LadyBug& ladybug = LadyBug::Instance();

  network::Socket server;
  server.Bind(35358);
  server.Listen(5);
  network::Socket sock = Accept(server);

  while (true) {
    LadyBugCommand command;
    bool message_received = sock.ReceiveMessage(command);
    if (!message_received) {
      break;
    } else {
      if (command.has_left_power())
        ladybug.SetLeftWheelPower(command.left_power());
      if (command.has_right_power())
        ladybug.SetRightWheelPower(command.right_power());

      const LadyBugState state = ladybug.GetState();
      sock.SendMessage(state);
      sock.Flush();
    }
  }
  sock.Shutdown(SHUT_WR);
}
