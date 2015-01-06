#ifndef __DALI_TIZEN_BUFFER_IMAGE_H__
#define __DALI_TIZEN_BUFFER_IMAGE_H__

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
#include <dali/public-api/images/native-image.h>
#include <tbm_surface.h>

namespace Dali
{
class Adaptor;

namespace Internal
{
namespace Adaptor
{
class TizenBufferImage;
} // namespace Adaptor
} // namespace Internal

class TizenBufferImage;

/**
 * @brief Pointer to Dali::TizenBufferImage.
 */
typedef IntrusivePtr< TizenBufferImage > TizenBufferImagePtr;

/**
 * @brief Class to display native Tizen Buffer Manager (TBM) surface.
 */
class TizenBufferImage : public NativeImage
{
public:

  /**
   * @brief Creates a new TizenBufferImage.
   * @param[in] surface The tbm_surface_h pointer with NULL as a default value
   * @return A smart-pointer to a newly allocated image
   */
  static TizenBufferImagePtr New( tbm_surface_h* surface = NULL);

  /**
   * @brief Sets new tbm_surface_h pointer.
   * @detailed TizenBufferImage is updated by the new tbm surface
   * @param[in] surface The tbm_surface_h pointer
   */
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

  /**
   * @brief Private constructor.
   * @param[in] surface The tbm_surface_h pointer
   */
  TizenBufferImage( tbm_surface_h* surface );

  /**
   * @brief Undefined copy constructor.
   */
  TizenBufferImage( const TizenBufferImage& image );

  /**
   * @brief Undefined assignment operator.
   */
  TizenBufferImage& operator=( const TizenBufferImage& image );

  /**
   * @brief Private destructor.
   */
  virtual ~TizenBufferImage();

private:
  Internal::Adaptor::TizenBufferImage* mImpl; ///< Implementation pointer
};


} // namespace Dali

#endif
