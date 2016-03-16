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
  Internal::Adaptor::VideoPlayerPtr player = Internal::Adaptor::VideoPlayer::New();

  if( player )
  {
    player->Initialize();
  }

  return VideoPlayer( player.Get() );
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
  GetImplementation( *this ).SetLooping( looping );
}

bool VideoPlayer::IsLooping()
{
  return GetImplementation( *this ).IsLooping();
}

void VideoPlayer::Play()
{
  GetImplementation( *this ).Play();
}

void VideoPlayer::Pause()
{
  GetImplementation( *this ).Pause();
}

void VideoPlayer::Resume()
{
  GetImplementation( *this ).Resume();
}

void VideoPlayer::Stop()
{
  GetImplementation( *this ).Stop();
}

void VideoPlayer::Forward( int millisecond )
{
  GetImplementation( *this ).Forward( millisecond );
}

void VideoPlayer::Backward( int millisecond )
{
  GetImplementation( *this ).Backward( millisecond );
}

void VideoPlayer::SetMute( bool mute )
{
  GetImplementation( *this ).SetMute( mute );
}

bool VideoPlayer::IsMute()
{
  return GetImplementation( *this ).IsMute();
}

void VideoPlayer::SetVolume( float left, float right )
{
  GetImplementation( *this ).SetVolume( left, right );
}

void VideoPlayer::GetVolume( float* left, float* right )
{
  GetImplementation( *this ).GetVolume( left, right );
}

NativeImage VideoPlayer::GetVideoStreamImage() const
{
  return GetImplementation( *this ).GetVideoStreamImage();
}

std::string VideoPlayer::GetState()
{
  return GetImplementation( *this ).GetState();
}

} // namespace Dali;

