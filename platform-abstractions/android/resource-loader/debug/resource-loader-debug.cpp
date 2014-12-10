/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

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

#include "resource-loader-debug.h"

#if defined(DEBUG_ENABLED)

#include <iostream>
#include <sstream>
#include <string>
#include <dali/integration-api/glyph-set.h>

namespace Dali
{
namespace SlpPlatform
{
using namespace Dali::Integration;

/**
 * Filter for resource loader debug. Change levels here to turn on debugging
 */
Debug::Filter* gLoaderFilter = Debug::Filter::New(Debug::Concise, false, "LOG_RESOURCE_LOADER");


std::string DebugRequestList(TextResourceType::CharacterList& chars)
{
  std::string textString;

  for(std::size_t i=0, length=chars.size(); i<length; i++)
  {
    std::ostringstream oss;
    const TextResourceType::GlyphPosition& glyph = chars[i];
    oss << static_cast<char>(glyph.character) << "(" << glyph.xPosition << "," << glyph.yPosition
        << " " << (glyph.quality?"Hg":"Lw") << " " << (glyph.loaded?"LD":"UN") << ") ";
    textString.append(oss.str());
  }
  return textString;
}

std::string DebugResourceList(LoadedResource& partialResource)
{
  GlyphSetPointer glyphSet = static_cast<GlyphSet*>(partialResource.resource.Get());
  const GlyphSet::CharacterList& characterList = glyphSet->GetCharacterList();
  std::string textString;

  for(std::size_t i=0, length=characterList.size(); i<length; i++)
  {
    std::ostringstream oss;
    const GlyphMetrics& gm = characterList[i].second;
    oss << static_cast<char>(gm.code) << "(" << gm.xPosition << "," << gm.yPosition
        << " " << (gm.quality?"H":"L") << ") ";
    textString.append(oss.str());
  }
  return textString;
}

} //SlpPlatform
} //Dali

#endif
