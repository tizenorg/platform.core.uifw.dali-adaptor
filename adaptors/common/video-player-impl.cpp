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

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/common/stage.h>

#include <adaptor-impl.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace // unnamed namespace
{

const char* VIDEO_PLUGIN_SO("libdali-video-player-plugin.so");

// Type Registration
Dali::BaseHandle Create()
{
  return Dali::VideoPlayer::Get();
}

Dali::TypeRegistration type( typeid(Dali::VideoPlayer), typeid(Dali::BaseHandle), Create );

} // unnamed namespace


Dali::VideoPlayer VideoPlayer::New()
{
  Dali::VideoPlayer player = Dali::VideoPlayer(new VideoPlayer());
  return player;
}

VideoPlayer::~VideoPlayer()
{
  if(mHandle != NULL)
  {
    dlclose(mHandle);
  }
}

void VideoPlayer::LinkVideoStreamImage(Dali::ImageActor imageActor)
{
  mImageActor = imageActor;
}

void VideoPlayer::Initialize(int width, int height)
{
  std::cout << "VideoPlayer::Initialize" << std::endl;
  mHandle = dlopen(VIDEO_PLUGIN_SO, RTLD_LAZY);
  if(mHandle == NULL)
  {
    std::cout << "VideoPlayer() error - dlopen can't open so file";
  }
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
}

void VideoPlayer::Forward(int millisecond)
{
}

void VideoPlayer::Stop()
{
}

void VideoPlayer::Resume()
{
}

VideoPlayer::VideoPlayer()
: mHandle (NULL)
, mLoopFlag( false )
, mViewWidth(0)
, mViewHeight(0)
{
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
