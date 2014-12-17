#ifndef __DALI_INTERNAL_TIZEN_BUFFER_IMAGE_H__
#define __DALI_INTERNAL_TIZEN_BUFFER_IMAGE_H__

#include <tizen-buffer-image.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
class Adaptor;
class EglImageExtensions;

class TizenBufferImage
{

public:

  static TizenBufferImage* New( tbm_surface_h* surface);

  void SetSurface( tbm_surface_h* surface );

  bool GlExtensionCreate();

  void GlExtensionDestroy();

  unsigned int TargetTexture();

  void PrepareTexture();

  unsigned int GetWidth() const;

  unsigned int GetHeight() const;

  Pixel::Format GetPixelFormat() const;

private:

  TizenBufferImage( tbm_surface_h* surface );

  virtual ~TizenBufferImage();

  TizenBufferImage& operator=(const TizenBufferImage& rhs);

private:

  void* mEglImageKHR;
  EglImageExtensions* mEglImageExtensions;
  tbm_surface_h* mTbmsurface;
  unsigned int mWidth;
  unsigned int mHeight;

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif
