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
#include <imf-manager-impl.h>

// EXTERNAL INCLUDES
#include <boost/bind.hpp>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/object/singleton-service.h>
#include <adaptor.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <window-render-surface.h>
#include <adaptor-impl.h>
#include <virtual-keyboard-impl.h>

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
  return ImfManager::Get();
}

TypeRegistration IMF_MANAGER_TYPE( typeid(Dali::ImfManager), typeid(Dali::BaseHandle), Create );

} // unnamed namespace

bool ImfManager::IsAvailable()
{
  return false;
}

Dali::ImfManager ImfManager::Get()
{
  // Return empty handle as not supported
  return Dali::ImfManager();
}

ImfManager::~ImfManager()
{
}

void ImfManager::DeleteContext()
{
}

void ImfManager::ConnectCallbacks()
{
}

void ImfManager::DisconnectCallbacks()
{
}

void ImfManager::Activate()
{
}

void ImfManager::Deactivate()
{
}

void ImfManager::Reset()
{
}

Ecore_IMF_Context* ImfManager::GetContext()
{
  return mIMFContext;
}

bool ImfManager::RestoreAfterFocusLost() const
{
  return mRestoreAfterFocusLost;
}

void ImfManager::SetRestoreAfterFocusLost( bool toggle )
{
  mRestoreAfterFocusLost = toggle;
}

void ImfManager::PreEditChanged( void *, Ecore_IMF_Context *imfContext, void *event_info )
{
}

void ImfManager::CommitReceived( void *, Ecore_IMF_Context *imfContext, void *event_info )
{
}

Eina_Bool ImfManager::RetrieveSurrounding( void *data, Ecore_IMF_Context *imfContext, char** text, int* cursorPosition )
{
  return EINA_TRUE;
}

void ImfManager::DeleteSurrounding( void *data, Ecore_IMF_Context *imfContext, void *event_info )
{
}

void ImfManager::NotifyCursorPosition()
{
}

int ImfManager::GetCursorPosition()
{
  return mIMFCursorPosition;
}

void ImfManager::SetCursorPosition( unsigned int cursorPosition )
{
  mIMFCursorPosition = ( int )cursorPosition;
}

void ImfManager::SetSurroundingText( std::string text )
{
  mSurroundingText = text;
}

std::string ImfManager::GetSurroundingText()
{
  return mSurroundingText;
}

} // Adaptor

} // Internal

} // Dali
