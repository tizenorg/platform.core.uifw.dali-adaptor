#ifndef _DALI_PLATFORM_IMAGE_LOADER_CLIENT_H_
#define _DALI_PLATFORM_IMAGE_LOADER_CLIENT_H_
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

// EXTERNAL INCLUDES

namespace Dali
{
namespace SlpPlatform
{

/**
 * @brief Abstract interface to the caller of an image loader.
 */
class ImageLoaderClient
{
public:
  /**
   * @brief Check whether the current request has been cancelled.
   *
   * This will throw an exception to unwind the stack if the current request
   * has been cancelled.
   * @note Only place calls to this function at exception-safe locations in loader code.
   **/
  virtual void InterruptForCancellation() const = 0;

protected:
  ImageLoaderClient() {}
  /** Destruction of an object through this interface is not allowed. */
  ~ImageLoaderClient() {}

private:
  ImageLoaderClient( const ImageLoaderClient& rhs );
  ImageLoaderClient& operator =( ImageLoaderClient& rhs );
};

} /* namespace SlpPlatform */
} /* namespace Dali */

#endif /* _DALI_PLATFORM_IMAGE_LOADER_CLIENT_H_ */
