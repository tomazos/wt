#include "core/must.h"
#include "main/noargs.h"
#include "network/socket.h"

using I = int32;
constexpr I k = 100'000'000;

void Main() {
  network::Socket server;
  server.Bind(35358);
  server.Listen(5);
  network::Socket sock = Accept(server);

  for (I i = 0; i < k; i++) MUST_EQ(sock.ReceiveInteger(), i);

  char buf;
  MUST_EQ(sock.TryReceive(&buf, 1), 0u);

  for (I i = 0; i < k; i++) sock.SendInteger(i * 2);

  sock.Shutdown(SHUT_WR);
}
