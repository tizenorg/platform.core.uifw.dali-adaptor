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
#include <adaptor-impl.h>

// The plugin factories
extern "C" DALI_EXPORT_API Dali::TizenBufferVideoPlayerPlugin* CreateTizenBufferVideoPlayerPlugin(void)
{
  return new Dali::TizenBufferTizenBufferVideoPlayer;
}

extern "C" DALI_EXPORT_API void DestroyTizenBufferVideoPlayerPlugin(Dali::TizenBufferVideoPlayerPlugin* plugin)
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

// Packet list
GList* gPacketList;
tbm_surface_h gSurface;

static void MediaPacketVideoDecodedCb(media_packet_h packet, void *user_data)
{
  TizenBufferVideoPlayer* pPlayer = static_cast< TizenBufferVideoPlayer* >(user_data);
  if(Dali::TizenBufferVideoPlayer TizenBufferVideoPlayer::New()
  {
    Dali::TizenBufferVideoPlayer player = Dali::TizenBufferVideoPlayer(new TizenBufferVideoPlayer());
    return player;
  }

  Dali::TizenBufferVideoPlayer TizenBufferVideoPlayer::Get()
  {
    Dali::TizenBufferVideoPlayer player = TizenBufferVideoPlayer::New();
    return player;
  }pPlayer == NULL)
  {
    std::cout << "Error MediaPacketVideoDecodedCb()\n";
    return;
  }

  g_mutex_lock(&pPlayer->mBufferLock);
  gPacketList = g_list_prepend(gPacketList, (gpointer)packet);
  g_mutex_unlock(&pPlayer->mBufferLock);
}

} // unnamed namespace

TizenBufferVideoPlayer::~TizenBufferVideoPlayer()
{
  if(mPlayer)
  {
    if(player_destroy(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "~VIdeoPlayer() error" << std::endl;
    }
    mPlayer = NULL;
    g_mutex_clear(&mBufferLock);
  }
}

bool TizenBufferVideoPlayer::Update()
{
  std::cout << "Update()================== begin\n";

  int ret;

  GList* lastItem = NULL;

  g_mutex_lock( &mBufferLock );

  if(mPacket)
  {
    if(media_packet_destroy(mPacket) != MEDIA_PACKET_ERROR_NONE)
    {
      std::cout << "Failed to destroy media packet. \n";
    }
    mPacket = NULL;
  }

  lastItem = g_list_last(gPacketList);
  if(lastItem)
  {
    mPacket = (media_packet_h)lastItem->data;
    gPacketList = g_list_remove(gPacketList, mPacket);
  }
  if(mPacket == NULL)
  {
    std::cout << "mPacket == NULL\n";
    g_mutex_unlock( &mBufferLock );
    return true;
  }

  ret = media_packet_get_tbm_surface(mPacket, &gSurface);
  if(ret != MEDIA_PACKET_ERROR_NONE)
  {
    media_packet_destroy(mPacket);
    mPacket = NULL;
    g_mutex_unlock( &mBufferLock );
    std::cout << "Update()= media_packet_get_tbm_surface(mPacket, &gSurface);\n";
    return true;
  }

  mTbImagePtr->SetSurface(&gSurface);
  mVideoStream = Dali::Image::New(*mTbImagePtr);
  mImageActor.SetImage(mVideoStream);

  g_mutex_unlock( &mBufferLock );

  Dali::Stage::GetCurrent().KeepRendering(0.0f);

  std::cout << "Update()================== end\n";

  return true;
}


void TizenBufferVideoPlayer::LinkVideoStreamImage(Dali::ImageActor imageActor)
{
  mImageActor = imageActor;

  CreateTizenBufferSurface(0, 0);
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
  std::cout << "TizenBufferVideoPlayer::CreateTbmSurface() b" << std::endl;
  mTbImagePtr = Dali::TizenBufferImage::New();
  std::cout << "TizenBufferVideoPlayer::CreateTbmSurface() e" << std::endl;
}

void TizenBufferVideoPlayer::Initialize()
{
  if(player_create(&mPlayer) != PLAYER_ERROR_NONE)
  {
  // error
    std::cout << "Inialize() error" << std::endl;
  }

  if(player_set_media_packet_video_frame_decoded_cb(mPlayer, MediaPacketVideoDecodedCb, this) != PLAYER_ERROR_NONE)
  {
    std::cout << "error player_set_media_packet_video_frame_decoded_cb\n";
  }
  if(mPlayer)
  {
    player_set_sound_type(mPlayer, SOUND_TYPE_MEDIA);

    if(player_set_looping(mPlayer, mLoopFlag) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error" << std::endl;
    }

    if(player_set_display_mode(mPlayer, PLAYER_DISPLAY_MODE_FULL_SCREEN) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error" << std::endl;
    }

  std::cout << "TizenBufferVideoPlayer::initialize() tbm" << std::endl;
    if(player_set_display(mPlayer, PLAYER_DISPLAY_TYPE_NONE, NULL) != PLAYER_ERROR_NONE)
    {
      std::cout << "player_set_display() error" << std::endl;
    }

  std::cout << "TizenBufferVideoPlayer::initialize() 3" << std::endl;
    if(player_set_display_visible(mPlayer, true) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error" << std::endl;
    }
  std::cout << "TizenBufferVideoPlayer::initialize() 4" << std::endl;
  } // if(mPlayer)

  mTimer = Dali::Timer::New(20);
  mTimer.TickSignal().Connect(this, &TizenBufferVideoPlayer::Update);
  mTimer.Start();
}

void TizenBufferVideoPlayer::SetVideoFile(const std::string& strFilename)
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

void TizenBufferVideoPlayer::SetSize(int width, int height)
{
  mViewWidth = width;
  mViewHeight = height;
}

void TizenBufferVideoPlayer::Play()
{
  if(mPlayer)
  {
    if(player_prepare(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Play() error" << std::endl;
    }

    mTimer.Start();

    if(player_start(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Play() error" << std::endl;
    }

  }
}

void TizenBufferVideoPlayer::Pause()
{
  if(mPlayer)
  {
    if(player_pause(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Pause() error" << std::endl;
    }

    mTimer.Stop();
  }
}

void TizenBufferVideoPlayer::Backward(int millisecond)
{
  int curPos;
  int nextPos;
  curPos = 0;
  nextPos = 0;

  if(mPlayer)
  {
    if(player_get_play_position(mPlayer, &curPos) != PLAYER_ERROR_NONE)
    {
      std::cout << "Backward() error" << std::endl;
    }

    nextPos = curPos - millisecond;
    nextPos = (nextPos < 0.)? 0.:nextPos;

    if(player_set_play_position(mPlayer, nextPos, false, NULL, NULL) != PLAYER_ERROR_NONE)
    {
      std::cout << "Backward() error" << std::endl;
    }
  }
}

void TizenBufferVideoPlayer::Forward(int millisecond)
{
  int curPos;
  int nextPos;
  int duration;
  curPos = 0;
  nextPos = 0;
  duration = 0;

  if(mPlayer)
  {
    if(player_get_play_position(mPlayer, &curPos) != PLAYER_ERROR_NONE)
    {
      std::cout << "Forward() error" << std::endl;
    }

    if(player_get_duration(mPlayer, &duration) != PLAYER_ERROR_NONE)
    {
      std::cout << "Forward() error" << std::endl;
    }

    nextPos = curPos + millisecond;
    nextPos = (nextPos > duration)? duration:nextPos;

    if(player_set_play_position(mPlayer, nextPos, false, NULL, NULL) != PLAYER_ERROR_NONE)
    {
      std::cout << "Forward() error" << std::endl;
    }
  }
}

void TizenBufferVideoPlayer::Stop()
{
  if(mPlayer)
  {
    if(player_stop(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Stop() error" << std::endl;
    }
    mTimer.Stop();
  }
}

void TizenBufferVideoPlayer::Resume()
{
  if(mPlayer)
  {
    if(player_start(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Resume() error" << std::endl;
    }
    mTimer.Start();
  }
}

TizenBufferVideoPlayer::TizenBufferVideoPlayer()
: mLoopFlag( false )
, mViewWidth(0)
, mViewHeight(0)
{
  gPacketList = NULL;
  mPacket = NULL;
  g_mutex_init(&mBufferLock);
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
