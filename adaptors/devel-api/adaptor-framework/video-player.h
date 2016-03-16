#ifndef __DALI_VIDEO_PLAYER_H__
#define __DALI_VIDEO_PLAYER_H__

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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/images/native-image.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
  class VideoPlayer;
} // namespace Adaptor

} // namespace Internal

/**
 * @brief VideoPlayer class is used for video playback.
 * @SINCE_1_1.24
 */
class DALI_IMPORT_API VideoPlayer: public BaseHandle
{
public:

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
   * @brief Creates a new instance of a VideoPlayer.
   * @SINCE_1_1.24
   */
  static VideoPlayer New();

 /**
   * @brief Copy constructor.
   *
   * @SINCE_1_1.24
   * @param[in] player VideoPlayer to copy. The copied player will point at the same implementation
   */
  VideoPlayer( const VideoPlayer& player );

 /**
   * @brief Assignment operator.
   *
   * @SINCE_1_1.24
   * @param[in] player The VideoPlayer to assign from.
   * @return The updated VideoPlayer.
   */
  VideoPlayer& operator=( const VideoPlayer& player );

  /**
   * @brief Downcast a handle to VideoPlayer handle.
   *
   * If handle points to a VideoPlayer the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_1.24
   * @param[in] handle Handle to an object
   * @return handle to a VideoPlayer or an uninitialized handle
   */
  static VideoPlayer DownCast( BaseHandle handle );

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
   * @SINCE_1_1.24
   *
   * @param [in] looping The new looping status: true or false
   */
  void SetLooping(bool looping);

  /**
   * @brief Returns the player looping status.
   * @SINCE_1_1.24
   *
   * @return true if player is looping, false otherwise.
   */
  bool IsLooping();

  /**
   * @brief Starts the video playback.
   * @SINCE_1_1.24
   */
  void Play();

  /**
   * @brief Pauses the video playback.
   * @SINCE_1_1.24
   */
  void Pause();

  /**
   * @brief Stops the video playback.
   * @SINCE_1_1.24
   */
  void Stop();

  /**
   * @brief Seeks forward by the specified number of milliseconds.
   * @SINCE_1_1.24
   */
  void Forward( int millisecond );

  /**
   * @brief Seeks backward by the specified number of milliseconds.
   * @SINCE_1_1.24
   */
  void Backward( int millisecond );

  /**
   * @brief Sets the player mute status.
   * @SINCE_1_1.24
   */
  void SetMute( bool mute );

  /**
   * @brief Returns the player mute status.
   * @SINCE_1_1.24
   */
  bool IsMuted();

  /**
   * @brief Sets the player volume.
   * @SINCE_1_1.24
   */
  void SetVolume( float left, float right );

  /**
   * @brief Returns current volume factor.
   * @SINCE_1_1.24
   */
  void GetVolume( float* left, float* right );

  /**
   * @brief Set NativeImage handle for video texture rendering.
   * @SINCE_1_1.24
   *
   * @remarks If this API isn't called before Play(), VideoPlay renders a video in full screen, h/w underlay.
   */
  const NativeImage& GetVideoStreamImage();

  /**
   * @brief Sets the position for playback.
   * @SINCE_1_1.24
   *
   * @param[in] millisecond The position for playback
   */
  void SetPlayPosition( int millisecond );

  /**
   * @brief Gets the current position in milliseconds.
   * @SINCE_1_1.24
   *
   * @return The current position of playback
   */
  int GetPlayPosition();

private:

  explicit DALI_INTERNAL VideoPlayer( Internal::Adaptor::VideoPlayer* internal );
};

} // namespace Dali;

#endif

