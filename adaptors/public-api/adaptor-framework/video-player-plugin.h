#ifndef __DALI_VIDEO_PLAYER_PLUGIN_H__
#define __DALI_VIDEO_PLAYER_PLUGIN_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>

namespace Dali
{

/**
 * @brief VideoPlayerPlugin is an abstract interface, used by dali-adaptor to access video player plugin.
 * A concrete implementation must be created for each platform and provided as dynamic library.
 */
class DALI_IMPORT_API VideoPlayerPlugin
{
public:

  /**
   * @brief Constructor.
   */
  VideoPlayerPlugin(){}

  /**
   * @brief Destructor.
   */
  virtual ~VideoPlayerPlugin(){}

  /**
   * @brief Sets a video file path and name for playing.
   * @param[in] filename The video file path and name
   */
  virtual void SetVideoFile( const std::string& filename ) = 0;

  /**
   * @brief Sets the player's looping status.
   * @param[in] looping The new looping status: true = looping, false = non-looping
   */
  virtual void SetVideoLooping( bool looping ) = 0;

  /**
   * @brief Gets the player's looping status.
   * @return The looping status: true = looping, false = non-looping
   */
  virtual bool IsVideoLooping() = 0;

  /**
   * @brief Starts the video playback.
   */
  virtual void Play() = 0;

  /**
   * @brief Pauses the video playback.
   */
  virtual void Pause() = 0;

  /**
   * @brief Seeks backward by the specified number of seconds.
   * @param[in] millisecond The millisecond to seek backward
   */
  virtual void Backward( int millisecond ) = 0;

  /**
   * @brief Seeks forward by the specified number of seconds.
   * @param[in] millisecond The millisecond to seek forward
   */
  virtual void Forward( int millisecond ) = 0;

  /**
   * @brief Stops the video playback.
   */
  virtual void Stop() = 0;

  /**
   * @brief Resumes the video playback.
   */
  virtual void Resume() = 0;

  /**
   * @brief Links the input 'imageActor' and the video stream image.
   * @param[in] imageActor The ImageActor to show the successive video stream images
   */
  virtual void LinkVideoStreamImage( Dali::ImageActor imageActor ) = 0;
};

} // namespace Dali

#endif
