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
class Any;

/**
 * @brief VideoPlayerPlugin is an abstract interface, used by dali-adaptor to access video player plugin.
 * A concrete implementation must be created for each platform and provided as dynamic library.
 * @SINCE_1_1.24
 */
class DALI_IMPORT_API VideoPlayerPlugin
{
public:

  /**
   * @SINCE_1_1.24
   */
  enum DisplayRotation
  {
    ROTATION_NONE,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270
  };

  /**
   * @brief Constructor.
   * @SINCE_1_1.24
   */
  VideoPlayerPlugin(){}

  /**
   * @brief Destructor.
   * @SINCE_1_1.24
   */
  virtual ~VideoPlayerPlugin(){}

  /**
   * @brief Initializes plugin member data.
   *
   * @SINCE_1_1.24
   */
  virtual void Initialize() = 0;

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
   * @SINCE_1_1.24
   *
   * @param [in] looping The new looping status: true or false
   */
  virtual void SetLooping(bool looping) = 0;

  /**
   * @brief Returns the player looping status.
   * @SINCE_1_1.24
   *
   * @return true if player is looping, false otherwise.
   */
  virtual bool IsLooping() = 0;

  /**
   * @brief Starts the video playback.
   * @SINCE_1_1.24
   */
  virtual void Play() = 0;

  /**
   * @brief Pauses the video playback.
   * @SINCE_1_1.24
   */
  virtual void Pause() = 0;

  /**
   * @brief Stops the video playback.
   * @SINCE_1_1.24
   */
  virtual void Stop() = 0;

  /**
   * @brief Sets the player mute status.
   * @SINCE_1_1.24
   */
  virtual void SetMute( bool mute ) = 0;

  /**
   * @brief Returns the player mute status.
   * @SINCE_1_1.24
   */
  virtual bool IsMuted() = 0;

  /**
   * @brief Sets the player volume.
   * @SINCE_1_1.24
   */
  virtual void SetVolume( float left, float right ) = 0;

  /**
   * @brief Returns current volume factor.
   * @SINCE_1_1.24
   */
  virtual void GetVolume( float* left, float* right ) = 0;

  /**
   * @brief Returns video stream image handle.
   * @SINCE_1_1.24
   */
//  virtual const NativeImage& GetVideoStreamImage() = 0;
  virtual void SetRenderingTarget( Any target ) = 0;

  /**
   * @brief Sets the position for playback.
   * @SINCE_1_1.24
   *
   * @param[in] millisecond The position for playback
   */
  virtual void SetPlayPosition( int millisecond ) = 0;

  /**
   * @brief Gets the current position in milliseconds.
   * @SINCE_1_1.24
   *
   * @return The current position of playback
   */
  virtual int GetPlayPosition() = 0;

  virtual void SetDisplayRotation( Dali::VideoPlayerPlugin::DisplayRotation rotation ) = 0;

  virtual int GetDisplayRotation() = 0;


};

} // namespace Dali;

#endif
