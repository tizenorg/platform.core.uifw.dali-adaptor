#ifndef __DALI_VIDEO_PLAYER_H__
#define __DALI_VIDEO_PLAYER_H__

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
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/actors/image-actor.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{
class VideoPlayer;
}

} // namespace Internal

/**
 * @brief An VideoPlayer class is used for showing a video.
 * It provides simple functions to handle video playback.
 */
class DALI_IMPORT_API VideoPlayer: public BaseHandle
{

public:

  /**
   * @brief Constructor
   */
  VideoPlayer();

  /**
   * @brief Destructor
   */
  ~VideoPlayer();

  /**
   * @brief Creates a new instance of a VideoPlayer.
   * @return A new VideoPlayer
   */
  static VideoPlayer New();

  /**
   * @brief Initializes dynamic library.
   * @return Returns true if successful, or false otherwise
   */
  bool Initialize();

  /**
   * @brief Sets a video file path and name for playing.
   * @param[in] filename The video file path and name
   */
  void SetVideoFile( const std::string& filename );

  /**
   * @brief Sets the player's looping status.
   * @param[in] looping The new looping status: true = looping, false = non-looping
   */
  void SetVideoLooping( bool looping );

  /**
   * @brief Gets the player's looping status.
   * @return The looping status: true = looping, false = non-looping
   */
  bool IsVideoLooping();

  /**
   * @brief Starts the video playback.
   */
  void Play();

  /**
   * @brief Pauses the video playback.
   */
  void Pause();

  /**
   * @brief Seeks backward by the specified number of seconds.
   * @param[in] millisecond The millisecond to seek backward
   */
  void Backward( int millisecond );

  /**
   * @brief Seeks forward by the specified number of seconds.
   * @param[in] millisecond The millisecond to seek forward
   */
  void Forward( int millisecond );

  /**
   * @brief Stops the video playback.
   */
  void Stop();

  /**
   * @brief Resumes the video playback.
   */
  void Resume();

  /**
   * @brief Links the input 'imageActor' and the video stream image.
   * @param[in] imageActor The ImageActor to show the successive video stream images
   */
  void LinkVideoStreamImage( Dali::ImageActor imageActor );

public:

  /**
   * @brief Internal constructor.
   */
  VideoPlayer( Internal::Adaptor::VideoPlayer* player );
};

} // namespace Dali

#endif
