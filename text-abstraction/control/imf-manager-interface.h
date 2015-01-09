#ifndef __DALI_TEXT_ABSTRACTION_IMF_MANAGER_INTERFACE_H__
#define __DALI_TEXT_ABSTRACTION_IMF_MANAGER_INTERFACE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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


#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>

// EXTERNAL INCLUDES
namespace Dali
{


// TODO: Temporary patch to hidden ecore dependency. Must fix it.
typedef void* ImfContext;
namespace Internal DALI_INTERNAL
{
namespace Adaptor
{
class ImfManager;
}
}

namespace TextAbstraction
{



class ImfManagerInterface
{

    /**
     * @brief Events that are generated by the IMF.
     */
    enum ImfEvent
    {
      VOID,                ///< No event
      PREEDIT,             ///< Pre-Edit changed
      COMMIT,              ///< Commit recieved
      DELETESURROUNDING,   ///< Event to delete a range of characters from the string
      GETSURROUNDING       ///< Event to query string and cursor position
    };

    /**
     * @brief This structure is used to pass on data from the IMF regarding predictive text.
     */
    struct ImfEventData
    {
      /**
       * @brief Default Constructor.
       */
      ImfEventData()
      : eventName( VOID ),
        predictiveString(""),
        cursorOffset( 0 ),
        numberOfChars ( 0 )
      {
      };

      /**
       * @brief Constructor
       *
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
      ImfEvent eventName; ///< The name of the event from the IMF.
      std::string predictiveString; ///< The pre-edit or commit string.
      int cursorOffset; ///< Start position from the current cursor position to start deleting characters.
      int numberOfChars; ///< number of characters to delete from the cursorOffset.
    };

    /**
     * @brief Data required by IMF from the callback
     */
    struct ImfCallbackData
    {
      /**
       * @brief Constructor
       */
      ImfCallbackData( )
      : update( false ), cursorPosition( 0 ), preeditResetRequired ( false )
      {
      }

      /**
       * @brief Constructor
       * @param[in] aUpdate True if cursor position needs to be updated
       * @param[in] aCursorPosition new position of cursor
       * @param[in] aCurrentText current text string
       * @param[in] aPreeditResetRequired flag if preedit reset is required.
       */
      ImfCallbackData(bool aUpdate, int aCursorPosition, std::string aCurrentText, bool aPreeditResetRequired )
      : update(aUpdate), cursorPosition(aCursorPosition), currentText( aCurrentText ), preeditResetRequired( aPreeditResetRequired )
      {
      }

      bool update; ///< if cursor position needs to be updated
      int cursorPosition; ///< new position of cursor
      std::string currentText; ///< current text string
      bool preeditResetRequired; ///< flag if preedit reset is required.
    };

    typedef SignalV2< void (Dali::Internal::Adaptor::ImfManager&) > ImfManagerSignalV2; ///< Keyboard actived signal

    typedef SignalV2< ImfCallbackData ( Dali::Internal::Adaptor::ImfManager&, const ImfEventData& ) > ImfEventSignalV2; ///< keyboard events

  public:

    /**
     * @brief Get the current imf context.
     * @return current imf context.
     */
     virtual ImfContext GetContext() = 0;

    /**
     * @brief Activate the IMF.
     *
     * It means that the text editing is started at somewhere.
     * If the H/W keyboard isn't connected then it will show the virtual keyboard.
     */
     virtual void Activate() = 0;

    /**
     * @brief Deactivate the IMF.
     *
     * It means that the text editing is finished at somewhere.
     */
     virtual void Deactivate() = 0;

    /**
     * @brief Get the restoration status, which controls if the keyboard is restored after the focus lost then regained.
     *
     * If true then keyboard will be restored (activated) after focus is regained.
     * @return restoration status.
     */
     virtual bool RestoreAfterFocusLost() const = 0;

    /**
     * @brief Set status whether the IMF has to restore the keyboard after losing focus.
     *
     * @param[in] toggle True means that keyboard should be restored after focus lost and regained.
     */
     virtual void SetRestoreAfterFocusLost( bool toggle ) = 0;

    /**
     * @brief Send message reset the pred-edit state / imf module.
     *
     * Used to interupt pre-edit state maybe due to a touch input.
     */
     virtual void Reset() = 0;

    /**
     * @brief Notifies IMF context that the cursor position has changed, required for features like auto-capitalisation.
     */
     virtual void NotifyCursorPosition() = 0;

    /**
     * @brief Sets cursor position stored in VirtualKeyboard, this is required by the IMF context.
     *
     * @param[in] cursorPosition position of cursor
     */
     virtual void SetCursorPosition( unsigned int cursorPosition ) = 0;

    /**
     * @brief Gets cursor position stored in VirtualKeyboard, this is required by the IMF context.
     *
     * @return current position of cursor
     */
     virtual int GetCursorPosition() = 0;

    /**
     * @brief Method to store the string required by the IMF, this is used to provide predictive word suggestions.
     *
     * @param[in] text The text string surrounding the current cursor point.
     */
     virtual void SetSurroundingText( std::string text ) = 0;

    /**
     * @brief Gets current text string set within the IMF manager, this is used to offer predictive suggestions.
     *
     * @return current position of cursor
     */
     virtual std::string GetSurroundingText() = 0;

  public:

    // Signals

    /**
     * @brief This is emitted when the virtual keyboard is connected to or the hardware keyboard is activated.
     *
     * @return The IMF Activated signal.
     */
     virtual ImfManagerSignalV2& ActivatedSignal() = 0;

    /**
     * @brief This is emitted when the IMF manager receives an event from the IMF.
     *
     * @return The Event signal containing the event data.
     */
     virtual ImfEventSignalV2& EventReceivedSignal() = 0;


protected:

     ImfManagerInterface() {}

     virtual ~ImfManagerInterface() {}

private:

  // Undefined copy constructor.
    ImfManagerInterface( const ImfManagerInterface& );

  // Undefined assignment operator.
    ImfManagerInterface& operator=( const ImfManagerInterface& );
};

} // namespace Platform

} // namespace Dali

#endif // __DALI_TEXT_ABSTRACTION_IMF_MANAGER_INTERFACE_H__
