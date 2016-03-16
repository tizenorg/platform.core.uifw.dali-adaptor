/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

namespace Dali
{

VideoPlayer::VideoPlayer()
{
}

VideoPlayer::VideoPlayer( Internal::Adaptor::VideoPlayer* internal )
: BaseHandle( internal )
{
}

VideoPlayer::~VideoPlayer()
{
}

VideoPlayer VideoPlayer::New()
{
  Internal::Adaptor::VideoPlayer player = Internal::Adaptor::VideoPlayer::New();

  if( player )
  {
    player.Initialize();
  }

  return videoPlayer( player.Get() );
}

void VideoPlayer::SetUrl( const std::string& url )
{
  GetImplementation( *this ).SetUrl( url );
}

std::string VideoPlayer::GetUrl()
{
  return GetImplementation( *this ).GetUrl();
}

void VideoPlayer::SetLooping(bool looping)
{
  GetImplementation( *this ).SetLooping();
}

bool VideoPlayer::IsLooping()
{
  return GetImplementation( *this ).IsLooping();
}

bool VideoPlayer::Play()
{
  return GetImplementation( *this ).Play();
}

bool VideoPlayer::Pause()
{
  return GetImplementation( *this ).Pause();
}

bool VideoPlayer::Resume()
{
  return GetImplementation( *this ).Resume();
}

bool VideoPlayer::Stop()
{
  return GetImplementation( *this ).Stop();
}

bool VideoPlayer::Forward( int millisecond )
{
  return GetImplementation( *this ).Forward( millisecond );
}

bool VideoPlayer::Backward( int millisecond )
{
  return GetImplementation( *this ).Backward( millisecond );
}

bool VideoPlayer::SetMute( bool mute )
{
  return GetImplementation( *this ).SetMute( mute );
}

bool VideoPlayer::IsMute()
{
  return GetImplementation( *this ).IsMute();
}

bool VideoPlayer::SetVolume( float left, float right )
{
  return GetImplementation( *this ).SetVolume( left, right );
}

void VideoPlayer::GetVolume( float* left, float* right )
{
  GetImplementation( *this ).GetVolume( left, right );
}

const NativeImage& VideoPlayer::GetVideoStreamImage()
{
  return GetImplementation( *this ).GetVideoStreamImage();
}

str::string VideoPlayer::GetState()
{
  return GetImplementation( *this ).GetState();
}

};

} // namespace Dali;

