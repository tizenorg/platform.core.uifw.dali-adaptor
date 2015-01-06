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
#include "tizen-buffer-video-player.h"

// INTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/integration-api/debug.h>

// The plugin factories
extern "C" DALI_EXPORT_API Dali::VideoPlayerPlugin* CreateVideoPlayerPlugin(void)
{
  return new Dali::Internal::Adaptor::TizenBufferVideoPlayer();
}

extern "C" DALI_EXPORT_API void DestroyVideoPlayerPlugin(Dali::VideoPlayerPlugin* plugin)
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

static void MediaPacketVideoDecodedCb(media_packet_h packet, void *user_data)
{
  TizenBufferVideoPlayer* pPlayer = static_cast< TizenBufferVideoPlayer* >(user_data);

  if(pPlayer == NULL)
  {
    DALI_LOG_ERROR("MediaPacketVideoDecodedCb() error - pPlayer == NULL\n");
    return;
  }

  boost::unique_lock< boost::mutex > locki( pPlayer->mMutex );
  pPlayer->mPacketVector.PushBack(packet);
}

} // unnamed namespace

TizenBufferVideoPlayer::~TizenBufferVideoPlayer()
{
  if(player_get_state(mPlayer, &mState) != PLAYER_ERROR_NONE)
  {
    DALI_LOG_ERROR("TizenBufferVideoPlayer::InitTizenBufferPlayer() error - player_get_state invalid\n");
  }

  if(mState == PLAYER_STATE_IDLE)
  {
    mError = player_destroy(mPlayer);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::~TizenBufferVideoPlayer()-player_destroy() error: %d\n", mError);
    }
    mPlayer = NULL;
  }
}

bool TizenBufferVideoPlayer::Update()
{
  boost::unique_lock< boost::mutex > lock( mMutex );

  if(mPacket)
  {
    mError = media_packet_destroy(mPacket);
    if(mError != MEDIA_PACKET_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::Update() - media_packet_destroy() error: %d\n", mError);
    }
    mPacket = NULL;
  }

  if(mPacketVector.Size() > 0)
  {
    mPacket = (media_packet_h)mPacketVector[0];
    mPacketVector.Remove(mPacketVector.Begin());
  }

  if(mPacket == NULL)
  {
    return true;
  }

  mError = media_packet_get_tbm_surface(mPacket, &mSurface);
  if(mError != MEDIA_PACKET_ERROR_NONE)
  {
    media_packet_destroy(mPacket);
    mPacket = NULL;
    DALI_LOG_ERROR("TizenBufferVideoPlayer::Update() - media_packet_get_tbm_surface() error: %d\n", mError);
    return true;
  }

  if(mTbImagePtr)
  {
    mTbImagePtr->SetSurface(&mSurface);
    mVideoStream = Dali::Image::New(*mTbImagePtr);
    mImageActor.SetImage(mVideoStream);
  }

  Dali::Stage::GetCurrent().KeepRendering(0.0f);

  return true;
}


void TizenBufferVideoPlayer::LinkVideoStreamImage(Dali::ImageActor imageActor)
{
  mImageActor = imageActor;

  CreateTizenBufferSurface();
  InitTizenBufferPlayer();
}

void TizenBufferVideoPlayer::CreateTizenBufferSurface()
{
  std::string strFrgPrefix(
    "#extension GL_OES_EGL_image_external:require\n\n#if 0\n");

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

  Dali::ShaderEffect shaderEffect = Dali::ShaderEffect::NewWithPrefix("", "",
                                         strFrgPrefix, strFragmentShader);

  mImageActor.SetShaderEffect(shaderEffect);
  mTbImagePtr = Dali::TizenBufferImage::New();
}

void TizenBufferVideoPlayer::InitTizenBufferPlayer()
{
  if(mState == PLAYER_STATE_NONE)
  {
    mError = player_create(&mPlayer);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::InitTizenBufferPlayer() - player_create() error: %d\n", mError);
      return;
    }
  }

  if(player_get_state(mPlayer, &mState) != PLAYER_ERROR_NONE)
  {
    DALI_LOG_ERROR("TizenBufferVideoPlayer::InitTizenBufferPlayer() error - player_get_state invalid\n");
    return;
  }

  if(mState == PLAYER_STATE_IDLE)
  {
    mError = player_set_media_packet_video_frame_decoded_cb(mPlayer, MediaPacketVideoDecodedCb, this);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::InitTizenBufferPlayer() - player_set_media_packet_video_frame_decoded_cb() error: %d\n", mError);
      return;
    }

    player_set_sound_type(mPlayer, SOUND_TYPE_MEDIA);

    mError = player_set_looping(mPlayer, mLoopFlag);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::InitTizenBufferPlayer() - player_set_looping() error: %d\n", mError);
      return;
    }

    mError = player_set_display_mode(mPlayer, PLAYER_DISPLAY_MODE_FULL_SCREEN);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::InitTizenBufferPlayer() - player_set_display_mode() error: %d\n", mError);
      return;
    }

    mError = player_set_display(mPlayer, PLAYER_DISPLAY_TYPE_NONE, NULL);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::InitTizenBufferPlayer() - player_set_display() error: %d\n", mError);
      return;
    }

    mError = player_set_display_visible(mPlayer, true);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::InitTizenBufferPlayer() - player_set_display_visible() error: %d\n", mError);
      return;
    }

    mTimer = Dali::Timer::New(20);
    mTimer.TickSignal().Connect(this, &TizenBufferVideoPlayer::Update);
  } // if(mState == PLAYER_STATE_IDLE)
}

void TizenBufferVideoPlayer::SetVideoFile(const std::string& strFilename)
{
  if(player_get_state(mPlayer, &mState) != PLAYER_ERROR_NONE)
  {
    DALI_LOG_ERROR("TizenBufferVideoPlayer::SetVideoFile() error - player_get_state invalid\n");
    return;
  }

  mVideoFilename = strFilename;
  if(mState == PLAYER_STATE_IDLE)
  {
    mError = player_set_uri(mPlayer, mVideoFilename.c_str());
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::SetVideoFile() - player_set_uri() error: %d\n", mError);
      return;
    }
  }
}

void TizenBufferVideoPlayer::Play()
{
  if(player_get_state(mPlayer, &mState) != PLAYER_ERROR_NONE)
  {
    DALI_LOG_ERROR("TizenBufferVideoPlayer::Play() error - player_get_state invalid\n");
    return;
  }

  if(mState == PLAYER_STATE_IDLE)
  {
    mError = player_prepare(mPlayer);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::Play() - player_prepare() error: %d\n", mError);
      return;
    }
  }

  if(player_get_state(mPlayer, &mState) != PLAYER_ERROR_NONE)
  {
    DALI_LOG_ERROR("TizenBufferVideoPlayer::Play() error - player_get_state invalid\n");
    return;
  }

  if(mState == PLAYER_STATE_READY)
  {
    mTimer.Start();

    mError = player_start(mPlayer);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::Play() - player_start() error: %d\n", mError);
      mTimer.Stop();
      return;
    }
  }
}

void TizenBufferVideoPlayer::Pause()
{
  if(player_get_state(mPlayer, &mState) != PLAYER_ERROR_NONE)
  {
    DALI_LOG_ERROR("TizenBufferVideoPlayer::Pause() error - player_get_state invalid\n");
    mTimer.Stop();
    return;
  }

  if(mState == PLAYER_STATE_PLAYING)
  {
    mError = player_pause(mPlayer);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::Pause() - player_pause() error: %d\n", mError);
      mTimer.Stop();
      return;
    }

    mTimer.Stop();
  }
}

void TizenBufferVideoPlayer::Backward(int millisecond)
{
  if(player_get_state(mPlayer, &mState) != PLAYER_ERROR_NONE)
  {
    DALI_LOG_ERROR("TizenBufferVideoPlayer::Backward() error - player_get_state invalid\n");
    return;
  }

  int curPos;
  int nextPos;
  curPos = 0;
  nextPos = 0;

  if(mState == PLAYER_STATE_PLAYING || mState == PLAYER_STATE_PAUSED)
  {
    mError = player_get_play_position(mPlayer, &curPos);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::Backward() - player_get_play_position() error: %d\n", mError);
      return;
    }

    nextPos = curPos - millisecond;

    mError = player_set_play_position(mPlayer, nextPos, true, NULL, NULL);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::Backward() - player_set_play_position() error: %d\n", mError);
      return;
    }
  }
}

void TizenBufferVideoPlayer::Forward(int millisecond)
{
  if(player_get_state(mPlayer, &mState) != PLAYER_ERROR_NONE)
  {
    DALI_LOG_ERROR("TizenBufferVideoPlayer::Forward() error - player_get_state invalid\n");
    return;
  }

  int curPos;
  int nextPos;
  curPos = 0;
  nextPos = 0;

  if(mState == PLAYER_STATE_PLAYING || mState == PLAYER_STATE_PAUSED)
  {
    mError = player_get_play_position(mPlayer, &curPos);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::Forward() - player_get_play_position() error: %d\n", mError);
      return;
    }

    nextPos = curPos + millisecond;

    mError = player_set_play_position(mPlayer, nextPos, true, NULL, NULL);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::Forward() - player_set_play_position() error: %d\n", mError);
      return;
    }
  }
}

void TizenBufferVideoPlayer::Stop()
{
  if(player_get_state(mPlayer, &mState) != PLAYER_ERROR_NONE)
  {
    DALI_LOG_ERROR("TizenBufferVideoPlayer::Stop() error - player_get_state invalid\n");
    return;
  }

  if(mState == PLAYER_STATE_PLAYING || mState == PLAYER_STATE_PAUSED)
  {
    mError = player_stop(mPlayer);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::Stop() - player_stop() error: %d\n", mError);
    }

    mTimer.Stop();

    mPacketVector.Clear();
  }
}

void TizenBufferVideoPlayer::Resume()
{
  if(player_get_state(mPlayer, &mState) != PLAYER_ERROR_NONE)
  {
    DALI_LOG_ERROR("TizenBufferVideoPlayer::Resume() error - player_get_state invalid\n");
    return;
  }

  if(mState == PLAYER_STATE_PAUSED)
  {
    mError = player_start(mPlayer);
    if(mError != PLAYER_ERROR_NONE)
    {
      DALI_LOG_ERROR("TizenBufferVideoPlayer::Resume() - player_start() error: %d\n", mError);
      return;
    }
    mTimer.Start();
  }
}

TizenBufferVideoPlayer::TizenBufferVideoPlayer()
: mLoopFlag( false ),
  mPacket(NULL)
{
  mState = PLAYER_STATE_NONE;
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
