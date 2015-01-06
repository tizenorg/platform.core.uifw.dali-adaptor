
#include <tizen-buffer-image.h>
#include <tizen-buffer-image-impl.h>


namespace Dali
{

TizenBufferImagePtr TizenBufferImage::New(tbm_surface_h* surface)
{
  TizenBufferImagePtr image = new TizenBufferImage(surface);
  return image;
}

TizenBufferImage::TizenBufferImage(tbm_surface_h* surface)
{
  mImpl = Internal::Adaptor::TizenBufferImage::New(surface);
}

TizenBufferImage::~TizenBufferImage()
{
}

void TizenBufferImage::SetSurface(tbm_surface_h* surface)
{
  mImpl->SetSurface(surface);
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

Pixel::Format TizenBufferImage::GetPixelFormat() const
{
  return mImpl->GetPixelFormat();
}

} // namespace Dali
