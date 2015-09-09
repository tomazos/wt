#pragma once

#include "core/sha3.h"

struct ProofOfWork {
  string subject;
  string suffix;
  int len;
};

inline bool Verify(const ProofOfWork& proof_of_work) {
  string concatenated = proof_of_work.subject + proof_of_work.suffix;
  auto bytes = SHA3_256(concatenated);
  for (int i = 0; i < proof_of_work.len; ++i) {
    if (bytes[i] != 0) return false;
  }
  return true;
}

inline optional<ProofOfWork> Work(const string& subject, int len,
                                  const string& init_suffix, int width) {
  string concatenated = subject + init_suffix + string(width, '\0');
  uint8* const input = (uint8*)concatenated.data();
  const size_t input_len = concatenated.size();

  uint8* const suffix = input + subject.size() + init_suffix.size();

  uint8 md[32];
  while (true) {
    SHA3_256(concatenated.data(), input_len, md);
    bool success = true;
    for (int i = 0; i < len; i++) {
      if (md[i] != 0) {
        success = false;
        break;
      }
    }
    if (success) {
      ProofOfWork proof_of_work;
      proof_of_work.subject = subject;
      proof_of_work.suffix = concatenated.substr(subject.size());
      proof_of_work.len = len;
      return proof_of_work;
    }
    uint8 carry = 1;
    for (int i = 0; i < width; i++) {
      suffix[i] += carry;
      carry = (carry && suffix[i] == 0x00);
      if (!carry) break;
    }
    if (carry) return nullopt;
  }
}
