#ifndef __DALI_OFFSCREEN_IMAGE_H__
#define __DALI_OFFSCREEN_IMAGE_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <string>

#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/native-image-interface.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/object/any.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class OffscreenImage;
}
}

class OffscreenImage;
/**
 * @brief Pointer to Dali::OffscreenImage.
 */
typedef IntrusivePtr<OffscreenImage> OffscreenImagePtr;

/**
 * @brief Used for displaying native Pixmap images.
 *
 * The native pixmap can be created internally or
 * externally by X11 or ECORE-X11.
 *
 */
class DALI_IMPORT_API OffscreenImage : public NativeImageInterface
{
public:

   /**
    * @brief When creating a pixmap the color depth has to be specified.
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
   * @brief Create a new OffscreenImage.
   *
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @return A smart-pointer to a newly allocated image.
   */
  static OffscreenImagePtr New( unsigned int width, unsigned int height, ColorDepth depth );

  /**
   * @brief Create a new OffscreenImage from an existing pixmap.
   *
   * @param[in] pixmap must be a X11 pixmap or a Ecore_X_Pixmap
   * @return A smart-pointer to a newly allocated image.
   */
  static OffscreenImagePtr New( Any pixmap );

  /**
   * @brief Retrieve the internal pixmap
   *
   * @return pixmap any object containing the internal pixmap
   */
  Any GetPixmap();

  /**
   * @brief Get a copy of the pixels used by OffscreenImage.
   *
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
   * @brief Convert the current pixel contents to either a JPEG or PNG format
   * and write that to the filesytem.
   *
   * @param[in] filename Identify the filesytem location at which to write the
   *                     encoded image. The extension determines the encoding used.
   *                     The two valid encoding are (".jpeg"|".jpg") and ".png".
   * @return    True if the pixels were written, and false otherwise.
   */
  bool EncodeToFile(const std::string& filename) const;

private:   // native image

  /**
   * @copydoc Dali::NativeImageInterface::GlExtensionCreate()
   */
  virtual bool GlExtensionCreate();

  /**
   * @copydoc Dali::NativeImageInterface::GlExtensionDestroy()
   */
  virtual void GlExtensionDestroy();

  /**
   * @copydoc Dali::NativeImageInterface::TargetTexture()
   */
  virtual unsigned int TargetTexture();

  /**
   * @copydoc Dali::NativeImageInterface::PrepareTexture()
   */
  virtual void PrepareTexture();

  /**
   * @copydoc Dali::NativeImageInterface::GetWidth()
   */
  virtual unsigned int GetWidth() const;

  /**
   * @copydoc Dali::NativeImageInterface::GetHeight()
   */
  virtual unsigned int GetHeight() const;

  /**
   * @copydoc Dali::NativeImageInterface::RequiresBlending()
   */
  virtual bool RequiresBlending() const;

private:

  /**
   * @brief Private constructor
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   */
  DALI_INTERNAL OffscreenImage( unsigned int width, unsigned int height, ColorDepth depth, Any pixmap );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   *
   * The implementation should destroy the NativeImage resources.
   */
  DALI_INTERNAL virtual ~OffscreenImage();

  /**
   * @brief Undefined assignment operator.
   *
   * This avoids accidental calls to a default assignment operator.
   * @param[in] rhs A reference to the object to copy.
   */
  DALI_INTERNAL OffscreenImage& operator=(const OffscreenImage& rhs);

private:

  Internal::Adaptor::OffscreenImage* mImpl; ///< Implementation pointer
};

} // namespace Dali

#endif // __DALI_OFFSCREEN_IMAGE_H__
