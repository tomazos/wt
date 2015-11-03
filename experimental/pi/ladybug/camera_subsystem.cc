#include "ladybug/camera_subsystem.h"

#include "core/must.h"

CameraSubsystem::CameraSubsystem() {
  raspicam_.setWidth(kWidth);
  raspicam_.setHeight(kHeight);
  raspicam_.setBrightness(50);
  raspicam_.setSharpness(0);
  raspicam_.setContrast(0);
  raspicam_.setSaturation(0);
  raspicam_.setShutterSpeed(0);
  raspicam_.setISO(400);
  //  raspicam_.setVideoStabilization(true);
  raspicam_.setExposureCompensation(0);
  raspicam_.setFormat(raspicam::RASPICAM_FORMAT_RGB);
  //  raspicam_.setExposure(raspicam::RASPICAM_EXPOSURE_AUTO);
  //  raspicam_.setAWB(raspicam::RASPICAM_AWB_AUTO);
  raspicam_.setExposure(raspicam::RASPICAM_EXPOSURE_NIGHT);
  raspicam_.setAWB(raspicam::RASPICAM_AWB_OFF);

  MUST(raspicam_.open(true));
  Start();
}

void CameraSubsystem::Tick() {
  MUST(raspicam_.grab());
  auto frame = raspicam_.getImageBufferData();
  {
    LockGuard l(mutex_);
    for (const Listener& listener : listeners_) {
      listener(frame);
    }
  }
}

CameraSubsystem::~CameraSubsystem() {
  Stop();
  raspicam_.release();
}
