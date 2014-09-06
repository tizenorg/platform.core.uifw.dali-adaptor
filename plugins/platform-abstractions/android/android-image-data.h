#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include <cstddef>
#include <vector>
#include <dali/public-api/object/ref-object.h>
#include <boost/atomic.hpp>
#include <dali/public-api/math/vector4.h>
#include <jni.h>

namespace Android
{
  class ImageDataV2;
  typedef Dali::IntrusivePtr<ImageDataV2> ImageDataHandle;

struct NinePatchInfo
{
  NinePatchInfo() : isNinePatch(false)
  {
  }

  NinePatchInfo(int left, int top, int right, int bottom)
    : isNinePatch(true), border(left, top, right, bottom)
  {
    // Dali wants nine patch border to be in [left, top, right, bottom]
  }

  bool          isNinePatch;
  Dali::Vector4 border;
};

class ImageDataV2 : public Dali::RefObject
{
public:
  ImageDataV2();
  #if 0
  ImageDataV2(jobject bitmap);
  #endif
  ~ImageDataV2();

  uint8_t* GetBuffer()
  {
    return mBuffer.data();
  }

  int GetWidth() const
  {
    return mWidth;
  }

  int GetHeight() const
  {
    return mHeight;
  }

  size_t GetSize() const
  {
    return mBuffer.size();
  }

  size_t GetStride()
  {
    return mStride;
  }

  int GetFormat()
  {
    return mFormat;
  }

  bool empty() const
  {
    return mBuffer.empty();
  }

  bool IsNinePatch() const
  {
    return mNinePatchInfo.isNinePatch;
  }

  void SetNinePatchInfo(const NinePatchInfo &info)
  {
    // binary copy
    mNinePatchInfo = info;
  }

  const NinePatchInfo& GetNinePatchInfo()
  {
    return mNinePatchInfo;
  }


protected:
  std::vector<uint8_t>    mBuffer; // owned
  int                     mWidth;
  int                     mHeight;
  int                     mFormat;
  size_t                  mStride;

  NinePatchInfo           mNinePatchInfo;
};


}; // namespace Android

#endif // IMAGEDATA_H
