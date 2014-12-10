#ifndef __DALI_SLP_PLATFORM_CAPABILITIES_H__
#define __DALI_SLP_PLATFORM_CAPABILITIES_H__

/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

#include <dali/public-api/math/math-utils.h>

namespace Dali
{

namespace SlpPlatform
{

/**
 * Returns true if non power of two textures are supported.
 */
inline bool SupportsNonPowerOfTwoTextures()
{
#ifdef NON_POWER_OF_TWO_TEXTURES
  return true; // Redwood 8974 and desktop builds have NPOT
#else
#error "NPOT are standard in GLES 2.0 if mipmaps are not used, they are standard with mipmaps and no restrictions in GLES 3.0, requiring them simplifies image handling code."
  return false;
#endif
}

/**
 * Returns the size that a textures dimension should have for a specific image size.
 */
inline unsigned int GetTextureDimension( unsigned int size )
{
#ifdef NON_POWER_OF_TWO_TEXTURES
  return size;
#else
#error "NPOT are standard in GLES 2.0 if mipmaps are not used, they are standard with mipmaps and no restrictions in GLES 3.0, requiring them simplifies image handling code."
  return NextPowerOfTwo( size );
#endif
}

} // namespace SlpPlatform

} // namespace Dali

#endif // __DALI_SLP_PLATFORM_CAPABILITIES_H__
