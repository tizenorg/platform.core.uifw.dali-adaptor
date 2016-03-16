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
#include <video-player-impl.h>

// EXTERNAL INCLUDES
#include <dlfcn.h>

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
const char* VIDEO_PLUGIN_SO( "libdali-videoplayer-plugin.so" );

Dali::BaseHandle Create()
{
  return Dali::VideoPlayer::New();
}

Dali::TypeRegistration type( typeid( Dali::VideoPlayer ), typeid( Dali::BaseHandle ), Create );

} // unnamed namespace

VideoPlayerPtr VideoPlayer::New()
{
  Dali::VideoPlayerPtr player = new VideoPlayer();
  return player;
}

VideoPlayer::VideoPlayer()
: mPlugin( NULL ),
  mHandle( NULL ),
  mCreateVideoPlayerPtr( NULL ),
  mDestroyVideoPlayerPtr( NULL )
{
}

VideoPlayer::~VideoPlayer()
{
  if( mHandle != NULL )
  {
    if( mDestroyVideoPlayerPtr != NULL )
    {
      mDestroyVideoPlayerPtr( mPlugin );
    }

    dlclose( mHandle );
  }
}

bool VideoPlayer::Initialize()
{
  char* error = NULL;

  mHandle = dlopen( VIDEO_PLUGIN_SO, RTLD_LAZY );

  error = dlerror();
  if( mHandle == NULL || error != NULL )
  {
    DALI_LOG_ERROR( "VideoPlayer::Initialize(), dlopen error: %s\n", error );
    return false;
  }

  mCreateVideoPlayerPtr = reinterpret_cast< CreateVideoPlayerFunction >( dlsym( mHandle, "CreateVideoPlayerPlugin" ) );
  if( mCreateVideoPlayerPtr == NULL )
  {
    DALI_LOG_ERROR( "Can't load symbol CreateVideoPlayerPlugin(), error: %s\n", error );
    return false;
  }

  mPlugin = mCreateVideoPlayerPtr();

  if( mPlugin == NULL )
  {
    DALI_LOG_ERROR( "Can't create the VideoPlayerPlugin object" );
    return false;
  }

  mDestroyVideoPlayerPtr = reinterpret_cast< DestroyVideoPlayerFunction >( dlsym( mHandle, "DestroyVideoPlayerPlugin" ) );
  if( mDestroyVideoPlayerPtr == NULL )
  {
    DALI_LOG_ERROR( "Can't load symbol DestroyVideoPlayerPlugin(), error: %s\n", error );
    return false;
  }

  return true;
}

void VideoPlayer::SetUrl( const std::string& url )
{
  if( mPlugin != NULL )
  {
    mPlugin->SetUrl( url );
  }
}

std::string VideoPlayer::GetUrl()
{
  if( mPlugin != NULL )
  {
    return mPlugin->GetUrl();
  }
}

void VideoPlayer::SetLooping(bool looping)
{
  if( mPlugin != NULL )
  {
    mPlugin->SetLooping( looping );
  }
}

bool VideoPlayer::IsLooping()
{
  if( mPlugin != NULL )
  {
    return mPlugin->IsLooping();
  }
}

bool VideoPlayer::Play()
{
  if( mPlugin != NULL )
  {
    return mPlugin->Play();
  }
}

bool VideoPlayer::Pause()
{
  if( mPlugin != NULL )
  {
    return mPlugin->Pause();
  }
}

bool VideoPlayer::Resume()
{
  if( mPlugin != NULL )
  {
    return mPlugin->Resume();
  }
}

bool VideoPlayer::Stop()
{
  if( mPlugin != NULL )
  {
    return mPlugin->Stop();
  }
}

bool VideoPlayer::Forward( int millisecond )
{
  if( mPlugin != NULL )
  {
    return mPlugin->Forward( millisecond );
  }
}

bool VideoPlayer::Backward( int millisecond )
{
  if( mPlugin != NULL )
  {
    return mPlugin->Backward( millisecond );
  }
}

bool VideoPlayer::SetMute( bool mute )
{
  if( mPlugin != NULL )
  {
    return mPlugin->SetMute( mute );
  }
}

bool VideoPlayer::IsMute()
{
  if( mPlugin != NULL )
  {
    return mPlugin->IsMute();
  }
}

bool VideoPlayer::SetVolume( float left, float right )
{
  if( mPlugin != NULL )
  {
    return mPlugin->SetVolume( left, right );
  }
}

void VideoPlayer::GetVolume( float* left, float* right )
{
  if( mPlugin != NULL )
  {
    mPlugin->GetVolume( left, right );
  }
}

const NativeImage& VideoPlayer::GetVideoStreamImage()
{
  if( mPlugin != NULL )
  {
    return mPlugin->GetVideoStreamImage();
  }
}

str::string VideoPlayer::GetState()
{
  if( mPlugin != NULL )
  {
    return mPlugin->GetState();
  }
}

};

} // namespace Adaptor;
} // namespace Internal;
} // namespace Dali;

