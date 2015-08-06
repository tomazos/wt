#pragma once

#include <atomic>

class Subsystem {
 public:
  Subsystem();

  void Start();

  void Stop();

 protected:
  virtual void Tick() = 0;

 private:
  void Run();

  std::atomic_bool is_running_;

  std::unique_ptr<std::thread> thread_;
};
