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
#include <tizen-video-player.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/devel-api/threading/mutex.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES

// The plugin factories
extern "C" DALI_EXPORT_API Dali::VideoPlayerPlugin* CreateVideoPlayerPlugin( void )
{
  return new Dali::Plugin::TizenVideoPlayer;
}

extern "C" DALI_EXPORT_API void DestroyVideoPlayerPlugin( Dali::VideoPlayerPlugin* plugin )
{
  if( plugin != NULL )
  {
    delete plugin;
  }
}

namespace Dali
{

namespace Plugin
{

namespace
{

const int TIMER_INTERVAL( 20 );

static void MediaPacketVideoDecodedCb( media_packet_h packet, void* user_data )
{
  TizenVideoPlayer* player = static_cast< TizenVideoPlayer* >( user_data );

  if( player == NULL )
  {
    DALI_LOG_ERROR( "Decoded callback got Null pointer as user_data." );
    return;
  }

  Dali::Mutex::ScopedLock lock( player->mPacketMutex );
  player->mPacketVector.PushBack( packet );
}

void LogPlayerError( int error )
{
  if( error != PLAYER_ERROR_NONE )
  {
    switch( error )
    {
      case PLAYER_ERROR_OUT_OF_MEMORY:
        DALI_LOG_ERROR( "Player error: Out of memory\n" );
        return;
      case PLAYER_ERROR_INVALID_PARAMETER:
        DALI_LOG_ERROR( "Player error: Invalid parameter\n" );
        return;
      case PLAYER_ERROR_NO_SUCH_FILE:
        DALI_LOG_ERROR( "Player error: No such file\n" );
        return;
      case PLAYER_ERROR_INVALID_OPERATION:
        DALI_LOG_ERROR( "Player error: Invalid operation\n" );
        return;
      case PLAYER_ERROR_FILE_NO_SPACE_ON_DEVICE:
        DALI_LOG_ERROR( "Player error: No space on device\n" );
        return;
      case PLAYER_ERROR_FEATURE_NOT_SUPPORTED_ON_DEVICE:
        DALI_LOG_ERROR( "Player error: Not supported feature on device\n" );
        return;
      case PLAYER_ERROR_SEEK_FAILED:
        DALI_LOG_ERROR( "Player error: Seek failed\n" );
        return;
      case PLAYER_ERROR_INVALID_STATE:
        DALI_LOG_ERROR( "Player error: Invalid state\n" );
        return;
      case PLAYER_ERROR_NOT_SUPPORTED_FILE:
        DALI_LOG_ERROR( "Player error: Not supported file\n" );
        return;
      case PLAYER_ERROR_INVALID_URI:
        DALI_LOG_ERROR( "Player error: Invalid uri\n" );
        return;
      case PLAYER_ERROR_SOUND_POLICY:
        DALI_LOG_ERROR( "Player error: Sound policy\n" );
        return;
      case PLAYER_ERROR_CONNECTION_FAILED:
        DALI_LOG_ERROR( "Player error: Connection failed\n" );
        return;
      case PLAYER_ERROR_VIDEO_CAPTURE_FAILED:
        DALI_LOG_ERROR( "Player error: Video capture failed\n" );
        return;
      case PLAYER_ERROR_DRM_EXPIRED:
        DALI_LOG_ERROR( "Player error: DRM expired\n" );
        return;
      case PLAYER_ERROR_DRM_NO_LICENSE:
        DALI_LOG_ERROR( "Player error: No license\n" );
        return;
      case PLAYER_ERROR_DRM_FUTURE_USE:
        DALI_LOG_ERROR( "Player error: License for future use\n" );
        return;
      case PLAYER_ERROR_DRM_NOT_PERMITTED:
        DALI_LOG_ERROR( "Player error: Format not permitted\n" );
        return;
      case PLAYER_ERROR_RESOURCE_LIMIT:
        DALI_LOG_ERROR( "Player error: Resource limit\n" );
        return;
      case PLAYER_ERROR_PERMISSION_DENIED:
        DALI_LOG_ERROR( "Player error: Permission denied\n" );
        return;
      case PLAYER_ERROR_SERVICE_DISCONNECTED:
        DALI_LOG_ERROR( "Player error: Service disconnected\n" );
        return;
      case PLAYER_ERROR_BUFFER_SPACE:
        DALI_LOG_ERROR( "Player error: Buffer space\n" );
        return;
    }
  }
}

} // unnamed namespace

TizenVideoPlayer::TizenVideoPlayer()
: mPlayerState( PLAYER_STATE_NONE )
 , mPacket( NULL )
{
  Any source;
  mNativeImageSourcePtr = Dali::NativeImageSource::New( source );
  mNativeImage = Dali::NativeImage::New( *mNativeImageSourcePtr );
}

TizenVideoPlayer::~TizenVideoPlayer()
{
}

void TizenVideoPlayer::GetPlayerState( player_state_e* state )
{
  if( player_get_state( mPlayer, state ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR( "Player error: Invalid parameter\n" );
    *state = PLAYER_STATE_NONE;
  }
}

void TizenVideoPlayer::SetUrl( const std::string& url )
{
  mUrl = url;

  GetPlayerState( &mPlayerState );

  if( mPlayerState == PLAYER_STATE_IDLE )
  {
    int error = player_set_uri( mPlayer, mUrl.c_str() );
    LogPlayerError( error );

    error = player_prepare( mPlayer );
    LogPlayerError( error );
  }
}

std::string TizenVideoPlayer::GetUrl()
{
  return mUrl;
}

void TizenVideoPlayer::SetLooping(bool looping)
{
  GetPlayerState( &mPlayerState );

  if( mPlayerState != PLAYER_STATE_NONE )
  {
    int error = player_set_looping( mPlayer, looping );
    LogPlayerError( error );
  }
}

bool TizenVideoPlayer::IsLooping()
{
  GetPlayerState( &mPlayerState );

  bool looping = false;
  if( mPlayerState != PLAYER_STATE_NONE )
  {
    int error = player_is_looping( mPlayer, &looping );
    LogPlayerError( error );
  }

  return looping;
}

void TizenVideoPlayer::Play()
{
  GetPlayerState( &mPlayerState );

  if( mPlayerState == PLAYER_STATE_READY || mPlayerState == PLAYER_STATE_PAUSED )
  {
    int error = player_start( mPlayer );
    LogPlayerError( error );
    mTimer.Start();
  }
}

void TizenVideoPlayer::Pause()
{
  GetPlayerState( &mPlayerState );

  if( mPlayerState == PLAYER_STATE_PLAYING )
  {
    mTimer.Stop();
    mPacketVector.Clear();
    int error = player_pause( mPlayer );
    LogPlayerError( error );
  }
}

void TizenVideoPlayer::Stop()
{
  GetPlayerState( &mPlayerState );

  if( mPlayerState == PLAYER_STATE_PLAYING || mPlayerState == PLAYER_STATE_PAUSED )
  {
    int error = player_stop( mPlayer );
    LogPlayerError( error );
    mTimer.Stop();
    mPacketVector.Clear();
  }
}

void TizenVideoPlayer::Forward( int millisecond )
{
  GetPlayerState( &mPlayerState );

  int curPos( 0 );
  int nextPos( 0 );

  if( mPlayerState == PLAYER_STATE_PLAYING || mPlayerState == PLAYER_STATE_PAUSED )
  {
    int error = player_get_play_position( mPlayer, &curPos );
    LogPlayerError( error );

    nextPos = curPos + millisecond;

    error = player_set_play_position( mPlayer, nextPos, true, NULL, NULL );
    LogPlayerError(error);
  }
}

void TizenVideoPlayer::Backward( int millisecond )
{
  GetPlayerState( &mPlayerState );

  int curPos( 0 );
  int nextPos( 0 );

  if( mPlayerState == PLAYER_STATE_PLAYING || mPlayerState == PLAYER_STATE_PAUSED )
  {
    int error = player_get_play_position( mPlayer, &curPos );
    LogPlayerError( error );

    nextPos = curPos - millisecond;
    nextPos = ( nextPos < 0 )? 0: nextPos;

    error = player_set_play_position( mPlayer, nextPos, true, NULL, NULL );
    LogPlayerError( error );
  }
}

void TizenVideoPlayer::SetMute( bool muted )
{
  GetPlayerState( &mPlayerState );

  if( mPlayerState == PLAYER_STATE_IDLE ||
      mPlayerState == PLAYER_STATE_READY ||
      mPlayerState == PLAYER_STATE_PLAYING ||
      mPlayerState == PLAYER_STATE_PAUSED
    )
  {
    int error = player_set_mute( mPlayer, muted );
    LogPlayerError( error );
  }
}

bool TizenVideoPlayer::IsMuted()
{
  GetPlayerState( &mPlayerState );
  bool muted = false;

   if( mPlayerState == PLAYER_STATE_IDLE ||
      mPlayerState == PLAYER_STATE_READY ||
      mPlayerState == PLAYER_STATE_PLAYING ||
      mPlayerState == PLAYER_STATE_PAUSED
    )
  {
    bool muted = false;
    int error = player_is_muted( mPlayer, &muted );
    LogPlayerError( error );
  }

  return muted;
}

void TizenVideoPlayer::SetVolume( float left, float right )
{
  GetPlayerState( &mPlayerState );

  int error = player_set_volume( mPlayer, left, right );
  LogPlayerError( error );
}

void TizenVideoPlayer::GetVolume( float* left, float* right )
{
  GetPlayerState( &mPlayerState );

  int error = player_get_volume( mPlayer, left, right );
  LogPlayerError( error );
}

const Dali::NativeImage& TizenVideoPlayer::GetVideoStreamImage()
{
  return mNativeImage;
}

std::string TizenVideoPlayer::GetState()
{
  return "NULL";
}

void TizenVideoPlayer::Initialize()
{
  int error;
  if( mPlayerState == PLAYER_STATE_NONE )
  {
    error = player_create( &mPlayer );
    LogPlayerError( error );
  }

  GetPlayerState( &mPlayerState );

  if( mPlayerState == PLAYER_STATE_IDLE )
  {
    error = player_set_media_packet_video_frame_decoded_cb( mPlayer, MediaPacketVideoDecodedCb, this );
    LogPlayerError( error );

    error = player_set_sound_type( mPlayer, SOUND_TYPE_MEDIA );
    LogPlayerError( error );

    error = player_set_display_mode( mPlayer, PLAYER_DISPLAY_MODE_FULL_SCREEN );
    LogPlayerError( error );

    error = player_set_display_mode( mPlayer, PLAYER_DISPLAY_MODE_FULL_SCREEN );
    LogPlayerError( error );

    error = player_set_display( mPlayer, PLAYER_DISPLAY_TYPE_NONE, NULL );
    LogPlayerError( error );

    error = player_set_display_visible( mPlayer, true );
    LogPlayerError( error );

    mTimer = Dali::Timer::New( TIMER_INTERVAL );
    mTimer.TickSignal().Connect( this, &TizenVideoPlayer::Update );
  }
}

bool TizenVideoPlayer::Update()
{
  if( mPlayerState != PLAYER_STATE_PLAYING && mPlayerState != PLAYER_STATE_READY)
  {
    mTimer.Stop();
    return false;
  }

  Dali::Mutex::ScopedLock lock( mPacketMutex );

  int error;

  if( mPacket != NULL )
  {
    error = media_packet_destroy( mPacket );
    if( error != MEDIA_PACKET_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Media packet destroy error: %d\n", error );
    }
    mPacket = NULL;
  }

  if( !mPacketVector.Empty() )
  {
    mPacket = static_cast< media_packet_h >( mPacketVector[0] );
    mPacketVector.Remove( mPacketVector.Begin() );
  }

  if( mPacket == NULL )
  {
    return true;
  }

  error = media_packet_get_tbm_surface( mPacket, &mTbmSurface );
  if( error != MEDIA_PACKET_ERROR_NONE )
  {
    media_packet_destroy( mPacket );
    mPacket = NULL;
    DALI_LOG_ERROR( " error: %d\n", error );
    return true;
  }

  Any source( mTbmSurface );
  mNativeImageSourcePtr->SetSource( source );
  Dali::Stage::GetCurrent().KeepRendering( 0.0f );

  return true;
}

} // namespace Plugin
} // namespace Dali;
