#ifndef __DALI_SLP_PLATFORM_LOADER_WBMP_H__
#define __DALI_SLP_PLATFORM_LOADER_WBMP_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/public-api/common/intrusive-ptr.h>

// EXTERNAL INCLUDES
#include <cstdio>

namespace Dali
{

namespace Integration
{
  class ImageData;
  typedef IntrusivePtr<ImageData> ImageDataPtr;
}
struct ImageAttributes;

namespace SlpPlatform
{

bool LoadBitmapFromWbmp(FILE *fp, ImageAttributes& attributes, Integration::ImageDataPtr& bitmap);

bool LoadWbmpHeader(FILE *fp, const ImageAttributes& attributes, unsigned int &width, unsigned int &height );

}

}
#endif
