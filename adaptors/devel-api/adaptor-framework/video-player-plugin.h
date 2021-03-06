#ifndef __DALI_VIDEO_PLAYER_PLUGIN_H__
#define __DALI_VIDEO_PLAYER_PLUGIN_H__

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
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

class Any;

/**
 * @brief VideoPlayerPlugin is an abstract interface, used by dali-adaptor to access video player plugin.
 * A concrete implementation must be created for each platform and provided as dynamic library.
 * @SINCE_1_1.38
 */
class VideoPlayerPlugin
{
public:

  typedef Signal< void () > VideoPlayerSignalType;

  /**
   * @brief Video display rotation option
   * @SINCE_1_1.38
   * @remarks The option is needed only for window surface rendering target
   */
  enum DisplayRotation
  {
    ROTATION_NONE,   ///< Display isn't rotated. @SINCE_1_1.38
    ROTATION_90,     ///< Display is rotated 90 degree. @SINCE_1_1.38
    ROTATION_180,    ///< Display is rotated 180 degree. @SINCE_1_1.38
    ROTATION_270     ///< Display is rotated 270 degree. @SINCE_1_1.38
  };

  /**
   * @brief Constructor.
   * @SINCE_1_1.38
   */
  VideoPlayerPlugin(){}

  /**
   * @brief Destructor.
   * @SINCE_1_1.38
   */
  virtual ~VideoPlayerPlugin(){}

  /**
   * @brief Sets a URL of the video file to play.
   *
   * @SINCE_1_1.38
   * @param [in] url The url of video file
   */
  virtual void SetUrl( const std::string& url ) = 0;

  /**
   * @brief Returns the URL of the video file.
   * @SINCE_1_1.38
   * @return Url of string type
   */
  virtual std::string GetUrl() = 0;

  /**
   * @brief Sets the player looping status.
   * @SINCE_1_1.38
   *
   * @param [in] looping The new looping status: true or false
   */
  virtual void SetLooping(bool looping) = 0;

  /**
   * @brief Returns the player looping status.
   * @SINCE_1_1.38
   *
   * @return True if player is looping, false otherwise.
   */
  virtual bool IsLooping() = 0;

  /**
   * @brief Starts the video playback.
   * @SINCE_1_1.38
   */
  virtual void Play() = 0;

  /**
   * @brief Pauses the video playback.
   * @SINCE_1_1.38
   */
  virtual void Pause() = 0;

  /**
   * @brief Stops the video playback.
   * @SINCE_1_1.38
   */
  virtual void Stop() = 0;

  /**
   * @brief Sets the player mute status.
   * @SINCE_1_1.38
   * @param[in] mute The new mute status, true is mute.
   */
  virtual void SetMute( bool mute ) = 0;

  /**
   * @brief Returns the player mute status.
   * @SINCE_1_1.38
   * @return True if player is mute.
   */
  virtual bool IsMuted() = 0;

  /**
   * @brief Sets the player volume.
   * @SINCE_1_1.38
   * @param[in] left The left volume scalar
   * @param[in] right The right volume scalar
   */
  virtual void SetVolume( float left, float right ) = 0;

  /**
   * @brief Gets current volume factor.
   * @SINCE_1_1.38
   * @param[out] left The current left volume scalar
   * @param[out] right The current right volume scalar
   */
  virtual void GetVolume( float& left, float& right ) = 0;

  /**
   * @brief Sets video rendering target.
   * @SINCE_1_1.38
   * @param[in] target The target for video rendering, window surface or native image source
   */
  virtual void SetRenderingTarget( Any target ) = 0;

  /**
   * @brief Sets the position for playback.
   * @SINCE_1_1.38
   *
   * @param[in] millisecond The position for playback
   */
  virtual void SetPlayPosition( int millisecond ) = 0;

  /**
   * @brief Returns the current position in milliseconds.
   * @SINCE_1_1.38
   *
   * @return The current position of playback
   */
  virtual int GetPlayPosition() = 0;

  /**
   * @brief Sets video display rotation
   * @SINCE_1_1.38
   * @param[in] rotation The rotation of display
   */
  virtual void SetDisplayRotation( Dali::VideoPlayerPlugin::DisplayRotation rotation ) = 0;

  /**
   * @brief Returns rotation of current video display
   * @SINCE_1_1.38
   * @return The rotation of current display
   */
  virtual Dali::VideoPlayerPlugin::DisplayRotation GetDisplayRotation() = 0;

  /**
   * @brief Connect to this signal to be notified when a video playback have finished.
   *
   * @SINCE_1_1.38
   * @return A signal object to connect with.
   */
  virtual VideoPlayerSignalType& FinishedSignal() = 0;

};

} // namespace Dali;

#endif
