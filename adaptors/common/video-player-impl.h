#ifndef __DALI_VIDEO_PLAYER_IMPL_H__
#define __DALI_VIDEO_PLAYER_IMPL_H__

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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/images/native-image.h>

// INTERNAL INCLUDES
#include <video-player.h>
#include <video-player-plugin.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class VideoPlayer;

typedef IntrusivePtr< VideoPlayer > VideoPlayerPtr;
/**
 * @brief VideoPlayer class is used for video playback.
 * @SINCE_1_1.24
 */
class VideoPlayer: public Dali::BaseObject
{
public:

  static VideoPlayerPtr New();
  /**
   * @brief Constructor.
   * @SINCE_1_1.24
   */
  VideoPlayer();

  /**
   * @brief Destructor.
   * @SINCE_1_1.24
   */
  virtual ~VideoPlayer();

  /**
   * @brief Sets a URL of the video file to play.
   *
   * @SINCE_1_1.24
   * @param [in] url
   */
  void SetUrl( const std::string& url );

  /**
   * @brief Returns the URL of the video file.
   * @SINCE_1_1.24
   */
  std::string GetUrl();

  /**
   * @brief Sets the player looping status.
   *
   * @param [in] looping The new looping status: true or false
   */
  void SetLooping(bool looping);

  /**
   * @brief Returns the player looping status.
   *
   * @return true if player is looping, false otherwise.
   */
  bool IsLooping();

  /**
   * @brief Starts the video playback.
   */
  void Play();

  /**
   * @brief Pauses the video playback.
   */
  void Pause();

  /**
   * @brief Stops the video playback.
   */
  void Stop();

  /**
   * @brief Seeks forward by the specified number of milliseconds.
   */
  void Forward( int millisecond );

  /**
   * @brief Seeks backward by the specified number of milliseconds.
   */
  void Backward( int millisecond );

  /**
   * @brief Sets the player mute status.
   */
  void SetMute( bool mute );

  /**
   * @brief Returns the player mute status.
   */
  bool IsMuted();

  /**
   * @brief Sets the player volume.
   */
  void SetVolume( float left, float right );

  /**
   * @brief Returns current volume factor.
   */
  void GetVolume( float* left, float* right );

  /**
   * @brief Returns video stream image handle.
   */
  Dali::NativeImage GetVideoStreamImage() const;

  /**
   * @brief Returns the current state such as latest error message.
   */
  std::string GetState();

  /**
   * @brief Initializes member data.
   */
  void Initialize();

private:

  VideoPlayer( const VideoPlayer& player );

  VideoPlayer& operator=( const VideoPlayer& player );

private:
  Dali::VideoPlayerPlugin* mPlugin;
  void* mHandle;

  typedef Dali::VideoPlayerPlugin* (*CreateVideoPlayerFunction)();
  typedef void (*DestroyVideoPlayerFunction)( Dali::VideoPlayerPlugin* plugin );

  CreateVideoPlayerFunction mCreateVideoPlayerPtr;
  DestroyVideoPlayerFunction mDestroyVideoPlayerPtr;
};

} // namespace Adaptor
} // namespace Internal

inline static Internal::Adaptor::VideoPlayer& GetImplementation( Dali::VideoPlayer& player )
{
  DALI_ASSERT_ALWAYS( player && "VideoPlayer handle is empty." );

  BaseObject& handle = player.GetBaseObject();

  return static_cast< Internal::Adaptor::VideoPlayer& >( handle );
}

inline static const Internal::Adaptor::VideoPlayer& GetImplementation( const Dali::VideoPlayer& player )
{
  DALI_ASSERT_ALWAYS( player && "VideoPlayer handle is empty." );

  const BaseObject& handle = player.GetBaseObject();

  return static_cast< const Internal::Adaptor::VideoPlayer& >( handle );
}

} // namespace Dali;

#endif

