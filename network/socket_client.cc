#include "core/must.h"
#include "main/noargs.h"
#include "network/socket.h"

using I = int32;
constexpr I k = 100'000'000;

void Main() {
  network::Socket sock;
  sock.Connect("192.168.0.17", "35358");

  for (I i = 0; i < k; i++) sock.SendInteger(i);

  sock.Shutdown(SHUT_WR);

  for (I i = 0; i < k; i++) MUST_EQ(sock.ReceiveInteger(), i * 2);

  char buf;
  MUST_EQ(sock.TryReceive(&buf, 1), 0u);
}
