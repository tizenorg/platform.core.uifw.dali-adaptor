/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//BEGIN_INCLUDE(all)
#include <jni.h>
#include <errno.h>

#include <android/log.h>
#include <android_native_app_glue.h>

#include <android-interface.h>
#include <android-jobs.h>
 #include "nativeLogging.h"

/**
 * Our saved state data.
 */
struct saved_state 
{
  int x;
  int y;
};

/**
 * Shared state for our app.
 */
struct app_state
{
  struct android_app* app;

  int32_t width;
  int32_t height;
  struct saved_state state;
};

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct app_state* engine)
{
}

class InputDeliveryJob : public Jobs::Job 
{
  int32_t mId;
  int32_t mEvent;
  float   mX;
  float   mY;
  int64_t mTimestamp;

public:
  void Execute(void *p)
  {
      androidcore_queue_touch_event(mId, mEvent, mX, mY, mTimestamp); 
  }

  // Convert timestamp to milliseconds.
  InputDeliveryJob(int32_t id, int32_t event, float x, float y, int64_t timestamp) : mId(id), mEvent(event), mX(x), mY(y), mTimestamp(timestamp / 1000000u) 
  {    
  }
};

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) 
{
  if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) 
  { 
      Jobs::JobHandle inputJob = new InputDeliveryJob(AMotionEvent_getPointerId(event, 0), 
                                                      AMotionEvent_getAction(event), 
                                                      AMotionEvent_getX(event, 0), 
                                                      AMotionEvent_getY(event, 0), 
                                                      AMotionEvent_getEventTime(event));
      Jobs::add_job(Jobs::get_main_queue(), inputJob);
      return 1;
  }
  return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) 
{
    struct app_state* engine = (struct app_state*)app->userData;
    switch (cmd) 
    {
        case APP_CMD_SAVE_STATE:
          /*
            // The system has asked us to save our current state.  Do so.
            app_state->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)app_state->app->savedState) = app_state->state;
            app_state->app->savedStateSize = sizeof(struct saved_state);
          */
          break;

        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            // Surface created
            if (engine->app->window) 
            {
              androidcore_surfaceCreated(engine->app->window);
            }
            break;

        case APP_CMD_TERM_WINDOW:
            // Surface destroyed
            // The window is being hidden or closed, clean it up.
            androidcore_surfaceDestroyed(engine->app->window);
            break;

        case APP_CMD_GAINED_FOCUS:
            // Restore
            androidcore_resume();
            break;

        case APP_CMD_LOST_FOCUS:
            // Pause
            androidcore_pause();
            break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */


 bool        keepRunningMainLoop;
 bool        isLooperRunning;
 std::string currentDemoName;
 ALooper*    daliLooper;

 extern void demo_main();
 extern void gold_main();
 extern void text_utf_example_main();
 extern void hello_main();
 extern void unlock_main();
 extern void gaussian_blur_main();

 extern void circle_unlock_main();
 extern void cluster_example_main();
 extern void cube_transition_effect_main();
 extern void motion_blur_main();
 extern void motion_stretch_main();
 extern void multi_scroll_view_main();
 extern void page_turn_main();
 extern void scroll_view_effect_main();
 extern void slide_layout_main();
 extern void soft_button_effect_calculator_main();
 extern void soft_button_dialer_main();
 extern void dissolve_effect_main();
 extern void magnifier_example_main();
 extern void item_view_main();

 void quit_looper()
 {
  keepRunningMainLoop = false;
  ALooper_wake(daliLooper);
 }

 void wake_looper()
 {
  ALooper_wake(daliLooper);
 }

void android_main(struct android_app* state) 
{
  srand(0);
  struct app_state engine;
  currentDemoName = "demo";
  bool first = true;

  // Make sure glue isn't stripped.
  app_dummy();

  memset(&engine, 0, sizeof(app_state));
  state->userData = &engine;
  state->onAppCmd = engine_handle_cmd;
  state->onInputEvent = engine_handle_input;
  engine.app = state;
  daliLooper = state->looper;
  AConfiguration* androidConfiguration = state->config;
  int32_t heightDp = AConfiguration_getScreenHeightDp(androidConfiguration);
  int32_t widthDp = AConfiguration_getScreenWidthDp(androidConfiguration);
  LOGW("Configuration DP %d %d\n", heightDp, widthDp);
  android_setDpi(widthDp, heightDp);

  while (1)
  {
    if (currentDemoName == "demo")
    {
      androidcore_init(&demo_main);
    }
    else if (currentDemoName == "gold-ui.demo")
    {
      androidcore_init(&gold_main);
    }  
    else if (currentDemoName == "hello")
    {
      androidcore_init(&hello_main);
    }
    else if (currentDemoName == "text_utf")
    {
      androidcore_init(&text_utf_example_main);
    }
    else if (currentDemoName == "metal-ui.demo")
    {
      androidcore_init(&gaussian_blur_main);
    }
    else if (currentDemoName == "unlock_blur_bubble.demo")
    {
      androidcore_init(&unlock_main);
    }
    else if (currentDemoName == "circle-unlock.demo")
    {
      androidcore_init(&circle_unlock_main);
    }
    else if (currentDemoName == "cluster.demo")
    {
      androidcore_init(&cluster_example_main);
    }
    else if (currentDemoName == "cube-transition-effect.demo")
    {
      androidcore_init(&cube_transition_effect_main);
    }
    else if (currentDemoName == "motion-blur.demo")
    {
      androidcore_init(&motion_blur_main);
    }
    else if (currentDemoName == "motion-stretch.demo")
    {
      androidcore_init(&motion_stretch_main);
    }
    else if (currentDemoName == "multi-scroll-view.demo")
    {
      androidcore_init(&multi_scroll_view_main);
    }
    else if (currentDemoName == "page-turn-view.demo")
    {
      androidcore_init(&page_turn_main);
    }
    else if (currentDemoName == "scroll-view-effect.demo")
    {
      androidcore_init(&scroll_view_effect_main);
    }
    else if (currentDemoName == "slide_layout_main")
    {
      androidcore_init(&slide_layout_main);
    }
    else if (currentDemoName == "soft-button-effect-calculator.demo")
    {
      androidcore_init(&soft_button_effect_calculator_main);
    }
    else if (currentDemoName == "soft-button-effect.demo")
    {
      androidcore_init(&soft_button_dialer_main);
    }
    else if (currentDemoName == "dissolve-effect-unlock.demo")
    {
      androidcore_init(&dissolve_effect_main);
    }
    else if (currentDemoName == "magnifier.demo")
    {
      androidcore_init(&magnifier_example_main);
    }
    else if (currentDemoName == "item-view.demo")
    {
      androidcore_init(&item_view_main);
    }

    keepRunningMainLoop = true;
    androidcore_SetAssetManager(state->activity->assetManager);
    Jobs::Create();
    androidcore_OnLoad();
    currentDemoName = "demo";

    if (!first)
    {
      engine_handle_cmd(state, APP_CMD_INIT_WINDOW);
      engine_handle_cmd(state, APP_CMD_GAINED_FOCUS);
    }

    first = false;

  #if 0
      if (state->savedState != NULL) 
      {
          // We are starting with a previous saved state; restore from it.
          engine.state = *(struct saved_state*)state->savedState;
      }
  #endif

      // loop waiting for stuff to do.

      while (keepRunningMainLoop) 
      {
          isLooperRunning = true;
          // Read all pending events.
          int ident;
          int events;
          struct android_poll_source* source;

          // If not animating, we will block forever waiting for events.
          // If animating, we loop until all events are read, then continue
          // to draw the next frame of animation.
          while ((ident = ALooper_pollAll(3, NULL, &events, (void**)&source)) >= 0) 
          {
          // Process this event.
            if (source != NULL)
            {
              source->process(state, source);
            }
          
            // Check if we are exiting.
            if (state->destroyRequested != 0) 
            {
                // quit
                engine_term_display(&engine);
                return;
            }
          }
      }

      isLooperRunning = false;
      androidcore_quit();
    }
}

//END_INCLUDE(all)