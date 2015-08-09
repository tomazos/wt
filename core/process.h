#pragma once

#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>

template <typename... Args>
void ExecuteShellCommand(const Args&... args) {
  const string command = EncodeAsString(args...);
  std::cout << command << std::endl;
  int system_result = system(command.c_str());
  const string call_desc = EncodeAsString("std::system(", command, "): ");
  if (system_result == -1) {
    FAIL(call_desc, "returned -1");
  } else {
    if (WIFEXITED(system_result)) {
      int exit_status = WEXITSTATUS(system_result);
      if (exit_status == 0) {
        return;
      } else {
        FAIL(call_desc, "exit_status = ", exit_status);
      }
    } else if (WIFSIGNALED(system_result)) {
      int term_sig = WIFSIGNALED(system_result);
      bool core_dump = WCOREDUMP(system_result);
      FAIL(call_desc, "term_sig = ", term_sig,
           (core_dump ? "(core dumped)" : ""));
    } else if (WIFSTOPPED(system_result)) {
      int stop_sig = WSTOPSIG(system_result);
      FAIL(call_desc, "unexpected WIFSTOPPED: stop_sig = ", stop_sig);
    } else if (WIFCONTINUED(system_result)) {
      FAIL(call_desc, "unexpected WIFCONTINUED");
    } else {
      FAIL(call_desc, "unknown return value meaning: ", system_result);
    }
  }
}
