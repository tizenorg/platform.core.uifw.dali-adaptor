#ifndef __DALI_PIXMAP_IMAGE_H__
#define __DALI_PIXMAP_IMAGE_H__

/*
Copyright (c) 2000-2014 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <boost/any.hpp>
#include <dali/public-api/images/native-image.h>

namespace Dali DALI_IMPORT_API
{
class Adaptor;

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class PixmapImage;
}
}

class PixmapImage;
typedef IntrusivePtr<PixmapImage> PixmapImagePtr;

/**
 * Used for displaying native Pixmap images.
 * The native pixmap can be created internally or
 * externally by X11 or ECORE-X11.
 *
 */
class PixmapImage : public NativeImage
{
public:

   /**
    * PixmapImage can use pixmaps created by X11 or ECORE X11
    */
   enum PixmapAPI
   {
     X11,        ///< X types
     ECORE_X11,  ///< EFL e-core x11 types
   };

   /**
    * When creating a pixmap the color depth has to be specified
    */
   enum ColorDepth
   {
     COLOR_DEPTH_DEFAULT,     ///< Uses the current X screen default depth (recommended)
     COLOR_DEPTH_8,           ///< 8 bits per pixel
     COLOR_DEPTH_16,          ///< 16 bits per pixel
     COLOR_DEPTH_24,          ///< 24 bits per pixel
     COLOR_DEPTH_32           ///< 32 bits per pixel
   };

  /**
   * Create a new PixmapImage.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] adaptor reference to dali adaptor
   * @return A smart-pointer to a newly allocated image.
   */
  static PixmapImagePtr New( unsigned int width, unsigned int height, ColorDepth depth,  Adaptor& adaptor );

  /**
   * Create a new PixmapImage from an existing pixmap.
   * @param[in] pixmap must be a X11 pixmap or a Ecore_X_Pixmap
   * @param[in] adaptor reference to dali adaptor
   * @return A smart-pointer to a newly allocated image.
   */
  static PixmapImagePtr New( boost::any pixmap, Adaptor& adaptor );

  /**
   * Retrieve the internal pixmap
   * @param api whether to return a pixmap that can be used with X11 or EFL
   * @return pixmap boost any object containing a pixmap of the type specified PixmapAPI
   */
  boost::any GetPixmap( PixmapAPI api );

  /**
   * Retrieve the display used to create the pixmap.
   * If the pixmap was created outside of Dali, then this display
   * is the one Dali uses internally.
   * @return boost any object containing the display
   */
  boost::any GetDisplay();

  /**
   * Get a copy of the pixels used by PixmapImage.
   * This is only supported for 24 bit RGB and 32 bit RGBA internal formats
   * (COLOR_DEPTH_24 and COLOR_DEPTH_32).
   * @param[out] pixbuf a vector to store the pixels in
   * @param[out] width  width of image
   * @param[out] height height of image
   * @param[out] pixelFormat pixel format used by image
   * @return     True if the pixels were gotten, and false otherwise.
   */
  bool GetPixels( std::vector<unsigned char>& pixbuf, unsigned int& width, unsigned int& height, Pixel::Format& pixelFormat ) const;

  /**
   * Convert the current pixel contents to either a JPEG or PNG format
   * and write that to the filesytem.
   * @param[in] filename Identify the filesytem location at which to write the
   *                     encoded image. The extension determines the encoding used.
   *                     The two valid encoding are (".jpeg"|".jpg") and ".png".
   * @return    True if the pixels were written, and false otherwise.
   */
  bool EncodeToFile(const std::string& filename) const;

private:   // native image

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

  /**
   * Private constructor
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] adaptor a reference to Dali adaptor
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   */
  PixmapImage(unsigned int width, unsigned int height, ColorDepth depth, Adaptor& adaptor, boost::any pixmap);

  /**
   * A reference counted object may only be deleted by calling Unreference().
   * The implementation should destroy the NativeImage resources.
   */
  virtual ~PixmapImage();

  /**
   * Undefined assignment operator.
   * This avoids accidental calls to a default assignment operator.
   * @param[in] rhs A reference to the object to copy.
   */
  PixmapImage& operator=(const PixmapImage& rhs);

private:

  Internal::Adaptor::PixmapImage* mImpl;

};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_PIXMAP_IMAGE_H__
