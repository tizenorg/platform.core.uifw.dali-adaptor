#ifndef __DALI_INTERNAL_DRAG_AND_DROP_DETECTOR_H__
#define __DALI_INTERNAL_DRAG_AND_DROP_DETECTOR_H__

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
#include <string>

#include <dali/public-api/object/base-object.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/adaptor-framework/common/drag-and-drop-detector.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

typedef IntrusivePtr< DragAndDropDetector > DragAndDropDetectorPtr;

/**
 * This class listens to Drag & Drop events.
 */
class DragAndDropDetector : public Dali::BaseObject
{
public:

  typedef Dali::DragAndDropDetector::DragAndDropSignalV2 DragAndDropSignalV2;

  // Creation

  /**
   * Create a DragAndDropDetector.
   * This should only be called once by the Window class.
   * @return A newly allocated drag-and-drop-detector.
   */
  static Dali::DragAndDropDetector New();

  // Public API

  /**
   * @copydoc Dali::DragAndDropDetector::GetContent() const
   */
  const std::string& GetContent() const;

  /**
   * @copydoc Dali::DragAndDropDetector::GetCurrentScreenPosition() const
   */
  Vector2 GetCurrentScreenPosition() const;

  // Called by Drag & Drop Event Handler

  /**
   * Queries whether drag & drop behaviour is really required.
   * @return true if drag & drop required, false otherwise.
   */
  bool IsEnabled() const;

  /**
   * Sets the dragged content.
   * @param[in] content  A string that represents the content that has been dropped.
   */
  void SetContent( const std::string& content );

  /**
   * Clears the stored content.
   */
  void ClearContent();

  /**
   * Sets the position the drop occurred.
   */
  void SetPosition( Vector2 screenPosition );

  /**
   * Called when a draggable object enters our window.
   */
  void EmitEnteredSignal();

  /**
   * Called when a draggable object leaves our window.
   */
  void EmitExitedSignal();

  /**
   * Called when a draggable object leaves our window.
   */
  void EmitMovedSignal();

  /**
   * Is called when a drop actually occurs.
   */
  void EmitDroppedSignal();

public: // Signals

  /**
   * @copydoc Dali::DragAndDropDetector::EnteredSignal
   */
  DragAndDropSignalV2& EnteredSignal()
  {
    return mEnteredSignalV2;
  }

  /**
   * @copydoc Dali::DragAndDropDetector::ExitedSignal
   */
  DragAndDropSignalV2& ExitedSignal()
  {
    return mExitedSignalV2;
  }

  /**
   * @copydoc Dali::DragAndDropDetector::MovedSignal
   */
  DragAndDropSignalV2& MovedSignal()
  {
    return mMovedSignalV2;
  }

  /**
   * @copydoc Dali::DragAndDropDetector::DroppedSignal
   */
  DragAndDropSignalV2& DroppedSignal()
  {
    return mDroppedSignalV2;
  }

private:

  // Construction & Destruction

  /**
   * Constructor.
   */
  DragAndDropDetector();

  /**
   * Destructor.
   */
  virtual ~DragAndDropDetector();

  // Undefined
  DragAndDropDetector( const DragAndDropDetector& );
  DragAndDropDetector& operator=( DragAndDropDetector& );

private:

  std::string mContent;    ///< The current Drag & drop content.
  Vector2 mScreenPosition; ///< The screen position of the drop location.

  DragAndDropSignalV2 mEnteredSignalV2;
  DragAndDropSignalV2 mExitedSignalV2;
  DragAndDropSignalV2 mMovedSignalV2;
  DragAndDropSignalV2 mDroppedSignalV2;
};

} // namespace Adaptor

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Adaptor::DragAndDropDetector& GetImplementation(Dali::DragAndDropDetector& detector)
{
  DALI_ASSERT_ALWAYS( detector && "DragAndDropDetector handle is empty" );

  BaseObject& handle = detector.GetBaseObject();

  return static_cast<Internal::Adaptor::DragAndDropDetector&>(handle);
}

inline const Internal::Adaptor::DragAndDropDetector& GetImplementation(const Dali::DragAndDropDetector& detector)
{
  DALI_ASSERT_ALWAYS( detector && "DragAndDropDetector handle is empty" );

  const BaseObject& handle = detector.GetBaseObject();

  return static_cast<const Internal::Adaptor::DragAndDropDetector&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_DRAG_AND_DROP_DETECTOR_H__
