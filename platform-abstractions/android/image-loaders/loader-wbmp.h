#ifndef __DALI_SLP_PLATFORM_LOADER_WBMP_H__
#define __DALI_SLP_PLATFORM_LOADER_WBMP_H__
#include <cstdio>

namespace Dali
{

namespace Integration
{
  class Bitmap;
}
struct ImageAttributes;

namespace SlpPlatform
{

bool LoadBitmapFromWbmp(FILE *fp, Integration::Bitmap& bitmap, ImageAttributes& attributes);

bool LoadWbmpHeader(FILE *fp, unsigned int &width, unsigned int &height);

}

}
#endif

