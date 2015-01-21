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
#include <tizen-buffer-image.h>

// INTERNAL INCLUDES
#include <tizen-buffer-image-impl.h>

namespace Dali
{

TizenBufferImagePtr TizenBufferImage::New( tbm_surface_h* surface )
{
  TizenBufferImagePtr image = new TizenBufferImage( surface );
  return image;
}

TizenBufferImage::TizenBufferImage( tbm_surface_h* surface )
{
  mImpl = Internal::Adaptor::TizenBufferImage::New( surface );
}

TizenBufferImage::~TizenBufferImage()
{
  delete mImpl;
}

void TizenBufferImage::SetSurface( tbm_surface_h* surface )
{
  mImpl->SetSurface( surface );
}

bool TizenBufferImage::GlExtensionCreate()
{
  return mImpl->GlExtensionCreate();
}

void TizenBufferImage::GlExtensionDestroy()
{
  return mImpl->GlExtensionDestroy();
}

unsigned int TizenBufferImage::TargetTexture()
{
  return mImpl->TargetTexture();
}

void TizenBufferImage::PrepareTexture()
{
}

unsigned int TizenBufferImage::GetWidth() const
{
  return mImpl->GetWidth();
}

unsigned int TizenBufferImage::GetHeight() const
{
  return mImpl->GetHeight();
}

bool TizenBufferImage::RequiresBlending() const
{
  return mImpl->RequiresBlending();
}

} // namespace Dali
