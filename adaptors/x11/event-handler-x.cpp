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
#include <events/event-handler.h>

// EXTERNAL INCLUDES
#include <Ecore.h>
#include <Ecore_Input.h>
#include <Ecore_X.h>
#include <cstring>

#include <sys/time.h>

#include <vconf.h>
#include <vconf-keys.h>

#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/touch-point.h>
#include <dali/public-api/events/key-event.h>
#include <dali/public-api/events/mouse-wheel-event.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/mouse-wheel-event-integ.h>

// INTERNAL INCLUDES
#include <events/gesture-manager.h>
#include <window-render-surface.h>
#include <clipboard-impl.h>
#include <key-impl.h>
#include <physical-keyboard-impl.h>
#include <style-monitor-impl.h>
#include <base/core-event-interface.h>

using namespace std;

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

#if defined(DEBUG_ENABLED)
namespace
{
Integration::Log::Filter* gTouchEventLogFilter  = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_ADAPTOR_EVENTS_TOUCH");
Integration::Log::Filter* gClientMessageLogFilter  = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_ADAPTOR_EVENTS_CLIENT_MESSAGE");
Integration::Log::Filter* gDragAndDropLogFilter = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_ADAPTOR_EVENTS_DND");
Integration::Log::Filter* gImfLogging  = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_ADAPTOR_EVENTS_IMF");
Integration::Log::Filter* gSelectionEventLogFilter = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_ADAPTOR_EVENTS_SELECTION");
} // unnamed namespace
#endif


namespace
{
// Currently this code is internal to dali/dali/internal/event/text/utf8.h but should be made Public and used from there instead.
size_t Utf8SequenceLength(const unsigned char leadByte)
{
  size_t length = 0;

  if ((leadByte & 0x80) == 0 )          //ASCII character (lead bit zero)
  {
    length = 1;
  }
  else if (( leadByte & 0xe0 ) == 0xc0 ) //110x xxxx
  {
    length = 2;
  }
  else if (( leadByte & 0xf0 ) == 0xe0 ) //1110 xxxx
  {
    length = 3;
  }
  else if (( leadByte & 0xf8 ) == 0xf0 ) //1111 0xxx
  {
    length = 4;
  }
  else
  {
    DALI_LOG_WARNING("Unrecognized lead byte  %c\n", leadByte);
  }

  return length;
}

const unsigned int PRIMARY_TOUCH_BUTTON_ID( 1 );

const char * CLIPBOARD_ATOM                = "CBHM_MSG";
const char * CLIPBOARD_SET_OWNER_MESSAGE   = "SET_OWNER";

/// The atoms required by Ecore for Drag & Drop behaviour.
Ecore_X_Atom DRAG_AND_DROP_ATOMS[] =
{
  ECORE_X_ATOM_XDND_ACTION_COPY,
};

/// The types that we support.
const char * DRAG_AND_DROP_TYPES[] =
{
  ECORE_X_SELECTION_TARGET_UTF8_STRING,
};

const unsigned int DRAG_AND_DROP_ATOMS_NUMBER = sizeof( DRAG_AND_DROP_ATOMS ) / sizeof( Ecore_X_Atom );
const unsigned int DRAG_AND_DROP_TYPES_NUMBER = sizeof( DRAG_AND_DROP_TYPES ) / sizeof( const char * );

const unsigned int BYTES_PER_CHARACTER_FOR_ATTRIBUTES = 3;

/**
 * Ecore_Event_Modifier enums in Ecore_Input.h do not match Ecore_IMF_Keyboard_Modifiers in Ecore_IMF.h.
 * This function converts from Ecore_Event_Modifier to Ecore_IMF_Keyboard_Modifiers enums.
 * @param[in] ecoreModifier the Ecore_Event_Modifier input.
 * @return the Ecore_IMF_Keyboard_Modifiers output.
 */
Ecore_IMF_Keyboard_Modifiers EcoreInputModifierToEcoreIMFModifier(unsigned int ecoreModifier)
{
   int modifier( ECORE_IMF_KEYBOARD_MODIFIER_NONE );  // If no other matches returns NONE.


   if ( ecoreModifier & ECORE_EVENT_MODIFIER_SHIFT )  // enums from ecore_input/Ecore_Input.h
   {
     modifier |= ECORE_IMF_KEYBOARD_MODIFIER_SHIFT;  // enums from ecore_imf/ecore_imf.h
   }

   if ( ecoreModifier & ECORE_EVENT_MODIFIER_ALT )
   {
     modifier |= ECORE_IMF_KEYBOARD_MODIFIER_ALT;
   }

   if ( ecoreModifier & ECORE_EVENT_MODIFIER_CTRL )
   {
     modifier |= ECORE_IMF_KEYBOARD_MODIFIER_CTRL;
   }

   if ( ecoreModifier & ECORE_EVENT_MODIFIER_WIN )
   {
     modifier |= ECORE_IMF_KEYBOARD_MODIFIER_WIN;
   }

   if ( ecoreModifier & ECORE_EVENT_MODIFIER_ALTGR )
   {
     modifier |= ECORE_IMF_KEYBOARD_MODIFIER_ALTGR;
   }

   return static_cast<Ecore_IMF_Keyboard_Modifiers>( modifier );
}


// Copied from x server
static unsigned int GetCurrentMilliSeconds(void)
{
  struct timeval tv;

  struct timespec tp;
  static clockid_t clockid;

  if (!clockid)
  {
#ifdef CLOCK_MONOTONIC_COARSE
    if (clock_getres(CLOCK_MONOTONIC_COARSE, &tp) == 0 &&
      (tp.tv_nsec / 1000) <= 1000 && clock_gettime(CLOCK_MONOTONIC_COARSE, &tp) == 0)
    {
      clockid = CLOCK_MONOTONIC_COARSE;
    }
    else
#endif
    if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0)
    {
      clockid = CLOCK_MONOTONIC;
    }
    else
    {
      clockid = ~0L;
    }
  }
  if (clockid != ~0L && clock_gettime(clockid, &tp) == 0)
  {
    return (tp.tv_sec * 1000) + (tp.tv_nsec / 1000000L);
  }

  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

const char * DALI_VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE = "db/setting/accessibility/font_name";  // It will be update at vconf-key.h and replaced.

} // unnamed namespace

// Impl to hide EFL implementation.
struct EventHandler::Impl
{
  // Construction & Destruction

  /**
   * Constructor
   */
  Impl( EventHandler* handler, Ecore_X_Window window )
  : mHandler( handler ),
    mEcoreEventHandler(),
    mWindow( window )
  {
    // Only register for touch and key events if we have a window
    if ( window != 0 )
    {
      // Register Touch events
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_EVENT_MOUSE_BUTTON_DOWN,  EcoreEventMouseButtonDown, handler ) );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_EVENT_MOUSE_BUTTON_UP,    EcoreEventMouseButtonUp,   handler ) );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_EVENT_MOUSE_MOVE,         EcoreEventMouseButtonMove, handler ) );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_EVENT_MOUSE_OUT,          EcoreEventMouseButtonUp,   handler ) ); // process mouse out event like up event

      // Register Mouse wheel events
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_EVENT_MOUSE_WHEEL,        EcoreEventMouseWheel,      handler ) );

      // Register Key events
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_EVENT_KEY_DOWN,           EcoreEventKeyDown,         handler ) );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_EVENT_KEY_UP,             EcoreEventKeyUp,           handler ) );

      // Register Focus events
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_WINDOW_FOCUS_IN,  EcoreEventWindowFocusIn,   handler ) );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_WINDOW_FOCUS_OUT, EcoreEventWindowFocusOut,  handler ) );

      // Register Window damage events
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_WINDOW_DAMAGE,    EcoreEventWindowDamaged, handler ) );

      // Enable Drag & Drop and register DnD events
      ecore_x_dnd_aware_set( window, EINA_TRUE );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_XDND_ENTER,       EcoreEventDndEnter,            handler) );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_XDND_POSITION,    EcoreEventDndPosition,         handler) );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_XDND_LEAVE,       EcoreEventDndLeave,            handler) );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_XDND_DROP,        EcoreEventDndDrop,             handler) );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_XDND_FINISHED,    EcoreEventDndFinished,         handler) );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_XDND_STATUS,      EcoreEventDndStatus,           handler) );

      // Register Client message events - accessibility etc.
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_CLIENT_MESSAGE,  EcoreEventClientMessage, handler ) );

      // Register Selection event - clipboard selection, Drag & Drop selection etc.
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_SELECTION_CLEAR, EcoreEventSelectionClear, handler ) );
      mEcoreEventHandler.push_back( ecore_event_handler_add( ECORE_X_EVENT_SELECTION_NOTIFY, EcoreEventSelectionNotify, handler ) );

      // Register Vconf notify - font name and size
      vconf_notify_key_changed( DALI_VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE, VconfNotifyFontNameChanged, handler );
      vconf_notify_key_changed( VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE, VconfNotifyFontSizeChanged, handler );
    }
  }

  /**
   * Destructor
   */
  ~Impl()
  {
    vconf_ignore_key_changed( VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE, VconfNotifyFontSizeChanged );
    vconf_ignore_key_changed( DALI_VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE, VconfNotifyFontNameChanged );

    for( std::vector<Ecore_Event_Handler*>::iterator iter = mEcoreEventHandler.begin(), endIter = mEcoreEventHandler.end(); iter != endIter; ++iter )
    {
      ecore_event_handler_del( *iter );
    }
  }

  // Static methods

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Touch Callbacks
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Called when a touch down is received.
   */
  static Eina_Bool EcoreEventMouseButtonDown( void* data, int type, void* event )
  {
    Ecore_Event_Mouse_Button *touchEvent( (Ecore_Event_Mouse_Button*)event );
    EventHandler* handler( (EventHandler*)data );

    if ( touchEvent->window == handler->mImpl->mWindow )
    {
      TouchPoint::State state ( TouchPoint::Down );

      // Check if the buttons field is set and ensure it's the primary touch button.
      // If this event was triggered by buttons other than the primary button (used for touch), then
      // just send an interrupted event to Core.
      if ( touchEvent->buttons && (touchEvent->buttons != PRIMARY_TOUCH_BUTTON_ID ) )
      {
        state = TouchPoint::Interrupted;
      }

      TouchPoint point( touchEvent->multi.device, state, touchEvent->x, touchEvent->y );
      handler->SendEvent( point, touchEvent->timestamp );
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when a touch up is received.
   */
  static Eina_Bool EcoreEventMouseButtonUp( void* data, int type, void* event )
  {
    Ecore_Event_Mouse_Button *touchEvent( (Ecore_Event_Mouse_Button*)event );
    EventHandler* handler( (EventHandler*)data );

    if ( touchEvent->window == handler->mImpl->mWindow )
    {
      TouchPoint point( touchEvent->multi.device, TouchPoint::Up, touchEvent->x, touchEvent->y );
      handler->SendEvent( point, touchEvent->timestamp );
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when a touch up is received.
   */
  static Eina_Bool EcoreEventMouseWheel( void* data, int type, void* event )
  {
    Ecore_Event_Mouse_Wheel *mouseWheelEvent( (Ecore_Event_Mouse_Wheel*)event );

    DALI_LOG_INFO( gImfLogging, Debug::General, "EVENT Ecore_Event_Mouse_Wheel: direction: %d, modifiers: %d, x: %d, y: %d, z: %d\n", mouseWheelEvent->direction, mouseWheelEvent->modifiers, mouseWheelEvent->x, mouseWheelEvent->y, mouseWheelEvent->z);

    EventHandler* handler( (EventHandler*)data );
    if ( mouseWheelEvent->window == handler->mImpl->mWindow )
    {
      MouseWheelEvent wheelEvent(mouseWheelEvent->direction, mouseWheelEvent->modifiers, Vector2(mouseWheelEvent->x, mouseWheelEvent->y), mouseWheelEvent->z, mouseWheelEvent->timestamp);
      handler->SendMouseWheelEvent( wheelEvent );
    }
    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when a touch motion is received.
   */
  static Eina_Bool EcoreEventMouseButtonMove( void* data, int type, void* event )
  {
    Ecore_Event_Mouse_Move *touchEvent( (Ecore_Event_Mouse_Move*)event );
    EventHandler* handler( (EventHandler*)data );

    if ( touchEvent->window == handler->mImpl->mWindow )
    {
      TouchPoint point( touchEvent->multi.device, TouchPoint::Motion, touchEvent->x, touchEvent->y );
      handler->SendEvent( point, touchEvent->timestamp );
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Key Callbacks
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Called when a key down is received.
   */
  static Eina_Bool EcoreEventKeyDown( void* data, int type, void* event )
  {
    DALI_LOG_INFO( gImfLogging, Debug::General, "EVENT >>EcoreEventKeyDown \n" );

    EventHandler* handler( (EventHandler*)data );
    Ecore_Event_Key *keyEvent( (Ecore_Event_Key*)event );
    bool eventHandled( false );

    Ecore_IMF_Context* imfContext = NULL;
    if ( Dali::Adaptor::IsAvailable() && handler->mImfManager )
    {
      imfContext = reinterpret_cast<Ecore_IMF_Context*>( handler->mImfManager.GetContext() );
    }

    // If a device key then skip ecore_imf_context_filter_event.
    if ( imfContext && !( KeyLookup::IsDeviceButton( keyEvent->keyname ) ) )
    {
      // We're consuming key down event so we have to pass to IMF so that it can parse it as well.
      Ecore_IMF_Event_Key_Down ecoreKeyDownEvent;
      ecoreKeyDownEvent.keyname   = keyEvent->keyname;
      ecoreKeyDownEvent.key       = keyEvent->key;
      ecoreKeyDownEvent.string    = keyEvent->string;
      ecoreKeyDownEvent.compose   = keyEvent->compose;
      ecoreKeyDownEvent.timestamp = keyEvent->timestamp;
      ecoreKeyDownEvent.modifiers = EcoreInputModifierToEcoreIMFModifier ( keyEvent->modifiers );
      ecoreKeyDownEvent.locks     = (Ecore_IMF_Keyboard_Locks) ECORE_IMF_KEYBOARD_LOCK_NONE;

      eventHandled = ecore_imf_context_filter_event( imfContext,
                                                     ECORE_IMF_EVENT_KEY_DOWN,
                                                     (Ecore_IMF_Event *) &ecoreKeyDownEvent );

      // If the event has not been handled by IMF then check if we should reset our IMF context
      if( !eventHandled )
      {
        if ( !strcmp( keyEvent->keyname, "Escape"   ) ||
             !strcmp( keyEvent->keyname, "Return"   ) ||
             !strcmp( keyEvent->keyname, "KP_Enter" ) )
        {
          ecore_imf_context_reset( imfContext );
        }
      }
    }

    // If the event wasn't handled then we should send a key event.
    if ( !eventHandled )
    {
      if ( keyEvent->window == handler->mImpl->mWindow )
      {
        std::string keyName( keyEvent->keyname );
        std::string keyString( "" );
        int keyCode = ecore_x_keysym_keycode_get(keyEvent->keyname);
        int modifier( keyEvent->modifiers );
        unsigned long time = keyEvent->timestamp;

        // Ensure key event string is not NULL as keys like SHIFT have a null string.
        if ( keyEvent->string )
        {
          keyString = keyEvent->string;
        }

        KeyEvent keyEvent(keyName, keyString, keyCode, modifier, time, KeyEvent::Down);
        handler->SendEvent( keyEvent );
      }
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when a key up is received.
   */
  static Eina_Bool EcoreEventKeyUp( void* data, int type, void* event )
  {
    DALI_LOG_INFO( gImfLogging, Debug::General, "EVENT >>EcoreEventKeyUp \n" );

    EventHandler* handler( (EventHandler*)data );
    Ecore_Event_Key *keyEvent( (Ecore_Event_Key*)event );
    bool eventHandled( false );

    Ecore_IMF_Context* imfContext = NULL;
    if ( Dali::Adaptor::IsAvailable() && handler->mImfManager )
    {
      imfContext = reinterpret_cast<Ecore_IMF_Context*>( handler->mImfManager.GetContext() );
    }

    // XF86Stop and XF86Send must skip ecore_imf_context_filter_event.
    if ( imfContext && strcmp( keyEvent->keyname, "XF86Send" ) && strcmp( keyEvent->keyname, "XF86Phone" ) && strcmp( keyEvent->keyname, "XF86Stop" ) )

    {
      // We're consuming key up event so we have to pass to IMF so that it can parse it as well.
      Ecore_IMF_Event_Key_Up ecoreKeyUpEvent;
      ecoreKeyUpEvent.keyname   = keyEvent->keyname;
      ecoreKeyUpEvent.key       = keyEvent->key;
      ecoreKeyUpEvent.string    = keyEvent->string;
      ecoreKeyUpEvent.compose   = keyEvent->compose;
      ecoreKeyUpEvent.timestamp = keyEvent->timestamp;
      ecoreKeyUpEvent.modifiers = EcoreInputModifierToEcoreIMFModifier ( keyEvent->modifiers );
      ecoreKeyUpEvent.locks     = (Ecore_IMF_Keyboard_Locks) ECORE_IMF_KEYBOARD_LOCK_NONE;

      eventHandled = ecore_imf_context_filter_event( imfContext,
                                                     ECORE_IMF_EVENT_KEY_UP,
                                                     (Ecore_IMF_Event *) &ecoreKeyUpEvent );
    }

    // If the event wasn't handled then we should send a key event.
    if ( !eventHandled )
    {
      if ( keyEvent->window == handler->mImpl->mWindow )
      {
        std::string keyName( keyEvent->keyname );
        std::string keyString( "" );
        int keyCode = ecore_x_keysym_keycode_get(keyEvent->keyname);
        int modifier( keyEvent->modifiers );
        unsigned long time( keyEvent->timestamp );

        // Ensure key event string is not NULL as keys like SHIFT have a null string.
        if ( keyEvent->string )
        {
          keyString = keyEvent->string;
        }

        KeyEvent keyEvent(keyName, keyString, keyCode, modifier, time, KeyEvent::Up);
        handler->SendEvent( keyEvent );

      }
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Window Callbacks
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Called when the window gains focus.
   */
  static Eina_Bool EcoreEventWindowFocusIn( void* data, int type, void* event )
  {
    Ecore_X_Event_Window_Focus_In* focusInEvent( (Ecore_X_Event_Window_Focus_In*)event );
    EventHandler* handler( (EventHandler*)data );

    DALI_LOG_INFO( gImfLogging, Debug::General, "EVENT >>EcoreEventWindowFocusIn \n" );

    // If the window gains focus and we hid the keyboard then show it again.
    if ( focusInEvent->win == handler->mImpl->mWindow )
    {
      DALI_LOG_INFO( gImfLogging, Debug::General, "EVENT EcoreEventWindowFocusIn - >>WindowFocusGained \n" );

      if ( handler->mImfManager )
      {
        ImfManager& imfManager( ImfManager::GetImplementation( handler->mImfManager ) );
        if( imfManager.RestoreAfterFocusLost() )
        {
          imfManager.Activate();
        }
      }
      // No need to connect callbacks as KeyboardStatusChanged will be called.
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when the window loses focus.
   */
  static Eina_Bool EcoreEventWindowFocusOut( void* data, int type, void* event )
  {
    Ecore_X_Event_Window_Focus_Out* focusOutEvent( (Ecore_X_Event_Window_Focus_Out*)event );
    EventHandler* handler( (EventHandler*)data );

    DALI_LOG_INFO( gImfLogging, Debug::General, "EVENT >>EcoreEventWindowFocusOut \n" );

    // If the window loses focus then hide the keyboard.
    if ( focusOutEvent->win == handler->mImpl->mWindow )
    {
      if ( handler->mImfManager )
      {
        ImfManager& imfManager( ImfManager::GetImplementation( handler->mImfManager ) );
        if( imfManager.RestoreAfterFocusLost() )
        {
          imfManager.Deactivate();
        }
      }

      // Clipboard don't support that whether clipboard is shown or not. Hide clipboard.
      Dali::Clipboard clipboard = Clipboard::Get();
      clipboard.HideClipboard();
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when the window is damaged.
   */
  static Eina_Bool EcoreEventWindowDamaged(void *data, int type, void *event)
  {
    Ecore_X_Event_Window_Damage* windowDamagedEvent( (Ecore_X_Event_Window_Damage*)event );
    EventHandler* handler( (EventHandler*)data );

    if( windowDamagedEvent->win == handler->mImpl->mWindow )
    {
      DamageArea area;
      area.x = windowDamagedEvent->x;
      area.y = windowDamagedEvent->y;
      area.width = windowDamagedEvent->w;
      area.height = windowDamagedEvent->h;

      handler->SendEvent( area );
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when the window properties are changed.
   * We are only interested in the font change.
   */


  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Drag & Drop Callbacks
  /////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Called when a dragged item enters our window's bounds.
   * This is when items are dragged INTO our window.
   */
  static Eina_Bool EcoreEventDndEnter( void* data, int type, void* event )
  {
    DALI_LOG_INFO( gDragAndDropLogFilter, Debug::Concise, "EcoreEventDndEnter\n" );

    Ecore_X_Event_Xdnd_Enter* enterEvent( (Ecore_X_Event_Xdnd_Enter*) event );
    EventHandler* handler( (EventHandler*)data );
    Ecore_X_Window window ( handler->mImpl->mWindow );

    if ( enterEvent->win == window )
    {
      DragAndDropDetectorPtr dndDetector( handler->mDragAndDropDetector );

      // Check whether the Drag & Drop detector has Drag & Drop behaviour enabled before we accept.
      if ( dndDetector && dndDetector->IsEnabled() )
      {
        // Tell Ecore that we want to enable drop in the entire window.
        Ecore_X_Rectangle rect;
        rect.x = rect.y = 0;
        ecore_x_window_geometry_get( window, NULL, NULL, (int*)&rect.width, (int*)&rect.height );

        // Tell Ecore that we are able to process a drop.
        ecore_x_dnd_send_status( EINA_TRUE, EINA_FALSE, rect, ECORE_X_ATOM_XDND_DROP );

        // Register the required atoms and types.
        ecore_x_dnd_actions_set( window, DRAG_AND_DROP_ATOMS, DRAG_AND_DROP_ATOMS_NUMBER );
        ecore_x_dnd_types_set(   window, DRAG_AND_DROP_TYPES, DRAG_AND_DROP_TYPES_NUMBER );

        // Request to get the content from Ecore.
        ecore_x_selection_xdnd_request( window, ECORE_X_SELECTION_TARGET_UTF8_STRING );

        DALI_LOG_INFO( gDragAndDropLogFilter, Debug::General, "EcoreEventDndEnter: Requesting Drag & Drop\n" );

        // Clear the previous content
        dndDetector->ClearContent();

        // Emit the entered signal
        dndDetector->EmitEnteredSignal();
      }
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when a dragged item is moved within our window.
   * This is when items are dragged INTO our window.
   */
  static Eina_Bool EcoreEventDndPosition( void* data, int type, void* event )
  {
    DALI_LOG_INFO(gDragAndDropLogFilter, Debug::Concise, "EcoreEventDndPosition\n" );

    Ecore_X_Event_Xdnd_Position* positionEvent( (Ecore_X_Event_Xdnd_Position*) event );
    EventHandler* handler( (EventHandler*)data );

    if ( positionEvent->win == handler->mImpl->mWindow )
    {
      DragAndDropDetectorPtr dndDetector( handler->mDragAndDropDetector );

      // If we have a detector then update its latest position.
      if ( dndDetector )
      {
        DALI_LOG_INFO(gDragAndDropLogFilter, Debug::General, "EcoreEventDndPosition: position ( %d x %d )\n", positionEvent->position.x, positionEvent->position.y );
        dndDetector->SetPosition( Vector2( positionEvent->position.x, positionEvent->position.y ));
        dndDetector->EmitMovedSignal();
      }
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when a dragged item leaves our window's bounds.
   * This is when items are dragged INTO our window.
   */
  static Eina_Bool EcoreEventDndLeave( void* data, int type, void* event )
  {
    DALI_LOG_INFO(gDragAndDropLogFilter, Debug::Concise, "EcoreEventDndLeave\n" );

    Ecore_X_Event_Xdnd_Leave* leaveEvent( (Ecore_X_Event_Xdnd_Leave*) event );
    EventHandler* handler( (EventHandler*)data );

    if ( leaveEvent->win == handler->mImpl->mWindow )
    {
      DragAndDropDetectorPtr dndDetector( handler->mDragAndDropDetector );

      // If we have a detector then clear its content and emit the exited-signal. Also tell Ecore that we have finished.
      if ( dndDetector )
      {
        dndDetector->ClearContent();
        dndDetector->EmitExitedSignal();

        ecore_x_dnd_send_finished();

        DALI_LOG_INFO( gDragAndDropLogFilter, Debug::General, "EcoreEventDndLeave: Finished\n" );
      }
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when the dragged item is dropped within our window's bounds.
   * This is when items are dragged INTO our window.
   */
  static Eina_Bool EcoreEventDndDrop( void* data, int type, void* event )
  {
    DALI_LOG_INFO(gDragAndDropLogFilter, Debug::Concise, "EcoreEventDndDrop\n" );

    Ecore_X_Event_Xdnd_Drop* dropEvent ( (Ecore_X_Event_Xdnd_Drop*) event);
    EventHandler* handler( (EventHandler*)data );

    if ( dropEvent->win == handler->mImpl->mWindow )
    {
      DragAndDropDetectorPtr dndDetector( handler->mDragAndDropDetector );

      // Something has been dropped, inform the detector (if we have one) and tell Ecore that we have finished.
      if ( dndDetector )
      {
        DALI_LOG_INFO(gDragAndDropLogFilter, Debug::General, "EcoreEventDndDrop: position ( %d x %d )\n", dropEvent->position.x, dropEvent->position.y );

        dndDetector->SetPosition( Vector2( dropEvent->position.x, dropEvent->position.y ) );
        dndDetector->EmitDroppedSignal();
        ecore_x_dnd_send_finished();

        DALI_LOG_INFO( gDragAndDropLogFilter, Debug::General, "EcoreEventDndDrop: Finished\n" );
      }
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when a dragged item is moved from our window and the target window has done processing it.
   * This is when items are dragged FROM our window.
   */
  static Eina_Bool EcoreEventDndFinished( void* data, int type, void* event )
  {
    DALI_LOG_INFO(gDragAndDropLogFilter, Debug::Concise, "EcoreEventDndFinished\n" );
    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when a dragged item is moved from our window and the target window has sent us a status.
   * This is when items are dragged FROM our window.
   */
  static Eina_Bool EcoreEventDndStatus( void* data, int type, void* event )
  {
    DALI_LOG_INFO(gDragAndDropLogFilter, Debug::Concise, "EcoreEventDndStatus\n" );
    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when the client messages (i.e. the accessibility events) are received.
   */
  static Eina_Bool EcoreEventClientMessage( void* data, int type, void* event )
  {
    Ecore_X_Event_Client_Message* clientMessageEvent( (Ecore_X_Event_Client_Message*)event );
    EventHandler* handler( (EventHandler*)data );

    if (clientMessageEvent->message_type == ECORE_X_ATOM_E_ILLUME_ACCESS_CONTROL)
    {
      if ( ( (unsigned int)clientMessageEvent->data.l[0] == handler->mImpl->mWindow ) && handler->mAccessibilityManager )
      {
        AccessibilityManager* accessibilityManager( &AccessibilityManager::GetImplementation( handler->mAccessibilityManager ) );

        if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_SCROLL)
        {
          // 2 finger touch & move, 2 finger flick

          // mouse state : e->data.l[2] (0: mouse down, 1: mouse move, 2: mouse up)
          // x : e->data.l[3]
          // y : e->data.l[4]
          TouchPoint::State state(TouchPoint::Down);

          if ((unsigned int)clientMessageEvent->data.l[2] == 0)
          {
            state = TouchPoint::Down; // mouse down
          }
          else if ((unsigned int)clientMessageEvent->data.l[2] == 1)
          {
            state = TouchPoint::Motion; // mouse move
          }
          else if ((unsigned int)clientMessageEvent->data.l[2] == 2)
          {
            state = TouchPoint::Up; // mouse up
          }
          else
          {
            state = TouchPoint::Interrupted; // error
          }

          DALI_LOG_INFO(gClientMessageLogFilter, Debug::General,
            "[%s:%d] [%d] %d, %d\n", __FUNCTION__, __LINE__,
            (unsigned int)clientMessageEvent->data.l[2],
            (unsigned int)clientMessageEvent->data.l[3], (unsigned int)clientMessageEvent->data.l[4]);

          // Send touch event to accessibility manager.
          TouchPoint point( 0, state, (float)clientMessageEvent->data.l[3], (float)clientMessageEvent->data.l[4] );

          // In accessibility mode, scroll action should be handled when the currently focused actor is contained in scrollable control
          accessibilityManager->HandleActionScrollEvent( point, GetCurrentMilliSeconds() );
        }
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_MOUSE)
        {
          // 1 finger double tap and hold

          // mouse state : e->data.l[2] (0: mouse down, 1: mouse move, 2: mouse up)
          // x : e->data.l[3]
          // y : e->data.l[4]
          TouchPoint::State state(TouchPoint::Down);

          if ((unsigned int)clientMessageEvent->data.l[2] == 0)
          {
            state = TouchPoint::Down; // mouse down
          }
          else if ((unsigned int)clientMessageEvent->data.l[2] == 1)
          {
            state = TouchPoint::Motion; // mouse move
          }
          else if ((unsigned int)clientMessageEvent->data.l[2] == 2)
          {
            state = TouchPoint::Up; // mouse up
          }
          else
          {
            state = TouchPoint::Interrupted; // error
          }

          DALI_LOG_INFO(gClientMessageLogFilter, Debug::General,
            "[%s:%d] [%d] %d, %d\n", __FUNCTION__, __LINE__,
            (unsigned int)clientMessageEvent->data.l[2],
            (unsigned int)clientMessageEvent->data.l[3], (unsigned int)clientMessageEvent->data.l[4]);

          // Send touch event to accessibility manager.
          TouchPoint point( 0, state, (float)clientMessageEvent->data.l[3], (float)clientMessageEvent->data.l[4] );

          // In accessibility mode, scroll action should be handled when the currently focused actor is contained in scrollable control
          accessibilityManager->HandleActionTouchEvent( point, GetCurrentMilliSeconds() );
        }
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_BACK)
        {
          // 2 finger circle draw, do back
          accessibilityManager->HandleActionBackEvent();
        }
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_NEXT)
        {
          // one finger flick down
          // focus next object
          if(accessibilityManager)
          {
            accessibilityManager->HandleActionNextEvent();
          }
        }
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_PREV)
        {
          // one finger flick up
          // focus previous object
          if(accessibilityManager)
          {
            accessibilityManager->HandleActionPreviousEvent();
          }
        }
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_ACTIVATE)
        {
          // one finger double tap
          // same as one finger tap in normal mode (i.e. execute focused actor)
          if(accessibilityManager)
          {
            accessibilityManager->HandleActionActivateEvent();
          }
        }
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_READ)
        {
          // one finger tap
          // focus & read an actor at ( e->data.l[2], e->data.l[3] ) position according to finger
          if(accessibilityManager)
          {
            accessibilityManager->HandleActionReadEvent((unsigned int)clientMessageEvent->data.l[2], (unsigned int)clientMessageEvent->data.l[3], true /* allow read again*/);
          }
        }
#if defined(DALI_PROFILE_MOBILE) || defined(DALI_PROFILE_TV)
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_OVER)
        {
          // one finger tap & move
          // mouse state : e->data.l[2] (0: mouse down, 1: mouse move, 2: mouse up)
          // x : e->data.l[3]
          // y : e->data.l[4]
          // focus & read an actor at (x, y) position according to finger
          if(accessibilityManager && (unsigned int)clientMessageEvent->data.l[2] == 1 /*only work for move event*/)
          {
            accessibilityManager->HandleActionReadEvent((unsigned int)clientMessageEvent->data.l[3], (unsigned int)clientMessageEvent->data.l[4], false /* not allow read again*/);
          }
        }
#endif
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_READ_NEXT)
        {
          // one finger flick right
          // focus next object
           if(accessibilityManager)
          {
            accessibilityManager->HandleActionReadNextEvent();
          }
        }
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_READ_PREV)
        {
          // one finger flick left
          // focus previous object
          if(accessibilityManager)
          {
            accessibilityManager->HandleActionReadPreviousEvent();
          }
        }
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_UP)
        {
          // double down and move (right, up)
          // change slider value
          if(accessibilityManager)
          {
            accessibilityManager->HandleActionUpEvent();
          }
        }
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_DOWN)
        {
          // double down and move (left, down)
          // change slider value
          if(accessibilityManager)
          {
            accessibilityManager->HandleActionDownEvent();
          }
        }
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_ENABLE)
        {
           if(accessibilityManager)
          {
            accessibilityManager->HandleActionEnableEvent();
          }
        }
        else if((unsigned int)clientMessageEvent->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_DISABLE)
        {
          if(accessibilityManager)
          {
            accessibilityManager->HandleActionDisableEvent();
          }
        }
        // TODO: some more actions could be added later
      }
    }
    else if(clientMessageEvent->message_type == ecore_x_atom_get(CLIPBOARD_ATOM))
    {
      std::string message(clientMessageEvent->data.b);
      if( message == CLIPBOARD_SET_OWNER_MESSAGE)
      {
        // Claim the ownership of the SECONDARY selection.
        ecore_x_selection_secondary_set(handler->mImpl->mWindow, "", 1);

        // Show the clipboard window
        Dali::Clipboard clipboard = Dali::Clipboard::Get();
        clipboard.ShowClipboard();
      }
    }
    else if( clientMessageEvent->message_type == ECORE_X_ATOM_E_WINDOW_ROTATION_CHANGE_PREPARE )
    {
      RotationEvent rotationEvent;
      rotationEvent.angle      = static_cast<int>(clientMessageEvent->data.l[1]);
      rotationEvent.winResize  = static_cast<int>(clientMessageEvent->data.l[2]);
      rotationEvent.width      = static_cast<int>(clientMessageEvent->data.l[3]);
      rotationEvent.height     = static_cast<int>(clientMessageEvent->data.l[4]);
      handler->SendRotationPrepareEvent( rotationEvent );
    }
    else if( clientMessageEvent->message_type == ECORE_X_ATOM_E_WINDOW_ROTATION_CHANGE_REQUEST )
    {
      handler->SendRotationRequestEvent();
    }

    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when the source window notifies us the content in clipboard is selected.
   */
  static Eina_Bool EcoreEventSelectionClear( void* data, int type, void* event )
  {
    DALI_LOG_INFO(gSelectionEventLogFilter, Debug::Concise, "EcoreEventSelectionClear\n" );
    Ecore_X_Event_Selection_Clear* selectionClearEvent( (Ecore_X_Event_Selection_Clear*) event );
    EventHandler* handler( (EventHandler*)data );

    if ( selectionClearEvent->win == handler->mImpl->mWindow )
    {
      if ( selectionClearEvent->selection == ECORE_X_SELECTION_SECONDARY )
      {
        // Request to get the content from Ecore.
        ecore_x_selection_secondary_request(selectionClearEvent->win, ECORE_X_SELECTION_TARGET_TEXT);
      }
    }
    return ECORE_CALLBACK_PASS_ON;
  }

  /**
   * Called when the source window sends us about the selected content.
   * For example, when dragged items are dragged INTO our window or when items are selected in the clipboard.
   */
  static Eina_Bool EcoreEventSelectionNotify( void* data, int type, void* event )
  {
    DALI_LOG_INFO(gSelectionEventLogFilter, Debug::Concise, "EcoreEventSelectionNotify\n" );

    Ecore_X_Event_Selection_Notify* selectionNotifyEvent( (Ecore_X_Event_Selection_Notify*) event );
    EventHandler* handler( (EventHandler*)data );

    if ( selectionNotifyEvent->win == handler->mImpl->mWindow )
    {
      Ecore_X_Selection_Data* selectionData( (Ecore_X_Selection_Data*) selectionNotifyEvent->data );
      if ( selectionData->data )
      {
        if ( selectionNotifyEvent->selection == ECORE_X_SELECTION_XDND )
        {
          DragAndDropDetectorPtr dndDetector( handler->mDragAndDropDetector );

          // We have got the content that is to be dropped, inform the DndListener (if we have one).
          if ( dndDetector )
          {
            std::string content( (char*) selectionData->data, selectionData->length );
            dndDetector->SetContent( content );

            DALI_LOG_INFO( gSelectionEventLogFilter, Debug::General, "EcoreEventSelectionNotify: Content(%d):\n" , selectionData->length );
            DALI_LOG_INFO( gSelectionEventLogFilter, Debug::General, "======================================\n" );
            DALI_LOG_INFO( gSelectionEventLogFilter, Debug::General, "%s\n", selectionData->data );
            DALI_LOG_INFO( gSelectionEventLogFilter, Debug::General, "======================================\n" );
          }
        }
        else if ( selectionNotifyEvent->selection == ECORE_X_SELECTION_SECONDARY )
        {
          // We have got the selected content, inform the clipboard event listener (if we have one).
          if ( handler->mClipboardEventNotifier )
          {
            ClipboardEventNotifier& clipboardEventNotifier( ClipboardEventNotifier::GetImplementation( handler->mClipboardEventNotifier ) );
            std::string content( (char*) selectionData->data, selectionData->length );
            clipboardEventNotifier.SetContent( content );
            clipboardEventNotifier.EmitContentSelectedSignal();
          }

          // Claim the ownership of the SECONDARY selection.
          ecore_x_selection_secondary_set(handler->mImpl->mWindow, "", 1);

          DALI_LOG_INFO( gSelectionEventLogFilter, Debug::General, "EcoreEventSelectionNotify: Content(%d):\n" , selectionData->length );
          DALI_LOG_INFO( gSelectionEventLogFilter, Debug::General, "======================================\n" );
          DALI_LOG_INFO( gSelectionEventLogFilter, Debug::General, "%s\n", selectionData->data );
          DALI_LOG_INFO( gSelectionEventLogFilter, Debug::General, "======================================\n" );
        }
      }
    }
    return ECORE_CALLBACK_PASS_ON;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Font Callbacks
  /////////////////////////////////////////////////////////////////////////////////////////////////
  /**
   * Called when a font name is changed.
   */
  static void VconfNotifyFontNameChanged( keynode_t* node, void* data )
  {
    EventHandler* handler = static_cast<EventHandler*>( data );

    StyleChange fontChange;
    fontChange.defaultFontChange = true;

    handler->SendEvent( fontChange );
  }

  /**
   * Called when a font size is changed.
   */
  static void VconfNotifyFontSizeChanged( keynode_t* node, void* data )
  {
    EventHandler* handler = static_cast<EventHandler*>( data );

    StyleChange fontChange;
    fontChange.defaultFontSizeChange = true;

    handler->SendEvent( fontChange );
  }

  // Data
  EventHandler* mHandler;
  std::vector<Ecore_Event_Handler*> mEcoreEventHandler;
  Ecore_X_Window mWindow;
};

EventHandler::EventHandler( RenderSurface* surface, CoreEventInterface& coreEventInterface, GestureManager& gestureManager, DamageObserver& damageObserver, DragAndDropDetectorPtr dndDetector )
: mCoreEventInterface(coreEventInterface),
  mGestureManager( gestureManager ),
  mStyleMonitor( StyleMonitor::Get() ),
  mDamageObserver( damageObserver ),
  mRotationObserver( NULL ),
  mDragAndDropDetector( dndDetector ),
  mAccessibilityManager( AccessibilityManager::Get() ),
  mClipboardEventNotifier( ClipboardEventNotifier::Get() ),
  mClipboard(Clipboard::Get()),
  mImfManager( ImfManager::Get() ),
  mImpl( NULL )
{
  Ecore_X_Window window = 0;

  if( surface->GetType() == Dali::RenderSurface::WINDOW )
  {
    // this code only works with the EcoreX11 RenderSurface so need to downcast
    ECore::WindowRenderSurface* ecoreSurface = dynamic_cast< ECore::WindowRenderSurface* >( surface );
    if( ecoreSurface )
    {
      // enable multi touch
      window = ecoreSurface->GetXWindow();
      ecore_x_input_multi_select( window );
    }
  }

  mImpl = new Impl(this, window);
}

EventHandler::~EventHandler()
{
  if(mImpl)
  {
    delete mImpl;
  }

  mGestureManager.Stop();
}

void EventHandler::SendEvent(TouchPoint& point, unsigned long timeStamp)
{
  if(timeStamp < 1)
  {
    timeStamp = GetCurrentMilliSeconds();
  }

  Integration::TouchEvent event;
  if (mCombiner.GetNextTouchEvent(point, timeStamp, event))
  {
    DALI_LOG_INFO(gTouchEventLogFilter, Debug::General, "%d: Device %d: Button state %d (%.2f, %.2f)\n", timeStamp, point.deviceId, point.state, point.local.x, point.local.y);

    // First the touch event & related gesture events are queued
    mCoreEventInterface.QueueCoreEvent( event );
    mGestureManager.SendEvent(event);

    // Next the events are processed with a single call into Core
    mCoreEventInterface.ProcessCoreEvents();
  }
}

void EventHandler::SendEvent(KeyEvent& keyEvent)
{
  Dali::PhysicalKeyboard physicalKeyboard = PhysicalKeyboard::Get();
  if ( physicalKeyboard )
  {
    if ( ! KeyLookup::IsDeviceButton( keyEvent.keyPressedName.c_str() ) )
    {
      GetImplementation( physicalKeyboard ).KeyReceived( keyEvent.time > 1 );
    }
  }

  // Create KeyEvent and send to Core.
  Integration::KeyEvent event(keyEvent.keyPressedName, keyEvent.keyPressed, keyEvent.keyCode,
  keyEvent.keyModifier, keyEvent.time, static_cast<Integration::KeyEvent::State>(keyEvent.state));
  mCoreEventInterface.QueueCoreEvent( event );
  mCoreEventInterface.ProcessCoreEvents();
}

void EventHandler::SendMouseWheelEvent( MouseWheelEvent& wheelEvent )
{
  // Create MouseWheelEvent and send to Core.
  Integration::MouseWheelEvent event(wheelEvent.direction, wheelEvent.modifiers, wheelEvent.point, wheelEvent.z, wheelEvent.timeStamp);
  mCoreEventInterface.QueueCoreEvent( event );
  mCoreEventInterface.ProcessCoreEvents();
}

void EventHandler::SendEvent(StyleChange styleChange)
{
  DALI_ASSERT_DEBUG( mStyleMonitor && "StyleMonitor Not Available" );
  GetImplementation( mStyleMonitor ).StyleChanged(styleChange);
}

void EventHandler::SendEvent( const DamageArea& area )
{
  mDamageObserver.OnDamaged( area );
}

void EventHandler::SendRotationPrepareEvent( const RotationEvent& event )
{
  if( mRotationObserver != NULL )
  {
    mRotationObserver->OnRotationPrepare( event );
  }
}

void EventHandler::SendRotationRequestEvent( )
{
  if( mRotationObserver != NULL )
  {
    mRotationObserver->OnRotationRequest( );
  }
}

void EventHandler::FeedTouchPoint( TouchPoint& point, int timeStamp)
{
  SendEvent(point, timeStamp);
}

void EventHandler::FeedWheelEvent( MouseWheelEvent& wheelEvent )
{
  SendMouseWheelEvent( wheelEvent );
}

void EventHandler::FeedKeyEvent( KeyEvent& event )
{
  SendEvent( event );
}

void EventHandler::FeedEvent( Integration::Event& event )
{
  mCoreEventInterface.QueueCoreEvent( event );
  mCoreEventInterface.ProcessCoreEvents();
}

void EventHandler::Reset()
{
  mCombiner.Reset();

  // Any touch listeners should be told of the interruption.
  Integration::TouchEvent event;
  TouchPoint point(0, TouchPoint::Interrupted, 0, 0);
  event.AddPoint( point );

  // First the touch event & related gesture events are queued
  mCoreEventInterface.QueueCoreEvent( event );
  mGestureManager.SendEvent( event );

  // Next the events are processed with a single call into Core
  mCoreEventInterface.ProcessCoreEvents();
}

void EventHandler::SetDragAndDropDetector( DragAndDropDetectorPtr detector )
{
  mDragAndDropDetector = detector;
}

void EventHandler::SetRotationObserver( RotationObserver* observer )
{
  mRotationObserver = observer;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
