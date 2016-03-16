#ifndef __DALI_TIZEN_VIDEO_PLAYER_PLUGIN_H__
#define __DALI_TIZEN_VIDEO_PLAYER_PLUGIN_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/images/native-image.h>
#include <dali/devel-api/threading/mutex.h>
#include <player.h>

// INTERNAL INCLUDES
#include <video-player-plugin.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

/**
 * @brief Implementation of the Tizen video player class which has Tizen platform dependency.
 * @SINCE_1_1.24
 */
class TizenVideoPlayer : public VideoPlayerPlugin, public ConnectionTracker
{
public:

  /**
   * @brief Constructor.
   * @SINCE_1_1.24
   */
  TizenVideoPlayer();

  /**
   * @brief Destructor.
   * @SINCE_1_1.24
   */
  virtual ~TizenVideoPlayer();

  /**
   * @copydoc Dali::VideoPlayerPlugin::SetUrl()
   */
  virtual void SetUrl( const std::string& url );

  /**
   * @copydoc Dali::VideoPlayerPlugin::GetUrl()
   */
  virtual std::string GetUrl();

  /**
   * @copydoc Dali::VideoPlayerPlugin::SetLooping()
   */
  virtual void SetLooping(bool looping);

  /**
   * @copydoc Dali::VideoPlayerPlugin::IsLooping()
   */
  virtual bool IsLooping();

  /**
   * @copydoc Dali::VideoPlayerPlugin::Play()
   */
  virtual void Play();

  /**
   * @copydoc Dali::VideoPlayerPlugin::Pause()
   */
  virtual void Pause();

  /**
   * @copydoc Dali::VideoPlayerPlugin::Resume()
   */
  virtual void Resume();

  /**
   * @copydoc Dali::VideoPlayerPlugin::Stop()
   */
  virtual void Stop();

  /**
   * @copydoc Dali::VideoPlayerPlugin::Forward()
   */
  virtual void Forward( int millisecond );

  /**
   * @copydoc Dali::VideoPlayerPlugin::Backward()
   */
  virtual void Backward( int millisecond );

  /**
   * @copydoc Dali::VideoPlayerPlugin::SetMute()
   */
  virtual void SetMute( bool mute );

  /**
   * @copydoc Dali::VideoPlayerPlugin::IsMute()
   */
  virtual bool IsMute();

  /**
   * @copydoc Dali::VideoPlayerPlugin::SetVolume()
   */
  virtual void SetVolume( float left, float right );

  /**
   * @copydoc Dali::VideoPlayerPlugin::GetVolume()
   */
  virtual void GetVolume( float* left, float* right );

  /**
   * @copydoc Dali::VideoPlayerPlugin::GetVideoStreamImage()
   */
  virtual const NativeImage& GetVideoStreamImage();

  /**
   * @copydoc Dali::VideoPlayerPlugin::GetState()
   */
  virtual str::string GetState();

private:

  void Initialize();

  bool Update();

private:

  TizenVideoPlayer( const TizenVideoPlayer& player );
  TizenVideoPlayer& operator=( const TizenVideoPlayer& player );

private:

  std::string mUrl;
  player_h mPlayer;
  player_state_e mState;
  Dali::NativeImage mNativeImage;
  tbm_surface_h mSurface;
  media_packet_h mPacket;
  Dali::NativeImageSourcePtr mNativeImageSourcePtr;

public:
  Dali::Mutex mPacketMutex;
  Dali::Vector< media_packet_h > mPacketVector;
};

} // namespace Adaptor
} // namespace Internal
} // namespace Dali;

#endif
