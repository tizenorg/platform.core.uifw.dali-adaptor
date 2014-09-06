#include "android-image-data.h"
#include "nativeLogging.h"

#include <android-jobs.h>
#include <android/bitmap.h>


Android::ImageDataV2::ImageDataV2() : mWidth(0),
                                      mHeight(0)
{
}

Android::ImageDataV2::~ImageDataV2()
{
}

#if 0
Android::ImageDataV2::ImageDataV2(jobject bitmap) : mWidth(0),
                                                    mHeight(0)
{
  if (bitmap)
  {
    // This will temporarily get an environment from the JVM
    // and temporarily attach it to the current thread.

    // Get bitmap information
    AndroidBitmapInfo bmpInfo;
    AndroidBitmap_getInfo(environment, bitmap, &bmpInfo);
    const size_t bitmapSize = bmpInfo.height * bmpInfo.stride;
    uint8_t* sourceAddress;
    int errorCode = AndroidBitmap_lockPixels(environment, bitmap, (void **)&sourceAddress);
    if (errorCode == 0)
    {
      mBuffer.resize(bitmapSize);
      memcpy(mBuffer.data(), sourceAddress, bitmapSize);
      AndroidBitmap_unlockPixels(environment, bitmap);
    }
    else
    {
      LOGE("Locking bitmap failed with error %d.\n", errorCode);
    }

    // Update information
    mWidth = bmpInfo.width;
    mHeight = bmpInfo.height;
    mStride = bmpInfo.stride;
    mFormat = bmpInfo.format;
  }
}
#endif