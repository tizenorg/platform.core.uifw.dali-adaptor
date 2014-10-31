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
#include "video-player-impl.h"

// EXTERNAL INCLUDES
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <player.h>
//#include <player_product.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace // unnamed namespace
{
// Type Registration
Dali::BaseHandle Create()
{
  return Dali::VideoPlayer::Get();
}

Dali::TypeRegistration type( typeid(Dali::VideoPlayer), typeid(Dali::BaseHandle), Create );

} // unnamed namespace


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

void VideoPlayer::Intialize()
{
  if(player_create(&mPlayer) != PLAYER_ERROR_NONE)
  {
  // error  
    std::cout << "Inialize() error" << std::endl;
  }

  if(mPlayer)
  {
    player_set_sound_type(mPlayer, SOUND_TYPE_MEDIA);

    if(player_set_looping(mPlayer, mLoopFlag) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error" << std::endl;
    }

    player_display_mode_e mode = PLAYER_DISPLAY_MODE_FULL_SCREEN;
    if(player_set_display_mode(mPlayer, mode) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error" << std::endl;
    }

    uintptr_t surface = mPixmap;
    if(player_set_display(mPlayer, PLAYER_DISPLAY_TYPE_X11, (void*)surface) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error" << std::endl;
    }

    if(player_set_x11_display_visible(mPlayer, true) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error" << std::endl;
    }
  } // if(mPlayer) 
}

void VideoPlayer::CreateVideoSurface()
{
  Dali::Any anyPix = mPixmapImage->GetPixmap(PixmapImage::ECORE_X11);
  mPixmap = Dali::AnyCast< Ecore_X_Pixmap >(anyPix);
}

void VideoPlayer::SetVideoFile(const std::string& strFilename)
{
  mVideoFilename = strFilename;
  if(mPlayer)
  {
    if(player_set_uri(mPlayer, mVideoFilename) != PLAYER_ERROR_NONE)
    {
      std::cout << "SetVideoFile() error" << std::endl;
    }
  }
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
    if(player_pause() != PLAYER_ERROR_NONE)
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
    if(player_get_play_position(mPlayer, &curPos) != PLAYER_ERROR_NONE)
    {
      std::cout << "Backward() error" << std::endl;
    }

    nextPos = curPos - millisecond;
    nextPos = (nextPos < 0.)? 0.:nextPos;
    if(player_set_play_position(mPlayer, nextPos, false, NULL, NULL) != PLAYER_ERROR_NONE)
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
    if(player_get_play_position(mPlayer, &curMillisecond) != PLAYER_ERROR_NONE)
    {
      std::cout << "Forward() error" << std::endl;
    }

    if(player_get_duration(mPlayer, &duration) != PLAYER_ERROR_NONE)
    {
      std::cout << "Forward() error" << std::endl;
    }

    nextPos = curPos + millisecond;
    nextPos = (nextPos > duration)? duration:nextPos;
 
    if(player_set_play_position(mPlayer, nextPos, false, NULL, NULL) != PLAYER_ERROR_NONE)
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

Dali::Image VideoPlayer::GetVideoStreamImage()
{
  Image image = Dali::Image::New(*mPixmapImage);

  return image;
}

} // namespace Adaptor

} // namespace Internal



} // namespace Dali
