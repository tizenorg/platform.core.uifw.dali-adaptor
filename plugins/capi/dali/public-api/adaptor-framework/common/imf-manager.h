#ifndef IMFMANAGER_H
#define IMFMANAGER_H

/*
Copyright (c) 2000-2014 Samsung Electronics Co., Ltd All Rights Reserved

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class ImfManager;
}
}

// TODO: Temporary patch to hidden ecore dependency. Must fix it.
typedef void* ImfContext;

/**
 * @brief The ImfManager class
 * Specifically manages the ecore input method framework which enables the virtual or hardware keyboards.
 */
class ImfManager : public BaseHandle
{
public:

  enum ImfEvent
  {
    VOID,
    PREEDIT,
    COMMIT,
    DELETESURROUNDING,
    GETSURROUNDING
  };

  /**
   * This structure is used to pass on data from the IMF regarding predictive text.
   */
  struct ImfEventData
  {
    /**
     * Default Constructor.
     */
    ImfEventData()
    : eventName( VOID ),
      predictiveString(""),
      cursorOffset( 0 ),
      numberOfChars ( 0 )
    {
    };

    /**
     * Constructor
     * @param[in] aEventName The name of the event from the IMF.
     * @param[in] aPredictiveString The pre-edit or commit string.
     * @param[in] aCursorOffset Start position from the current cursor position to start deleting characters.
     * @param[in] aNumberOfChars The number of characters to delete from the cursorOffset.
     */
    ImfEventData(ImfEvent aEventName, const std::string& aPredictiveString, int aCursorOffset,int aNumberOfChars  )
    : eventName(aEventName), predictiveString(aPredictiveString), cursorOffset( aCursorOffset ), numberOfChars( aNumberOfChars )
    {
    }

    // Data
    ImfEvent eventName; // The name of the event from the IMF.
    std::string predictiveString; // The pre-edit or commit string.
    int cursorOffset; // Start position from the current cursor position to start deleting characters.
    int numberOfChars; //number of characters to delete from the cursorOffset.
  };

  /**
   * Data required my IMF from the callback
   */
  struct ImfCallbackData
  {
    ImfCallbackData( )
    : update( false ), cursorPosition( 0 ), preeditResetRequired ( false )
    {
    }

    ImfCallbackData(bool aUpdate, int aCursorPosition, std::string aCurrentText, bool aPreeditResetRequired )
    : update(aUpdate), cursorPosition(aCursorPosition), currentText( aCurrentText ), preeditResetRequired( aPreeditResetRequired )
    {
    }

    bool update; // if cursor position needs to be updated
    int cursorPosition; // new position of cursor
    std::string currentText; // current text string
    bool preeditResetRequired; // flag if preedit reset is required.
  };

  typedef SignalV2< void (ImfManager&) > ImfManagerSignalV2;

  typedef SignalV2< ImfCallbackData ( ImfManager&, const ImfEventData& ) > ImfEventSignalV2;

public:

  /**
   * Retrieve a handle to the instance of ImfManager.
   * @return A handle to the ImfManager.
   */
  static ImfManager Get();

  /**
   * Get the current imf context.
   * @return current imf context.
   */
  ImfContext GetContext();

  /**
   * Activate the IMF.
   * It means that the text editing is started at somewhere.
   * If the H/W keyboard isn't connected then it will show the virtual keyboard.
   */
  void Activate();

  /**
   * Deactivate the IMF.
   * It means that the text editing is finished at somewhere.
   */
  void Deactivate();

  /**
   * Get the restoration status, which controls if the keyboard is restored after the focus lost then regained.
   * If true then keyboard will be restored (activated) after focus is regained.
   * @return restoration status.
   */
  bool RestoreAfterFocusLost() const;

  /**
   * Set status whether the IMF has to restore the keyboard after losing focus.
   * @param[in] toggle True means that keyboard should be restored after focus lost and regained.
   */
  void SetRestoreAferFocusLost( bool toggle );

  /**
   * Send message reset the pred-edit state / imf module.  Used to interupt pre-edit state maybe due to a touch input.
   */
  void Reset();

  /**
   * Notifies IMF context that the cursor position has changed, required for features like auto-capitalisation
   */
  void NotifyCursorPosition();

  /**
   * Sets cursor position stored in VirtualKeyboard, this is required by the IMF context
   * @param[in] cursorPosition position of cursor
   */
  void SetCursorPosition( unsigned int cursorPosition );

  /**
   * Gets cursor position stored in VirtualKeyboard, this is required by the IMF context
   * @return current position of cursor
   */
  int GetCursorPosition();

  /**
   * Method to store the string required by the IMF, this is used to provide predictive word suggestions.
   * @param[in] text The text string surrounding the current cursor point.
   */
  void SetSurroundingText( std::string text );

  /**
   * Gets current text string set within the IMF manager, this is used to offer predictive suggestions
   * @return current position of cursor
   */
  std::string GetSurroundingText();

public:

  // Signals

  /**
   * This is emitted when the virtual keyboard is connected to or the hardware keyboard is activated.
   * @return The IMF Activated signal.
   */
  ImfManagerSignalV2& ActivatedSignal();

  /**
   * This is emitted when the IMF manager receives an event from the IMF
   * @return The Event signal containing the event data.
   */
  ImfEventSignalV2& EventReceivedSignal();

  // Construction & Destruction

  /**
   * Constructor
   */
  ImfManager();

  /**
   * Virtual destructor.
   */
  virtual ~ImfManager();

  /**
   * This constructor is used by ImfManager::Get().
   * @param[in] imfManager A pointer to the imf Manager.
   */
  ImfManager( Internal::Adaptor::ImfManager* imfManager );
};

} // namespace Dali

/**
 * @}
 */
#endif // IMFMANAGER_H
