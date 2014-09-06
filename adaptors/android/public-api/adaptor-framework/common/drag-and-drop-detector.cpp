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
#include <dali/public-api/adaptor-framework/common/drag-and-drop-detector.h>

// INTERNAL INCLUDES
#include <internal/common/drag-and-drop-detector-impl.h>

namespace Dali
{

const char* const DragAndDropDetector::SIGNAL_ENTERED( "drag-and-drop-entered" );
const char* const DragAndDropDetector::SIGNAL_EXITED( "drag-and-drop-exited" );
const char* const DragAndDropDetector::SIGNAL_MOVED( "drag-and-drop-moved" );
const char* const DragAndDropDetector::SIGNAL_DROPPED( "drag-and-drop-dropped" );

DragAndDropDetector::DragAndDropDetector()
{
}

DragAndDropDetector::~DragAndDropDetector()
{
}

const std::string& DragAndDropDetector::GetContent() const
{
  return GetImplementation(*this).GetContent();
}

Vector2 DragAndDropDetector::GetCurrentScreenPosition() const
{
  return GetImplementation(*this).GetCurrentScreenPosition();
}

//DragAndDropDetector::DragAndDropSignal DragAndDropDetector::SignalEntered()
//{
//  return GetImplementation(*this).SignalEntered();
//}

DragAndDropDetector::DragAndDropSignalV2& DragAndDropDetector::EnteredSignal()
{
  return GetImplementation(*this).EnteredSignal();
}

//DragAndDropDetector::DragAndDropSignal DragAndDropDetector::SignalExited()
//{
//  return GetImplementation(*this).SignalExited();
//}

DragAndDropDetector::DragAndDropSignalV2& DragAndDropDetector::ExitedSignal()
{
  return GetImplementation(*this).ExitedSignal();
}

//DragAndDropDetector::DragAndDropSignal DragAndDropDetector::SignalMoved()
//{
//  return GetImplementation(*this).SignalMoved();
//}

DragAndDropDetector::DragAndDropSignalV2& DragAndDropDetector::MovedSignal()
{
  return GetImplementation(*this).MovedSignal();
}

//DragAndDropDetector::DragAndDropSignal DragAndDropDetector::SignalDropped()
//{
//  return GetImplementation(*this).SignalDropped();
//}

DragAndDropDetector::DragAndDropSignalV2& DragAndDropDetector::DroppedSignal()
{
  return GetImplementation(*this).DroppedSignal();
}

DragAndDropDetector::DragAndDropDetector( Internal::Adaptor::DragAndDropDetector* detector )
: BaseHandle( detector )
{
}

} // namespace Dali
