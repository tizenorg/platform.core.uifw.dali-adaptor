#ifndef __DALI_INTERNAL_NATIVE_IMAGE_SOURCE_EXTENSION_H__
#define __DALI_INTERNAL_NATIVE_IMAGE_SOURCE_EXTENSION_H__

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

// INTERNAL INCLUDES
#include <dali/devel-api/images/native-image-interface-extension.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{
const char* FRAGMENT_PREFIX = "#extension GL_OES_EGL_image_external:require\n";
const char* SAMPLER_TYPE = "samplerExternalOES";
} // namespace

/**
 * Dali internal NativeImageSource.
 */
class NativeImageSourceExtension: public NativeImageInterface::Extension
{
public:

  const char* GetCustomFragmentPreFix()
  {
    return FRAGMENT_PREFIX;
  }

  const char* GetCustomSamplerTypename()
  {
    return SAMPLER_TYPE;
  }

};

} // namespace Adaptor

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_NATIVE_IMAGE_SOURCE_EXTENSION_H__
