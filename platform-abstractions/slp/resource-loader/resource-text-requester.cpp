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

#include "resource-text-requester.h"

#include "debug/resource-loader-debug.h"

using namespace Dali::Integration;

namespace Dali
{

namespace SlpPlatform
{

namespace
{
const uint32_t FIRST_VISIBLE_CHAR( 0x21 ); // 0x20 is the white space

bool IsVisible(uint32_t code)
{
  return !(code < FIRST_VISIBLE_CHAR);
}

// helper to remove invisible characters
void RemoveInvisibleCharacters( Integration::TextResourceType::CharacterList& text )
{
  Integration::TextResourceType::CharacterList temp;
  temp.reserve(text.size());
  for(std::size_t i=0, length= text.size(); i< length; i++)
  {
    if (IsVisible( text[i].character ))
    {
      temp.push_back(text[i]);
    }
  }
  text = temp;
}

// helper to remove a single character
void RemoveCharacter( TextResourceType::CharacterList& text, uint32_t code)
{
  // the string doesn't have duplicates so we can early exit
  for(std::size_t i=0, length= text.size(); i< length; i++)
  {
    if(text[i].character == code )
    {
      text.erase(text.begin() + i);
      return;
    }
  }
}

} // Anonymous namespace

ResourceTextRequester::ResourceTextRequester( ResourceLoader& resourceLoader )
: ResourceRequesterBase( resourceLoader ),
  mThreadText( NULL )
{
}

ResourceTextRequester::~ResourceTextRequester()
{
  delete mThreadText;
}

void ResourceTextRequester::Pause()
{
  if( mThreadText )
  {
    mThreadText->Pause();
  }
}

void ResourceTextRequester::Resume()
{
  if( mThreadText )
  {
    mThreadText->Resume();
  }
}

void ResourceTextRequester::LoadResource( Integration::ResourceRequest& request )
{
  TextResourceType* textRequest = dynamic_cast<TextResourceType*>(request.GetType());
  if( textRequest )
  {
    TextResourceType::CharacterList& text(textRequest->mCharacterList);

    // Remove from stored request
    RemoveInvisibleCharacters( text );
    if( text.empty() )
    {
      IntrusivePtr<GlyphSet> cachedGlyphs(  new GlyphSet() );
      LoadedResource resource( request.GetId(), request.GetType()->id, cachedGlyphs );
      mResourceLoader.AddLoadedResource(resource);
    }
    else
    {
      CreateThread();
      mThreadText->AddRequest( request, ResourceThreadBase::RequestLoad );
    }
  }
}

LoadStatus ResourceTextRequester::LoadFurtherResources( ResourceRequest& request, LoadedResource partialResource )
{
  // nothing to do

  return RESOURCE_COMPLETELY_LOADED;
}

void ResourceTextRequester::SaveResource(const Integration::ResourceRequest& request )
{
  // Nothing to do
}

void ResourceTextRequester::CancelLoad(Integration::ResourceId id, Integration::ResourceTypeId typeId)
{
  if( mThreadText )
  {
    mThreadText->CancelRequest(id);
  }
}

void ResourceTextRequester::CreateThread()
{
  if( ! mThreadText )
  {
    mThreadText = new ResourceThreadText(  mResourceLoader , TextResourceType::TextQualityHigh );
  }
}

} // SlpPlatform

} // Dali
