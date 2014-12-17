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
#include "video-player-impl.h"

// EXTERNAL INCLUDES
#include <stdio.h>
#include <Ecore.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/shader-effects/shader-effect.h>

#include <adaptor-impl.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

namespace // unnamed namespace
{
// Type Registration
Dali::BaseHandle Create()
{
  return Dali::VideoPlayer::Get();
}

Dali::TypeRegistration type( typeid(Dali::VideoPlayer), typeid(Dali::BaseHandle), Create );

#if _TIZEN_BUFFER_MANAGER_SUPPORT_

// Packet list
GList* gPacketList;
tbm_surface_h gSurface;

static void MediaPacketVideoDecodedCb(media_packet_h packet, void *user_data)
{
  VideoPlayer* pPlayer = static_cast< VideoPlayer* >(user_data);
  if(pPlayer == NULL)
  {
    std::cout << "Error MediaPacketVideoDecodedCb()\n";
    return;
  }

  g_mutex_lock(&pPlayer->mBufferLock);
  gPacketList = g_list_prepend(gPacketList, (gpointer)packet);
  g_mutex_unlock(&pPlayer->mBufferLock);
}

#else
static Eina_Bool PixmapUpdate(void *data, int type, void *event)
{
  Dali::Stage::GetCurrent().KeepRendering(0.0f);
  return ECORE_CALLBACK_PASS_ON;
}
#endif

} // unnamed namespace


Dali::VideoPlayer VideoPlayer::New()
{
  Dali::VideoPlayer player = Dali::VideoPlayer(new VideoPlayer());
  return player;
}

Dali::VideoPlayer VideoPlayer::Get()
{
  Dali::VideoPlayer player = VideoPlayer::New();
  return player;
}

VideoPlayer::~VideoPlayer()
{
  if(mPlayer)
  {
    if(player_destroy(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "~VIdeoPlayer() error" << std::endl;
    }
    mPlayer = NULL;
#if _TIZEN_BUFFER_MANAGER_SUPPORT_
    g_mutex_clear(&mBufferLock);
#endif
  }
}

bool VideoPlayer::Update()
{
  std::cout << "Update()================== begin\n";

#if _TIZEN_BUFFER_MANAGER_SUPPORT_

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

#endif
  std::cout << "Update()================== end\n";

  Dali::Stage::GetCurrent().KeepRendering(0.0f);

  return true;
}


void VideoPlayer::LinkVideoStreamImage(Dali::ImageActor imageActor)
{
  mImageActor = imageActor;

#if _TIZEN_BUFFER_MANAGER_SUPPORT_
  CreateTizenBufferSurface(0, 0);
#else
  CreateXPixmapSurface(0,0);
#endif
}

void VideoPlayer::Initialize(int width, int height)
{
  std::cout << "VideoPlayer::Initialize" << std::endl;

#if _TIZEN_BUFFER_MANAGER_SUPPORT_
  InitTizenBufferPlayer();
#else
  InitX11Player();
#endif
}

#if _TIZEN_BUFFER_MANAGER_SUPPORT_
void VideoPlayer::CreateTizenBufferSurface(int width, int height)
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
  std::cout << "VideoPlayer::CreateTbmSurface() b" << std::endl;
  mTbImagePtr = Dali::TizenBufferImage::New();
  std::cout << "VideoPlayer::CreateTbmSurface() e" << std::endl;
}

void VideoPlayer::InitTizenBufferPlayer()
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

  std::cout << "VideoPlayer::initialize() tbm" << std::endl;
    if(player_set_display(mPlayer, PLAYER_DISPLAY_TYPE_NONE, NULL) != PLAYER_ERROR_NONE)
    {
      std::cout << "player_set_display() error" << std::endl;
    }

  std::cout << "VideoPlayer::initialize() 3" << std::endl;
    if(player_set_display_visible(mPlayer, true) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error" << std::endl;
    }
  std::cout << "VideoPlayer::initialize() 4" << std::endl;
  } // if(mPlayer)

  mTimer = Dali::Timer::New(20);
  mTimer.TickSignal().Connect(this, &VideoPlayer::Update);
  mTimer.Start();
}
#else

void VideoPlayer::CreateXPixmapSurface(int width, int height)
{
  std::cout << "VideoPlayer::CreateVideoSurface() 0" << std::endl;
  mViewWidth = width;
  mViewHeight = height;

  Dali::RenderSurface& surface = Dali::Adaptor::Get().GetSurface();

  Dali::Any renderableSurface = surface.GetSurface();

  Ecore_X_Window daliWindow = Dali::AnyCast< Ecore_X_Window >(renderableSurface);

  std::cout << "VideoPlayer::CreateVideoSurface() 1" << std::endl;

  int depth = ecore_x_default_depth_get(ecore_x_display_get(), ecore_x_default_screen_get());

  mPixmap = ecore_x_pixmap_new(daliWindow, mViewWidth, mViewHeight, depth);

  std::cout << "VideoPlayer::CreateVideoSurface() 2" << std::endl;

  // create the graphics context for drawing.
  unsigned int foreground;
  Ecore_X_GC gc;
  foreground = 0x000033;
  gc = ecore_x_gc_new(mPixmap, ECORE_X_GC_VALUE_MASK_FOREGROUND, &foreground);
  ecore_x_drawable_rectangle_fill(static_cast<Ecore_X_Drawable>(mPixmap), gc, 0, 0, mViewWidth, mViewHeight);
  ecore_x_gc_free(gc);
  ecore_x_sync();

  // connect damage notify
//  ecore_x_damage_new (mPixmap, ECORE_X_DAMAGE_REPORT_RAW_RECTANGLES);
//  ecore_event_handler_add (ECORE_X_EVENT_DAMAGE_NOTIFY, PixmapUpdate, this);

  Dali::PixmapImagePtr pixmapImage = Dali::PixmapImage::New( Dali::Any(mPixmap) );
  mVideoStream = Dali::Image::New(*pixmapImage);

  mImageActor.SetImage(mVideoStream);

  std::cout << "VideoPlayer::CreateVideoSurface() e" << std::endl;
}

void VideoPlayer::InitX11Player()
{
  if(player_create(&mPlayer) != PLAYER_ERROR_NONE)
  {
  // error
    std::cout << "Inialize() error1 " << std::endl;
  }

  if(mPlayer)
  {
    if(player_set_sound_type(mPlayer, SOUND_TYPE_MEDIA) != PLAYER_ERROR_NONE)
    {
      std::cout << "Sound type error ===========================\n";
    }

    if(player_set_looping(mPlayer, mLoopFlag) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error 2" << std::endl;
    }

    if(player_set_display_mode(mPlayer, PLAYER_DISPLAY_MODE_FULL_SCREEN) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error 3" << std::endl;
    }

    uintptr_t surface = mPixmap;
    if(player_set_display(mPlayer, PLAYER_DISPLAY_TYPE_X11, (void*)surface) != PLAYER_ERROR_NONE)
    {
      std::cout << "player_set_display() error" << std::endl;
    }

    if(player_set_x11_display_visible(mPlayer, true) != PLAYER_ERROR_NONE)
    {
      std::cout << "Inialize() error 4" << std::endl;
    }
  } // if(mPlayer)

  mTimer = Dali::Timer::New(20);
  mTimer.TickSignal().Connect(this, &VideoPlayer::Update);
  mTimer.Start();
}
#endif

void VideoPlayer::SetVideoFile(const std::string& strFilename)
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

void VideoPlayer::SetSize(int width, int height)
{
  mViewWidth = width;
  mViewHeight = height;
}

void VideoPlayer::Play()
{
  if(mPlayer)
  {
    if(player_prepare(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Play() error" << std::endl;
    }

#if _TIZEN_BUFFER_MANAGER_SUPPORT_
    mTimer.Start();
#endif

    if(player_start(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Play() error" << std::endl;
    }

  }
}

void VideoPlayer::Pause()
{
  if(mPlayer)
  {
    if(player_pause(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Pause() error" << std::endl;
    }

#if _TIZEN_BUFFER_MANAGER_SUPPORT_
    mTimer.Stop();
#endif

  }
}

void VideoPlayer::Backward(int millisecond)
{
  int curPos;
  int nextPos;
  curPos = 0;
  nextPos = 0;

  if(mPlayer)
  {
#if _TIZEN_BUFFER_MANAGER_SUPPORT_
    if(player_get_play_position(mPlayer, &curPos) != PLAYER_ERROR_NONE)
#else
    if(player_get_position(mPlayer, &curPos) != PLAYER_ERROR_NONE)
#endif
    {
      std::cout << "Backward() error" << std::endl;
    }

    nextPos = curPos - millisecond;
    nextPos = (nextPos < 0.)? 0.:nextPos;

#if _TIZEN_BUFFER_MANAGER_SUPPORT_
    if(player_set_play_position(mPlayer, nextPos, false, NULL, NULL) != PLAYER_ERROR_NONE)
#else
    if(player_set_position(mPlayer, nextPos, NULL, NULL) != PLAYER_ERROR_NONE)
#endif
    {
      std::cout << "Backward() error" << std::endl;
    }
  }
}

void VideoPlayer::Forward(int millisecond)
{
  int curPos;
  int nextPos;
  int duration;
  curPos = 0;
  nextPos = 0;
  duration = 0;

  if(mPlayer)
  {
#if _TIZEN_BUFFER_MANAGER_SUPPORT_
    if(player_get_play_position(mPlayer, &curPos) != PLAYER_ERROR_NONE)
#else
    if(player_get_position(mPlayer, &curPos) != PLAYER_ERROR_NONE)
#endif
    {
      std::cout << "Forward() error" << std::endl;
    }

    if(player_get_duration(mPlayer, &duration) != PLAYER_ERROR_NONE)
    {
      std::cout << "Forward() error" << std::endl;
    }

    nextPos = curPos + millisecond;
    nextPos = (nextPos > duration)? duration:nextPos;

#if _TIZEN_BUFFER_MANAGER_SUPPORT_
    if(player_set_play_position(mPlayer, nextPos, false, NULL, NULL) != PLAYER_ERROR_NONE)
#else
    if(player_set_position(mPlayer, nextPos, NULL, NULL) != PLAYER_ERROR_NONE)
#endif
    {
      std::cout << "Forward() error" << std::endl;
    }
  }
}

void VideoPlayer::Stop()
{
  if(mPlayer)
  {
    if(player_stop(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Stop() error" << std::endl;
    }
#if _TIZEN_BUFFER_MANAGER_SUPPORT_
    mTimer.Stop();
#endif
  }
}

void VideoPlayer::Resume()
{
  if(mPlayer)
  {
    if(player_start(mPlayer) != PLAYER_ERROR_NONE)
    {
      std::cout << "Resume() error" << std::endl;
    }
#if _TIZEN_BUFFER_MANAGER_SUPPORT_
    mTimer.Start();
#endif
  }
}

VideoPlayer::VideoPlayer()
: mLoopFlag( false )
, mViewWidth(0)
, mViewHeight(0)
{
#if _TIZEN_BUFFER_MANAGER_SUPPORT_
  gPacketList = NULL;
  mPacket = NULL;
  g_mutex_init(&mBufferLock);
#endif
}

} // namespace Adaptor

} // namespace Internal

} // namespace Dali
