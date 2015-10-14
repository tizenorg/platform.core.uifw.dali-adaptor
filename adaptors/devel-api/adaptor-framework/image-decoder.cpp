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

// CLASS HEADER
#include "image-decoder.h"

// EXTERNAL INCLUDES
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/resource-types.h>

// INTERNAL INCLUDES
#include "image-loaders/image-loader.h"

namespace Dali
{

namespace ImageDecoder
{
Integration::BitmapPtr DecodeBuffer( Dali::Vector<char>& buffer,
                                     bool orientationCorrection )
{
 return DecodeBuffer( buffer, ImageDimensions( 0, 0 ),  FittingMode::DEFAULT, SamplingMode::BOX_THEN_LINEAR, orientationCorrection);
}

Integration::BitmapPtr DecodeBuffer( Dali::Vector<char>& buffer,
                                     ImageDimensions size,
                                     FittingMode::Type fittingMode,
                                     SamplingMode::Type samplingMode,
                                     bool orientationCorrection )
{
  Integration::BitmapResourceType resourceType( size, fittingMode, samplingMode, orientationCorrection );

  return TizenPlatform::ImageLoader::DecodeBuffer( resourceType, &buffer[0], buffer.Count() );
}

} //ImageDecoder

} //Dali
