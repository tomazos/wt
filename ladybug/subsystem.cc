#include "ladybug/subsystem.h"

#include "must.h"

Subsystem::Subsystem() : is_running_(false) {}

void Subsystem::Start() {
  MUST(!thread_);
  MUST(!is_running_);
  is_running_ = true;
  thread_ = std::make_unique<std::thread>([this] { Run(); });
}

void Subsystem::Run() {
  while (is_running_) {
    Tick();
  }
}

void Subsystem::Stop() {
  MUST(thread_);
  MUST(is_running_);
  is_running_ = false;
  thread_->join();
  thread_.reset();
}
