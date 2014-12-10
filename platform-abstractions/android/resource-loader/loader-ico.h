#ifndef __DALI_SLP_PLATFORM_LOADER_ICO_H__
#define __DALI_SLP_PLATFORM_LOADER_ICO_H__
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
namespace Ico
{
//00 00 01 00 01 00 20 20
const unsigned char MAGIC_BYTE_1 = 0x00;
const unsigned char MAGIC_BYTE_2 = 0x00;
}

bool LoadBitmapFromIco(FILE *fp, Integration::Bitmap& bitmap, ImageAttributes& attributes);

bool LoadIcoHeader(FILE *fp, unsigned int &width, unsigned int &height);

}

}
#endif
