/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

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
#include "drag-and-drop-detector-impl.h"

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

Dali::DragAndDropDetector DragAndDropDetector::New()
{
  Dali::DragAndDropDetector detector = Dali::DragAndDropDetector(new DragAndDropDetector());

  return detector;
}

const std::string& DragAndDropDetector::GetContent() const
{
  return mContent;
}

Vector2 DragAndDropDetector::GetCurrentScreenPosition() const
{
  return mScreenPosition;
}

bool DragAndDropDetector::IsEnabled() const
{
  return !mDroppedSignalV2.Empty() || !mEnteredSignalV2.Empty() || !mExitedSignalV2.Empty() || !mMovedSignalV2.Empty() ;
}

void DragAndDropDetector::SetContent( const std::string& content )
{
  mContent = content;
}

void DragAndDropDetector::ClearContent()
{
  mContent.clear();
}

void DragAndDropDetector::SetPosition( Vector2 screenPosition )
{
  mScreenPosition = screenPosition;
}

void DragAndDropDetector::EmitEnteredSignal()
{
  if ( !mEnteredSignalV2.Empty() )
  {
    Dali::DragAndDropDetector handle( this );
    mEnteredSignalV2.Emit( handle );
  }
}

void DragAndDropDetector::EmitExitedSignal()
{
  if ( !mExitedSignalV2.Empty() )
  {
    Dali::DragAndDropDetector handle( this );
    mExitedSignalV2.Emit( handle );
  }
}

void DragAndDropDetector::EmitMovedSignal()
{
  if ( !mMovedSignalV2.Empty() )
  {
    Dali::DragAndDropDetector handle( this );
    mMovedSignalV2.Emit( handle );
  }
}

void DragAndDropDetector::EmitDroppedSignal()
{
  if ( !mDroppedSignalV2.Empty() )
  {
    Dali::DragAndDropDetector handle( this );
    mDroppedSignalV2.Emit( handle );
  }
}

DragAndDropDetector::DragAndDropDetector()
: mContent(),
  mScreenPosition()
{
}

DragAndDropDetector::~DragAndDropDetector()
{
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
