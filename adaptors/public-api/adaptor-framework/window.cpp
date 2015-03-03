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
#include <window.h>

// INTERNAL INCLUDES
#include <window-impl.h>
#include <orientation-impl.h>

namespace Dali
{

Window Window::New(PositionSize posSize, const std::string name, bool isTransparent)
{
  Internal::Adaptor::Window* window = Internal::Adaptor::Window::New(posSize, name, isTransparent);
  return Window(window);
}

Window::Window()
{
}

Window::~Window()
{
}

Window::Window(const Window& handle)
: BaseHandle(handle)
{
}

Window& Window::operator=(const Window& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void Window::SetIndicatorStyle( IndicatorStyle style )
{
  GetImplementation(*this).SetIndicatorStyle( style );
}

void Window::ShowIndicator( IndicatorVisibleMode visibleMode )
{
  GetImplementation(*this).ShowIndicator( visibleMode );
}

Window::IndicatorSignalType& Window::IndicatorVisibilityChangedSignal()
{
  return GetImplementation(*this).IndicatorVisibilityChangedSignal();
}

void Window::SetIndicatorBgOpacity( IndicatorBgOpacity opacity )
{
  GetImplementation(*this).SetIndicatorBgOpacity( opacity );
}

void Window::RotateIndicator( WindowOrientation orientation )
{
  GetImplementation(*this).RotateIndicator( orientation );
}

void Window::SetClass( std::string name, std::string klass )
{
  GetImplementation(*this).SetClass( name, klass );
}

void Window::Raise()
{
  GetImplementation(*this).Raise();
}

void Window::Lower()
{
  GetImplementation(*this).Lower();
}

void Window::Activate()
{
  GetImplementation(*this).Activate();
}

Orientation Window::GetOrientation()
{
  Internal::Adaptor::OrientationPtr orientation = GetImplementation(*this).GetOrientation();
  return Orientation(orientation.Get());
}

void Window::AddAvailableOrientation( WindowOrientation orientation )
{
  GetImplementation(*this).AddAvailableOrientation( orientation );
}

void Window::RemoveAvailableOrientation( WindowOrientation orientation )
{
  GetImplementation(*this).RemoveAvailableOrientation( orientation );
}

void Window::SetAvailableOrientations(const std::vector<Dali::Window::WindowOrientation>& orientations)
{
  GetImplementation(*this).SetAvailableOrientations( orientations );
}

const std::vector<Dali::Window::WindowOrientation>& Window::GetAvailableOrientations()
{
  return GetImplementation(*this).GetAvailableOrientations();
}

void Window::SetPreferredOrientation( Dali::Window::WindowOrientation orientation )
{
  GetImplementation(*this).SetPreferredOrientation( orientation );
}

Dali::Window::WindowOrientation Window::GetPreferredOrientation()
{
  return GetImplementation(*this).GetPreferredOrientation();
}

DragAndDropDetector Window::GetDragAndDropDetector() const
{
  return GetImplementation(*this).GetDragAndDropDetector();
}

Any Window::GetNativeHandle() const
{
  return GetImplementation(*this).GetNativeHandle();
}

void Window::SetEffect( const std::string& effect )
{
  GetImplementation(*this).SetEffect( effect );
}

void Window::RemoveEffect( const std::string& effect )
{
  GetImplementation(*this).RemoveEffect( effect );
}

const Window::WindowEffectList& Window::GetSupportedEffects()
{
  return GetImplementation(*this).GetSupportedEffects();
}

const Window::WindowEffectList& Window::GetAppliedEffects()
{
  return GetImplementation(*this).GetAppliedEffects();
}

Window::Window( Internal::Adaptor::Window* window )
: BaseHandle( window )
{
}

} // namespace Dali
