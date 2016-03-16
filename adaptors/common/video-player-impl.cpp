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
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace // unnamed namespace
{
const char* VIDEO_PLUGIN_SO( "libdali-video-player-plugin.so" );

Dali::BaseHandle Create()
{
  return Dali::VideoPlayer::New();
}

Dali::TypeRegistration type( typeid( Dali::VideoPlayer ), typeid( Dali::BaseHandle ), Create );

} // unnamed namespace

VideoPlayerPtr VideoPlayer::New()
{
  VideoPlayerPtr player = new VideoPlayer();
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

void VideoPlayer::Initialize()
{
  char* error = NULL;

  mHandle = dlopen( VIDEO_PLUGIN_SO, RTLD_LAZY );

  error = dlerror();
  if( mHandle == NULL || error != NULL )
  {
    DALI_LOG_ERROR( "VideoPlayer::Initialize(), dlopen error: %s\n", error );
    return;
  }

  mCreateVideoPlayerPtr = reinterpret_cast< CreateVideoPlayerFunction >( dlsym( mHandle, "CreateVideoPlayerPlugin" ) );
  if( mCreateVideoPlayerPtr == NULL )
  {
    DALI_LOG_ERROR( "Can't load symbol CreateVideoPlayerPlugin(), error: %s\n", error );
    return;
  }

  mPlugin = mCreateVideoPlayerPtr();

  if( mPlugin == NULL )
  {
    DALI_LOG_ERROR( "Can't create the VideoPlayerPlugin object" );
    return;
  }

  mDestroyVideoPlayerPtr = reinterpret_cast< DestroyVideoPlayerFunction >( dlsym( mHandle, "DestroyVideoPlayerPlugin" ) );
  if( mDestroyVideoPlayerPtr == NULL )
  {
    DALI_LOG_ERROR( "Can't load symbol DestroyVideoPlayerPlugin(), error: %s\n", error );
    return;
  }

  mPlugin->Initialize();
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

  return std::string( NULL );
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

  return false;
}

void VideoPlayer::Play()
{
  if( mPlugin != NULL )
  {
    mPlugin->Play();
  }
}

void VideoPlayer::Pause()
{
  if( mPlugin != NULL )
  {
    mPlugin->Pause();
  }
}

void VideoPlayer::Resume()
{
  if( mPlugin != NULL )
  {
    mPlugin->Resume();
  }
}

void VideoPlayer::Stop()
{
  if( mPlugin != NULL )
  {
    mPlugin->Stop();
  }
}

void VideoPlayer::Forward( int millisecond )
{
  if( mPlugin != NULL )
  {
    mPlugin->Forward( millisecond );
  }
}

void VideoPlayer::Backward( int millisecond )
{
  if( mPlugin != NULL )
  {
    mPlugin->Backward( millisecond );
  }
}

void VideoPlayer::SetMute( bool mute )
{
  if( mPlugin != NULL )
  {
    mPlugin->SetMute( mute );
  }
}

bool VideoPlayer::IsMuted()
{
  if( mPlugin != NULL )
  {
    return mPlugin->IsMuted();
  }

  return false;
}

void VideoPlayer::SetVolume( float left, float right )
{
  if( mPlugin != NULL )
  {
    mPlugin->SetVolume( left, right );
  }
}

void VideoPlayer::GetVolume( float* left, float* right )
{
  if( mPlugin != NULL )
  {
    mPlugin->GetVolume( left, right );
  }
}

Dali::NativeImage VideoPlayer::GetVideoStreamImage() const
{
  if( mPlugin != NULL )
  {
    return mPlugin->GetVideoStreamImage();
  }

  Dali::NativeImage nativeImage;
  return nativeImage;
}

std::string VideoPlayer::GetState()
{
  if( mPlugin != NULL )
  {
    return mPlugin->GetState();
  }

  return std::string( NULL );
}

} // namespace Adaptor;
} // namespace Internal;
} // namespace Dali;

