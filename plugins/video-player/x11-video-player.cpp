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
#include "x11-video-player.h"

// EXTERNAL INCLUDES
#include <stdio.h>
#include <Ecore.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/public-api/shader-effects/shader-effect.h>

#include <adaptor-impl.h>

// The plugin factories
extern "C" DALI_EXPORT_API Dali::VideoPlayerPlugin* CreateVideoPlayerPlugin(void)
{
  return new Dali::X11VideoPlayer;
}

extern "C" DALI_EXPORT_API void DestroyVideoPlayerPlugin(Dali::VideoPlayerPlugin* plugin)
{
  delete plugin;
}

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace // unnamed namespace
{

static Eina_Bool PixmapUpdate(void *data, int type, void *event)
{
  Dali::Stage::GetCurrent().KeepRendering(0.0f);
  return ECORE_CALLBACK_PASS_ON;
}

} // unnamed namespace


Dali::VideoPlayer VideoPlayer::New()
{
  Dali::VideoPlayer player = Dali::VideoPlayer(new VideoPlayer());
  return player;
}

Dali::VideoPlayer VideoPlayer::Get()
{
  Dali::VideoPlayer player = VideoPlayer::New();
  return player;
}

VideoPlayer::~VideoPlayer()
{
  if(mPlayer)
  {
    if(player_destroy(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "~VIdeoPlayer() error" << std::endl;
    }
    mPlayer = NULL;
  }
}

void VideoPlayer::LinkVideoStreamImage(Dali::ImageActor imageActor)
{
  mImageActor = imageActor;

  CreateXPixmapSurface(0,0);
}

void VideoPlayer::Initialize(int width, int height)
{
  std::cout << "VideoPlayer::Initialize" << std::endl;

  InitX11Player();
}

void VideoPlayer::CreateXPixmapSurface(int width, int height)
{
  std::cout << "VideoPlayer::CreateVideoSurface() 0" << std::endl;
  mViewWidth = width;
  mViewHeight = height;

  Dali::RenderSurface& surface = Dali::Adaptor::Get().GetSurface();

  Dali::Any renderableSurface = surface.GetSurface();

  Ecore_X_Window daliWindow = Dali::AnyCast< Ecore_X_Window >(renderableSurface);

  std::cout << "VideoPlayer::CreateVideoSurface() 1" << std::endl;

  int depth = ecore_x_default_depth_get(ecore_x_display_get(), ecore_x_default_screen_get());

  mPixmap = ecore_x_pixmap_new(daliWindow, mViewWidth, mViewHeight, depth);

  std::cout << "VideoPlayer::CreateVideoSurface() 2" << std::endl;

  // create the graphics context for drawing.
  unsigned int foreground;
  Ecore_X_GC gc;
  foreground = 0x000033;
  gc = ecore_x_gc_new(mPixmap, ECORE_X_GC_VALUE_MASK_FOREGROUND, &foreground);
  ecore_x_drawable_rectangle_fill(static_cast<Ecore_X_Drawable>(mPixmap), gc, 0, 0, mViewWidth, mViewHeight);
  ecore_x_gc_free(gc);
  ecore_x_sync();

  // connect damage notify
//  ecore_x_damage_new (mPixmap, ECORE_X_DAMAGE_REPORT_RAW_RECTANGLES);
//  ecore_event_handler_add (ECORE_X_EVENT_DAMAGE_NOTIFY, PixmapUpdate, this);

  Dali::PixmapImagePtr pixmapImage = Dali::PixmapImage::New( Dali::Any(mPixmap) );
  mVideoStream = Dali::Image::New(*pixmapImage);

  mImageActor.SetImage(mVideoStream);

  std::cout << "VideoPlayer::CreateVideoSurface() e" << std::endl;
}

void VideoPlayer::InitX11Player()
{
  if(player_create(&mPlayer) != PLAYER_ERROR_NONE)
  {
  // error
    std::cout << "Inialize() error1 " << std::endl;
  }

  if(mPlayer)
  {
    if(player_set_sound_type(mPlayer, SOUND_TYPE_MEDIA) != PLAYER_ERROR_NONE)
    {
      std::cout << "Sound type error ===========================\n";
    }

    if(player_set_looping(mPlayer, mLoopFlag) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error 2" << std::endl;
    }

    if(player_set_display_mode(mPlayer, PLAYER_DISPLAY_MODE_FULL_SCREEN) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error 3" << std::endl;
    }

    uintptr_t surface = mPixmap;
    if(player_set_display(mPlayer, PLAYER_DISPLAY_TYPE_X11, (void*)surface) != PLAYER_ERROR_NONE)
    {
      std::cout << "player_set_display() error" << std::endl;
    }

    if(player_set_x11_display_visible(mPlayer, true) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error 4" << std::endl;
    }
  } // if(mPlayer)

  mTimer = Dali::Timer::New(20);
  mTimer.TickSignal().Connect(this, &VideoPlayer::Update);
  mTimer.Start();
}

void VideoPlayer::SetVideoFile(const std::string& strFilename)
{
  mVideoFilename = strFilename;
  if(mPlayer)
  {
    if(player_set_uri(mPlayer, mVideoFilename.c_str()) != PLAYER_ERROR_NONE)
    {
      std::cout << "SetVideoFile() error" << std::endl;
    }
  }
}

void VideoPlayer::SetSize(int width, int height)
{
  mViewWidth = width;
  mViewHeight = height;
}

void VideoPlayer::Play()
{
  if(mPlayer)
  {
    if(player_prepare(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Play() error" << std::endl;
    }

    if(player_start(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Play() error" << std::endl;
    }

  }
}

void VideoPlayer::Pause()
{
  if(mPlayer)
  {
    if(player_pause(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Pause() error" << std::endl;
    }

  }
}

void VideoPlayer::Backward(int millisecond)
{
  int curPos;
  int nextPos;
  curPos = 0;
  nextPos = 0;

  if(mPlayer)
  {
    if(player_get_position(mPlayer, &curPos) != PLAYER_ERROR_NONE)
    {
      std::cout << "Backward() error" << std::endl;
    }

    nextPos = curPos - millisecond;
    nextPos = (nextPos < 0.)? 0.:nextPos;

    if(player_set_position(mPlayer, nextPos, NULL, NULL) != PLAYER_ERROR_NONE)
    {
      std::cout << "Backward() error" << std::endl;
    }
  }
}

void VideoPlayer::Forward(int millisecond)
{
  int curPos;
  int nextPos;
  int duration;
  curPos = 0;
  nextPos = 0;
  duration = 0;

  if(mPlayer)
  {
    if(player_get_position(mPlayer, &curPos) != PLAYER_ERROR_NONE)
    {
      std::cout << "Forward() error" << std::endl;
    }

    if(player_get_duration(mPlayer, &duration) != PLAYER_ERROR_NONE)
    {
      std::cout << "Forward() error" << std::endl;
    }

    nextPos = curPos + millisecond;
    nextPos = (nextPos > duration)? duration:nextPos;

    if(player_set_position(mPlayer, nextPos, NULL, NULL) != PLAYER_ERROR_NONE)
    {
      std::cout << "Forward() error" << std::endl;
    }
  }
}

void VideoPlayer::Stop()
{
  if(mPlayer)
  {
    if(player_stop(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Stop() error" << std::endl;
    }
  }
}

void VideoPlayer::Resume()
{
  if(mPlayer)
  {
    if(player_start(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Resume() error" << std::endl;
    }
  }
}

VideoPlayer::VideoPlayer()
: mLoopFlag( false )
, mViewWidth(0)
, mViewHeight(0)
{
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
