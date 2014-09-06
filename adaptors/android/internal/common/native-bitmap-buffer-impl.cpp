/*
  Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

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

// CLASS HEADER
#include "native-bitmap-buffer-impl.h"
#include <dali/integration-api/debug.h>
#include <dali/integration-api/bitmap.h>


// EXTERNAL HEADERS

// INTERNAL HEADERS

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

NativeBitmapBuffer::NativeBitmapBuffer( Adaptor* adaptor, unsigned int width, unsigned int height, Pixel::Format pFormat )
: mWidth(width),
  mHeight(height),
  mPixelFormat(pFormat)
{
  DALI_ASSERT_ALWAYS( adaptor );
  mBuffer = new Integration::LocklessBuffer( width * height * Pixel::GetBytesPerPixel(pFormat) );
  mGlAbstraction = &(adaptor->GetGlAbstraction());
}

NativeBitmapBuffer::~NativeBitmapBuffer()
{
  delete mBuffer;
}

void NativeBitmapBuffer::PrepareTexture()
{
  DALI_ASSERT_ALWAYS( mBuffer );
  GLenum pixelFormat = GL_RGBA;
  GLenum pixelDataType = GL_UNSIGNED_BYTE;

  Integration::ConvertToGlFormat( mPixelFormat, pixelDataType, pixelFormat );

  const unsigned char* buf = mBuffer->Read();

  if( buf )
  {
    // The active texture has already been set to a sampler and bound.
    mGlAbstraction->TexImage2D( GL_TEXTURE_2D, 0, pixelFormat, mWidth, mHeight, 0, pixelFormat, pixelDataType, buf );
  }
}

void NativeBitmapBuffer::Write( const unsigned char *src, size_t size )
{
  mBuffer->Write( src, size );
}

bool NativeBitmapBuffer::GlExtensionCreate()
{
  return true;
}

void NativeBitmapBuffer::GlExtensionDestroy()
{
}

unsigned int NativeBitmapBuffer::TargetTexture()
{
  return 0;
}

unsigned int NativeBitmapBuffer::GetWidth() const
{
  return mWidth;
}

unsigned int NativeBitmapBuffer::GetHeight() const
{
  return mHeight;
}

Pixel::Format NativeBitmapBuffer::GetPixelFormat() const
{
  return mPixelFormat;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
