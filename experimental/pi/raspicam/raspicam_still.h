#pragma once

#include <cstdio>

#include "raspicam/raspicamtypes.h"

namespace raspicam {

namespace _private {
class Private_Impl_Still;
};

/**Raspicam API for still camera
 */
class RaspiCam_Still {
  // the implementation of the camera
  _private::Private_Impl_Still *_impl;

 public:
  // Constructor
  RaspiCam_Still();
  // Destructor
  ~RaspiCam_Still();
  // Opens camera connection
  bool open();
  // Grabs and set the data into the data buffer which has the indicated length.
  // It is your responsability
  // to alloc the buffer. You can use getImageBufferSize for that matter.
  bool grab_retrieve(unsigned char *data, unsigned int length);
  // Releases the camera
  void release();  // not working
  // Returns the size of the images captured with the current parameters
  size_t getImageBufferSize() const;

  void commitParameters();
  void setWidth(unsigned int width);
  void setHeight(unsigned int height);
  void setCaptureSize(unsigned int width, unsigned int height);
  void setBrightness(unsigned int brightness);
  void setQuality(unsigned int quality);
  void setRotation(int rotation);
  void setISO(int iso);
  void setSharpness(int sharpness);
  void setContrast(int contrast);
  void setSaturation(int saturation);
  void setEncoding(RASPICAM_ENCODING encoding);
  void setExposure(RASPICAM_EXPOSURE exposure);
  void setAWB(RASPICAM_AWB awb);
  void setImageEffect(RASPICAM_IMAGE_EFFECT imageEffect);
  void setMetering(RASPICAM_METERING metering);
  void setHorizontalFlip(bool hFlip);
  void setVerticalFlip(bool vFlip);

  unsigned int getWidth();
  unsigned int getHeight();
  unsigned int getBrightness();
  unsigned int getRotation();
  unsigned int getQuality();
  int getISO();
  int getSharpness();
  int getContrast();
  int getSaturation();
  RASPICAM_ENCODING getEncoding();
  RASPICAM_EXPOSURE getExposure();
  RASPICAM_AWB getAWB();
  RASPICAM_IMAGE_EFFECT getImageEffect();
  RASPICAM_METERING getMetering();
  bool isHorizontallyFlipped();
  bool isVerticallyFlipped();
};
}
