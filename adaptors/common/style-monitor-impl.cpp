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
#include "style-monitor-impl.h"

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <adaptor-impl.h>
#include <singleton-service-impl.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace
{

BaseHandle Create()
{
  BaseHandle handle( StyleMonitor::Get() );

  if ( !handle && Adaptor::IsAvailable() )
  {
    Dali::SingletonService service( SingletonService::Get() );
    if ( service )
    {
      Adaptor& adaptorImpl( Adaptor::GetImplementation( Adaptor::Get() ) );
      Dali::StyleMonitor styleMonitor = Dali::StyleMonitor( new StyleMonitor( adaptorImpl.GetPlatformAbstraction() ) );
      service.Register( typeid( styleMonitor ), styleMonitor );
      handle = styleMonitor;
    }
  }

  return handle;
}
TypeRegistration STYLE_MONITOR_TYPE( typeid(Dali::StyleMonitor), typeid(Dali::BaseHandle), Create, true /* Create Instance At Startup */ );

} // unnamed namespace

Dali::StyleMonitor StyleMonitor::Get()
{
  Dali::StyleMonitor styleMonitor;

  Dali::SingletonService service( SingletonService::Get() );
  if ( service )
  {
    // Check whether the singleton is already created
    Dali::BaseHandle handle = service.GetSingleton( typeid( Dali::StyleMonitor ) );
    if(handle)
    {
      // If so, downcast the handle
      styleMonitor = Dali::StyleMonitor( dynamic_cast< StyleMonitor* >( handle.GetObjectPtr() ) );
    }
  }

  return styleMonitor;
}

StyleMonitor::StyleMonitor(Integration::PlatformAbstraction& platformAbstraction)
: mPlatformAbstraction(platformAbstraction)
{
}

StyleMonitor::~StyleMonitor()
{
}

void StyleMonitor::StyleChanged(StyleChange styleChange)
{
  if (styleChange.defaultFontChange || styleChange.defaultFontSizeChange)
  {
    mPlatformAbstraction.UpdateDefaultsFromDevice();
  }

  EmitStyleChangeSignal(styleChange);
}

std::string StyleMonitor::GetDefaultFontFamily() const
{
  return mPlatformAbstraction.GetDefaultFontFamily();
}

float StyleMonitor::GetDefaultFontSize() const
{
  return mPlatformAbstraction.GetDefaultFontSize();
}

const std::string& StyleMonitor::GetTheme() const
{
  return mUserDefinedThemeFilePath;
}

void StyleMonitor::SetTheme(const std::string& path)
{
  StyleChange styleChange;
  styleChange.themeChange = true;
  styleChange.themeFilePath = path;
  mUserDefinedThemeFilePath = path;

  EmitStyleChangeSignal(styleChange);
}

Dali::StyleMonitor::StyleChangeSignalV2& StyleMonitor::StyleChangeSignal()
{
  return mStyleChangeSignalV2;
}

void StyleMonitor::EmitStyleChangeSignal(StyleChange styleChange)
{
  if( !mStyleChangeSignalV2.Empty() )
  {
    Dali::StyleMonitor handle( this );
    mStyleChangeSignalV2.Emit( handle, styleChange );
  }
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
