#ifndef __DALI_IMAGE_DECODER_H__
#define __DALI_IMAGE_DECODER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 */

#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/images/image-operations.h>

namespace Dali
{

namespace Integration
{
class Bitmap;
} // namespace Integration

namespace ImageDecoder
{
/**
 * @brief Decode a buffer of data into bitmap.
 *
 * @param[in] buffer The buffer of data.
 * @param[in] orientationCorrection Re-orient the image to respect any orientation metadata.
 *
 * @return A pointer to the decoded buffer.
 */
DALI_IMPORT_API IntrusivePtr<Integration::Bitmap> DecodeBuffer( Dali::Vector<char>& buffer,
                                                                bool orientationCorrection = true);

/**
 * @brief Decode a buffer of data into bitmap.
 *
 * @param[in] buffer The buffer of data.
 * @param [in] size The width and height to fit the decoded bitmap to.
 * @param [in] fittingMode The method used when resizing images to fit desired dimensions.
 * @param [in] samplingMode The filtering method used to sample original pixels when resizing images .
 * @param [in] orientationCorrection Re-orient the image to respect any orientation metadatar.
 *
 * @return A pointer to the decoded buffer.
 */
DALI_IMPORT_API IntrusivePtr<Integration::Bitmap> DecodeBuffer( Dali::Vector<char>& buffer,
                                                                ImageDimensions size,
                                                                FittingMode::Type fittingMode = FittingMode::DEFAULT,
                                                                SamplingMode::Type samplingMode = SamplingMode::BOX_THEN_LINEAR,
                                                                bool orientationCorrection = true);
}

}

#endif // __DALI_IMAGE_DECODER_H__
