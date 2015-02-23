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
#include <window-extensions.h>

// EXTERNAL INCLUDES
#include <sstream>
#include <Ecore.h>
#include <Ecore_X.h>

// INTERNAL INCLUDES
#include <window.h>

namespace
{
const char* EFFECT_ENABLE_POSTFIX = ":1";
const char* EFFECT_DISABLE_POSTFIX = ":0";

void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = ",")
{
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (std::string::npos != pos || std::string::npos != lastPos)
  {
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
  }
}

}

namespace Dali
{
namespace WindowExtensions
{

void SetEffect( Window window, const std::string& effect )
{
  EffectContainer supportedEffects = GetSupportedEffects();

  std::vector< std::string >::iterator iter = std::find( supportedEffects.begin(), supportedEffects.end(), effect );
  DALI_ASSERT_ALWAYS( iter != supportedEffects.end() && "No available window effect." );

  Any nativeWindow = window.GetNativeHandle();
  if ( !nativeWindow.Empty() )
  {
    EffectContainer appliedEffects = GetAppliedEffects( window );

    std::stringstream ss;
    ss << appliedEffects.size() << ":" << effect << EFFECT_ENABLE_POSTFIX;

    // Applied the window effect to the current window.
    Ecore_X_Window ecoreWindow;
    nativeWindow.Get(ecoreWindow);
    ecore_x_window_prop_property_set( ecoreWindow, ECORE_X_ATOM_E_WINDOW_AUX_HINT,
                                      ECORE_X_ATOM_STRING, 8,
                                      (void*)ss.str().c_str(), ss.str().size() + 1 );
  }
}

void RemoveEffect( Window window, const std::string& effect )
{
  EffectContainer appliedEffects = GetAppliedEffects( window );

  std::vector< std::string >::iterator iter = std::find( appliedEffects.begin(), appliedEffects.end(), effect );
  DALI_ASSERT_ALWAYS( iter != appliedEffects.end() && "This effect isn't enabled." );

  Any nativeWindow = window.GetNativeHandle();
  if ( !nativeWindow.Empty() )
  {
    std::stringstream ss;
    ss << iter - appliedEffects.begin() << ":" << effect << EFFECT_DISABLE_POSTFIX;

    // Remove the window effect from the current window.
    Ecore_X_Window ecoreWindow;
    nativeWindow.Get(ecoreWindow);
    ecore_x_window_prop_property_set( ecoreWindow, ECORE_X_ATOM_E_WINDOW_AUX_HINT,
                                      ECORE_X_ATOM_STRING, 8,
                                      (void*)ss.str().c_str(), ss.str().size() + 1 );
  }
}

EffectContainer GetSupportedEffects()
{
  Ecore_X_Window root = ecore_x_window_root_first_get();
  unsigned char* data = NULL;
  int n = 0;
  int res = ecore_x_window_prop_property_get(root, ECORE_X_ATOM_E_WINDOW_AUX_HINT_SUPPORTED_LIST,
                                             ECORE_X_ATOM_STRING, 0, &data, &n);

  EffectContainer supportedEffects;

  if ((res == 8) && (n > 0))
  {
    std::stringstream ss;
    ss << data;
    Tokenize(ss.str(), supportedEffects);
  }

  free(data);

  return supportedEffects;
}

EffectContainer GetAppliedEffects( Window window )
{
  EffectContainer appliedEffects;

  Any nativeWindow = window.GetNativeHandle();
  if ( !nativeWindow.Empty() )
  {
    Ecore_X_Window ecoreWindow;
    nativeWindow.Get( ecoreWindow );

    unsigned char* data = NULL;
    int n = 0;
    int res = ecore_x_window_prop_property_get( ecoreWindow, ECORE_X_ATOM_E_WINDOW_AUX_HINT_SUPPORTED_LIST,
                                                ECORE_X_ATOM_STRING, 0, &data, &n );

    if ((res == 8) && (n > 0))
    {
      std::stringstream ss;
      ss << data;
      Tokenize(ss.str(), appliedEffects);
    }

    free(data);
  }

  return appliedEffects;
}

} // namespace WindowExtensions
} // namespace Dali


