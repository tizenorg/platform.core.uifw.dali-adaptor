#ifndef __DALI_INTERNAL_OFFSCREEN_IMAGE_H__
#define __DALI_INTERNAL_OFFSCREEN_IMAGE_H__

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

// INTERNAL INCLUDES
#include <offscreen-image.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
class EglImageExtensions;

/**
 * Dali internal OffscreenImage.
 */
class OffscreenImage
{
public:

  /**
   * Create a new OffscreenImage internally.
   * Depending on hardware the width and height may have to be a power of two.
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] depth color depth of the pixmap
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   * @return A smart-pointer to a newly allocated image.
   */
  static OffscreenImage* New(unsigned int width,
                          unsigned int height,
                          Dali::OffscreenImage::ColorDepth depth,
                          Any pixmap);

  /**
   * @copydoc Dali::OffscreenImage::GetPixmap()
   */
  Any GetPixmap() const;

  /**
   * @copydoc Dali::OffscreenImage::GetPixels()
   */
  bool GetPixels(std::vector<unsigned char> &pixbuf, unsigned int &width, unsigned int &height, Pixel::Format& pixelFormat ) const;

  /**
   * @copydoc Dali::OffscreenImage::EncodeToFile(const std::string& )
   */
  bool EncodeToFile(const std::string& filename) const;

  /**
   * destructor
   */
  ~OffscreenImage();

  /**
   * @copydoc Dali::OffscreenImage::GlExtensionCreate()
   */
  bool GlExtensionCreate();

  /**
   * @copydoc Dali::OffscreenImage::GlExtensionDestroy()
   */
  void GlExtensionDestroy();

  /**
   * @copydoc Dali::OffscreenImage::TargetTexture()
   */
  unsigned int TargetTexture();

  /**
   * @copydoc Dali::OffscreenImage::GetWidth()
   */
  unsigned int GetWidth() const
  {
    return mWidth;
  }

  /**
   * @copydoc Dali::OffscreenImage::GetHeight()
   */
  unsigned int GetHeight() const
  {
    return mHeight;
  }

  /**
   * @copydoc Dali::OffscreenImage::RequiresBlending()
   */
  bool RequiresBlending() const
  {
    return mBlendingRequired;
  }

private:

  /**
   * Private constructor; @see OffscreenImage::New()
   * @param[in] width The width of the image.
   * @param[in] height The height of the image.
   * @param[in] colour depth of the pixmap
   * @param[in] pixmap contains either: pixmap of type X11 Pixmap , a Ecore_X_Pixmap or is empty
   */
  OffscreenImage(unsigned int width,
              unsigned  int height,
              Dali::OffscreenImage::ColorDepth depth,
              Any pixmap);

  /**
   * 2nd phase construction.
   */
  void Initialize();

  /**
   * Decide whether blending is required based on the color depth.
   * @param depth the PixelImage depth enum
   */
  void SetBlending(Dali::OffscreenImage::ColorDepth depth);

  /**
   * Given an existing pixmap, the function uses X to find out
   * the width, heigth and depth of that pixmap.
   */
  void GetPixmapDetails();

private:

  unsigned int mWidth;                        ///< pixmap width
  unsigned int mHeight;                       ///< pixmap heights
  bool mOwnPixmap;                            ///< Whether we created pixmap or not
  bool mBlendingRequired;                      ///< Whether blending is required
  Dali::OffscreenImage::ColorDepth mColorDepth;  ///< color depth of pixmap
  void* mEglImageKHR;                         ///< From EGL extension
  EglImageExtensions* mEglImageExtensions;    ///< The EGL Image Extensions
};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_OFFSCREEN_IMAGE_H__
