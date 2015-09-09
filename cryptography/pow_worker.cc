#include "cryptography/proof_of_work.h"

#include "core/must.h"
#include "cryptography/pow.pb.h"
#include "network/socket.h"

void Main() {
  while (true) {
    network::Socket sock;
    sock.Connect("66.85.186.238", "35358");
    WorkRequest request;
    MUST(sock.ReceiveMessage(request));
    optional<ProofOfWork> proof_of_work =
        Work(request.subject(), request.len(), request.init_suffix(),
             request.width());
    WorkResponse response;
    if (proof_of_work) {
      ProofOfWorkProto* proof_of_work_proto = response.mutable_proof_of_work();
      proof_of_work_proto->set_subject(proof_of_work->subject);
      proof_of_work_proto->set_suffix(proof_of_work->suffix);
      proof_of_work_proto->set_len(proof_of_work->len);
    }
    sock.SendMessage(response);
    sock.Flush();
    sock.Shutdown(SHUT_WR);
    MUST(!sock.ReceiveInteger());
  }
}
