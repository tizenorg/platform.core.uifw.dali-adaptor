#include "android-image-data.h"
#include "nativeLogging.h"

#include <android-jobs.h>
#include <android/bitmap.h>


// -------------------------------------------------------------------------------------------------
// simple bi-linear filtering + small pixel/texel/pixmap support to make it easier to apply
// NOTE: this isn't optimised and has been developed just to see if concept works,
// further work is required.

// it isn't clean solution but will allow write new ratio of scaling from outside
float android_image_data_scale_ratio = 1.0f;

namespace
{
  struct Texel;
  struct Pixel
  {
    unsigned char r, g, b, a;
    Pixel(const Texel& tx);
  };
  struct Texel
  {
    float r, g, b, a;
    Texel(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
    Texel() {}
    Texel(const Pixel& px);
    inline struct Texel operator*(float val) { return Texel(r*val, g*val, b*val, a*val); }
    inline struct Texel operator+(const Texel& t) { return Texel(r+t.r, g+t.g, b+t.b, a+t.a); }
  };
  Pixel::Pixel(const Texel& tx)
  {
    r = (unsigned char)std::min(255, (int)floor(tx.r*255.0f));
    g = (unsigned char)std::min(255, (int)floor(tx.g*255.0f));
    b = (unsigned char)std::min(255, (int)floor(tx.b*255.0f));
    a = (unsigned char)std::min(255, (int)floor(tx.a*255.0f));
  }
  Texel::Texel(const Pixel& px)
  {
    r = (float)px.r/(float)255.0f;
    g = (float)px.g/(float)255.0f;
    b = (float)px.b/(float)255.0f;
    a = (float)px.a/(float)255.0f;
  }

  struct Pixmap
  {
    Pixel *pBuffer;
    int width, height;

    Pixmap() : width(0), height(0), pBuffer(0) {}

    inline Pixel getPixel(int x, int y) const { return pBuffer[x + width * y]; }
    inline void setPixel(const Pixel& p, int x, int y) const { pBuffer[x + width * y] = p; }
    inline Pixel getPixelBilinearUV(float u, float v) const
    {
      u *= (float)width - 0.5f;
      v *= (float)height - 0.5f;
      int x = floor(u);
      int y = floor(v);
      double u_ratio = u - x;
      double v_ratio = v - y;
      double u_opposite = 1 - u_ratio;
      double v_opposite = 1 - v_ratio;

      Texel x0y0(getPixel(x, y));
      Texel x1y0 = x == width ? x0y0 :  (Texel)getPixel(x+1, y);
      Texel x0y1 = y == height ? x0y0 : (Texel)getPixel(x, y+1);
      Texel x1y1 = x == width || y == height ? x0y0 : (Texel)getPixel(x+1, y+1);

      Pixel result(
            (x0y0*u_opposite+x1y0*u_ratio)*v_opposite+
            (x0y1*u_opposite+x1y1*u_ratio)*v_ratio
            );

      return result;
    }
  };

  Pixmap* pixmap_new(const void *data, int w, int h)
  {
    Pixmap *pDstPixmap = new Pixmap();
    pDstPixmap->pBuffer = (Pixel*)(malloc(w*h*sizeof(Pixel)));
    pDstPixmap->width = w;
    pDstPixmap->height = h;
    if(data)
      memcpy(pDstPixmap->pBuffer, data, w*h*4);
    return pDstPixmap;
  }

  Pixmap* pixmap_new_scaled(Pixmap *pSrcPixmap, int dstWidth, int dstHeight)
  {
    Pixmap *pDstPixmap = new Pixmap();
    pDstPixmap->pBuffer = (Pixel*)(malloc(dstWidth*dstHeight*4));
    pDstPixmap->width = dstWidth;
    pDstPixmap->height = dstHeight;
    float dx = (float)pSrcPixmap->width / (float)dstWidth;
    float dy = (float)pSrcPixmap->height / (float)dstHeight;
    float srcX = 0.0f, srcY = 0.0f;
    for(int y = 0; y < dstHeight; ++y)
    {
      srcX = 0.0f;
      for(int x = 0; x < dstWidth; ++x)
      {
        float u = srcX / (float)pSrcPixmap->width;
        float v = srcY / (float)pSrcPixmap->height;
        Pixel out = pSrcPixmap->getPixelBilinearUV(u, v);
        pDstPixmap->setPixel(out, x, y);
        srcX += dx;
      }
      srcY += dy;
    }
    return pDstPixmap;
  }

  void pixmap_free(Pixmap *pPixmap)
  {
    if(pPixmap->pBuffer)
      free(pPixmap->pBuffer);
    delete(pPixmap);
  }
}

Android::ImageDataV2::ImageDataV2() : mWidth(0),
                                      mHeight(0)
{
}

Android::ImageDataV2::~ImageDataV2()
{
}

Android::ImageDataV2::ImageDataV2(jobject bitmap) : mWidth(0),
                                                    mHeight(0)
{
  if (bitmap)
  {
    // This will temporarily get an environment from the JVM
    // and temporarily attach it to the current thread.
    Jobs::JavaEnv environment;

    // Get bitmap information
    AndroidBitmapInfo bmpInfo;
    AndroidBitmap_getInfo(environment, bitmap, &bmpInfo);
    size_t bitmapSize = bmpInfo.height * bmpInfo.stride;
    uint8_t* sourceAddress;
    int errorCode = AndroidBitmap_lockPixels(environment, bitmap, (void **)&sourceAddress);
    bool downscale = (android_image_data_scale_ratio < 1.0f);
    int w = 0, h = 0;
    if (errorCode == 0)
    {
      if(downscale)
      {
        Pixmap *pSrc, *pDst;
        w = (int)((float)bmpInfo.width*android_image_data_scale_ratio);
        h = (int)((float)bmpInfo.height*android_image_data_scale_ratio);
        if(w < 2)
          w = 2;
        if(h < 2)
          h = 2;
        pSrc = pixmap_new(sourceAddress, bmpInfo.width, bmpInfo.height);
        pDst = pixmap_new_scaled(pSrc, w, h);
        bitmapSize = w*h*4;
        mBuffer.resize(bitmapSize);
        memcpy(mBuffer.data(), pDst->pBuffer, bitmapSize);
        pixmap_free(pSrc);
        pixmap_free(pDst);
      }
      else
      {
        mBuffer.resize(bitmapSize);
        memcpy(mBuffer.data(), sourceAddress, bitmapSize);
      }

      AndroidBitmap_unlockPixels(environment, bitmap);
    }
    else
    {
      LOGE("Locking bitmap failed with error %d.\n", errorCode);
    }

    // Update information
    if(downscale)
    {
      mWidth = w;
      mHeight = h;
      mStride = w<<2;
    }
    else
    {
      mWidth = bmpInfo.width;
      mHeight = bmpInfo.height;
      mStride = bmpInfo.stride;
    }
    mFormat = bmpInfo.format;
  }
}
