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
#include <key-grab.h>

// EXTERNAL INCLUDES
#include <Ecore_Wayland.h>

// INTERNAL INCLUDES
#include <window.h>
#include <key-impl.h>
#include <iostream>
using namespace std;

namespace Dali
{

namespace KeyGrab
{

bool GrabKey( Window window, Dali::KEY daliKey, KeyGrabMode grabMode )
{
  Ecore_Wl_Window_Keygrab_Mode wlGrabMode;
  if( grabMode == TOP_POSITION )
  {
    wlGrabMode = ECORE_WL_WINDOW_KEYGRAB_TOPMOST;
  }
  else if( grabMode == SHARED )
  {
    wlGrabMode = ECORE_WL_WINDOW_KEYGRAB_SHARED;
  }
  else
  {
    return false;
  }

  return ecore_wl_window_keygrab_set( AnyCast<Ecore_X_Window>( window.GetNativeHandle() ),
                                      Dali::Internal::Adaptor::KeyLookup::GetKeyName( daliKey ),
                                      EINA_UNUSED, EINA_UNUSED, EINA_UNUSED, wlGrabMode );
}

bool UngrabKey( Window window, Dali::KEY daliKey )
{
  return ecore_wl_window_keygrab_unset( AnyCast<Ecore_X_Window>( window.GetNativeHandle() ),
                                      Dali::Internal::Adaptor::KeyLookup::GetKeyName( daliKey ),
                                      EINA_UNUSED, EINA_UNUSED );
}

} // namespace KeyGrab

} // namespace Dali


