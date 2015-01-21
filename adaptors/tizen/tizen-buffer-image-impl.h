#ifndef __DALI_INTERNAL_TIZEN_BUFFER_IMAGE_H__
#define __DALI_INTERNAL_TIZEN_BUFFER_IMAGE_H__

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

// INTERNAL INCLUDES
#include <tizen-buffer-image.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
class EglImageExtensions;

/**
 * @brief Implementation of the TizenBufferImage class.
 */
class TizenBufferImage
{

public:

  /**
   * @copydoc Dali::TizenBufferImage::New()
   */
  static TizenBufferImage* New( tbm_surface_h* surface );

  /**
   * @copydoc Dali::TizenBufferImage::SetSurface()
   */
  void SetSurface( tbm_surface_h* surface );

  /**
   * @copydoc Dali::TizenBufferImage::GlExtensionCreate()
   */
  bool GlExtensionCreate();

  /**
   * @copydoc Dali::TizenBufferImage::GlExtensionDestroy()
   */
  void GlExtensionDestroy();

  /**
   * @copydoc Dali::TizenBufferImage::TargetTexture()
   */
  unsigned int TargetTexture();

  /**
   * @copydoc Dali::TizenBufferImage::PrepareTexture()
   */
  void PrepareTexture();

  /**
   * @copydoc Dali::TizenBufferImage::GetWidth()
   */
  unsigned int GetWidth() const;

  /**
   * @copydoc Dali::TizenBufferImage::GetHeight()
   */
  unsigned int GetHeight() const;

  /**
   * @copydoc Dali::TizenBufferImage::GetPixelFormat()
   */
  Pixel::Format GetPixelFormat() const;

private:

  /**
   * @copydoc Dali::TizenBufferImage::TizenBufferImage(tbm_surface_h*)
   */
  TizenBufferImage( tbm_surface_h* surface );

   /**
   * @copydoc Dali::TizenBufferImage::TizenBufferImage(const TizenBufferImage&)
   */
  TizenBufferImage( const TizenBufferImage& image );

  /**
   * @copydoc Dali::TizenBufferImage::~TizenBufferImage()
   */
  virtual ~TizenBufferImage();

  /**
   * @copydoc Dali::TizenBufferImage::operator=()
   */
  TizenBufferImage& operator=( const TizenBufferImage& image );

private:

  void* mEglImageKHR;                        ///< From EGL extension
  EglImageExtensions* mEglImageExtensions;   ///< The EGL image extensions
  tbm_surface_h* mTbmsurface;                ///< tbm_surface_h pointer
  unsigned int mWidth;                       ///< tbm surface width
  unsigned int mHeight;                      ///< tbm surface height

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif
