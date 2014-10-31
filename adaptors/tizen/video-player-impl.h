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

// EXTERNAL INCLUDES
#include <string>
#include <Ecore_X.h>

#include <dali/integration-api/debug.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/adaptor-framework/pixmap-image.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class VideoPlayer : public Dali::BaseObject
{

public:

  void SetVideoFile(const std::string& strFilename);

  void Play();

  void Pause();

  void Backward(int millisecond);

  void Forward(int millisecond);

  void Stop();

  void Resume();

  Dali::Image GetVideoStreamImage();

private:

  VideoPlayer();
  VideoPlayer(const VideoPlayer&);

  virtual ~VideoPlayer();

  void Intialize();

  void CreateVideoSurface();

private:

  std::string mVideoFilename;
  player_h mPlayer;

  PixmapImagePtr mPixmapImage;
  Ecore_X_Pixmap mPixmap;
};

} // namespace Adaptor

} // namespace INternal

inline Internal::Adaptor::VideoPlayer& GetImplementation(Dali::VideoPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "VideoPlayer handle is empty" );

  BaseObject& handle = player.GetBaseObject();

  return static_cast<Internal::Adaptor::VideoPlayer&>(handle);
}

inline const Internal::Adaptor::VideoPlayer& GetImplementation(Dali::VideoPlayer& player)
{
  DALI_ASSERT_ALWAYS( player && "VideoPlayer handle is empty" );

  const BaseObject& handle = player.GetBaseObject();

  return static_cast<const Internal::Adaptor::VideoPlayer&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_VIDEO_PLAYER_H__
