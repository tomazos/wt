#include <boost/asio.hpp>
#include <iostream>

#include "core/must.h"
#include "main/noargs.h"

namespace asio = boost::asio;
using asio::ip::tcp;

void Main() {
  asio::io_service service;

  tcp::resolver::query q("192.168.0.17", "35358");
  tcp::resolver r(service);
  tcp::socket s(service);
  asio::connect(s, r.resolve(q));
  asio::write(s, asio::buffer("ping", 5));
  char buf[5];
  asio::read(s, asio::buffer(buf, 5));
  MUST_EQ(string(buf), "pong");
}
