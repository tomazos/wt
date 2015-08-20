#include "network/socket.h"

#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "core/must.h"

namespace network {

Socket::Socket() : fd_(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) {
  if (fd_ == -1) {
    THROW_ERRNO("socket");
  }
}

Socket::~Socket() {
  if (fd_ != -1) {
    int close_result = close(fd_);
    if (close_result != 0) {
      THROW_ERRNO("close");
    }
  }
}

void Socket::Bind(uint16 port) {
  sockaddr_in bind_sockaddr;
  std::memset(&bind_sockaddr, 0, sizeof(sockaddr_in));
  bind_sockaddr.sin_family = AF_INET;
  bind_sockaddr.sin_addr.s_addr = INADDR_ANY;
  bind_sockaddr.sin_port = htons(port);
  int bind_result = bind(fd_, (sockaddr*)&bind_sockaddr, sizeof(bind_sockaddr));
  if (bind_result != 0) THROW_ERRNO("bind(", port, ")");
}

void Socket::Listen(int backlog) {
  int listen_result = listen(fd_, backlog);
  if (listen_result != 0) THROW_ERRNO("listen");
}

Socket Accept(const Socket& sock) {
  sockaddr_in client_sockaddr;
  socklen_t client_sockaddr_len = sizeof(client_sockaddr);

  std::memset(&client_sockaddr, 0, sizeof(sockaddr_in));

  int accept_result =
      accept(sock.fd_, (sockaddr*)&client_sockaddr, &client_sockaddr_len);
  MUST_EQ(client_sockaddr_len, sizeof(client_sockaddr));
  if (accept_result <= 0) THROW_ERRNO("accept");
  return Socket(accept_result);
}

void Socket::Connect(const string& node, const string& service) {
  addrinfo hints;
  std::memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = 0;
  addrinfo* result = nullptr;
  int getaddrinfo_result =
      getaddrinfo(node.c_str(), service.c_str(), &hints, &result);
  if (getaddrinfo_result != 0) {
    const char* error = gai_strerror(getaddrinfo_result);
    FAIL("getaddrinfo(", node, ", ", service, ") failed with: ", error);
  }
  MUST(result);
  MUST(!result->ai_next);
  int connect_result = connect(fd_, result->ai_addr, result->ai_addrlen);
  freeaddrinfo(result);
  if (connect_result != 0) {
    THROW_ERRNO("connect");
  }
}

size_t Socket::TrySend(const void* buf, size_t len) {
  ssize_t send_result = send(fd_, buf, len, 0 /*flags*/);
  if (send_result == -1) {
    THROW_ERRNO("send");
  }
  return size_t(send_result);
}

void Socket::Send(const void* const buf, const size_t len) {
  const char* ptr = static_cast<const char*>(buf);
  const char* end = ptr + len;
  while (ptr < end) ptr += TrySend(ptr, end - ptr);
}

void Socket::SendInteger(bigint n) {
  std::vector<uint8> ne;
  do {
    uint8 m = (uint8(n) & 0b0111'1111);
    n >>= 7;
    m |= 0b1000'0000;
    ne.push_back(m);
  } while (n > 0);
  Reverse(ne);
  ne.back() &= 0b0111'1111;
  Send(ne.data(), ne.size());
}

void Socket::SendString(const string& str) {
  SendInteger(str.size());
  Send(str.data(), str.size());
}

void Socket::SendMessage(const protobuf::Message& message) {
  string message_string;
  if (!message.SerializeToString(&message_string))
    FAIL("SerializeToString failed.");
  SendString(message_string);
}

size_t Socket::TryReceive(void* const buf, const size_t len) {
  ssize_t recv_result = recv(fd_, buf, len, 0 /*flags*/);
  if (recv_result == -1) {
    THROW_ERRNO("recv");
  }
  return size_t(recv_result);
}

void Socket::Receive(void* buf, size_t len) {
  if (len == 0) return;
  char* ptr = static_cast<char*>(buf);
  const char* end = ptr + len;
  while (ptr < end) {
    size_t received_bytes = TryReceive(ptr, end - ptr);
    if (received_bytes == 0) FAIL("Unexpected end-of-stream.");
    ptr += received_bytes;
  }
}

optional<bigint> Socket::ReceiveInteger() {
  uint8 m;
  size_t first_receive = TryReceive(&m, 1);
  if (first_receive == 0) return nullopt;
  bigint result = (m & 0b0111'1111);
  while (m & 0b1000'0000) {
    Receive(&m, 1);
    result <<= 7;
    result |= (m & 0b0111'1111);
  }
  return result;
}

optional<string> Socket::ReceiveString() {
  optional<bigint> string_length = ReceiveInteger();
  if (!string_length) return nullopt;
  MUST_LE(*string_length, std::numeric_limits<size_t>::max());
  string result(size_t(*string_length), '\0');
  Receive(&result[0], result.size());
  return result;
}

[[gnu::warn_unused_result]] bool Socket::ReceiveMessage(
    protobuf::Message& message) {
  optional<string> message_string = ReceiveString();
  if (!message_string) return false;
  const bool parse_success = message.ParseFromString(*message_string);
  if (!parse_success) {
    FAIL("ParseFromString failed.");
  }
  return true;
}

void Socket::Shutdown(int how) {
  int shutdown_result = shutdown(fd_, how);
  if (shutdown_result != 0) {
    THROW_ERRNO("shutdown");
  }
}

void Socket::Flush() {
  SetOptInt(IPPROTO_TCP, TCP_NODELAY, 1);
  SetOptInt(IPPROTO_TCP, TCP_NODELAY, 0);
}

void Socket::GetOpt(int level, int optname, void* optval, socklen_t* optlen) {
  int getsockopt_result = getsockopt(fd_, level, optname, optval, optlen);
  if (getsockopt_result != 0) THROW_ERRNO("getsockopt");
}

int Socket::GetOptInt(int level, int optname) {
  int optval;
  socklen_t optlen = sizeof(optval);
  GetOpt(level, optname, &optval, &optlen);
  MUST_EQ(optlen, sizeof(optval));
  return optval;
}

void Socket::SetOpt(int level, int optname, const void* optval,
                    socklen_t optlen) {
  int setsockopt_result = setsockopt(fd_, level, optname, optval, optlen);
  if (setsockopt_result != 0) THROW_ERRNO("setsockopt");
}

void Socket::SetOptInt(const int level, const int optname, const int optval) {
  SetOpt(level, optname, &optval, sizeof(optval));
}

}  // namespace network
