#pragma once

#include <vector>

#include "ladybug/subsystem.h"
#include "raspicam/raspicam.h"

class CameraSubsystem : public Subsystem {
 public:
  static constexpr int kWidth = 320;
  static constexpr int kHeight = 240;
  static constexpr int kPixels = kWidth * kHeight;
  static constexpr int kBytes = kPixels * 3;

  CameraSubsystem();
  ~CameraSubsystem();

  using Listener = std::function<void(uint8* frame)>;
  void AddListener(Listener listener) {
    LockGuard l(mutex_);
    listeners_.push_back(listener);
  }

 protected:
  void Tick() override;

 private:
  raspicam::RaspiCam raspicam_;

  Mutex mutex_;
  std::vector<Listener> listeners_;
};
