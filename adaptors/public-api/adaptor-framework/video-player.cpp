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
#include <video-player.h>

// INTERNAL INCLUDES
#include <video-player-impl.h>
#include <adaptor-impl.h>

namespace Dali
{

VideoPlayer::VideoPlayer()
{
}

VideoPlayer VideoPlayer::New()
{
  return Internal::Adaptor::VideoPlayer::New();
}

VideoPlayer::~VideoPlayer()
{
}

bool VideoPlayer::Initialize()
{
  return GetImplementation(*this).Initialize();
}

void VideoPlayer::SetVideoFile(const std::string& strFilename)
{
  return GetImplementation(*this).SetVideoFile(strFilename);
}

void VideoPlayer::Play()
{
  return GetImplementation(*this).Play();
}

void VideoPlayer::Stop()
{
  GetImplementation(*this).Stop();
}

void VideoPlayer::Pause()
{
  GetImplementation(*this).Pause();
}

void VideoPlayer::Resume()
{
  GetImplementation(*this).Resume();
}

void VideoPlayer::Backward(int millisecond)
{
  GetImplementation(*this).Backward(millisecond);
}

void VideoPlayer::Forward(int millisecond)
{
  GetImplementation(*this).Forward(millisecond);
}

VideoPlayer::VideoPlayer( Internal::Adaptor::VideoPlayer* player )
: BaseHandle( player )
{
}

void VideoPlayer::LinkVideoStreamImage(Dali::ImageActor imageActor)
{
  GetImplementation(*this).LinkVideoStreamImage(imageActor);
}

} // namespace Dali
