
// CLASS HEADER
#include "tizen-buffer-image-impl.h"

#include <adaptor-impl.h>
#include <gl/egl-factory.h>
#include <gl/egl-image-extensions.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

TizenBufferImage* TizenBufferImage::New(tbm_surface_h* surface)
{
  TizenBufferImage* image = new TizenBufferImage(surface);
  return image;
}

TizenBufferImage::TizenBufferImage(tbm_surface_h* surface)
: mEglImageKHR(NULL)
, mEglImageExtensions(NULL)
, mTbmsurface(surface)
, mWidth(0)
, mHeight(0)
{
  EglFactory& eglFactory = Adaptor::GetImplementation( Adaptor::Get() ).GetEGLFactory();
  mEglImageExtensions = eglFactory.GetImageExtensions();
}

TizenBufferImage::~TizenBufferImage()
{
}

void TizenBufferImage::SetSurface(tbm_surface_h* surface)
{
  mTbmsurface = surface;
}

bool TizenBufferImage::GlExtensionCreate()
{
  if(mEglImageKHR != NULL)
  {
    GlExtensionDestroy();
  }

  if(mTbmsurface == NULL)
  {
    return false;
  }

  if(mEglImageExtensions == NULL)
  {
    return false;
  }

#if _TIZEN_BUFFER_MANAGER_SUPPORT_

  mEglImageKHR = mEglImageExtensions->CreateImageKHR( *mTbmsurface );

  return mEglImageKHR != NULL;
#endif

  DALI_LOG_ERROR("Dali can not support TizenBufferImage\n");
  return false;
}

void TizenBufferImage::GlExtensionDestroy()
{
  if(mEglImageExtensions == NULL)
  {
    return;
  }

  mEglImageExtensions->DestroyImageKHR(mEglImageKHR);

  mEglImageKHR = NULL;
}

unsigned int TizenBufferImage::TargetTexture()
{
  if(mEglImageExtensions == NULL)
  {
    return 0;
  }

#if _TIZEN_BUFFER_MANAGER_SUPPORT_

  mEglImageExtensions->TargetTextureKHR(mEglImageKHR);

  return 0;
#endif

  DALI_LOG_ERROR("Dali can not support TizenBufferImage\n");
  return 0;
}

void TizenBufferImage::PrepareTexture()
{
}

unsigned int TizenBufferImage::GetWidth() const
{
#if _TIZEN_BUFFER_MANAGER_SUPPORT_
  if(mTbmsurface == NULL) return 0;
  return tbm_surface_get_width(*mTbmsurface);
#endif

  DALI_LOG_ERROR("Dali can not support TizenBufferImage\n");
  return 0;
}

unsigned int TizenBufferImage::GetHeight() const
{
#if _TIZEN_BUFFER_MANAGER_SUPPORT_
  if(mTbmsurface == NULL) return 0;
  return tbm_surface_get_height(*mTbmsurface);
#endif

  DALI_LOG_ERROR("Dali can not support TizenBufferImage\n");
  return 0;
}

Pixel::Format TizenBufferImage::GetPixelFormat() const
{
#if _TIZEN_BUFFER_MANAGER_SUPPORT_

  if(mTbmsurface == NULL) return Pixel::NOT_SUPPORTED;

  tbm_format format = tbm_surface_get_format(*mTbmsurface);

  switch(format)
  {
    case TBM_FORMAT_RGB565:
      return Pixel::RGB565;
    case TBM_FORMAT_BGR565:
      return Pixel::BGR565;
    case TBM_FORMAT_RGBA4444:
      return Pixel::RGBA4444;
    case TBM_FORMAT_BGRA4444:
      return Pixel::BGRA4444;
    case TBM_FORMAT_RGBA5551:
      return Pixel::RGBA5551;
    case TBM_FORMAT_BGRA5551:
      return Pixel::BGRA5551;
    case TBM_FORMAT_RGB888:
      return Pixel::RGB888;
    case TBM_FORMAT_RGBA8888:
      return Pixel::RGBA8888;
    case TBM_FORMAT_BGRA8888:
      return Pixel::BGRA8888;
  }
#endif

  DALI_LOG_ERROR("Dali can not support TizenBufferImage\n");
  return Pixel::NOT_SUPPORTED;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
