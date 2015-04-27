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
#include <key-grab.h>

// EXTERNAL INCLUDES
#include <Ecore.h>
#include <Ecore_Wayland.h>

// INTERNAL INCLUDES
#include <window.h>
#include <key-impl.h>
#include <ecore-wl-types.h>
#include <window-render-surface.h>
#include <adaptor-impl.h>
#include "keyrouter-client-protocol.h"

namespace
{
const int KEYCODE_GAP = 8; /* keycode gap between kernel keycode and appside keycode */

struct wl_keyrouter *keyrouter = NULL;
Dali::WlDisplay* display = NULL;
int sync_return_value;
int done;

static void
global_registry_handler(void * data, struct wl_registry * registry, uint32_t id, const char * interface, uint32_t version)
{
   if (0 == strncmp(interface, "wl_keyrouter", 12) )
   {
      keyrouter = (wl_keyrouter*)wl_registry_bind(registry, id, &wl_keyrouter_interface, 1);
   }
}

static void
global_registry_remover(void * data, struct wl_registry * registry, uint32_t id)
{
}

static const struct wl_registry_listener registry_listener = {
    global_registry_handler,
    global_registry_remover
};

static void
keygrab_wl_sync_callback(void *data, struct wl_callback *callback, uint32_t serial)
{
   (void) data;
   (void) callback;
   (void) serial;

   done = 1;
   wl_callback_destroy(callback);
}

static const struct wl_callback_listener keygrab_wl_sync_listener =
{
   keygrab_wl_sync_callback
};

static void
keygrab_request(struct wl_keyrouter *wl_keyrouter, struct wl_surface *surface, uint32_t key, uint32_t mode)
{
   wl_keyrouter_set_keygrab(wl_keyrouter, surface, key+KEYCODE_GAP/* keycode */, mode);
}

static void
keyungrab_request(struct wl_keyrouter *wl_keyrouter, struct wl_surface *surface, uint32_t key)
{
   wl_keyrouter_unset_keygrab(wl_keyrouter, surface, key+KEYCODE_GAP/* keycode */);
}

static int
keygrab_sync_request(struct wl_keyrouter *wl_keyrouter, struct wl_surface *surface, uint32_t key, uint32_t mode)
{
   struct wl_callback *callback;

   /* Request to grab a key */
   keygrab_request(wl_keyrouter, surface, key, mode);

   /* Send sync to wayland compositor and register sync callback to exit while dispatch loop below */
   done = 0;
   callback = wl_display_sync(display);
   wl_callback_add_listener(callback, &keygrab_wl_sync_listener, NULL);

   while (!done)
     wl_display_dispatch(display);

   return sync_return_value;
}

static int
keyungrab_sync_request(struct wl_keyrouter *wl_keyrouter, struct wl_surface *surface, uint32_t key)
{
   struct wl_callback *callback;

   keyungrab_request(wl_keyrouter, surface, key);

   done = 0;
   callback = wl_display_sync(display);
   wl_callback_add_listener(callback, &keygrab_wl_sync_listener, NULL);

   return sync_return_value;
}

} // unnamed namespace

namespace Dali
{

namespace KeyGrab
{

bool GrabKey( Window window, Dali::KEY daliKey, KeyGrabMode grabMode )
{
  wl_keyrouter_mode wlGrabMode;
  if( grabMode == TOP_POSITION )
  {
    wlGrabMode = WL_KEYROUTER_MODE_TOPMOST;
  }
  else if( grabMode == SHARED )
  {
    wlGrabMode = WL_KEYROUTER_MODE_SHARED;
  }
  else
  {
    return false;
  }

  if( keyrouter == NULL )
  {
    display = ecore_wl_display_get();
    struct wl_registry *registry = wl_display_get_registry( display );
    wl_registry_add_listener( registry, &registry_listener, NULL );
  }

  if( keyrouter )
  {
    //RenderSurface& surface = Adaptor::Get().GetSurface();
    //ECore::WindowRenderSurface* ecoreSurface = dynamic_cast< ECore::WindowRenderSurface* >( &surface );
    //struct wl_surface* wlSurface = ecoreSurface->GetWlSurface();

    Dali::WlSurface* wlSurface = ecore_wl_window_surface_get( AnyCast<Ecore_Wl_Window*>( window.GetNativeHandle() ) );
    int ret = keygrab_sync_request(keyrouter, wlSurface, daliKey, wlGrabMode);
    return ret==0;
  }
  else
  {
    return false;
  }
}

bool UngrabKey( Window window, Dali::KEY daliKey )
{
  if( keyrouter == NULL )
  {
    display = ecore_wl_display_get();
    struct wl_registry *registry = wl_display_get_registry( display );
    wl_registry_add_listener( registry, &registry_listener, NULL );
  }

  if( keyrouter )
  {
    //RenderSurface& surface = Adaptor::Get().GetSurface();
    //ECore::WindowRenderSurface* ecoreSurface = dynamic_cast< ECore::WindowRenderSurface* >( &surface );
    //struct wl_surface* wlSurface = ecoreSurface->GetWlSurface();

    Dali::WlSurface* wlSurface = ecore_wl_window_surface_get( AnyCast<Ecore_Wl_Window*>( window.GetNativeHandle() ) );
    int ret = keyungrab_sync_request(keyrouter, wlSurface, daliKey);
    return ret==0;
  }
  else
  {
    return false;
  }
}

} // namespace KeyGrab

} // namespace Dali


