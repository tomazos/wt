#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "core/bigint.h"

namespace network {

class Socket {
 public:
  Socket();
  ~Socket();

  Socket(Socket&& that) : fd_(that.fd_) { that.fd_ = -1; }
  Socket& operator=(Socket&& that) {
    fd_ = that.fd_;
    that.fd_ = -1;
    return *this;
  }

  void Bind(uint16 port);

  void Listen(int backlog);

  void Connect(const string& node, const string& service);

  size_t TrySend(const void* buf, size_t len);
  void Send(const void* buf, size_t len);
  void SendInteger(bigint n);
  void SendString(const string& str);

  size_t TryReceive(void* buf, size_t len);
  void Receive(void* buf, size_t len);
  bigint ReceiveInteger();
  string ReceiveString();

  void Shutdown(int how);

 private:
  Socket(int fd) : fd_(fd) {}
  int fd_;

  Socket(const Socket&) = delete;
  Socket& operator=(const Socket&) = delete;

  friend Socket Accept(const Socket& sock);
};

Socket Accept(const Socket& sock);

}  // namespace network
