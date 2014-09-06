/*
Copyright (c) 2000-2012 Samsung Electronics Co., Ltd All Rights Reserved

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

// CLASS HEADER
#include "style-monitor-impl.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <internal/common/adaptor-impl.h>

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
  BaseHandle handle( NULL );
  return handle;
}
TypeRegistration STYLE_MONITOR_TYPE( typeid(Dali::StyleMonitor), typeid(Dali::BaseHandle), Create, true /* Create Instance At Startup */ );

} // unnamed namespace

Dali::StyleMonitor StyleMonitor::Get()
{
  Dali::StyleMonitor styleMonitor;
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
