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
#include <video-player-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>

// EXTERNAL INCLUDES
#include <dlfcn.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace // unnamed namespace
{

const char* VIDEO_PLUGIN_SO("libdali-videoplayer-plugin.so");

Dali::BaseHandle Create()
{
  return Dali::VideoPlayer::New();
}

Dali::TypeRegistration type( typeid(Dali::VideoPlayer), typeid(Dali::BaseHandle), Create );

} // unnamed namespace


Dali::VideoPlayer VideoPlayer::New()
{
  Dali::VideoPlayer player = Dali::VideoPlayer( new VideoPlayer() );

  return player;
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

void VideoPlayer::LinkVideoStreamImage( Dali::ImageActor imageActor )
{
  if( mPlugin != NULL )
  {
    mPlugin->LinkVideoStreamImage( imageActor );
  }
}

bool VideoPlayer::Initialize()
{
  char* error = NULL;
  mHandle = dlopen( VIDEO_PLUGIN_SO, RTLD_LAZY );

  error = dlerror();
  if( mHandle == NULL || error != NULL )
  {
    DALI_LOG_ERROR( "VideoPlayer::Initialize(), dlopen error : %s\n", error );
    return false;
  }

  mCreateVideoPlayerPtr = reinterpret_cast<CreateVideoPlayerFunction>( dlsym( mHandle, "CreateVideoPlayerPlugin") );
  if( mCreateVideoPlayerPtr == NULL )
  {
    DALI_LOG_ERROR( "Cannot load symbol CreateVideoPlayerPlugin() error : %s\n", dlerror() );
    return false;
  }

  mPlugin = mCreateVideoPlayerPtr();

  if( mPlugin == NULL )
  {
    DALI_LOG_ERROR( "Error - mCreateVideoPlayerPtr() is NULL" );
    return false;
  }

  mDestroyVideoPlayerPtr = reinterpret_cast<DestroyVideoPlayerFunction>( dlsym( mHandle, "DestroyVideoPlayerPlugin") );
  if( mDestroyVideoPlayerPtr == NULL )
  {
    DALI_LOG_ERROR( "Cannot load symbol DestroyVideoPlayerPlugin() error : %s\n", dlerror() );
    return false;
  }

  return true;
}

void VideoPlayer::SetVideoFile( const std::string& filename )
{
  if( mPlugin != NULL )
  {
    mPlugin->SetVideoFile( filename );
  }
}

void VideoPlayer::SetVideoLooping( bool looping )
{
  if( mPlugin != NULL )
  {
    mPlugin->SetVideoLooping( looping );
  }
}

bool VideoPlayer::IsVideoLooping()
{
  if( mPlugin != NULL )
  {
    return mPlugin->IsVideoLooping();
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

void VideoPlayer::Backward( int millisecond )
{
  if( mPlugin != NULL )
  {
    mPlugin->Backward( millisecond );
  }
}

void VideoPlayer::Forward( int millisecond )
{
  if( mPlugin != NULL )
  {
    mPlugin->Forward( millisecond );
  }
}

void VideoPlayer::Stop()
{
  if( mPlugin != NULL )
  {
    mPlugin->Stop();
  }
}

void VideoPlayer::Resume()
{
  if( mPlugin != NULL )
  {
    mPlugin->Resume();
  }
}

VideoPlayer::VideoPlayer()
: mHandle( NULL )
, mCreateVideoPlayerPtr( NULL )
, mDestroyVideoPlayerPtr( NULL )
{
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
