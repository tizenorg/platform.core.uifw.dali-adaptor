/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali/public-api/adaptor-framework/egl-buffer.h>

// INTERNAL INCLUDES
#include <egl-buffer-impl.h>

namespace Dali
{

EglBufferPtr EglBuffer::New(unsigned int width, unsigned int height, ColorDepth depth, IEglBufferRenderer* renderer)
{
  EglBufferPtr buffer = new EglBuffer(width, height, depth, renderer);
  return buffer;
}

bool EglBuffer::GlExtensionCreate()
{
  return mImpl->GlExtensionCreate();
}

void EglBuffer::GlExtensionDestroy()
{
  mImpl->GlExtensionDestroy();
}

unsigned int EglBuffer::TargetTexture()
{
  return mImpl->TargetTexture();
}

void EglBuffer::TextureCreated(unsigned textureId)
{
  mImpl->TextureCreated(textureId);
}

void EglBuffer::PrepareTexture()
{
  mImpl->PrepareTexture();
}

void EglBuffer::Resize( const Vector2& newSize )
{
  mImpl->Resize(newSize);
}

bool EglBuffer::IsReady()
{
  return mImpl->IsReady();
}

unsigned int EglBuffer::GetWidth() const
{
  return mImpl->GetWidth();
}

unsigned int EglBuffer::GetHeight() const
{
  return mImpl->GetHeight();
}

bool EglBuffer::RequiresBlending() const
{
  return mImpl->RequiresBlending();
}

void EglBuffer::GlContextCreated()
{
  mImpl->GlContextCreated();
}

void EglBuffer::GlContextDestroyed()
{
  mImpl->GlContextDestroyed();
}

EglBuffer::EglBuffer(unsigned int width, unsigned int height, ColorDepth depth, IEglBufferRenderer* renderer)
{
  Dali::Adaptor& adaptor = Dali::Adaptor::Get();
  mImpl = Internal::Adaptor::EglBuffer::New( width, height, depth, adaptor, renderer);
}

EglBuffer::~EglBuffer()
{
  delete mImpl;
}

void EglBuffer::ClearRenderer()
{
  mImpl->ClearRenderer();
}

} // namespace Dali
