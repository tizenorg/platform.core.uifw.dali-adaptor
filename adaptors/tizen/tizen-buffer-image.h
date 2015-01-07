#ifndef __DALI_TIZEN_BUFFER_IMAGE_H__
#define __DALI_TIZEN_BUFFER_IMAGE_H__

#include <dali/public-api/images/native-image.h>

#include <tbm_surface.h>

namespace Dali DALI_IMPORT_API
{
class Adaptor;

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class TizenBufferImage;
}
}

class TizenBufferImage;

/**
 * @brief Pointer to Dali::TizenBufferImage.
 */
typedef IntrusivePtr< TizenBufferImage > TizenBufferImagePtr;


class TizenBufferImage : public NativeImage
{

public:

  static TizenBufferImagePtr New( tbm_surface_h* surface = NULL);

  void SetSurface( tbm_surface_h* surface );

private:

  /**
   * @copydoc Dali::NativeImage::GlExtensionCreate()
   */
  virtual bool GlExtensionCreate();

  /**
   * @copydoc Dali::NativeImage::GlExtensionDestroy()
   */
  virtual void GlExtensionDestroy();

  /**
   * @copydoc Dali::NativeImage::TargetTexture()
   */
  virtual unsigned int TargetTexture();

  /**
   * @copydoc Dali::NativeImage::PrepareTexture()
   */
  virtual void PrepareTexture();

  /**
   * @copydoc Dali::NativeImage::GetWidth()
   */
  virtual unsigned int GetWidth() const;

  /**
   * @copydoc Dali::NativeImage::GetHeight()
   */
  virtual unsigned int GetHeight() const;

  /**
   * @copydoc Dali::NativeImage::GetPixelFormat()
   */
  virtual Pixel::Format GetPixelFormat() const;

private:

  TizenBufferImage( tbm_surface_h* surface );

  TizenBufferImage& operator=(const TizenBufferImage& rhs);

  virtual ~TizenBufferImage();

private:
  Internal::Adaptor::TizenBufferImage* mImpl; ///< Implementation pointer
};


} // namespace Dali

#endif
