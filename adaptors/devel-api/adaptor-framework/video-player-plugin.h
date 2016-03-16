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

namespace Dali
{

class NativeImage;

/**
 * @brief VideoPlayerPlugin is an abstract interface, used by dali-adaptor to access video player plugin.
 * A concrete implementation must be created for each platform and provided as dynamic library.
 * @SINCE_1_1.24
 */
class DALI_IMPORT_API VideoPlayerPlugin
{
public:

  /**
   * @brief Constructor.
   * @SINCE_1_1.24
   */
  VideoPlayerPlugin();

  /**
   * @brief Destructor.
   * @SINCE_1_1.24
   */
  virtual ~VideoPlayerPlugin();

  /**
   * @brief Sets a URL of the video file to play.
   *
   * @SINCE_1_1.24
   * @param [in] url
   */
  virtual void SetUrl( const std::string& url ) = 0;

  /**
   * @brief Returns the URL of the video file.
   * @SINCE_1_1.24
   */
  virtual std::string GetUrl() = 0;

  /**
   * @brief Sets the player looping status.
   *
   * @param [in] looping The new looping status: true or false
   */
  virtual void SetLooping(bool looping) = 0;

  /**
   * @brief Returns the player looping status.
   *
   * @return true if player is looping, false otherwise.
   */
  virtual bool IsLooping() = 0;

  /**
   * @brief Starts the video playback.
   */
  virtual void Play() = 0;

  /**
   * @brief Pauses the video playback.
   */
  virtual void Pause() = 0;

  /**
   * @brief Resumes the video playback.
   */
  virtual void Resume() = 0;

  /**
   * @brief Stops the video playback.
   */
  virtual void Stop() = 0;

  /**
   * @brief Seeks forward by the specified number of milliseconds.
   */
  virtual void Forward( int millisecond ) = 0;

  /**
   * @brief Seeks backward by the specified number of milliseconds.
   */
  virtual void Backward( int millisecond ) = 0;

  /**
   * @brief Sets the player mute status.
   */
  virtual void SetMute( bool mute ) = 0;

  /**
   * @brief Returns the player mute status.
   */
  virtual bool IsMute() = 0;

  /**
   * @brief Sets the player volume.
   */
  virtual void SetVolume( float left, float right ) = 0;

  /**
   * @brief Returns current volume factor.
   */
  virtual void GetVolume( float* left, float* right ) = 0;

  /**
   * @brief Returns video stream image handle.
   */
  virtual const NativeImage& GetVideoStreamImage() = 0;

  /**
   * @brief Returns the current state such as latest error message.
   */
  virtual std::string GetState() = 0;
};

} // namespace Dali;

#endif
