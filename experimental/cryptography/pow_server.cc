#include <atomic>
#include <list>

#include "core/hex.h"
#include "core/must.h"
#include "experimental/cryptography/proof_of_work.h"
#include "experimental/cryptography/pow.pb.h"
#include "main/noargs.h"
#include "network/socket.h"

const std::string subject = "Hello, Worldz!";
const int width = 3;

std::atomic_int next_init_suffix(0);

Mutex best_len_mu;
std::atomic_int best_len(1);

void Handler(network::Socket sock) {
  WorkRequest request;
  request.set_subject(subject);
  request.set_len(best_len + 1);
  int id = next_init_suffix++;
  if (0 == (id & (id - 1))) {
    std::cout << "request " << id << std::endl;
  }
  request.set_init_suffix(std::to_string(id));
  request.set_width(width);
  sock.SendMessage(request);
  sock.Flush();
  WorkResponse response;
  MUST(sock.ReceiveMessage(response));
  if (response.has_proof_of_work()) {
    LockGuard l(best_len_mu);
    const ProofOfWorkProto& proof_of_work_proto = response.proof_of_work();
    ProofOfWork proof_of_work;
    proof_of_work.subject = proof_of_work_proto.subject();
    proof_of_work.suffix = proof_of_work_proto.suffix();
    proof_of_work.len = proof_of_work_proto.len();
    if (Verify(proof_of_work)) {
      if (proof_of_work.len > best_len) {
        best_len = proof_of_work.len;
        std::cout << "proof of len " << best_len
                  << " found: " << ByteArrayToHexString(proof_of_work.suffix)
                  << std::endl;
      }
    } else {
      std::cerr << "unverified proof received" << std::endl;
    }
  }
  MUST(!sock.ReceiveInteger());
  sock.Shutdown(SHUT_WR);
}

void Main() {
  network::Socket server;
  server.Bind(35358);
  server.Listen(5);

  while (true) {
    network::Socket sock = Accept(server);
    std::thread t(Handler, std::move(sock));
    t.detach();
  }
}
