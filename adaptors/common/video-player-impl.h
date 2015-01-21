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

// INTERNAL INCLUDES
#include <video-player.h>
#include <video-player-plugin.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/images/image.h>
#include <string>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace Adaptor
{

class Adaptor;

/**
 * Implementation of the VideoPlayer class.
 */
class VideoPlayer : public Dali::BaseObject
{

public:

  /**
   * @copydoc Dali::VideoPlayer::New()
   */
  static Dali::VideoPlayer New();

  /**
   * @copydoc Dali::VideoPlayer::Initialize()
   */
  bool Initialize();

  /**
   * @copydoc Dali::VideoPlayer::SetVideoFile()
   */
  void SetVideoFile( const std::string& filename );

  /**
   * @copydoc Dali::VideoPlayer::SetVideoLooping()
   */
  void SetVideoLooping( bool looping );

  /**
   * @copydoc Dali::VideoPlayer::IsVideoLooping()
   */
  bool IsVideoLooping();

  /**
   * @copydoc Dali::VideoPlayer::Play()
   */
  void Play();

  /**
   * @copydoc Dali::VideoPlayer::Pause()
   */
  void Pause();

  /**
   * @copydoc Dali::VideoPlayer::Backward()
   */
  void Backward( int millisecond );

  /**
   * @copydoc Dali::VideoPlayer::Forward()
   */
  void Forward( int millisecond );

  /**
   * @copydoc Dali::VideoPlayer::Stop()
   */
  void Stop();

  /**
   * @copydoc Dali::VideoPlayer::Resume()
   */
  void Resume();

  /**
   * @copydoc Dali::VideoPlayer::LinkVideoStreamImage()
   */
  void LinkVideoStreamImage( Dali::ImageActor imageActor );

private:

  /**
   * @brief Private constructor.
   */
  VideoPlayer();

  /**
   * @brief Undefined copy constructor.
   */
  VideoPlayer( const VideoPlayer& );

  /**
   * @brief Private destructor.
   */
  virtual ~VideoPlayer();

  /**
   * @brief Undefined assignment operator.
   */
  VideoPlayer& operator=( const VideoPlayer& );

private:

  Dali::VideoPlayerPlugin* mPlugin;                                           ///< VideoPlayerPlugin pointer of dynamic library
  void* mHandle;                                                              ///< Pointer of dynamic library

  typedef Dali::VideoPlayerPlugin* (*CreateVideoPlayerFunction)();            ///< Type definition of function pointer for creating a plugin instance
  typedef void (*DestroyVideoPlayerFunction)(Dali::VideoPlayerPlugin* plugin);///< Type definition of function pointer for destroying a plugin instance
  CreateVideoPlayerFunction mCreateVideoPlayerPtr;                            ///< Function pointer for creating an instance of platform based VideoPlayer plugin
  DestroyVideoPlayerFunction mDestroyVideoPlayerPtr;                          ///< Function pointer for destroying an instance of platform based VideoPlayer plugin


};

} // namespace Adaptor

} // namespace INternal

inline Internal::Adaptor::VideoPlayer& GetImplementation( Dali::VideoPlayer& player )
{
//  DALI_ASSERT_ALWAYS( player && "VideoPlayer handle is empty" );

  BaseObject& handle = player.GetBaseObject();

  return static_cast<Internal::Adaptor::VideoPlayer&>( handle );
}

inline const Internal::Adaptor::VideoPlayer& GetImplementation( const Dali::VideoPlayer& player )
{
//  DALI_ASSERT_ALWAYS( player && "VideoPlayer handle is empty" );

  const BaseObject& handle = player.GetBaseObject();

  return static_cast<const Internal::Adaptor::VideoPlayer&>( handle );
}

} // namespace Dali

#endif // __DALI_INTERNAL_VIDEO_PLAYER_H__
