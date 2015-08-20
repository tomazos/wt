#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <google/protobuf/message.h>

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
  void SendMessage(const protobuf::Message& message);

  size_t TryReceive(void* buf, size_t len);
  void Receive(void* buf, size_t len);
  optional<bigint> ReceiveInteger();
  optional<string> ReceiveString();
  [[gnu::warn_unused_result]] bool ReceiveMessage(protobuf::Message& message);

  void Shutdown(int how);

  void Flush();

  void GetOpt(int level, int optname, void* optval, socklen_t* optlen);
  int GetOptInt(int level, int optname);

  void SetOpt(int level, int optname, const void* optval, socklen_t optlen);
  void SetOptInt(int level, int optname, int optval);

 private:
  Socket(int fd) : fd_(fd) {}
  int fd_;

  Socket(const Socket&) = delete;
  Socket& operator=(const Socket&) = delete;

  friend Socket Accept(const Socket& sock);
};

Socket Accept(const Socket& sock);

}  // namespace network
