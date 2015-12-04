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

// CLASS HEADER
#include "key-impl.h"

// EXTERNAL INCLUDES
#include <map>
#include <string.h>
#include <iostream>

#include <dali/integration-api/debug.h>


namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace KeyLookup
{

namespace
{

struct KeyLookup
{
  const char* keyName;      ///< X string representation
  const int   daliKeyCode;  ///< Dali Enum Representation
  const bool  deviceButton; ///< Whether the key is from a button on the device
};

// matches a DALI_KEY enum, to a X key name
KeyLookup KeyLookupTable[]=
{
  // more than one key name can be assigned to a single dali-key code
  // e.g. Menu and KEY_MENU("FS86KeyMenu") are both assigned to  DALI_KEY_MENU

  { "Escape",               DALI_KEY_ESCAPE,          false },  // item not defined in utilX
  { "Menu",                 DALI_KEY_MENU,            false },  // item not defined in utilX
};

const std::size_t KEY_LOOKUP_COUNT = (sizeof( KeyLookupTable))/ (sizeof(KeyLookup));

class KeyMap
{
  public:

  KeyMap():
  mLookup( cmpString )
  {
    // create the lookup
    for( size_t i = 0; i < KEY_LOOKUP_COUNT ; ++i )
    {
      const KeyLookup&  keyLookup( KeyLookupTable[i] );
      mLookup[ keyLookup.keyName  ] = DaliKeyType( keyLookup.daliKeyCode, keyLookup.deviceButton );
    }
  }

  int GetDaliKeyEnum( const char* keyName ) const
  {
    Lookup::const_iterator i = mLookup.find( keyName );
    if( i == mLookup.end() )
    {
      return -1;
    }
    else
    {
      return (*i).second.first;
    }
  }

  bool IsDeviceButton( const char* keyName ) const
  {
    Lookup::const_iterator i = mLookup.find( keyName );
    if ( i != mLookup.end() )
    {
      return (*i).second.second;
    }
    return false;
  }

  private:

  /**
   * compare function, to compare string by pointer
   */
  static bool cmpString( const char* a, const char* b)
  {
    return strcmp(a, b) < 0;
  }

  typedef std::pair< int, bool > DaliKeyType;
  typedef std::map<const char* /* key name */, DaliKeyType /* key code */, bool(*) ( char const* a, char const* b) > Lookup;
  Lookup mLookup;

};
const KeyMap globalKeyLookup;

} // un-named name space

bool IsKey( const Dali::KeyEvent& keyEvent, Dali::KEY daliKey)
{
  int key = globalKeyLookup.GetDaliKeyEnum( keyEvent.keyPressedName.c_str() );
  return daliKey == key;
}

bool IsDeviceButton( const char* keyName )
{
  return globalKeyLookup.IsDeviceButton( keyName );
}

} // namespace KeyLookup

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
