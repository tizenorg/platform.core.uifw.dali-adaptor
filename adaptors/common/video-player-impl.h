#ifndef __DALI_INTERNAL_VIDEO_PLAYER_H__
#define __DALI_INTERNAL_VIDEO_PLAYER_H__

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

#include <video-player.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/images/image.h>

#if _TIZEN_BUFFER_MANAGER_SUPPORT_

#include <tizen-buffer-image.h>
#include <timer.h>
#else

#include <pixmap-image.h>

#endif

// EXTERNAL INCLUDES
#include <string>
#include <Ecore_X.h>
#include <player.h>
#include <glib.h>

#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class Adaptor;

class VideoPlayer : public Dali::BaseObject, public ConnectionTracker
{

public:

  static Dali::VideoPlayer New();

  static Dali::VideoPlayer Get();

  void SetVideoFile(const std::string& strFilename);

  void SetSize(int width, int height);

  void Play();

  void Pause();

  void Backward(int millisecond);

  void Forward(int millisecond);

  void Stop();

  void Resume();

  void Initialize(int width, int height);

  bool Update();

  void LinkVideoStreamImage(Dali::ImageActor imageActor);

private:

  VideoPlayer();
  VideoPlayer(const VideoPlayer&);

  virtual ~VideoPlayer();

#if _TIZEN_BUFFER_MANAGER_SUPPORT_
  void CreateTizenBufferSurface(int width, int height);
  void InitTizenBufferPlayer();
#else
  void CreateXPixmapSurface(int width, int height);
  void InitX11Player();
#endif

private:

  bool mLoopFlag;
  std::string mVideoFilename;
  player_h mPlayer;
  int mViewWidth;
  int mViewHeight;
  Dali::Image mVideoStream;
  Dali::ImageActor mImageActor;

#if _TIZEN_BUFFER_MANAGER_SUPPORT_

  media_packet_h mPacket;
  Dali::TizenBufferImagePtr mTbImagePtr;
  Dali::Timer mTimer;
#else
  Ecore_X_Pixmap mPixmap;
  Dali::PixmapImagePtr mPixmapImage;
#endif

public:

#if _TIZEN_BUFFER_MANAGER_SUPPORT_
  GMutex mBufferLock;
#endif
};

} // namespace Adaptor

} // namespace INternal

inline Internal::Adaptor::VideoPlayer& GetImplementation(Dali::VideoPlayer& player)
{
//  DALI_ASSERT_ALWAYS( player && "VideoPlayer handle is empty" );

  BaseObject& handle = player.GetBaseObject();

  return static_cast<Internal::Adaptor::VideoPlayer&>(handle);
}

inline const Internal::Adaptor::VideoPlayer& GetImplementation(const Dali::VideoPlayer& player)
{
//  DALI_ASSERT_ALWAYS( player && "VideoPlayer handle is empty" );

  const BaseObject& handle = player.GetBaseObject();

  return static_cast<const Internal::Adaptor::VideoPlayer&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_VIDEO_PLAYER_H__
