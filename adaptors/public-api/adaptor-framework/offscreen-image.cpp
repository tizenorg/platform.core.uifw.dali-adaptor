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

// CLASS HEADER
#include <offscreen-image.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/any.h>

// INTERNAL INCLUDES
#include <offscreen-image-impl.h>

namespace Dali
{

OffscreenImagePtr OffscreenImage::New( unsigned int width, unsigned int height, ColorDepth depth )
{
  Any empty;
  OffscreenImagePtr image = new OffscreenImage( width, height, depth, empty );
  return image;
}

Any OffscreenImage::GetPixmap()
{
  return mImpl->GetPixmap();
}

OffscreenImagePtr OffscreenImage::New( Any pixmap )
{
  OffscreenImagePtr image = new OffscreenImage(0, 0, COLOR_DEPTH_DEFAULT, pixmap);
  return image;
}

bool OffscreenImage::GetPixels( std::vector<unsigned char> &pixbuf, unsigned int &width, unsigned int &height, Pixel::Format& pixelFormat ) const
{
  return mImpl->GetPixels( pixbuf, width, height, pixelFormat );
}

bool OffscreenImage::EncodeToFile(const std::string& filename) const
{
  return mImpl->EncodeToFile(filename);
}

bool OffscreenImage::GlExtensionCreate()
{
  return mImpl->GlExtensionCreate();
}

void OffscreenImage::GlExtensionDestroy()
{
  mImpl->GlExtensionDestroy();
}

unsigned int OffscreenImage::TargetTexture()
{
  return mImpl->TargetTexture();
}

void OffscreenImage::PrepareTexture()
{

}

unsigned int OffscreenImage::GetWidth() const
{
  return mImpl->GetWidth();
}

unsigned int OffscreenImage::GetHeight() const
{
  return mImpl->GetHeight();
}

bool OffscreenImage::RequiresBlending() const
{
  return mImpl->RequiresBlending();
}

OffscreenImage::OffscreenImage( unsigned int width, unsigned int height, ColorDepth depth, Any pixmap )
{
   mImpl = Internal::Adaptor::OffscreenImage::New( width, height, depth, pixmap );
}

OffscreenImage::~OffscreenImage()
{
  delete mImpl;
}

} // namespace Dali
