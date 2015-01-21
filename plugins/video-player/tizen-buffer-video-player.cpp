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
#include <tizen-buffer-video-player.h>

// EXTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/integration-api/debug.h>

// The plugin factories
extern "C" DALI_EXPORT_API Dali::VideoPlayerPlugin* CreateVideoPlayerPlugin( void )
{
  return new Dali::Internal::Adaptor::TizenBufferVideoPlayer();
}

extern "C" DALI_EXPORT_API void DestroyVideoPlayerPlugin( Dali::VideoPlayerPlugin* plugin )
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

static void MediaPacketVideoDecodedCb( media_packet_h packet, void *user_data )
{
  TizenBufferVideoPlayer* player = static_cast< TizenBufferVideoPlayer* >( user_data );

  if( player == NULL )
  {
    DALI_LOG_ERROR( "Player callback input error\n" );
    return;
  }

  boost::unique_lock< boost::mutex > locki( player->mMutex );
  player->mPacketVector.PushBack( packet );
}

const int TIMER_INTERVAL( 20 );

} // unnamed namespace

TizenBufferVideoPlayer::~TizenBufferVideoPlayer()
{
  if( player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR( "Player state error\n" );
  }

  if( mState == PLAYER_STATE_IDLE )
  {
    mError = player_destroy( mPlayer );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Player handle destroy error: %d\n", mError );
    }
  }
}

bool TizenBufferVideoPlayer::Update()
{
  boost::unique_lock< boost::mutex > lock( mMutex );

  if( mPacket != NULL)
  {
    mError = media_packet_destroy( mPacket );
    if( mError != MEDIA_PACKET_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Packet destroy error: %d\n", mError );
    }
    mPacket = NULL;
  }

  if( mPacketVector.Size() > 0 )
  {
    mPacket = static_cast< media_packet_h >( mPacketVector[0] );
    mPacketVector.Remove( mPacketVector.Begin() );
  }

  if( mPacket == NULL )
  {
    return true;
  }

  mError = media_packet_get_tbm_surface( mPacket, &mSurface );
  if( mError != MEDIA_PACKET_ERROR_NONE )
  {
    media_packet_destroy( mPacket );
    mPacket = NULL;
    DALI_LOG_ERROR( "TBM surface can not be gotten. error: %d\n", mError );
    return true;
  }

  if( mTbImagePtr )
  {
    mTbImagePtr->SetSurface( &mSurface );
    mVideoStream = Dali::Image::New( *mTbImagePtr );
    mImageActor.SetImage( mVideoStream );
  }

  Dali::Stage::GetCurrent().KeepRendering( 0.0f );

  return true;
}


void TizenBufferVideoPlayer::LinkVideoStreamImage( Dali::ImageActor imageActor )
{
  mImageActor = imageActor;

  CreateTizenBufferImage();
  InitTizenBufferPlayer();
}

void TizenBufferVideoPlayer::CreateTizenBufferImage()
{
  std::string strFrgPrefix(
    "#extension GL_OES_EGL_image_external:require\n\n#if 0\n"
    );

  std::string strFragmentShader(
    "#endif\n"
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "precision highp float;\n"
    "#else\n"
    "precision mediump float;\n"
    "#endif\n"
    "uniform samplerExternalOES tex;\n"
    "uniform vec4 uColor;\n"
    "varying vec2 vTexCoord;\n"
    "void main()\n"
    "{\n"
    "  gl_FragColor=texture2D(tex, vTexCoord.xy) * uColor;\n"
    "}\n"
    );

  Dali::ShaderEffect shaderEffect = Dali::ShaderEffect::NewWithPrefix(
                                         "", "",
                                         strFrgPrefix, strFragmentShader
                                         );

  mImageActor.SetShaderEffect( shaderEffect );
  mTbImagePtr = Dali::TizenBufferImage::New();
}

void TizenBufferVideoPlayer::InitTizenBufferPlayer()
{
  if( mState == PLAYER_STATE_NONE )
  {
    mError = player_create( &mPlayer );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Player handle can not be created. error: %d\n", mError );
      return;
    }
  }

  if(player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR( "Player state invalid\n" );
    return;
  }

  if( mState == PLAYER_STATE_IDLE )
  {
    mError = player_set_media_packet_video_frame_decoded_cb( mPlayer, MediaPacketVideoDecodedCb, this );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Player callback error: %d\n", mError );
      return;
    }

    player_set_sound_type( mPlayer, SOUND_TYPE_MEDIA );

    mError = player_set_display_mode( mPlayer, PLAYER_DISPLAY_MODE_FULL_SCREEN );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Full screen mode error: %d\n", mError );
      return;
    }

    mError = player_set_display( mPlayer, PLAYER_DISPLAY_TYPE_NONE, NULL );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Player display type error: %d\n", mError );
      return;
    }

    mError = player_set_display_visible( mPlayer, true );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Player display visible error: %d\n", mError );
      return;
    }

    mTimer = Dali::Timer::New( TIMER_INTERVAL );
    mTimer.TickSignal().Connect( this, &TizenBufferVideoPlayer::Update );
  } // if(mState == PLAYER_STATE_IDLE)
}

void TizenBufferVideoPlayer::SetVideoFile( const std::string& filename )
{
  if( player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR( "Player state invalid\n" );
    return;
  }

  mVideoFilename = filename;
  if( mState == PLAYER_STATE_IDLE )
  {
    mError = player_set_uri( mPlayer, mVideoFilename.c_str() );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Video file open error: %d\n", mError );
      return;
    }
  }
}

void TizenBufferVideoPlayer::SetVideoLooping( bool looping )
{
  if( player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR( "Player state invalid\n" );
    return;
  }

  if( mState != PLAYER_STATE_NONE )
  {
    mError = player_set_looping( mPlayer, looping );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Looping flag setting error: %d\n", mError );
      return;
    }
  }
}

bool TizenBufferVideoPlayer::IsVideoLooping()
{
  if( player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR( "Player state invalid\n" );
    return false;
  }

  bool looping = false;
  if( mState != PLAYER_STATE_NONE )
  {
    mError = player_is_looping( mPlayer, &looping );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Looping flag state error: %d\n", mError );
      return false;
    }
  }
  return looping;
}

void TizenBufferVideoPlayer::Play()
{
  if( player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR( "Player state invalid\n" );
    return;
  }

  if( mState == PLAYER_STATE_IDLE )
  {
    mError = player_prepare( mPlayer );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Play does not work. error: %d\n", mError );
      return;
    }
  }

  if( player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR( "Player state invalid\n" );
    return;
  }

  if( mState == PLAYER_STATE_READY )
  {
    mTimer.Start();

    mError = player_start( mPlayer );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Play does not work. error: %d\n", mError );
      mTimer.Stop();
      return;
    }
  }
}

void TizenBufferVideoPlayer::Pause()
{
  if( player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR( "Player state invalid\n" );
    mTimer.Stop();
    return;
  }

  if( mState == PLAYER_STATE_PLAYING )
  {
    mError = player_pause( mPlayer );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Pause does not work. error: %d\n", mError );
      mTimer.Stop();
      return;
    }

    mTimer.Stop();
  }
}

void TizenBufferVideoPlayer::Backward( int millisecond )
{
  if( player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR( "Player state invalid\n" );
    return;
  }

  int curPos( 0 );
  int nextPos( 0 );

  if( mState == PLAYER_STATE_PLAYING || mState == PLAYER_STATE_PAUSED )
  {
    mError = player_get_play_position( mPlayer, &curPos );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Backward does not work. error: %d\n", mError );
      return;
    }

    nextPos = curPos - millisecond;
    nextPos = ( nextPos < 0 )? 0 : nextPos;

    mError = player_set_play_position( mPlayer, nextPos, true, NULL, NULL );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Backward does not work. error: %d\n", mError );
      return;
    }
  }
}

void TizenBufferVideoPlayer::Forward( int millisecond )
{
  if( player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR("Player state invalid\n" );
    return;
  }

  int curPos( 0 );
  int nextPos( 0 );

  if( mState == PLAYER_STATE_PLAYING || mState == PLAYER_STATE_PAUSED )
  {
    mError = player_get_play_position( mPlayer, &curPos );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Forward does not work. error: %d\n", mError );
      return;
    }

    nextPos = curPos + millisecond;

    mError = player_set_play_position( mPlayer, nextPos, true, NULL, NULL );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Forward does not work. error: %d\n", mError );
      return;
    }
  }
}

void TizenBufferVideoPlayer::Stop()
{
  if( player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR("Player state invalid\n" );
    return;
  }

  if( mState == PLAYER_STATE_PLAYING || mState == PLAYER_STATE_PAUSED )
  {
    mError = player_stop( mPlayer );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR("Stop does not work. error: %d\n", mError );
    }

    mTimer.Stop();

    mPacketVector.Clear();
  }
}

void TizenBufferVideoPlayer::Resume()
{
  if( player_get_state( mPlayer, &mState ) != PLAYER_ERROR_NONE )
  {
    DALI_LOG_ERROR( "Player state invalid\n" );
    return;
  }

  if( mState == PLAYER_STATE_PAUSED )
  {
    mError = player_start( mPlayer );
    if( mError != PLAYER_ERROR_NONE )
    {
      DALI_LOG_ERROR( "Resume does not work. error: %d\n", mError );
      return;
    }
    mTimer.Start();
  }
}

TizenBufferVideoPlayer::TizenBufferVideoPlayer()
: mPacket( NULL )
{
  mState = PLAYER_STATE_NONE;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
