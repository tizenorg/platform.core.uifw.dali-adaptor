#ifndef __DALI_DRAG_AND_DROP_DETECTOR_H__
#define __DALI_DRAG_AND_DROP_DETECTOR_H__

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

// EXTERNAL INCLUDES
#include <boost/function.hpp>

#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class DragAndDropDetector;
}
}

/**
 * The DragAndDropDetectors provides signals when draggable objects are dragged into our window.
 * It provides signals for when the draggable object enters our window, moves around in our window,
 * leaves our window and when it is finally dropped into our window.
 * The basic usage is shown below:
 *
 * @code
 *
 *  void Example()
 *  {
 *    DragAndDropDetector detector( window::GetDragAndDropDetector() );
 *
 *    // Get notifications when the draggable item enters our window
 *    detector.SignalEntered().Connect( &OnEntered );
 *
 *    // Get notifications when the draggable item leaves our window
 *    detector.SignalExited().Connect( &OnExited );
 *
 *    // Get notifications when the draggable item is moved within our window
 *    detector.SignalMoved().Connect( &OnMoved );
 *
 *    // Get notifications when the draggable item is dropped
 *    detector.SignalDropped().Connect( &OnDropped );
 *  }
 *
 *  void OnEntered( DragAndDropDetector detector )
 *  {
 *    // Change mode as required
 *  }
 *
 *  void OnExited( DragAndDropDetector detector )
 *  {
 *    // Change mode as required
 *  }
 *
 *  void OnMoved( DragAndDropDetector detector )
 *  {
 *    // Query the new values
 *    std::cout << "Position = " << detector.GetCurrentScreenPosition() << std::endl;
 *  }
 *
 *  void OnDropped( DragAndDropDetector detector )
 *  {
 *    // Query the new values
 *    std::cout << "Position = " << detector.GetCurrentScreenPosition() << ", Content = " << detector.GetContent() << std::endl;
 *  }
 *
 * @endcode
 */
class DragAndDropDetector : public BaseHandle
{
public:

  // Typedefs

  // Drag & Drop
  typedef SignalV2< void ( DragAndDropDetector ) > DragAndDropSignalV2;

  //typedef boost::signals2::signal_type< void ( DragAndDropDetector ), DummyMutex >::type DragAndDropSignalType;
  //typedef boost::function<void ( DragAndDropDetector )> DragAndDropCallbackType;
  //typedef Signal<DragAndDropSignalType, DragAndDropCallbackType> DragAndDropSignal;

  // Signal Names
  static const char* const SIGNAL_ENTERED;
  static const char* const SIGNAL_EXITED;
  static const char* const SIGNAL_MOVED;
  static const char* const SIGNAL_DROPPED;

  /**
   * Create an uninitialized handle.
   * This can be initialized by calling getting the detector from Dali::Window.
   */
  DragAndDropDetector();

  /**
   * Virtual Destructor.
   */
  virtual ~DragAndDropDetector();

  /**
   * Returns the dropped content.
   * @return A reference to the string representing the dropped content.
   */
  const std::string& GetContent() const;

  /**
   * Returns the current position of the dragged object.  This is the dropped position when an object
   * is dropped.
   * @return The current screen position.
   */
  Vector2 GetCurrentScreenPosition() const;

public:  // Signals

  /**
   * This is emitted when a dragged object enters a DALi window.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  //DragAndDropSignal SignalEntered();
  DragAndDropSignalV2& EnteredSignal();

  /**
   * This is emitted when a dragged object leaves a DALi window.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  //DragAndDropSignal SignalExited();
  DragAndDropSignalV2& ExitedSignal();

  /**
   * @deprecated
   * This is emitted when a dragged object is moved within the DALi window.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( DragAndDropDetector detector );
   * @endcode
   * This will be replaced by a property notification system once that is in place.
   * @return The signal to connect to.
   */
  //DragAndDropSignal SignalMoved();
  DragAndDropSignalV2& MovedSignal();

  /**
   * This is emitted when a dragged object is dropped within a DALi window.
   * A callback of the following type may be connected:
   * @code
   *   void YourCallback( DragAndDropDetector detector );
   * @endcode
   * @return The signal to connect to.
   */
  //DragAndDropSignal SignalDropped();
  DragAndDropSignalV2& DroppedSignal();

public: // Not intended for application developers

  /**
   * This constructor is used by DragAndDropDetector::Get().
   * @param[in] detector A pointer to the drag and drop detector.
   */
  DragAndDropDetector( Internal::Adaptor::DragAndDropDetector* detector );
};

} // namespace Dali

#endif // __DALI_DRAG_AND_DROP_DETECTOR_H__
