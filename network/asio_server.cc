#include <boost/asio.hpp>

#include "core/must.h"
#include "main/noargs.h"

namespace asio = boost::asio;
using boost::asio::ip::tcp;

void Main() {
  asio::io_service service;

  tcp::endpoint e(tcp::v4(), 35358);
  tcp::acceptor a(service, e);
  a.listen();
  tcp::socket s(service);
  a.accept(s, e);
  char buf[5];
  asio::read(s, asio::buffer(buf, 5));
  MUST_EQ(string(buf), "ping");
  asio::write(s, asio::buffer("pong", 5));
}
