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
#include "clipboard-event-notifier-impl.h"

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>

// INTERNAL INCLUDES
#include <adaptor-impl.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

Dali::ClipboardEventNotifier ClipboardEventNotifier::New()
{
  Dali::ClipboardEventNotifier notifier = Dali::ClipboardEventNotifier(new ClipboardEventNotifier());

  // Register the object
  GetImplementation(notifier).RegisterObject();

  return notifier;
}

const std::string& ClipboardEventNotifier::GetContent() const
{
  return mContent;
}

void ClipboardEventNotifier::SetContent( const std::string& content )
{
  mContent = content;
}

void ClipboardEventNotifier::ClearContent()
{
  mContent.clear();
}

void ClipboardEventNotifier::EmitContentSelectedSignal()
{
  if ( !mContentSelectedSignalV2.Empty() )
  {
    Dali::ClipboardEventNotifier handle( this );
    mContentSelectedSignalV2.Emit( handle );
  }
}

ClipboardEventNotifier::ClipboardEventNotifier()
: mContent()
{
}

ClipboardEventNotifier::~ClipboardEventNotifier()
{
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
