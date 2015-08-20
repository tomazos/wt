#include "raspicam/raspicam_still.h"
#include "raspicam/private_still_impl.h"

namespace raspicam {
RaspiCam_Still::RaspiCam_Still() { _impl = new _private::Private_Impl_Still(); }
RaspiCam_Still::~RaspiCam_Still() {
  release();
  delete _impl;
}

bool RaspiCam_Still::open() { return _impl->initialize() == 0; }
bool RaspiCam_Still::grab_retrieve(unsigned char* preallocated_data,
                                   unsigned int length) {
  return _impl->takePicture(preallocated_data, length);
}
void RaspiCam_Still::release() {}

size_t RaspiCam_Still::getImageBufferSize() const {
  return _impl->getImageBufferSize();
}
void RaspiCam_Still::commitParameters() { _impl->commitParameters(); }
void RaspiCam_Still::setWidth(unsigned int width) { _impl->setWidth(width); }
void RaspiCam_Still::setHeight(unsigned int height) {
  _impl->setHeight(height);
}
void RaspiCam_Still::setCaptureSize(unsigned int width, unsigned int height) {
  _impl->setCaptureSize(width, height);
}
void RaspiCam_Still::setBrightness(unsigned int brightness) {
  _impl->setBrightness(brightness);
}
void RaspiCam_Still::setQuality(unsigned int quality) {
  _impl->setQuality(quality);
}
void RaspiCam_Still::setRotation(int rotation) { _impl->setRotation(rotation); }
void RaspiCam_Still::setISO(int iso) { _impl->setISO(iso); }
void RaspiCam_Still::setSharpness(int sharpness) {
  _impl->setSharpness(sharpness);
}
void RaspiCam_Still::setContrast(int contrast) { _impl->setContrast(contrast); }
void RaspiCam_Still::setSaturation(int saturation) {
  _impl->setSaturation(saturation);
}
void RaspiCam_Still::setEncoding(RASPICAM_ENCODING encoding) {
  _impl->setEncoding(encoding);
}
void RaspiCam_Still::setExposure(RASPICAM_EXPOSURE exposure) {
  _impl->setExposure(exposure);
}
void RaspiCam_Still::setAWB(RASPICAM_AWB awb) { _impl->setAWB(awb); }
void RaspiCam_Still::setImageEffect(RASPICAM_IMAGE_EFFECT imageEffect) {
  _impl->setImageEffect(imageEffect);
}
void RaspiCam_Still::setMetering(RASPICAM_METERING metering) {
  _impl->setMetering(metering);
}
void RaspiCam_Still::setHorizontalFlip(bool hFlip) {
  _impl->setHorizontalFlip(hFlip);
}
void RaspiCam_Still::setVerticalFlip(bool vFlip) {
  _impl->setVerticalFlip(vFlip);
}

unsigned int RaspiCam_Still::getWidth() { return _impl->getWidth(); }
unsigned int RaspiCam_Still::getHeight() { return _impl->getHeight(); }
unsigned int RaspiCam_Still::getBrightness() { return _impl->getBrightness(); }
unsigned int RaspiCam_Still::getRotation() { return _impl->getRotation(); }
unsigned int RaspiCam_Still::getQuality() { return _impl->getQuality(); }
int RaspiCam_Still::getISO() { return _impl->getISO(); }
int RaspiCam_Still::getSharpness() { return _impl->getSharpness(); }
int RaspiCam_Still::getContrast() { return _impl->getContrast(); }
int RaspiCam_Still::getSaturation() { return _impl->getSaturation(); }
RASPICAM_ENCODING RaspiCam_Still::getEncoding() { return _impl->getEncoding(); }
RASPICAM_EXPOSURE RaspiCam_Still::getExposure() { return _impl->getExposure(); }
RASPICAM_AWB RaspiCam_Still::getAWB() { return _impl->getAWB(); }
RASPICAM_IMAGE_EFFECT RaspiCam_Still::getImageEffect() {
  return _impl->getImageEffect();
}
RASPICAM_METERING RaspiCam_Still::getMetering() { return _impl->getMetering(); }
bool RaspiCam_Still::isHorizontallyFlipped() {
  return _impl->isHorizontallyFlipped();
}
bool RaspiCam_Still::isVerticallyFlipped() {
  return _impl->isVerticallyFlipped();
}
}
